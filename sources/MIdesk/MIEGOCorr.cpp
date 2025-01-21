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

/** \file MIEGOCorr.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "resource.h"
#include "MIEGOCorr.h"
#include "common/GDIHelpers.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMIEGOCorr::CMIEGOCorr()
{
 //empty
}

CMIEGOCorr::~CMIEGOCorr()
{
 //empty
}

void CMIEGOCorr::Create(CWnd* pParent)
{
 MeasInstrBase::Create(pParent, IDC_MI_EGO_CORR, false, false); //create window

 m_meter.SetRange (-50.0, 50.0);
 m_meter.SetLabelsDecimals(1);
 m_meter.SetValueDecimals(1);
 m_meter.SetTitle(MLL::LoadString(IDS_MI_EGO_CORR_TITLE));
 m_meter.SetColor(meter_value,RGB(10,80,255));
 m_meter.SetColor(meter_bground, GetSysColor(COLOR_BTNFACE));
 m_meter.SetColor(meter_labels, GDIHelpers::InvColor(GetSysColor(COLOR_BTNFACE)));
 m_meter.SetUnit(MLL::LoadString(IDS_MI_PERCENT_UNIT));
 m_meter.SetTickNumber(16);
 m_meter.AddAlertZone(-50,-20,RGB(230,130,130));
 m_meter.AddAlertZone(-20,20,RGB(120,190,120));
 m_meter.AddAlertZone(20,50,RGB(230,130,130));
 m_meter.SetTRPane(_T("n/a"));
 m_meter.SetNeedleValue(0.0);
 m_meter.Update();
}

void CMIEGOCorr::Append(const SECU3IO::SensorDat* i_values, bool i_revdir /*= false*/)
{
 MeasInstrBase::Append(i_values->lambda_corr, 0, 0, i_revdir);
}

CMIEGOCorr2::CMIEGOCorr2()
{
 //empty
}

CMIEGOCorr2::~CMIEGOCorr2()
{
 //empty
}

void CMIEGOCorr2::Create(CWnd* pParent)
{
 MeasInstrBase::Create(pParent, IDC_MI_EGO_CORR2, false, false); //create window

 m_meter.SetRange (-50.0, 50.0);
 m_meter.SetLabelsDecimals(1);
 m_meter.SetValueDecimals(1);
 m_meter.SetTitle(MLL::LoadString(IDS_MI_EGO_CORR2_TITLE));
 m_meter.SetColor(meter_value,RGB(10,80,255));
 m_meter.SetColor(meter_bground, GetSysColor(COLOR_BTNFACE));
 m_meter.SetColor(meter_labels, GDIHelpers::InvColor(GetSysColor(COLOR_BTNFACE)));
 m_meter.SetUnit(MLL::LoadString(IDS_MI_PERCENT_UNIT));
 m_meter.SetTickNumber(16);
 m_meter.AddAlertZone(-50,-20,RGB(230,130,130));
 m_meter.AddAlertZone(-20,20,RGB(120,190,120));
 m_meter.AddAlertZone(20,50,RGB(230,130,130));
 m_meter.SetTRPane(_T("n/a"));
 m_meter.SetNeedleValue(0.0);
 m_meter.Update();
}

void CMIEGOCorr2::Append(const SECU3IO::SensorDat* i_values, bool i_revdir /*= false*/)
{
 MeasInstrBase::Append(i_values->lambda_corr2, 0, 0, i_revdir);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMIEGOCorrGraph::CMIEGOCorrGraph()
{
 //empty
}

CMIEGOCorrGraph::~CMIEGOCorrGraph()
{
 //empty
}

void CMIEGOCorrGraph::Create(CWnd* pParent)
{
 // create the window of control
 MeasInstrBase::Create(pParent, IDC_MI_EGO_CORRGRAPH);

 // customize the control
 m_scope.SetRange(-50, 50);
 m_scope.SetDecPlaces(0, 2);
 m_scope.SetGridNumberY(10);
 m_scope.ReserveCharsY(5);
 m_scope.SetUnitY(MLL::GetString(IDS_MI_EGO_CORRGRAPH_V_UNIT));
 m_scope.SetUnitX(MLL::GetString(IDS_MI_KNOCKGRAPH_H_UNIT));
 m_scope.SetBackgroundColor(RGB(0, 64, 0));
 m_scope.SetGridColor(RGB(192, 192, 255));
 m_scope.SetPlotColor(RGB(255, 255, 255));
}

void CMIEGOCorrGraph::Append(const SECU3IO::SensorDat* i_values, bool i_revdir /*= false*/)
{
 MeasInstrBase::Append(i_values->lambda_corr, i_revdir);
}

CMIEGOCorr2Graph::CMIEGOCorr2Graph()
{
 //empty
}

CMIEGOCorr2Graph::~CMIEGOCorr2Graph()
{
 //empty
}

void CMIEGOCorr2Graph::Create(CWnd* pParent)
{
 // create the window of control
 MeasInstrBase::Create(pParent, IDC_MI_EGO_CORR2GRAPH);

 // customize the control
 m_scope.SetRange(-50, 50);
 m_scope.SetDecPlaces(0, 2);
 m_scope.SetGridNumberY(10);
 m_scope.ReserveCharsY(5);
 m_scope.SetUnitY(MLL::GetString(IDS_MI_EGO_CORR2GRAPH_V_UNIT));
 m_scope.SetUnitX(MLL::GetString(IDS_MI_KNOCKGRAPH_H_UNIT));
 m_scope.SetBackgroundColor(RGB(0, 64, 0));
 m_scope.SetGridColor(RGB(192, 192, 255));
 m_scope.SetPlotColor(RGB(255, 255, 255));
}

void CMIEGOCorr2Graph::Append(const SECU3IO::SensorDat* i_values, bool i_revdir /*= false*/)
{
 MeasInstrBase::Append(i_values->lambda_corr2, i_revdir);
}
