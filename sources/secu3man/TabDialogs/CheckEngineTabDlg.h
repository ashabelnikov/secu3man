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

/** \file CheckEngineTabDlg.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include <map>
#include <memory>
#include <vector>
#include "common/fastdelegate.h"
#include "common/unicodesupport.h"
#include "ui-core/TabDialog.h"
#include "ui-core/ListCtrlEx.h"
#include "ui-core/MapEditorCtrl.h"

class CHeaderCtrlEx;
class CToolTipCtrlEx;
class CLabel;

class CCheckEngineTabDlg : public CTabDialog
{
  typedef CTabDialog Super;
  typedef fastdelegate::FastDelegate1<int> EventWithCode;
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
  void setOnTrimtabButton(EventHandler OnFunction) {m_OnTrimtabButton = OnFunction;}
  void setOnTrimtabReadButton(EventHandler OnFunction) {m_OnTrimtabReadButton = OnFunction;}
  void setOnTrimtabResetButton(EventHandler OnFunction) {m_OnTrimtabResetButton = OnFunction;}
  void setOnTrimtabSaveButton(EventHandler OnFunction) {m_OnTrimtabSaveButton = OnFunction;}
  void setOnTrimtabExportMenu(EventWithCode OnFunction) {m_OnTrimtabExportMenu = OnFunction;}

  bool GetRealTimeErrorsCheck(void) const;

  void AppendErrorsList(size_t i_id, const _TSTRING& i_bc, const _TSTRING& i_description, bool i_state = false);
  void ResetErrorsList(void);
  void SetErrorState(size_t i_id, bool i_state);
  bool GetErrorState(size_t i_id) const;
  void SetInertShow(bool i_inert);
  bool GetInertShow(void) const;

  void EnableAll(bool i_enable);
  void EnableRWButtons(bool i_enable);

  void SetLocale(int locale);
  bool GetTrimtabButtonState(void);
  void BindMaps(float* pTrimtab, float* pTrimtab2);
  void BindRPMGrid(float* pGrid);
  void BindLoadGrid(float* pGrid);
  void EnableTrimtab(bool i_enable);
  void UpdateView(bool axisLabels = false);
  void SetArguments(int rpm, float load, bool strt_use);

  void EnableTrimtabFWExport(bool en);
  void EnableTrimtabEEExport(bool en);
  void SetTrimtabExpMenuStrings(const std::vector<_TSTRING> &strings, bool eeprom);

  void ShowFuelInjNote(bool show);

 // Implementation
 protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  virtual BOOL OnInitDialog();
  afx_msg void OnClose();
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnRealTimeErrorsCheckbox();
  afx_msg void OnReadSavedErrors();
  afx_msg void OnWriteSavedErrors();
  afx_msg void OnUpdateControls(CCmdUI* pCmdUI);
  afx_msg void OnUpdateInertnessCheckbox(CCmdUI* pCmdUI);
  afx_msg void OnUpdateRWButtons(CCmdUI* pCmdUI);
  afx_msg void OnUpdateControlsLTFT(CCmdUI* pCmdUI);
  afx_msg void OnUpdateControlsLTFTFWExp(CCmdUI* pCmdUI);
  afx_msg void OnUpdateControlsLTFTEEExp(CCmdUI* pCmdUI);
  afx_msg void OnDestroy();
  afx_msg void OnTimer(UINT nIDEvent);
  afx_msg void OnListSetAllErrors();
  afx_msg void OnListClearAllErrors();
  afx_msg void OnCustomdrawList(NMHDR*, LRESULT*);
  afx_msg void OnPaint();
  afx_msg void OnTrimTableButton();
  afx_msg void OnTrimtabReadButton();
  afx_msg void OnTrimtabResetButton();
  afx_msg void OnTrimtabSaveButton();
  afx_msg void OnTrimtabExportButton();
  afx_msg void OnTrimtabExportMenu(UINT id);
  afx_msg void OnTrimTab1Button();
  afx_msg void OnTrimTab2Button();
  DECLARE_MESSAGE_MAP()

 private:
  virtual void OnShow(bool show);
  void OnCeresetLinkClick(void);

  CStatic m_quick_help_text;
  CStatic m_ltft_unit_text;
  CStatic m_ltft_fuelinj_text;
  CListCtrlEx m_errors_list;
  CImageList m_image_list;
  CButton m_realtime_checkbox;
  CButton m_show_with_inertness;
  CButton m_read_saved_button;
  CButton m_write_saved_button;
  CButton m_list_set_all_button;
  CButton m_list_clear_all_button;
  CButton m_trimtab_button;
  CMapEditorCtrl m_ltft_map;
  CMapEditorCtrl m_ltft2_map;
  CButton m_ltft_read_button;
  CButton m_ltft_reset_button;
  CButton m_ltft_save_button;
  CButton m_ltft_export_button;
  CMenu m_ltftexp_menu;
  CFont m_font;
  CButton m_ltft_radio;
  CButton m_ltft2_radio;

  EventHandler  m_OnRealTimeErrors;
  EventHandler  m_OnReadSavedErrors;
  EventHandler  m_OnWriteSavedErrors;
  EventHandler  m_OnListSetAllErrors;
  EventHandler  m_OnListClearAllErrors;
  EventHandler  m_OnTrimtabButton;
  EventHandler  m_OnTrimtabReadButton;
  EventHandler  m_OnTrimtabResetButton;
  EventHandler  m_OnTrimtabSaveButton;
  EventWithCode m_OnTrimtabExportMenu;

  //Key = ID, Value = index of item in the list
  typedef std::map<size_t,size_t> Indexes;
  Indexes m_list_items_indexes;
  size_t m_list_next_item_index;

  bool m_all_enabled;
  bool m_rw_buttons_enabled;
  bool m_initialized;

  COLORREF m_gray_text_color;
  COLORREF m_normal_text_color;

  std::auto_ptr<CHeaderCtrlEx> m_header_ctrl;
  std::auto_ptr<CToolTipCtrlEx> mp_ttc;
  std::auto_ptr<CLabel> mp_ceresetLink;

  int m_locale; //0 - english, 1 - russian

  const float* mp_trimTab;
  const float* mp_trimTab2;
  const float* mp_rpmGrid;
  const float* mp_loadGrid;
  bool m_trimtab_enabled; //trim table functionality

  bool m_trimtab_fwexp_enabled;
  bool m_trimtab_eeexp_enabled;
  bool m_fuelinj_note;
};

