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

/** \file SECU3IO.h
 * \author Alexey A. Shabelnikov
 */

//Представление данных передаваемых между SECU и менеджером. Структуры данных представлены без привязки
//к конкретной платформе - они абстрагированы
#pragma once

#include <utility>

namespace SECU3IO
{
 //описывает все входы системы - их производные и интегральные величины
 struct SensorDat
 {
  int frequen;                          //частота вращения коленвала (усредненная)
  float pressure;                       //давление во впускном коллекторе (усредненное)
  float voltage;                        //напряжение бортовой сети (усредненное)
  float temperat;                       //температура охлаждающей жидкости (усредненная)
  float adv_angle;                      //текущий УОЗ
  unsigned char carb;                   //состояние концевика карбюратора
  unsigned char gas;                    //состояние газового клапана
  unsigned char air_flow;               //расход воздуха
  unsigned char ephh_valve;             //состояние клапана ЭПХХ
  unsigned char epm_valve;              //состояние клапана ЭМР
  unsigned char cool_fan;               //состояние вентилятора (включен/выключен)
  unsigned char st_block;               //состояние выхода блокировки стартера
  unsigned char acceleration;           //acceleration/deceleration mode
  float knock_k;                        //уровень сигнала детонации (усредненный за время фазового окна)
  float knock_retard;                   //корректировка УОЗ при детонации
  unsigned char ce_state;               // !currently is not used!
  WORD ce_errors;                       //биты ошибок СЕ
  float tps;                            //ДПДЗ (TPS sensor value)
  float add_i1;                         //ADD_I1 voltage
  float add_i2;                         //ADD_I2 voltage
  float choke_pos;                      //choke position (%)
  float gasdose_pos;                    //gas dosator position
  float speed;                          //vehicle speed (Km/h)
  float distance;                       //passed distance since last ignition turn on (meters)
  float inj_fff;                        //value of instant fuel consumption, Hz (16000 pulses per 1L of burnt fuel)
  float inj_ffd;                        //value of instant fuel consumption, L/100km (habitual format)                 
  float air_temp;                       //intake air temperature
  bool  add_i2_mode;                    //0 - show voltage, 1 - show IAT

  bool  knkret_use;
  float strt_aalt;                      //advance angle from start map
  bool  strt_use;
  float idle_aalt;                      //advance angle from idle map
  bool  idle_use;
  float work_aalt;                      //advance angle from work map
  bool  work_use;
  float temp_aalt;                      //advance angle from coolant temperature correction map
  bool  temp_use;
  float airt_aalt;                      //advance angle from air temperature correction map
  bool  airt_use;
  float idlreg_aac;                     //advance angle correction from idling RPM regulator
  bool idlreg_use;
  float octan_aac;                      //octane correction value
  bool octan_use;
  float lambda_corr;                    //lambda correction in %

  float inj_pw;                         //injector pulse width in ms

  int tpsdot;                           //TPS dot (d%/dt)

  float map2;                           // MAP2 (gas pressure)
  float tmp2;                           // TMP2 (gas temperature)
  float mapd;                           // Diffrerential pressure = (MAP2 - MAP)
 };

 struct DbgvarDat
 {
  int var1;                             //переменная 1
  int var2;                             //переменная 2
  int var3;                             //переменная 3
  int var4;                             //переменная 4
 };

 struct FnNameDat
 {
  unsigned char tables_num;
  unsigned char index;
  char   name[64];
 };

 struct StartrPar
 {
  int  starter_off;                     //порог выключения стартера (обороты)
  int  smap_abandon;                    //обороты перехода с пусковой карты на рабочую
  float inj_cranktorun_time;            //sec.
  int   inj_aftstr_strokes;             //strokes
  float inj_prime_cold;                 //prime pulse width at -30C
  float inj_prime_hot;                  //prime pulse width at 70C
  float inj_prime_delay;                //prime pulse delay
  float inj_floodclear_tps;             //flood clear mode threshold
 };

 struct AnglesPar
 {
  float  max_angle;
  float  min_angle;
  float  angle_corr;
  float  dec_speed;
  float  inc_speed;
  unsigned char zero_adv_ang;
 };


 struct FunSetPar
 {
  unsigned char fn_benzin;
  unsigned char fn_gas;
  float map_lower_pressure;
  float map_upper_pressure;
  float map_curve_offset;
  float map_curve_gradient;
  float map2_curve_offset;
  float map2_curve_gradient;
  float tps_curve_offset;
  float tps_curve_gradient;
  unsigned char load_src_cfg;
  int uni_benzin;
  int uni_gas;
  int barocorr_type;
 };

