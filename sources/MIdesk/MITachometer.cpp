 /****************************************************************
 *       SECU-3  - An open source, free engine control unit
 *    Designed by Alexey A. Shabelnikov. Ukraine, Gorlovka 2007.
 *       Microprocessors systems - design & programming.
 *    contacts:
 *              http://secu-3.narod.ru
 *              ICQ: 405-791-931
 ****************************************************************/

#include "stdafx.h"
#include "MITachometer.h"

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

}

CMITachometer::~CMITachometer()
{

}


void CMITachometer::Create(void)
{
  m_meter.SetRange (0, 8000.0) ;
  m_meter.SetLabelsDecimals(0) ;
  m_meter.SetValueDecimals(0) ;
  m_meter.SetTitle("Обороты") ;
  m_meter.SetFontScale(80);    
  m_meter.SetColor(meter_value,RGB(10,80,255));
  m_meter.SetUnit("мин-1");
  m_meter.SetTickNumber(16);
  m_meter.AddAlertZone(0,4000,RGB(100,255,100));
  m_meter.AddAlertZone(4000,6000,RGB(255,255,100));
  m_meter.AddAlertZone(6000,8000,RGB(255,100,100));
  m_meter.UpdateNeedle(0.0); 
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
  m_meter.SetRange(loLimit, upLimit);
}
	
void CMITachometer::SetTicks(int number)
{
  m_meter.SetTickNumber(number);
}
//----------------------------------------------------