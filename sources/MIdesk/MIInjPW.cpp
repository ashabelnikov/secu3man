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

/** \file MIInjPW.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "resource.h"
#include "MIInjPW.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMIInjPW::CMIInjPW()
{
 //empty
}

CMIInjPW::~CMIInjPW()
{
 //empty
}

void CMIInjPW::Create(CWnd* pParent)
{
 MeasInstrBase::Create(pParent, IDC_MI_INJ_PW); //create window

 m_meter.SetRange (0, 24.00);
 m_meter.SetLabelsDecimals(0);
 m_meter.SetValueDecimals(2);
 m_meter.SetTitle(MLL::LoadString(IDS_MI_INJ_PW_TITLE));
 m_meter.SetFontScale(80);
 m_meter.SetColor(meter_value,RGB(10,80,255));
 m_meter.SetColor(meter_bground, GetSysColor(COLOR_BTNFACE));
 m_meter.SetUnit(MLL::LoadString(IDS_MI_MS_UNIT));
 m_meter.SetTickNumber(24);
 m_meter.AddAlertZone(0,1.0,RGB(255,100,100));
 m_meter.AddAlertZone(1.0,10.0,RGB(100,255,100));
 m_meter.AddAlertZone(10.0,24,RGB(150,150,250));
 m_meter.SetNeedleValue(0.0);
 m_meter.Update();
}

void CMIInjPW::SetLimits(float loLimit, float upLimit)
{
 m_meter.ResetAlertZones();
 m_meter.AddAlertZone(loLimit, upLimit * 0.5, RGB(100,255,100));
 m_meter.AddAlertZone(upLimit * 0.5, upLimit * 0.75, RGB(255,255,100));
 m_meter.AddAlertZone(upLimit * 0.75, upLimit, RGB(255,100,100));

 m_meter.SetRange(loLimit, upLimit);
}