 struct IdlRegPar
 {
  bool idl_regul;                       //use idling regulator
  bool use_regongas;                    //use regulator when fuel type is gas
  bool preg_mode;                       //proportional regulator mode
  float  ifac1;
  float  ifac2;
  int  MINEFR;
  int  idling_rpm;
  float min_angle;
  float max_angle;
  float turn_on_temp;                   //idling regulator turn on temperature

  bool closed_loop;                     //use closed loop mode (fuel injection only)
  float idl_to_run_add;                 //Value (in %) added to IAC position when exiting from closed loop
  int rpm_on_run_add;                   //Value added to target RPM when vehicle starts to run
  float idl_reg_p;                      //IAC closeed loop proportional coefficient
  float idl_reg_i;                      //IAC closed loop integral coefficient
  float idl_coef_thrd1;                 //coefficient for calculating closed loop entering RPM threshold
  float idl_coef_thrd2;                 //coefficient for calculating closed loop leaving RPM threshold
  int  idl_intrpm_lim;                  //RPM error limit for integrator
  float idl_map_value;                  //intake manifold pressure on idling
  float idl_iacminpos;
  float idl_iacmaxpos;
  bool  idl_useiacclongas;
 };

 struct CarburPar
 {
  int  ephh_lot;
  int  ephh_hit;
  unsigned char carb_invers;
  float epm_ont;                        //порог включения ЭМР
  int  ephh_lot_g;
  int  ephh_hit_g;
  float shutoff_delay;                  //задержка выключения клапана
  float tps_threshold;                  //порог переключения в режим ХХ по ДПДЗ
  float fuelcut_map_thrd;               //fuel cut MAP threshold 
  float fuelcut_cts_thrd;               //fuel cut CTS threshold
  int  revlim_lot;                      //rev.limitting lo threshold
  int  revlim_hit;                      //rev.limitting hi threshold
 };

 struct TemperPar
 {
  bool tmp_use;
  bool vent_pwm;
  bool cts_use_map;
  float  vent_on;
  float  vent_off;
  int vent_pwmfrq;
  float cond_pvt_on;
  float cond_pvt_off;
  int cond_min_rpm;
  float vent_tmr;
 };

 struct ADCCompenPar
 {
  float  map_adc_factor;                //коэффициент передаточной погрешности
  float  map_adc_correction;            //сдвиг в вольтах
  float  ubat_adc_factor;
  float  ubat_adc_correction;
  float  temp_adc_factor;
  float  temp_adc_correction;
  float  tps_adc_factor;
  float  tps_adc_correction;
  float  ai1_adc_factor;
  float  ai1_adc_correction;
  float  ai2_adc_factor;
  float  ai2_adc_correction;
  float  ai3_adc_factor;
  float  ai3_adc_correction;
  float  ai4_adc_factor;
  float  ai4_adc_correction;
 };

 struct RawSensDat
 {
  float map_value;
  float ubat_value;
  float temp_value;
  float knock_value;
  float tps_value;
  float add_i1_value;
  float add_i2_value;
  float add_i3_value;   //secu-3i only
  float add_i4_value;   //secu-3i only and TPIC8101 option used
 };

 struct CKPSPar
 {
  bool ckps_edge_type;
  bool ref_s_edge_type;
  unsigned char ckps_cogs_btdc;
  unsigned char ckps_ignit_cogs;
  unsigned char ckps_engine_cyl;
  bool ckps_merge_ign_outs;
  unsigned char ckps_cogs_num;
  unsigned char ckps_miss_num;
  float hall_wnd_width; 
  bool ckps_rising_spark;
  float hall_degrees_btdc;
  bool ckps_use_cam_ref;
 };

 struct OPCompNc                        //используется если надо просто принять или послать определенный код действия
 {
  BYTE opcode;   //operation code
  BYTE opdata;   //operation data
 };

 const int OPCODE_EEPROM_PARAM_SAVE    = 1;
 const int OPCODE_CE_SAVE_ERRORS       = 2;
 const int OPCODE_READ_FW_SIG_INFO     = 3;
 const int OPCODE_LOAD_TABLSET         = 4;  //realtime tables
 const int OPCODE_SAVE_TABLSET         = 5;  //realtime tables
 const int OPCODE_DIAGNOST_ENTER       = 6;  //enter diagnostic mode
 const int OPCODE_DIAGNOST_LEAVE       = 7;  //leave diagnostic mode
 const int OPCODE_RESET_EEPROM         = 0xCF;//reset EEPROM
 const int OPCODE_BL_CONFIRM           = 0xCB; //boot loader entering confirmation

