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

/** \file MIKnockGraph.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "resource.h"
#include "MIKnockGraph.h"
#include "common/GDIHelpers.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMIKnock::CMIKnock()
{
 //empty
}

CMIKnock::~CMIKnock()
{
 //empty
}

void CMIKnock::Create(CWnd* pParent)
{
 MeasInstrBase::Create(pParent, IDC_MI_KNOCK); //create window

 m_meter.SetRange (0.0, 5.0);
 m_meter.SetLabelsDecimals(1);
 m_meter.SetValueDecimals(3);
 m_meter.SetTitle(MLL::LoadString(IDS_MI_KNOCK_TITLE));
 m_meter.SetColor(meter_value,RGB(10,80,255));
 m_meter.SetColor(meter_bground, GetSysColor(COLOR_BTNFACE));
 m_meter.SetColor(meter_labels, GDIHelpers::InvColor(GetSysColor(COLOR_BTNFACE)));
 m_meter.SetUnit(MLL::LoadString(IDS_MI_VOLTAGE_UNIT));
 m_meter.SetTickNumber(21);
 m_meter.AddAlertZone(0.0,0.5,RGB(40 , 80,220));
 m_meter.AddAlertZone(0.5,1.0,RGB(60 ,100,180));
 m_meter.AddAlertZone(1.0,1.5,RGB(80 ,120,160));
 m_meter.AddAlertZone(1.5,2.0,RGB(100,140,140));
 m_meter.AddAlertZone(2.0,2.5,RGB(120,160,120));
 m_meter.AddAlertZone(2.5,3.0,RGB(140,180,100));
 m_meter.AddAlertZone(3.0,3.5,RGB(160,160,100));
 m_meter.AddAlertZone(3.5,4.0,RGB(180,140,100));
 m_meter.AddAlertZone(4.0,4.5,RGB(200,120,100));
 m_meter.AddAlertZone(4.5,5.0,RGB(230,100,100));
 m_meter.SetNeedleValue(0.0);
 m_meter.Update();
 m_meter.SetMeterSize(130);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMIKnockGraph::CMIKnockGraph()
{
 //empty
}

CMIKnockGraph::~CMIKnockGraph()
{
 //empty
}

void CMIKnockGraph::Create(CWnd* pParent)
{
 // create the window of control
 CRect rect(0,0, 100,100);
 VERIFY(m_scope.Create(WS_VISIBLE | WS_CHILD, rect, pParent, IDC_MI_KNOCKGRAPH));

 // customize the control
 m_scope.SetRange(0, 5.0, 1, 2);
 m_scope.ReserveCharsY(5);
 m_scope.SetUnitY(MLL::GetString(IDS_MI_KNOCKGRAPH_V_UNIT));
 m_scope.SetUnitX(MLL::GetString(IDS_MI_KNOCKGRAPH_H_UNIT));
 m_scope.SetBackgroundColor(RGB(0, 64, 0));
 m_scope.SetGridColor(RGB(192, 192, 255));
 m_scope.SetPlotColor(RGB(255, 255, 255));
}
