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
#include "io-core/MapIds.h"

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
#define F_TMP_SLOTS            16
#define F_LOAD_SLOTS           16
#define F_IRPM_SLOTS           8
#define F_ILOAD_SLOTS          8
#define F_TLOAD_SLOTS          16
#define ATS_CORR_LOOKUP_TABLE_SIZE 16
#define GASDOSE_POS_RPM_SIZE   16
#define GASDOSE_POS_TPS_SIZE   16
#define BAROCORR_SIZE          9
#define PA4_LOOKUP_TABLE_SIZE  16
#define CTS_CRKCORR_SIZE       16
#define CRANK_THRD_SIZE        16
#define CRANK_TIME_SIZE        16
#define SMAPABAN_THRD_SIZE     16
#define AFTSTR_STRK_SIZE       16
#define KNKZONE_TPS_SIZE       16
#define EGOZONE_LOAD_SIZE      16

#define INJ_VE_POINTS_L        16
#define INJ_VE_POINTS_F        16
#define INJ_IVE_POINTS_L       8
#define INJ_IVE_POINTS_F       8
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
#define INJ_TPSZON_SIZE        16
#define INJ_GTS_CORR_SIZE      16
#define INJ_GPS_CORR_SIZE      17
#define INJ_ATS_CORR_SIZE      16
#define PWMIAC_UCOEF_SIZE      16
#define FTLS_LOOKUP_TABLE_SIZE 17          // Size of "fuel tank level vs voltage" map
#define EGTS_LOOKUP_TABLE_SIZE 17          // Size of "exhaust gas temperature vs voltage" map
#define OPS_LOOKUP_TABLE_SIZE  17          // Size of "oil pressure vs voltage" map
#define INJPWCOEF_LUT_SIZE     17
#define MAF_FLOW_CURVE_SIZE    64          // Size of MAF flow curve lookup table
#define FTLSCOR_UCOEF_SIZE     32
#define INJ_CYLADD_SIZE        8
#define INJ_AE_MAP_LOOKUP_TABLE_SIZE 8
#define INJ_THRASS_SIZE        16
#define FTS_LOOKUP_TABLE_SIZE  17
#define FUELDENS_CORR_SIZE     16
#define XTAU_FACT_SIZE         16
#define INJ_NONLIN_SIZE        8
#define EGO_DELAY_SIZE         16
#define WU_AFR_SIZE            16
#define ETC_SPRPREL_SIZE       8
#define ETC_ACCEPTERR_SIZE     6
#define ETC_POS_APPS_SIZE      16
#define ETC_POS_RPM_SIZE       16
#define OTS_LOOKUP_TABLE_SIZE  17           // oil temperature sensor

//Number of set of maps stored in flash (read only memory)
#define TABLES_NUMBER          4
//Number of set of maps stored in EEPROM
#define TUNABLE_TABLES_NUMBER  1

struct CESettingsData
{  
 CESettingsData()
 : map_v_min(.0f), map_v_max(.0f), map_v_em(.0f), map_v_useem(false)
 , vbat_v_min(.0f), vbat_v_max(.0f), vbat_v_em(.0f), vbat_v_useem(false)
 , cts_v_min(.0f), cts_v_max(.0f), cts_v_em(.0f), cts_v_useem(false)
 , ks_v_min(.0f), ks_v_max(.0f), ks_v_em(.0f), ks_v_useem(false)
 , tps_v_min(.0f), tps_v_max(.0f), tps_v_em(.0f), tps_v_useem(false)
 , add_i1_v_min(.0f), add_i1_v_max(.0f), add_i1_v_em(.0f), add_i1_v_useem(false)
 , add_i2_v_min(.0f), add_i2_v_max(.0f), add_i2_v_em(.0f), add_i2_v_useem(false)
 , add_i3_v_min(.0f), add_i3_v_max(.0f), add_i3_v_em(.0f), add_i3_v_useem(false)
 , add_i4_v_min(.0f), add_i4_v_max(.0f), add_i4_v_em(.0f), add_i4_v_useem(false)
 , add_i5_v_min(.0f), add_i5_v_max(.0f), add_i5_v_em(.0f), add_i5_v_useem(false)
 , add_i6_v_min(.0f), add_i6_v_max(.0f), add_i6_v_em(.0f), add_i6_v_useem(false)
 , add_i7_v_min(.0f), add_i7_v_max(.0f), add_i7_v_em(.0f), add_i7_v_useem(false)
 , add_i8_v_min(.0f), add_i8_v_max(.0f), add_i8_v_em(.0f), add_i8_v_useem(false)
 , oilpress_thrd(.0f), oilpress_timer(0), stepperic_flg(false), tpsdiff_thrd(.0f)
 , appsdiff_thrd(.0f), addi5678_flg(false)
 {}

