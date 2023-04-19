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

/** \file MISensAFR.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include "MeasInstrBase.h"

class AFX_EXT_CLASS CMISensAFR : public MeasInstrBase
{
 public:
  CMISensAFR();
  virtual ~CMISensAFR();

  //-------interface-----------------------
  virtual void Create(CWnd* pParent);
  //---------------------------------------
};

class AFX_EXT_CLASS CMISensAFR2 : public MeasInstrBase
{
 public:
  CMISensAFR2();
  virtual ~CMISensAFR2();

  //-------interface-----------------------
  virtual void Create(CWnd* pParent);
  //---------------------------------------
};

class AFX_EXT_CLASS CMISensAFRGraph : public MeasInstrBase
{
 public:
  CMISensAFRGraph();
  virtual ~CMISensAFRGraph();

  //-------interface-----------------------
  virtual void Create(CWnd* pParent);
  //---------------------------------------
};

class AFX_EXT_CLASS CMISensAFR2Graph : public MeasInstrBase
{
 public:
  CMISensAFR2Graph();
  virtual ~CMISensAFR2Graph();

  //-------interface-----------------------
  virtual void Create(CWnd* pParent);
  //---------------------------------------
};
