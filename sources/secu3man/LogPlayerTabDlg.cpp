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

#include <limits>

using namespace std;

#undef max //numeric_limits<>::max() cases warning C4003: not enough actual parameters for macro...

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
 DDX_Control(pDX, IDC_LOG_PLAYER_TIME_FACTOR_COMBO, m_time_factor_combo);
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

void CLogPlayerTabDlg::FillTimeFactorCombo(std::vector<_TSTRING>& i_factor_strings)
{
 for(std::vector<_TSTRING>::iterator it = i_factor_strings.begin(); it != i_factor_strings.end(); ++it)
 {
  int index = m_time_factor_combo.AddString((*it).c_str());
  ASSERT(index != LB_ERR);
  m_time_factor_combo.SetItemData(index, std::distance(i_factor_strings.begin(), it));
 }

}

size_t CLogPlayerTabDlg::GetTimeFactor(void) const
{
 int index = m_time_factor_combo.GetCurSel(); 
 return (index != LB_ERR ? m_time_factor_combo.GetItemData(index) : numeric_limits<size_t>::max());
}

void CLogPlayerTabDlg::SetTimeFactor(size_t i_factor_id)
{
 int count = m_time_factor_combo.GetCount();
 for(int i = 0; i < count; ++i)
 {
  size_t id = m_time_factor_combo.GetItemData(i);
  if (id==i_factor_id)
  {
   m_time_factor_combo.SetCurSel(i);
   return;
  }
 }
 ASSERT(0); //unknown item id!
}
