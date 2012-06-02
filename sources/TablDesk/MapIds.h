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

#pragma once

enum EMapTypes
{
 TYPE_MAP_DA_START = 0,    //must be =0
 TYPE_MAP_DA_IDLE = 1,     //must be =1
 TYPE_MAP_DA_WORK = 2,     //must be =2
 TYPE_MAP_DA_TEMP_CORR = 3,//must be =3
 // 4, 5, 6, 7 - are reserved
 TYPE_MAP_ATTENUATOR = 8,  //must be =8. used by derived classes
 TYPE_MAP_DWELLCNTRL = 9,  //must be =9. used by derived classes
 TYPE_MAP_CTS_CURVE  = 10
};
