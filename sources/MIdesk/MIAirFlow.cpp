 /****************************************************************
 *       SECU-3  - An open source, free engine control unit
 *    Designed by Alexey A. Shabelnikov. Ukraine, Gorlovka 2007.
 *       Microprocessors systems - design & programming.
 *    contacts:
 *              http://secu-3.narod.ru
 *              ICQ: 405-791-931
 ****************************************************************/

#include "stdafx.h"
#include "MIAirFlow.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMIAirFlow::CMIAirFlow()
: m_loLimit(0.0f)
, m_upLimit(16.0f)
{

}

CMIAirFlow::~CMIAirFlow()
{

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

  Enable(m_prev_enable);
}


//--------------------interface-----------------------
void CMIAirFlow::SetValue(float value)
{
  if ((value > m_upLimit)|| (value < m_loLimit))
	return;

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