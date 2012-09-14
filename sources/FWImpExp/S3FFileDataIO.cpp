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

#pragma pack( push, enter_S3FFileMap )
#pragma pack(1) //<--SECU3

//define our own types
typedef unsigned short s3f_uint16_t;
typedef signed short s3f_int16_t;
typedef unsigned char s3f_uint8_t;

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

struct S3FMapSet
{
 s3f_int16_t f_str[F_STR_POINTS]; //start
 s3f_int16_t f_idl[F_IDL_POINTS]; //idle 
 s3f_int16_t f_wrk[F_WRK_POINTS_L * F_WRK_POINTS_F]; //work
 s3f_int16_t f_tmp[F_TMP_POINTS]; //temperature
 s3f_uint8_t name[F_NAME_SIZE];   // ассоциированное имя (имя семейства)
};

struct S3FFileMaps
{
 S3FMapSet maps[TABLES_NUMBER]; //sets of tables
 s3f_int16_t attenuator_table[KC_ATTENUATOR_LOOKUP_TABLE_SIZE]; //attenuator table (for knock detection)
 s3f_int16_t dwellcntrl_table[COIL_ON_TIME_LOOKUP_TABLE_SIZE];  //dwell control look up table
 s3f_int16_t ctscurve_table[THERMISTOR_LOOKUP_TABLE_SIZE];      //coolant sensor look up table
 s3f_int16_t ctscurve_vlimits[2];    //volatge limits for coolant sensor look up table
 s3f_int16_t reserved[128];     //reserved bytes, = 0
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
 BYTE rawdata[65536];
 size_t filesize = (size_t)file.GetLength();
 if ((filesize > 65536) || (filesize < (sizeof(S3FFileHdr) + sizeof(S3FFileMaps))))
 {
  file.Close();
  return false; //некорректный размер файла
 }
 file.Read(rawdata, filesize);
 file.Close();

 //parse file's contents. 
 //(1) First of all check the file format
 //(2) Next we have to check CRC and store status.
 //(3) Convert maps
 const S3FFileHdr* p_fileHdr = (S3FFileHdr*)rawdata;
 if (memcmp(p_fileHdr->hdr, "S3F", 3))
  return false; //wrong file format!
 s3f_uint16_t crc = crc16(rawdata + 5, filesize - 5);
 m_file_crc_ok = (crc == p_fileHdr->crc16);
 if (p_fileHdr->nofsets < TABLES_NUMBER)
  return false; //incompatible

 const S3FFileMaps* p_fileMaps = (S3FFileMaps*)(rawdata + sizeof(S3FFileHdr));
 for(size_t s = 0; s < TABLES_NUMBER; ++s)
 {
  size_t i;
  for(i = 0; i < F_STR_POINTS; ++i)
   m_data.maps[s].f_str[i] = p_fileMaps->maps[s].f_str[i];
  for(i = 0; i < F_IDL_POINTS; ++i)
   m_data.maps[s].f_idl[i] = p_fileMaps->maps[s].f_idl[i];
  for(i = 0; i < (F_WRK_POINTS_L * F_WRK_POINTS_F); ++i)
   m_data.maps[s].f_wrk[i] = p_fileMaps->maps[s].f_wrk[i];
  for(i = 0; i < F_TMP_POINTS; ++i)
   m_data.maps[s].f_tmp[i] = p_fileMaps->maps[s].f_tmp[i];
  //convert name
  char raw_string[F_NAME_SIZE + 1];
  memset(raw_string, 0, F_NAME_SIZE + 1);
  memcpy(raw_string, p_fileMaps->maps[s].name, F_NAME_SIZE);
  TCHAR string[128];
  OemToChar(raw_string, string);
  m_data.maps[s].name = _TSTRING(string);
 }
 size_t i;
 for(i = 0; i < KC_ATTENUATOR_LOOKUP_TABLE_SIZE; ++i)
  m_data.attenuator_table[i] = p_fileMaps->attenuator_table[i];
 for(i = 0; i < COIL_ON_TIME_LOOKUP_TABLE_SIZE; ++i)
  m_data.dwellcntrl_table[i] = p_fileMaps->dwellcntrl_table[i];
 for(i = 0; i < THERMISTOR_LOOKUP_TABLE_SIZE; ++i)
  m_data.ctscurve_table[i] = p_fileMaps->ctscurve_table[i];
 for(i = 0; i < 2; ++i)
  m_data.ctscurve_vlimits[i] = p_fileMaps->ctscurve_vlimits[i];

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

