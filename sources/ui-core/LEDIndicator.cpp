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
#include "LEDIndicator.h"

/////////////////////////////////////////////////////////////////////////////
// CLEDIndicator

CLEDIndicator::CLEDIndicator()
: m_state(false)
, m_rectWidth(2)
{
 m_rectColor = RGB(0,0,0);
 m_onColor   = RGB(255,255,30);
 m_offColor  = RGB(10,10,10);

 ActuateColors();
}

CLEDIndicator::~CLEDIndicator()
{
 //empty
}

BEGIN_MESSAGE_MAP(CLEDIndicator, CStatic)
 ON_WM_PAINT()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLEDIndicator message handlers

void CLEDIndicator::OnPaint()
{
 CPaintDC dc(this); // device context for painting

 CRect rc;
 GetClientRect(&rc);

 CBrush* pBrushOld = NULL;
 CPen* pPenOld = NULL;

 if (IsWindowEnabled())
 {
  if(m_rectPen.m_hObject)
   pPenOld = dc.SelectObject(&m_rectPen);

  if (m_state)
  { //ON
   if(m_onBrush.m_hObject)
    pBrushOld = dc.SelectObject(&m_onBrush);
  }
  else
  { //OFF
   if(m_offBrush.m_hObject)
    pBrushOld = dc.SelectObject(&m_offBrush);
  }
  dc.RoundRect(rc,CPoint(rc.Width(),rc.Height()));
 }

 // old pen / brush
 if (pPenOld)
  dc.SelectObject(pPenOld);
 if (pBrushOld)
  dc.SelectObject(pBrushOld);

 // Do not call CStatic::OnPaint() for painting messages
}

void CLEDIndicator::SetState(bool state)
{
 if (state!=m_state)
 { //avoid stupid redrawing if state is already equal to m_state
  m_state = state;
  InvalidateRect(NULL,TRUE);
 }
}

bool CLEDIndicator::GetState(void)
{
 return m_state;
}

void CLEDIndicator::ActuateColors()
{
 if(m_rectPen.m_hObject)
  m_rectPen.DeleteObject();

 if(m_rectPen.m_hObject == NULL)
  m_rectPen.CreatePen(PS_SOLID, m_rectWidth, m_rectColor);

 if(m_onBrush.m_hObject)
  m_onBrush.DeleteObject();

 if(m_onBrush.m_hObject == NULL)
  m_onBrush.CreateSolidBrush(m_onColor);

 if(m_offBrush.m_hObject)
  m_offBrush.DeleteObject();

 if(m_offBrush.m_hObject == NULL)
  m_offBrush.CreateSolidBrush(m_offColor);
}

//////////////////////////////////////////////////////
void CLEDIndicator::SetColor(enum LEDMemberEnum led_member, COLORREF Color)
{
 switch(led_member)
 {
  case led_rect:
   m_rectColor = Color;
   break;

  case led_on:
   m_onColor = Color;
   break;

  case led_off:
   m_offColor = Color;
   break;
 }

 // set pen/brush colors
 ActuateColors();
}

//////////////////////////////////////////////////////
void CLEDIndicator::GetColor(enum LEDMemberEnum led_member, COLORREF* pColor)
{
 switch(led_member)
 {
  case led_rect:
   *pColor = m_rectColor;
   break;

  case led_on:
   *pColor = m_onColor;
   break;

  case led_off:
   *pColor = m_offColor;
   break;
 }
}
