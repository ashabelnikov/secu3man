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

/** \file MIMap2.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "resource.h"
#include "MIMap2.h"
#include "common/GDIHelpers.h"

//MAP2:
CMIMap2::CMIMap2()
{
 //empty
}

CMIMap2::~CMIMap2()
{
 //empty
}

void CMIMap2::Create(CWnd* pParent)
{
 MeasInstrBase::Create(pParent, IDC_MI_MAP2, false, false); //create window

 m_meter.SetRange (0.0, 400.0);
 m_meter.SetLabelsDecimals(0);
 m_meter.SetValueDecimals(2);
 m_meter.SetTitle(MLL::LoadString(IDS_MI_MAP2_TITLE));
 m_meter.SetColor(meter_value,RGB(10,80,255));
 m_meter.SetColor(meter_bground, GetSysColor(COLOR_BTNFACE));
 m_meter.SetColor(meter_labels, GDIHelpers::InvColor(GetSysColor(COLOR_BTNFACE)));
 m_meter.SetUnit(MLL::LoadString(IDS_MI_PRESSURE_UNIT));
 m_meter.SetTickNumber(20);
 m_meter.AddAlertZone(0,100,RGB(180,130,130));
 m_meter.AddAlertZone(100,400,RGB(120,200,120));
 m_meter.SetNeedleValue(0.0);
 m_meter.Update();
}

void CMIMap2::SetLimits(float loLimit, float upLimit)
{
 m_meter.SetRange(loLimit, upLimit, true); //<-- also update alert zones
}

void CMIMap2::Append(const SECU3IO::SensorDat* i_values, bool i_revdir /*= false*/)
{
 MeasInstrBase::Append(i_values->map2, 0, 0, i_revdir);
}

CMIMap2Graph::CMIMap2Graph()
{
 //empty
}

CMIMap2Graph::~CMIMap2Graph()
{
 //empty
}

void CMIMap2Graph::Create(CWnd* pParent)
{
 // create the window of control
 MeasInstrBase::Create(pParent, IDC_MI_MAP2GRAPH);

 // customize the control
 m_scope.SetRange(0, 400);
 m_scope.SetDecPlaces(0, 1);
 m_scope.SetGridNumberY(10);
 m_scope.ReserveCharsY(5);
 m_scope.SetUnitY(MLL::GetString(IDS_MI_MAP2GRAPH_V_UNIT));
 m_scope.SetUnitX(MLL::GetString(IDS_MI_KNOCKGRAPH_H_UNIT));
 m_scope.SetBackgroundColor(RGB(0, 64, 0));
 m_scope.SetGridColor(RGB(192, 192, 255));
 m_scope.SetPlotColor(RGB(255, 255, 255));
}

void CMIMap2Graph::Append(const SECU3IO::SensorDat* i_values, bool i_revdir /*= false*/)
{
 MeasInstrBase::Append(i_values->map2, i_revdir);
}

//GPS:
CMIGps::CMIGps()
{
 //empty
}

CMIGps::~CMIGps()
{
 //empty
}

void CMIGps::Create(CWnd* pParent)
{
 MeasInstrBase::Create(pParent, IDC_MI_GPS, false, false); //create window

 m_meter.SetRange (0.0, 400.0);
 m_meter.SetLabelsDecimals(0);
 m_meter.SetValueDecimals(2);
 m_meter.SetTitle(MLL::LoadString(IDS_MI_GPS_TITLE));
 m_meter.SetColor(meter_value,RGB(10,80,255));
 m_meter.SetColor(meter_bground, GetSysColor(COLOR_BTNFACE));
 m_meter.SetColor(meter_labels, GDIHelpers::InvColor(GetSysColor(COLOR_BTNFACE)));
 m_meter.SetUnit(MLL::LoadString(IDS_MI_PRESSURE_UNIT));
 m_meter.SetTickNumber(20);
 m_meter.AddAlertZone(0,100,RGB(180,130,130));
 m_meter.AddAlertZone(100,400,RGB(120,200,120));
 m_meter.SetNeedleValue(0.0);
 m_meter.Update();
}

