/* SECU-3  - An open source, free engine control unit
   Copyright (C) 2007 Alexey A. Shabelnikov. Ukraine, Kiev

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

/** \file MapImpExpDlg.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "resource.h"
#include "MapImpExpDlg.h"
#include "io-core/SECU3IO.h"
#include "ui-core/fnt_helpers.h"
#include "ui-core/ToolTipCtrlEx.h"

const UINT CMapImpExpDlg::IDD = IDD_MAP_IMPEXP;

#define WM_KICKIDLE 0x036A //hack

/////////////////////////////////////////////////////////////////////////////
// CMapImpExpDlg dialog

BEGIN_MESSAGE_MAP(CMapImpExpDlg, Super)
 ON_BN_CLICKED(IDC_MAP_IMPEXP_EXCHANGE_BUTTON, OnMapImpexpExchangeButton)
 ON_NOTIFY(LVN_ITEMCHANGED, IDC_MAP_IMPEXP_CURRENT_FWD_LIST, OnChangeFWDCurrentList)
 ON_NOTIFY(LVN_ENDLABELEDIT, IDC_MAP_IMPEXP_CURRENT_FWD_LIST, OnEndLabelEditFWDCurrentList)
 ON_NOTIFY(LVN_ITEMCHANGED, IDC_MAP_IMPEXP_OTHER_FWD_LIST, OnChangeFWDOtherList)
 ON_NOTIFY(LVN_ENDLABELEDIT, IDC_MAP_IMPEXP_OTHER_FWD_LIST, OnEndLabelEditFWDOtherList)
 ON_UPDATE_COMMAND_UI(IDC_MAP_IMPEXP_EXCHANGE_BUTTON, OnUpdateExchangeButton )
 ON_MESSAGE(WM_KICKIDLE, OnKickIdle)
END_MESSAGE_MAP()

CMapImpExpDlg::CMapImpExpDlg(CWnd* pParent /*=NULL*/)
: Super(CMapImpExpDlg::IDD, pParent)
{
 m_current_fwd_title_string = _T("");
 m_other_fwd_title_string = _T("");
 m_fwd_flags[FLAG_START_MAP] = FALSE;
 m_fwd_flags[FLAG_IDLE_MAP] = FALSE;
 m_fwd_flags[FLAG_WORK_MAP] = FALSE;
 m_fwd_flags[FLAG_TEMP_MAP] = FALSE;
 //fuel injection
 m_fwd_flags[FLAG_VE_MAP] = FALSE;
 m_fwd_flags[FLAG_AFR_MAP] = FALSE;
 m_fwd_flags[FLAG_CRNK_MAP] = FALSE;
 m_fwd_flags[FLAG_WRMP_MAP] = FALSE;
 m_fwd_flags[FLAG_DEAD_MAP] = FALSE;
 m_fwd_flags[FLAG_IDLR_MAP] = FALSE;
 m_fwd_flags[FLAG_IDLC_MAP] = FALSE;
 m_fwd_flags[FLAG_AETPS_MAP] = FALSE;
 m_fwd_flags[FLAG_AERPM_MAP] = FALSE;
 m_fwd_flags[FLAG_AFTSTR_MAP] = FALSE;
 //separate maps
 m_fwd_flags[FLAG_DWLCNTR_MAP] = FALSE;
 m_fwd_flags[FLAG_ATTEN_MAP] = FALSE;
 m_fwd_flags[FLAG_CTS_MAP] = FALSE;
 m_fwd_flags[FLAG_CHOKE_MAP] = FALSE;
 m_fwd_flags[FLAG_ATS_MAP] = FALSE;
 m_fwd_flags[FLAG_ATSAAC_MAP] = FALSE;
 m_fwd_flags[FLAG_GASDOSE_MAP] = FALSE;
}

void CMapImpExpDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);
 DDX_Control(pDX, IDC_MAP_IMPEXP_EXCHANGE_BUTTON, m_exchange_button);
 DDX_Control(pDX, IDC_MAP_IMPEXP_OTHER_FWD_LIST, m_other_fwd_list);
 DDX_Control(pDX, IDC_MAP_IMPEXP_CURRENT_FWD_LIST, m_current_fwd_list);
 DDX_Text(pDX, IDC_MAP_IMPEXP_CURRENT_FWD_TITLE, m_current_fwd_title_string);
 DDX_Text(pDX, IDC_MAP_IMPEXP_OTHER_FWD_TITLE, m_other_fwd_title_string);
 //ignition
 DDX_Check(pDX, IDC_MAP_IMPEXP_STARTMAP_FLAG,m_fwd_flags[FLAG_START_MAP]);
 DDX_Check(pDX, IDC_MAP_IMPEXP_IDLEMAP_FLAG, m_fwd_flags[FLAG_IDLE_MAP]);
 DDX_Check(pDX, IDC_MAP_IMPEXP_WORKMAP_FLAG, m_fwd_flags[FLAG_WORK_MAP]);
 DDX_Check(pDX, IDC_MAP_IMPEXP_TEMPMAP_FLAG, m_fwd_flags[FLAG_TEMP_MAP]);
 //fuel injection
 DDX_Check(pDX, IDC_MAP_IMPEXP_VEMAP_FLAG, m_fwd_flags[FLAG_VE_MAP]);
 DDX_Check(pDX, IDC_MAP_IMPEXP_AFRMAP_FLAG, m_fwd_flags[FLAG_AFR_MAP]);
 DDX_Check(pDX, IDC_MAP_IMPEXP_CRNKMAP_FLAG, m_fwd_flags[FLAG_CRNK_MAP]);
 DDX_Check(pDX, IDC_MAP_IMPEXP_WRMPMAP_FLAG, m_fwd_flags[FLAG_WRMP_MAP]);
 DDX_Check(pDX, IDC_MAP_IMPEXP_DEADMAP_FLAG, m_fwd_flags[FLAG_DEAD_MAP]);
 DDX_Check(pDX, IDC_MAP_IMPEXP_IDLRMAP_FLAG, m_fwd_flags[FLAG_IDLR_MAP]);
 DDX_Check(pDX, IDC_MAP_IMPEXP_IDLCMAP_FLAG, m_fwd_flags[FLAG_IDLC_MAP]);
 DDX_Check(pDX, IDC_MAP_IMPEXP_AETPSMAP_FLAG, m_fwd_flags[FLAG_AETPS_MAP]);
 DDX_Check(pDX, IDC_MAP_IMPEXP_AERPMMAP_FLAG, m_fwd_flags[FLAG_AERPM_MAP]);
 DDX_Check(pDX, IDC_MAP_IMPEXP_AFTSTRMAP_FLAG, m_fwd_flags[FLAG_AFTSTR_MAP]);
 //separate
 DDX_Check(pDX, IDC_MAP_IMPEXP_DWELLCNTRL_FLAG, m_fwd_flags[FLAG_DWLCNTR_MAP]);
 DDX_Check(pDX, IDC_MAP_IMPEXP_ATTENUATOR_FLAG, m_fwd_flags[FLAG_ATTEN_MAP]);
 DDX_Check(pDX, IDC_MAP_IMPEXP_CTS_FLAG, m_fwd_flags[FLAG_CTS_MAP]);
 DDX_Check(pDX, IDC_MAP_IMPEXP_CHOKE_FLAG, m_fwd_flags[FLAG_CHOKE_MAP]);
 DDX_Check(pDX, IDC_MAP_IMPEXP_ATS_FLAG, m_fwd_flags[FLAG_ATS_MAP]);
 DDX_Check(pDX, IDC_MAP_IMPEXP_ATSAAC_FLAG, m_fwd_flags[FLAG_ATSAAC_MAP]);
 DDX_Check(pDX, IDC_MAP_IMPEXP_GASDOSE_FLAG, m_fwd_flags[FLAG_GASDOSE_MAP]);

 //ignition
 DDX_Control(pDX, IDC_MAP_IMPEXP_STARTMAP_FLAG,m_fwd_flags_buttons[FLAG_START_MAP]);
 DDX_Control(pDX, IDC_MAP_IMPEXP_IDLEMAP_FLAG, m_fwd_flags_buttons[FLAG_IDLE_MAP]);
 DDX_Control(pDX, IDC_MAP_IMPEXP_WORKMAP_FLAG, m_fwd_flags_buttons[FLAG_WORK_MAP]);
 DDX_Control(pDX, IDC_MAP_IMPEXP_TEMPMAP_FLAG, m_fwd_flags_buttons[FLAG_TEMP_MAP]);
 //fuel injection
 DDX_Control(pDX, IDC_MAP_IMPEXP_VEMAP_FLAG, m_fwd_flags_buttons[FLAG_VE_MAP]);
 DDX_Control(pDX, IDC_MAP_IMPEXP_AFRMAP_FLAG, m_fwd_flags_buttons[FLAG_AFR_MAP]);
 DDX_Control(pDX, IDC_MAP_IMPEXP_CRNKMAP_FLAG, m_fwd_flags_buttons[FLAG_CRNK_MAP]);
 DDX_Control(pDX, IDC_MAP_IMPEXP_WRMPMAP_FLAG, m_fwd_flags_buttons[FLAG_WRMP_MAP]);
 DDX_Control(pDX, IDC_MAP_IMPEXP_DEADMAP_FLAG, m_fwd_flags_buttons[FLAG_DEAD_MAP]);
 DDX_Control(pDX, IDC_MAP_IMPEXP_IDLRMAP_FLAG, m_fwd_flags_buttons[FLAG_IDLR_MAP]);
 DDX_Control(pDX, IDC_MAP_IMPEXP_IDLCMAP_FLAG, m_fwd_flags_buttons[FLAG_IDLC_MAP]);
 DDX_Control(pDX, IDC_MAP_IMPEXP_AETPSMAP_FLAG, m_fwd_flags_buttons[FLAG_AETPS_MAP]);
 DDX_Control(pDX, IDC_MAP_IMPEXP_AERPMMAP_FLAG, m_fwd_flags_buttons[FLAG_AERPM_MAP]);
 DDX_Control(pDX, IDC_MAP_IMPEXP_AFTSTRMAP_FLAG, m_fwd_flags_buttons[FLAG_AFTSTR_MAP]);
 //separate
 DDX_Control(pDX, IDC_MAP_IMPEXP_DWELLCNTRL_FLAG, m_fwd_flags_buttons[FLAG_DWLCNTR_MAP]);
 DDX_Control(pDX, IDC_MAP_IMPEXP_ATTENUATOR_FLAG, m_fwd_flags_buttons[FLAG_ATTEN_MAP]);
 DDX_Control(pDX, IDC_MAP_IMPEXP_CTS_FLAG, m_fwd_flags_buttons[FLAG_CTS_MAP]);
 DDX_Control(pDX, IDC_MAP_IMPEXP_CHOKE_FLAG, m_fwd_flags_buttons[FLAG_CHOKE_MAP]);
 DDX_Control(pDX, IDC_MAP_IMPEXP_ATS_FLAG, m_fwd_flags_buttons[FLAG_ATS_MAP]);
 DDX_Control(pDX, IDC_MAP_IMPEXP_ATSAAC_FLAG, m_fwd_flags_buttons[FLAG_ATSAAC_MAP]);
 DDX_Control(pDX, IDC_MAP_IMPEXP_GASDOSE_FLAG, m_fwd_flags_buttons[FLAG_GASDOSE_MAP]);
}

