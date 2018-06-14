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

/** \file MIPressure.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "resource.h"
#include "MIPressure.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMIPressure::CMIPressure()
{
 //empty
}

CMIPressure::~CMIPressure()
{
 //empty
}

void CMIPressure::Create(CWnd* pParent)
{
 MeasInstrBase::Create(pParent, IDC_MI_MAP); //create window

 m_meter.SetRange (10.0, 110.0);
 m_meter.SetLabelsDecimals(0);
 m_meter.SetValueDecimals(0);
 m_meter.SetTitle(MLL::LoadString(IDS_MI_PRESSURE_TITLE));
 m_meter.SetFontScale(80);
 m_meter.SetColor(meter_value,RGB(10,80,255));
 m_meter.SetColor(meter_bground, GetSysColor(COLOR_BTNFACE));
 m_meter.SetUnit(MLL::LoadString(IDS_MI_PRESSURE_UNIT));
 m_meter.SetTickNumber(20);
 m_meter.AddAlertZone(10,30,RGB(180,130,130));
 m_meter.AddAlertZone(30,90,RGB(120,200,120));
 m_meter.AddAlertZone(90,110,RGB(180,130,130));
 m_meter.SetNeedleValue(0.0);
 m_meter.Update();
}

void CMIPressure::SetLimits(float loLimit, float upLimit)
{
 m_meter.ResetAlertZones();
 m_meter.AddAlertZone(loLimit, upLimit * 0.2727, RGB(180,130,130));
 m_meter.AddAlertZone(upLimit * 0.2727, upLimit * 0.8181, RGB(120,200,120));
 m_meter.AddAlertZone(upLimit * 0.8181, upLimit, RGB(180,130,130));

 m_meter.SetRange(loLimit, upLimit);
}
