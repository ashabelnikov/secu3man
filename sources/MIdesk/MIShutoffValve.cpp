 /****************************************************************
 *       SECU-3  - An open source, free engine control unit
 *    Designed by Alexey A. Shabelnikov. Ukraine, Gorlovka 2007.
 *       Microprocessors systems - design & programming.
 *    contacts:
 *              http://secu-3.narod.ru
 *              ICQ: 405-791-931
 ****************************************************************/

#include "stdafx.h"
#include "MIShutoffValve.h"
#include "MIHelpers.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMIShutoffValve::CMIShutoffValve()
{
 //na
}

CMIShutoffValve::~CMIShutoffValve()
{
 //na
}

void CMIShutoffValve::Create(void)
{
 m_led.SetColor(led_rect,RGB(0,0,0));
 m_led.SetColor(led_on,RGB(255,255,0));
 m_led.SetColor(led_off,RGB(40,40,40));

 Enable(m_prev_enable);
}

//--------------------interface-----------------------
void CMIShutoffValve::SetValue(float value)
{
 m_led.SetState((value) ? true : false);
}

float CMIShutoffValve::GetValue(void)
{
 bool state = m_led.GetState();
 return ((state) ? 1.0f : 0.0f);
}
	
void CMIShutoffValve::Show(bool show)
{
 int nCmdShow = (show) ? SW_SHOW : SW_HIDE;
 m_led.ShowWindow(nCmdShow);  
 m_caption.ShowWindow(nCmdShow);
}
	
void CMIShutoffValve::Enable(bool enable)
{
 BOOL b_enable = (enable) ? TRUE : FALSE;
 m_led.EnableWindow(b_enable);
 m_caption.EnableWindow(b_enable);
 m_led.InvalidateRect(NULL,TRUE);

 m_prev_enable = enable;
}
	
bool CMIShutoffValve::IsVisible(void)
{
 BOOL state = m_led.IsWindowVisible();
 return ((state) ? true : false);
}
	
bool CMIShutoffValve::IsEnabled(void)
{
 BOOL state = m_led.IsWindowEnabled();
 return ((state) ? true : false);
}	
	
void CMIShutoffValve::SetLimits(float loLimit, float upLimit)
{
 //not used by LED
}
	
void CMIShutoffValve::SetTicks(int number)
{
 //not used by LED
}
//----------------------------------------------------

void CMIShutoffValve::DDX_Controls(CDataExchange* pDX, int nIDC_led, int nIDC_caption)
{
 DDX_Control(pDX, nIDC_led, m_led);
 DDX_Control(pDX, nIDC_caption, m_caption);
}

void CMIShutoffValve::Scale(float i_x_factor, float i_y_factor)
{
 CRect rect; 

 rect = MIHelpers::GetChildWndRect(&m_led);
 MIHelpers::ScaleRect(rect, i_x_factor, i_y_factor);
 m_led.MoveWindow(rect); 

 rect = MIHelpers::GetChildWndRect(&m_caption);
 MIHelpers::ScaleRect(rect, i_x_factor, i_y_factor);
 m_caption.MoveWindow(rect); 
}
