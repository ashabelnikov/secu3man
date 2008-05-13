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

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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
	// NOTE: the ClassWizard will add DDX and DDV calls here
  //}}AFX_DATA_MAP
}

LPCTSTR CParamMonTabDlg::GetDialogID(void) const
{
  return (LPCTSTR)IDD; 
}


BEGIN_MESSAGE_MAP(CParamMonTabDlg, CDialog)
  //{{AFX_MSG_MAP(CParamMonTabDlg)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CParamMonTabDlg message handlers

BOOL CParamMonTabDlg::OnInitDialog() 
{
  CDialog::OnInitDialog();
	
  m_MIDeskDlg.Create(CMIDeskDlg::IDD,this);
  m_MIDeskDlg.MoveWindow(280,0,427,319);
  m_MIDeskDlg.ShowWindow(SW_SHOWNORMAL);
  m_MIDeskDlg.Show(true);
//  m_MIDeskDlg.Enable(false);

  m_ParamDeskDlg.Create(CParamDeskDlg::IDD,this);
  m_ParamDeskDlg.SetTitle("Параметры в EEPROM:");
  m_ParamDeskDlg.ShowWindow(SW_SHOWNORMAL);
 // m_ParamDeskDlg.Enable(true);
  m_ParamDeskDlg.Show(true);

 /* std::vector<std::wstring> v;    SHOULD BE REMOVED!!!

  v.push_back(_T("qwerty"));
  v.push_back(_T("asdfgh"));
  v.push_back(_T("zxcvbn"));

  m_ParamDeskDlg.SetFunctionsNames(v);*/
	
  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}
