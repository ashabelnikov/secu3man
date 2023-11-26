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

//Representation of data being sent between SECU-3 unit and SECU-3 Manager
#pragma once

#include <utility>

namespace SECU3IO
{
 const int UNI_OUTPUT_NUM = 6;

 //описывает все входы системы - их производные и интегральные величины
 struct SensorDat
 {
  int frequen;                          //частота вращения коленвала (усредненная)
  float pressure;                       //давление во впускном коллекторе (усредненное)
  float voltage;                        //напряжение бортовой сети (усредненное)
  float temperat;                       //температура охлаждающей жидкости (усредненная)
  float adv_angle;                      //текущий УОЗ
  unsigned char air_flow;               //расход воздуха

  bool carb;                            //состояние концевика карбюратора
  bool gas;                             //состояние газового клапана
  bool ephh_valve;                      //состояние клапана ЭПХХ
  bool epm_valve;                       //состояние клапана ЭМР
  bool cool_fan;                        //состояние вентилятора (включен/выключен)
  bool st_block;                        //состояние выхода блокировки стартера
  bool acceleration;                    //acceleration/deceleration mode
  bool ce_state;                        // state of CE
  bool fc_revlim;                       // fuel cut rev.lim. flag
  bool floodclear;                      // flood clear mode flag   
  bool sys_locked;                      // system locked flag (immobilizer)
  bool ign_i;                           //IGN_I
  bool cond_i;                          //COND_I
  bool epas_i;                          //EPAS_I

  float knock_k;                        //уровень сигнала детонации (усредненный за время фазового окна)
  float knock_retard;                   //корректировка УОЗ при детонации
  DWORD ce_errors;                      //биты ошибок СЕ
  float tps;                            //ДПДЗ (TPS sensor value)
  float add_i1;                         //ADD_I1 voltage
  float add_i2;                         //ADD_I2 voltage
  float choke_pos;                      //choke position (%)
  float gasdose_pos;                    //gas dosator position
  float speed;                          //vehicle speed (Km/h)
  float distance;                       //passed distance since last ignition turn on (meters)
  float inj_fff;                        //value of instant fuel consumption, Hz (16000 pulses per 1L of burnt fuel)
  float inj_ffd;                        //value of instant fuel consumption, L/100km (habitual format)
  float inj_ffh;                        //value of instant fuel consumption, L/h 
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
  float lambda_corr2;                   //lambda correction in %

  float inj_pw;                         //injector pulse width in ms

  int tpsdot;                           //TPS dot (d%/dt)
  int mapdot;                           //MAP dot (dP%/dt)

  float map2;                           // MAP2 (gas pressure)
  float tmp2;                           // TMP2 (gas temperature)
  float mapd;                           // Diffrerential pressure = (MAP2 - MAP)
  float grts;                           // Gas reducer's temperature sensor

  float afr;                            // AFR calculated from lambda sensor
  float afr2;                           // AFR calculated from lambda sensor
  float afrmap;                         // AFR value from map

  float load;                           // Load value (MAP or TPS or synthetic MAP+TPS)
  float baro_press;                     // Baro pressure

  float inj_tim_begin;                  //inj.timing of the beginning of pulse
  float inj_tim_end;                    //inj.timing of the end of pulse

  float rigid_arg;                      //idling regulator rigidity's argument
  bool  rigid_use;

  int rxlaf;                            //RxL air flow

  bool aftstr_enr;                      //flag, indicates that after start enrichment is active

  float ftls;                           //level of fuel in the fuel tank
  float egts;                           //exhaust gas temperature
  float ops;                            //oil pressure
 
  bool iac_cl_loop;                     //IAC closed loop flag

  float inj_duty;                       //injector's duty
  float maf;                            //mass air flow in g/sec
  float vent_duty;                      //PWM duty of cooling fan
  bool uniout[UNI_OUTPUT_NUM];          //States of universal outputs

  float fts;                            //Fuel temperature sensor
  float cons_fuel;                      //Consumed fuel (L)

  float lambda_mx;                      //mixed voltages from two EGO sensors

