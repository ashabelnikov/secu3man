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
#include <vector>
#include "propgrid/custom/btncheckbox.h"
#include "common/FastDelegate.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace std;
using namespace fastdelegate;

CKnockChannelTabDlg::CKnockChannelTabDlg(CWnd* pParent /*=NULL*/)
: Super(CKnockChannelTabDlg::IDD, pParent)
{
  //{{AFX_DATA_INIT(CKnockChannelTabDlg)
	// NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT
}


void CKnockChannelTabDlg::DoDataExchange(CDataExchange* pDX)
{
  Super::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CKnockChannelTabDlg) 
  DDX_Control(pDX, IDC_PROPERTY_GRID, m_ctrlGrid);
  //}}AFX_DATA_MAP
}

LPCTSTR CKnockChannelTabDlg::GetDialogID(void) const
{
  return (LPCTSTR)IDD; 
}


BEGIN_MESSAGE_MAP(CKnockChannelTabDlg, Super)
  //{{AFX_MSG_MAP(CKnockChannelTabDlg)	
	ON_WM_DESTROY()
	ON_MESSAGE(WM_PG_ITEMCHANGED, OnItemChanged)
    ON_UPDATE_COMMAND_UI(IDC_PROPERTY_GRID, OnUpdatePropertyGrid)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CKnockChannelTabDlg message handlers

void CKnockChannelTabDlg::OnUpdatePropertyGrid(CCmdUI* pCmdUI) 
{
  pCmdUI->Enable(TRUE);
}

BOOL CKnockChannelTabDlg::OnInitDialog() 
{
  Super::OnInitDialog();

  HSECTION hs = m_ctrlGrid.AddSection("Параметры");

  m_ctrlGrid.AddBoolItem(hs, "Разрешить контроль детонации", true);

  CPropertyGridInPlaceEdit::InplaceEditParamsEx ex1;
  ex1.m_decimal_places = 2;
  ex1.m_spin = true;
  ex1.m_delta = 0.1;
  ex1.m_mode = CEditEx::MODE_FLOAT | CEditEx::MODE_SIGNED;
  ex1.m_lower = -15;
  ex1.m_upper = 25;
  ex1.m_limit_text = 6;
  m_ctrlGrid.AddDoubleItem(hs, "Начало фазового окна", 5,"%g°",true,false,-1,&ex1);

  CPropertyGridInPlaceEdit::InplaceEditParamsEx ex2;
  ex2.m_decimal_places = 2;
  ex2.m_spin = true;
  ex2.m_delta = 0.1;
  ex2.m_mode = CEditEx::MODE_FLOAT | CEditEx::MODE_SIGNED;
  ex2.m_lower = -15;
  ex2.m_upper = 25;
  ex2.m_limit_text = 6;
  m_ctrlGrid.AddDoubleItem(hs, "Конец фазового окна", 25,"%g°",true,false,-1,&ex2);

  vector<string> v;
  v.push_back("1.22");
  v.push_back("1.26");
  v.push_back("1.31");
  v.push_back("1.35");
  m_ctrlGrid.AddComboItem(hs, "Частота ПФ (кГц)", v, 2, true);

  m_item_attenuator_table_showhide.SetCheckedText(_T("Закрыть"));
  m_item_attenuator_table_showhide.SetUncheckedText(_T("Редактировать"));
  m_item_attenuator_table_showhide_item = m_ctrlGrid.AddCustomItem(hs, "Точка перекл. аттенюатора", &m_item_attenuator_table_showhide);
	
  m_ctrlGrid.SetFocusDisabled(false);
  m_ctrlGrid.SetEditable(true);

  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}


void CKnockChannelTabDlg::OnDestroy() 
{
  Super::OnDestroy();
  m_ctrlGrid.ResetContents();
}


LRESULT CKnockChannelTabDlg::OnItemChanged(WPARAM wParam, LPARAM lParam)
{
  if (wParam == m_item_attenuator_table_showhide_item)
  {
  
  }
  return 0;
}