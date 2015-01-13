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

#ifndef _SECU3_TABLES_DEF_H_
#define _SECU3_TABLES_DEF_H_

#include "SECU3Types.h"

#pragma pack( push, enter_SECU3TablesDef )
#pragma pack(1) //<--SECU3

namespace SECU3IO {

using namespace SECU3Types;

//описывает основные таблицы (таблицы УОЗ и впрыска топлива) в системе SECU-3
typedef struct
{
 _uchar name[F_NAME_SIZE];                             // ассоциированное имя (имя семейства)
 //ignition maps
 _char f_str[F_STR_POINTS];                            // функция УОЗ на старте
 _char f_idl[F_IDL_POINTS];                            // функция УОЗ для ХХ
 _char f_wrk[F_WRK_POINTS_L][F_WRK_POINTS_F];          // основная функция УОЗ (3D)
 _char f_tmp[F_TMP_POINTS];                            // функция коррект. УОЗ по температуре
 //fuel injection maps
 _uchar inj_ve[INJ_VE_POINTS_L][INJ_VE_POINTS_F];      // VE
 _uchar inj_afr[INJ_VE_POINTS_L][INJ_VE_POINTS_F];     // AFR
 _uint inj_cranking[INJ_CRANKING_LOOKUP_TABLE_SIZE];   // Cranking PW
 _uchar inj_warmup[INJ_WARMUP_LOOKUP_TABLE_SIZE];      // Warmup enrichment
 _uint inj_dead_time[INJ_DT_LOOKUP_TABLE_SIZE];        // Injector's dead time
 _uchar inj_iac_run_pos[INJ_IAC_POS_TABLE_SIZE];       // IAC/PWM position on run
 _uchar inj_iac_crank_pos[INJ_IAC_POS_TABLE_SIZE];     // IAC/PWM position on cranking
 _uchar inj_ae_tps_enr[INJ_AE_TPS_LOOKUP_TABLE_SIZE];  // values of the AE's TPS lookup table
 _char  inj_ae_tps_bins[INJ_AE_TPS_LOOKUP_TABLE_SIZE]; // bins of the AE's TPS lookup table
 _uchar inj_ae_rpm_enr[INJ_AE_RPM_LOOKUP_TABLE_SIZE];  // values of the AE's RPM lookup table
 _uchar inj_ae_rpm_bins[INJ_AE_RPM_LOOKUP_TABLE_SIZE]; // bins of the AE's RPM lookup table
 _uchar inj_aftstr[INJ_AFTSTR_LOOKUP_TABLE_SIZE];      // afterstart enrichment table

 _uchar reserved[702];                                 // reserved bytes - for compatibility
}f_data_t;

} //SECU3IO

#pragma pack( pop, enter_SECU3TablesDef )

#endif //_SECU3_TABLES_DEF_H_
