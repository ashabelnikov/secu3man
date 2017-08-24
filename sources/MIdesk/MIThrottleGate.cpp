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
#include "MIHelpers.h"
#include "resource.h"
#include <float.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMIThrottleGate::CMIThrottleGate()
: m_led(true)
, m_loLimit(.0f)
, m_upLimit(.0f)
, m_value(FLT_MAX)
{
 //empty
}

CMIThrottleGate::~CMIThrottleGate()
{
 //empty
}

void CMIThrottleGate::Create(void)
{
 m_led.SetColor(led_rect,RGB(0,0,0));
 m_led.SetColor(led_on,RGB(255,255,0));
 m_led.SetColor(led_off,RGB(40,40,40));

 Enable(m_prev_enable);

 m_value = FLT_MAX; //reset cache
}

//--------------------interface-----------------------
void CMIThrottleGate::SetValue(float value)
{
 m_led.SetState((value) ? true : false);
}

float CMIThrottleGate::GetValue(void)
{
 bool state = m_led.GetState();
 return ((state) ? 1.0f : 0.0f);
}

void CMIThrottleGate::Show(bool show)
{
 int nCmdShow = (show) ? SW_SHOW : SW_HIDE;
 m_led.ShowWindow(nCmdShow);
 m_caption.ShowWindow(nCmdShow);
}

void CMIThrottleGate::Enable(bool enable)
{
 int b_enable = (enable) ? TRUE : FALSE ;
 m_led.EnableWindow(b_enable);
 m_caption.EnableWindow(b_enable);
 m_led.InvalidateRect(NULL,TRUE);

 m_prev_enable = enable;
}

bool CMIThrottleGate::IsVisible(void)
{
 BOOL state = m_led.IsWindowVisible();
 return ((state) ? true : false);
}

bool CMIThrottleGate::IsEnabled(void)
{
 BOOL state = m_led.IsWindowEnabled();
 return ((state) ? true : false);
}

void CMIThrottleGate::SetLimits(float loLimit, float upLimit)
{
 m_loLimit = loLimit;
 m_upLimit = upLimit;
}

void CMIThrottleGate::SetTicks(int number)
{
 //not used by LED
}
//----------------------------------------------------

void CMIThrottleGate::SetPosition(float value)
{
 if (value < m_loLimit) value = m_loLimit;
 if (value > m_upLimit) value = m_upLimit;
 m_led.SetPosition(((m_upLimit - m_loLimit) / 100.0f) * (value-m_loLimit));

 if (m_value!=value)
 {
  std::string fmt(MLL::GetString(IDS_MI_TPS_FMT_STR));
  TCHAR buff[32];
  _stprintf(buff, fmt.c_str(), value);
  m_caption.SetWindowText(buff);
  m_value = value;
 }
}

float CMIThrottleGate::GetPosition(void) const
{
 return m_led.GetPosition();
}

void CMIThrottleGate::DDX_Controls(CDataExchange* pDX, int nIDC_led, int nIDC_caption)
{
 DDX_Control(pDX, nIDC_led, m_led);
 DDX_Control(pDX, nIDC_caption, m_caption);
}

void CMIThrottleGate::Scale(float i_x_factor, float i_y_factor)
{
 CRect rect;

 rect = MIHelpers::GetChildWndRect(&m_led);
 MIHelpers::ScaleRect(rect, i_x_factor, i_y_factor);
 m_led.MoveWindow(rect);

 rect = MIHelpers::GetChildWndRect(&m_caption);
 MIHelpers::ScaleRect(rect, i_x_factor, i_y_factor);
 m_caption.MoveWindow(rect);
}
