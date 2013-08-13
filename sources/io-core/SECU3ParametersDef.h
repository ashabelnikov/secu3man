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

#ifndef _SECU3_PARAMETERS_DEF_H_
#define _SECU3_PARAMETERS_DEF_H_

#include "SECU3Types.h"

#pragma pack( push, enter_SECU3ParametersDef )
#pragma pack(1) //<--SECU3

namespace SECU3IO {

using namespace SECU3Types;

//описывает параметры системы
typedef struct
{
 _uchar tmp_use;                        //признак комплектации ДТОЖ-ом
 _uchar carb_invers;                    //инверсия концевика на карбюраторе
 _uchar idl_regul;                      //поддерживать заданные обороты ХХ регулмрованием УОЗ
 _uchar fn_benzin;                      //номер набора характеристик используемый для бензина
 _uchar fn_gas;                         //номер набора характеристик используемый для газа
 _uint  map_lower_pressure;             //нижнее значене ДАД по оси таблицы(кПа)
 _uint  ephh_lot;                       //нижний порог ЭПХХ (мин-1)
 _uint  ephh_hit;                       //верхний порог ЭПХХ (мин-1)
 _uint  starter_off;                    //порог выключения стартера (мин-1)
 _int   map_upper_pressure;             //верхнее значене ДАД по оси таблицы (кПа)
 _uint  smap_abandon;                   //обороты перехода с пусковой карты на рабочую  (мин-1)
 _int   max_angle;                      //ограничение максимального УОЗ
 _int   min_angle;                      //ограничение минимального УОЗ
 _int   angle_corr;                     //октан-коррекция УОЗ
 _uint  idling_rpm;                     //заданные обороты ХХ для поддержания регулмрованием УОЗ
 _int   ifac1;                          //коэффициенты П-регулятора оборотов ХХ, для положительной и
 _int   ifac2;                          //отрицательной ошибке соответственно.
 _int   MINEFR;                         //зона нечувствительности регулятора (обороты)
 _int   vent_on;                        //температура включения вентилятора
 _int   vent_off;                       //температура выключения вентилятора

 _int  map_adc_factor;
 _long map_adc_correction;
 _int  ubat_adc_factor;
 _long ubat_adc_correction;
 _int  temp_adc_factor;
 _long temp_adc_correction;

 _uchar ckps_edge_type;
 _uchar ckps_cogs_btdc;
 _uchar ckps_ignit_cogs;

 _int angle_dec_spead;
 _int angle_inc_spead;
 _int idlreg_min_angle;
 _int idlreg_max_angle;
 _int map_curve_offset;              //can be negative  
 _int map_curve_gradient;            //can be negative (in case of inverse characteristic curve)!

 _int epm_on_threshold;

 _uint  ephh_lot_g;                  //нижний порог ЭПХХ (газ)
 _uint  ephh_hit_g;                  //верхний порог ЭПХХ (газ)
 _uchar shutoff_delay;               //задержка выключения клапана

 _uint  uart_divisor;                //делитель для соответствующей скорости UART-a
 _uchar uart_period_t_ms;            //период посылки пакетов в десятках миллисекунд

 _uchar ckps_engine_cyl;             //кол-во цилиндров двигателя

 //--knock
 _uchar knock_use_knock_channel;     //признак использования канала детенации
 _uchar knock_bpf_frequency;         //центральная частота полосового фильтра
 _int   knock_k_wnd_begin_angle;     //начало детонационного окна (градусы)
 _int   knock_k_wnd_end_angle;       //конец детонационного окна (градусы)
 _uchar knock_int_time_const;        //постоянная времени интегрирования (код)

 _int knock_retard_step;             //шаг смещения УОЗ при детонации
 _int knock_advance_step;            //шаг восстановления УОЗ
 _int knock_max_retard;              //максимальное смещение УОЗ
 _uint knock_threshold;              //порог детонации - напряжение
 _uchar knock_recovery_delay;        //задержка восстановления УОЗ в рабочих циклах двигателя

 //--/knock

 _uchar vent_pwm;                    //control ventilator by using pwm

 _uchar ign_cutoff;                  // Cutoff ignition when RPM reaches specified threshold
 _uint ign_cutoff_thrd;              // Cutoff threshold (RPM)

 _uchar zero_adv_ang;                // Zero advance angle flag

 _uchar ckps_merge_ign_outs;         // Merge ignition signals to single ouput

 _char  hop_start_cogs;              // Hall output: start of pulse in teeth relatively to TDC 
 _uchar hop_durat_cogs;              // Hall output: duration of pulse in teeth

 _uchar cts_use_map;                 // Flag which indicates using of lookup table for coolant temperature sensor

 _uchar ckps_cogs_num;               // number of crank wheel's teeth 
 _uchar ckps_miss_num;               // number of missing crank wheel's teeth

 _uchar ref_s_edge_type;             // REF_S edge type (тип фронта ДНО)

 _int  tps_adc_factor;               // ADC error compensation factor for TPS
 _long tps_adc_correction;           // ADC error compensation correction for TPS
 _int  ai1_adc_factor;               // ADC error compensation factor for ADD_IO1 input 
 _long ai1_adc_correction;           // ADC error compensation correction for ADD_IO1 input
 _int  ai2_adc_factor;               // ADC error compensation factor for ADD_IO2 input
 _long ai2_adc_correction;           // ADC error compensation correction for ADD_IO2 input

 _int  tps_curve_offset;             // offset of curve in volts
 _int  tps_curve_gradient;           // gradient of curve in Percentage/V

 _uchar tps_threshold;               // TPS threshold used to switch work and idle modes
 _uint  sm_steps;                    // Number of steps of choke stepper motor

 _int idlreg_turn_on_temp;           // Idling regulator turn on temperature

 _uchar bt_flags;                    // Bluetooth and security related flags

 _uchar hall_flags;                  // Hall sensor flags

 //Эти зарезервированные байты необходимы для сохранения бинарной совместимости
 //новых версий прошивок с более старыми версиями. При добавлении новых данных
 //в структуру, необходимо расходовать эти байты.
 _uchar reserved[2];

 _uint crc;                          //контрольная сумма данных этой структуры (для проверки корректности данных после считывания из EEPROM)
}params_t;

} //SECU3IO

#pragma pack( pop, enter_SECU3ParametersDef )

#endif //_SECU3_PARAMETERS_DEF_H_