 float map_v_min;
 float map_v_max;
 float map_v_em;
 bool  map_v_useem;

 float vbat_v_min;
 float vbat_v_max;
 float vbat_v_em;
 bool  vbat_v_useem;

 float cts_v_min;
 float cts_v_max;
 float cts_v_em;
 bool  cts_v_useem;

 float ks_v_min;
 float ks_v_max;
 float ks_v_em;
 bool  ks_v_useem;

 float tps_v_min;
 float tps_v_max;
 float tps_v_em;
 bool  tps_v_useem;

 float add_i1_v_min;
 float add_i1_v_max;
 float add_i1_v_em;
 bool  add_i1_v_useem;

 float add_i2_v_min;
 float add_i2_v_max;
 float add_i2_v_em;
 bool  add_i2_v_useem;

 float add_i3_v_min;
 float add_i3_v_max;
 float add_i3_v_em;
 bool  add_i3_v_useem;

 float add_i4_v_min;
 float add_i4_v_max;
 float add_i4_v_em;
 bool  add_i4_v_useem;

 float add_i5_v_min;
 float add_i5_v_max;
 float add_i5_v_em;
 bool  add_i5_v_useem;

 float add_i6_v_min;
 float add_i6_v_max;
 float add_i6_v_em;
 bool  add_i6_v_useem;

 float add_i7_v_min;
 float add_i7_v_max;
 float add_i7_v_em;
 bool  add_i7_v_useem;

 float add_i8_v_min;
 float add_i8_v_max;
 float add_i8_v_em;
 bool  add_i8_v_useem;

 float oilpress_thrd;
 int  oilpress_timer;

 bool stepperic_flg;

 float tpsdiff_thrd;
 float appsdiff_thrd;

 bool addi5678_flg;
};

struct SECU3FWMapsItem
{
 SECU3FWMapsItem()
 : name(_T(""))
 {
  for (int i = ETMT_SET_START; i <= ETMT_SET_END; ++i)
   std::fill(GetMap(i), GetMap(i) + GetMapSize(i), .0f); 
 };

 _TSTRING name;                // имя набора характеристик
 //ignition maps
 float f_str[F_STR_POINTS];    // функция УОЗ на старте
 float f_idl[F_IDL_POINTS];    // функция УОЗ для ХХ
 float f_wrk[F_WRK_POINTS_L * F_WRK_POINTS_F]; // основная функция УОЗ (3D)
 float f_tmp[F_TMP_POINTS];    // функция коррект. УОЗ по температуре
 float f_tmp_idl[F_TMP_POINTS];// ignition timing correction vs CLT (idling)
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
 float pwm_duty1[F_WRK_POINTS_L * F_WRK_POINTS_F]; //PWM1 duty
 float pwm_duty2[F_WRK_POINTS_L * F_WRK_POINTS_F]; //PWM2 duty
 float iac_mat_corr[INJ_ATS_CORR_SIZE];         //IAC position's correction vs MAT
 float inj_ve2[INJ_VE_POINTS_L * INJ_VE_POINTS_F]; // Secondary VE
 float inj_tpszon[INJ_TPSZON_SIZE];             // MAP/TPS load axis allocation
 float inj_cylmult[INJ_CYLADD_SIZE];            // Inj. PW multiplier
 float inj_cyladd[INJ_CYLADD_SIZE];             // Inj. PW addition
 float inj_ae_map[INJ_AE_MAP_LOOKUP_TABLE_SIZE * 2]; // bins and values of the AE's MAP lookup table
 float inj_thrass[INJ_THRASS_SIZE];             //IAC's throttle assistant map
 float inj_ive[INJ_IVE_POINTS_L * INJ_IVE_POINTS_F];// Idling VE

