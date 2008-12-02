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

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace std;

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
  DDX_Control(pDX, IDC_PROPERTY_GRID, m_ctrlGrid);
  //}}AFX_DATA_MAP
}

LPCTSTR CKnockChannelTabDlg::GetDialogID(void) const
{
  return (LPCTSTR)IDD; 
}


BEGIN_MESSAGE_MAP(CKnockChannelTabDlg, CDialog)
  //{{AFX_MSG_MAP(CKnockChannelTabDlg)	
	ON_WM_CLOSE()
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
  CDialog::OnInitDialog();

  HSECTION hs = m_ctrlGrid.AddSection("Basic Items");
  m_ctrlGrid.AddStringItem(hs, "String", "A single line string item");
  m_ctrlGrid.AddTextItem(hs, "Text", "A multi line text item.\r\nSecond line...");
  m_ctrlGrid.AddStringItem(hs, "Disabled item", "A disabled item", false);

  CPropertyGridInPlaceEdit::InplaceEditParamsEx ex1;
  ex1.m_decimal_places = 4;
  ex1.m_spin = true;
  ex1.m_delta = 1;
  ex1.m_mode = CEditEx::MODE_HEX;
  ex1.m_lower = 0;
  ex1.m_upper = 150;
  ex1.m_limit_text = 8;
  m_ctrlGrid.AddHexItem(hs, "Integer", 10,"%x",true,false,-1,&ex1);

  m_ctrlGrid.AddIntegerItem(hs, "Formatted Integer", 8, "%d inches");

  CPropertyGridInPlaceEdit::InplaceEditParamsEx ex2;
  ex2.m_decimal_places = 2;
  ex2.m_delta = 0.25;
  ex2.m_spin = true;
  ex2.m_mode = CEditEx::MODE_FLOAT;
  ex2.m_lower = -50;
  ex2.m_upper = 150;
  m_ctrlGrid.AddDoubleItem(hs, "Double", 7.33,"",true,false,-1,&ex2);

  CPropertyGridInPlaceEdit::InplaceEditParamsEx ex3;
  ex3.m_decimal_places = 2;
  ex3.m_delta = 0.25;
  ex3.m_mode = CEditEx::MODE_FLOAT;
  ex3.m_spin = true;
  ex3.m_lower = -50;
  ex3.m_upper = 150;
  m_ctrlGrid.AddDoubleItem(hs, "Formatted Double", 10.25, "%gmm",true,false,-1,&ex3);

  m_ctrlGrid.AddBoolItem(hs, "Boolean", true);

/*  // custom items
 // hs = m_ctrlGrid.AddSection("Custom Items");
//  m_ctrlGrid.AddCustomItem(hs, "Custom In-place Edit", &m_item_serial);
//  m_ctrlGrid.AddCustomItem(hs, "Custom Modal Edit", &m_item_rect);
//  m_ctrlGrid.AddCustomItem(hs, "Ccustom Dropdown Edit", &m_item_tree);
 // m_ctrlGrid.AddCustomItem(hs, "Full Custom Edit", &m_item_gradient);

  // more items inserted before the custom ones
  hs = m_ctrlGrid.AddSection("More Items", true, hs);

  // a combo
  vector<string> v;
  v.push_back("English");
  v.push_back("French");
  v.push_back("German");
  v.push_back("Spanish");
  m_ctrlGrid.AddComboItem(hs, "Combo", v, 2, true);

  // some "simple"    
  m_ctrlGrid.AddDateItem(hs, "Date", COleDateTime::GetCurrentTime());
  m_ctrlGrid.AddDateItem(hs, "Formatted Date", COleDateTime::GetCurrentTime(), "%A, %d %B %Y");
  m_ctrlGrid.AddDateTimeItem(hs, "Date Time", COleDateTime::GetCurrentTime());
  m_ctrlGrid.AddColorItem(hs, "Color", RGB(255,156,12));
  m_ctrlGrid.AddFileItem(hs, "File", "C:\\AUTOEXEC.BAT", "All Files (*.*)|*.*||");
  m_ctrlGrid.AddFolderItem(hs, "Folder", "C:\\", "Select a folder");
*/
  // a font
 /* LOGFONT lf;
  CFont font;
  font.CreatePointFont(80, "Tahoma");
  font.GetLogFont(&lf);
  m_ctrlGrid.AddFontItem(hs, "Font", lf, true);*/

 /* // customization
  hs = m_ctrlGrid.AddSection("Grid customization");
  m_hItemShade = m_ctrlGrid.AddBoolItem(hs, "Shade titles", m_ctrlGrid.GetShadeTitles());
  m_hItemLines = m_ctrlGrid.AddBoolItem(hs, "Draw lines", m_ctrlGrid.GetDrawLines());
  m_hItemGutter = m_ctrlGrid.AddBoolItem(hs, "Draw gutter", m_ctrlGrid.GetDrawGutter());
  m_hItemFocusDis = m_ctrlGrid.AddBoolItem(hs, "Focus disabled items", m_ctrlGrid.GetFocusDisabled());*/

  // stuff
  //m_ctrlGrid.SetOkCancelStrings("OK", "Annuler");
  //m_ctrlGrid.SetDateTimeStrings("Date", "Heure");

  //m_ctrlGrid.SetDisplayMode(CPropertyGrid::DM_CATEGORIZED);

  //m_ctrlGrid.EnableWindow(TRUE);


		
  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}


void CKnockChannelTabDlg::OnClose() 
{
  // TODO: Add your message handler code here and/or call default
	
  //CDialog::OnClose();
}


LRESULT CKnockChannelTabDlg::OnItemChanged(WPARAM wParam, LPARAM lParam)
{
/*  if (wParam == m_hItemLines)
  {
    bool draw_lines;
    m_ctrlGrid.GetItemValue(m_hItemLines, draw_lines);
    m_ctrlGrid.SetDrawLines(draw_lines);
  }
  else if (wParam == m_hItemShade)
  {
    bool shade_titles;
    m_ctrlGrid.GetItemValue(m_hItemShade, shade_titles);
    m_ctrlGrid.SetShadeTitles(shade_titles);
  }
  else if (wParam == m_hItemGutter)
  {
    bool draw_gutter;
    m_ctrlGrid.GetItemValue(m_hItemGutter, draw_gutter);
    m_ctrlGrid.SetDrawGutter(draw_gutter);
  }
  else if (wParam == m_hItemFocusDis)
  {
    bool focus_disabled;
    m_ctrlGrid.GetItemValue(m_hItemFocusDis, focus_disabled);
    m_ctrlGrid.SetFocusDisabled(focus_disabled);
  }*/

  return 0;
}