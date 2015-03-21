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

/** \file MIAirFlow.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "MIAirFlow.h"
#include "MIHelpers.h"
#include "ui-core/fnt_helpers.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMIAirFlow::CMIAirFlow()
: m_loLimit(0.0f)
, m_upLimit(16.0f)
{
 //empty
}

CMIAirFlow::~CMIAirFlow()
{
 //empty
}

void CMIAirFlow::DDX_Controls(CDataExchange* pDX, int nIDC_progress, int nIDC_digits, int nIDC_caption )
{
 DDX_Control(pDX, nIDC_progress, m_progress);
 DDX_Control(pDX, nIDC_digits, m_digits);
 DDX_Control(pDX, nIDC_caption, m_caption);
}

void CMIAirFlow::Create()
{
 m_progress.SetRange((int)m_loLimit,(int)m_upLimit);
 m_progress.SetPos(0);
 m_digits.SetWindowText(_T("00"));

 CloneWndFont(&m_digits, &m_textFont, 11, true);
 m_digits.SetFont(&m_textFont);

 Enable(m_prev_enable);
}

//--------------------interface-----------------------
void CMIAirFlow::SetValue(float value)
{
 if ((value > m_upLimit)|| (value < m_loLimit))
  return;

 if (m_progress.GetPos()==((int)value))
  return; //avoid flickering

 CString cs;
 cs.Format(_T("%02d"),(int)value);
 m_progress.SetPos((int)value);
 m_digits.SetWindowText(cs);
}

float CMIAirFlow::GetValue(void)
{
 return (float)m_progress.GetPos();
}

void CMIAirFlow::Show(bool show)
{
 BOOL enable = ((show) ? TRUE : FALSE);
 m_progress.ShowWindow(enable);
 m_digits.ShowWindow(enable);
 m_caption.ShowWindow(enable);
}

void CMIAirFlow::Enable(bool enable)
{
 BOOL b_enable = ((enable) ? TRUE : FALSE);
 m_progress.EnableWindow(b_enable);
 m_digits.EnableWindow(b_enable);
 m_caption.EnableWindow(b_enable);
 m_progress.Invalidate();

 m_prev_enable = enable;
}

bool CMIAirFlow::IsVisible(void)
{
 BOOL state = m_progress.IsWindowVisible();
 return  ((state) ? TRUE : FALSE);
}

bool CMIAirFlow::IsEnabled(void)
{
 BOOL state = m_progress.IsWindowEnabled();
 return  ((state) ? TRUE : FALSE);
}

void CMIAirFlow::SetLimits(float loLimit, float upLimit)
{
 m_loLimit = loLimit;
 m_upLimit = upLimit;
 m_progress.SetRange((int)loLimit,(int)upLimit);
}

void CMIAirFlow::SetTicks(int number)
{
 //not used
}
//----------------------------------------------------

void CMIAirFlow::Scale(float i_x_factor, float i_y_factor)
{
 CRect rect;

 rect = MIHelpers::GetChildWndRect(&m_progress);
 MIHelpers::ScaleRect(rect, i_x_factor, i_y_factor);
 m_progress.MoveWindow(rect);

 rect = MIHelpers::GetChildWndRect(&m_digits);
 MIHelpers::ScaleRect(rect, i_x_factor, i_y_factor);
 m_digits.MoveWindow(rect);

 rect = MIHelpers::GetChildWndRect(&m_caption);
 MIHelpers::ScaleRect(rect, i_x_factor, i_y_factor);
 m_caption.MoveWindow(rect);
}
