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
#include <math.h>
#include "LEDIndicator.h"

#define ROUND(x) (int)((x) + 0.5 - (double)((x) < 0))

namespace {
 const float minValue =  0.01f;
 const float maxValue = 99.99f;
} 

/////////////////////////////////////////////////////////////////////////////
// CLEDIndicator

CLEDIndicator::CLEDIndicator(bool pieIndicator /*= false*/)
: m_state(false)
, m_value(0.0f)
, m_rectWidth(1)
, m_pieIndicator(pieIndicator)
, m_dPI(4.0 * atan(1.0)) //PI
{
 m_rectColor = RGB(0,0,0);
 m_onColor   = RGB(255,255,30);
 m_offColor  = RGB(10,10,10);
 m_posColor  = RGB(30,255,100);

 m_bkBrush.CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
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

 if (m_pieIndicator)
 {
  CDC memDC;
  CBitmap memBitmap;

  memDC.CreateCompatibleDC(&dc);
  memBitmap.CreateCompatibleBitmap(&dc, rc.Width(), rc.Height());
  memDC.SelectObject(&memBitmap);
  memDC.FillRect(&rc, &m_bkBrush); //background

  DrawLED(memDC, rc);

  // copy the resulting bitmap to the destination DC
  dc.BitBlt(rc.left, rc.top, rc.Width(), rc.Height(), &memDC, 0, 0, SRCCOPY);
 }
 else
  DrawLED(dc, rc);

 // Do not call CStatic::OnPaint() for painting messages
}

void CLEDIndicator::DrawLED(CDC& dc, const CRect& rect)
{
 CBrush* pBrushOld = NULL;
 CPen* pPenOld = NULL;
 CRect rc = rect;

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
  dc.RoundRect(rc, CPoint(rc.Width(),rc.Height()));

  if (m_posBrush.m_hObject)
   dc.SelectObject(&m_posBrush);

  if(m_posPen.m_hObject)
   dc.SelectObject(&m_posPen);

  if (m_pieIndicator)
  {//Draw a pie if enabled    
   rc.DeflateRect(1,1);
   double value = m_value;
   if (value < minValue) value = minValue;
   if (value > maxValue) value = maxValue;
   double xR = (double)rc.Width() * 100.0;   //calculate x radius
   double yR = (double)rc.Height() * 100.0;  //calculate y radius  
   double eAngle = (((2.0*m_dPI) / 100.0) * value), bAngle = 0;
   CPoint end(ROUND(rc.CenterPoint().x - xR * sin(eAngle)), ROUND(rc.CenterPoint().y + yR * cos(eAngle)));
   CPoint begin(ROUND(rc.CenterPoint().x - xR * sin(bAngle)), ROUND(rc.CenterPoint().y + yR * cos(bAngle)));
   dc.Pie(rc, end, begin);
  }

  // old pen / brush
  if (pPenOld)
   dc.SelectObject(pPenOld);
  if (pBrushOld)
   dc.SelectObject(pBrushOld);
 }
}

void CLEDIndicator::SetState(bool state)
{
 if (state!=m_state)
 { //avoid stupid redrawing if state is already equal to m_state
  m_state = state;
  InvalidateRect(NULL,TRUE);
 }
}

void CLEDIndicator::SetPosition(float value)
{
 if (value!=m_value)
 { //avoid stupid redrawing if value is already equal to m_value
  m_value = value;
  InvalidateRect(NULL,TRUE);
 }
}

bool CLEDIndicator::GetState(void)
{
 return m_state;
}

float CLEDIndicator::GetPosition(void) const
{
 return m_value;
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

 if(m_posBrush.m_hObject)
  m_posBrush.DeleteObject();

 if(m_posBrush.m_hObject == NULL)
  m_posBrush.CreateSolidBrush(m_posColor);

 if(m_posPen.m_hObject)
  m_posPen.DeleteObject();

 if(m_posPen.m_hObject == NULL)
  m_posPen.CreatePen(PS_SOLID, 1, m_posColor);

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

  case led_pos:
   m_posColor = Color;
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

  case led_pos:
   *pColor = m_posColor;
   break;
 }
}
