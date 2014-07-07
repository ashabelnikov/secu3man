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

#pragma once

#include <map>
#include <memory>
#include "common/fastdelegate.h"
#include "common/unicodesupport.h"
#include "ui-core/TabDialog.h"

class CHeaderCtrlEx;

class CCheckEngineTabDlg : public CTabDialog
{
  typedef CTabDialog Super;
  typedef fastdelegate::FastDelegate0<> EventHandler;

 public:
  CCheckEngineTabDlg(CWnd* pParent = NULL);   // standard constructor
  static const UINT IDD;
  virtual LPCTSTR GetDialogID(void) const;

  void setOnRealTimeErrors(EventHandler OnFunction) {m_OnRealTimeErrors = OnFunction;}
  void setOnReadSavedErrors(EventHandler OnFunction) {m_OnReadSavedErrors = OnFunction;}
  void setOnWriteSavedErrors(EventHandler OnFunction) {m_OnWriteSavedErrors = OnFunction;}
  void setOnListSetAllErrors(EventHandler OnFunction) {m_OnListSetAllErrors = OnFunction;}
  void setOnListClearAllErrors(EventHandler OnFunction) {m_OnListClearAllErrors = OnFunction;}

  bool GetRealTimeErrorsCheck(void) const;

  void AppendErrorsList(size_t i_id, const _TSTRING& i_bc, const _TSTRING& i_description, bool i_state = false);
  void ResetErrorsList(void);
  void SetErrorState(size_t i_id, bool i_state);
  bool GetErrorState(size_t i_id) const;
  void SetInertShow(bool i_inert);
  bool GetInertShow(void) const;

  void EnableAll(bool i_enable);
  void EnableRWButtons(bool i_enable);

 // Implementation
 protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  virtual BOOL OnInitDialog();
  afx_msg void OnClose();
  afx_msg void OnRealTimeErrorsCheckbox();
  afx_msg void OnReadSavedErrors();
  afx_msg void OnWriteSavedErrors();
  afx_msg void OnUpdateControls(CCmdUI* pCmdUI);
  afx_msg void OnUpdateInertnessCheckbox(CCmdUI* pCmdUI);
  afx_msg void OnUpdateRWButtons(CCmdUI* pCmdUI);
  afx_msg void OnDestroy();
  afx_msg void OnTimer(UINT nIDEvent);
  afx_msg void OnListSetAllErrors();
  afx_msg void OnListClearAllErrors();
  afx_msg void OnCustomdrawList(NMHDR*, LRESULT*);
  DECLARE_MESSAGE_MAP()

 private:
  CStatic m_quick_help_text;
  CListCtrl m_errors_list;
  CImageList m_image_list;
  CButton m_realtime_checkbox;
  CButton m_show_with_inertness;
  CButton m_read_saved_button;
  CButton m_write_saved_button;
  CButton m_list_set_all_button;
  CButton m_list_clear_all_button;

  EventHandler  m_OnRealTimeErrors;
  EventHandler  m_OnReadSavedErrors;
  EventHandler  m_OnWriteSavedErrors;
  EventHandler  m_OnListSetAllErrors;
  EventHandler  m_OnListClearAllErrors;

  //Key = ID, Value = index of item in the list
  typedef std::map<size_t,size_t> Indexes;
  Indexes m_list_items_indexes;
  size_t m_list_next_item_index;

  bool m_all_enabled;
  bool m_rw_buttons_enabled;

  COLORREF m_gray_text_color;
  COLORREF m_normal_text_color;

  std::auto_ptr<CHeaderCtrlEx> m_header_ctrl;
};
