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

#pragma once

enum EMapTypes
{
 TYPE_MAP_DA_START     = 1,  //must be = 1
 TYPE_MAP_DA_IDLE      = 2,  //must be = 2
 TYPE_MAP_DA_WORK      = 3,  //must be = 3
 TYPE_MAP_DA_TEMP_CORR = 4,  //must be = 4
 TYPE_MAP_INJ_VE       = 5,  //must be = 5
 TYPE_MAP_INJ_AFR      = 6,  //must be = 6
 TYPE_MAP_INJ_CRNK     = 7,  //must be = 7
 TYPE_MAP_INJ_WRMP     = 8,  //must be = 8
 TYPE_MAP_INJ_DEAD     = 9,  //must be = 9
 TYPE_MAP_INJ_IDLR     = 10, //must be = 10
 TYPE_MAP_INJ_IDLC     = 11, //must be = 11
 //separate maps
 TYPE_MAP_ATTENUATOR   = 12, //
 TYPE_MAP_DWELLCNTRL   = 13, //
 TYPE_MAP_CTS_CURVE    = 14, //
 TYPE_MAP_CHOKE_OP     = 15, //
 TYPE_MAP_ATS_CURVE    = 16, //
 TYPE_MAP_ATS_CORR     = 17, //

 TYPE_MAP_GME_WND      = 32  //pseudo map, grid mode editing window
};