void CMIGps::SetLimits(float loLimit, float upLimit)
{
 m_meter.SetRange(loLimit, upLimit, true); //<-- also update alert zones
}

void CMIGps::Append(const SECU3IO::SensorDat* i_values, bool i_revdir /*= false*/)
{
 MeasInstrBase::Append(i_values->gps, 0, 0, i_revdir);
}

CMIGpsGraph::CMIGpsGraph()
{
 //empty
}

CMIGpsGraph::~CMIGpsGraph()
{
 //empty
}

void CMIGpsGraph::Create(CWnd* pParent)
{
 // create the window of control
 MeasInstrBase::Create(pParent, IDC_MI_GPSGRAPH);

 // customize the control
 m_scope.SetRange(0, 400);
 m_scope.SetDecPlaces(0, 1);
 m_scope.SetGridNumberY(10);
 m_scope.ReserveCharsY(5);
 m_scope.SetUnitY(MLL::GetString(IDS_MI_GPSGRAPH_V_UNIT));
 m_scope.SetUnitX(MLL::GetString(IDS_MI_KNOCKGRAPH_H_UNIT));
 m_scope.SetBackgroundColor(RGB(0, 64, 0));
 m_scope.SetGridColor(RGB(192, 192, 255));
 m_scope.SetPlotColor(RGB(255, 255, 255));
}

void CMIGpsGraph::Append(const SECU3IO::SensorDat* i_values, bool i_revdir /*= false*/)
{
 MeasInstrBase::Append(i_values->gps, i_revdir);
}

//FPS:
CMIFps::CMIFps()
{
 //empty
}

CMIFps::~CMIFps()
{
 //empty
}

void CMIFps::Create(CWnd* pParent)
{
 MeasInstrBase::Create(pParent, IDC_MI_FPS, false, false); //create window

 m_meter.SetRange (0.0, 700.0);
 m_meter.SetLabelsDecimals(0);
 m_meter.SetValueDecimals(2);
 m_meter.SetTitle(MLL::LoadString(IDS_MI_FPS_TITLE));
 m_meter.SetColor(meter_value,RGB(10,80,255));
 m_meter.SetColor(meter_bground, GetSysColor(COLOR_BTNFACE));
 m_meter.SetColor(meter_labels, GDIHelpers::InvColor(GetSysColor(COLOR_BTNFACE)));
 m_meter.SetUnit(MLL::LoadString(IDS_MI_PRESSURE_UNIT));
 m_meter.SetTickNumber(20);
 m_meter.AddAlertZone(0,300,RGB(180,130,130));
 m_meter.AddAlertZone(300,700,RGB(120,200,120));
 m_meter.SetNeedleValue(0.0);
 m_meter.Update();
}

void CMIFps::SetLimits(float loLimit, float upLimit)
{
 m_meter.SetRange(loLimit, upLimit, true); //<-- also update alert zones
}

void CMIFps::Append(const SECU3IO::SensorDat* i_values, bool i_revdir /*= false*/)
{
 MeasInstrBase::Append(i_values->fps, 0, 0, i_revdir);
}

CMIFpsGraph::CMIFpsGraph()
{
 //empty
}

CMIFpsGraph::~CMIFpsGraph()
{
 //empty
}

void CMIFpsGraph::Create(CWnd* pParent)
{
 // create the window of control
 MeasInstrBase::Create(pParent, IDC_MI_FPSGRAPH);

 // customize the control
 m_scope.SetRange(0, 700);
 m_scope.SetDecPlaces(0, 1);
 m_scope.SetGridNumberY(10);
 m_scope.ReserveCharsY(5);
 m_scope.SetUnitY(MLL::GetString(IDS_MI_FPSGRAPH_V_UNIT));
 m_scope.SetUnitX(MLL::GetString(IDS_MI_KNOCKGRAPH_H_UNIT));
 m_scope.SetBackgroundColor(RGB(0, 64, 0));
 m_scope.SetGridColor(RGB(192, 192, 255));
 m_scope.SetPlotColor(RGB(255, 255, 255));
}

void CMIFpsGraph::Append(const SECU3IO::SensorDat* i_values, bool i_revdir /*= false*/)
{
 MeasInstrBase::Append(i_values->fps, i_revdir);
}
