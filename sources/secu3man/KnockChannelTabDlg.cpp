
#include "stdafx.h"
#include "resource.h"
#include "KnockChannelTabDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CKnockChannelTabDlg::CKnockChannelTabDlg(CWnd* pParent /*=NULL*/)
: CTabDialog(CKnockChannelTabDlg::IDD, pParent)
{
  //{{AFX_DATA_INIT(CKnockChannelTabDlg)
	// NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT
}


void CKnockChannelTabDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CKnockChannelTabDlg)  
  //}}AFX_DATA_MAP
}

LPCTSTR CKnockChannelTabDlg::GetDialogID(void) const
{
  return (LPCTSTR)IDD; 
}


BEGIN_MESSAGE_MAP(CKnockChannelTabDlg, CDialog)
  //{{AFX_MSG_MAP(CKnockChannelTabDlg)	
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CKnockChannelTabDlg message handlers

BOOL CKnockChannelTabDlg::OnInitDialog() 
{
  CDialog::OnInitDialog();
		
  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}


void CKnockChannelTabDlg::OnClose() 
{
  // TODO: Add your message handler code here and/or call default
	
  //CDialog::OnClose();
}
