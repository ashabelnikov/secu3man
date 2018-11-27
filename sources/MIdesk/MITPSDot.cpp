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

/** \file MITPSDot.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "resource.h"
#include "MITPSDot.h"

CMITPSDot::CMITPSDot()
{
 //empty
}

CMITPSDot::~CMITPSDot()
{
 //empty
}

void CMITPSDot::Create(CWnd* pParent)
{
 MeasInstrBase::Create(pParent, IDC_MI_TPSDOT); //create window

 m_tlpFmt = _T("%0.0f ");

 m_meter.SetRange (-500.0, 500.0);
 m_meter.SetLabelsDecimals(0);
 m_meter.SetValueDecimals(0);
 m_meter.SetTitle(MLL::LoadString(IDS_MI_TPSDOT_TITLE));
 m_meter.SetColor(meter_value,RGB(10,80,255));
 m_meter.SetColor(meter_bground, GetSysColor(COLOR_BTNFACE));
 m_meter.SetUnit(MLL::LoadString(IDS_MI_TPSDOT_UNIT));
 m_meter.SetTickNumber(10);
 m_meter.AddAlertZone(-500,-200,RGB(250,100,100));
 m_meter.AddAlertZone(-200,-50,RGB(100,250,100));
 m_meter.AddAlertZone(-50,0,RGB(100,100,250));
 m_meter.AddAlertZone(0,50,RGB(100,100,250));
 m_meter.AddAlertZone(50,200,RGB(100,250,100));
 m_meter.AddAlertZone(200,500,RGB(250,100,100));
 m_meter.SetNeedleValue(0.0);
 m_meter.Update();
}
