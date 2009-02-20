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
#include "common\unicodesupport.h"
#include "ui-core\HeaderCtrlEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#pragma warning( disable : 4800 ) //: forcing value to bool 'true' or 'false' (performance warning)

#define TIMER_ID 0

CCheckEngineTabDlg::CCheckEngineTabDlg(CWnd* pParent /*=NULL*/)
: CTabDialog(CCheckEngineTabDlg::IDD, pParent)
, m_all_enabled(true)
, m_rw_buttons_enabled(true)
, m_header_ctrl(new CHeaderCtrlEx())
{
 m_image_list.Create(IDB_CHECK_ENGINE_LIST_ICONS, 16, 2, RGB(255,255,255));
 m_gray_text_color = ::GetSysColor(COLOR_GRAYTEXT); 
 m_normal_text_color = ::GetSysColor(COLOR_BTNTEXT);
}

void CCheckEngineTabDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CCheckEngineTabDlg)  
  DDX_Control(pDX, IDC_CHECK_ENGINE_QUICK_HELP, m_quick_help_text);
  DDX_Control(pDX, IDC_CHECK_ENGINE_ERRORS_LIST, m_errors_list);
  DDX_Control(pDX, IDC_CHECK_ENGINE_READ_REALTIME_CHECKBOX, m_realtime_checkbox);
  DDX_Control(pDX, IDC_CHECK_ENGINE_READ_ERRORS_BUTTON, m_read_saved_button);
  DDX_Control(pDX, IDC_CHECK_ENGINE_WRITE_ERRORS_BUTTON, m_write_saved_button);
  DDX_Control(pDX, IDC_CHECK_ENGINE_LIST_SETALL_BUTTON, m_list_set_all_button);
  DDX_Control(pDX, IDC_CHECK_ENGINE_LIST_CLEARALL_BUTTON, m_list_clear_all_button);
  //}}AFX_DATA_MAP
}

LPCTSTR CCheckEngineTabDlg::GetDialogID(void) const
{
  return (LPCTSTR)IDD; 
}


BEGIN_MESSAGE_MAP(CCheckEngineTabDlg, CDialog)
  //{{AFX_MSG_MAP(CCheckEngineTabDlg)	
  ON_WM_CLOSE()
  ON_WM_DESTROY()
  ON_WM_TIMER()
  ON_BN_CLICKED(IDC_CHECK_ENGINE_READ_REALTIME_CHECKBOX, OnRealTimeErrorsCheckbox)
  ON_BN_CLICKED(IDC_CHECK_ENGINE_READ_ERRORS_BUTTON, OnReadSavedErrors)
  ON_BN_CLICKED(IDC_CHECK_ENGINE_WRITE_ERRORS_BUTTON, OnWriteSavedErrors)
  ON_BN_CLICKED(IDC_CHECK_ENGINE_LIST_SETALL_BUTTON, OnListSetAllErrors)
  ON_BN_CLICKED(IDC_CHECK_ENGINE_LIST_CLEARALL_BUTTON, OnListClearAllErrors)

  ON_UPDATE_COMMAND_UI(IDC_CHECK_ENGINE_QUICK_HELP, OnUpdateControls)
  ON_UPDATE_COMMAND_UI(IDC_CHECK_ENGINE_ERRORS_LIST, OnUpdateControls)
  ON_UPDATE_COMMAND_UI(IDC_CHECK_ENGINE_READ_REALTIME_CHECKBOX, OnUpdateControls)
  ON_UPDATE_COMMAND_UI(IDC_CHECK_ENGINE_READ_ERRORS_BUTTON, OnUpdateRWButtons)
  ON_UPDATE_COMMAND_UI(IDC_CHECK_ENGINE_WRITE_ERRORS_BUTTON, OnUpdateRWButtons)
  ON_UPDATE_COMMAND_UI(IDC_CHECK_ENGINE_LIST_SETALL_BUTTON, OnUpdateControls)
  ON_UPDATE_COMMAND_UI(IDC_CHECK_ENGINE_LIST_CLEARALL_BUTTON, OnUpdateControls)
  ON_NOTIFY(NM_CUSTOMDRAW, IDC_CHECK_ENGINE_ERRORS_LIST, OnCustomdrawList)
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
  m_errors_list.SetImageList(&m_image_list, LVSIL_STATE);

  m_errors_list.InsertColumn(0,_T("Состояние"),LVCFMT_LEFT,70);
  m_errors_list.InsertColumn(1,_T("Описание ошибки"),LVCFMT_LEFT,450);

  SetTimer(TIMER_ID,250,NULL);

  ResetErrorsList();

  m_header_ctrl->Init(m_errors_list.GetHeaderCtrl());
  m_header_ctrl->SetTextColor(m_gray_text_color);
  
  UpdateDialogControls(this,TRUE);
  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}

void CCheckEngineTabDlg::OnRealTimeErrorsCheckbox()
{
  if (m_OnRealTimeErrors)
    m_OnRealTimeErrors();
}  

void CCheckEngineTabDlg::OnReadSavedErrors()
{
  if (m_OnReadSavedErrors)
    m_OnReadSavedErrors();
}  

void CCheckEngineTabDlg::OnWriteSavedErrors()
{
  if (m_OnWriteSavedErrors)
    m_OnWriteSavedErrors();
}  