 float* GetMap(int i_mapType)
 {
  switch(i_mapType)
  {
   case ETMT_IGN_START: return f_str; //ignition maps
   case ETMT_IGN_IDLE: return f_idl;
   case ETMT_IGN_WORK: return f_wrk;
   case ETMT_IGN_TEMP: return f_tmp;
   case ETMT_IGN_TEMPI: return f_tmp_idl;  
   case ETMT_INJ_VE: return inj_ve;      //fuel injection
   case ETMT_INJ_VE2: return inj_ve2;
   case ETMT_INJ_AFR: return inj_afr;
   case ETMT_INJ_CRNK: return inj_cranking;
   case ETMT_INJ_WRMP: return inj_warmup;
   case ETMT_INJ_DEAD: return inj_dead_time;
   case ETMT_INJ_IDLR: return inj_iac_run_pos;
   case ETMT_INJ_IDLC: return inj_iac_crank_pos;
   case ETMT_INJ_THRASS: return inj_thrass;
   case ETMT_INJ_AETPS: return inj_ae_tps;
   case ETMT_INJ_AEMAP: return inj_ae_map;
   case ETMT_INJ_AERPM: return inj_ae_rpm;
   case ETMT_INJ_AFTSTR: return inj_aftstr;
   case ETMT_INJ_IT: return inj_timing;
   case ETMT_INJ_ITRPM: return inj_target_rpm;
   case ETMT_INJ_RIGID: return inj_idl_rigidity;
   case ETMT_INJ_EGOCRV: return inj_ego_curve;
   case ETMT_INJ_IACC: return inj_iac_corr;
   case ETMT_INJ_IACCW: return inj_iac_corr_w;
   case ETMT_INJ_IATCLT: return inj_iatclt_corr;
   case ETMT_INJ_TPSSWT: return inj_tpsswt;
   case ETMT_INJ_GTSC: return inj_gts_corr;
   case ETMT_INJ_GPSC: return inj_gps_corr;
   case ETMT_INJ_ATSC: return inj_ats_corr;
   case ETMT_PWM1: return pwm_duty1;
   case ETMT_PWM2: return pwm_duty2;
   case ETMT_INJ_IACMAT: return iac_mat_corr;
   case ETMT_INJ_TPSZON: return inj_tpszon;
   case ETMT_INJ_CYLMULT: return inj_cylmult;
   case ETMT_INJ_CYLADD: return inj_cyladd;
   case ETMT_INJ_IVE: return inj_ive;      //fuel injection
  }
  return NULL; //undefined type of map
 }

 static size_t GetMapSize(int i_mapType)
 {
  switch(i_mapType)
  {
   case ETMT_IGN_START: return F_STR_POINTS; //ignition maps
   case ETMT_IGN_IDLE: return F_IDL_POINTS;
   case ETMT_IGN_WORK: return F_WRK_POINTS_L * F_WRK_POINTS_F;
   case ETMT_IGN_TEMP: return F_TMP_POINTS;
   case ETMT_IGN_TEMPI: return F_TMP_POINTS;
   case ETMT_INJ_VE: return INJ_VE_POINTS_L * INJ_VE_POINTS_F; //fuel injection maps
   case ETMT_INJ_VE2: return INJ_VE_POINTS_L * INJ_VE_POINTS_F;
   case ETMT_INJ_AFR: return INJ_VE_POINTS_L * INJ_VE_POINTS_F;
   case ETMT_INJ_CRNK: return INJ_CRANKING_LOOKUP_TABLE_SIZE;
   case ETMT_INJ_WRMP: return INJ_WARMUP_LOOKUP_TABLE_SIZE;
   case ETMT_INJ_DEAD: return INJ_DT_LOOKUP_TABLE_SIZE;
   case ETMT_INJ_IDLR:
   case ETMT_INJ_IDLC: return INJ_IAC_POS_TABLE_SIZE;
   case ETMT_INJ_THRASS: return INJ_THRASS_SIZE;
   case ETMT_INJ_AETPS: return INJ_AE_TPS_LOOKUP_TABLE_SIZE * 2; //bins and values
   case ETMT_INJ_AEMAP: return INJ_AE_MAP_LOOKUP_TABLE_SIZE * 2; //bins and values
   case ETMT_INJ_AERPM: return INJ_AE_RPM_LOOKUP_TABLE_SIZE * 2; //bins and values
   case ETMT_INJ_AFTSTR: return INJ_AFTSTR_LOOKUP_TABLE_SIZE;
   case ETMT_INJ_IT: return INJ_VE_POINTS_L * INJ_VE_POINTS_F;
   case ETMT_INJ_ITRPM: return INJ_TARGET_RPM_TABLE_SIZE;
   case ETMT_INJ_RIGID: return INJ_IDL_RIGIDITY_TABLE_SIZE;
   case ETMT_INJ_EGOCRV: return INJ_EGO_CURVE_SIZE+2;
   case ETMT_INJ_IACC: return INJ_IAC_CORR_SIZE+2;
   case ETMT_INJ_IACCW: return INJ_IAC_CORR_W_SIZE+2;
   case ETMT_INJ_IATCLT: return INJ_IATCLT_CORR_SIZE+2;
   case ETMT_INJ_TPSSWT: return INJ_TPSSWT_SIZE;
   case ETMT_INJ_GTSC: return INJ_GTS_CORR_SIZE;
   case ETMT_INJ_GPSC: return INJ_GPS_CORR_SIZE+2;
   case ETMT_INJ_ATSC: return INJ_ATS_CORR_SIZE;
   case ETMT_PWM1: return F_WRK_POINTS_L * F_WRK_POINTS_F;
   case ETMT_PWM2: return F_WRK_POINTS_L * F_WRK_POINTS_F;
   case ETMT_INJ_IACMAT: return INJ_ATS_CORR_SIZE;
   case ETMT_INJ_TPSZON: return INJ_TPSZON_SIZE;
   case ETMT_INJ_CYLMULT:
   case ETMT_INJ_CYLADD: return INJ_CYLADD_SIZE;
   case ETMT_INJ_IVE: return INJ_IVE_POINTS_L * INJ_IVE_POINTS_F; //VE map for idling
  }
  ASSERT(0);
  return 0; //undefined type of map
 }
};