/////////////////////////////////////////////////////////////////////////////
// CMapImpExpDlg message handlers

LRESULT CMapImpExpDlg::OnKickIdle(WPARAM wParam, LPARAM lParam)
{
 UpdateDialogControls(this, FALSE);
 return 0L;
}

void CMapImpExpDlg::OnUpdateExchangeButton(CCmdUI* pCmdUI)
{
 if (m_IsExchengeButtonAllowed)
 {
  bool enable = m_IsExchengeButtonAllowed();
  pCmdUI->Enable(enable ? TRUE : FALSE);
 }
 else
 {
  pCmdUI->Enable(FALSE);
 }
}

void CMapImpExpDlg::OnMapImpexpExchangeButton()
{
 if (m_OnExchangeButton)
  m_OnExchangeButton();
}

//----------------------------------------------------------------------------
//изменилось выделение в спимке семейств характеристик
void CMapImpExpDlg::OnChangeFWDCurrentList(NMHDR* pNMHDR, LRESULT* pResult)
{
 NMLISTVIEW *pnmv = (NM_LISTVIEW FAR *) pNMHDR;

 if (pnmv->uChanged == LVIF_STATE && (pnmv->uNewState & LVIS_SELECTED))
 {
  //состояние выделения изменилось
  int selected_index = 0;

  //will return -1 if there are no selected items, at all
  selected_index = m_current_fwd_list.GetNextItem(-1, LVNI_SELECTED);

  if (m_OnFWDCurrentSelectionChanged)
   m_OnFWDCurrentSelectionChanged(selected_index);
 }

 *pResult = 0;
}

