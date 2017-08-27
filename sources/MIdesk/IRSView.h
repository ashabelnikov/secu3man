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

/** \file IRSView.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

namespace SECU3IO
{
 struct RawSensDat;
}

class IRSView
{
 public:
  virtual void Show(bool show) = 0;                                //show/hide content of view
  virtual void Enable(bool enable) = 0;                            //enable/disable view
  virtual void SetValues(const SECU3IO::RawSensDat* i_values) = 0; //set data of view
  virtual void GetValues(SECU3IO::RawSensDat* o_values) = 0;       //get data from view
  virtual void EnableSECU3TItems(bool i_enable) = 0;               //Enable SECU-3T specific features
  virtual void EnableExtraIO(bool i_enable) = 0;                   //Enable ExtraIO features available in the SECU-3i only
};