 BYTE rawdata[65536];
 memset(rawdata, 0, 65536);
 size_t dataSize = 1024; //must be at least 1024 bytes
 size_t size = sizeof(S3FFileHdr) + sizeof(S3FFileMaps) + dataSize;
 S3FFileHdr* p_fileHdr = (S3FFileHdr*)(rawdata);
 memcpy(p_fileHdr->hdr, "S3F", 3);
 p_fileHdr->btpmi = sizeof(s3f_int16_t);
 p_fileHdr->nofsets = TABLES_NUMBER;
 p_fileHdr->sofdat = dataSize; //size of additional data
 p_fileHdr->version = 0x0100; //01.00

 S3FFileMaps* p_fileMaps = (S3FFileMaps*)(rawdata + sizeof(S3FFileHdr));
 for(size_t s = 0; s < TABLES_NUMBER; ++s)
 {
  size_t i;
  for(i = 0; i < F_STR_POINTS; ++i)
   p_fileMaps->maps[s].f_str[i] = MathHelpers::Round(m_data.maps[s].f_str[i]);
  for(i = 0; i < F_IDL_POINTS; ++i)
   p_fileMaps->maps[s].f_idl[i] = MathHelpers::Round(m_data.maps[s].f_idl[i]);
  for(i = 0; i < (F_WRK_POINTS_L * F_WRK_POINTS_F); ++i)
   p_fileMaps->maps[s].f_wrk[i] = MathHelpers::Round(m_data.maps[s].f_wrk[i]);
  for(i = 0; i < F_TMP_POINTS; ++i)
   p_fileMaps->maps[s].f_tmp[i] = MathHelpers::Round(m_data.maps[s].f_tmp[i]);

  //Convert name, string must be fixed length
  _TSTRING str = m_data.maps[s].name;
  while(str.length() < F_NAME_SIZE)
   str+=' ';
  char raw_string[F_NAME_SIZE + 1];
  memset(raw_string, 0, F_NAME_SIZE + 1);
  CharToOem(str.c_str(), raw_string);
  memcpy(p_fileMaps->maps[s].name, raw_string, F_NAME_SIZE);
 }
 size_t i;
 for(i = 0; i < KC_ATTENUATOR_LOOKUP_TABLE_SIZE; ++i)
  p_fileMaps->attenuator_table[i] = MathHelpers::Round(m_data.attenuator_table[i]);
 for(i = 0; i < COIL_ON_TIME_LOOKUP_TABLE_SIZE; ++i)
  p_fileMaps->dwellcntrl_table[i] = MathHelpers::Round(m_data.dwellcntrl_table[i]);
 for(i = 0; i < THERMISTOR_LOOKUP_TABLE_SIZE; ++i)
  p_fileMaps->ctscurve_table[i] = MathHelpers::Round(m_data.ctscurve_table[i]);
 for(i = 0; i < 2; ++i)
  p_fileMaps->ctscurve_vlimits[i] = MathHelpers::Round(m_data.ctscurve_vlimits[i]);

 //Finally. Update file CRC and write the file
 p_fileHdr->crc16 = crc16(rawdata + 5, size - 5);
 file.Write(rawdata, size);
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

int S3FFileDataIO::GetMapSetsNumber(void) const
{
 return TABLES_NUMBER;
}

bool S3FFileDataIO::IsFileIntegrityOk(void) const
{
 return m_file_crc_ok;
}
