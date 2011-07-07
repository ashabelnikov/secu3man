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

#pragma once

#include <vector>
#include "ButtonsPanel.h"
#include "common/FastDelegate.h"
#include "common/unicodesupport.h"

/////////////////////////////////////////////////////////////////////////////
// CTablesSetPanel dialog

class AFX_EXT_CLASS CTablesSetPanel : public CButtonsPanel
{
  typedef CButtonsPanel Super;
  typedef fastdelegate::FastDelegate2<int,CString> EventWithCodeAndString;

 public:
  CTablesSetPanel(CWnd* pParent = NULL);   // standard constructor
  static const UINT IDD;

  void SetFunSetListBox(std::vector<_TSTRING> i_list_of_names);
  void SetFunSetListBoxSelection(int i_selected_index);

  float* GetAttenuatorMap(bool i_original);
  float* GetCoilRegulMap(bool i_original);

  //returns NULL if corresponding window wasn't opened
  virtual HWND GetMapWindow(int wndType);

  virtual void UpdateOpenedCharts(void);

  void EnableCoilRegulation(bool enable);

 public: //установка обработчиков событий
  void setOnFunSetSelectionChanged(EventWithCode OnFunction);
  void setOnFunSetNamechanged(EventWithCodeAndString OnFunction);

// Implementation
 protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  virtual BOOL OnInitDialog();
  afx_msg void OnViewAttenuatorMap();
  afx_msg void OnViewCoilRegulMap();
  afx_msg void OnUpdateViewAttenuatorMap(CCmdUI* pCmdUI);
  afx_msg void OnUpdateViewCoilRegulMap(CCmdUI* pCmdUI);
  afx_msg void OnUpdateControls(CCmdUI* pCmdUI);
  afx_msg void OnChangeFunsetList(NMHDR* pNMHDR, LRESULT* pResult);
  afx_msg void OnEndLabelEditFunsetList(NMHDR* pNMHDR, LRESULT* pResult);
  DECLARE_MESSAGE_MAP()

  virtual bool IsAllowed(void);

 private:
  CListCtrl m_funset_listbox;
  CButton   m_view_attenuator_map_btn;
  CButton   m_view_coilregul_map_btn;

 private:
  EventWithCode m_OnFunSetSelectionChanged;
  EventWithCodeAndString m_OnFunSetNamechanged;

  static void __cdecl OnChangeAttenuatorTable(void* i_param);
  static void __cdecl OnCloseAttenuatorTable(void* i_param);
  static void __cdecl OnChangeCoilRegulTable(void* i_param);
  static void __cdecl OnCloseCoilRegulTable(void* i_param);
  static void __cdecl OnGetYAxisLabel(LPTSTR io_label_string, void* i_param);

  bool m_coilreg_enabled;
  int m_attenuator_map_chart_state;
  int m_coilregul_map_chart_state;

  HWND m_attenuator_map_wnd_handle;
  HWND m_coilregul_map_wnd_handle;

  float m_attenuator_table_slots[128];

  float m_attenuator_map_active[128];
  float m_attenuator_map_original[128];

  float m_coilregul_map_active[32];
  float m_coilregul_map_original[32];
};
