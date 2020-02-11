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

/** \file MapImpExpDlg.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include <vector>
#include "common/FastDelegate.h"
#include "common/UnicodeSupport.h"


//типы флажков определ€ющих какие данные перемещать
enum EFWDFlags
{
 FLAG_START_MAP     = 0,     //-ign
 FLAG_IDLE_MAP      = 1,     //-ign
 FLAG_WORK_MAP      = 2,     //-ign
 FLAG_TEMP_MAP      = 3,     //-ign
 FLAG_VE_MAP        = 4,     //inj
 FLAG_AFR_MAP       = 5,     //inj
 FLAG_CRNK_MAP      = 6,     //inj
 FLAG_WRMP_MAP      = 7,     //inj
 FLAG_DEAD_MAP      = 8,     //inj
 FLAG_IDLR_MAP      = 9,     //inj
 FLAG_IDLC_MAP      = 10,    //inj
 FLAG_AETPS_MAP     = 11,    //inj
 FLAG_AERPM_MAP     = 12,    //inj
 FLAG_AFTSTR_MAP    = 13,    //inj
 FLAG_IT_MAP        = 14,    //inj
 FLAG_ITRPM_MAP     = 15,    //inj
 FLAG_RIGID_MAP     = 16,    //inj
 FLAG_EGOCRV_MAP    = 17,    //inj
 FLAG_IACCORRW_MAP  = 18,    //inj
 FLAG_IACCORR_MAP   = 19,    //inj
 FLAG_IATCLT_MAP    = 20,    //inj
 FLAG_TPSSWT_MAP    = 21,    //inj
 FLAG_GTSC_MAP      = 22,    //inj
 FLAG_GPSC_MAP      = 23,    //inj
 FLAG_ATSC_MAP      = 24,    //inj

 FLAG_DWLCNTR_MAP   = 25,    //separate
 FLAG_ATTEN_MAP     = 26,    //separate
 FLAG_CTS_MAP       = 27,    //separate
 FLAG_ATS_MAP       = 28,    //separate
 FLAG_ATSAAC_MAP    = 29,    //separate
 FLAG_GASDOSE_MAP   = 30,    //separate
 FLAG_BAROCORR_MAP  = 31,    //separate
 FLAG_MANIT_MAP     = 32,    //separate
 FLAG_TMP2CURVE_MAP = 33,    //separate
 FLAG_CRKTEMP_MAP   = 34,    //separate
 FLAG_EHPAUSE_MAP   = 35,    //separate
 FLAG_CRNKTHRD_MAP  = 36,    //separate
 FLAG_CRNKTIME_MAP  = 37,    //separate
 FLAG_ABANTHRD_MAP  = 38,    //separate
 FLAG_CESETT_DAT    = 39,    //separate (pseudomap)

 FLAG_NR_OF_FLAGS   = 40,    //total number of flags

 FLAG_SET_START     = 0,
 FLAG_SET_END       = 24,
 FLAG_SEP_START     = 25,
 FLAG_SEP_END       = 39,
};

class CToolTipCtrlEx;
class CWndScroller;

//view
class CMapImpExpDlg : public CDialog
{
  typedef CDialog Super;
  typedef fastdelegate::FastDelegate0<> EventHandler;
  typedef fastdelegate::FastDelegate0<bool> EventResult;
  typedef fastdelegate::FastDelegate2<int, CString> EventWithCodeAndString;
  typedef fastdelegate::FastDelegate1<int> EventWithCode;

 public:
  CMapImpExpDlg(CWnd* pParent = NULL);   // standard constructor
  static const UINT IDD;

  void setOnExchangeButton(EventHandler OnFunction) {m_OnExchangeButton = OnFunction;};
  void setOnFWDCurrentListNamechanged(EventWithCodeAndString OnFunction) {m_OnFWDCurrentListNamechanged = OnFunction;};
  void setOnFWDCurrentSelectionChanged(EventWithCode OnFunction) {m_OnFWDCurrentSelectionChanged = OnFunction;};
  void setOnFWDOtherListNamechanged(EventWithCodeAndString OnFunction) {m_OnFWDOtherListNamechanged = OnFunction;};
  void setOnFWDOtherSelectionChanged(EventWithCode OnFunction) {m_OnFWDOtherSelectionChanged = OnFunction;};
  void setIsExchengeButtonAllowed(EventResult OnFunction) {m_IsExchengeButtonAllowed = OnFunction;};

  void setOnOkButton(EventHandler OnFunction) {m_OnOkButton = OnFunction;};
  void setOnCancelButton(EventHandler OnFunction) {m_OnCancelButton = OnFunction;};
  void setOnActivate(EventHandler OnActivate) {m_OnActivate = OnActivate;};

  void FillFWDCurrentList(std::vector<_TSTRING> i_list_of_names);
  void SetFWDCurrentListSelection(int i_selected_index);
  int  GetFWDCurrentListSelection(void);
  void SetFWDCurrentListTitle(const _TSTRING& i_title);

  void FillFWDOtherList(std::vector<_TSTRING> i_list_of_names);
  void SetFWDOtherListSelection(int i_selected_index);
  int  GetFWDOtherListSelection(void);
  void SetFWDOtherListTitle(const _TSTRING& i_title);

  void SetFWDFlag(EFWDFlags i_flag_type, bool i_state);
  bool GetFWDFlag(EFWDFlags i_flag_type);
  void EnableFWDFlag(EFWDFlags i_flag_type, bool i_enable);
  bool IsEnabledFWDFlag(EFWDFlags i_flag_type);
  void SetExchangeButtonCaption(const _TSTRING& i_text);

    // Implementation
 protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  virtual void OnOK();
  virtual void OnCancel();
  virtual BOOL OnInitDialog();
  afx_msg void OnDestroy();
  afx_msg void OnMapImpexpExchangeButton();
  afx_msg void OnChangeFWDCurrentList(NMHDR* pNMHDR, LRESULT* pResult);
  afx_msg void OnEndLabelEditFWDCurrentList(NMHDR* pNMHDR, LRESULT* pResult);
  afx_msg void OnChangeFWDOtherList(NMHDR* pNMHDR, LRESULT* pResult);
  afx_msg void OnEndLabelEditFWDOtherList(NMHDR* pNMHDR, LRESULT* pResult);
  afx_msg void OnUpdateExchangeButton(CCmdUI* pCmdUI);
  afx_msg LRESULT OnKickIdle(WPARAM, LPARAM);
  afx_msg void OnSetSetAllClick();
  afx_msg void OnSepSetAllClick();
  afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
  afx_msg void OnSize(UINT nType, int cx, int cy);
  DECLARE_MESSAGE_MAP()

 private:
  void _UpdateScrlViewSize(void);

  CButton   m_exchange_button;
  CListCtrl m_other_fwd_list;
  CListCtrl m_current_fwd_list;
  CString   m_current_fwd_title_string;
  CString   m_other_fwd_title_string;
  CFont     m_exbtnFont;
  CButton   m_set_all;
  CButton   m_sep_all;

  BOOL m_fwd_flags[FLAG_NR_OF_FLAGS];
  CButton m_fwd_flags_buttons[FLAG_NR_OF_FLAGS];

  std::auto_ptr<CToolTipCtrlEx> mp_ttc;
  std::auto_ptr<CWndScroller> mp_scr;

  EventHandler m_OnExchangeButton;
  EventWithCodeAndString m_OnFWDCurrentListNamechanged;
  EventWithCode m_OnFWDCurrentSelectionChanged;
  EventWithCodeAndString m_OnFWDOtherListNamechanged;
  EventWithCode m_OnFWDOtherSelectionChanged;
  EventHandler m_OnOkButton;
  EventHandler m_OnCancelButton;
  EventHandler m_OnActivate;
  EventResult m_IsExchengeButtonAllowed;

  CSize m_createSize;
  bool m_initialized;
};