 struct KnockPar
 {
  unsigned char knock_use_knock_channel;
  unsigned char knock_bpf_frequency;
  float knock_k_wnd_begin_angle;
  float knock_k_wnd_end_angle;
  unsigned char knock_int_time_const;

  float knock_retard_step;              //шаг смещения УОЗ при детонации
  float knock_advance_step;             //шаг восстановления УОЗ
  float knock_max_retard;               //максимальное смещение УОЗ
  float knock_threshold;                //in Volts
  int knock_recovery_delay;             //in engine strokes
 };

 struct CEErrors
 {
  DWORD flags;
 };

 const int FW_SIGNATURE_INFO_SIZE = 48;
 struct FWInfoDat
 {
  char   info[FW_SIGNATURE_INFO_SIZE+1];//*one more byte for string's termination
  DWORD  options;
  BYTE   fw_version[2];                 //index: 0 - minor, 1 - major
 };

 struct MiscelPar
 {
  int baud_rate;                        //UART baud rate
  int period_ms;                        //период посылки пакетов мс.
  unsigned char ign_cutoff;             //признак использования отсечки зажигания
  int ign_cutoff_thrd;                  //обороты отсечки зажигания
  int hop_start_cogs;                   //Выход ДХ: Начало испульса в зубьях шкива относ. ВМТ 
  int hop_durat_cogs;                   //Выход ДХ: Длительность импульса в зубьях шкива
  bool flpmp_offongas;                  //Flag. Turn off or not fuel pump when fuel type is gas
  bool inj_offongas;                    //Turn off injector(s) on gas
  float evap_afbegin;
  float evap_afslope;
 };

 struct EditTabPar
 {
  unsigned char tab_id;                 //идентификатор таблицы(данных) в наборе
  unsigned int address;                 //адрес начала фрагмента данных в таблице
  float table_data[32];                 //фрагмент данных (не более 16-ти значений)
  TCHAR name_data[32];                  //содержит текстовую информацию
  unsigned int data_size;               //размер фрагмента данных
  float beginPadding;
  float endPadding;
 };

 const int ETMT_NAME_STR = 0;           //name of tables's set
 const int ETMT_STRT_MAP = 1;           //start map
 const int ETMT_IDLE_MAP = 2;           //idle map
 const int ETMT_WORK_MAP = 3;           //work map
 const int ETMT_TEMP_MAP = 4;           //temp.corr. map

 const int ETMT_VE_MAP   = 5;           //VE
 const int ETMT_AFR_MAP  = 6;           //AFR
 const int ETMT_CRNK_MAP = 7;           //Cranking PW
 const int ETMT_WRMP_MAP = 8;           //Warmup enrichment
 const int ETMT_DEAD_MAP = 9;           //Injector dead time
 const int ETMT_IDLR_MAP = 10;          //IAC run pos
 const int ETMT_IDLC_MAP = 11;          //IAC cranking pos
 const int ETMT_AETPS_MAP = 12;         //AE TPS map
 const int ETMT_AERPM_MAP = 13;         //AE RPM map
 const int ETMT_AFTSTR_MAP = 14;        //afterstart enrichment
 const int ETMT_IT_MAP   = 15;          //injection timing
 const int ETMT_ITRPM_MAP = 16;         //Idling RPM
 const int ETMT_RIGID_MAP = 17;         //Idl. regulator's rigidity
 const int ETMT_EGOCRV_MAP = 18;        //EGO curve (WBO sensor emulation)
 const int ETMT_IACC_MAP = 19;          //mixture correction vs IAC pos.
 const int ETMT_IACCW_MAP = 20;         //weight of mixture correction vs IAC pos.
 const int ETMT_IATCLT_MAP = 21;        //IAT/CLT correction vs air flow
 const int ETMT_TPSSWT_MAP = 22;        //MAP/TPS switch point vs RPM
 const int ETMT_GTSC_MAP = 23;          //Inj. PW correction coefficient vs gas temperature
 const int ETMT_GPSC_MAP = 24;          //Inj. PW correction coefficient vs gas pressure
 const int ETMT_ATSC_MAP = 25;          //Inj. PW correction coefficient vs air temperature

