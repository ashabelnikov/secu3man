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

/** \file MITachometer.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include "MeasInstrBase.h"
#include "ui-core/OscillCtrl.h"

class AFX_EXT_CLASS CMITachometer : public MeasInstrBase
{
 public:
  CMITachometer();
  virtual ~CMITachometer();

  //-------interface-----------------------
  virtual void Create(CWnd* pParent);
  virtual void Append(const SECU3IO::SensorDat* i_values, bool i_revdir = false);
  //---------------------------------------

};

class AFX_EXT_CLASS CMITachometerGraph : public MeasInstrBase
{
 public:
  CMITachometerGraph();
  virtual ~CMITachometerGraph();

  //-------interface-----------------------
  virtual void Create(CWnd* pParent);
  virtual void Append(const SECU3IO::SensorDat* i_values, bool i_revdir = false);
  //---------------------------------------
};
