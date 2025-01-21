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

/** \file MIMap2.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include "MeasInstrBase.h"

//MAP2
class AFX_EXT_CLASS CMIMap2 : public MeasInstrBase
{
 public:
  CMIMap2();
  virtual ~CMIMap2();

  //-------interface-----------------------
  virtual void Create(CWnd* pParent);
  virtual void Append(const SECU3IO::SensorDat* i_values, bool i_revdir = false);
  //---------------------------------------
};

class AFX_EXT_CLASS CMIMap2Graph : public MeasInstrBase
{
 public:
  CMIMap2Graph();
  virtual ~CMIMap2Graph();

  //-------interface-----------------------
  virtual void Create(CWnd* pParent);
  virtual void Append(const SECU3IO::SensorDat* i_values, bool i_revdir = false);
  //---------------------------------------
};

//GPS:
class AFX_EXT_CLASS CMIGps : public MeasInstrBase
{
 public:
  CMIGps();
  virtual ~CMIGps();

  //-------interface-----------------------
  virtual void Create(CWnd* pParent);
  virtual void Append(const SECU3IO::SensorDat* i_values, bool i_revdir = false);
  //---------------------------------------
};

class AFX_EXT_CLASS CMIGpsGraph : public MeasInstrBase
{
 public:
  CMIGpsGraph();
  virtual ~CMIGpsGraph();

  //-------interface-----------------------
  virtual void Create(CWnd* pParent);
  virtual void Append(const SECU3IO::SensorDat* i_values, bool i_revdir = false);
  //---------------------------------------
};

//FPS:
class AFX_EXT_CLASS CMIFps : public MeasInstrBase
{
 public:
  CMIFps();
  virtual ~CMIFps();

  //-------interface-----------------------
  virtual void Create(CWnd* pParent);
  virtual void Append(const SECU3IO::SensorDat* i_values, bool i_revdir = false);
  //---------------------------------------
};

class AFX_EXT_CLASS CMIFpsGraph : public MeasInstrBase
{
 public:
  CMIFpsGraph();
  virtual ~CMIFpsGraph();

  //-------interface-----------------------
  virtual void Create(CWnd* pParent);
  virtual void Append(const SECU3IO::SensorDat* i_values, bool i_revdir = false);
  //---------------------------------------
};
