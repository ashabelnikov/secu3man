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
  float knock_k;                        //уровень сигнала детонации (усредненный за время фазового окна)
  float knock_retard;                   //корректировка УОЗ при детонации
  unsigned char ce_state;               // !currently is not used! 
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
 };
 
 struct AnglesPar
 {
  float  max_angle;
  float  min_angle;
  float  angle_corr;
  float  dec_spead;
  float  inc_spead;
 };


 struct FunSetPar
 {
  unsigned char fn_benzin;
  unsigned char fn_gas;
  float map_lower_pressure;
  float map_upper_pressure;
  float map_curve_offset;
  float map_curve_gradient;
 };

 struct IdlRegPar
 {
  unsigned char idl_regul;
  float  ifac1;
  float  ifac2;
  int  MINEFR;
  int  idling_rpm;
  float min_angle;
  float max_angle;
 };


 struct CarburPar
 {
  int  ephh_lot;
  int  ephh_hit;
  unsigned char carb_invers;
  float epm_ont;                       //порог включения ЭМР
  int  ephh_lot_g;
  int  ephh_hit_g;
  float shutoff_delay;                 //задержка выключения клапана
 };

 struct TemperPar
 {
  unsigned char tmp_use;
  unsigned char vent_pwm;
  float  vent_on;
  float  vent_off;
 };

 struct ADCCompenPar
 {
  float  map_adc_factor;               //коэффициент передаточной погрешности
  float  map_adc_correction;           //сдвиг в вольтах
  float  ubat_adc_factor;
  float  ubat_adc_correction;
  float  temp_adc_factor;
  float  temp_adc_correction;
 };

 struct RawSensDat
 {
  float map_value;
  float ubat_value; 
  float temp_value;
  float knock_value;
 };

 struct CKPSPar
 {
  unsigned char ckps_edge_type;
  unsigned char ckps_cogs_btdc;
  unsigned char ckps_ignit_cogs;
  unsigned char ckps_engine_cyl;
 };

 struct OPCompNc                       //спользуется если надо просто принять или послать определенный код
 {
  unsigned char opcode;
 };

 const int OPCODE_EEPROM_PARAM_SAVE = 1;
 const int OPCODE_CE_SAVE_ERRORS    = 2;
 const int OPCODE_READ_FW_SIG_INFO  = 3;

 struct KnockPar
 {
  unsigned char knock_use_knock_channel;
  float knock_bpf_frequency;
  float knock_k_wnd_begin_angle;
  float knock_k_wnd_end_angle;
  int knock_int_time_const;

  float knock_retard_step;    //шаг смещения УОЗ при детонации
  float knock_advance_step;   //шаг восстановления УОЗ 
  float knock_max_retard;     //максимальное смещение УОЗ 
  float knock_threshold;      //в вольтах
  int knock_recovery_delay;   //в рабочих циклах двигателя
 };

 struct CEErrors
 {
  DWORD flags;
 };

 const int FW_SIGNATURE_INFO_SIZE = 48;
 struct FWInfoDat
 {
  char   info[FW_SIGNATURE_INFO_SIZE];
 };

 struct MiscelPar
 {
  int baud_rate;                       //скорость UART-a
  int period_ms;                       //период посылки пакетов мс.
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
 const int SECU3_ALLOWABLE_UART_DIVISORS_COUNT = 7;
 //<spead, divisor>
#define _SD std::pair<int, int>
 static std::pair<int, int> secu3_allowable_uart_divisors[SECU3_ALLOWABLE_UART_DIVISORS_COUNT] =
 {
  _SD(2400,   0x01A0),
  _SD(4800,   0x00CF),
  _SD(9600,   0x0067),
  _SD(14400,  0x0044),
  _SD(19200,  0x0033),
  _SD(38400,  0x0019),
  _SD(57600,  0x0010)
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
 };

 const float start_map_rpm_slots[16] = {200,240,280,320,360,400,440,480,520,560,600,640,680,720,760,800};
 const float idle_map_rpm_slots[16]  = {600,720,840,990,1170,1380,1650,1950,2310,2730,3210,3840,4530,5370,6360,7500};
 const int work_map_rpm_slots[16]    = {600,720,840,990,1170,1380,1650,1950,2310,2730,3210,3840,4530,5370,6360,7500};
 const float temp_map_rpm_slots[16]  = {-30,-20,-10,0,10,20,30,40,50,60,70,80,90,100,110,120};
 const float coilregul_map_slots[32] = { 5.4f, 5.8f, 6.2f, 6.6f, 7.0f, 7.4f, 7.8f, 8.2f, 8.6f, 9.0f, 9.4f, 9.8f,10.2f,10.6f,11.0f,11.4f, 
                                        11.8f,12.2f,12.6f,13.0f,13.4f,13.8f,14.2f,14.6f,15.0f,15.4f,15.8f,16.2f,16.6f,17.0f,17.4f,17.8f};
};