 struct SepTabPar
 {
  unsigned int address;                 //адрес начала фрагмента данных в таблице
  float table_data[32];                 //фрагмент данных (не более 16-ти байт)
  unsigned int data_size;               //размер фрагмента данных
 };

 struct DiagInpDat
 {
  float voltage;                        //board voltage
  float map;                            //MAP sensor
  float temp;                           //coolant temperature
  float add_i1;                         //additional input 1 (analog)
  float add_i2;                         //additional input 2 (analog)
  float add_i3;                         //additional input 3 (analog)
  float add_i4;                         //additional input 4 (analog, available only in firwares with TPIC8101 support)
  float carb;                           //carburetor switch, throttle position sensor (analog)
  int gas;                              //gas valve state (digital)
  int ckps;                             //CKP sensor (digital)
  int ref_s;                            //VR type cam sensor (digital)
  int ps;                               //Hall-effect cam sensor (digital)
  int bl;                               //"Bootloader" jumper
  int de;                               //"Default EEPROM" jumper
  float ks_1;                           //knock sensor 1
  float ks_2;                           //knock sensor 2
  int ign_i;                            //IGN_I 
  int cond_i;                           //COND_I
  int epas_i;                           //EPAS_I
 };

 struct DiagOutDat
 {
  bool mode;                            //false - SECU-3T, true - SECU-3i
  int  out[32];                         //states of outputs
  //Note: BL,DE are 3-state outputs (0 - hiZ, 1 - "0", 2 - "1")
  //SECU-3T (13 values):
  // IGN_OUT1, IGN_OUT2, IGN_OUT3, IGN_OUT4, IE, FE, ECF, CE, ST_BLOCK, ADD_O1, ADD_O2, BL, DE
  //SECU-3i (21 values):
  // IGN_O1, IGN_O2, IGN_O3, IGN_O4, IGN_O5, ECF, INJ_O1, INJ_O2, INJ_O3, INJ_O4, INJ_O5, BL, DE, STBL_O, CEL_O, FPMP_O, PWRR_O, EVAP_O, O2SH_O, COND_O, ADD_O2
 };

 struct ChokePar
 {
  int sm_steps;                         //number of stepper motor steps
  unsigned char testing;                //not a parameter, (flag) indicates that system is in choke testng mode
  signed char manual_delta;             //delta position value for manual choke control
  float choke_rpm_if;                   //Integral choke RPM regulator factor
  float choke_corr_time[2];             //Startup corretion time
  bool  useclrpmreg;                    //Use closed loop RPM regulator (I-regulator)
  bool  offrpmreg_ongas;                //Turn off RPM regulator when fuel type is gas
  bool  usethrottle_pos;                //Use throttle position in choke initialization
  int   sm_freq;                        //Frequency of stepper motor's pulses (allowed values: 0,1,2,3)
  bool  sm_maxfreqinit;                 //Use maximum STEP frequency at initialization of SM
  float inj_cranktorun_time;            //sec.
 };

 struct GasdosePar
 {
  int gd_steps;                         //number of gas dose stepper motor steps
  unsigned char testing;                //not a parameter, (flag) indicates that system is in GD testng mode
  signed char manual_delta;             //delta position value for manual GD control
  float fc_closing;                     //How much close gas doser in fuel cut mode (%)
  float lam_corr_limit_p;               //"+" correction limit
  float lam_corr_limit_m;               //"-" correction limit
  float lam_stoichval;                  //stoichiometry value for LPG
  int   gd_freq;                        //Frequency of stepper motor's pulses (allowed values: 0,1,2,3)
  bool  gd_maxfreqinit;                 //Use maximum STEP frequency at initialization of SM
 };

 const int IBTN_KEYS_NUM = 2;           //Number of iButton keys
 const int IBTN_KEY_SIZE = 6;           //Size of iButton key (except CRC8 and family code)

 struct SecurPar
 {
  TCHAR bt_name[10];                    //bluetooth name, this parameter is not stored in EEPROM
  TCHAR bt_pass[10];                    //bluetooth password, this parameter is not stored in EEPROM
  bool use_bt;                          //flag specifying to use or not to use bluetooth
  bool set_btbr;                        //flag indicating that bluetooth baud rate has to be set
  bool use_imm;                         //flag specifying to use or not to use immobilizer
  bool use_respar;                      //use reserve parameters (from FLASH only)
  bool chk_fwcrc;                       //check integrity of firmware
  bool bt_type;                         //Bluetooth type: 0 - BC417, 1 - BK3231
  BYTE ibtn_keys[IBTN_KEYS_NUM][IBTN_KEY_SIZE];//iButton keys
 };

