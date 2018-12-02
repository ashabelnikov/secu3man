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

/** \file MIEGOCorr.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "resource.h"
#include "MIEGOCorr.h"
#include "common/GDIHelpers.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMIEGOCorr::CMIEGOCorr()
{
 //empty
}

CMIEGOCorr::~CMIEGOCorr()
{
 //empty
}

void CMIEGOCorr::Create(CWnd* pParent)
{
 MeasInstrBase::Create(pParent, IDC_MI_EGO_CORR); //create window

 m_meter.SetRange (-50.0, 50.0);
 m_meter.SetLabelsDecimals(1);
 m_meter.SetValueDecimals(1);
 m_meter.SetTitle(MLL::LoadString(IDS_MI_EGO_CORR_TITLE));
 m_meter.SetColor(meter_value,RGB(10,80,255));
 m_meter.SetColor(meter_bground, GetSysColor(COLOR_BTNFACE));
 m_meter.SetColor(meter_labels, GDIHelpers::InvColor(GetSysColor(COLOR_BTNFACE)));
 m_meter.SetUnit(MLL::LoadString(IDS_MI_PERCENT_UNIT));
 m_meter.SetTickNumber(16);
 m_meter.AddAlertZone(-50,-20,RGB(230,130,130));
 m_meter.AddAlertZone(-20,20,RGB(120,190,120));
 m_meter.AddAlertZone(20,50,RGB(230,130,130));
 m_meter.SetTRPane(_T("n/a"));
 m_meter.SetNeedleValue(0.0);
 m_meter.Update();
}
