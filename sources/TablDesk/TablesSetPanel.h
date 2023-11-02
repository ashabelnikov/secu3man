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

/** \file TablesSetPanel.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include <memory>
#include <vector>
#include "common/FastDelegate.h"
#include "ui-core/ListCtrlEx.h"

class CToolTipCtrlEx;
class CWndScroller;
class CButtonsPanel;
class CSeptabsPanel;

/////////////////////////////////////////////////////////////////////////////
// CTablesSetPanel dialog

class AFX_EXT_CLASS CTablesSetPanel : public CDialog
{
  typedef CDialog Super;
  typedef fastdelegate::FastDelegate0<bool> EventResult;
  typedef fastdelegate::FastDelegate1<int> EventWithCode;
  typedef fastdelegate::FastDelegate2<int,CString> EventWithCodeAndString;

 public:
  CTablesSetPanel();
  virtual ~CTablesSetPanel();
  virtual BOOL Create(CWnd* pParentWnd = NULL);
  //note: use wnd_insert_after parameter to change tab order!
  void SetPosition(int x_pos, int y_pos, CWnd* wnd_insert_after = NULL);

  void OnShow(bool show);

  void SetFunSetListBox(const std::vector<_TSTRING>& i_list_of_names);
  void SetFunSetListBoxSelection(int i_selected_index);
  void setIsAllowed(EventResult IsFunction);

  //returns NULL if corresponding window wasn't opened
  HWND GetMapWindow(int wndType);

  void UpdateOpenedCharts(void);

  void CloseCharts(void);

  void ShowOpenedCharts(bool i_show);

  void MakeChartsChildren(bool children);

  void EnableToggleMapWnd(bool toggle);

  void EnableEmbedMapWnd(bool embed, const CRect& rc);

  std::auto_ptr<CButtonsPanel> mp_ButtonsPanel;  //set of maps
  std::auto_ptr<CSeptabsPanel> mp_SeptabsPanel;  //group of separate maps

 public: //set event handlers
  void setOnFunSetSelectionChanged(EventWithCode OnFunction);
  void setOnFunSetNamechanged(EventWithCodeAndString OnFunction);

// Implementation
 protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  virtual BOOL OnInitDialog();
  afx_msg void OnDestroy();
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnUpdateControls(CCmdUI* pCmdUI);
  afx_msg void OnChangeFunsetList(NMHDR* pNMHDR, LRESULT* pResult);
  afx_msg void OnEndLabelEditFunsetList(NMHDR* pNMHDR, LRESULT* pResult);
  DECLARE_MESSAGE_MAP()

  bool IsAllowed(void);

 private:
  CListCtrlEx m_funset_listbox;

 private:
  EventWithCode m_OnFunSetSelectionChanged;
  EventWithCodeAndString m_OnFunSetNamechanged;
  EventResult   m_IsAllowed;

  std::auto_ptr<CToolTipCtrlEx> mp_ttc;
  std::auto_ptr<CWndScroller> mp_scr;
};