 //Describes a universal programmable output
 struct uni_output_par
 {
  unsigned char logicFunc;              //logic function between two conditions
  bool invers_1;                        //inversion flags for condition 1
  bool invers_2;                        //inversion flags for condition 2
  unsigned char condition1;             //code of condition 1
  unsigned char condition2;             //code of condition 2
  float on_thrd_1;                      //ON threshold (if value > on_thrd_1)
  float off_thrd_1;                     //OFF threshold (if value < off_thrd_1)
  float on_thrd_2;                      //same as on_thrd_1
  float off_thrd_2;                     //same as off_thrd_1
 };

 const int UNI_OUTPUT_NUM = 3;

 struct UniOutPar
 {
  uni_output_par out[UNI_OUTPUT_NUM];
  unsigned char logicFunc12;            //logic function between 1st and 2nd outputs
 };

 //logic functions
 const int UNIOUT_LF_OR = 0;
 const int UNIOUT_LF_AND = 1;
 const int UNIOUT_LF_XOR = 2;
 const int UNIOUT_LF_2ND = 3;
 const int UNIOUT_LF_NONE = 15;
 //conditions
 const int UNIOUT_COND_CTS = 0;       //coolant temperature
 const int UNIOUT_COND_RPM = 1;       //RPM
 const int UNIOUT_COND_MAP = 2;       //MAP
 const int UNIOUT_COND_UBAT = 3;      //board voltage
 const int UNIOUT_COND_CARB = 4;      //throttle position limit switch
 const int UNIOUT_COND_VSPD = 5;      //vehicle speed
 const int UNIOUT_COND_AIRFL = 6;     //air flow
 const int UNIOUT_COND_TMR = 7;       //timer, allowed only for 2nd condition
 const int UNIOUT_COND_ITTMR = 8;     //timer, triggered after turning on of ignition
 const int UNIOUT_COND_ESTMR = 9;     //timer, triggered after starting of engine
 const int UNIOUT_COND_CPOS = 10;     //choke position
 const int UNIOUT_COND_AANG = 11;     //advance angle
 const int UNIOUT_COND_KLEV = 12;     //knock signal level
 const int UNIOUT_COND_TPS = 13;      //throttle position sensor
 const int UNIOUT_COND_ATS = 14;      //intake air temperature sensor
 const int UNIOUT_COND_AI1 = 15;      //analog input 1
 const int UNIOUT_COND_AI2 = 16;      //analog input 2
 const int UNIOUT_COND_GASV = 17;     //gas valve input
 const int UNIOUT_COND_IPW = 18;      //injector pulse width
 const int UNIOUT_COND_CE = 19;       //CE state
 const int UNIOUT_COND_OFTMR = 20;    //On/Off delay timer

 //define allowed fuel injection configurations
 const int INJCFG_THROTTLEBODY  = 0;  //single injector for N cylinders
 const int INJCFG_SIMULTANEOUS = 1;   //N injectors, all injectors work simultaneously
 const int INJCFG_2BANK_ALTERN = 2;   //N injectors split into 2 banks, banks work alternately
 const int INJCFG_SEMISEQUENTIAL = 3; //N injectors, injectors work in pairs
 const int INGCFG_FULLSEQUENTIAL = 4; //N injectors, each injector works 1 time per cycle
 const int INJCFG_SEMISEQSEPAR  = 5;  //N injectors, injectors work in pairs, each injector has its own separate output

 const int INJANGLESPEC_BEGIN = 0;
 const int INJANGLESPEC_MIDDLE = 1;
 const int INJANGLESPEC_END = 2;

 struct InjctrPar
 {
  bool inj_usetimingmap[2];           //Use timing map instead of constant parameter
  bool inj_useaddcorrs;               //Use additional corrections (gas.eq.)
  bool inj_useairden;                 //Use air density correction map
  bool inj_usediffpress;              //Use diff.pressure for PW correction from GPS
  int inj_config[2];                  //Injection configuration (throttle-body, simultaneous, semi-sequential, fullsequential)
  int inj_squirt_num[2];              //Number of squirts per cycle
  float inj_flow_rate[2];             //Injector flow rate
  float inj_cyl_disp;                 //The displacement of one cylinder in liters
  float inj_sd_igl_const[2];          //precalculated constant
  int cyl_num;                        //read-only parameter: number of cylinders
  float inj_timing[2];                //injection timing
  float inj_timing_crk[2];            //injection timing on cranking
  int inj_anglespec[2];               //Specifies how inj.timing coupled with inj.pulse (beginning, middle, end)
  float fff_const;                    //fuel consumption const.
 };

