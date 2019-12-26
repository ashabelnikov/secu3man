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

/** \file MIVoltage.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include "MeasInstrBase.h"

class CToolTipCtrlEx;

class AFX_EXT_CLASS CMIVoltage : public MeasInstrBase
{
 public:
  CMIVoltage();
  virtual ~CMIVoltage();

  //-------interface-----------------------
  virtual void Create(CWnd* pParent, UINT id);
  //---------------------------------------

protected:
  std::auto_ptr<CToolTipCtrlEx> mp_ttc;
};


class AFX_EXT_CLASS CMIAddI1 : public CMIVoltage
{
 public:
  CMIAddI1() {}
  virtual ~CMIAddI1() {}

  virtual void Create(CWnd* pParent);
};

class AFX_EXT_CLASS CMIAddI1Graph : public MeasInstrBase
{
 public:
  CMIAddI1Graph();
  virtual ~CMIAddI1Graph();

  //-------interface-----------------------
  virtual void Create(CWnd* pParent);
  //---------------------------------------
};

class AFX_EXT_CLASS CMIAddI2 : public CMIVoltage
{
 public:
  CMIAddI2() {}
  virtual ~CMIAddI2() {}

  virtual void Create(CWnd* pParent);
};

class AFX_EXT_CLASS CMIAddI2Graph : public MeasInstrBase
{
 public:
  CMIAddI2Graph();
  virtual ~CMIAddI2Graph();

  //-------interface-----------------------
  virtual void Create(CWnd* pParent);
  //---------------------------------------
};
