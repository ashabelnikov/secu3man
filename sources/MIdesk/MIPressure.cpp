 /****************************************************************
 *       SECU-3  - An open source, free engine control unit
 *    Designed by Alexey A. Shabelnikov. Ukraine, Gorlovka 2007.
 *       Microprocessors systems - design & programming.
 *    contacts:
 *              http://secu-3.narod.ru
 *              ICQ: 405-791-931
 ****************************************************************/

#include "stdafx.h"
#include "resource.h"
#include "MIPressure.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMIPressure::CMIPressure()
{

}

CMIPressure::~CMIPressure()
{

}


void CMIPressure::Create(void)
{   
  m_meter.SetRange (10.0, 110.0) ;
  m_meter.SetLabelsDecimals(0) ;
  m_meter.SetValueDecimals(0) ;
  m_meter.SetTitle(MLL::LoadString(IDS_MI_PRESSURE_TITLE)) ;
  m_meter.SetFontScale(80);    
  m_meter.SetColor(meter_value,RGB(10,80,255));
  m_meter.SetUnit(MLL::LoadString(IDS_MI_PRESSURE_UNIT));
  m_meter.SetTickNumber(20);
  m_meter.AddAlertZone(10,30,RGB(180,130,130));
  m_meter.AddAlertZone(30,90,RGB(120,200,120));
  m_meter.AddAlertZone(90,110,RGB(180,130,130));
  m_meter.UpdateNeedle(0.0);
}

//--------------------interface-----------------------
void CMIPressure::SetValue(float value)
{
  m_meter.UpdateNeedle((double)value);
}

float CMIPressure::GetValue(void)
{
  return (float)m_meter.GetNeedlePos();
}
	
void CMIPressure::Show(bool show)
{
  m_meter.ShowWindow((show) ? SW_SHOW : SW_HIDE);
}
	
void CMIPressure::Enable(bool enable)
{
  m_meter.SetState(meter_needle, enable);
  m_meter.SetState(meter_value, enable);
  m_meter.SetState(meter_grid, enable);
  m_meter.SetState(meter_labels, enable);
  m_meter.SetState(meter_unit, enable);

  m_meter.Redraw();
}
	
bool CMIPressure::IsVisible(void)
{
  return (m_meter.IsWindowVisible()) ? true : false;
}
	
bool CMIPressure::IsEnabled(void)
{
  bool State = false;
  m_meter.GetState(meter_needle, &State);
  return State;
}	
	
void CMIPressure::SetLimits(float loLimit, float upLimit)
{
  m_meter.SetRange(loLimit, upLimit);
}
	
void CMIPressure::SetTicks(int number)
{
  m_meter.SetTickNumber(number);
}
//----------------------------------------------------
