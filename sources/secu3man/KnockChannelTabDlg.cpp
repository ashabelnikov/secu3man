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
#include "KnockChannelTabDlg.h"
#include "common/FastDelegate.h"
#include "DLLLinkedFunctions.h"
#include <math.h>

#include "HiSCCtrl/sources/ChartPointsSerie.h"
#include "HiSCCtrl/sources/ChartLineSerie.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace std;
using namespace fastdelegate;

#define TIMER_ID 0

CKnockChannelTabDlg::CKnockChannelTabDlg(CWnd* pParent /*=NULL*/)
: Super(CKnockChannelTabDlg::IDD, pParent)
, mp_RTChart(NULL)
, m_all_enabled(true)
, m_pPointSerie(NULL)
, m_pLineSerie(NULL)
{
 //na  
}


void CKnockChannelTabDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);
 DDX_Control(pDX, IDC_KNOCK_CHANNEL_SAVE_PARAM_BUTTON,m_param_save_button);  
}

LPCTSTR CKnockChannelTabDlg::GetDialogID(void) const
{
 return (LPCTSTR)IDD; 
}


BEGIN_MESSAGE_MAP(CKnockChannelTabDlg, Super)
 ON_WM_DESTROY()	
 ON_BN_CLICKED(IDC_KNOCK_CHANNEL_SAVE_PARAM_BUTTON, OnSaveParameters)
 ON_UPDATE_COMMAND_UI(IDC_KNOCK_CHANNEL_SAVE_PARAM_BUTTON, OnUpdateControls)
 ON_WM_TIMER()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CKnockChannelTabDlg message handlers

CChartPointsSerie* pPointSerie;

BOOL CKnockChannelTabDlg::OnInitDialog() 
{
 Super::OnInitDialog();

 //создаем диалог с параметрами ДД
 m_knock_parameters_dlg.Create(CKnockPageDlg::IDD,this);
 m_knock_parameters_dlg.SetWindowPos(NULL,44,60,0,0,SWP_NOZORDER|SWP_NOSIZE);
 m_knock_parameters_dlg.ShowWindow(SW_SHOWNORMAL);

 SetTimer(TIMER_ID,250,NULL);

 //Инициализируем построитель функций
 _InitializeRPMKnockSignalControl();

 //инициализируем осциллограф
 _InitializeOscilloscopeControl();

 return TRUE;  // return TRUE unless you set the focus to a control
               // EXCEPTION: OCX Property Pages should return FALSE
}


void CKnockChannelTabDlg::OnDestroy() 
{
 Super::OnDestroy();  
 delete mp_RTChart;
 KillTimer(TIMER_ID);
}

void CKnockChannelTabDlg::OnSaveParameters(void)
{
 if (m_OnSaveParameters)
  m_OnSaveParameters();
}

void CKnockChannelTabDlg::EnableAll(bool i_enable)
{
 m_all_enabled = i_enable; //remember state 
 UpdateDialogControls(this,TRUE);
}

void CKnockChannelTabDlg::OnUpdateControls(CCmdUI* pCmdUI) 
{
 pCmdUI->Enable(m_all_enabled);  
}

void CKnockChannelTabDlg::OnTimer(UINT nIDEvent) 
{
 //dirty hack
 UpdateDialogControls(this,TRUE);
 Super::OnTimer(nIDEvent);  
}


void CKnockChannelTabDlg::AppendPoint(float value)
{
 m_OScopeCtrl.AppendPoint(value);
}


void CKnockChannelTabDlg::SetRPMKnockSignal(const std::vector<float> &i_values)
{
 _ASSERTE(i_values.size()==RPM_KNOCK_SIGNAL_POINTS);
 mp_RTChart->EnableRefresh(false);
 for(size_t i = 0; i < i_values.size(); i++)
 {
  m_pPointSerie->SetYPointValue(i, i_values[i]);
  m_pLineSerie->SetYPointValue(i, i_values[i]);
 }
 mp_RTChart->EnableRefresh(true);
}

void CKnockChannelTabDlg::_InitializeRPMKnockSignalControl(void)
{
 //Инициализируем построитель функций
 mp_RTChart = new CChartCtrl();
 CRect rect;
 GetDlgItem(IDC_KNOCK_CHANNEL_REALTIME_CHART_HOLDER)->GetWindowRect(rect);
 ScreenToClient(rect);
 mp_RTChart->Create(this,rect,IDC_KNOCK_CHANNEL_REALTIME_CHART);

 m_pPointSerie = dynamic_cast<CChartPointsSerie*>(mp_RTChart->AddSerie(CChartSerie::stPointsSerie));
 m_pLineSerie = dynamic_cast<CChartLineSerie*>(mp_RTChart->AddSerie(CChartSerie::stLineSerie));

 m_pLineSerie->SetColor(RGB(80,80,200));

 int rpm = 200;
 int rpm_step = 60;
 // Sets the min and max values of the bottom and left axis.
 mp_RTChart->GetBottomAxis()->SetMinMax(rpm, rpm + (rpm_step * RPM_KNOCK_SIGNAL_POINTS));
 mp_RTChart->GetLeftAxis()->SetMinMax(0,5.0); 

 for (size_t i = 0; i < RPM_KNOCK_SIGNAL_POINTS; i++)
  {	  
  m_pPointSerie->AddPoint(rpm,0.0);
  m_pLineSerie->AddPoint(rpm,0.0);
  rpm+=rpm_step;
  }
}


//инициализация осциллографа
void CKnockChannelTabDlg::_InitializeOscilloscopeControl(void)
{
 CRect rect;
 GetDlgItem(IDC_KNOCK_CHANNEL_SIGNAL_OSCILLOSCOPE_HOLDER)->GetWindowRect(rect) ;
 ScreenToClient(rect) ;

 // create the control
 m_OScopeCtrl.Create(WS_VISIBLE | WS_CHILD, rect, this) ; 

 // customize the control
 m_OScopeCtrl.SetRange(0.0, 5.0, 1) ;
 m_OScopeCtrl.SetYUnits(MLL::LoadString(IDS_KC_OSCILLOSCOPE_V_UNIT));
 m_OScopeCtrl.SetXUnits(MLL::LoadString(IDS_KC_OSCILLOSCOPE_H_UNIT));
 m_OScopeCtrl.SetBackgroundColor(RGB(0, 0, 64)) ;
 m_OScopeCtrl.SetGridColor(RGB(192, 192, 255)) ;
 m_OScopeCtrl.SetPlotColor(RGB(255, 255, 255)) ;
}

void CKnockChannelTabDlg::setOnSaveParameters(EventHandler OnFunction) 
{
 m_OnSaveParameters = OnFunction;
}