  float tchrg;                          //Corrected MAT value
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
  int   inj_aftstr_strokes[2];          //strokes for petrol and for gas
  float inj_prime_cold;                 //prime pulse width at -30C
  float inj_prime_hot;                  //prime pulse width at 70C
  float inj_prime_delay;                //prime pulse delay
  float inj_floodclear_tps;             //flood clear mode threshold
  int stbl_str_cnt;
  bool fldclr_start;
 };

 struct AnglesPar
 {
  float  max_angle;
  float  min_angle;
  float  angle_corr;
  float  dec_speed;
  float  inc_speed;
  unsigned char zero_adv_ang;
  bool igntim_wrkmap;
  bool manigntim_idl;
  float shift_igntim;
  bool  zero_adv_ang_oct;
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
  bool use_load_grid;
  int ve2_map_func;
  int cyl_num;                        //read-only parameter: number of cylinders
  float inj_cyl_disp;                 //The displacement of one cylinder in liters
  float mafload_const;                //precalculated constant for MAF (for calculation of MAF load)
  int uni_gas_v;
  float tps_raw;
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
  float idl_reg_p[2];                   //IAC closeed loop proportional coefficient, [0] - negative, [1] - positive
  float idl_reg_i[2];                   //IAC closed loop integral coefficient, [0] - negative, [1] - positive
  float idl_coef_thrd1;                 //coefficient for calculating closed loop entering RPM threshold
  float idl_coef_thrd2;                 //coefficient for calculating closed loop leaving RPM threshold
  int  idl_intrpm_lim;                  //RPM error limit for integrator
  float idl_map_value;                  //intake manifold pressure on idling
  float idl_iacminpos;
  float idl_iacmaxpos;
  bool  idl_useiacclongas;
  int iac_reg_db;
  bool use_thrassmap;                   //use throttle assist map
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
  int fuelcut_uni;
  int igncut_uni;
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
  float  ai5_adc_factor;                //MCP3204
  float  ai5_adc_correction;
  float  ai6_adc_factor;
  float  ai6_adc_correction;
  float  ai7_adc_factor;
  float  ai7_adc_correction;
  float  ai8_adc_factor;
  float  ai8_adc_correction;
  bool   adc_comp_mode;
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
  float add_i4_value;   //only if TPIC8101 option used
  float add_i5_value;   //secu-3i only and MCP3204 option used
  float add_i6_value;   //secu-3i only and MCP3204 option used
  float add_i7_value;   //secu-3i only and MCP3204 option used
  float add_i8_value;   //secu-3i only and MCP3204 option used
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
 const int OPCODE_BL_MANSTART          = 0xC8; //boot loader started manually (using a jumper)
 const int OPDATA_BL_MANSTART          = 0x8C; //data for OPCODE_BL_MANSTART
 const int OPCODE_RESET_LTFT           = 0xCA; //Reset LTFT map
 const int OPDATA_RESET_LTFT           = 0xBB; //Reset LTFT map
 const int OPCODE_SAVE_LTFT            = 9;  //save LTFT

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
  bool knock_selch[8];                  //selection of KS for each channel (cylinder)
  float knkclt_thrd;
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
  float hop_start_ang;                  //Выход ДХ: Начало испульса в градусах относ. ВМТ 
  float hop_durat_ang;                  //Выход ДХ: Длительность импульса в градусах
  bool flpmp_offongas;                  //Flag. Turn off or not fuel pump when fuel type is gas
  bool inj_offongas;                    //Turn off injector(s) on gas
  bool inj_offonpet;                    //Turn off injector(s) on petrol
  float evap_afbegin;
  float evap_afslope;
  float fp_timeout_strt;                //Fuel pump timeout for cranking (sec.)
  int pwm2_pwmfrq[2];                   //Frequencies for PWM channels
  float vss_pp1km;                      //number of VSS pulses per 1km of passed distance
 };

 struct EditTabPar
 {
  unsigned char tab_id;                 //идентификатор таблицы(данных) в наборе
  unsigned int address;                 //адрес начала фрагмента данных в таблице
  float table_data[64];                 //фрагмент данных (не более 16-ти значений)
  TCHAR name_data[32];                  //содержит текстовую информацию
  unsigned int data_size;               //размер фрагмента данных
  float beginPadding;
  float endPadding;
 };

