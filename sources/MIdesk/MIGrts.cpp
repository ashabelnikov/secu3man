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

/** \file MIGrts.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "resource.h"
#include "MIGrts.h"
#include "common/GDIHelpers.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMIGrts::CMIGrts()
{
 //empty
}

CMIGrts::~CMIGrts()
{
 //empty
}

void CMIGrts::Create(CWnd* pParent)
{
 MeasInstrBase::Create(pParent, IDC_MI_GRTS); //create window

 m_meter.SetRange (-40.0, 120.0);
 m_meter.SetLabelsDecimals(1);
 m_meter.SetValueDecimals(1);
 m_meter.SetTitle(MLL::LoadString(IDS_MI_GRTS_TITLE));
 m_meter.SetColor(meter_value,RGB(10,80,255));
 m_meter.SetColor(meter_bground, GetSysColor(COLOR_BTNFACE));
 m_meter.SetColor(meter_labels, GDIHelpers::InvColor(GetSysColor(COLOR_BTNFACE)));
 m_meter.SetUnit(MLL::LoadString(IDS_MI_TEMPERATURE_UNIT));
 m_meter.SetTickNumber(16);
 m_meter.AddAlertZone(-40,50,RGB(130,130,180));
 m_meter.AddAlertZone(50,100,RGB(120,120,120));
 m_meter.AddAlertZone(100,120,RGB(230,130,130));
 m_meter.SetTRPane(_T("n/a"));
 m_meter.SetNeedleValue(-40.0);
 m_meter.Update();
}

void CMIGrts::SetLimits(float loLimit, float upLimit)
{
 m_meter.SetRange(loLimit, upLimit, true); //<-- also update alert zones
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMIGrtsGraph::CMIGrtsGraph()
{
 //empty
}

CMIGrtsGraph::~CMIGrtsGraph()
{
 //empty
}

void CMIGrtsGraph::Create(CWnd* pParent)
{
 // create the window of control
 CRect rect(0,0, 100,100);
 VERIFY(m_scope.Create(WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS, rect, pParent, IDC_MI_GRTSGRAPH));

 // customize the control
 m_scope.SetRange(-40.0, 120);
 m_scope.SetDecPlaces(0, 1);
 m_scope.SetGridNumberY(8);
 m_scope.ReserveCharsY(5);
 m_scope.SetUnitY(MLL::GetString(IDS_MI_GRTSGRAPH_V_UNIT));
 m_scope.SetUnitX(MLL::GetString(IDS_MI_KNOCKGRAPH_H_UNIT));
 m_scope.SetBackgroundColor(RGB(0, 64, 0));
 m_scope.SetGridColor(RGB(192, 192, 255));
 m_scope.SetPlotColor(RGB(255, 255, 255));
}

void CMIGrtsGraph::SetLimits(float loLimit, float upLimit)
{
 m_scope.SetGridNumberY(8);
 m_scope.SetRange(loLimit, upLimit);
}
