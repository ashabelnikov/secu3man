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
 FLAG_START_MAP = 0,
 FLAG_IDLE_MAP,
 FLAG_WORK_MAP,
 FLAG_TEMP_MAP,
 FLAG_VE_MAP,       //inj
 FLAG_AFR_MAP,      //inj
 FLAG_CRNK_MAP,     //inj
 FLAG_WRMP_MAP,     //inj
 FLAG_DEAD_MAP,     //inj
 FLAG_IDLR_MAP,     //inj
 FLAG_IDLC_MAP,     //inj
 FLAG_AETPS_MAP,    //inj
 FLAG_AERPM_MAP,    //inj
 FLAG_AFTSTR_MAP,   //inj
 FLAG_IT_MAP,       //inj
 FLAG_ITRPM_MAP,    //inj
 FLAG_RIGID_MAP,    //inj
 FLAG_EGOCRV_MAP,   //inj
 FLAG_IACCORRW_MAP, //inj
 FLAG_IACCORR_MAP,  //inj
 FLAG_IATCLT_MAP,   //inj
 FLAG_TPSSWT_MAP,   //inj
 FLAG_GTSC_MAP,     //inj
 FLAG_GPSC_MAP,     //inj
 FLAG_ATSC_MAP,     //inj
 FLAG_DWLCNTR_MAP,  //separate
 FLAG_ATTEN_MAP,    //separate
 FLAG_CTS_MAP,      //separate
 FLAG_ATS_MAP,      //separate
 FLAG_ATSAAC_MAP,   //separate
 FLAG_GASDOSE_MAP,  //separate
 FLAG_BAROCORR_MAP, //separate 
 FLAG_MANIT_MAP,    //separate 
 FLAG_TMP2CURVE_MAP,//separate 
 FLAG_CRKTEMP_MAP,  //separate 
 FLAG_EHPAUSE_MAP,  //separate 
 FLAG_NR_OF_FLAGS
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
  DECLARE_MESSAGE_MAP()

 private:
  CButton   m_exchange_button;
  CListCtrl m_other_fwd_list;
  CListCtrl m_current_fwd_list;
  CString   m_current_fwd_title_string;
  CString   m_other_fwd_title_string;
  CFont     m_exbtnFont;

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
};