 struct SepTabPar
 {
  int reserv;                           //reserved
  unsigned int address;                 //адрес начала фрагмента данных в таблице
  float table_data[32];                 //фрагмент данных (не более 16-ти байт)
  unsigned int data_size;               //размер фрагмента данных
 };

 struct DiagInpDat
 {
  bool f_secu3t;                        //SECU-3T flag
  float voltage;                        //board voltage
  float map;                            //MAP sensor
  float temp;                           //coolant temperature
  float add_i1;                         //additional input 1 (analog)
  float add_i2;                         //additional input 2 (analog)
  float add_i3;                         //additional input 3 (analog)
  float add_i4;                         //additional input 4 (analog, available only in firwares with TPIC8101 support)
  float add_i5;                         //MCP3204
  float add_i6;
  float add_i7;
  float add_i8;
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
  // IGN_O1, IGN_O2, IGN_O3, IGN_O4, IGN_O5, ECF, INJ_O1, INJ_O2, INJ_O3, INJ_O4, INJ_O5, BL, DE, STBL_O, CEL_O, FPMP_O, PWRR_O, EVAP_O, O2SH_O, COND_O, ADD_O2, TACH_O

  float diag_frq;                       //test signal frequency (10...5000 Hz)
  float diag_duty;                      //test signal duty (0...100%)
  int   diag_chan;                      // =0 - no selection
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
  int  bt_type;                         //Bluetooth type (2 bits): 0 - BC417, 1 - BK3231, 2 - BK3231S (JDY-31), 3 - BC352(HC-05)
  BYTE ibtn_keys[IBTN_KEYS_NUM][IBTN_KEY_SIZE];//iButton keys
 };