//Аппаратно независимое представление данных таблиц хранимых в прошивке SECU-3
struct FWMapsDataHolder
{
 std::vector<SECU3FWMapsItem> maps;
 float rpm_slots[F_RPM_SLOTS];   //RPM grid used with these curves
 float clt_slots[F_TMP_SLOTS];   //CLT grid used with these curves
 float load_slots[F_LOAD_SLOTS]; //Load grid used with these curves
 float irpm_slots[F_IRPM_SLOTS]; //RPM grid for idling VE
 float iload_slots[F_ILOAD_SLOTS]; //Load grid for idling VE
 float tload_slots[F_TLOAD_SLOTS]; //Load grid for TPS (VE2)
 float attenuator_table[KC_ATTENUATOR_LOOKUP_TABLE_SIZE];
 float dwellcntrl_table[COIL_ON_TIME_LOOKUP_TABLE_SIZE];
 float ctscurve_table[THERMISTOR_LOOKUP_TABLE_SIZE+2]; //voltage limits are stored together with table
 float atscurve_table[THERMISTOR_LOOKUP_TABLE_SIZE+2]; //voltage limits are stored together with table
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
 float knock_zone[F_WRK_POINTS_L * F_WRK_POINTS_F]; //knock zones vs rpm & tps
 float grts_curve[THERMISTOR_LOOKUP_TABLE_SIZE+2];
 float grheat_duty[F_TMP_POINTS];
 float pwmiac_ucoef[PWMIAC_UCOEF_SIZE];
 float aftstr_strk0[AFTSTR_STRK_SIZE];
 float aftstr_strk1[AFTSTR_STRK_SIZE];
 float grv_delay[F_TMP_POINTS];
 float ftls_curve[FTLS_LOOKUP_TABLE_SIZE+2];
 float egts_curve[EGTS_LOOKUP_TABLE_SIZE+2];
 float ops_curve[OPS_LOOKUP_TABLE_SIZE+2];
 float injpw_coef[INJPWCOEF_LUT_SIZE];
 float maf_curve[MAF_FLOW_CURVE_SIZE+2+1];
 float ftls_corr[FTLSCOR_UCOEF_SIZE];
 float lambda_zone[F_WRK_POINTS_L * F_WRK_POINTS_F]; //lambda zones vs rpm & load
 float fts_curve[FTS_LOOKUP_TABLE_SIZE+2]; //FTS curve
 float fueldens_corr[FUELDENS_CORR_SIZE]; //Fuel density correction
 float xtau_xfacc[XTAU_FACT_SIZE];
 float xtau_xfdec[XTAU_FACT_SIZE];
 float xtau_tfacc[XTAU_FACT_SIZE];
 float xtau_tfdec[XTAU_FACT_SIZE];
 float inj_nonlinp_corr[INJ_NONLIN_SIZE * 2];
 float inj_nonling_corr[INJ_NONLIN_SIZE * 2];
 float inj_ego_delay[EGO_DELAY_SIZE];
 float inj_wu_afr0[WU_AFR_SIZE];
 float inj_wu_afr1[WU_AFR_SIZE];
 float etc_sprprel_duty[ETC_SPRPREL_SIZE * 2];
 float etc_accept_error[ETC_ACCEPTERR_SIZE * 2];
 float etc_throttle_pos[ETC_POS_APPS_SIZE * ETC_POS_RPM_SIZE];
 float ots_curve[OTS_LOOKUP_TABLE_SIZE+2]; //OTS curve

