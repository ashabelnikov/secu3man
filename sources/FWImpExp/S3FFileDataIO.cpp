/* SECU-3  - An open source, free engine control unit
   Copyright (C) 2007 Alexey A. Shabelnikov. Ukraine, Gorlovka

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

   contacts:
              http://secu-3.org
              email: shabelnikov@secu-3.org
*/

#include "stdafx.h"
#include "resource.h"
#include "S3FFileDataIO.h"

#include <vector>
#include "common/MathHelpers.h"
#include "io-core/CRC16.h"
#include "io-core/FirmwareMapsDataHolder.h"
#include "io-core/SECU3IO.h"

#pragma pack( push, enter_S3FFileMap )
#pragma pack(1) //<--SECU3

#define INT_MULTIPLIER 10000.0f
#define MIN_OPTDATA_SIZE 1024
#define MIN_NOFSETS TABLES_NUMBER
#define MAX_NOFSETS 64

//define our own types
typedef unsigned short s3f_uint16_t;
typedef signed int s3f_int32_t;
typedef unsigned char s3f_uint8_t;

// Format is following:
//   _______________________________
//  / File header (S3FFileHdr)      \
//  |-------------------------------|
//  | array of S3FMapSetItem blocks |
//  |-------------------------------| 
//  | S3FFileMaps block             |
//  |-------------------------------| 
//  | Optional data (size >= 1024)  |
//  \_______________________________/
//

// S3F Version history
// 01.00 - initial version
// 01.01 - Choke opening map added (24.05.2013)
// 01.02 - RPM grid added (29.04.2014)

struct S3FFileHdr
{
 s3f_uint8_t  hdr[3];          //Contains 'S3F'
 s3f_uint16_t crc16;           //CRC16 of whole file (except first 5 bytes of file)
 s3f_uint16_t version;         //File version in xx.xx format
 s3f_uint8_t  nofsets;         //Number of set of tables
 s3f_uint8_t  btpmi;           //Bytes per 1 item in the map
 s3f_uint16_t sofdat;          //Size of optional data block, size must be >= 1024
 s3f_uint8_t  reserved[8];     //reserved bytes, = 0
};

//One entry of map sets array
struct S3FMapSetItem
{
 s3f_int32_t f_str[F_STR_POINTS]; //start
 s3f_int32_t f_idl[F_IDL_POINTS]; //idle 
 s3f_int32_t f_wrk[F_WRK_POINTS_L * F_WRK_POINTS_F]; //work
 s3f_int32_t f_tmp[F_TMP_POINTS]; //temperature
 s3f_uint8_t name[F_NAME_SIZE];   // ��������������� ��� (��� ���������)
};

//Separate maps
struct S3FSepMaps
{
 s3f_int32_t attenuator_table[KC_ATTENUATOR_LOOKUP_TABLE_SIZE]; //attenuator table (for knock detection)
 s3f_int32_t dwellcntrl_table[COIL_ON_TIME_LOOKUP_TABLE_SIZE];  //dwell control look up table
 s3f_int32_t ctscurve_table[THERMISTOR_LOOKUP_TABLE_SIZE];      //coolant sensor look up table
 s3f_int32_t ctscurve_vlimits[2]; //volatge limits for coolant sensor look up table
 s3f_int32_t choke_op_table[CHOKE_CLOSING_LOOKUP_TABLE_SIZE]; //choke opening map (appeared in version 1.01, reserved bytes were utilized)
 s3f_int32_t rpm_slots[F_RPM_SLOTS]; //RPM grid (appeared in version 1.02, reserved bytes were utilized)
 s3f_int32_t reserved[96];       //reserved bytes, = 0
};

#pragma pack( pop, enter_S3FFileMap )

/////////////////////////////////////////////////////////////////////////////////////
S3FFileDataIO::S3FFileDataIO()
: m_file_crc_ok(false)
{
 //empty
}

/////////////////////////////////////////////////////////////////////////////////////
S3FFileDataIO::~S3FFileDataIO()
{
 //empty
}

