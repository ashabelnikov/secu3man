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

/** \file MISynLoad.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "resource.h"
#include "MISynLoad.h"
#include "common/GDIHelpers.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMISynLoad::CMISynLoad()
{
 //empty
}

CMISynLoad::~CMISynLoad()
{
 //empty
}

void CMISynLoad::Create(CWnd* pParent)
{
 MeasInstrBase::Create(pParent, IDC_MI_LOAD); //create window

 m_meter.SetRange (0.0, 250.0);
 m_meter.SetLabelsDecimals(0);
 m_meter.SetValueDecimals(2);
 m_meter.SetTitle(MLL::LoadString(IDS_MI_LOAD_TITLE));
 m_meter.SetColor(meter_value,RGB(10,80,255));
 m_meter.SetColor(meter_bground, GetSysColor(COLOR_BTNFACE));
 m_meter.SetColor(meter_labels, GDIHelpers::InvColor(GetSysColor(COLOR_BTNFACE)));
// m_meter.SetUnit(MLL::LoadString(IDS_MI_???_UNIT));
 m_meter.SetTickNumber(20);
 m_meter.AddAlertZone(0,100,RGB(130,250,130));
 m_meter.AddAlertZone(100,250,RGB(250,130,130));
 m_meter.SetNeedleValue(0.0);
 m_meter.Update();
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMISynLoadGraph::CMISynLoadGraph()
{
 //empty
}

CMISynLoadGraph::~CMISynLoadGraph()
{
 //empty
}

void CMISynLoadGraph::Create(CWnd* pParent)
{
 // create the window of control
 CRect rect(0,0, 100,100);
 VERIFY(m_scope.Create(WS_VISIBLE | WS_CHILD, rect, pParent, IDC_MI_LOADGRAPH));

 // customize the control
 m_scope.SetRange(0, 250, 0, 1);
 m_scope.SetGridNumberY(10);
 m_scope.ReserveCharsY(5);
 m_scope.SetUnitY(MLL::GetString(IDS_MI_LOADGRAPH_V_UNIT));
 m_scope.SetUnitX(MLL::GetString(IDS_MI_KNOCKGRAPH_H_UNIT));
 m_scope.SetBackgroundColor(RGB(0, 64, 0));
 m_scope.SetGridColor(RGB(192, 192, 255));
 m_scope.SetPlotColor(RGB(255, 255, 255));
}
