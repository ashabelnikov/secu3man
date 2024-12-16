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

/** \file MIDiffAFR.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "resource.h"
#include "MIDiffAFR.h"
#include "common/GDIHelpers.h"

void CMIDiffAFRBase::Create(CWnd* pParent, UINT Id)
{
 MeasInstrBase::Create(pParent, Id, false, false); //create window

 m_meter.SetRange (-9.0, 9.0);
 m_meter.SetLabelsDecimals(1);
 m_meter.SetValueDecimals(2);
 m_meter.SetColor(meter_value, RGB(10,80,255));
 m_meter.SetColor(meter_bground, GetSysColor(COLOR_BTNFACE));
 m_meter.SetColor(meter_labels, GDIHelpers::InvColor(GetSysColor(COLOR_BTNFACE)));
 m_meter.SetUnit(MLL::LoadString(IDS_MI_SENSAFR_UNIT));
 m_meter.SetTickNumber(18);
 m_meter.AddAlertZone(-9.0, -2.0, RGB(130,130,255));
 m_meter.AddAlertZone(-2.0, -1.0, RGB(240,240,130));
 m_meter.AddAlertZone(-1.0,  1.0, RGB(130,255,130));
 m_meter.AddAlertZone( 1.0,  2.0, RGB(240,240,130));
 m_meter.AddAlertZone( 2.0,  9.0, RGB(255,130,130));
 m_meter.SetNeedleValue(0.0);
 m_meter.Update();
 m_meter.SetMeterSize(130);
}

void CMIDiffAFRBase::SetLimits(float loLimit, float upLimit)
{
 m_meter.SetRange(loLimit, upLimit, true); //<-- also update alert zones
}

void CMIDiffAFR::Create(CWnd* pParent)
{
 CMIDiffAFRBase::Create(pParent, IDC_MI_DIFFAFR);
 m_meter.SetTitle(MLL::LoadString(IDS_MI_DIFFAFR_TITLE));
}

void CMIDiffAFR::Append(const SECU3IO::SensorDat* i_values, bool i_revdir /*= false*/)
{
 MeasInstrBase::Append(i_values->afr - i_values->afrmap, 0, 0, i_revdir);
}

void CMIDiffAFR2::Create(CWnd* pParent)
{
 CMIDiffAFRBase::Create(pParent, IDC_MI_DIFFAFR2);
 m_meter.SetTitle(MLL::LoadString(IDS_MI_DIFFAFR2_TITLE));
}

void CMIDiffAFR2::Append(const SECU3IO::SensorDat* i_values, bool i_revdir /*= false*/)
{
 MeasInstrBase::Append(i_values->afr2 - i_values->afrmap, 0, 0, i_revdir);
}

void CMIDiffAFRGraphBase::Create(CWnd* pParent, UINT Id)
{
 // create the window of control
 MeasInstrBase::Create(pParent, Id);

 // customize the control
 m_scope.SetRange(-9, 9);
 m_scope.SetDecPlaces(0, 1);
 m_scope.SetGridNumberY(9);
 m_scope.ReserveCharsY(5);
 m_scope.SetUnitX(MLL::GetString(IDS_MI_KNOCKGRAPH_H_UNIT));
 m_scope.SetBackgroundColor(RGB(0, 64, 0));
 m_scope.SetGridColor(RGB(192, 192, 255));
 m_scope.SetPlotColor(RGB(255, 255, 255));
}

void CMIDiffAFRGraph::Create(CWnd* pParent)
{
 CMIDiffAFRGraphBase::Create(pParent, IDC_MI_DIFFAFRGRAPH);
 m_scope.SetUnitY(MLL::GetString(IDS_MI_DIFFAFRGRAPH_V_UNIT));
}

void CMIDiffAFRGraph::Append(const SECU3IO::SensorDat* i_values, bool i_revdir /*= false*/)
{
 MeasInstrBase::Append(i_values->afr - i_values->afrmap, i_revdir);
}

void CMIDiffAFR2Graph::Create(CWnd* pParent)
{
 CMIDiffAFRGraphBase::Create(pParent, IDC_MI_DIFFAFR2GRAPH);
 m_scope.SetUnitY(MLL::GetString(IDS_MI_DIFFAFR2GRAPH_V_UNIT));
}

void CMIDiffAFR2Graph::Append(const SECU3IO::SensorDat* i_values, bool i_revdir /*= false*/)
{
 MeasInstrBase::Append(i_values->afr2 - i_values->afrmap, i_revdir);
}