void CMapImpExpDlg::OnEndLabelEditFWDCurrentList(NMHDR* pNMHDR, LRESULT* pResult)
{
 NMLVDISPINFO *pdi = (LV_DISPINFO FAR *) pNMHDR;

 if (pdi->item.mask == LVIF_TEXT)
 {
  m_current_fwd_list.SetItem(&pdi->item);
  if (m_OnFWDCurrentListNamechanged)
   m_OnFWDCurrentListNamechanged(pdi->item.iItem,pdi->item.pszText);
 }
}

void CMapImpExpDlg::FillFWDCurrentList(std::vector<_TSTRING> i_list_of_names)
{
 m_current_fwd_list.DeleteAllItems();
 for (size_t i = 0; i < i_list_of_names.size(); i++)
 {
  m_current_fwd_list.InsertItem(i,i_list_of_names[i].c_str());
 }
}

void CMapImpExpDlg::SetFWDCurrentListSelection(int i_selected_index)
{
 if (i_selected_index!=-1)
  m_current_fwd_list.SetItemState(i_selected_index, LVIS_SELECTED, LVIS_SELECTED);
}

int CMapImpExpDlg::GetFWDCurrentListSelection(void)
{
 //will return -1 if there are no selected items, at all
 int selected_index = m_current_fwd_list.GetNextItem(-1, LVNI_SELECTED);
 return selected_index;
}

void CMapImpExpDlg::SetFWDCurrentListTitle(const _TSTRING& i_title)
{
 m_current_fwd_title_string = i_title.c_str();
 UpdateData(FALSE);
}

//----------------------------------------------------------------------------
//изменилось выделение в спимке семейств характеристик
void CMapImpExpDlg::OnChangeFWDOtherList(NMHDR* pNMHDR, LRESULT* pResult)
{
 NMLISTVIEW *pnmv = (NM_LISTVIEW FAR *) pNMHDR;

 if (pnmv->uChanged == LVIF_STATE && (pnmv->uNewState & LVIS_SELECTED))
 {
  //состояние выделения изменилось
  int selected_index = 0;

  //will return -1 if there are no selected items, at all
  selected_index = m_other_fwd_list.GetNextItem(-1, LVNI_SELECTED);

  if (m_OnFWDOtherSelectionChanged)
   m_OnFWDOtherSelectionChanged(selected_index);
 }

 *pResult = 0;
}

void CMapImpExpDlg::OnEndLabelEditFWDOtherList(NMHDR* pNMHDR, LRESULT* pResult)
{
 NMLVDISPINFO *pdi = (LV_DISPINFO FAR *) pNMHDR;

 if (pdi->item.mask == LVIF_TEXT)
 {
  m_other_fwd_list.SetItem(&pdi->item);
  if (m_OnFWDOtherListNamechanged)
   m_OnFWDOtherListNamechanged(pdi->item.iItem,pdi->item.pszText);
 }
}

void CMapImpExpDlg::FillFWDOtherList(std::vector<_TSTRING> i_list_of_names)
{
 m_other_fwd_list.DeleteAllItems();
 for (size_t i = 0; i < i_list_of_names.size(); i++)
 {
  m_other_fwd_list.InsertItem(i,i_list_of_names[i].c_str());
 }
}