 //Describes a universal programmable output
 struct uni_output_par
 {
  bool use;                             //output's usage check
  unsigned char logicFunc;              //logic function between two conditions
  bool invers_1;                        //inversion flags for condition 1
  bool invers_2;                        //inversion flags for condition 2
  bool invers;                          //common inversion
  unsigned char condition1;             //code of condition 1
  unsigned char condition2;             //code of condition 2
  float on_thrd_1;                      //ON threshold (if value > on_thrd_1)
  float off_thrd_1;                     //OFF threshold (if value < off_thrd_1)
  float on_thrd_2;                      //same as on_thrd_1
  float off_thrd_2;                     //same as off_thrd_1
 };

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
 const int UNIOUT_COND_AI3 = 21;      //analog input 3
 const int UNIOUT_COND_AI4 = 22;      //analog input 4
 const int UNIOUT_COND_LPTMR = 23;    //looped timer
 const int UNIOUT_COND_AI5 = 24;      //analog input 5
 const int UNIOUT_COND_AI6 = 25;      //analog input 6
 const int UNIOUT_COND_AI7 = 26;      //analog input 7
 const int UNIOUT_COND_AI8 = 27;      //analog input 8
 const int UNIOUT_COND_GRTS = 28;     //GRTS
 const int UNIOUT_COND_MAP2 = 29;     //MAP2
 const int UNIOUT_COND_TMP2 = 30;     //TMP2
 const int UNIOUT_COND_INPUT1 = 31;   //INPUT1
 const int UNIOUT_COND_INPUT2 = 32;   //INPUT2
 const int UNIOUT_COND_MAF = 33;      //MAF
 const int UNIOUT_COND_TPSDOT = 34;   //TPS dot

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
  bool inj_secinjrowswt;              //Switch between inj. rows
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
  float inj_min_pw[2];                //min. inj PW (ms)
  float inj_maf_const[2];             //precalculated constant for MAF (for calculation of inj. PW)
  float mafload_const;                //precalculated constant for MAF (for calculation of MAF load)
  float inj_max_pw[2];                //maximum injection PW in ms
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
  bool lam_idlcorr;                   //Use lambda correction on idling
  bool  lam_crkheat;
  float lam_2stoichval;               //Stoichiometric value for 2-nd fuel
  bool lam_selch[8];                  //selection of KS for each channel (cylinder)
  bool lam_mixsen;                    //Mix signals from 2 sensors to sensor #1
  //heating:
  float eh_heating_time[2];
  float eh_temper_thrd;
  float eh_heating_act;
  float eh_aflow_thrd;  
 };

 struct AccelPar
 {
  float ae_tpsdot_thrd;               //TPS %/sec threshold
  float ae_coldacc_mult;              //Cold acceleration multiplier
  int ae_decay_time;                  //Decay time in strokes
  int ae_type;                        //Type of AE
  int ae_time;                        //time of active phase of AE
  float ae_mapdot_thrd;               //MAP kPa/sec threshold
  float ae_ballance;                  //MAP/TPS balance for acceleration enrichment (%)
  float xtau_s_thrd;
  float xtau_f_thrd;
  int wallwet_model;
 };


 //Size of all look up tables
 static const int LUTABSIZE = 16;
 static const int PWCORSIZE = 5;

 //data send and received to/from SECU-LZID driver
 struct InjDrvPar
 {
  bool reset_eeprom;     //to LZID: true - reset EEPROM (load sets from a flash)
  bool start_bldr;       //to LZID: true - start boot loader
  bool ee_status;        //from LZID: true - idle, false - busy. To LZID: true - says to save settings into the EEPROM
  bool set0_corrupted;   //from LZID only 
  bool set1_corrupted;   //from LZID only 
  bool dev_address;      //to/from LZID. This flag is ignored if broadcast flag is set
  float voltage;         //from LZID only
  bool broadcast;        //to LZID only - if this flag is set then all devices must accept this packet (edit settings simultaneously)

  int type;              //from LZID only
  int version;           //from LZID only
  int fw_opt;            //from LZID only
  bool gas_v;            //from LZID only

  int set_idx;           //allowed values: 0, 1
  int m_testch_sel;      //0 - turned off, channel selection: 1,2,3,4,5
  float m_testch_frq;    //2.5...200Hz
  float m_testch_duty;   //0...100%

  bool direct_flags[8];
  float m_pwm_period;
  float m_peak_duty;
  float m_hold_duty;
  float m_peak_on_time;
  float m_peak_pwm_time;
  float m_pth_pause;
  float m_reserved; //reserved and can be used for debug purposes

  float m_peak_on_tab[LUTABSIZE];
  float m_peak_duty_tab[LUTABSIZE];
  float m_hold_duty_tab[LUTABSIZE];
  float m_peak_full_tab[LUTABSIZE]; //since v2.0
  float m_pth_pause_tab[LUTABSIZE]; //since v2.0
  float m_pwmul_tab[PWCORSIZE]; //since v2.6
  float m_pwadd_tab[PWCORSIZE]; //since v2.6

  //flags
  bool m_peak_on_usetab;
  bool m_peak_duty_usetab;
  bool m_hold_duty_usetab;
  bool m_peak_full_usetab; //since v2.0
  bool m_pth_pause_usetab; //since v2.0

  bool m_tst_peak_pwm;
  bool m_tst_hold_pwm;
 };

 struct LzidBLHS
 {
  char data[5];
 };

 struct InjDrvAdr
 {
  int dev_address;
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
 const int SECU3_ALLOWABLE_UART_DIVISORS_COUNT = 11;
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
  _SD(115200,0x0015),
  _SD(250000,0x0009),
  _SD(500000,0x0004),
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
  SECU3IO::InjDrvPar    m_InjDrvPar;  //SECU-LZID
  SECU3IO::LzidBLHS     m_LzidBLHS;   //SECU-LZID  
 };

