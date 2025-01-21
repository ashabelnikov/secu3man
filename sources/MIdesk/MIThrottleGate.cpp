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

/** \file MIThrottleGate.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "MIThrottleGate.h"
#include "resource.h"
#include "common/GDIHelpers.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMIThrottleGate::CMIThrottleGate()
{
 //empty
}

CMIThrottleGate::~CMIThrottleGate()
{
 //empty
}

void CMIThrottleGate::Create(CWnd* pParent)
{
 MeasInstrBase::Create(pParent, IDC_MI_TPS, true, false); //create window

 m_tlpFmt = _T("%0.0f ");

 m_meter.SetRange (.0, 100.0);
 m_meter.SetLabelsDecimals(1);
 m_meter.SetValueDecimals(1);
 m_meter.SetTitle(MLL::LoadString(IDS_MI_THROTTLE_GATE_TITLE));
 m_meter.SetColor(meter_value,RGB(10,80,255));
 m_meter.SetColor(meter_bground, GetSysColor(COLOR_BTNFACE));
 m_meter.SetColor(meter_labels, GDIHelpers::InvColor(GetSysColor(COLOR_BTNFACE)));
 m_meter.SetUnit(MLL::LoadString(IDS_MI_PERCENT_UNIT));
 m_meter.SetTickNumber(16);
 m_meter.AddAlertZone(0,5,RGB(130,130,180));
 m_meter.AddAlertZone(5,75,RGB(120,180,150));
 m_meter.AddAlertZone(75,100,RGB(230,130,130));
 m_meter.SetTRPane(_T("n/a"));
 m_meter.SetTLPane(_T("n/a"));
 m_meter.SetNeedleValue(0.0);
 m_meter.Update();
}

void CMIThrottleGate::Append(const SECU3IO::SensorDat* i_values, bool i_revdir /*= false*/)
{
 MeasInstrBase::Append(i_values->tps, i_values->air_flow, 0, i_revdir);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMIThrottleGateGraph::CMIThrottleGateGraph()
{
 //empty
}

CMIThrottleGateGraph::~CMIThrottleGateGraph()
{
 //empty
}

void CMIThrottleGateGraph::Create(CWnd* pParent)
{
 // create the window of control
 MeasInstrBase::Create(pParent, IDC_MI_THROTTLEGATEGRAPH);

 // customize the control
 m_scope.SetRange(0.0, 100.0);
 m_scope.SetDecPlaces(0, 1);
 m_scope.SetGridNumberY(10);
 m_scope.ReserveCharsY(5);
 m_scope.SetUnitY(MLL::GetString(IDS_MI_THROTTLEGATE_V_UNIT));
 m_scope.SetUnitX(MLL::GetString(IDS_MI_KNOCKGRAPH_H_UNIT));
 m_scope.SetBackgroundColor(RGB(0, 64, 0));
 m_scope.SetGridColor(RGB(192, 192, 255));
 m_scope.SetPlotColor(RGB(255, 255, 255));
}

void CMIThrottleGateGraph::Append(const SECU3IO::SensorDat* i_values, bool i_revdir /*= false*/)
{
 MeasInstrBase::Append(i_values->tps, i_revdir);
}
