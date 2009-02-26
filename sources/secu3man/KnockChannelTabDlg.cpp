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
{
  //{{AFX_DATA_INIT(CKnockChannelTabDlg)
	// NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT
  
}


void CKnockChannelTabDlg::DoDataExchange(CDataExchange* pDX)
{
  Super::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CKnockChannelTabDlg) 
  DDX_Control(pDX, IDC_KNOCK_CHANNEL_SAVE_PARAM_BUTTON,m_param_save_button);  
  //}}AFX_DATA_MAP
}

LPCTSTR CKnockChannelTabDlg::GetDialogID(void) const
{
  return (LPCTSTR)IDD; 
}


BEGIN_MESSAGE_MAP(CKnockChannelTabDlg, Super)
  //{{AFX_MSG_MAP(CKnockChannelTabDlg)	
	ON_WM_DESTROY()	
	ON_BN_CLICKED(IDC_KNOCK_CHANNEL_SAVE_PARAM_BUTTON, OnSaveParameters)
	ON_UPDATE_COMMAND_UI(IDC_KNOCK_CHANNEL_SAVE_PARAM_BUTTON, OnUpdateControls)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
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
  mp_RTChart = new CChartCtrl();
  CRect rect;
  GetDlgItem(IDC_KNOCK_CHANNEL_REALTIME_CHART_HOLDER)->GetWindowRect(rect);
  ScreenToClient(rect);
  mp_RTChart->Create(this,rect,IDC_KNOCK_CHANNEL_REALTIME_CHART);
 
  pPointSerie = dynamic_cast<CChartPointsSerie*>(mp_RTChart->AddSerie(CChartSerie::stPointsSerie));

 
COLORREF BackColor = RGB(0,50,0);
COLORREF GridColor = RGB(0,180,0);
COLORREF TextColor = RGB(0,180,0);
COLORREF SerieColor = RGB(250,250,180);


// Specifies a sunken border for the control
mp_RTChart->SetEdgeType(EDGE_SUNKEN);

// Sets the color of the border and the back color
mp_RTChart->SetBorderColor(TextColor);
mp_RTChart->SetBackColor(BackColor);

// Sets the min and max values of the bottom and left axis to -15 -> 15
mp_RTChart->GetBottomAxis()->SetMinMax(0,10);
mp_RTChart->GetLeftAxis()->SetMinMax(0,5);

//Sets the color of the different elements of the bottom axis
mp_RTChart->GetBottomAxis()->SetColor(TextColor);
mp_RTChart->GetBottomAxis()->SetTextColor(TextColor);
mp_RTChart->GetBottomAxis()->GetGrid()->SetColor(GridColor);

// Sets the color of the different elements of the left axis
mp_RTChart->GetLeftAxis()->SetColor(TextColor);
mp_RTChart->GetLeftAxis()->SetTextColor(TextColor);
mp_RTChart->GetLeftAxis()->GetGrid()->SetColor(GridColor);
mp_RTChart->GetLeftAxis()->SetTickIncrement(false, 1.0);

// Change the color of the line series
pPointSerie->SetColor(SerieColor);

  for (int i = 0; i < NUMBER; i++)
  {	  
  XValues[i] = i * (10.0 / NUMBER);
  }


//////////////////////////////////////////////////////////////////////////
  {
  CRect rect;
  GetDlgItem(IDC_KNOCK_CHANNEL_SIGNAL_OSCILLOSCOPE_HOLDER)->GetWindowRect(rect) ;
  ScreenToClient(rect) ;

  // create the control

  m_OScopeCtrl.Create(WS_VISIBLE | WS_CHILD, rect, this) ; 

  // customize the control

  m_OScopeCtrl.SetRange(0.0, 5.0, 1) ;
  m_OScopeCtrl.SetYUnits("Volts") ;
  m_OScopeCtrl.SetXUnits("Samples, grid 1x4") ;
  m_OScopeCtrl.SetBackgroundColor(RGB(0, 0, 64)) ;
  m_OScopeCtrl.SetGridColor(RGB(192, 192, 255)) ;
  m_OScopeCtrl.SetPlotColor(RGB(255, 255, 255)) ;
  }

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
 /*size_t c = pPointSerie->GetPointsCount();

 for (int i = 0; i < (NUMBER -1); i++)
   YValues[i] = YValues[i+1];  

 YValues[NUMBER - 1] = value;

 if (c!=NUMBER)
  pPointSerie->SetPoints(&XValues[(NUMBER-1)-c],&YValues[(NUMBER-1)-c], c+1);
 else
  pPointSerie->SetPoints(XValues,YValues,NUMBER);
*/

 m_OScopeCtrl.AppendPoint(value);
}
