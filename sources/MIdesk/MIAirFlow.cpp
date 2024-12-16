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

/** \file MIAirFlow.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "resource.h"
#include "MIAirFlow.h"
#include "common/GDIHelpers.h"

CMIAirFlow::CMIAirFlow()
{
 //empty
}

CMIAirFlow::~CMIAirFlow()
{
 //empty
}

void CMIAirFlow::Create(CWnd* pParent)
{
 MeasInstrBase::Create(pParent, IDC_MI_AIR_FLOW, false, false); //create window

 m_meter.SetRange (0.0, 16.0);
 m_meter.SetLabelsDecimals(0);
 m_meter.SetValueDecimals(0);
 m_meter.SetTitle(MLL::LoadString(IDS_MI_AIR_FLOW_TITLE));
 m_meter.SetColor(meter_value,RGB(10,80,255));
 m_meter.SetColor(meter_bground, GetSysColor(COLOR_BTNFACE));
 m_meter.SetColor(meter_labels, GDIHelpers::InvColor(GetSysColor(COLOR_BTNFACE)));
 m_meter.SetUnit(MLL::LoadString(IDS_MI_AIR_FLOW_UNIT));
 m_meter.SetTickNumber(16);
 m_meter.AddAlertZone(0,1,RGB(190,150,150));
 m_meter.AddAlertZone(1,2,RGB(180,150,160));
 m_meter.AddAlertZone(2,3,RGB(170,150,170));
 m_meter.AddAlertZone(3,4,RGB(160,150,180));
 m_meter.AddAlertZone(4,5,RGB(150,150,190));
 m_meter.AddAlertZone(5,6,RGB(150,150,200));
 m_meter.AddAlertZone(6,7,RGB(150,150,210));
 m_meter.AddAlertZone(7,8,RGB(150,150,220));
 m_meter.AddAlertZone(8,9,RGB(150,150,230));
 m_meter.AddAlertZone(9,10,RGB(150,150,230));
 m_meter.AddAlertZone(10,11,RGB(150,150,240));
 m_meter.AddAlertZone(11,12,RGB(150,150,250));
 m_meter.AddAlertZone(12,13,RGB(150,150,255));
 m_meter.AddAlertZone(13,14,RGB(150,150,255));
 m_meter.AddAlertZone(14,15,RGB(150,150,255));
 m_meter.AddAlertZone(15,16,RGB(150,150,255));
 m_meter.SetNeedleValue(0.0);
 m_meter.Update();
}

void CMIAirFlow::SetLimits(float loLimit, float upLimit)
{
 m_meter.SetRange(loLimit, upLimit, true); //<-- also update alert zones
}

void CMIAirFlow::Append(const SECU3IO::SensorDat* i_values, bool i_revdir /*= false*/)
{
 MeasInstrBase::Append(i_values->air_flow, 0, 0, i_revdir);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMIAirFlowGraph::CMIAirFlowGraph()
{
 //empty
}

CMIAirFlowGraph::~CMIAirFlowGraph()
{
 //empty
}

void CMIAirFlowGraph::Create(CWnd* pParent)
{
 // create the window of control
 MeasInstrBase::Create(pParent, IDC_MI_AIR_FLOWGRAPH);

 // customize the control
 m_scope.SetRange(0, 16);
 m_scope.SetDecPlaces(0, 0);
 m_scope.SetGridNumberY(16);
 m_scope.ReserveCharsY(5);
 m_scope.SetUnitY(MLL::GetString(IDS_MI_AIR_FLOWGRAPH_V_UNIT));
 m_scope.SetUnitX(MLL::GetString(IDS_MI_KNOCKGRAPH_H_UNIT));
 m_scope.SetBackgroundColor(RGB(0, 64, 0));
 m_scope.SetGridColor(RGB(192, 192, 255));
 m_scope.SetPlotColor(RGB(255, 255, 255));
}

void CMIAirFlowGraph::Append(const SECU3IO::SensorDat* i_values, bool i_revdir /*= false*/)
{
 MeasInstrBase::Append(i_values->air_flow, i_revdir);
}
