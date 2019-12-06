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

/** \file FirmwareMapsDataHolder.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include <vector>
#include "common/unicodesupport.h"

//определяем количество узлов интерполяции для каждой функции
#define F_WRK_POINTS_F         16
#define F_WRK_POINTS_L         16
#define F_TMP_POINTS           16
#define F_STR_POINTS           16
#define F_IDL_POINTS           16
#define F_NAME_SIZE            16
#define KC_ATTENUATOR_LOOKUP_TABLE_SIZE 128
#define COIL_ON_TIME_LOOKUP_TABLE_SIZE 32
#define THERMISTOR_LOOKUP_TABLE_SIZE 16
#define F_RPM_SLOTS            16
#define ATS_CORR_LOOKUP_TABLE_SIZE 16
#define GASDOSE_POS_RPM_SIZE   16
#define GASDOSE_POS_TPS_SIZE   16
#define BAROCORR_SIZE          9
#define PA4_LOOKUP_TABLE_SIZE  16
#define CTS_CRKCORR_SIZE       16
#define CRANK_THRD_SIZE        16
#define CRANK_TIME_SIZE        16
#define SMAPABAN_THRD_SIZE     16

#define INJ_VE_POINTS_L        16
#define INJ_VE_POINTS_F        16
#define INJ_CRANKING_LOOKUP_TABLE_SIZE 16
#define INJ_WARMUP_LOOKUP_TABLE_SIZE 16
#define INJ_DT_LOOKUP_TABLE_SIZE 32
#define INJ_IAC_POS_TABLE_SIZE 16
#define INJ_AE_TPS_LOOKUP_TABLE_SIZE 8
#define INJ_AE_RPM_LOOKUP_TABLE_SIZE 4
#define INJ_AFTSTR_LOOKUP_TABLE_SIZE 16
#define INJ_TARGET_RPM_TABLE_SIZE 16
#define INJ_IDL_RIGIDITY_TABLE_SIZE 8
#define INJ_EGO_CURVE_SIZE     16
#define INJ_IAC_CORR_W_SIZE    16
#define INJ_IAC_CORR_SIZE      8
#define INJ_IATCLT_CORR_SIZE   8
#define INJ_TPSSWT_SIZE        16
#define INJ_GTS_CORR_SIZE      16
#define INJ_GPS_CORR_SIZE      9
#define INJ_ATS_CORR_SIZE      16

//количество наборов характеристик хранимых в памяти программ
#define TABLES_NUMBER          4
//количество наборов характеристик хранимых в EEPROM
#define TUNABLE_TABLES_NUMBER  1

struct SECU3FWMapsItem
{
 _TSTRING name;                // имя набора характеристик
 //ignition maps
 float f_str[F_STR_POINTS];    // функция УОЗ на старте
 float f_idl[F_IDL_POINTS];    // функция УОЗ для ХХ
 float f_wrk[F_WRK_POINTS_L * F_WRK_POINTS_F]; // основная функция УОЗ (3D)
 float f_tmp[F_TMP_POINTS];    // функция коррект. УОЗ по температуре
 //fuel injection maps
 float inj_ve[INJ_VE_POINTS_L * INJ_VE_POINTS_F];  // VE
 float inj_afr[INJ_VE_POINTS_L * INJ_VE_POINTS_F]; // AFR
 float inj_cranking[INJ_CRANKING_LOOKUP_TABLE_SIZE]; //Cranking PW
 float inj_warmup[INJ_WARMUP_LOOKUP_TABLE_SIZE];  // Warmup enrichment
 float inj_dead_time[INJ_DT_LOOKUP_TABLE_SIZE];   // Injector's dead time
 float inj_iac_run_pos[INJ_IAC_POS_TABLE_SIZE];  // IAC/PWM position on run
 float inj_iac_crank_pos[INJ_IAC_POS_TABLE_SIZE];// IAC/PWM position on cranking
 float inj_ae_tps[INJ_AE_TPS_LOOKUP_TABLE_SIZE * 2]; // bins and values of the AE's TPS lookup table
 float inj_ae_rpm[INJ_AE_RPM_LOOKUP_TABLE_SIZE * 2]; // bins and values of the AE's RPM lookup table
 float inj_aftstr[INJ_AFTSTR_LOOKUP_TABLE_SIZE]; //afterstart enrichment vs coolant temperature
 float inj_timing[INJ_VE_POINTS_L * INJ_VE_POINTS_F]; // Injection timing
 float inj_target_rpm[INJ_TARGET_RPM_TABLE_SIZE]; //Target RPM on idling
 float inj_idl_rigidity[INJ_IDL_RIGIDITY_TABLE_SIZE]; //Idling regulator's rigidity
 float inj_ego_curve[INJ_EGO_CURVE_SIZE+2]; //EGO curve (WBO sensor emulation)
 float inj_iac_corr_w[INJ_IAC_CORR_W_SIZE+2];   // IAC correction weight lookup table
 float inj_iac_corr[INJ_IAC_CORR_SIZE+2];       // IAC correction lookup table
 float inj_iatclt_corr[INJ_IATCLT_CORR_SIZE+2]; // IAT/CLT correction vs air flow
 float inj_tpsswt[INJ_TPSSWT_SIZE];             // MAP/TPS switch point vs RPM
 float inj_gts_corr[INJ_GTS_CORR_SIZE];         // PW correction vs gas temperature
 float inj_gps_corr[INJ_GPS_CORR_SIZE+2];       // PW correction vs gas pressure
 float inj_ats_corr[INJ_ATS_CORR_SIZE];         // PW correction vs air temperature
};

//Аппаратно независимое представление данных таблиц хранимых в прошивке SECU-3
struct FWMapsDataHolder
{
 std::vector<SECU3FWMapsItem> maps;
 float attenuator_table[KC_ATTENUATOR_LOOKUP_TABLE_SIZE];
 float dwellcntrl_table[COIL_ON_TIME_LOOKUP_TABLE_SIZE];
 float ctscurve_table[THERMISTOR_LOOKUP_TABLE_SIZE];
 float ctscurve_vlimits[2]; //voltage limits are stored together with table
 float atscurve_table[THERMISTOR_LOOKUP_TABLE_SIZE];
 float atscurve_vlimits[2]; //voltage limits are stored together with table
 float rpm_slots[F_RPM_SLOTS]; //сетка оборотов исполузуемая вместе с этими кривыми
 float ats_corr_table[ATS_CORR_LOOKUP_TABLE_SIZE];  //коррекция УОЗ по ДТВ
 float gasdose_pos_table[GASDOSE_POS_TPS_SIZE * GASDOSE_POS_RPM_SIZE]; //gas dosator position
 float barocorr_table[BAROCORR_SIZE+2]; //barometric correction
 float pa4_igntim_corr[PA4_LOOKUP_TABLE_SIZE];
 float tmp2_curve[THERMISTOR_LOOKUP_TABLE_SIZE+2];
 float ctscrk_corr[CTS_CRKCORR_SIZE];
 float eh_pause_table[COIL_ON_TIME_LOOKUP_TABLE_SIZE];
 float cranking_thrd[CRANK_THRD_SIZE];
 float cranking_time[CRANK_TIME_SIZE];
 float smapaban_thrd[SMAPABAN_THRD_SIZE];

 //default constructor
 FWMapsDataHolder(size_t setNum = TABLES_NUMBER)
 {
  static const SECU3FWMapsItem defval = {_TSTRING(_T("")),{.0f},{.0f},{.0f},{.0f},{.0f},{.0f},{.0f},{.0f},{.0f},{.0f},{.0f},{.0f},{.0f},{.0f},{.0f},{.0f},{.0f},{.0f},{.0f},{.0f},{.0f},{.0f},{.0f},{.0f},{.0f}};
  maps.assign(setNum, defval);
  std::fill(attenuator_table, attenuator_table + KC_ATTENUATOR_LOOKUP_TABLE_SIZE, .0f);
  std::fill(dwellcntrl_table, dwellcntrl_table + COIL_ON_TIME_LOOKUP_TABLE_SIZE, .0f);
  std::fill(ctscurve_table, ctscurve_table + THERMISTOR_LOOKUP_TABLE_SIZE, .0f);
  std::fill(ctscurve_vlimits, ctscurve_vlimits + 2, .0f);
  std::fill(rpm_slots, rpm_slots + F_RPM_SLOTS, .0f);
  std::fill(atscurve_table, atscurve_table + THERMISTOR_LOOKUP_TABLE_SIZE, .0f);
  std::fill(atscurve_vlimits, atscurve_vlimits + 2, .0f);
  std::fill(ats_corr_table, ats_corr_table + ATS_CORR_LOOKUP_TABLE_SIZE, .0f);
  std::fill(gasdose_pos_table, gasdose_pos_table + (GASDOSE_POS_TPS_SIZE * GASDOSE_POS_RPM_SIZE), .0f);
  std::fill(barocorr_table, barocorr_table + BAROCORR_SIZE + 2, .0f);  
  std::fill(pa4_igntim_corr, pa4_igntim_corr + PA4_LOOKUP_TABLE_SIZE, .0f);
  std::fill(tmp2_curve, tmp2_curve + THERMISTOR_LOOKUP_TABLE_SIZE + 2, .0f);
  std::fill(ctscrk_corr, ctscrk_corr + CTS_CRKCORR_SIZE, .0f);
  std::fill(eh_pause_table, eh_pause_table + COIL_ON_TIME_LOOKUP_TABLE_SIZE, .0f);
  std::fill(cranking_thrd, cranking_thrd + CRANK_THRD_SIZE, .0f);
  std::fill(cranking_time, cranking_time + CRANK_TIME_SIZE, .0f);
  std::fill(smapaban_thrd, smapaban_thrd + SMAPABAN_THRD_SIZE, .0f);
 }
 //get composed list of names
 std::vector<_TSTRING> GetListOfNames(void) const
 {
  std::vector<_TSTRING> list;
  for (size_t i = 0; i < maps.size(); i++)
   list.push_back(maps[i].name);
  return list;
 };
};
