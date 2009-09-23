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
#include "LogPlayerTabDlg.h"
#include "common/FastDelegate.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace fastdelegate;

CLogPlayerTabDlg::CLogPlayerTabDlg(CWnd* pParent /*=NULL*/)
: CTabDialog(CLogPlayerTabDlg::IDD, pParent)
{
 //todo
}


void CLogPlayerTabDlg::DoDataExchange(CDataExchange* pDX)
{
 CDialog::DoDataExchange(pDX);
}

LPCTSTR CLogPlayerTabDlg::GetDialogID(void) const
{
 return (LPCTSTR)IDD; 
}


BEGIN_MESSAGE_MAP(CLogPlayerTabDlg, CDialog)
 ON_WM_CLOSE()
END_MESSAGE_MAP()


BOOL CLogPlayerTabDlg::OnInitDialog() 
{
 CDialog::OnInitDialog();
	
 m_MIDeskDlg.Create(CMIDeskDlg::IDD, this);
 m_MIDeskDlg.MoveWindow(280,0,427,312);
 m_MIDeskDlg.ShowWindow(SW_SHOWNORMAL);
 m_MIDeskDlg.Show(true);

 UpdateDialogControls(this,TRUE);	
 return TRUE;
}

void CLogPlayerTabDlg::OnClose() 
{	
//	CDialog::OnClose();
}
