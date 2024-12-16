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

/** \file MIVentDuty.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "resource.h"
#include "MIVentDuty.h"
#include "common/GDIHelpers.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMIVentDuty::CMIVentDuty()
{
 //empty
}

CMIVentDuty::~CMIVentDuty()
{
 //empty
}

void CMIVentDuty::Create(CWnd* pParent)
{
 MeasInstrBase::Create(pParent, IDC_MI_VENT_DUTY, false, false); //create window

 m_meter.SetRange (0, 100.00);
 m_meter.SetLabelsDecimals(0);
 m_meter.SetValueDecimals(1);
 m_meter.SetTitle(MLL::LoadString(IDS_MI_VENT_DUTY_TITLE));
 m_meter.SetColor(meter_value,RGB(10,80,255));
 m_meter.SetColor(meter_bground, GetSysColor(COLOR_BTNFACE));
 m_meter.SetColor(meter_labels, GDIHelpers::InvColor(GetSysColor(COLOR_BTNFACE)));
 m_meter.SetUnit(MLL::LoadString(IDS_MI_PERCENT_UNIT));
 m_meter.SetTickNumber(21);
 m_meter.AddAlertZone(0,10.0,RGB(100,100,255));
 m_meter.AddAlertZone(10.0,70.0,RGB(100,255,100));
 m_meter.AddAlertZone(70.0,90.0,RGB(250,255,100));
 m_meter.AddAlertZone(90.0,100.0,RGB(255,100,100));
 m_meter.SetNeedleValue(0.0);
 m_meter.Update();
}

void CMIVentDuty::SetLimits(float loLimit, float upLimit)
{
 m_meter.SetRange(loLimit, upLimit, true); //<-- also update alert zones
}

void CMIVentDuty::Append(const SECU3IO::SensorDat* i_values, bool i_revdir /*= false*/)
{
 MeasInstrBase::Append(i_values->vent_duty, 0, 0, i_revdir);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMIVentDutyGraph::CMIVentDutyGraph()
{
 //empty
}

CMIVentDutyGraph::~CMIVentDutyGraph()
{
 //empty
}

void CMIVentDutyGraph::Create(CWnd* pParent)
{
 // create the window of control
 MeasInstrBase::Create(pParent, IDC_MI_VENT_DUTYGRAPH);

 // customize the control
 m_scope.SetRange(0, 100);
 m_scope.SetDecPlaces(0, 2);
 m_scope.SetGridNumberY(8);
 m_scope.ReserveCharsY(5);
 m_scope.SetUnitY(MLL::GetString(IDS_MI_VENT_DUTY_V_UNIT));
 m_scope.SetUnitX(MLL::GetString(IDS_MI_KNOCKGRAPH_H_UNIT));
 m_scope.SetBackgroundColor(RGB(0, 64, 0));
 m_scope.SetGridColor(RGB(192, 192, 255));
 m_scope.SetPlotColor(RGB(255, 255, 255));
}

void CMIVentDutyGraph::Append(const SECU3IO::SensorDat* i_values, bool i_revdir /*= false*/)
{
 MeasInstrBase::Append(i_values->vent_duty, i_revdir);
}