bool CCheckEngineTabDlg::GetRealTimeErrorsCheck(void) const
{
 return m_realtime_checkbox.GetCheck();
}

void CCheckEngineTabDlg::AppendErrorsList(size_t i_id, const _TSTRING& i_description, bool i_state /* = false */)
{
 m_errors_list.InsertItem(m_list_next_item_index,_T(""));
 m_errors_list.SetItemText(m_list_next_item_index, 1, i_description.c_str());
 m_errors_list.SetCheck(m_list_next_item_index, i_state);
 m_list_items_indexes.insert(Indexes::value_type(i_id, m_list_next_item_index));
 m_list_next_item_index++;
}

void CCheckEngineTabDlg::ResetErrorsList(void)
{
 m_list_next_item_index = 0;
 m_list_items_indexes.clear();
 m_errors_list.DeleteAllItems();
}

void CCheckEngineTabDlg::SetErrorState(size_t i_id, bool i_state)
{
 bool valid_id = m_list_items_indexes.find(i_id)!=m_list_items_indexes.end();
 _ASSERTE(valid_id);
 if (false==valid_id)
   return;
 m_errors_list.SetCheck(m_list_items_indexes[i_id], i_state); 
}


bool CCheckEngineTabDlg::GetErrorState(size_t i_id) const
{
 Indexes::const_iterator it = m_list_items_indexes.find(i_id);
 _ASSERTE(it!=m_list_items_indexes.end());
 if (it==m_list_items_indexes.end())
   return false;
 return m_errors_list.GetCheck((*it).second); 
}


void CCheckEngineTabDlg::EnableAll(bool i_enable)
{
 m_all_enabled = i_enable; //remember state 
 m_header_ctrl->SetTextColor(m_all_enabled ? m_normal_text_color : m_gray_text_color);
}

void CCheckEngineTabDlg::EnableRWButtons(bool i_enable)
{
 m_rw_buttons_enabled = i_enable; //remember state
}


void CCheckEngineTabDlg::OnUpdateControls(CCmdUI* pCmdUI) 
{
 pCmdUI->Enable(m_all_enabled);  
}

void CCheckEngineTabDlg::OnUpdateRWButtons(CCmdUI* pCmdUI) 
{
 pCmdUI->Enable(m_rw_buttons_enabled && m_all_enabled);  
}

void CCheckEngineTabDlg::OnClose() 
{	
  //CDialog::OnClose();
}

void CCheckEngineTabDlg::OnDestroy() 
{
  CDialog::OnDestroy();
  KillTimer(TIMER_ID);		
}

void CCheckEngineTabDlg::OnTimer(UINT nIDEvent) 
{
  //dirty hack
  UpdateDialogControls(this,TRUE);
  CDialog::OnTimer(nIDEvent);  
}

void CCheckEngineTabDlg::OnListSetAllErrors()
{
 if (m_OnListSetAllErrors)
  m_OnListSetAllErrors(); 
}

void CCheckEngineTabDlg::OnListClearAllErrors()
{
 if (m_OnListClearAllErrors)
  m_OnListClearAllErrors(); 
}

void CCheckEngineTabDlg::OnCustomdrawList ( NMHDR* pNMHDR, LRESULT* pResult )
{
 NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>( pNMHDR );

 // Take the default processing unless we set this to something else below.
 *pResult = 0;

 bool is_enabled = ::IsWindowEnabled(m_errors_list.m_hWnd);

 // If this is the beginning of the control's paint cycle, request
 // notifications for each item.
 if ( CDDS_PREPAINT == pLVCD->nmcd.dwDrawStage )
 {
  *pResult = CDRF_NOTIFYITEMDRAW;
 }
 else if ( CDDS_ITEMPREPAINT == pLVCD->nmcd.dwDrawStage )
 {
  // This is the pre-paint stage for an item.  We need to make another
  // request to be notified during the post-paint stage.
  if (!is_enabled)
   pLVCD->clrText = m_gray_text_color;
  *pResult = CDRF_NOTIFYPOSTPAINT;
 }
 else if ( CDDS_ITEMPOSTPAINT == pLVCD->nmcd.dwDrawStage )
 {  		
  int    nItem = static_cast<int>( pLVCD->nmcd.dwItemSpec );
  LVITEM rItem;
  ZeroMemory (&rItem, sizeof(LVITEM) );
  rItem.mask  = LVIF_IMAGE | LVIF_STATE;
  rItem.iItem = nItem;    
  rItem.stateMask = 0xFFFF;     // get all state flags
  m_errors_list.GetItem ( &rItem );

  CDC*  pDC = CDC::FromHandle ( pLVCD->nmcd.hdc );
  // Get the rect that holds the item's icon.
  CRect rcIcon;
  m_errors_list.GetItemRect ( nItem, &rcIcon, LVIR_BOUNDS );
  UINT nStateImageMask = rItem.state & LVIS_STATEIMAGEMASK;
  int nImage = (nStateImageMask >> 12) - 1;
  
  //TODO: What is the magic number - 2?
  DrawState(pDC->m_hDC,NULL,NULL,(LPARAM)m_image_list.ExtractIcon(nImage), 0, 
	  rcIcon.left+2, rcIcon.top, rcIcon.Width(), rcIcon.Height(),
	  DST_ICON| (is_enabled ? 0 : DSS_DISABLED));
    
  *pResult = CDRF_DODEFAULT;
 }	
}

