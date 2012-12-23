/* SECU-3  - An open source, free engine control unit
   Copyright (C) 2007 Alexey A. Shabelnikov. Ukraine, Gorlovka

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

   contacts:
              http://secu-3.org
              email: shabelnikov@secu-3.org
*/

#include "stdafx.h"
#include "Resources/resource.h"
#include "CheckEngineTabDlg.h"
#include "common/DPIAware.h"
#include "common/unicodesupport.h"
#include "ui-core/HeaderCtrlEx.h"

#define TIMER_ID 0

const UINT CCheckEngineTabDlg::IDD = IDD_CHECK_ENGINE;

CCheckEngineTabDlg::CCheckEngineTabDlg(CWnd* pParent /*=NULL*/)
: Super(CCheckEngineTabDlg::IDD, pParent)
, m_all_enabled(false)
, m_rw_buttons_enabled(false)
, m_header_ctrl(new CHeaderCtrlEx())
{
 m_image_list.Create(IDB_CE_LIST_ICONS, 16, 2, RGB(255,255,255));
 m_gray_text_color = ::GetSysColor(COLOR_GRAYTEXT);
 m_normal_text_color = ::GetSysColor(COLOR_BTNTEXT);
}

void CCheckEngineTabDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);
 DDX_Control(pDX, IDC_CE_QUICK_HELP, m_quick_help_text);
 DDX_Control(pDX, IDC_CE_ERRORS_LIST, m_errors_list);
 DDX_Control(pDX, IDC_CE_READ_REALTIME_CHECKBOX, m_realtime_checkbox);
 DDX_Control(pDX, IDC_CE_READ_INERTNESS_CHECKBOX, m_show_with_inertness);
 DDX_Control(pDX, IDC_CE_READ_ERRORS_BUTTON, m_read_saved_button);
 DDX_Control(pDX, IDC_CE_WRITE_ERRORS_BUTTON, m_write_saved_button);
 DDX_Control(pDX, IDC_CE_LIST_SETALL_BUTTON, m_list_set_all_button);
 DDX_Control(pDX, IDC_CE_LIST_CLEARALL_BUTTON, m_list_clear_all_button);
}

LPCTSTR CCheckEngineTabDlg::GetDialogID(void) const
{
 return (LPCTSTR)IDD;
}

BEGIN_MESSAGE_MAP(CCheckEngineTabDlg, Super)
 ON_WM_DESTROY()
 ON_WM_TIMER()
 ON_BN_CLICKED(IDC_CE_READ_REALTIME_CHECKBOX, OnRealTimeErrorsCheckbox)
 ON_BN_CLICKED(IDC_CE_READ_ERRORS_BUTTON, OnReadSavedErrors)
 ON_BN_CLICKED(IDC_CE_WRITE_ERRORS_BUTTON, OnWriteSavedErrors)
 ON_BN_CLICKED(IDC_CE_LIST_SETALL_BUTTON, OnListSetAllErrors)
 ON_BN_CLICKED(IDC_CE_LIST_CLEARALL_BUTTON, OnListClearAllErrors)

 ON_UPDATE_COMMAND_UI(IDC_CE_QUICK_HELP, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_CE_ERRORS_LIST, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_CE_READ_REALTIME_CHECKBOX, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_CE_READ_INERTNESS_CHECKBOX, OnUpdateInertnessCheckbox)
 ON_UPDATE_COMMAND_UI(IDC_CE_READ_ERRORS_BUTTON, OnUpdateRWButtons)
 ON_UPDATE_COMMAND_UI(IDC_CE_WRITE_ERRORS_BUTTON, OnUpdateRWButtons)
 ON_UPDATE_COMMAND_UI(IDC_CE_LIST_SETALL_BUTTON, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_CE_LIST_CLEARALL_BUTTON, OnUpdateControls)
 ON_NOTIFY(NM_CUSTOMDRAW, IDC_CE_ERRORS_LIST, OnCustomdrawList)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCheckEngineTabDlg message handlers

BOOL CCheckEngineTabDlg::OnInitDialog()
{
 Super::OnInitDialog();

 m_quick_help_text.SetWindowText(MLL::LoadString(IDS_CEPAGE_QUICK_HELP_TEXT));

 m_errors_list.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES | LVS_EX_GRIDLINES);

 //устанавливаем картинки состояния для чекбоксов...
 m_errors_list.SetImageList(&m_image_list, LVSIL_STATE);

 DPIAware dpia;
 m_errors_list.InsertColumn(0,MLL::LoadString(IDS_CEPAGE_ERROR_STATE),LVCFMT_LEFT,dpia.ScaleX(70));
 m_errors_list.InsertColumn(1,MLL::LoadString(IDS_CEPAGE_ERROR_DESCRIPTION),LVCFMT_LEFT,450);

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

void CCheckEngineTabDlg::SetInertShow(bool i_inert)
{
 m_show_with_inertness.SetCheck(i_inert ? BST_CHECKED : BST_UNCHECKED);
}

bool CCheckEngineTabDlg::GetInertShow(void) const
{
 return m_show_with_inertness.GetCheck() == BST_CHECKED;
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

void CCheckEngineTabDlg::OnUpdateInertnessCheckbox(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_all_enabled && BST_CHECKED == m_realtime_checkbox.GetCheck());
}

void CCheckEngineTabDlg::OnUpdateRWButtons(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_rw_buttons_enabled && m_all_enabled);
}

void CCheckEngineTabDlg::OnDestroy()
{
 Super::OnDestroy();
 KillTimer(TIMER_ID);
}

void CCheckEngineTabDlg::OnTimer(UINT nIDEvent)
{
 //dirty hack
 UpdateDialogControls(this,TRUE);
 Super::OnTimer(nIDEvent);
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
