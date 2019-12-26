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

/** \file MIInjTim.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include "MeasInstrBase.h"

class AFX_EXT_CLASS CMIInjTim : public MeasInstrBase
{
 public:
  CMIInjTim();
  virtual ~CMIInjTim();

  //-------interface-----------------------
  virtual void Create(CWnd* pParent, UINT id);
  //---------------------------------------

  void SetITMode(int mode);
};


class AFX_EXT_CLASS CMIInjTimB : public CMIInjTim
{
 public:
  CMIInjTimB() {}
  virtual ~CMIInjTimB() {}

  virtual void Create(CWnd* pParent);
};

class AFX_EXT_CLASS CMIInjTimE : public CMIInjTim
{
 public:
  CMIInjTimE() {}
  virtual ~CMIInjTimE() {}

  virtual void Create(CWnd* pParent);
};

//-------------------------------------------------------------

class AFX_EXT_CLASS CMIInjTimGraph : public MeasInstrBase
{
 public:
  CMIInjTimGraph();
  virtual ~CMIInjTimGraph();

  //-------interface-----------------------
  virtual void Create(CWnd* pParent, UINT id);
  //---------------------------------------

  void SetITMode(int mode);
};

class AFX_EXT_CLASS CMIInjTimBGraph : public CMIInjTimGraph
{
 public:
  CMIInjTimBGraph() {}
  virtual ~CMIInjTimBGraph() {}

  virtual void Create(CWnd* pParent);
};

class AFX_EXT_CLASS CMIInjTimEGraph : public CMIInjTimGraph
{
 public:
  CMIInjTimEGraph() {}
  virtual ~CMIInjTimEGraph() {}

  virtual void Create(CWnd* pParent);
};
