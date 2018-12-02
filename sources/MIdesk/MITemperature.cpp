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

/** \file MITemperature.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "resource.h"
#include "MITemperature.h"
#include "common/GDIHelpers.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMITemperature::CMITemperature()
{
 //empty
}

CMITemperature::~CMITemperature()
{
 //empty
}

void CMITemperature::Create(CWnd* pParent)
{
 MeasInstrBase::Create(pParent, IDC_MI_TEMPERATURE); //create window
 
 m_tlpFmt = _T("%0.1f ");
 m_trpFmt = _T("%0.1f ");

 m_tlpUnit = _T("%%");
 m_trpUnit = _T("%%");

 m_meter.SetRange (-40.0, 120.0) ;
 m_meter.SetLabelsDecimals(1) ;
 m_meter.SetValueDecimals(1) ;
 m_meter.SetTitle(MLL::LoadString(IDS_MI_TEMPERATURE_TITLE)) ;
 m_meter.SetColor(meter_value,RGB(10,80,255));
 m_meter.SetColor(meter_bground, GetSysColor(COLOR_BTNFACE));
 m_meter.SetColor(meter_labels, GDIHelpers::InvColor(GetSysColor(COLOR_BTNFACE)));
 m_meter.SetUnit(MLL::LoadString(IDS_MI_TEMPERATURE_UNIT));
 m_meter.SetTickNumber(16);
 m_meter.AddAlertZone(-40,50,RGB(130,130,180));
 m_meter.AddAlertZone(50,100,RGB(120,120,120));
 m_meter.AddAlertZone(100,120,RGB(230,130,130));
 m_meter.SetTRPane(_T("n/a"));
 m_meter.SetTLPane(_T("n/a"));
 m_meter.SetNeedleValue(-40.0);
 m_meter.Update();
}