 struct LambdaPar
 {
  unsigned char lam_senstype;         //type of EGO sensor (NBO, WBO) 
  int lam_str_per_stp;                //Number of strokes per step for lambda control
  int lam_ms_per_stp;                 //Number of ms per step for lambda control
  float lam_step_size_p;              //"+" Size of step 
  float lam_step_size_m;              //"-" Size of step 
  float lam_corr_limit_p;             //"+" correction limit
  float lam_corr_limit_m;             //"-" correction limit
  float lam_swt_point;                //switch point in Volts
  float lam_temp_thrd;                //Temperature turn on threshold
  int lam_rpm_thrd;                   //RPM turn on threshold
  int lam_activ_delay;                //Afterstart activation delay
  float lam_dead_band;                //Switch point dead band (in volts)
  bool lam_htgdet;                    //Determine heating by voltage
  float lam_2stoichval;               //Stoichiometric value for 2-nd fuel
 };

 struct AccelPar
 {
  float ae_tpsdot_thrd;               //TPS %/sec threshold
  float ae_coldacc_mult;              //Cold acceleration multiplier
  int ae_decay_time;                  //Decay time in strokes
 };

 //таблица перекодировки кода частоты ПФ в частоту
 const int GAIN_FREQUENCES_SIZE = 64;
 static float hip9011_gain_frequences[GAIN_FREQUENCES_SIZE] =
 {
  01.22f,01.26f,01.31f,01.35f,01.40f,01.45f,01.51f,01.57f,
  01.63f,01.71f,01.78f,01.87f,01.96f,02.07f,02.18f,02.31f,
  02.46f,02.54f,02.62f,02.71f,02.81f,02.92f,03.03f,03.15f,
  03.28f,03.43f,03.59f,03.76f,03.95f,04.16f,04.39f,04.66f,
  04.95f,05.12f,05.29f,05.48f,05.68f,05.90f,06.12f,06.37f,
  06.64f,06.94f,07.27f,07.63f,08.02f,08.46f,08.95f,09.50f,
  10.12f,10.46f,10.83f,11.22f,11.65f,12.10f,12.60f,13.14f,
  13.72f,14.36f,15.07f,15.84f,16.71f,17.67f,18.76f,19.98f
 };

 //таблица перекодировки кода коэфф. усиления аттенюатора в коэфф. усиления
 const int ATTENUATOR_LEVELS_SIZE = 64;
 static float hip9011_attenuator_gains[ATTENUATOR_LEVELS_SIZE] =
 {
  2.000f, 1.882f, 1.778f, 1.684f, 1.600f, 1.523f, 1.455f, 1.391f,
  1.333f, 1.280f, 1.231f, 1.185f, 1.143f, 1.063f, 1.000f, 0.944f,
  0.895f, 0.850f, 0.810f, 0.773f, 0.739f, 0.708f, 0.680f, 0.654f,
  0.630f, 0.607f, 0.586f, 0.567f, 0.548f, 0.500f, 0.471f, 0.444f,
  0.421f, 0.400f, 0.381f, 0.364f, 0.348f, 0.333f, 0.320f, 0.308f,
  0.296f, 0.286f, 0.276f, 0.267f, 0.258f, 0.250f, 0.236f, 0.222f,
  0.211f, 0.200f, 0.190f, 0.182f, 0.174f, 0.167f, 0.160f, 0.154f,
  0.148f, 0.143f, 0.138f, 0.133f, 0.129f, 0.125f, 0.118f, 0.111f
 };

 //таблица перекодировки кода постоянной времени интегрирования в постоянную времени
 //интегрирования
 const int INTEGRATOR_LEVELS_SIZE = 32;
 static float hip9011_integrator_const[INTEGRATOR_LEVELS_SIZE] =
 {
  40,  45,  50,  55,  60,  65,  70,   75,
  80,  90,  100, 110, 120, 130, 140, 150,
  160, 180, 200, 220, 240, 260, 280, 300,
  320, 360, 400, 440, 480, 520, 560, 600
 };

