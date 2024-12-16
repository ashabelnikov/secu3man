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

/** \file MIMAPDot.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "resource.h"
#include "MIMAPDot.h"
#include "common/GDIHelpers.h"

CMIMAPDot::CMIMAPDot()
{
 //empty
}

CMIMAPDot::~CMIMAPDot()
{
 //empty
}

void CMIMAPDot::Create(CWnd* pParent)
{
 MeasInstrBase::Create(pParent, IDC_MI_MAPDOT); //create window

 m_tlpFmt = _T("%0.0f ");

 m_meter.SetRange (-500.0, 500.0);
 m_meter.SetLabelsDecimals(0);
 m_meter.SetValueDecimals(0);
 m_meter.SetTitle(MLL::LoadString(IDS_MI_MAPDOT_TITLE));
 m_meter.SetColor(meter_value,RGB(10,80,255));
 m_meter.SetColor(meter_bground, GetSysColor(COLOR_BTNFACE));
 m_meter.SetColor(meter_labels, GDIHelpers::InvColor(GetSysColor(COLOR_BTNFACE)));
 m_meter.SetUnit(MLL::LoadString(IDS_MI_MAPDOT_UNIT));
 m_meter.SetTickNumber(10);
 m_meter.AddAlertZone(-500,-200,RGB(250,100,100));
 m_meter.AddAlertZone(-200,-50,RGB(100,250,100));
 m_meter.AddAlertZone(-50,0,RGB(100,100,250));
 m_meter.AddAlertZone(0,50,RGB(100,100,250));
 m_meter.AddAlertZone(50,200,RGB(100,250,100));
 m_meter.AddAlertZone(200,500,RGB(250,100,100));
 m_meter.SetNeedleValue(0.0);
 m_meter.Update();
}

void CMIMAPDot::SetLimits(float loLimit, float upLimit)
{
 m_meter.SetRange(loLimit, upLimit, true); //<-- also update alert zones
}

void CMIMAPDot::Append(const SECU3IO::SensorDat* i_values, bool i_revdir /*= false*/)
{
 MeasInstrBase::Append((float)i_values->mapdot, 0, 0, i_revdir);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMIMAPDotGraph::CMIMAPDotGraph()
{
 //empty
}

CMIMAPDotGraph::~CMIMAPDotGraph()
{
 //empty
}

void CMIMAPDotGraph::Create(CWnd* pParent)
{
 // create the window of control
 CRect rect(0,0, 100,100);
 VERIFY(m_scope.Create(WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS, rect, pParent, IDC_MI_MAPDOTGRAPH));

 // customize the control
 m_scope.SetRange(-500, 500);
 m_scope.SetDecPlaces(0, 0);
 m_scope.SetGridNumberY(10);
 m_scope.ReserveCharsY(5);
 m_scope.SetUnitY(MLL::GetString(IDS_MI_MAPDOTGRAPH_V_UNIT));
 m_scope.SetUnitX(MLL::GetString(IDS_MI_KNOCKGRAPH_H_UNIT));
 m_scope.SetBackgroundColor(RGB(0, 64, 0));
 m_scope.SetGridColor(RGB(192, 192, 255));
 m_scope.SetPlotColor(RGB(255, 255, 255));
}

void CMIMAPDotGraph::Append(const SECU3IO::SensorDat* i_values, bool i_revdir /*= false*/)
{
 MeasInstrBase::Append((float)i_values->mapdot, i_revdir);
}