 CESettingsData cesd;

 //default constructor
 FWMapsDataHolder(size_t setNum = TABLES_NUMBER)
 {
  maps.resize(setNum);
  for(int i = ETMT_SEP_START; i <= ETMT_SEP_END; ++i)
   std::fill(GetMap(i), GetMap(i) + GetMapSize(i), .0f);
 }
 //get composed list of names
 std::vector<_TSTRING> GetListOfNames(void) const
 {
  std::vector<_TSTRING> list;
  list.reserve(16);
  for (size_t i = 0; i < maps.size(); i++)
   list.push_back(maps[i].name);
  return list;
 };

 float* GetMap(int i_mapType)
 { 
  switch(i_mapType)
  {
   case ETMT_ATTENUATOR: return attenuator_table;
   case ETMT_DWELLCNTRL: return dwellcntrl_table;
   case ETMT_CTS_CURVE: return ctscurve_table;
   case ETMT_ATS_CURVE: return atscurve_table;
   case ETMT_ATS_CORR: return ats_corr_table;
   case ETMT_GASDOSE: return gasdose_pos_table;
   case ETMT_BAROCORR: return barocorr_table;
   case ETMT_MANIGNTIM: return pa4_igntim_corr;
   case ETMT_TMP2_CURVE: return tmp2_curve;
   case ETMT_CRKCLT_CORR: return ctscrk_corr;
   case ETMT_EH_PAUSE: return eh_pause_table;
   case ETMT_CRANKING_THRD: return cranking_thrd;
   case ETMT_CRANKING_TIME: return cranking_time;
   case ETMT_SMAPABAN_THRD: return smapaban_thrd;
   case ETMT_KNOCK_ZONE: return knock_zone;
   case ETMT_GRTS_CURVE: return grts_curve;
   case ETMT_GRHEAT_DUTY: return grheat_duty;
   case ETMT_PWMIAC_UCOEF: return pwmiac_ucoef;
   case ETMT_AFTSTR_STRK0: return aftstr_strk0;
   case ETMT_AFTSTR_STRK1: return aftstr_strk1;
   case ETMT_GRVDELAY: return grv_delay;
   case ETMT_FTLS_CURVE: return ftls_curve;
   case ETMT_EGTS_CURVE: return egts_curve;
   case ETMT_OPS_CURVE: return ops_curve;
   case ETMT_MANINJPWC: return injpw_coef;
   case ETMT_MAF_CURVE: return maf_curve;
   case ETMT_FTLSCOR: return ftls_corr;
   case ETMT_LAMBDA_ZONE: return lambda_zone;
   case ETMT_FTS_CURVE: return fts_curve;
   case ETMT_FUELDENS_CORR: return fueldens_corr;
   case ETMT_XTAU_XFACC: return xtau_xfacc;
   case ETMT_XTAU_XFDEC: return xtau_xfdec;
   case ETMT_XTAU_TFACC: return xtau_tfacc;
   case ETMT_XTAU_TFDEC: return xtau_tfdec;
   case ETMT_INJNONLINP: return inj_nonlinp_corr;
   case ETMT_INJNONLING: return inj_nonling_corr;
   case ETMT_EGO_DELAY: return inj_ego_delay;
   case ETMT_WU_AFR0: return inj_wu_afr0;
   case ETMT_WU_AFR1: return inj_wu_afr1;
   case ETMT_ETC_SPRPREL: return etc_sprprel_duty;
   case ETMT_ETC_ACCEERR: return etc_accept_error;
   case ETMT_ETC_THROPOS: return etc_throttle_pos;
   case ETMT_OTS_CURVE: return ots_curve;
  }
  return NULL; //undefined type of map
 }

