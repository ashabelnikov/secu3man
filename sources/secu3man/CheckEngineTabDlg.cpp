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
#include "CheckEngineTabDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CCheckEngineTabDlg::CCheckEngineTabDlg(CWnd* pParent /*=NULL*/)
: CTabDialog(CCheckEngineTabDlg::IDD, pParent)
{
  //{{AFX_DATA_INIT(CCheckEngineTabDlg)
	// NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT

  m_image_list.Create(IDB_CHECK_ENGINE_LIST_ICONS, 16, 2, RGB(255,255,255));
}


void CCheckEngineTabDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CCheckEngineTabDlg)  
  DDX_Control(pDX, IDC_CHECK_ENGINE_QUICK_HELP, m_quick_help_text);
  DDX_Control(pDX, IDC_CHECK_ENGINE_ERRORS_LIST, m_errors_list);
  //}}AFX_DATA_MAP
}

LPCTSTR CCheckEngineTabDlg::GetDialogID(void) const
{
  return (LPCTSTR)IDD; 
}


BEGIN_MESSAGE_MAP(CCheckEngineTabDlg, CDialog)
  //{{AFX_MSG_MAP(CCheckEngineTabDlg)	
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCheckEngineTabDlg message handlers

BOOL CCheckEngineTabDlg::OnInitDialog() 
{
  CDialog::OnInitDialog();
		

  m_quick_help_text.SetWindowText("\
    Режим считывания ошибок в реальном времени позволяет видеть возникающие\
 и исчезающие ошибки. Это полезно для выяснения причины возникновения конкретной\
 ошибки. \n    В список сохраненных ошибок попадают все когда-либо\
 (даже еднократно) возникавшие ошибки.");

  m_errors_list.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES | LVS_EX_GRIDLINES);	

  //устанавливаем картинки состояния для чекбоксов...
  m_errors_list.SetImageList(&m_image_list,LVSIL_STATE);

  m_errors_list.InsertColumn(0,_T("Состояние"),LVCFMT_LEFT,70);
  m_errors_list.InsertColumn(1,_T("Описание ошибки"),LVCFMT_LEFT,450);


  /////////////////////////////////////////////////
  m_errors_list.InsertItem(0,_T(""));
  m_errors_list.SetItemText(0,1,_T("Error"));
  m_errors_list.SetCheck(0,TRUE);
  /////////////////////////////////////////////////

  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}


void CCheckEngineTabDlg::OnClose() 
{
  // TODO: Add your message handler code here and/or call default
	
  //CDialog::OnClose();
}
