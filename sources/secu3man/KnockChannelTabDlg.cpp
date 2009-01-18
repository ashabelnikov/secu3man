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


CKnockChannelTabDlg::CKnockChannelTabDlg(CWnd* pParent /*=NULL*/)
: Super(CKnockChannelTabDlg::IDD, pParent)
, mp_RTChart(NULL)
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
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CKnockChannelTabDlg message handlers


BOOL CKnockChannelTabDlg::OnInitDialog() 
{
  Super::OnInitDialog();

  m_knock_parameters.Create(CKnockPageDlg::IDD,this);
  m_knock_parameters.SetWindowPos(NULL,50,60,0,0,SWP_NOZORDER|SWP_NOSIZE);
  m_knock_parameters.ShowWindow(SW_SHOWNORMAL);

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
}

void CKnockChannelTabDlg::OnSaveParameters(void)
{
 if (m_OnSaveParameters)
  m_OnSaveParameters();
}
