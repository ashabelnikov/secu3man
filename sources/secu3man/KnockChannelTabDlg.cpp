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
 
  CChartPointsSerie* pPointSerie = dynamic_cast<CChartPointsSerie*>(mp_RTChart->AddSerie(CChartSerie::stPointsSerie));

  pPointSerie->AddPoint(4,7);
  pPointSerie->AddPoint(6,3);
  pPointSerie->AddPoint(2,8);
  pPointSerie->AddPoint(5,4);

  
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
