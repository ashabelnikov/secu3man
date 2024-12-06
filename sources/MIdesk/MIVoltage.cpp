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

/** \file MIVoltage.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "resource.h"
#include "MIVoltage.h"
#include "ui-core/ToolTipCtrlEx.h"
#include "common/GDIHelpers.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMIVoltage::CMIVoltage()
{
 //empty
}

CMIVoltage::~CMIVoltage()
{
 //empty
}

void CMIVoltage::Create(CWnd* pParent, UINT id)
{
 MeasInstrBase::Create(pParent, id); //create window

 m_meter.SetRange (0.0, 5.0);
 m_meter.SetLabelsDecimals(1);
 m_meter.SetValueDecimals(2);
 m_meter.SetColor(meter_value,RGB(10,80,255));
 m_meter.SetColor(meter_bground, GetSysColor(COLOR_BTNFACE));
 m_meter.SetColor(meter_labels, GDIHelpers::InvColor(GetSysColor(COLOR_BTNFACE)));
 m_meter.SetUnit(MLL::LoadString(IDS_MI_VOLTAGE_UNIT));
 m_meter.SetTickNumber(20);
 m_meter.AddAlertZone(0.0,5.0,RGB(180,180,230));
 m_meter.SetNeedleValue(0.0);
 m_meter.Update();
 m_meter.SetMeterSize(130);

 //create a tooltip control and assign tooltips
 mp_ttc.reset(new CToolTipCtrlEx());
 VERIFY(mp_ttc->Create(&m_meter, WS_POPUP | TTS_ALWAYSTIP | TTS_BALLOON));
 mp_ttc->SetMaxTipWidth(100); //Enable text wrapping
 mp_ttc->ActivateToolTips(true);
}

void CMIVoltage::SetLimits(float loLimit, float upLimit)
{
 m_meter.SetRange(loLimit, upLimit, true); //<-- also update alert zones
}

void CMIAddI1::Create(CWnd* pParent)
{
 CMIVoltage::Create(pParent, IDC_MI_ADD_I1);
 m_meter.SetTitle(MLL::GetString(IDS_MI_ADD_I1_TITLE).c_str());
 VERIFY(mp_ttc->AddWindow(&m_meter, MLL::GetString(IDS_MI_ADD_I1_TT)));
}

void CMIAddI2::Create(CWnd* pParent)
{
 CMIVoltage::Create(pParent, IDC_MI_ADD_I2);
 m_meter.SetTitle(MLL::GetString(IDS_MI_ADD_I2_TITLE).c_str());
 VERIFY(mp_ttc->AddWindow(&m_meter, MLL::GetString(IDS_MI_ADD_I2_TT)));
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMIAddI1Graph::CMIAddI1Graph()
{
 //empty
}

CMIAddI1Graph::~CMIAddI1Graph()
{
 //empty
}

void CMIAddI1Graph::Create(CWnd* pParent)
{
 // create the window of control
 CRect rect(0,0, 100,100);
 VERIFY(m_scope.Create(WS_VISIBLE | WS_CHILD, rect, pParent, IDC_MI_ADD_I1GRAPH));

 // customize the control
 m_scope.SetRange(0, 5);
 m_scope.SetDecPlaces(1, 2);
 m_scope.SetGridNumberY(10);
 m_scope.ReserveCharsY(5);
 m_scope.SetUnitY(MLL::GetString(IDS_MI_ADD_I1_V_UNIT));
 m_scope.SetUnitX(MLL::GetString(IDS_MI_KNOCKGRAPH_H_UNIT));
 m_scope.SetBackgroundColor(RGB(0, 64, 0));
 m_scope.SetGridColor(RGB(192, 192, 255));
 m_scope.SetPlotColor(RGB(255, 255, 255));
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMIAddI2Graph::CMIAddI2Graph()
{
 //empty
}

CMIAddI2Graph::~CMIAddI2Graph()
{
 //empty
}

void CMIAddI2Graph::Create(CWnd* pParent)
{
 // create the window of control
 CRect rect(0,0, 100,100);
 VERIFY(m_scope.Create(WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS, rect, pParent, IDC_MI_ADD_I2GRAPH));

 // customize the control
 m_scope.SetRange(0, 5);
 m_scope.SetDecPlaces(1, 2);
 m_scope.SetGridNumberY(10);
 m_scope.ReserveCharsY(5);
 m_scope.SetUnitY(MLL::GetString(IDS_MI_ADD_I2_V_UNIT));
 m_scope.SetUnitX(MLL::GetString(IDS_MI_KNOCKGRAPH_H_UNIT));
 m_scope.SetBackgroundColor(RGB(0, 64, 0));
 m_scope.SetGridColor(RGB(192, 192, 255));
 m_scope.SetPlotColor(RGB(255, 255, 255));
}