 static size_t GetMapSize(int i_mapType)
 {
  switch(i_mapType)
  {
   case ETMT_ATTENUATOR: return KC_ATTENUATOR_LOOKUP_TABLE_SIZE;
   case ETMT_DWELLCNTRL: return COIL_ON_TIME_LOOKUP_TABLE_SIZE;
   case ETMT_CTS_CURVE: return THERMISTOR_LOOKUP_TABLE_SIZE+2;
   case ETMT_ATS_CURVE: return THERMISTOR_LOOKUP_TABLE_SIZE+2;
   case ETMT_ATS_CORR: return ATS_CORR_LOOKUP_TABLE_SIZE;
   case ETMT_GASDOSE: return GASDOSE_POS_TPS_SIZE * GASDOSE_POS_RPM_SIZE;
   case ETMT_BAROCORR: return BAROCORR_SIZE+2;
   case ETMT_MANIGNTIM: return PA4_LOOKUP_TABLE_SIZE;
   case ETMT_TMP2_CURVE: return THERMISTOR_LOOKUP_TABLE_SIZE+2;
   case ETMT_CRKCLT_CORR: return CTS_CRKCORR_SIZE;
   case ETMT_EH_PAUSE: return COIL_ON_TIME_LOOKUP_TABLE_SIZE;
   case ETMT_CRANKING_THRD: return CRANK_THRD_SIZE;
   case ETMT_CRANKING_TIME: return CRANK_TIME_SIZE;
   case ETMT_SMAPABAN_THRD: return SMAPABAN_THRD_SIZE;
   case ETMT_KNOCK_ZONE: return F_WRK_POINTS_L * F_WRK_POINTS_F;
   case ETMT_GRTS_CURVE: return THERMISTOR_LOOKUP_TABLE_SIZE+2;
   case ETMT_GRHEAT_DUTY: return F_TMP_POINTS;
   case ETMT_PWMIAC_UCOEF: return PWMIAC_UCOEF_SIZE;
   case ETMT_AFTSTR_STRK0: return AFTSTR_STRK_SIZE;
   case ETMT_AFTSTR_STRK1: return AFTSTR_STRK_SIZE;
   case ETMT_GRVDELAY: return F_TMP_POINTS;
   case ETMT_FTLS_CURVE: return FTLS_LOOKUP_TABLE_SIZE+2;
   case ETMT_EGTS_CURVE: return EGTS_LOOKUP_TABLE_SIZE+2;
   case ETMT_OPS_CURVE: return OPS_LOOKUP_TABLE_SIZE+2;
   case ETMT_MANINJPWC: return INJPWCOEF_LUT_SIZE;
   case ETMT_MAF_CURVE: return MAF_FLOW_CURVE_SIZE+2+1;
   case ETMT_FTLSCOR: return FTLSCOR_UCOEF_SIZE;
   case ETMT_LAMBDA_ZONE: return F_WRK_POINTS_L * F_WRK_POINTS_F;
   case ETMT_FTS_CURVE: return FTS_LOOKUP_TABLE_SIZE+2;
   case ETMT_FUELDENS_CORR: return FUELDENS_CORR_SIZE;
   case ETMT_XTAU_XFACC: return XTAU_FACT_SIZE;
   case ETMT_XTAU_XFDEC: return XTAU_FACT_SIZE;
   case ETMT_XTAU_TFACC: return XTAU_FACT_SIZE;
   case ETMT_XTAU_TFDEC: return XTAU_FACT_SIZE;
   case ETMT_INJNONLINP: return INJ_NONLIN_SIZE * 2;
   case ETMT_INJNONLING: return INJ_NONLIN_SIZE * 2;
   case ETMT_EGO_DELAY: return EGO_DELAY_SIZE;
   case ETMT_WU_AFR0: return WU_AFR_SIZE;
   case ETMT_WU_AFR1: return WU_AFR_SIZE;
   case ETMT_ETC_SPRPREL: return ETC_SPRPREL_SIZE * 2;
   case ETMT_ETC_ACCEERR: return ETC_ACCEPTERR_SIZE * 2;
   case ETMT_ETC_THROPOS: return ETC_POS_APPS_SIZE * ETC_POS_RPM_SIZE;
   case ETMT_OTS_CURVE: return OTS_LOOKUP_TABLE_SIZE+2;
  }
  ASSERT(0);
  return 0; //undefined type of map
 }
};
