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

/** \file IMIView.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

namespace SECU3IO
{
 struct SensorDat;
}

//Interface for controlling over fixtures panel (View)
class IMIView
{
 public:
  virtual void Show(bool show) = 0;         //show/hide content of view
  virtual void Enable(bool enable) = 0;                            //enable/disable view
  virtual void SetValues(const SECU3IO::SensorDat* i_values, bool i_dir) = 0;  //set view values
  virtual void Reset(void) = 0;
};