 //Это маленькая база данных хранящая скорости UART-a SECU-3 и соответствующие им делители.
 const int SECU3_ALLOWABLE_UART_DIVISORS_COUNT = 9;
 //<speed, divisor>
#define _SD std::pair<int, int>
 static std::pair<int, int> secu3_allowable_uart_divisors[SECU3_ALLOWABLE_UART_DIVISORS_COUNT] =
 {
  _SD(2400,  0x0411),
  _SD(4800,  0x0208),
  _SD(9600,  0x0103),
  _SD(14400, 0x00AD),
  _SD(19200, 0x0081),
  _SD(28800, 0x0056),
  _SD(38400, 0x0040),
  _SD(57600, 0x002A),
  _SD(115200,0x0015)
 };
#undef _SD

 union SECU3Packet
 {
  SECU3IO::SensorDat    m_SensorDat;
  SECU3IO::FnNameDat    m_FnNameDat;
  SECU3IO::StartrPar    m_StartrPar;
  SECU3IO::AnglesPar    m_AnglesPar;
  SECU3IO::FunSetPar    m_FunSetPar;
  SECU3IO::IdlRegPar    m_IdlRegPar;
  SECU3IO::CarburPar    m_CarburPar;
  SECU3IO::TemperPar    m_TemperPar;
  SECU3IO::ADCCompenPar m_ADCCompenPar;
  SECU3IO::RawSensDat   m_RawSensDat;
  SECU3IO::CKPSPar      m_CKPSPar;
  SECU3IO::OPCompNc     m_OPCompNc;
  SECU3IO::KnockPar     m_KnockPar;
  SECU3IO::CEErrors     m_CEErrors;
  SECU3IO::FWInfoDat    m_FWInfoDat;
  SECU3IO::MiscelPar    m_MiscelPar;
  SECU3IO::EditTabPar   m_EditTabPar;
  SECU3IO::SepTabPar    m_SepTabPar;
  SECU3IO::DbgvarDat    m_DbgvarDat;
  SECU3IO::DiagInpDat   m_DiagInpDat;
  SECU3IO::DiagOutDat   m_DiagOutDat;
  SECU3IO::ChokePar     m_ChokePar;
  SECU3IO::GasdosePar   m_GasdosePar; //GD
  SECU3IO::SecurPar     m_SecurPar;
  SECU3IO::UniOutPar    m_UniOutPar;
  SECU3IO::InjctrPar    m_InjctrPar;
  SECU3IO::LambdaPar    m_LambdaPar;
  SECU3IO::AccelPar     m_AccelPar;
 };

 struct CESettingsData
 {  
  float map_v_min;
  float map_v_max;
  float map_v_em;

  float vbat_v_min;
  float vbat_v_max;
  float vbat_v_em;

  float cts_v_min;
  float cts_v_max;
  float cts_v_em;

  float ks_v_min;
  float ks_v_max;
  float ks_v_em;

  float tps_v_min;
  float tps_v_max;
  float tps_v_em;

  float add_i1_v_min;
  float add_i1_v_max;
  float add_i1_v_em;

  float add_i2_v_min;
  float add_i2_v_max;
  float add_i2_v_em;

  float add_i3_v_min;
  float add_i3_v_max;
  float add_i3_v_em;

  float add_i4_v_min;
  float add_i4_v_max;
  float add_i4_v_em;
 };

 const float start_map_rpm_slots[16] = {200,240,280,320,360,400,440,480,520,560,600,640,680,720,760,800};
 const float work_map_rpm_slots[16]  = {600,720,840,990,1170,1380,1650,1950,2310,2730,3210,3840,4530,5370,6360,7500};
 const float temp_map_tmp_slots[16]  = {-30,-20,-10,0,10,20,30,40,50,60,70,80,90,100,110,120};
 const float dwellcntrl_map_slots[32] = { 5.4f, 5.8f, 6.2f, 6.6f, 7.0f, 7.4f, 7.8f, 8.2f, 8.6f, 9.0f, 9.4f, 9.8f,10.2f,10.6f,11.0f,11.4f,
                                        11.8f,12.2f,12.6f,13.0f,13.4f,13.8f,14.2f,14.6f,15.0f,15.4f,15.8f,16.2f,16.6f,17.0f,17.4f,17.8f};
 const float rigid_map_slots[8]  = {1,2,3,4,5,6,7,8};
 const float manigntim_map_slots[16]  = {.0f,0.33f,0.66f,1.00f,1.33f,1.66f,2.00f,2.33f,2.66f,3.00f,3.33f,3.66f,4.00f,4.33f,4.66f,5.0f};

