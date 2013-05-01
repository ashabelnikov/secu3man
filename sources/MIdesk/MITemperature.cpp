/* SECU-3  - An open source, free engine control unit
   Copyright (C) 2007 Alexey A. Shabelnikov. Ukraine, Gorlovka

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
#include "MITemperature.h"
#include "MIHelpers.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMITemperature::CMITemperature()
: m_showChokePos(false)
{
 //empty
}

CMITemperature::~CMITemperature()
{
 //empty
}

void CMITemperature::Create(void)
{
 m_meter.SetRange (-40.0, 120.0) ;
 m_meter.SetLabelsDecimals(1) ;
 m_meter.SetValueDecimals(1) ;
 m_meter.SetTitle(MLL::LoadString(IDS_MI_TEMPERATURE_TITLE)) ;
 m_meter.SetFontScale(80);
 m_meter.SetColor(meter_value,RGB(10,80,255));
 m_meter.SetUnit(MLL::LoadString(IDS_MI_TEMPERATURE_UNIT));
 m_meter.SetTickNumber(16);
 m_meter.AddAlertZone(-40,50,RGB(130,130,180));
 m_meter.AddAlertZone(50,100,RGB(120,120,120));
 m_meter.AddAlertZone(100,120,RGB(230,130,130));
 m_meter.SetTRPane(_T("n/a"));
 m_meter.UpdateNeedle(-40.0);
}

void CMITemperature::DDX_Controls(CDataExchange* pDX, int nIDC_meter)
{
 DDX_Control(pDX, nIDC_meter, m_meter);
}

//--------------------interface-----------------------
void CMITemperature::SetValue(float value)
{
 m_meter.UpdateNeedle((double)value);
}

float CMITemperature::GetValue(void)
{
 return (float)m_meter.GetNeedlePos();
}

void CMITemperature::Show(bool show)
{
 m_meter.ShowWindow((show) ? SW_SHOW : SW_HIDE);
}

void CMITemperature::Enable(bool enable)
{
 m_meter.SetState(meter_needle, enable);
 m_meter.SetState(meter_value, enable);
 m_meter.SetState(meter_grid, enable);
 m_meter.SetState(meter_labels, enable);
 m_meter.SetState(meter_unit, enable);
 m_meter.SetState(meter_trpane, enable && m_showChokePos);
 COLORREF bk_color;
 m_meter.GetColor(meter_bground, &bk_color);
 m_meter.SetColor(meter_bground, enable ? bk_color : ::GetSysColor(COLOR_BTNFACE));

 m_meter.Redraw();
}

bool CMITemperature::IsVisible(void)
{
 return (m_meter.IsWindowVisible()) ? true : false;
}

bool CMITemperature::IsEnabled(void)
{
 bool State = false;
 m_meter.GetState(meter_needle, &State);
 return State;
}

void CMITemperature::SetLimits(float loLimit, float upLimit)
{
 m_meter.SetRange(loLimit, upLimit);
}

void CMITemperature::SetTicks(int number)
{
 m_meter.SetTickNumber(number);
}
//----------------------------------------------------

void CMITemperature::Scale(float i_x_factor, float i_y_factor)
{
 CRect rect = MIHelpers::GetChildWndRect(&m_meter);
 MIHelpers::ScaleRect(rect, i_x_factor, i_y_factor);
 m_meter.MoveWindow(rect);
}

void CMITemperature::SetChokePos(float value, bool redraw /*= false*/)
{
 CString str;
 str.Format(_T("%0.1f"), value);
 m_meter.SetTRPane(str);
 if (redraw)
  m_meter.UpdateNeedle(m_meter.GetNeedlePos());
}

float CMITemperature::GetChokePos(void) const
{
 CString str = m_meter.GetTRPane();
 float f_value = 0.0f;
 _stscanf(str.GetBuffer(), _T("%f"), &f_value);
 return f_value;
}

void CMITemperature::ShowChokePos(bool i_show, bool redraw /*= false*/)
{
 m_showChokePos = i_show;
 m_meter.SetState(meter_trpane, IsEnabled() && i_show);
 if (redraw)
  m_meter.Redraw();
}
