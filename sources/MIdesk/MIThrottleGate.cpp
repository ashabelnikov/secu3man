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

/** \file MIThrottleGate.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "MIThrottleGate.h"
#include "common/GDIHelpers.h"
#include "resource.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMIThrottleGate::CMIThrottleGate()
{
 //empty
}

CMIThrottleGate::~CMIThrottleGate()
{
 //empty
}

void CMIThrottleGate::Create(void)
{
 m_meter.SetRange (.0, 100.0) ;
 m_meter.SetLabelsDecimals(1) ;
 m_meter.SetValueDecimals(1) ;
 m_meter.SetTitle(MLL::LoadString(IDS_MI_THROTTLE_GATE_TITLE)) ;
 m_meter.SetFontScale(80);
 m_meter.SetColor(meter_value,RGB(10,80,255));
 m_meter.SetUnit(MLL::LoadString(IDS_MI_PERCENT_UNIT));
 m_meter.SetTickNumber(16);
 m_meter.AddAlertZone(0,5,RGB(130,130,180));
 m_meter.AddAlertZone(5,75,RGB(120,180,150));
 m_meter.AddAlertZone(75,100,RGB(230,130,130));
 m_meter.SetTRPane(_T("n/a"));
 m_meter.SetTLPane(_T("n/a"));
 m_meter.SetNeedleValue(0.0);
 m_meter.Update();

 m_rect = GDIHelpers::GetChildWndRect(&m_meter);
}

//--------------------interface-----------------------
void CMIThrottleGate::SetValue(float value)
{
 m_meter.SetNeedleValue((double)value);
 m_meter.Update();
}

float CMIThrottleGate::GetValue(void)
{
 return (float)m_meter.GetNeedlePos();
}

void CMIThrottleGate::Show(bool show)
{
 m_meter.ShowWindow((show) ? SW_SHOW : SW_HIDE);
}

void CMIThrottleGate::Enable(bool enable)
{
 m_meter.SetState(meter_needle, enable);
 m_meter.SetState(meter_value, enable);
 m_meter.SetState(meter_grid, enable);
 m_meter.SetState(meter_labels, enable);
 m_meter.SetState(meter_unit, enable);
 m_meter.SetState(meter_trpane, false);
 m_meter.SetState(meter_tlpane, enable);
 COLORREF bk_color;
 m_meter.GetColor(meter_bground, &bk_color);
 m_meter.SetColor(meter_bground, enable ? bk_color : ::GetSysColor(COLOR_BTNFACE));

 m_meter.Redraw();
}

bool CMIThrottleGate::IsVisible(void)
{
 return (m_meter.IsWindowVisible()) ? true : false;
}

bool CMIThrottleGate::IsEnabled(void)
{
 bool State = false;
 m_meter.GetState(meter_needle, &State);
 return State;
}

void CMIThrottleGate::SetLimits(float loLimit, float upLimit)
{
 m_meter.SetRange(loLimit, upLimit);
}

void CMIThrottleGate::SetTicks(int number)
{
 m_meter.SetTickNumber(number);
}
//----------------------------------------------------

void CMIThrottleGate::SetAirFlow(float value, bool redraw /*= false*/)
{
 CString str;
 str.Format(_T("%d"), (int)value);
 m_meter.SetTLPane(str);
 if (redraw) m_meter.Update();
}

void CMIThrottleGate::DDX_Controls(CDataExchange* pDX, int nIDC_meter)
{
 DDX_Control(pDX, nIDC_meter, m_meter);
}

void CMIThrottleGate::Scale(float i_x_factor, float i_y_factor, bool repaint /*= true*/)
{
 CRect rect = m_rect;
 GDIHelpers::ScaleRect(rect, i_x_factor, i_y_factor);
 m_meter.MoveWindow(rect, repaint);
}
