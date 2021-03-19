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

static void _GetITModeRange(int mode, float& y1, float& y2)
{
 y1 = (mode < 2) ? .0f : -360.0f;
 y2 = (mode < 2) ? 720.0f : 360.0f;
}

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

void CMIInjTim::SetITMode(int mode)
{
 float y1, y2;
 _GetITModeRange(mode, y1, y2);
 SetLimits(y1, y2);
 m_meter.ResetAlertZones();
 m_meter.AddAlertZone(y1, y2, RGB(250,250,250));
 m_meter.Update();
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

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMIInjTimGraph::CMIInjTimGraph()
{
 //empty
}

CMIInjTimGraph::~CMIInjTimGraph()
{
 //empty
}

void CMIInjTimGraph::Create(CWnd* pParent, UINT id)
{
 // create the window of control
 CRect rect(0,0, 100,100);
 VERIFY(m_scope.Create(WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS, rect, pParent, id));

 // customize the control
 m_scope.SetRange(0, 720, 0, 1);
 m_scope.SetGridNumberY(10);
 m_scope.ReserveCharsY(5);
 m_scope.SetUnitX(MLL::GetString(IDS_MI_KNOCKGRAPH_H_UNIT));
 m_scope.SetBackgroundColor(RGB(0, 64, 0));
 m_scope.SetGridColor(RGB(192, 192, 255));
 m_scope.SetPlotColor(RGB(255, 255, 255));
}

void CMIInjTimGraph::SetITMode(int mode)
{
 float y1, y2;
 _GetITModeRange(mode, y1, y2);
 m_scope.SetRange(y1, y2, 0, 1);
}

void CMIInjTimBGraph::Create(CWnd* pParent)
{
 CMIInjTimGraph::Create(pParent, IDC_MI_INJTIMBGRAPH);
 m_scope.SetUnitY(MLL::GetString(IDS_MI_INJTIMBGRAPH_V_UNIT));
}

void CMIInjTimEGraph::Create(CWnd* pParent)
{
 CMIInjTimGraph::Create(pParent, IDC_MI_INJTIMEGRAPH);
 m_scope.SetUnitY(MLL::GetString(IDS_MI_INJTIMEGRAPH_V_UNIT));
}
