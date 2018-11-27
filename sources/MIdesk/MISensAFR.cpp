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

/** \file MISensAFR.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "resource.h"
#include "MISensAFR.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMISensAFR::CMISensAFR()
{
 //empty
}

CMISensAFR::~CMISensAFR()
{
 //empty
}

void CMISensAFR::Create(CWnd* pParent)
{
 MeasInstrBase::Create(pParent, IDC_MI_SENSAFR); //create window

 m_meter.SetRange (6.0, 24.0);
 m_meter.SetLabelsDecimals(1);
 m_meter.SetValueDecimals(2);
 m_meter.SetTitle(MLL::LoadString(IDS_MI_SENSAFR_TITLE));
 m_meter.SetColor(meter_value,RGB(10,80,255));
 m_meter.SetColor(meter_bground, GetSysColor(COLOR_BTNFACE));
 m_meter.SetUnit(MLL::LoadString(IDS_MI_SENSAFR_UNIT));
 m_meter.SetTickNumber(18);
 m_meter.AddAlertZone(6.0,10.0,RGB(255,130,130));
 m_meter.AddAlertZone(10.0,14.55,RGB(240,240,130));
 m_meter.AddAlertZone(14.55,14.85,RGB(130,255,130));
 m_meter.AddAlertZone(14.85,15.45,RGB(240,240,120));
 m_meter.AddAlertZone(15.45,15.75,RGB(130,255,130));
 m_meter.AddAlertZone(15.75,20.0,RGB(240,240,130));
 m_meter.AddAlertZone(20.0,24.0,RGB(255,130,130));
 m_meter.SetNeedleValue(0.0);
 m_meter.Update();
 m_meter.SetMeterSize(130);
}
