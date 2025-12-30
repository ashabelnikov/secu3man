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

/** \file MIOts.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "resource.h"
#include "MIOts.h"
#include "common/GDIHelpers.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMIOts::CMIOts()
{
 //empty
}

CMIOts::~CMIOts()
{
 //empty
}

void CMIOts::Create(CWnd* pParent)
{
 MeasInstrBase::Create(pParent, IDC_MI_OTS, false, false); //create window

 m_meter.SetRange (-40.0, 140.0);
 m_meter.SetLabelsDecimals(1);
 m_meter.SetValueDecimals(1);
 m_meter.SetTitle(MLL::LoadString(IDS_MI_OTS_TITLE));
 m_meter.SetColor(meter_value,RGB(10,80,255));
 m_meter.SetColor(meter_bground, GetSysColor(COLOR_BTNFACE));
 m_meter.SetColor(meter_labels, GDIHelpers::InvColor(GetSysColor(COLOR_BTNFACE)));
 m_meter.SetUnit(MLL::LoadString(IDS_MI_TEMPERATURE_UNIT));
 m_meter.SetTickNumber(16);
 m_meter.AddAlertZone(-40,50,RGB(130,130,180));
 m_meter.AddAlertZone(50,100,RGB(120,120,120));
 m_meter.AddAlertZone(100,140,RGB(230,130,130));
 m_meter.SetNeedleValue(-40.0);
 m_meter.Update();
}

void CMIOts::Append(const SECU3IO::SensorDat* i_values, bool i_revdir /*= false*/)
{
 MeasInstrBase::Append(i_values->ots, 0, 0, i_revdir);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMIOtsGraph::CMIOtsGraph()
{
 //empty
}

CMIOtsGraph::~CMIOtsGraph()
{
 //empty
}

void CMIOtsGraph::Create(CWnd* pParent)
{
 // create the window of control
 MeasInstrBase::Create(pParent, IDC_MI_OTSGRAPH);

 // customize the control
 m_scope.SetRange(-40.0, 140);
 m_scope.SetDecPlaces(0, 1);
 m_scope.SetGridNumberY(8);
 m_scope.ReserveCharsY(5);
 m_scope.SetUnitY(MLL::GetString(IDS_MI_OTSGRAPH_V_UNIT));
 m_scope.SetUnitX(MLL::GetString(IDS_MI_KNOCKGRAPH_H_UNIT));
 m_scope.SetBackgroundColor(RGB(0, 64, 0));
 m_scope.SetGridColor(RGB(192, 192, 255));
 m_scope.SetPlotColor(RGB(255, 255, 255));
}

void CMIOtsGraph::Append(const SECU3IO::SensorDat* i_values, bool i_revdir /*= false*/)
{
 MeasInstrBase::Append(i_values->ots, i_revdir);
}
