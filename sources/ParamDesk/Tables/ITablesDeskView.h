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

/** \file ITablesDeskView.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include "common/FastDelegate.h"
#include "common/unicodesupport.h"

namespace SECU3IO { struct SensorDat; };

class ITablesDeskView
{
 public:
  //Dynamically indicated values in the GME window(s)
  virtual void SetDynamicValues(const SECU3IO::SensorDat* sd) = 0;

  //enable/disable indication of ignition timing values in GME window
  virtual void EnableAdvanceAngleIndication(bool i_enable) = 0;
};