void CMapImpExpDlg::SetFWDOtherListSelection(int i_selected_index)
{
 if (i_selected_index!=-1)
  m_other_fwd_list.SetItemState(i_selected_index, LVIS_SELECTED, LVIS_SELECTED);
}

int CMapImpExpDlg::GetFWDOtherListSelection(void)
{
 //will return -1 if there are no selected items, at all
 int selected_index = m_other_fwd_list.GetNextItem(-1, LVNI_SELECTED);
 return selected_index;
}

void CMapImpExpDlg::SetFWDOtherListTitle(const _TSTRING& i_title)
{
 m_other_fwd_title_string = i_title.c_str();
 UpdateData(FALSE);
}

void CMapImpExpDlg::SetFWDFlag(EFWDFlags i_flag_type, bool i_state)
{
 m_fwd_flags[i_flag_type] = i_state ? TRUE : FALSE;
 UpdateData(FALSE);
}

bool CMapImpExpDlg::GetFWDFlag(EFWDFlags i_flag_type)
{
 UpdateData();
 bool state = m_fwd_flags[i_flag_type] ? true : false;
 return state;
}

void CMapImpExpDlg::EnableFWDFlag(EFWDFlags i_flag_type, bool i_enable)
{
 BOOL enable = i_enable ? TRUE : FALSE;
 m_fwd_flags_buttons[i_flag_type].EnableWindow(enable);

 //if all check boxes are disabled then disable corresponding group box
 enable = ((m_fwd_flags_buttons[FLAG_ATTEN_MAP].IsWindowEnabled()) || (m_fwd_flags_buttons[FLAG_DWLCNTR_MAP].IsWindowEnabled()) ||
           (m_fwd_flags_buttons[FLAG_CTS_MAP].IsWindowEnabled()) || (m_fwd_flags_buttons[FLAG_CHOKE_MAP].IsWindowEnabled()) ||
           (m_fwd_flags_buttons[FLAG_ATS_MAP].IsWindowEnabled()) || (m_fwd_flags_buttons[FLAG_ATSAAC_MAP].IsWindowEnabled()) ||
           (m_fwd_flags_buttons[FLAG_GASDOSE_MAP].IsWindowEnabled()));
 GetDlgItem(IDC_MAP_IMPEXP_SEPTAB_GROUP)->EnableWindow(enable);
}

void CMapImpExpDlg::OnOK()
{
 //Before closing dialog, notify controller
 if (m_OnOkButton)
  m_OnOkButton();

 Super::OnOK();
}

void CMapImpExpDlg::OnCancel()
{
 //Before closing dialog, notify controller
 if (m_OnCancelButton)
  m_OnCancelButton();

 Super::OnCancel();
}