/////////////////////////////////////////////////////////////////////////////////////
bool S3FFileDataIO::Load(const _TSTRING i_file_name)
{
 CFile file;
 CFileException ex;
 TCHAR szError[1024];
 if(!file.Open(i_file_name.c_str(),CFile::modeRead,&ex))
 {
  ex.GetErrorMessage(szError, 1024);
  AfxMessageBox(szError);
  return false;
 }
 //read the file into memory
 size_t filesize = (size_t)file.GetLength();
 size_t minFileSize = sizeof(S3FFileHdr) + (sizeof(S3FMapSetItem) * MIN_NOFSETS) + sizeof(S3FSepMaps) + MIN_OPTDATA_SIZE;
 if ((filesize > 1048576) || (filesize < minFileSize))
 {
  file.Close();
  return false; //������������ ������ �����
 }
 std::vector<BYTE> rawdata(filesize, 0);
 file.Read(&rawdata[0], filesize);
 file.Close();

 //parse file's contents. 
 //(1) First of all check the file format
 //(2) Next we have to check CRC and store status.
 //(3) Convert maps
 const S3FFileHdr* p_fileHdr = (S3FFileHdr*)(&rawdata[0]);
 if (memcmp(p_fileHdr->hdr, "S3F", 3))
  return false; //wrong file format!
 s3f_uint16_t crc = crc16(&rawdata[5], filesize - 5);
 m_file_crc_ok = (crc == p_fileHdr->crc16);
 if (p_fileHdr->nofsets < MIN_NOFSETS || p_fileHdr->nofsets > MAX_NOFSETS)
  return false; //incompatible

 //Size of a whole array of map sets
 size_t mapSetArrSize = sizeof(S3FMapSetItem) * p_fileHdr->nofsets;

 //resize container to hold all map sets
 m_data = FWMapsDataHolder(p_fileHdr->nofsets);

 //convert sets of tables
 const S3FMapSetItem* p_setItem = (S3FMapSetItem*)(&rawdata[sizeof(S3FFileHdr)]);
 for(size_t s = 0; s < p_fileHdr->nofsets; ++s)
 {
  size_t i;
  for(i = 0; i < F_STR_POINTS; ++i)
   m_data.maps[s].f_str[i] = p_setItem[s].f_str[i] / INT_MULTIPLIER;
  for(i = 0; i < F_IDL_POINTS; ++i)
   m_data.maps[s].f_idl[i] = p_setItem[s].f_idl[i] / INT_MULTIPLIER;
  for(i = 0; i < (F_WRK_POINTS_L * F_WRK_POINTS_F); ++i)
   m_data.maps[s].f_wrk[i] = p_setItem[s].f_wrk[i] / INT_MULTIPLIER;
  for(i = 0; i < F_TMP_POINTS; ++i)
   m_data.maps[s].f_tmp[i] = p_setItem[s].f_tmp[i] / INT_MULTIPLIER;
  //convert name
  char raw_string[F_NAME_SIZE + 1];
  memset(raw_string, 0, F_NAME_SIZE + 1);
  memcpy(raw_string, p_setItem[s].name, F_NAME_SIZE);
  TCHAR string[128];
  OemToChar(raw_string, string);
  m_data.maps[s].name = _TSTRING(string);
 }

 //convert separate maps
 const S3FSepMaps* p_sepMaps = (S3FSepMaps*)(&rawdata[sizeof(S3FFileHdr) + mapSetArrSize]);
 size_t i;
 for(i = 0; i < KC_ATTENUATOR_LOOKUP_TABLE_SIZE; ++i)
  m_data.attenuator_table[i] = p_sepMaps->attenuator_table[i] / INT_MULTIPLIER;
 for(i = 0; i < COIL_ON_TIME_LOOKUP_TABLE_SIZE; ++i)
  m_data.dwellcntrl_table[i] = p_sepMaps->dwellcntrl_table[i] / INT_MULTIPLIER;
 for(i = 0; i < THERMISTOR_LOOKUP_TABLE_SIZE; ++i)
  m_data.ctscurve_table[i] = p_sepMaps->ctscurve_table[i] / INT_MULTIPLIER;
 for(i = 0; i < 2; ++i)
  m_data.ctscurve_vlimits[i] = p_sepMaps->ctscurve_vlimits[i] / INT_MULTIPLIER;
 for(i = 0; i < CHOKE_CLOSING_LOOKUP_TABLE_SIZE; ++i)
  m_data.choke_op_table[i] = p_sepMaps->choke_op_table[i] / INT_MULTIPLIER;

 //convert RPM grid
 bool empty = true;
 for(i = 0; i < F_RPM_SLOTS; ++i)
 {
  if (0 != p_sepMaps->rpm_slots[i])
   empty = false;
  m_data.rpm_slots[i] = p_sepMaps->rpm_slots[i] / INT_MULTIPLIER;
 }

 if (empty || (p_fileHdr->version < 0x0102)) //copy standard RPM grid if old version of S3F is being loaded
  std::copy(SECU3IO::work_map_rpm_slots, SECU3IO::work_map_rpm_slots + F_RPM_SLOTS, m_data.rpm_slots);

 return true;
}

