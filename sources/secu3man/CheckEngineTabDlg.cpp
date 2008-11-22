
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
}


void CCheckEngineTabDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CCheckEngineTabDlg)  
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
		
  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}


void CCheckEngineTabDlg::OnClose() 
{
  // TODO: Add your message handler code here and/or call default
	
  //CDialog::OnClose();
}
