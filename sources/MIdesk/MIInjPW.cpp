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

/** \file MIInjPW.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "resource.h"
#include "MIInjPW.h"
#include "common/GDIHelpers.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMIInjPW::CMIInjPW()
{
 //empty
}

CMIInjPW::~CMIInjPW()
{
 //empty
}

void CMIInjPW::Create(CWnd* pParent)
{
 MeasInstrBase::Create(pParent, IDC_MI_INJ_PW); //create window

 m_meter.SetRange (0, 24.00);
 m_meter.SetLabelsDecimals(0);
 m_meter.SetValueDecimals(2);
 m_meter.SetTitle(MLL::LoadString(IDS_MI_INJ_PW_TITLE));
 m_meter.SetColor(meter_value,RGB(10,80,255));
 m_meter.SetColor(meter_bground, GetSysColor(COLOR_BTNFACE));
 m_meter.SetColor(meter_labels, GDIHelpers::InvColor(GetSysColor(COLOR_BTNFACE)));
 m_meter.SetUnit(MLL::LoadString(IDS_MI_MS_UNIT));
 m_meter.SetTickNumber(24);
 m_meter.AddAlertZone(0,1.0,RGB(255,100,100));
 m_meter.AddAlertZone(1.0,10.0,RGB(100,255,100));
 m_meter.AddAlertZone(10.0,24,RGB(150,150,250));
 m_meter.SetNeedleValue(0.0);
 m_meter.Update();
}

void CMIInjPW::SetLimits(float loLimit, float upLimit)
{
 m_meter.ResetAlertZones();
 m_meter.AddAlertZone(loLimit, upLimit * (1.0/24), RGB(255,100,100));
 m_meter.AddAlertZone(upLimit * (1.0/24), upLimit * (10.0/24), RGB(100,255,100));
 m_meter.AddAlertZone(upLimit * (10.0/24), upLimit, RGB(150,150,250));
 m_meter.SetRange(loLimit, upLimit);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMIInjPWGraph::CMIInjPWGraph()
{
 //empty
}

CMIInjPWGraph::~CMIInjPWGraph()
{
 //empty
}

void CMIInjPWGraph::Create(CWnd* pParent)
{
 // create the window of control
 CRect rect(0,0, 100,100);
 VERIFY(m_scope.Create(WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS, rect, pParent, IDC_MI_INJ_PWGRAPH));

 // customize the control
 m_scope.SetRange(0, 24, 0, 2);
 m_scope.SetGridNumberY(8);
 m_scope.ReserveCharsY(5);
 m_scope.SetUnitY(MLL::GetString(IDS_MI_INJ_PW_V_UNIT));
 m_scope.SetUnitX(MLL::GetString(IDS_MI_KNOCKGRAPH_H_UNIT));
 m_scope.SetBackgroundColor(RGB(0, 64, 0));
 m_scope.SetGridColor(RGB(192, 192, 255));
 m_scope.SetPlotColor(RGB(255, 255, 255));
}

void CMIInjPWGraph::SetLimits(float loLimit, float upLimit)
{
 m_scope.SetGridNumberY(8);
 m_scope.SetRange(loLimit, upLimit, 0, 2);
}
