
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

/** \file DynamicValues.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

namespace TablDesk
{
 struct DynVal
 {
  int rpm;
  float temp;
  int air_flow;

  float adv_ang;
  float knock_retard;
  bool knkret_use;
  float strt_aalt;
  bool strt_use;
  float idle_aalt;
  bool idle_use;
  float work_aalt;
  bool work_use;
  float temp_aalt;
  bool temp_use;
  float airt_aalt;
  bool airt_use;
  float idlreg_aac;
  bool idlreg_use;
  float octan_aac;
  bool octan_use;

  float tps;
  float iac_pos;
  int tpsdot;
  float voltage;
  float add_i1;  //lambda sensor
  float tmp2;
  float baro_press;
  float load;

  float afr;
  bool acceleration; //acceleration/deceleration
  bool ie; //fuel cut if = 0

  float air_temp; //IAT

  float rigid_arg; //idling regulator rigidity's argument
  bool rigid_use;
 };
}
