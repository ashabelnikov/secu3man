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

/** \file MIOps.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "resource.h"
#include "MIOps.h"
#include "common/GDIHelpers.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMIOps::CMIOps()
{
 //empty
}

CMIOps::~CMIOps()
{
 //empty
}

void CMIOps::Create(CWnd* pParent)
{
 MeasInstrBase::Create(pParent, IDC_MI_OPS); //create window
 
 float loLimit = 0.0f; float upLimit = 6.0f;
 m_meter.SetRange (loLimit, upLimit);
 m_meter.SetLabelsDecimals(1);
 m_meter.SetValueDecimals(2);
 m_meter.SetTitle(MLL::LoadString(IDS_MI_OPS_TITLE));
 m_meter.SetColor(meter_value,RGB(10,80,255));
 m_meter.SetColor(meter_bground, GetSysColor(COLOR_BTNFACE));
 m_meter.SetColor(meter_labels, GDIHelpers::InvColor(GetSysColor(COLOR_BTNFACE)));
 m_meter.SetUnit(MLL::LoadString(IDS_MI_OPS_UNIT));
 m_meter.SetTickNumber(12);
 m_meter.AddAlertZone(loLimit, upLimit * 0.17, RGB(130,130,190));
 m_meter.AddAlertZone(upLimit * 0.17, upLimit * 0.59, RGB(120,240,120));
 m_meter.AddAlertZone(upLimit * 0.59, upLimit * 0.83, RGB(240,240,120));
 m_meter.AddAlertZone(upLimit * 0.83, upLimit, RGB(255,130,130));
 m_meter.SetNeedleValue(0.0);
 m_meter.Update();
}

void CMIOps::SetLimits(float loLimit, float upLimit)
{
 m_meter.SetRange(loLimit, upLimit, true); //<-- also update alert zones
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMIOpsGraph::CMIOpsGraph()
{
 //empty
}

CMIOpsGraph::~CMIOpsGraph()
{
 //empty
}

void CMIOpsGraph::Create(CWnd* pParent)
{
 // create the window of control
 CRect rect(0,0, 100,100);
 VERIFY(m_scope.Create(WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS, rect, pParent, IDC_MI_OPSGRAPH));

 // customize the control
 m_scope.SetRange(0.0, 6.0);
 m_scope.SetDecPlaces(0, 1);
 m_scope.SetGridNumberY(8);
 m_scope.ReserveCharsY(5);
 m_scope.SetUnitY(MLL::GetString(IDS_MI_OPSGRAPH_V_UNIT));
 m_scope.SetUnitX(MLL::GetString(IDS_MI_KNOCKGRAPH_H_UNIT));
 m_scope.SetBackgroundColor(RGB(0, 64, 0));
 m_scope.SetGridColor(RGB(192, 192, 255));
 m_scope.SetPlotColor(RGB(255, 255, 255));
}

void CMIOpsGraph::SetLimits(float loLimit, float upLimit)
{
 m_scope.SetGridNumberY(8);
 m_scope.SetRange(loLimit, upLimit);
}
