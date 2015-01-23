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

#include "stdafx.h"
#include "resource.h"
#include "MIInjPW.h"
#include "MIHelpers.h"

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

void CMIInjPW::Create(void)
{
 m_meter.SetRange (0, 65.00);
 m_meter.SetLabelsDecimals(0);
 m_meter.SetValueDecimals(2);
 m_meter.SetTitle(MLL::LoadString(IDS_MI_INJ_PW_TITLE));
 m_meter.SetFontScale(80);
 m_meter.SetColor(meter_value,RGB(10,80,255));
 m_meter.SetUnit(MLL::LoadString(IDS_MI_MS_UNIT));
 m_meter.SetTickNumber(16);
 m_meter.AddAlertZone(0,2.0,RGB(255,100,100));
 m_meter.AddAlertZone(2.0,10.0,RGB(100,255,100));
 m_meter.AddAlertZone(10.0,65,RGB(150,150,250));
 m_meter.SetNeedleValue(0.0);
 m_meter.Update();
}

void CMIInjPW::DDX_Controls(CDataExchange* pDX, int nIDC_meter)
{
 DDX_Control(pDX, nIDC_meter, m_meter);
}

//--------------------interface-----------------------
void CMIInjPW::SetValue(float value)
{
 m_meter.SetNeedleValue((double)value);
 m_meter.Update();
}

float CMIInjPW::GetValue(void)
{
 return (float)m_meter.GetNeedlePos();
}

void CMIInjPW::Show(bool show)
{
 m_meter.ShowWindow((show) ? SW_SHOW : SW_HIDE);
}

void CMIInjPW::Enable(bool enable)
{
 m_meter.SetState(meter_needle, enable);
 m_meter.SetState(meter_value, enable);
 m_meter.SetState(meter_grid, enable);
 m_meter.SetState(meter_labels, enable);
 m_meter.SetState(meter_unit, enable);
 COLORREF bk_color;
 m_meter.GetColor(meter_bground, &bk_color);
 m_meter.SetColor(meter_bground, enable ? bk_color : ::GetSysColor(COLOR_BTNFACE));

 m_meter.Redraw();
}

bool CMIInjPW::IsVisible(void)
{
 return (m_meter.IsWindowVisible()) ? true : false;
}

bool CMIInjPW::IsEnabled(void)
{
 bool State = false;
 m_meter.GetState(meter_needle, &State);
 return State;
}

void CMIInjPW::SetLimits(float loLimit, float upLimit)
{
 m_meter.ResetAlertZones();
 m_meter.AddAlertZone(loLimit, upLimit * 0.5, RGB(100,255,100));
 m_meter.AddAlertZone(upLimit * 0.5, upLimit * 0.75, RGB(255,255,100));
 m_meter.AddAlertZone(upLimit * 0.75, upLimit, RGB(255,100,100));

 m_meter.SetRange(loLimit, upLimit);
}

void CMIInjPW::SetTicks(int number)
{
 m_meter.SetTickNumber(number);
}
//----------------------------------------------------

void CMIInjPW::Scale(float i_x_factor, float i_y_factor)
{
 CRect rect = MIHelpers::GetChildWndRect(&m_meter);
 MIHelpers::ScaleRect(rect, i_x_factor, i_y_factor);
 m_meter.MoveWindow(rect);
}
