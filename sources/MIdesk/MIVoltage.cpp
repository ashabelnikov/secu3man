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
#include "MIVoltage.h"
#include "MIHelpers.h"

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

void CMIVoltage::Create(void)
{
 m_meter.SetRange (0.0, 5.0) ;
 m_meter.SetLabelsDecimals(1) ;
 m_meter.SetValueDecimals(2) ;
 m_meter.SetFontScale(80);
 m_meter.SetColor(meter_value,RGB(10,80,255));
 m_meter.SetUnit(MLL::LoadString(IDS_MI_VOLTAGE_UNIT));
 m_meter.SetTickNumber(10);
 m_meter.AddAlertZone(0.0,5.0,RGB(180,180,230));
 m_meter.UpdateNeedle(0.0);
 m_meter.SetMeterSize(130);
}

void CMIVoltage::DDX_Controls(CDataExchange* pDX, int nIDC_meter)
{
 DDX_Control(pDX, nIDC_meter, m_meter);
}

//--------------------interface-----------------------
void CMIVoltage::SetValue(float value)
{
 m_meter.UpdateNeedle((double)value);
}

float CMIVoltage::GetValue(void)
{
 return (float)m_meter.GetNeedlePos();
}

void CMIVoltage::Show(bool show)
{
 m_meter.ShowWindow((show) ? SW_SHOW : SW_HIDE);
}

void CMIVoltage::Enable(bool enable)
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

bool CMIVoltage::IsVisible(void)
{
 return (m_meter.IsWindowVisible()) ? true : false;
}

bool CMIVoltage::IsEnabled(void)
{
 bool State = false;
 m_meter.GetState(meter_needle, &State);
 return State;
}

void CMIVoltage::SetLimits(float loLimit, float upLimit)
{
 m_meter.SetRange(loLimit, upLimit);
}

void CMIVoltage::SetTicks(int number)
{
 m_meter.SetTickNumber(number);
}
//----------------------------------------------------

void CMIVoltage::Scale(float i_x_factor, float i_y_factor)
{
 CRect rect = MIHelpers::GetChildWndRect(&m_meter);
 MIHelpers::ScaleRect(rect, i_x_factor, i_y_factor);
 m_meter.MoveWindow(rect);
}

void CMIVoltage::SetTitle(const _TSTRING& title)
{
 m_meter.SetTitle(title.c_str());
}
