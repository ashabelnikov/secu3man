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
#include "MIDwellAngle.h"
#include "MIHelpers.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMIDwellAngle::CMIDwellAngle()
{
 //na
}

CMIDwellAngle::~CMIDwellAngle()
{
 //na
}

void CMIDwellAngle::Create(void)
{
 m_meter.SetRange (-15.0, 65.0) ;
 m_meter.SetLabelsDecimals(1) ;
 m_meter.SetValueDecimals(2) ;
 m_meter.SetTitle(MLL::LoadString(IDS_MI_ADVANGLE_TITLE)) ;
 m_meter.SetFontScale(80);    
 m_meter.SetColor(meter_value,RGB(10,80,255));
 m_meter.SetUnit(MLL::LoadString(IDS_MI_ADVANGLE_UNIT));
 m_meter.SetTickNumber(16);
 m_meter.AddAlertZone(-15,0,RGB(180,130,130));
 m_meter.AddAlertZone(0,45,RGB(220,220,120));
 m_meter.AddAlertZone(45,65,RGB(255,100,100));
 m_meter.UpdateNeedle(-15.0);
 m_meter.SetMeterSize(145);
}

void CMIDwellAngle::DDX_Controls(CDataExchange* pDX, int nIDC_meter)
{
 DDX_Control(pDX, nIDC_meter, m_meter);
}

//--------------------interface-----------------------
void CMIDwellAngle::SetValue(float value)
{
 m_meter.UpdateNeedle((double)value);
}

float CMIDwellAngle::GetValue(void)
{
 return (float)m_meter.GetNeedlePos();
}
	
void CMIDwellAngle::Show(bool show)
{
 m_meter.ShowWindow((show) ? SW_SHOW : SW_HIDE);
}
	
void CMIDwellAngle::Enable(bool enable)
{
 m_meter.SetState(meter_needle, enable);
 m_meter.SetState(meter_value, enable);
 m_meter.SetState(meter_grid, enable);
 m_meter.SetState(meter_labels, enable);
 m_meter.SetState(meter_unit, enable);
 COLORREF bk_color;
 m_meter.GetColor(meter_bground, &bk_color);
 m_meter.SetColor(meter_bground, enable ? bk_color : ::GetSysColor(COLOR_BTNFACE));

 m_meter.Redraw();
}
	
bool CMIDwellAngle::IsVisible(void)
{
 return (m_meter.IsWindowVisible()) ? true : false;
}
	
bool CMIDwellAngle::IsEnabled(void)
{
 bool State = false;
 m_meter.GetState(meter_needle, &State);
 return State;
}	
	
void CMIDwellAngle::SetLimits(float loLimit, float upLimit)
{
 m_meter.SetRange(loLimit, upLimit);
}
	
void CMIDwellAngle::SetTicks(int number)
{
 m_meter.SetTickNumber(number);
}
//----------------------------------------------------

void CMIDwellAngle::Scale(float i_x_factor, float i_y_factor)
{
 CRect rect = MIHelpers::GetChildWndRect(&m_meter);
 MIHelpers::ScaleRect(rect, i_x_factor, i_y_factor);
 m_meter.MoveWindow(rect); 
}
