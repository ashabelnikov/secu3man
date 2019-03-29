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

/** \file MIInjTim.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "resource.h"
#include "MIInjTim.h"
#include "common/GDIHelpers.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMIInjTim::CMIInjTim()
{
 //empty
}

CMIInjTim::~CMIInjTim()
{
 //empty
}

void CMIInjTim::Create(CWnd* pParent, UINT id)
{
 MeasInstrBase::Create(pParent, id); //create window

 m_meter.SetRange (.0, 720.0);
 m_meter.SetLabelsDecimals(1);
 m_meter.SetValueDecimals(1);
 m_meter.SetColor(meter_value,RGB(10,80,255));
 m_meter.SetColor(meter_bground, GetSysColor(COLOR_BTNFACE));
 m_meter.SetColor(meter_labels, GDIHelpers::InvColor(GetSysColor(COLOR_BTNFACE)));
 m_meter.SetUnit(MLL::LoadString(IDS_MI_ADVANGLE_UNIT));
 m_meter.SetTickNumber(16);
 m_meter.AddAlertZone(0,720.0,RGB(250,250,250));
 m_meter.SetNeedleValue(.0);
 m_meter.Update();
 m_meter.SetMeterSize(145);
}


void CMIInjTimB::Create(CWnd* pParent)
{
 CMIInjTim::Create(pParent, IDC_MI_INJTIMB);
 m_meter.SetTitle(MLL::GetString(IDS_MI_INJTIMB_TITLE).c_str());
}

void CMIInjTimE::Create(CWnd* pParent)
{
 CMIInjTim::Create(pParent, IDC_MI_INJTIME);
 m_meter.SetTitle(MLL::GetString(IDS_MI_INJTIME_TITLE).c_str());
}
