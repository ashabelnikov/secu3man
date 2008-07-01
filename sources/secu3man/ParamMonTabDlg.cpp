 /****************************************************************
 *       SECU-3  - An open source, free engine control unit
 *    Designed by Alexey A. Shabelnikov. Ukraine, Gorlovka 2007.
 *       Microprocessors systems - design & programming.
 *    contacts:
 *              http://secu-3.narod.ru
 *              ICQ: 405-791-931
 ****************************************************************/

#include "stdafx.h"
#include "secu3man.h"
#include "ParamMonTabDlg.h"
#include "common/FastDelegate.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace fastdelegate;

/////////////////////////////////////////////////////////////////////////////
// CParamMonModePageDlg dialog


CParamMonTabDlg::CParamMonTabDlg(CWnd* pParent /*=NULL*/)
: CTabDialog(CParamMonTabDlg::IDD, pParent)
{
  //{{AFX_DATA_INIT(CParamMonTabDlg)
	// NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT
}


void CParamMonTabDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CParamMonTabDlg)
  DDX_Control(pDX,IDC_PM_SHOW_RAW_SENSORS,m_raw_sensors_check);
  //}}AFX_DATA_MAP
}

LPCTSTR CParamMonTabDlg::GetDialogID(void) const
{
  return (LPCTSTR)IDD; 
}


BEGIN_MESSAGE_MAP(CParamMonTabDlg, CDialog)
  //{{AFX_MSG_MAP(CParamMonTabDlg)
	ON_BN_CLICKED(IDC_PM_SHOW_RAW_SENSORS, OnPmShowRawSensors)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CParamMonTabDlg message handlers

BOOL CParamMonTabDlg::OnInitDialog() 
{
  CDialog::OnInitDialog();
	
  m_MIDeskDlg.Create(CMIDeskDlg::IDD,this);
  m_MIDeskDlg.MoveWindow(280,0,427,312);
  m_MIDeskDlg.ShowWindow(SW_SHOWNORMAL);
  m_MIDeskDlg.Show(true);
//  m_MIDeskDlg.Enable(false);

  m_RSDeskDlg.Create(CRSDeskDlg::IDD,this);
  m_RSDeskDlg.MoveWindow(280,0,427,312);
  m_RSDeskDlg.ShowWindow(SW_HIDE);
  m_RSDeskDlg.Show(true);

  m_ParamDeskDlg.Create(CParamDeskDlg::IDD,this);
  m_ParamDeskDlg.SetTitle("Параметры в EEPROM:");
  m_ParamDeskDlg.ShowWindow(SW_SHOWNORMAL);
 // m_ParamDeskDlg.Enable(true);
  m_ParamDeskDlg.Show(true);
	
  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}

void CParamMonTabDlg::OnPmShowRawSensors() 
{//делегируем обработку события чекбокса	
  if (m_OnRawSensorsCheck)
	m_OnRawSensorsCheck();  
}


bool CParamMonTabDlg::GetRawSensorsCheckState(void)
{
  int check = m_raw_sensors_check.GetCheck();
  return (check==BST_CHECKED) ? true : false;
}