BOOL CMapImpExpDlg::OnInitDialog()
{
 Super::OnInitDialog();

 if (m_OnActivate)
  m_OnActivate();

 CloneWndFont(&m_exchange_button, &m_exbtnFont, 16, true);
 m_exchange_button.SetFont(&m_exbtnFont);

 //create a tooltip control and assign tooltips
 mp_ttc.reset(new CToolTipCtrlEx());
 VERIFY(mp_ttc->Create(this, WS_POPUP | TTS_ALWAYSTIP | TTS_BALLOON));
 //ignition
 VERIFY(mp_ttc->AddWindow(&m_fwd_flags_buttons[FLAG_START_MAP], MLL::GetString(IDS_MAP_IMPEXP_STARTMAP_FLAG_TT)));
 VERIFY(mp_ttc->AddWindow(&m_fwd_flags_buttons[FLAG_IDLE_MAP], MLL::GetString(IDS_MAP_IMPEXP_STARTMAP_FLAG_TT)));
 VERIFY(mp_ttc->AddWindow(&m_fwd_flags_buttons[FLAG_WORK_MAP], MLL::GetString(IDS_MAP_IMPEXP_STARTMAP_FLAG_TT)));
 VERIFY(mp_ttc->AddWindow(&m_fwd_flags_buttons[FLAG_TEMP_MAP], MLL::GetString(IDS_MAP_IMPEXP_STARTMAP_FLAG_TT)));
 //fuel injection
 VERIFY(mp_ttc->AddWindow(&m_fwd_flags_buttons[FLAG_VE_MAP], MLL::GetString(IDS_MAP_IMPEXP_STARTMAP_FLAG_TT)));
 VERIFY(mp_ttc->AddWindow(&m_fwd_flags_buttons[FLAG_AFR_MAP], MLL::GetString(IDS_MAP_IMPEXP_STARTMAP_FLAG_TT)));
 VERIFY(mp_ttc->AddWindow(&m_fwd_flags_buttons[FLAG_CRNK_MAP], MLL::GetString(IDS_MAP_IMPEXP_STARTMAP_FLAG_TT)));
 VERIFY(mp_ttc->AddWindow(&m_fwd_flags_buttons[FLAG_WRMP_MAP], MLL::GetString(IDS_MAP_IMPEXP_STARTMAP_FLAG_TT)));
 VERIFY(mp_ttc->AddWindow(&m_fwd_flags_buttons[FLAG_DEAD_MAP], MLL::GetString(IDS_MAP_IMPEXP_STARTMAP_FLAG_TT)));
 VERIFY(mp_ttc->AddWindow(&m_fwd_flags_buttons[FLAG_IDLR_MAP], MLL::GetString(IDS_MAP_IMPEXP_STARTMAP_FLAG_TT)));
 VERIFY(mp_ttc->AddWindow(&m_fwd_flags_buttons[FLAG_IDLC_MAP], MLL::GetString(IDS_MAP_IMPEXP_STARTMAP_FLAG_TT)));
 VERIFY(mp_ttc->AddWindow(&m_fwd_flags_buttons[FLAG_AETPS_MAP], MLL::GetString(IDS_MAP_IMPEXP_STARTMAP_FLAG_TT)));
 VERIFY(mp_ttc->AddWindow(&m_fwd_flags_buttons[FLAG_AERPM_MAP], MLL::GetString(IDS_MAP_IMPEXP_STARTMAP_FLAG_TT)));
 VERIFY(mp_ttc->AddWindow(&m_fwd_flags_buttons[FLAG_AFTSTR_MAP], MLL::GetString(IDS_MAP_IMPEXP_STARTMAP_FLAG_TT)));
 //separate
 VERIFY(mp_ttc->AddWindow(&m_fwd_flags_buttons[FLAG_DWLCNTR_MAP], MLL::GetString(IDS_MAP_IMPEXP_DWELLCNTRL_FLAG_TT)));
 VERIFY(mp_ttc->AddWindow(&m_fwd_flags_buttons[FLAG_ATTEN_MAP], MLL::GetString(IDS_MAP_IMPEXP_DWELLCNTRL_FLAG_TT)));
 VERIFY(mp_ttc->AddWindow(&m_fwd_flags_buttons[FLAG_CTS_MAP], MLL::GetString(IDS_MAP_IMPEXP_DWELLCNTRL_FLAG_TT)));
 VERIFY(mp_ttc->AddWindow(&m_fwd_flags_buttons[FLAG_CHOKE_MAP], MLL::GetString(IDS_MAP_IMPEXP_DWELLCNTRL_FLAG_TT)));
 VERIFY(mp_ttc->AddWindow(&m_fwd_flags_buttons[FLAG_ATS_MAP], MLL::GetString(IDS_MAP_IMPEXP_DWELLCNTRL_FLAG_TT)));
 VERIFY(mp_ttc->AddWindow(&m_fwd_flags_buttons[FLAG_ATSAAC_MAP], MLL::GetString(IDS_MAP_IMPEXP_DWELLCNTRL_FLAG_TT)));
 VERIFY(mp_ttc->AddWindow(&m_fwd_flags_buttons[FLAG_GASDOSE_MAP], MLL::GetString(IDS_MAP_IMPEXP_DWELLCNTRL_FLAG_TT)));

 mp_ttc->SetMaxTipWidth(250); //Enable text wrapping
 mp_ttc->ActivateToolTips(true);

 return TRUE;  // return TRUE unless you set the focus to a control
}

void CMapImpExpDlg::SetExchangeButtonCaption(const _TSTRING& i_text)
{
 m_exchange_button.SetWindowText(i_text.c_str());
}
