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
#include "MITachometer.h"
#include "MIHelpers.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMITachometer::CMITachometer()
{
 //na
}

CMITachometer::~CMITachometer()
{
 //na
}

void CMITachometer::Create(void)
{
 m_meter.SetRange (0, 8000.0);
 m_meter.SetLabelsDecimals(0);
 m_meter.SetValueDecimals(0);
 m_meter.SetTitle(MLL::LoadString(IDS_MI_TACHOMETER_TITLE));
 m_meter.SetFontScale(80);
 m_meter.SetColor(meter_value,RGB(10,80,255));
 m_meter.SetUnit(MLL::LoadString(IDS_MI_TACHOMETER_UNIT));
 m_meter.SetTickNumber(16);
 m_meter.AddAlertZone(0,4000,RGB(100,255,100));
 m_meter.AddAlertZone(4000,6000,RGB(255,255,100));
 m_meter.AddAlertZone(6000,8000,RGB(255,100,100));
 m_meter.UpdateNeedle(0.0);
}

void CMITachometer::DDX_Controls(CDataExchange* pDX, int nIDC_meter)
{
 DDX_Control(pDX, nIDC_meter, m_meter);
}

//--------------------interface-----------------------
void CMITachometer::SetValue(float value)
{
 m_meter.UpdateNeedle((double)value);
}

float CMITachometer::GetValue(void)
{
 return (float)m_meter.GetNeedlePos();
}

void CMITachometer::Show(bool show)
{
 m_meter.ShowWindow((show) ? SW_SHOW : SW_HIDE);
}

void CMITachometer::Enable(bool enable)
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

bool CMITachometer::IsVisible(void)
{
 return (m_meter.IsWindowVisible()) ? true : false;
}

bool CMITachometer::IsEnabled(void)
{
 bool State = false;
 m_meter.GetState(meter_needle, &State);
 return State;
}

void CMITachometer::SetLimits(float loLimit, float upLimit)
{
 m_meter.ResetAlertZones();
 m_meter.AddAlertZone(loLimit, upLimit * 0.5, RGB(100,255,100));
 m_meter.AddAlertZone(upLimit * 0.5, upLimit * 0.75, RGB(255,255,100));
 m_meter.AddAlertZone(upLimit * 0.75, upLimit, RGB(255,100,100));

 m_meter.SetRange(loLimit, upLimit);
}

void CMITachometer::SetTicks(int number)
{
 m_meter.SetTickNumber(number);
}
//----------------------------------------------------

void CMITachometer::Scale(float i_x_factor, float i_y_factor)
{
 CRect rect = MIHelpers::GetChildWndRect(&m_meter);
 MIHelpers::ScaleRect(rect, i_x_factor, i_y_factor);
 m_meter.MoveWindow(rect);
}
