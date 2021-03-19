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
#include "common/GDIHelpers.h"


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
 m_meter.SetValueDecimals(2);
 m_meter.SetTitle(MLL::LoadString(IDS_MI_PRESSURE_TITLE));
 m_meter.SetColor(meter_value,RGB(10,80,255));
 m_meter.SetColor(meter_bground, GetSysColor(COLOR_BTNFACE));
 m_meter.SetColor(meter_labels, GDIHelpers::InvColor(GetSysColor(COLOR_BTNFACE)));
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

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMIPressureGraph::CMIPressureGraph()
{
 //empty
}

CMIPressureGraph::~CMIPressureGraph()
{
 //empty
}

void CMIPressureGraph::Create(CWnd* pParent)
{
 // create the window of control
 CRect rect(0,0, 100,100);
 VERIFY(m_scope.Create(WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS, rect, pParent, IDC_MI_PRESSURE_GRAPH));

 // customize the control
 m_scope.SetRange(0, 110.0, 0, 1);
 m_scope.SetGridNumberY(10);
 m_scope.ReserveCharsY(5);
 m_scope.SetUnitY(MLL::GetString(IDS_MI_PRESSURE_V_UNIT));
 m_scope.SetUnitX(MLL::GetString(IDS_MI_KNOCKGRAPH_H_UNIT));
 m_scope.SetBackgroundColor(RGB(0, 64, 0));
 m_scope.SetGridColor(RGB(192, 192, 255));
 m_scope.SetPlotColor(RGB(255, 255, 255));
}

void CMIPressureGraph::SetLimits(float loLimit, float upLimit)
{
 m_scope.SetGridNumberY(10);
 m_scope.SetRange(loLimit, upLimit, 0, 1);
}
