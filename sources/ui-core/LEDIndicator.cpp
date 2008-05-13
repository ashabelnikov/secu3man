 /****************************************************************
 *
 *  Created by Alexey A. Shabelnikov. Ukraine, Gorlovka 2008. 
 *   ICQ: 405-791-931. e-mail: shabelnikov-stc@mail.ru
 *  Microprocessors systems - design & programming.
 *
 *****************************************************************/

#include "stdafx.h"
#include "LEDIndicator.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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
}


BEGIN_MESSAGE_MAP(CLEDIndicator, CStatic)
	//{{AFX_MSG_MAP(CLEDIndicator)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
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
  m_state = state;
  InvalidateRect(NULL,TRUE);
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
