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
#include "MIVoltmeter.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMIVoltmeter::CMIVoltmeter()
{

}

CMIVoltmeter::~CMIVoltmeter()
{

}

void CMIVoltmeter::Create(void)
{
  m_meter.SetRange (2.0, 18.0) ;
  m_meter.SetLabelsDecimals(1) ;
  m_meter.SetValueDecimals(2) ;
  m_meter.SetTitle(MLL::LoadString(IDS_MI_VOLTAGE_TITLE)) ;
  m_meter.SetFontScale(80);    
  m_meter.SetColor(meter_value,RGB(10,80,255));
  m_meter.SetUnit(MLL::LoadString(IDS_MI_VOLTAGE_UNIT));
  m_meter.SetTickNumber(16);
  m_meter.AddAlertZone(2.0,10.0,RGB(230,130,130));
  m_meter.AddAlertZone(10.0,15.0,RGB(120,200,120));
  m_meter.AddAlertZone(15.0,18.0,RGB(255,160,160));
  m_meter.UpdateNeedle(0.0);
  m_meter.SetMeterSize(130);
}

//--------------------interface-----------------------
void CMIVoltmeter::SetValue(float value)
{
  m_meter.UpdateNeedle((double)value);
}

float CMIVoltmeter::GetValue(void)
{
  return (float)m_meter.GetNeedlePos();
}
	
void CMIVoltmeter::Show(bool show)
{
  m_meter.ShowWindow((show) ? SW_SHOW : SW_HIDE);
}
	
void CMIVoltmeter::Enable(bool enable)
{
  m_meter.SetState(meter_needle, enable);
  m_meter.SetState(meter_value, enable);
  m_meter.SetState(meter_grid, enable);
  m_meter.SetState(meter_labels, enable);
  m_meter.SetState(meter_unit, enable);

  m_meter.Redraw();
}
	
bool CMIVoltmeter::IsVisible(void)
{
  return (m_meter.IsWindowVisible()) ? true : false;
}
	
bool CMIVoltmeter::IsEnabled(void)
{
  bool State = false;
  m_meter.GetState(meter_needle, &State);
  return State;
}	
	
void CMIVoltmeter::SetLimits(float loLimit, float upLimit)
{
  m_meter.SetRange(loLimit, upLimit);
}
	
void CMIVoltmeter::SetTicks(int number)
{
  m_meter.SetTickNumber(number);
}
//----------------------------------------------------
