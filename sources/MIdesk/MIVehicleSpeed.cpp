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

/** \file MIVehicleSpeed.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "resource.h"
#include "MIVehicleSpeed.h"
#include "common/GDIHelpers.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMIVehicleSpeed::CMIVehicleSpeed()
{
 //empty
}

CMIVehicleSpeed::~CMIVehicleSpeed()
{
 //empty
}

void CMIVehicleSpeed::Create(CWnd* pParent)
{
 MeasInstrBase::Create(pParent, IDC_MI_VSS); //create window

 m_meter.SetRange (0.0, 220.0);
 m_meter.SetLabelsDecimals(0);
 m_meter.SetValueDecimals(1);
 m_meter.SetTitle(MLL::LoadString(IDS_MI_VSS_TITLE));
 m_meter.SetColor(meter_value,RGB(10,80,255));
 m_meter.SetColor(meter_bground, GetSysColor(COLOR_BTNFACE));
 m_meter.SetColor(meter_labels, GDIHelpers::InvColor(GetSysColor(COLOR_BTNFACE)));
 m_meter.SetTickNumber(22);
 m_meter.AddAlertZone(0.0,60.0,RGB(100,255,100));
 m_meter.AddAlertZone(60.0,120.0,RGB(250,230,100));
 m_meter.AddAlertZone(120.0,180.0,RGB(230,130,100));
 m_meter.AddAlertZone(180.0,220.0,RGB(255,120,120));
 m_meter.SetNeedleValue(0.0);
 m_meter.Update();
 m_meter.SetMeterSize(130);
}




//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMIVehicleSpeedGraph::CMIVehicleSpeedGraph()
{
 //empty
}

CMIVehicleSpeedGraph::~CMIVehicleSpeedGraph()
{
 //empty
}

void CMIVehicleSpeedGraph::Create(CWnd* pParent)
{
 // create the window of control
 CRect rect(0,0, 100,100);
 VERIFY(m_scope.Create(WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS, rect, pParent, IDC_MI_VSSGRAPH));

 // customize the control
 m_scope.SetRange(0.0, 220, 0, 1);
 m_scope.SetGridNumberY(10);
 m_scope.ReserveCharsY(5);
 m_scope.SetUnitY(MLL::GetString(IDS_MI_VSSGRAPH_V_UNIT));
 m_scope.SetUnitX(MLL::GetString(IDS_MI_KNOCKGRAPH_H_UNIT));
 m_scope.SetBackgroundColor(RGB(0, 64, 0));
 m_scope.SetGridColor(RGB(192, 192, 255));
 m_scope.SetPlotColor(RGB(255, 255, 255));
}
