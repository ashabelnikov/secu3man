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

#ifndef _SECU3_TABLES_DEF_H_
#define _SECU3_TABLES_DEF_H_

#include "SECU3Types.h"

#pragma pack( push, enter_SECU3TablesDef )
#pragma pack(1) //<--SECU3

namespace SECU3IO {

using namespace SECU3Types;

//описывает основные таблицы (таблицы УОЗ) в системе SECU-3
typedef struct
{
 _char f_str[F_STR_POINTS];                       // функция УОЗ на старте
 _char f_idl[F_IDL_POINTS];                       // функция УОЗ для ХХ
 _char f_wrk[F_WRK_POINTS_L][F_WRK_POINTS_F];     // основная функция УОЗ (3D)
 _char f_tmp[F_TMP_POINTS];                       // функция коррект. УОЗ по температуре
 _uchar name[F_NAME_SIZE];                        // ассоциированное имя (имя семейства)
}f_data_t;

} //SECU3IO

#pragma pack( pop, enter_SECU3TablesDef )

#endif //_SECU3_TABLES_DEF_H_
