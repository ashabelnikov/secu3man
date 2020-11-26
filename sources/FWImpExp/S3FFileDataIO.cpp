/* SECU-3  - An open source, free engine control unit
   Copyright (C) 2007 Alexey A. Shabelnikov. Ukraine, Kiev

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

/** \file S3FFileDataIO.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "resource.h"
#include "S3FFileDataIO.h"

#include <vector>
#include "common/MathHelpers.h"
#include "io-core/BitMask.h"
#include "io-core/CRC16.h"
#include "io-core/FirmwareMapsDataHolder.h"
#include "io-core/SECU3IO.h"
#include "ui-core/MsgBox.h"

#pragma pack( push, enter_S3FFileMap )
#pragma pack(1) //<--SECU3

#define INT_MULTIPLIER 10000.0f
#define MIN_OPTDATA_SIZE 1024
#define MIN_NOFSETS TABLES_NUMBER
#define MAX_NOFSETS 64
#define CURRENT_VERSION 0x0115 //01.15

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
// 01.03 - Major changes (02.11.2014). Fuel injection and intake air maps added.
// 01.04 - Gas dose position map added (20.12.2015)
// 01.05 - Injection timing and EGO curve maps were added (02.08.2017)
// 01.06 - Rest fuel injection maps were added (28.09.2017)
// 01.07 - Added ability to store single set of maps and no separate maps. (23.10.2017)
// 01.07 - Added TEMP2 curve map (28.12.2017)
// 01.08 - Abandoned choke opening map (body of map left for compatibility reasons)
// 01.09 - Added some maps which were not added yet (28.12.2018)
// 01.10 - Added 3 new separate tables which were not added yet
// 01.11 - CLT grid has been added (02.02.2020)
// 01.12 - Fixed overflow bug in inj_iatclt_corr map (05.05.2020)
// 01.13 - Added 3 new maps: pwm_duty1, pwm_duty2, knock_zone (15.09.2020)
// 01.14 - Added CE settings for ADD_I5/6/7/8, GRTS curve map, GRHEAT map (28.10.2020-30.10.2020)
// 01.15 - Added after start strokes maps: petrol and gas, PWM IAC voltage coeff. map (25.11.2020)

//Numbers of flag bits
#define S3FF_NOSEPMAPS 0

struct S3FFileHdr
{
 s3f_uint8_t  hdr[3];          //Contains 'S3F'
 s3f_uint16_t crc16;           //CRC16 of whole file (except first 5 bytes of file)
 s3f_uint16_t version;         //File version in xx.xx format
 s3f_uint8_t  nofsets;         //Number of set of tables
 s3f_uint8_t  btpmi;           //Bytes per 1 item in the map
 s3f_uint16_t sofdat;          //Size of optional data block, size must be >= 1024
 s3f_uint8_t  flags;           //flags (appeared in version v01.07): 
                               //  0 bit - indicates that S3F contain or not contain separate maps (0 - contains, 1 - no sep. maps)
 s3f_uint8_t  reserved[7];     //reserved bytes, = 0
};

//One entry of map sets array
struct S3FMapSetItem
{
 s3f_uint8_t name[F_NAME_SIZE];   // associated name (name of set of maps)
 //ignition
 s3f_int32_t f_str[F_STR_POINTS];                          //start
 s3f_int32_t f_idl[F_IDL_POINTS];                          //idle 
 s3f_int32_t f_wrk[F_WRK_POINTS_L * F_WRK_POINTS_F];       //work
 s3f_int32_t f_tmp[F_TMP_POINTS];                          //temperature
 //fuel injection, since v01.03
 s3f_int32_t inj_ve[INJ_VE_POINTS_L * INJ_VE_POINTS_F];    // VE
 s3f_int32_t inj_afr[INJ_VE_POINTS_L * INJ_VE_POINTS_F];   // AFR
 s3f_int32_t inj_cranking[INJ_CRANKING_LOOKUP_TABLE_SIZE]; // Cranking PW
 s3f_int32_t inj_warmup[INJ_WARMUP_LOOKUP_TABLE_SIZE];     // Warmup enrichment
 s3f_int32_t inj_dead_time[INJ_DT_LOOKUP_TABLE_SIZE];      // Injector's dead time
 s3f_int32_t inj_iac_run_pos[INJ_IAC_POS_TABLE_SIZE];      // IAC/PWM position on run
 s3f_int32_t inj_iac_crank_pos[INJ_IAC_POS_TABLE_SIZE];    // IAC/PWM position on cranking
 s3f_int32_t inj_ae_tps[INJ_AE_TPS_LOOKUP_TABLE_SIZE * 2]; // AE TPS (values and horizontal axis bins)
 s3f_int32_t inj_ae_rpm[INJ_AE_RPM_LOOKUP_TABLE_SIZE * 2]; // AE RPM (values and horizontal axis bins)
 s3f_int32_t inj_aftstr[INJ_AFTSTR_LOOKUP_TABLE_SIZE];     // Afterstart enrichment
 //injection timing and EGO curve, since v01.05
 s3f_int32_t inj_timing[INJ_VE_POINTS_L * INJ_VE_POINTS_F];// Injection timing
 s3f_int32_t inj_ego_curve[INJ_EGO_CURVE_SIZE+2];          // EGO curve
 //since v01.06
 s3f_int32_t inj_target_rpm[INJ_TARGET_RPM_TABLE_SIZE];    // Target RPM
 s3f_int32_t inj_idl_rigidity[INJ_IDL_RIGIDITY_TABLE_SIZE];// Idling regulator's rigidity
 s3f_int32_t inj_iac_corr_w[INJ_IAC_CORR_W_SIZE+2];        // IAC correction weight lookup table
 s3f_int32_t inj_iac_corr[INJ_IAC_CORR_SIZE+2];            // IAC correction lookup table
 s3f_int32_t inj_iatclt_corr[INJ_IATCLT_CORR_SIZE+2];      // IAT/CLT correction vs air flow
 //since v01.09
 s3f_int32_t inj_tpsswt[INJ_TPSSWT_SIZE];                  // MAP/TPS switch point vs RPM
 s3f_int32_t inj_gts_corr[INJ_GTS_CORR_SIZE];              // PW correction vs gas temperature
 s3f_int32_t inj_gps_corr[INJ_GPS_CORR_SIZE+2];            // PW correction vs gas pressure
 s3f_int32_t inj_ats_corr[INJ_ATS_CORR_SIZE];              // PW correction vs air temperature
 //since v01.13
 s3f_int32_t pwm_duty1[F_WRK_POINTS_L * F_WRK_POINTS_F];   // PWM1 duty
 s3f_int32_t pwm_duty2[F_WRK_POINTS_L * F_WRK_POINTS_F];   // PWM2 duty

 s3f_int32_t reserved[117]; //reserved bytes, = 0
};


//CE settings
typedef struct
{
 s3f_int32_t map_v_min;                //minimum correct value
 s3f_int32_t map_v_max;                //maximum correct value
 s3f_int32_t map_v_em;                 //emergency value, used in case of error
 s3f_int32_t map_v_flg;

 s3f_int32_t vbat_v_min;
 s3f_int32_t vbat_v_max;
 s3f_int32_t vbat_v_em;
 s3f_int32_t vbat_v_flg;

 s3f_int32_t cts_v_min;
 s3f_int32_t cts_v_max;
 s3f_int32_t cts_v_em;
 s3f_int32_t cts_v_flg;

 s3f_int32_t ks_v_min;
 s3f_int32_t ks_v_max;
 s3f_int32_t ks_v_em;
 s3f_int32_t ks_v_flg;

 s3f_int32_t tps_v_min;
 s3f_int32_t tps_v_max;
 s3f_int32_t tps_v_em;
 s3f_int32_t tps_v_flg;

 s3f_int32_t add_i1_v_min;
 s3f_int32_t add_i1_v_max;
 s3f_int32_t add_i1_v_em;
 s3f_int32_t add_i1_v_flg;

 s3f_int32_t add_i2_v_min;
 s3f_int32_t add_i2_v_max;
 s3f_int32_t add_i2_v_em;
 s3f_int32_t add_i2_v_flg;

 s3f_int32_t add_i3_v_min;
 s3f_int32_t add_i3_v_max;
 s3f_int32_t add_i3_v_em;
 s3f_int32_t add_i3_v_flg;

 s3f_int32_t add_i4_v_min;
 s3f_int32_t add_i4_v_max;
 s3f_int32_t add_i4_v_em;
 s3f_int32_t add_i4_v_flg;

 //since v01.14
 s3f_int32_t add_i5_v_min;
 s3f_int32_t add_i5_v_max;
 s3f_int32_t add_i5_v_em;
 s3f_int32_t add_i5_v_flg;

 s3f_int32_t add_i6_v_min;
 s3f_int32_t add_i6_v_max;
 s3f_int32_t add_i6_v_em;
 s3f_int32_t add_i6_v_flg;

 s3f_int32_t add_i7_v_min;
 s3f_int32_t add_i7_v_max;
 s3f_int32_t add_i7_v_em;
 s3f_int32_t add_i7_v_flg;

 s3f_int32_t add_i8_v_min;
 s3f_int32_t add_i8_v_max;
 s3f_int32_t add_i8_v_em;
 s3f_int32_t add_i8_v_flg;

 s3f_int32_t reserved[496];
}s3f_ce_sett_t;


//Separate maps
struct S3FSepMaps
{
 s3f_int32_t attenuator_table[KC_ATTENUATOR_LOOKUP_TABLE_SIZE]; //attenuator table (for knock detection)
 s3f_int32_t dwellcntrl_table[COIL_ON_TIME_LOOKUP_TABLE_SIZE];  //dwell control look up table
 s3f_int32_t ctscurve_table[THERMISTOR_LOOKUP_TABLE_SIZE];      //coolant sensor look up table
 s3f_int32_t ctscurve_vlimits[2]; //volatge limits for coolant sensor look up table
 s3f_int32_t choke_op_table[16]; //choke opening map (appeared in version 1.01, abandoned 02 Feb 2018 and left for compatibility reasons)
 s3f_int32_t rpm_slots[F_RPM_SLOTS]; //RPM grid (appeared in version 1.02, reserved bytes were utilized)
 s3f_int32_t atscurve_table[THERMISTOR_LOOKUP_TABLE_SIZE];      //intake air temperature sensor look up table, since v01.03
 s3f_int32_t atscurve_vlimits[2]; //volatge limits for intake air temperature sensor look up table, since v01.03
 s3f_int32_t ats_corr_table[ATS_CORR_LOOKUP_TABLE_SIZE];      //advance angle correction form intake air temperature look up table, since v01.03
 s3f_int32_t gasdose_pos_table[GASDOSE_POS_TPS_SIZE * GASDOSE_POS_RPM_SIZE]; //gas dose position map
 s3f_int32_t tmp2_curve[THERMISTOR_LOOKUP_TABLE_SIZE+2];      //TEMP2 temperature sensor look up table, since v01.07
 //since v01.09
 s3f_int32_t barocorr_table[BAROCORR_SIZE+2]; //barometric correction
 s3f_int32_t pa4_igntim_corr[PA4_LOOKUP_TABLE_SIZE];
 s3f_int32_t ctscrk_corr[CTS_CRKCORR_SIZE];
 s3f_int32_t eh_pause_table[COIL_ON_TIME_LOOKUP_TABLE_SIZE];
 //since v01.10
 s3f_int32_t cranking_thrd[CRANK_THRD_SIZE];
 s3f_int32_t cranking_time[CRANK_TIME_SIZE];
 s3f_int32_t smapaban_thrd[SMAPABAN_THRD_SIZE];
 //since v01.11
 s3f_int32_t clt_slots[F_TMP_SLOTS]; //CLT grid (appeared in version 01.11, reserved bytes were utilized)
 s3f_ce_sett_t cesd; //CE settings' data
 //since v01.13
 s3f_int32_t knock_zone[F_WRK_POINTS_L * F_WRK_POINTS_F]; //knock zones map
 //since v01.14
 s3f_int32_t grts_curve[THERMISTOR_LOOKUP_TABLE_SIZE+2];      //GRTS temperature sensor look up table, since v01.14 
 s3f_int32_t grheat_duty[F_TMP_POINTS];                       //temperature
 s3f_int32_t load_slots[F_LOAD_SLOTS]; //load grid (appeared in version 01.14, reserved bytes were utilized); note: values are in reverse direction

 //since v01.15
 s3f_int32_t pwmiac_ucoef[PWMIAC_UCOEF_SIZE];
 s3f_int32_t aftstr_strk0[AFTSTR_STRK_SIZE];
 s3f_int32_t aftstr_strk1[AFTSTR_STRK_SIZE];

 s3f_int32_t reserved[733];       //reserved bytes, = 0
};


//One entry of map sets array
struct S3FMapSetItem_v0102
{
 s3f_int32_t f_str[F_STR_POINTS]; //start
 s3f_int32_t f_idl[F_IDL_POINTS]; //idle 
 s3f_int32_t f_wrk[F_WRK_POINTS_L * F_WRK_POINTS_F]; //work
 s3f_int32_t f_tmp[F_TMP_POINTS]; //temperature
 s3f_uint8_t name[F_NAME_SIZE];   //assosiated name of set
};

//Separate maps
struct S3FSepMaps_v0102
{
 s3f_int32_t attenuator_table[KC_ATTENUATOR_LOOKUP_TABLE_SIZE]; //attenuator table (for knock detection)
 s3f_int32_t dwellcntrl_table[COIL_ON_TIME_LOOKUP_TABLE_SIZE];  //dwell control look up table
 s3f_int32_t ctscurve_table[THERMISTOR_LOOKUP_TABLE_SIZE];      //coolant sensor look up table
 s3f_int32_t ctscurve_vlimits[2]; //volatge limits for coolant sensor look up table
 s3f_int32_t choke_op_table[16]; //choke opening map (appeared in version 1.01, abandoned 02 Feb 2018 and left for compatibility reasons)
 s3f_int32_t rpm_slots[F_RPM_SLOTS]; //RPM grid (appeared in version 1.02, reserved bytes were utilized)
 s3f_int32_t reserved[96];       //reserved bytes, = 0
};

#pragma pack( pop, enter_S3FFileMap )

/////////////////////////////////////////////////////////////////////////////////////
S3FFileDataIO::S3FFileDataIO(bool sepmaps /*= true*/)
: m_file_crc_ok(false)
, m_version(0)
, m_sepmaps(sepmaps)
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
  SECUMessageBox(szError);
  return false;
 }
 //read the file into memory
 size_t filesize = (size_t)file.GetLength();
 size_t minFileSize = sizeof(S3FFileHdr) + (sizeof(S3FMapSetItem) * 1) + sizeof(S3FSepMaps) + MIN_OPTDATA_SIZE;
 size_t minFileSize_v0106 = sizeof(S3FFileHdr) + (sizeof(S3FMapSetItem) * MIN_NOFSETS) + sizeof(S3FSepMaps) + MIN_OPTDATA_SIZE;
 size_t minFileSize_v0102 = sizeof(S3FFileHdr) + (sizeof(S3FMapSetItem_v0102) * MIN_NOFSETS) + sizeof(S3FSepMaps_v0102) + MIN_OPTDATA_SIZE;
 if ((filesize > 1048576) || ((filesize < minFileSize) && (filesize < minFileSize_v0102) && (filesize < minFileSize_v0106)))
 {
  file.Close();
  return false; //некорректный размер файла
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
 int min_nofsets = (p_fileHdr->version <= 0x0106) ? MIN_NOFSETS : 1;
 if (p_fileHdr->nofsets < min_nofsets || p_fileHdr->nofsets > MAX_NOFSETS)
  return false; //incompatible

 m_version = p_fileHdr->version;  //save version number
 m_sepmaps = !CHECKBIT8(p_fileHdr->flags, S3FF_NOSEPMAPS); //save sep.maps presence flag

 if (p_fileHdr->version <= 0x0102)
 {
  return _ReadData_v0102(&rawdata[0], p_fileHdr);
 }
 else
  return _ReadData(&rawdata[0], p_fileHdr);
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
  SECUMessageBox(szError);
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
 p_fileHdr->version = CURRENT_VERSION; //version of S3F file
 p_fileHdr->flags = 0; //flags
 WRITEBIT8(p_fileHdr->flags, S3FF_NOSEPMAPS, !m_sepmaps);
 
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
  //fuel injection
  for(i = 0; i < (INJ_VE_POINTS_L * INJ_VE_POINTS_F); ++i)
   p_setItem[s].inj_ve[i] = MathHelpers::Round(m_data.maps[s].inj_ve[i] * INT_MULTIPLIER);
  for(i = 0; i < (INJ_VE_POINTS_L * INJ_VE_POINTS_F); ++i)
   p_setItem[s].inj_afr[i] = MathHelpers::Round(m_data.maps[s].inj_afr[i] * INT_MULTIPLIER);
  for(i = 0; i < INJ_CRANKING_LOOKUP_TABLE_SIZE; ++i)
   p_setItem[s].inj_cranking[i] = MathHelpers::Round(m_data.maps[s].inj_cranking[i] * INT_MULTIPLIER);
  for(i = 0; i < INJ_WARMUP_LOOKUP_TABLE_SIZE; ++i)
   p_setItem[s].inj_warmup[i] = MathHelpers::Round(m_data.maps[s].inj_warmup[i] * INT_MULTIPLIER);
  for(i = 0; i < INJ_DT_LOOKUP_TABLE_SIZE; ++i)
   p_setItem[s].inj_dead_time[i] = MathHelpers::Round(m_data.maps[s].inj_dead_time[i] * INT_MULTIPLIER);
  for(i = 0; i < INJ_IAC_POS_TABLE_SIZE; ++i)
   p_setItem[s].inj_iac_run_pos[i] = MathHelpers::Round(m_data.maps[s].inj_iac_run_pos[i] * INT_MULTIPLIER);
  for(i = 0; i < INJ_IAC_POS_TABLE_SIZE; ++i)
   p_setItem[s].inj_iac_crank_pos[i] = MathHelpers::Round(m_data.maps[s].inj_iac_crank_pos[i] * INT_MULTIPLIER);
  for(i = 0; i < INJ_AE_TPS_LOOKUP_TABLE_SIZE * 2; ++i) //size*2 because values and bins are in one
   p_setItem[s].inj_ae_tps[i] = MathHelpers::Round(m_data.maps[s].inj_ae_tps[i] * INT_MULTIPLIER);
  for(i = 0; i < INJ_AE_RPM_LOOKUP_TABLE_SIZE * 2; ++i)
   p_setItem[s].inj_ae_rpm[i] = MathHelpers::Round(m_data.maps[s].inj_ae_rpm[i] * INT_MULTIPLIER);
  for(i = 0; i < INJ_AFTSTR_LOOKUP_TABLE_SIZE; ++i)
   p_setItem[s].inj_aftstr[i] = MathHelpers::Round(m_data.maps[s].inj_aftstr[i] * INT_MULTIPLIER);
  for(i = 0; i < (INJ_VE_POINTS_L * INJ_VE_POINTS_F); ++i)
   p_setItem[s].inj_timing[i] = MathHelpers::Round(m_data.maps[s].inj_timing[i] * INT_MULTIPLIER);
  for(i = 0; i < INJ_EGO_CURVE_SIZE+2; ++i)
   p_setItem[s].inj_ego_curve[i] = MathHelpers::Round(m_data.maps[s].inj_ego_curve[i] * INT_MULTIPLIER);
  for(i = 0; i < INJ_TARGET_RPM_TABLE_SIZE; ++i)
   p_setItem[s].inj_target_rpm[i] = MathHelpers::Round(m_data.maps[s].inj_target_rpm[i] * INT_MULTIPLIER);
  for(i = 0; i < INJ_IDL_RIGIDITY_TABLE_SIZE; ++i)
   p_setItem[s].inj_idl_rigidity[i] = MathHelpers::Round(m_data.maps[s].inj_idl_rigidity[i] * INT_MULTIPLIER);
  for(i = 0; i < INJ_IAC_CORR_W_SIZE+2; ++i)
   p_setItem[s].inj_iac_corr_w[i] = MathHelpers::Round(m_data.maps[s].inj_iac_corr_w[i] * INT_MULTIPLIER);
  for(i = 0; i < INJ_IAC_CORR_SIZE+2; ++i)
   p_setItem[s].inj_iac_corr[i] = MathHelpers::Round(m_data.maps[s].inj_iac_corr[i] * INT_MULTIPLIER);
  for(i = 0; i < INJ_IATCLT_CORR_SIZE+2; ++i)
  {
   if (i < INJ_IATCLT_CORR_SIZE)
    p_setItem[s].inj_iatclt_corr[i] = MathHelpers::Round(m_data.maps[s].inj_iatclt_corr[i] * INT_MULTIPLIER);
   else
    p_setItem[s].inj_iatclt_corr[i] = MathHelpers::Round(m_data.maps[s].inj_iatclt_corr[i]); //do not use INT_MULTIPLIER for last 2 values (preventing overflow)
  }
  for(i = 0; i < INJ_TPSSWT_SIZE; ++i)
   p_setItem[s].inj_tpsswt[i] = MathHelpers::Round(m_data.maps[s].inj_tpsswt[i] * INT_MULTIPLIER);
  for(i = 0; i < INJ_GTS_CORR_SIZE; ++i)
   p_setItem[s].inj_gts_corr[i] = MathHelpers::Round(m_data.maps[s].inj_gts_corr[i] * INT_MULTIPLIER);
  for(i = 0; i < INJ_GPS_CORR_SIZE+2; ++i)
   p_setItem[s].inj_gps_corr[i] = MathHelpers::Round(m_data.maps[s].inj_gps_corr[i] * INT_MULTIPLIER);
  for(i = 0; i < INJ_ATS_CORR_SIZE; ++i)
   p_setItem[s].inj_ats_corr[i] = MathHelpers::Round(m_data.maps[s].inj_ats_corr[i] * INT_MULTIPLIER);
  for(i = 0; i < (F_WRK_POINTS_L * F_WRK_POINTS_F); ++i)
   p_setItem[s].pwm_duty1[i] = MathHelpers::Round(m_data.maps[s].pwm_duty1[i] * INT_MULTIPLIER);
  for(i = 0; i < (F_WRK_POINTS_L * F_WRK_POINTS_F); ++i)
   p_setItem[s].pwm_duty2[i] = MathHelpers::Round(m_data.maps[s].pwm_duty2[i] * INT_MULTIPLIER);

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
 for(i = 0; i < 16; ++i)
  p_sepMaps->choke_op_table[i] = MathHelpers::Round(0 * INT_MULTIPLIER); //abandoned, fill with 0.0
 for(i = 0; i < THERMISTOR_LOOKUP_TABLE_SIZE; ++i)
  p_sepMaps->atscurve_table[i] = MathHelpers::Round(m_data.atscurve_table[i] * INT_MULTIPLIER);
 for(i = 0; i < 2; ++i)
  p_sepMaps->atscurve_vlimits[i] = MathHelpers::Round(m_data.atscurve_vlimits[i] * INT_MULTIPLIER);
 for(i = 0; i < ATS_CORR_LOOKUP_TABLE_SIZE; ++i)
  p_sepMaps->ats_corr_table[i] = MathHelpers::Round(m_data.ats_corr_table[i] * INT_MULTIPLIER);
 for(i = 0; i < (GASDOSE_POS_TPS_SIZE * GASDOSE_POS_RPM_SIZE); ++i)
  p_sepMaps->gasdose_pos_table[i] = MathHelpers::Round(m_data.gasdose_pos_table[i] * INT_MULTIPLIER);
 for(i = 0; i < THERMISTOR_LOOKUP_TABLE_SIZE+2; ++i)
  p_sepMaps->tmp2_curve[i] = MathHelpers::Round(m_data.tmp2_curve[i] * INT_MULTIPLIER);
 for(i = 0; i < BAROCORR_SIZE+2; ++i)
  p_sepMaps->barocorr_table[i] = MathHelpers::Round(m_data.barocorr_table[i] * INT_MULTIPLIER);
 for(i = 0; i < PA4_LOOKUP_TABLE_SIZE; ++i)
  p_sepMaps->pa4_igntim_corr[i] = MathHelpers::Round(m_data.pa4_igntim_corr[i] * INT_MULTIPLIER);
 for(i = 0; i < CTS_CRKCORR_SIZE; ++i)
  p_sepMaps->ctscrk_corr[i] = MathHelpers::Round(m_data.ctscrk_corr[i] * INT_MULTIPLIER);
 for(i = 0; i < COIL_ON_TIME_LOOKUP_TABLE_SIZE; ++i)
  p_sepMaps->eh_pause_table[i] = MathHelpers::Round(m_data.eh_pause_table[i] * INT_MULTIPLIER);
 for(i = 0; i < CRANK_THRD_SIZE; ++i)
  p_sepMaps->cranking_thrd[i] = MathHelpers::Round(m_data.cranking_thrd[i] * INT_MULTIPLIER);
 for(i = 0; i < CRANK_TIME_SIZE; ++i)
  p_sepMaps->cranking_time[i] = MathHelpers::Round(m_data.cranking_time[i] * INT_MULTIPLIER);
 for(i = 0; i < SMAPABAN_THRD_SIZE; ++i)
  p_sepMaps->smapaban_thrd[i] = MathHelpers::Round(m_data.smapaban_thrd[i] * INT_MULTIPLIER);
 for(i = 0; i < (F_WRK_POINTS_L * F_WRK_POINTS_F); ++i)
  p_sepMaps->knock_zone[i] = MathHelpers::Round(m_data.knock_zone[i] * INT_MULTIPLIER);
 for(i = 0; i < THERMISTOR_LOOKUP_TABLE_SIZE+2; ++i)
  p_sepMaps->grts_curve[i] = MathHelpers::Round(m_data.grts_curve[i] * INT_MULTIPLIER);
 for(i = 0; i < F_TMP_POINTS; ++i)
  p_sepMaps->grheat_duty[i] = MathHelpers::Round(m_data.grheat_duty[i] * INT_MULTIPLIER);
 for(i = 0; i < PWMIAC_UCOEF_SIZE; ++i)
  p_sepMaps->pwmiac_ucoef[i] = MathHelpers::Round(m_data.pwmiac_ucoef[i] * INT_MULTIPLIER);
 for(i = 0; i < AFTSTR_STRK_SIZE; ++i)
  p_sepMaps->aftstr_strk0[i] = MathHelpers::Round(m_data.aftstr_strk0[i] * INT_MULTIPLIER);
 for(i = 0; i < AFTSTR_STRK_SIZE; ++i)
  p_sepMaps->aftstr_strk1[i] = MathHelpers::Round(m_data.aftstr_strk1[i] * INT_MULTIPLIER);

 //convert RPM grid
 for(i = 0; i < F_RPM_SLOTS; ++i)
  p_sepMaps->rpm_slots[i] = MathHelpers::Round(m_data.rpm_slots[i] * INT_MULTIPLIER);
 //convert CLT grid
 for(i = 0; i < F_TMP_SLOTS; ++i)
  p_sepMaps->clt_slots[i] = MathHelpers::Round(m_data.clt_slots[i] * INT_MULTIPLIER);
 //convert load grid
 for(i = 0; i < F_LOAD_SLOTS; ++i)
  p_sepMaps->load_slots[i] = MathHelpers::Round(m_data.load_slots[i] * INT_MULTIPLIER);

 //CE settings
 p_sepMaps->cesd.map_v_min = MathHelpers::Round(m_data.cesd.map_v_min * INT_MULTIPLIER);
 p_sepMaps->cesd.map_v_max = MathHelpers::Round(m_data.cesd.map_v_max * INT_MULTIPLIER);
 p_sepMaps->cesd.map_v_em = MathHelpers::Round(m_data.cesd.map_v_em * INT_MULTIPLIER);
 WRITEBIT8(p_sepMaps->cesd.map_v_flg, 0, m_data.cesd.map_v_useem);

 p_sepMaps->cesd.vbat_v_min = MathHelpers::Round(m_data.cesd.vbat_v_min * INT_MULTIPLIER);
 p_sepMaps->cesd.vbat_v_max = MathHelpers::Round(m_data.cesd.vbat_v_max * INT_MULTIPLIER);
 p_sepMaps->cesd.vbat_v_em = MathHelpers::Round(m_data.cesd.vbat_v_em * INT_MULTIPLIER);
 WRITEBIT8(p_sepMaps->cesd.vbat_v_flg, 0, m_data.cesd.vbat_v_useem);

 p_sepMaps->cesd.cts_v_min = MathHelpers::Round(m_data.cesd.cts_v_min * INT_MULTIPLIER);
 p_sepMaps->cesd.cts_v_max = MathHelpers::Round(m_data.cesd.cts_v_max * INT_MULTIPLIER);
 p_sepMaps->cesd.cts_v_em = MathHelpers::Round(m_data.cesd.cts_v_em * INT_MULTIPLIER);
 WRITEBIT8(p_sepMaps->cesd.cts_v_flg, 0, m_data.cesd.cts_v_useem);

 p_sepMaps->cesd.ks_v_min = MathHelpers::Round(m_data.cesd.ks_v_min * INT_MULTIPLIER);
 p_sepMaps->cesd.ks_v_max = MathHelpers::Round(m_data.cesd.ks_v_max * INT_MULTIPLIER);
 p_sepMaps->cesd.ks_v_em = MathHelpers::Round(m_data.cesd.ks_v_em * INT_MULTIPLIER);
 WRITEBIT8(p_sepMaps->cesd.ks_v_flg, 0, m_data.cesd.ks_v_useem);

 p_sepMaps->cesd.tps_v_min = MathHelpers::Round(m_data.cesd.tps_v_min * INT_MULTIPLIER);
 p_sepMaps->cesd.tps_v_max = MathHelpers::Round(m_data.cesd.tps_v_max * INT_MULTIPLIER);
 p_sepMaps->cesd.tps_v_em = MathHelpers::Round(m_data.cesd.tps_v_em * INT_MULTIPLIER);
 WRITEBIT8(p_sepMaps->cesd.tps_v_flg, 0, m_data.cesd.tps_v_useem);

 p_sepMaps->cesd.add_i1_v_min = MathHelpers::Round(m_data.cesd.add_i1_v_min * INT_MULTIPLIER);
 p_sepMaps->cesd.add_i1_v_max = MathHelpers::Round(m_data.cesd.add_i1_v_max * INT_MULTIPLIER);
 p_sepMaps->cesd.add_i1_v_em = MathHelpers::Round(m_data.cesd.add_i1_v_em * INT_MULTIPLIER);
 WRITEBIT8(p_sepMaps->cesd.add_i1_v_flg, 0, m_data.cesd.add_i1_v_useem);

 p_sepMaps->cesd.add_i2_v_min = MathHelpers::Round(m_data.cesd.add_i2_v_min * INT_MULTIPLIER);
 p_sepMaps->cesd.add_i2_v_max = MathHelpers::Round(m_data.cesd.add_i2_v_max * INT_MULTIPLIER);
 p_sepMaps->cesd.add_i2_v_em = MathHelpers::Round(m_data.cesd.add_i2_v_em * INT_MULTIPLIER);
 WRITEBIT8(p_sepMaps->cesd.add_i2_v_flg, 0, m_data.cesd.add_i2_v_useem);

 p_sepMaps->cesd.add_i3_v_min = MathHelpers::Round(m_data.cesd.add_i3_v_min * INT_MULTIPLIER);
 p_sepMaps->cesd.add_i3_v_max = MathHelpers::Round(m_data.cesd.add_i3_v_max * INT_MULTIPLIER);
 p_sepMaps->cesd.add_i3_v_em = MathHelpers::Round(m_data.cesd.add_i3_v_em * INT_MULTIPLIER);
 WRITEBIT8(p_sepMaps->cesd.add_i3_v_flg, 0, m_data.cesd.add_i3_v_useem);

 p_sepMaps->cesd.add_i4_v_min = MathHelpers::Round(m_data.cesd.add_i4_v_min * INT_MULTIPLIER);
 p_sepMaps->cesd.add_i4_v_max = MathHelpers::Round(m_data.cesd.add_i4_v_max * INT_MULTIPLIER);
 p_sepMaps->cesd.add_i4_v_em = MathHelpers::Round(m_data.cesd.add_i4_v_em * INT_MULTIPLIER);
 WRITEBIT8(p_sepMaps->cesd.add_i4_v_flg, 0, m_data.cesd.add_i4_v_useem);

 p_sepMaps->cesd.add_i5_v_min = MathHelpers::Round(m_data.cesd.add_i5_v_min * INT_MULTIPLIER);
 p_sepMaps->cesd.add_i5_v_max = MathHelpers::Round(m_data.cesd.add_i5_v_max * INT_MULTIPLIER);
 p_sepMaps->cesd.add_i5_v_em = MathHelpers::Round(m_data.cesd.add_i5_v_em * INT_MULTIPLIER);
 WRITEBIT8(p_sepMaps->cesd.add_i5_v_flg, 0, m_data.cesd.add_i5_v_useem);

 p_sepMaps->cesd.add_i6_v_min = MathHelpers::Round(m_data.cesd.add_i6_v_min * INT_MULTIPLIER);
 p_sepMaps->cesd.add_i6_v_max = MathHelpers::Round(m_data.cesd.add_i6_v_max * INT_MULTIPLIER);
 p_sepMaps->cesd.add_i6_v_em = MathHelpers::Round(m_data.cesd.add_i6_v_em * INT_MULTIPLIER);
 WRITEBIT8(p_sepMaps->cesd.add_i6_v_flg, 0, m_data.cesd.add_i6_v_useem);

 p_sepMaps->cesd.add_i7_v_min = MathHelpers::Round(m_data.cesd.add_i7_v_min * INT_MULTIPLIER);
 p_sepMaps->cesd.add_i7_v_max = MathHelpers::Round(m_data.cesd.add_i7_v_max * INT_MULTIPLIER);
 p_sepMaps->cesd.add_i7_v_em = MathHelpers::Round(m_data.cesd.add_i7_v_em * INT_MULTIPLIER);
 WRITEBIT8(p_sepMaps->cesd.add_i7_v_flg, 0, m_data.cesd.add_i7_v_useem);

 p_sepMaps->cesd.add_i8_v_min = MathHelpers::Round(m_data.cesd.add_i8_v_min * INT_MULTIPLIER);
 p_sepMaps->cesd.add_i8_v_max = MathHelpers::Round(m_data.cesd.add_i8_v_max * INT_MULTIPLIER);
 p_sepMaps->cesd.add_i8_v_em = MathHelpers::Round(m_data.cesd.add_i8_v_em * INT_MULTIPLIER);
 WRITEBIT8(p_sepMaps->cesd.add_i8_v_flg, 0, m_data.cesd.add_i8_v_useem);

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

unsigned short S3FFileDataIO::GetVersion(void) const
{
 return m_version;
}

bool S3FFileDataIO::HasSeparateMaps(void) const
{
 return m_sepmaps;
}

bool S3FFileDataIO::_ReadData(const BYTE* rawdata, const S3FFileHdr* p_fileHdr)
{
 //Size of a whole array of map sets
 size_t mapSetArrSize = sizeof(S3FMapSetItem) * p_fileHdr->nofsets;

 //resize container to hold all map sets
 m_data = FWMapsDataHolder(p_fileHdr->nofsets);

 bool wrong_iatclt_corr_x_grid = (p_fileHdr->version < 0x0112);

 //convert sets of tables
 const S3FMapSetItem* p_setItem = (S3FMapSetItem*)(&rawdata[sizeof(S3FFileHdr)]);
 for(size_t s = 0; s < p_fileHdr->nofsets; ++s)
 {
  size_t i;
  //ignition
  for(i = 0; i < F_STR_POINTS; ++i)
   m_data.maps[s].f_str[i] = p_setItem[s].f_str[i] / INT_MULTIPLIER;
  for(i = 0; i < F_IDL_POINTS; ++i)
   m_data.maps[s].f_idl[i] = p_setItem[s].f_idl[i] / INT_MULTIPLIER;
  for(i = 0; i < (F_WRK_POINTS_L * F_WRK_POINTS_F); ++i)
   m_data.maps[s].f_wrk[i] = p_setItem[s].f_wrk[i] / INT_MULTIPLIER;
  for(i = 0; i < F_TMP_POINTS; ++i)
   m_data.maps[s].f_tmp[i] = p_setItem[s].f_tmp[i] / INT_MULTIPLIER;
  //fuel injection
  for(i = 0; i < (INJ_VE_POINTS_L * INJ_VE_POINTS_F); ++i)
   m_data.maps[s].inj_ve[i] = p_setItem[s].inj_ve[i] / INT_MULTIPLIER;
  for(i = 0; i < (INJ_VE_POINTS_L * INJ_VE_POINTS_F); ++i)
   m_data.maps[s].inj_afr[i] = p_setItem[s].inj_afr[i] / INT_MULTIPLIER;
  for(i = 0; i < INJ_CRANKING_LOOKUP_TABLE_SIZE; ++i)
   m_data.maps[s].inj_cranking[i] = p_setItem[s].inj_cranking[i] / INT_MULTIPLIER;
  for(i = 0; i < INJ_WARMUP_LOOKUP_TABLE_SIZE; ++i)
   m_data.maps[s].inj_warmup[i] = p_setItem[s].inj_warmup[i] / INT_MULTIPLIER;
  for(i = 0; i < INJ_DT_LOOKUP_TABLE_SIZE; ++i)
   m_data.maps[s].inj_dead_time[i] = p_setItem[s].inj_dead_time[i] / INT_MULTIPLIER;
  for(i = 0; i < INJ_IAC_POS_TABLE_SIZE; ++i)
   m_data.maps[s].inj_iac_run_pos[i] = p_setItem[s].inj_iac_run_pos[i] / INT_MULTIPLIER;
  for(i = 0; i < INJ_IAC_POS_TABLE_SIZE; ++i)
   m_data.maps[s].inj_iac_crank_pos[i] = p_setItem[s].inj_iac_crank_pos[i] / INT_MULTIPLIER;
  for(i = 0; i < INJ_AE_TPS_LOOKUP_TABLE_SIZE * 2; ++i) //size*2 because values and bins are in one
   m_data.maps[s].inj_ae_tps[i] = p_setItem[s].inj_ae_tps[i] / INT_MULTIPLIER;
  for(i = 0; i < INJ_AE_RPM_LOOKUP_TABLE_SIZE * 2; ++i)
   m_data.maps[s].inj_ae_rpm[i] = p_setItem[s].inj_ae_rpm[i] / INT_MULTIPLIER;
  for(i = 0; i < INJ_AFTSTR_LOOKUP_TABLE_SIZE; ++i)
   m_data.maps[s].inj_aftstr[i] = p_setItem[s].inj_aftstr[i] / INT_MULTIPLIER;
  for(i = 0; i < (INJ_VE_POINTS_L * INJ_VE_POINTS_F); ++i)
   m_data.maps[s].inj_timing[i] = p_setItem[s].inj_timing[i] / INT_MULTIPLIER;
  for(i = 0; i < INJ_EGO_CURVE_SIZE+2; ++i)
   m_data.maps[s].inj_ego_curve[i] = p_setItem[s].inj_ego_curve[i] / INT_MULTIPLIER;
  for(i = 0; i < INJ_TARGET_RPM_TABLE_SIZE; ++i)
   m_data.maps[s].inj_target_rpm[i] = p_setItem[s].inj_target_rpm[i] / INT_MULTIPLIER;
  for(i = 0; i < INJ_IDL_RIGIDITY_TABLE_SIZE; ++i)
   m_data.maps[s].inj_idl_rigidity[i] = p_setItem[s].inj_idl_rigidity[i] / INT_MULTIPLIER;
  for(i = 0; i < INJ_IAC_CORR_W_SIZE+2; ++i)
   m_data.maps[s].inj_iac_corr_w[i] = p_setItem[s].inj_iac_corr_w[i] / INT_MULTIPLIER;
  for(i = 0; i < INJ_IAC_CORR_SIZE+2; ++i)
   m_data.maps[s].inj_iac_corr[i] = p_setItem[s].inj_iac_corr[i] / INT_MULTIPLIER;
  for(i = 0; i < INJ_IATCLT_CORR_SIZE+2; ++i)
  {
   if (i < INJ_IATCLT_CORR_SIZE)
    m_data.maps[s].inj_iatclt_corr[i] = p_setItem[s].inj_iatclt_corr[i] / INT_MULTIPLIER;
   else if (!wrong_iatclt_corr_x_grid)
    m_data.maps[s].inj_iatclt_corr[i] = (float)p_setItem[s].inj_iatclt_corr[i]; //do not use INT_MULTIPLIER
  }
  if (wrong_iatclt_corr_x_grid)
  { //use default values when loading from old version of S3F (which contains bug)
   m_data.maps[s].inj_iatclt_corr[INJ_IATCLT_CORR_SIZE] = 4992;
   m_data.maps[s].inj_iatclt_corr[INJ_IATCLT_CORR_SIZE+1] = 960000;
  }
  for(i = 0; i < INJ_TPSSWT_SIZE; ++i)
   m_data.maps[s].inj_tpsswt[i] = p_setItem[s].inj_tpsswt[i] / INT_MULTIPLIER;
  for(i = 0; i < INJ_GTS_CORR_SIZE; ++i)
   m_data.maps[s].inj_gts_corr[i] = p_setItem[s].inj_gts_corr[i] / INT_MULTIPLIER;
  for(i = 0; i < INJ_GPS_CORR_SIZE+2; ++i)
   m_data.maps[s].inj_gps_corr[i] = p_setItem[s].inj_gps_corr[i] / INT_MULTIPLIER;
  for(i = 0; i < INJ_ATS_CORR_SIZE; ++i)
   m_data.maps[s].inj_ats_corr[i] = p_setItem[s].inj_ats_corr[i] / INT_MULTIPLIER;
  for(i = 0; i < (F_WRK_POINTS_L * F_WRK_POINTS_F); ++i)
   m_data.maps[s].pwm_duty1[i] = p_setItem[s].pwm_duty1[i] / INT_MULTIPLIER;
  for(i = 0; i < (F_WRK_POINTS_L * F_WRK_POINTS_F); ++i)
   m_data.maps[s].pwm_duty2[i] = p_setItem[s].pwm_duty2[i] / INT_MULTIPLIER;

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
 for(i = 0; i < THERMISTOR_LOOKUP_TABLE_SIZE; ++i)
  m_data.atscurve_table[i] = p_sepMaps->atscurve_table[i] / INT_MULTIPLIER;
 for(i = 0; i < 2; ++i)
  m_data.atscurve_vlimits[i] = p_sepMaps->atscurve_vlimits[i] / INT_MULTIPLIER;
 for(i = 0; i < ATS_CORR_LOOKUP_TABLE_SIZE; ++i)
  m_data.ats_corr_table[i] = p_sepMaps->ats_corr_table[i] / INT_MULTIPLIER;
 for(i = 0; i < (GASDOSE_POS_TPS_SIZE * GASDOSE_POS_RPM_SIZE); ++i)
  m_data.gasdose_pos_table[i] = p_sepMaps->gasdose_pos_table[i] / INT_MULTIPLIER;
 for(i = 0; i < THERMISTOR_LOOKUP_TABLE_SIZE+2; ++i)
  m_data.tmp2_curve[i] = p_sepMaps->tmp2_curve[i] / INT_MULTIPLIER;
 for(i = 0; i < BAROCORR_SIZE+2; ++i)
  m_data.barocorr_table[i] = p_sepMaps->barocorr_table[i] / INT_MULTIPLIER;
 for(i = 0; i < PA4_LOOKUP_TABLE_SIZE; ++i)
  m_data.pa4_igntim_corr[i] = p_sepMaps->pa4_igntim_corr[i] / INT_MULTIPLIER;
 for(i = 0; i < CTS_CRKCORR_SIZE; ++i)
  m_data.ctscrk_corr[i] = p_sepMaps->ctscrk_corr[i] / INT_MULTIPLIER;
 for(i = 0; i < COIL_ON_TIME_LOOKUP_TABLE_SIZE; ++i)
  m_data.eh_pause_table[i] = p_sepMaps->eh_pause_table[i] / INT_MULTIPLIER;
 for(i = 0; i < CRANK_THRD_SIZE; ++i)
  m_data.cranking_thrd[i] = p_sepMaps->cranking_thrd[i] / INT_MULTIPLIER;
 for(i = 0; i < CRANK_TIME_SIZE; ++i)
  m_data.cranking_time[i] = p_sepMaps->cranking_time[i] / INT_MULTIPLIER;
 for(i = 0; i < SMAPABAN_THRD_SIZE; ++i)
  m_data.smapaban_thrd[i] = p_sepMaps->smapaban_thrd[i] / INT_MULTIPLIER;
 for(i = 0; i < (F_WRK_POINTS_L * F_WRK_POINTS_F); ++i)
  m_data.knock_zone[i] = p_sepMaps->knock_zone[i] / INT_MULTIPLIER;
 for(i = 0; i < THERMISTOR_LOOKUP_TABLE_SIZE+2; ++i)
  m_data.grts_curve[i] = p_sepMaps->grts_curve[i] / INT_MULTIPLIER;
 for(i = 0; i < F_TMP_POINTS; ++i)
  m_data.grheat_duty[i] = p_sepMaps->grheat_duty[i] / INT_MULTIPLIER;
 for(i = 0; i < PWMIAC_UCOEF_SIZE; ++i)
  m_data.pwmiac_ucoef[i] = p_sepMaps->pwmiac_ucoef[i] / INT_MULTIPLIER;
 for(i = 0; i < AFTSTR_STRK_SIZE; ++i)
  m_data.aftstr_strk0[i] = p_sepMaps->aftstr_strk0[i] / INT_MULTIPLIER;
 for(i = 0; i < AFTSTR_STRK_SIZE; ++i)
  m_data.aftstr_strk1[i] = p_sepMaps->aftstr_strk1[i] / INT_MULTIPLIER;

 //convert RPM grid
 for(i = 0; i < F_RPM_SLOTS; ++i)
  m_data.rpm_slots[i] = p_sepMaps->rpm_slots[i] / INT_MULTIPLIER;

 //convert CLT grid
 bool empty = true;
 for(i = 0; i < F_TMP_SLOTS; ++i)
 {
  if (0 != p_sepMaps->clt_slots[i])
   empty = false;
  m_data.clt_slots[i] = p_sepMaps->clt_slots[i] / INT_MULTIPLIER;
 }

 //convert load grid
 empty = true;
 for(i = 0; i < F_LOAD_SLOTS; ++i)
 {
  if (0 != p_sepMaps->load_slots[i])
   empty = false;
  m_data.load_slots[i] = p_sepMaps->load_slots[i] / INT_MULTIPLIER;
 }

 if (empty || (p_fileHdr->version < 0x0111)) //copy standard CLT grid if old version of S3F is being loaded
  std::copy(SECU3IO::temp_map_tmp_slots, SECU3IO::temp_map_tmp_slots + F_TMP_SLOTS, m_data.clt_slots);

 if (empty || (p_fileHdr->version < 0x0114)) //copy standard load grid if old version of S3F is being loaded
  std::copy(SECU3IO::work_map_lod_slots, SECU3IO::work_map_lod_slots + F_LOAD_SLOTS, m_data.load_slots);

 if (p_fileHdr->version >= 0x0111) //CE settings appeared since v01.11
 {
  //CE settings
  m_data.cesd.map_v_min = p_sepMaps->cesd.map_v_min / INT_MULTIPLIER;
  m_data.cesd.map_v_max = p_sepMaps->cesd.map_v_max / INT_MULTIPLIER;
  m_data.cesd.map_v_em = p_sepMaps->cesd.map_v_em / INT_MULTIPLIER;
  m_data.cesd.map_v_useem = CHECKBIT8(p_sepMaps->cesd.map_v_flg, 0);

  m_data.cesd.vbat_v_min = p_sepMaps->cesd.vbat_v_min / INT_MULTIPLIER;
  m_data.cesd.vbat_v_max = p_sepMaps->cesd.vbat_v_max / INT_MULTIPLIER;
  m_data.cesd.vbat_v_em = p_sepMaps->cesd.vbat_v_em / INT_MULTIPLIER;
  m_data.cesd.vbat_v_useem = CHECKBIT8(p_sepMaps->cesd.vbat_v_flg, 0);

  m_data.cesd.cts_v_min = p_sepMaps->cesd.cts_v_min / INT_MULTIPLIER;
  m_data.cesd.cts_v_max = p_sepMaps->cesd.cts_v_max / INT_MULTIPLIER;
  m_data.cesd.cts_v_em = p_sepMaps->cesd.cts_v_em / INT_MULTIPLIER;
  m_data.cesd.cts_v_useem = CHECKBIT8(p_sepMaps->cesd.cts_v_flg, 0);

  m_data.cesd.ks_v_min = p_sepMaps->cesd.ks_v_min / INT_MULTIPLIER;
  m_data.cesd.ks_v_max = p_sepMaps->cesd.ks_v_max / INT_MULTIPLIER;
  m_data.cesd.ks_v_em = p_sepMaps->cesd.ks_v_em / INT_MULTIPLIER;
  m_data.cesd.ks_v_useem = CHECKBIT8(p_sepMaps->cesd.ks_v_flg, 0);

  m_data.cesd.tps_v_min = p_sepMaps->cesd.tps_v_min / INT_MULTIPLIER;
  m_data.cesd.tps_v_max = p_sepMaps->cesd.tps_v_max / INT_MULTIPLIER;
  m_data.cesd.tps_v_em = p_sepMaps->cesd.tps_v_em / INT_MULTIPLIER;
  m_data.cesd.tps_v_useem = CHECKBIT8(p_sepMaps->cesd.tps_v_flg, 0);

  m_data.cesd.add_i1_v_min = p_sepMaps->cesd.add_i1_v_min / INT_MULTIPLIER;
  m_data.cesd.add_i1_v_max = p_sepMaps->cesd.add_i1_v_max / INT_MULTIPLIER;
  m_data.cesd.add_i1_v_em = p_sepMaps->cesd.add_i1_v_em / INT_MULTIPLIER;
  m_data.cesd.add_i1_v_useem = CHECKBIT8(p_sepMaps->cesd.add_i1_v_flg, 0);

  m_data.cesd.add_i2_v_min = p_sepMaps->cesd.add_i2_v_min / INT_MULTIPLIER;
  m_data.cesd.add_i2_v_max = p_sepMaps->cesd.add_i2_v_max / INT_MULTIPLIER;
  m_data.cesd.add_i2_v_em = p_sepMaps->cesd.add_i2_v_em / INT_MULTIPLIER;
  m_data.cesd.add_i2_v_useem = CHECKBIT8(p_sepMaps->cesd.add_i2_v_flg, 0);

  m_data.cesd.add_i3_v_min = p_sepMaps->cesd.add_i3_v_min / INT_MULTIPLIER;
  m_data.cesd.add_i3_v_max = p_sepMaps->cesd.add_i3_v_max / INT_MULTIPLIER;
  m_data.cesd.add_i3_v_em = p_sepMaps->cesd.add_i3_v_em / INT_MULTIPLIER;
  m_data.cesd.add_i3_v_useem = CHECKBIT8(p_sepMaps->cesd.add_i3_v_flg, 0);

  m_data.cesd.add_i4_v_min = p_sepMaps->cesd.add_i4_v_min / INT_MULTIPLIER;
  m_data.cesd.add_i4_v_max = p_sepMaps->cesd.add_i4_v_max / INT_MULTIPLIER;
  m_data.cesd.add_i4_v_em = p_sepMaps->cesd.add_i4_v_em / INT_MULTIPLIER;
  m_data.cesd.add_i4_v_useem = CHECKBIT8(p_sepMaps->cesd.add_i4_v_flg, 0);

  m_data.cesd.add_i5_v_min = p_sepMaps->cesd.add_i5_v_min / INT_MULTIPLIER;
  m_data.cesd.add_i5_v_max = p_sepMaps->cesd.add_i5_v_max / INT_MULTIPLIER;
  m_data.cesd.add_i5_v_em = p_sepMaps->cesd.add_i5_v_em / INT_MULTIPLIER;
  m_data.cesd.add_i5_v_useem = CHECKBIT8(p_sepMaps->cesd.add_i5_v_flg, 0);

  m_data.cesd.add_i6_v_min = p_sepMaps->cesd.add_i6_v_min / INT_MULTIPLIER;
  m_data.cesd.add_i6_v_max = p_sepMaps->cesd.add_i6_v_max / INT_MULTIPLIER;
  m_data.cesd.add_i6_v_em = p_sepMaps->cesd.add_i6_v_em / INT_MULTIPLIER;
  m_data.cesd.add_i6_v_useem = CHECKBIT8(p_sepMaps->cesd.add_i6_v_flg, 0);

  m_data.cesd.add_i7_v_min = p_sepMaps->cesd.add_i7_v_min / INT_MULTIPLIER;
  m_data.cesd.add_i7_v_max = p_sepMaps->cesd.add_i7_v_max / INT_MULTIPLIER;
  m_data.cesd.add_i7_v_em = p_sepMaps->cesd.add_i7_v_em / INT_MULTIPLIER;
  m_data.cesd.add_i7_v_useem = CHECKBIT8(p_sepMaps->cesd.add_i7_v_flg, 0);

  m_data.cesd.add_i8_v_min = p_sepMaps->cesd.add_i8_v_min / INT_MULTIPLIER;
  m_data.cesd.add_i8_v_max = p_sepMaps->cesd.add_i8_v_max / INT_MULTIPLIER;
  m_data.cesd.add_i8_v_em = p_sepMaps->cesd.add_i8_v_em / INT_MULTIPLIER;
  m_data.cesd.add_i8_v_useem = CHECKBIT8(p_sepMaps->cesd.add_i8_v_flg, 0);
 }

 return true;
}

//Function for reading data from older formats
bool S3FFileDataIO::_ReadData_v0102(const BYTE* rawdata, const S3FFileHdr* p_fileHdr)
{
 //Size of a whole array of map sets
 size_t mapSetArrSize = sizeof(S3FMapSetItem_v0102) * p_fileHdr->nofsets;

 //resize container to hold all map sets
 m_data = FWMapsDataHolder(p_fileHdr->nofsets);

 //convert sets of tables
 const S3FMapSetItem_v0102* p_setItem = (S3FMapSetItem_v0102*)(&rawdata[sizeof(S3FFileHdr)]);
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
 const S3FSepMaps_v0102* p_sepMaps = (S3FSepMaps_v0102*)(&rawdata[sizeof(S3FFileHdr) + mapSetArrSize]);
 size_t i;
 for(i = 0; i < KC_ATTENUATOR_LOOKUP_TABLE_SIZE; ++i)
  m_data.attenuator_table[i] = p_sepMaps->attenuator_table[i] / INT_MULTIPLIER;
 for(i = 0; i < COIL_ON_TIME_LOOKUP_TABLE_SIZE; ++i)
  m_data.dwellcntrl_table[i] = p_sepMaps->dwellcntrl_table[i] / INT_MULTIPLIER;
 for(i = 0; i < THERMISTOR_LOOKUP_TABLE_SIZE; ++i)
  m_data.ctscurve_table[i] = p_sepMaps->ctscurve_table[i] / INT_MULTIPLIER;
 for(i = 0; i < 2; ++i)
  m_data.ctscurve_vlimits[i] = p_sepMaps->ctscurve_vlimits[i] / INT_MULTIPLIER;

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

 //copy standard CLT grid when loading old format
 std::copy(SECU3IO::temp_map_tmp_slots, SECU3IO::temp_map_tmp_slots + F_TMP_SLOTS, m_data.clt_slots);

 //copy standard load grid when loading old format
 std::copy(SECU3IO::work_map_lod_slots, SECU3IO::work_map_lod_slots + F_LOAD_SLOTS, m_data.load_slots);

 return true;
}