/////////////////////////////////////////////////////////////////////////////////////
bool S3FFileDataIO::Save(const _TSTRING i_file_name)
{
 CFile file;
 CFileException ex;
 TCHAR szError[1024];
 if(!file.Open(i_file_name.c_str(), CFile::modeWrite|CFile::modeCreate, &ex))
 {
  ex.GetErrorMessage(szError, 1024);
  AfxMessageBox(szError);
  return false;
 }

 size_t dataSize = MIN_OPTDATA_SIZE; //must be at least 1024 bytes
 size_t mapSetArrSize = sizeof(S3FMapSetItem) * m_data.maps.size(); //Size of a whole array of map sets
 size_t size = sizeof(S3FFileHdr) + mapSetArrSize + sizeof(S3FSepMaps) + dataSize;
 std::vector<BYTE> rawdata(size, 0);

 S3FFileHdr* p_fileHdr = (S3FFileHdr*)(&rawdata[0]);
 memcpy(p_fileHdr->hdr, "S3F", 3);
 p_fileHdr->btpmi = sizeof(s3f_int32_t);
 p_fileHdr->nofsets = m_data.maps.size();
 p_fileHdr->sofdat = dataSize; //size of additional data
 p_fileHdr->version = 0x0102; //01.02

 //convert sets of maps
 S3FMapSetItem* p_setItem = (S3FMapSetItem*)(&rawdata[sizeof(S3FFileHdr)]);
 for(size_t s = 0; s < p_fileHdr->nofsets; ++s)
 {
  size_t i;
  for(i = 0; i < F_STR_POINTS; ++i)
   p_setItem[s].f_str[i] = MathHelpers::Round(m_data.maps[s].f_str[i] * INT_MULTIPLIER);
  for(i = 0; i < F_IDL_POINTS; ++i)
   p_setItem[s].f_idl[i] = MathHelpers::Round(m_data.maps[s].f_idl[i] * INT_MULTIPLIER);
  for(i = 0; i < (F_WRK_POINTS_L * F_WRK_POINTS_F); ++i)
   p_setItem[s].f_wrk[i] = MathHelpers::Round(m_data.maps[s].f_wrk[i] * INT_MULTIPLIER);
  for(i = 0; i < F_TMP_POINTS; ++i)
   p_setItem[s].f_tmp[i] = MathHelpers::Round(m_data.maps[s].f_tmp[i] * INT_MULTIPLIER);

  //Convert name, string must be fixed length
  _TSTRING str = m_data.maps[s].name;
  while(str.length() < F_NAME_SIZE)
   str+=' ';
  char raw_string[F_NAME_SIZE + 1];
  memset(raw_string, 0, F_NAME_SIZE + 1);
  CharToOem(str.c_str(), raw_string);
  memcpy(p_setItem[s].name, raw_string, F_NAME_SIZE);
 }

 //convert separate maps
 S3FSepMaps* p_sepMaps = (S3FSepMaps*)(&rawdata[sizeof(S3FFileHdr) + mapSetArrSize]);
 size_t i;
 for(i = 0; i < KC_ATTENUATOR_LOOKUP_TABLE_SIZE; ++i)
  p_sepMaps->attenuator_table[i] = MathHelpers::Round(m_data.attenuator_table[i] * INT_MULTIPLIER);
 for(i = 0; i < COIL_ON_TIME_LOOKUP_TABLE_SIZE; ++i)
  p_sepMaps->dwellcntrl_table[i] = MathHelpers::Round(m_data.dwellcntrl_table[i] * INT_MULTIPLIER);
 for(i = 0; i < THERMISTOR_LOOKUP_TABLE_SIZE; ++i)
  p_sepMaps->ctscurve_table[i] = MathHelpers::Round(m_data.ctscurve_table[i] * INT_MULTIPLIER);
 for(i = 0; i < 2; ++i)
  p_sepMaps->ctscurve_vlimits[i] = MathHelpers::Round(m_data.ctscurve_vlimits[i] * INT_MULTIPLIER);
 for(i = 0; i < CHOKE_CLOSING_LOOKUP_TABLE_SIZE; ++i)
  p_sepMaps->choke_op_table[i] = MathHelpers::Round(m_data.choke_op_table[i] * INT_MULTIPLIER);

 //convert RPM grid
 for(i = 0; i < F_RPM_SLOTS; ++i)
  p_sepMaps->rpm_slots[i] = MathHelpers::Round(m_data.rpm_slots[i] * INT_MULTIPLIER);

 //Finally. Update file CRC and write the file
 p_fileHdr->crc16 = crc16(&rawdata[5], size - 5);
 file.Write(&rawdata[0], size);
 file.Close();
 return true;
}

const FWMapsDataHolder& S3FFileDataIO::GetData() const
{
 return m_data;
}

FWMapsDataHolder& S3FFileDataIO::GetDataLeft(void)
{
 return m_data;
}

bool S3FFileDataIO::IsFileIntegrityOk(void) const
{
 return m_file_crc_ok;
}
