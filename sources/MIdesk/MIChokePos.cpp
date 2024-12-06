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

/** \file MIChokePos.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "resource.h"
#include "MIChokePos.h"
#include "common/GDIHelpers.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMIChokePosBase::CMIChokePosBase()
{
 //empty
}

CMIChokePosBase::~CMIChokePosBase()
{
 //empty
}

void CMIChokePosBase::Create(CWnd* pParent, UINT id)
{
 MeasInstrBase::Create(pParent, id); //create window

 m_meter.SetRange (0.0, 100.0);
 m_meter.SetLabelsDecimals(0);
 m_meter.SetValueDecimals(1);
 m_meter.SetColor(meter_value,RGB(10,80,255));
 m_meter.SetColor(meter_bground, GetSysColor(COLOR_BTNFACE));
 m_meter.SetColor(meter_labels, GDIHelpers::InvColor(GetSysColor(COLOR_BTNFACE)));
 m_meter.SetUnit(MLL::LoadString(IDS_MI_PERCENT_UNIT));
 m_meter.SetTickNumber(20);
 m_meter.AddAlertZone(0.0,100.0,RGB(0,230,250));
 m_meter.SetNeedleValue(0.0);
 m_meter.Update();
 m_meter.SetMeterSize(130);
}

void CMIChokePosBase::SetLimits(float loLimit, float upLimit)
{
 m_meter.SetRange(loLimit, upLimit, true); //<-- also update alert zones
}

void CMIChokePos::Create(CWnd* pParent)
{
 CMIChokePosBase::Create(pParent, IDC_MI_CHOKEPOS);
 m_meter.SetTitle(MLL::GetString(IDS_MI_CHOKEPOS_TITLE).c_str());
}

void CMIGDPos::Create(CWnd* pParent)
{
 CMIChokePosBase::Create(pParent, IDC_MI_GDPOS);
 m_meter.SetTitle(MLL::GetString(IDS_MI_GDPOS_TITLE).c_str());
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMIChokePosBaseGraph::CMIChokePosBaseGraph()
{
 //empty
}

CMIChokePosBaseGraph::~CMIChokePosBaseGraph()
{
 //empty
}

void CMIChokePosBaseGraph::Create(CWnd* pParent, UINT id)
{
 // create the window of control
 CRect rect(0,0, 100,100);
 VERIFY(m_scope.Create(WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS, rect, pParent, id));

 // customize the control
 m_scope.SetRange(0, 100);
 m_scope.SetDecPlaces(0, 1);
 m_scope.SetGridNumberY(10);
 m_scope.ReserveCharsY(5);
 m_scope.SetUnitX(MLL::GetString(IDS_MI_KNOCKGRAPH_H_UNIT));
 m_scope.SetBackgroundColor(RGB(0, 64, 0));
 m_scope.SetGridColor(RGB(192, 192, 255));
 m_scope.SetPlotColor(RGB(255, 255, 255));
}

void CMIChokePosGraph::Create(CWnd* pParent)
{
 CMIChokePosBaseGraph::Create(pParent, IDC_MI_CHOKEPOSGRAPH);
 m_scope.SetUnitY(MLL::GetString(IDS_MI_CHOKEPOSGRAPH_V_UNIT));
}

void CMIGDPosGraph::Create(CWnd* pParent)
{
 CMIChokePosBaseGraph::Create(pParent, IDC_MI_GDPOSGRAPH);
 m_scope.SetUnitY(MLL::GetString(IDS_MI_GDPOSGRAPH_V_UNIT));
}
