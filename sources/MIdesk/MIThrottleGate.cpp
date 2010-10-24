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
              http://secu-3.narod.ru
              email: secu-3@yandex.ru
*/

#include "stdafx.h"
#include "MIThrottleGate.h"
#include "MIHelpers.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMIThrottleGate::CMIThrottleGate()
{
 //na
}

CMIThrottleGate::~CMIThrottleGate()
{
 //na
}

void CMIThrottleGate::Create(void)
{
 m_led.SetColor(led_rect,RGB(0,0,0));
 m_led.SetColor(led_on,RGB(255,255,0));
 m_led.SetColor(led_off,RGB(40,40,40));

 Enable(m_prev_enable);
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
 //not used by LED
}
	
void CMIThrottleGate::SetTicks(int number)
{
 //not used by LED
}
//----------------------------------------------------

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
