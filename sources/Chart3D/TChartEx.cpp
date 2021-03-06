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

/** \file TChartEx.cpp
 * \author Alexey A. Shabelnikov
 */

#include <vcl\vcl.h>
#pragma hdrstop

#include "TChartEx.h"

//---------------------------------------------------------------------------
namespace
{
 void __fastcall Register()
 {
  TComponentClass classes[1] = {__classid(TChartEx)};
  RegisterComponents("secu-3_vcl", classes, 0);
 }
}

//---------------------------------------------------------------------------
__fastcall TChartEx::TChartEx(TComponent* Owner)
: TChart(Owner)
{
 //empty
}

//---------------------------------------------------------------------------
static inline TChartEx *ValidCtrCheck()
{
 return new TChartEx(NULL);
}

//---------------------------------------------------------------------------