 const int SECU3_COMPILE_OPTIONS_BITS_COUNT = 32;
 //<bitnumber, name>
#define _SD std::pair<int, TCHAR*>
 static std::pair<int, TCHAR*> secu3_compile_options_bits[SECU3_COMPILE_OPTIONS_BITS_COUNT] =
 {
  _SD(0,  _T("COPT_OBD_SUPPORT")),
  _SD(1,  _T("COPT_ATMEGA1284")),
  _SD(2,  _T("COPT_ATMEGA64")),
  _SD(3,  _T("COPT_ATMEGA128")),
  _SD(4,  _T("COPT_VPSEM")),
  _SD(5,  _T("COPT_TPIC8101")),
  _SD(6,  _T("COPT_INVERSE_IGN_OUTPUTS")), /*Obsolete! Left for compatibility reasons*/
  _SD(7,  _T("COPT_DWELL_CONTROL")),
  _SD(8,  _T("COPT_COOLINGFAN_PWM")),
  _SD(9,  _T("COPT_REALTIME_TABLES")),
  _SD(10, _T("COPT_ICCAVR_COMPILER")),
  _SD(11, _T("COPT_AVRGCC_COMPILER")),
  _SD(12, _T("COPT_DEBUG_VARIABLES")),
  _SD(13, _T("COPT_PHASE_SENSOR")),
  _SD(14, _T("COPT_PHASED_IGNITION")),
  _SD(15, _T("COPT_FUEL_PUMP")),
  _SD(16, _T("COPT_THERMISTOR_CS")),
  _SD(17, _T("COPT_SECU3T")),
  _SD(18, _T("COPT_DIAGNOSTICS")),
  _SD(19, _T("COPT_HALL_OUTPUT")),
  _SD(20, _T("COPT_REV9_BOARD")),
  _SD(21, _T("COPT_STROBOSCOPE")),
  _SD(22, _T("COPT_SM_CONTROL")),
  _SD(23, _T("COPT_VREF_5V")),
  _SD(24, _T("COPT_HALL_SYNC")),
  _SD(25, _T("COPT_UART_BINARY")),
  _SD(26, _T("COPT_CKPS_2CHIGN")),
  _SD(27, _T("COPT_ATMEGA644")),
  _SD(28, _T("COPT_FUEL_INJECT")),
  _SD(29, _T("COPT_GD_CONTROL")),
  _SD(30, _T("COPT_CARB_AFR")),
  _SD(31, _T("COPT_CKPS_NPLUS1"))
 };
#undef _SD

 const int COPT_OBD_SUPPORT = 0;
 const int COPT_ATMEGA1284 = 1;
 const int COPT_ATMEGA64 = 2;
 const int COPT_ATMEGA128 = 3;
 const int COPT_VPSEM = 4;
 const int COPT_TPIC8101 = 5;
 const int COPT_INVERSE_IGN_OUTPUTS = 6; /*Obsolete! Left for compatibility reasons*/
 const int COPT_DWELL_CONTROL = 7;
 const int COPT_COOLINGFAN_PWM = 8;
 const int COPT_REALTIME_TABLES = 9;
 const int COPT_ICCAVR_COMPILER = 10;
 const int COPT_AVRGCC_COMPILER = 11;
 const int COPT_DEBUG_VARIABLES = 12;
 const int COPT_PHASE_SENSOR = 13;
 const int COPT_PHASED_IGNITION = 14;
 const int COPT_FUEL_PUMP = 15;
 const int COPT_THERMISTOR_CS = 16;
 const int COPT_SECU3T = 17;
 const int COPT_DIAGNOSTICS = 18;
 const int COPT_HALL_OUTPUT = 19;
 const int COPT_REV9_BOARD = 20;
 const int COPT_STROBOSCOPE = 21;
 const int COPT_SM_CONTROL = 22;
 const int COPT_VREF_5V = 23;
 const int COPT_HALL_SYNC = 24;
 const int COPT_UART_BINARY = 25;
 const int COPT_CKPS_2CHIGN = 26;
 const int COPT_ATMEGA644 = 27;
 const int COPT_FUEL_INJECT = 28;
 const int COPT_GD_CONTROL = 29;
 const int COPT_CARB_AFR = 30;
 const int COPT_CKPS_NPLUS1 = 31;
};