const int INPAVNUM = 14;

 struct FwConstsData
 {  
  int fi_enter_strokes;
  int fi_leave_strokes;
  float iac_cond_add;
  float aircond_clt;
  float aircond_tps;
  float idl_ve;
  float evap_clt;
  float evap_tps_lo;
  float evap_tps_hi;
  float frap;
  float idl_ve_g;
  float heating_t_off;
  float heating_time;
  float idltorun_stp_en;
  float idltorun_stp_le;
  int inpavnum[INPAVNUM];
  float vent_delay;
  float vent_iacoff;
  float epas_iacoff;
  int vent_pwmsteps;
  int vent_minband;
  int an_tps_mul;
  int hall_predict;
  float vtachom_mult;
  float grheat_time;
  int add_i1_sub;
  int add_i2_sub;
  int idlreg_captrange;
  float idlent_timval;
  float gasval_ontime;
  float tdc_angle[8];
  float smp_angle;
  float dwl_dead_time;
  float sfc_tps_thrd;
  float evap_map_thrd;
  int ckps_skip_trig;
  int maninjpw_idl;
  int oilpress_cut;
  float tpsdot_mindt;
  float irr_k_load;
  float irr_k_rpm;
  int cold_eng_int;
  float iacreg_period;
  float iacreg_turn_on_temp;
  int vent_maxband;
  float pwron_time;
  float pwron_time1;

  int    ltft_mode;       //!< 0 - LTFT is turned off, 1 - use only for petrol, 2 - use only for gas, 3 - use for both petrol and gas
  float  ltft_learn_clt;  //!< Temperature threshold for learning
  float  ltft_cell_band;  //!< cell band in %
  float  ltft_stab_time;  //!< Learn stability time
  float  ltft_learn_grad; //!< Learning gradient
  float  ltft_learn_gpa;
  float  ltft_learn_gpd;
  int    ltft_neigh_rad;
  int    ltft_sigswt_num;
  int    ltft_stab_str;

  int  pwrrelay_uni;

  int thrass_algo;

  bool btbaud_use[5];

  float mapdot_mindt;
  bool uart_silent;  

  int fueldens_corr_use;
  bool fts_source;
  bool tmrpmtc_mode;

  float vent_pwm_turnoff_hyst;

  float save_param_timeout;
  
  float fuelcut_vss_thrd;

  float evap_on_vss_thrd;
  float evap_off_vss_thrd;
  float iac_addonrun_vss_thrd;

  int iac_min_rpm_on_run;

  bool ltft_on_idling;

  float ltft_min;
  float ltft_max;

  bool use_injnonlin_corr;

  int ego_fc_delay;
  int ego_ac_delay;
 };

 const float start_map_rpm_slots[16] = {200,240,280,320,360,400,440,480,520,560,600,640,680,720,760,800};
 const float work_map_rpm_slots[16]  = {600,720,840,990,1170,1380,1650,1950,2310,2730,3210,3840,4530,5370,6360,7500};
 const float temp_map_tmp_slots[16]  = {-30,-20,-10,0,10,20,30,40,50,60,70,80,90,100,110,120};
 const float dwellcntrl_map_slots[32] = { 5.4f, 5.8f, 6.2f, 6.6f, 7.0f, 7.4f, 7.8f, 8.2f, 8.6f, 9.0f, 9.4f, 9.8f,10.2f,10.6f,11.0f,11.4f,
                                        11.8f,12.2f,12.6f,13.0f,13.4f,13.8f,14.2f,14.6f,15.0f,15.4f,15.8f,16.2f,16.6f,17.0f,17.4f,17.8f};
 const float rigid_map_slots[8]  = {1,2,3,4,5,6,7,8};
 const float manigntim_map_slots[16]  = {.0f,0.33f,0.66f,1.00f,1.33f,1.66f,2.00f,2.33f,2.66f,3.00f,3.33f,3.66f,4.00f,4.33f,4.66f,5.0f};

 const float maninjpwc_map_slots[17]  = {.0f,0.312f,0.625f,0.937f,1.250f,1.562f,1.875f,2.187f,2.50f,2.812f,3.125f,3.437f,3.750f,4.062f,4.375f,4.687f,5.000f};

 const float voltage_map_slots[16] = {5.4f,6.2f,7.0f,7.8f,8.6f,9.4f,10.2f,11.0f,11.8f,12.6f,13.4f,14.2f,15.0f,15.8f,16.6f,17.4f};

 const float work_map_lod_slots[16]  = {25,30,35,40,45,50,55,60,65,70,75,80,85,90,95,100};

 const float inj_cyladd_map_slots[8] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f};

 const int COPT_OBD_SUPPORT = 0;
 const int COPT_ATMEGA1284 = 1;
 const int COPT_ODDFIRE_ALGO = 2;
 const int COPT_ATMEGA128 = 3;
 const int COPT_SPLIT_ANGLE = 4;
 const int COPT_TPIC8101 = 5;
 const int COPT_CAM_SYNC = 6;
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
 const int COPT_UART_BINARY = 25;   //obsolete, left for compatibility
 const int COPT_CKPS_2CHIGN = 26;
 const int COPT_ATMEGA644 = 27;     //obsolete, left for compatibility
 const int COPT_FUEL_INJECT = 28;
 const int COPT_GD_CONTROL = 29;
 const int COPT_CARB_AFR = 30;
 const int COPT_CKPS_NPLUS1 = 31;

 const int SECU3_COMPILE_OPTIONS_BITS_COUNT = 32;
 //<bitnumber, name>
#define _SD std::pair<int, TCHAR*>
 static std::pair<int, TCHAR*> secu3_compile_options_bits[SECU3_COMPILE_OPTIONS_BITS_COUNT] =
 {
  _SD(COPT_OBD_SUPPORT,     _T("OBD_SUPPORT")),
  _SD(COPT_ATMEGA1284,      _T("ATMEGA1284")),
  _SD(COPT_ODDFIRE_ALGO,    _T("ODDFIRE_ALGO")),
  _SD(COPT_ATMEGA128,       _T("ATMEGA128")),
  _SD(COPT_SPLIT_ANGLE,     _T("SPLIT_ANGLE")),
  _SD(COPT_TPIC8101,        _T("TPIC8101")),
  _SD(COPT_CAM_SYNC,        _T("CAM_SYNC")),
  _SD(COPT_DWELL_CONTROL,   _T("DWELL_CONTROL")),
  _SD(COPT_COOLINGFAN_PWM,  _T("COOLINGFAN_PWM")),
  _SD(COPT_REALTIME_TABLES, _T("REALTIME_TABLES")),
  _SD(COPT_ICCAVR_COMPILER, _T("ICCAVR_COMPILER")),
  _SD(COPT_AVRGCC_COMPILER, _T("AVRGCC_COMPILER")),
  _SD(COPT_DEBUG_VARIABLES, _T("DEBUG_VARIABLES")),
  _SD(COPT_PHASE_SENSOR,    _T("PHASE_SENSOR")),
  _SD(COPT_PHASED_IGNITION, _T("PHASED_IGNITION")),
  _SD(COPT_FUEL_PUMP,       _T("FUEL_PUMP")),
  _SD(COPT_THERMISTOR_CS,   _T("THERMISTOR_CS")),
  _SD(COPT_SECU3T,          _T("SECU3T")),
  _SD(COPT_DIAGNOSTICS,     _T("DIAGNOSTICS")),
  _SD(COPT_HALL_OUTPUT,     _T("HALL_OUTPUT")),
  _SD(COPT_REV9_BOARD,      _T("REV9_BOARD")),
  _SD(COPT_STROBOSCOPE,     _T("STROBOSCOPE")),
  _SD(COPT_SM_CONTROL,      _T("SM_CONTROL")),
  _SD(COPT_VREF_5V,         _T("VREF_5V")),
  _SD(COPT_HALL_SYNC,       _T("HALL_SYNC")),
  _SD(COPT_UART_BINARY,     _T("UART_BINARY")),  //obsolete, left for compatibility
  _SD(COPT_CKPS_2CHIGN,     _T("CKPS_2CHIGN")),
  _SD(COPT_ATMEGA644,       _T("ATMEGA644")),    //obsolete, left for compatibility
  _SD(COPT_FUEL_INJECT,     _T("FUEL_INJECT")),
  _SD(COPT_GD_CONTROL,      _T("GD_CONTROL")),
  _SD(COPT_CARB_AFR,        _T("CARB_AFR")),
  _SD(COPT_CKPS_NPLUS1,     _T("CKPS_NPLUS1"))
 };
#undef _SD

};
