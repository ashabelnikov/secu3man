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
#include <vector>

#include "common/ParamPageEvents.h"
#include "common/unicodesupport.h"
#include "ITablesDeskView.h"
#include "ui-core/UpdatableDialog.h"

class CButtonsPanel;
class CTDContextMenuManager;


/////////////////////////////////////////////////////////////////////////////
// CTablesDeskDlg dialog

class AFX_EXT_CLASS CTablesDeskDlg : public CModelessUpdatableDialog, public ITablesDeskView, public ParamPageEvents
{
 typedef CModelessUpdatableDialog Super;
 typedef fastdelegate::FastDelegate0<> EventHandler;

 public:
  CTablesDeskDlg(CWnd* pParent = NULL);
  virtual ~CTablesDeskDlg();
  static UINT IDD;
  static UINT IDD_F;

  //note: wnd_insert_after parameter may be used to fix tab order!
  void SetPosition(int x_pos, int y_pos, CWnd* wnd_insert_after = NULL);
  void SetTitle(const CString& i_str);
  void GetTitle(CString& o_str);

  //----------interface implementation---------------------------
  virtual bool IsEnabled(void);
  virtual void Enable(bool enable);
  virtual void Show(bool show);
  virtual void ShowSaveButton(bool i_show);
  virtual void ShowOpenedCharts(bool i_show);  
  virtual void UpdateOpenedCharts(void);
  virtual void SetReadOnlyTablesSetName(bool readonly);
  virtual void SetModificationFlag(bool value);
  virtual void MakeChartsChildren(bool children);
  virtual void SetFunctionsNames(const std::vector<_TSTRING>& i_fwnames, const std::vector<_TSTRING>& i_eenames, int sep_index); //Set names of read-only and read/write tables
  virtual void SetRPMGrid(const float* values);

  //events
  virtual void setOnMapChanged(EventWithCode OnFunction);
  virtual void setOnCloseMapWnd(EventWithHWND OnFunction);
  virtual void setOnOpenMapWnd(EventWithHWND OnFunction);
  virtual void setOnTabActivate(EventHandler OnFunction);
  virtual void setOnSaveButton(EventHandler OnFunction);
  virtual void setOnChangeTablesSetName(EventHandler OnFunction);
  virtual void setOnLoadTablesFrom(EventWithCode OnFunction);
  virtual void setOnSaveTablesTo(EventWithCode OnFunction);

  virtual void SetTablesSetName(const _TSTRING& name);
  virtual _TSTRING GetTablesSetName(void) const;
  virtual float* GetMap(int i_mapType, bool i_original);

  virtual void SetDynamicValues(int rpm, float temp, int air_flow, float adv_ang, float knock_retard, bool knkret_use,
   float strt_aalt, bool strt_use, float idle_aalt, bool idle_use, float work_aalt, bool work_use, float temp_aalt, bool temp_use,
   float airt_aalt, bool airt_use, float idlreg_aac, bool idlreg_use, float octan_aac, bool octan_use);
  virtual void EnableAdvanceAngleIndication(bool i_enable);
  //-------------------------------------------------------------

  //CButtonsPanel
  std::auto_ptr<CButtonsPanel> mp_ButtonsPanel;

 // Implementation
 protected:
  virtual void DoDataExchange(CDataExchange* pDX);// DDX/DDV support
  virtual BOOL OnInitDialog();
  afx_msg void OnDestroy();
  afx_msg void OnUpdateControls(CCmdUI* pCmdUI);
  afx_msg void OnSaveButton();
  afx_msg void OnSysCommand(UINT, LONG);
  afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
  afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
  afx_msg void OnRangeCmdsLoad(UINT nID);
  afx_msg void OnRangeCmdsSave(UINT nID);
  afx_msg void OnEditKillFocus();
  afx_msg void OnChangeTablesSetName();
  DECLARE_MESSAGE_MAP()

 private:
  //events from CTablesPageDlg->CButtonsPanel
  void OnMapChanged(int i_mapType);
  void OnCloseMapWnd(HWND i_hwnd, int i_mapType);
  void OnOpenMapWnd(HWND i_hwnd, int i_mapType);
  void OnWndActivation(HWND i_hwnd, long cmd);

  //from CButtonsPanel
  bool IsAllowed(void);

  void _MakeWindowChild(HWND hwnd, bool child);
 
  bool m_children_charts;
  bool m_enabled;
  bool m_tsneb_readonly;
  bool m_lock_enchange;
  bool m_lock_killfocus;

  CStatic m_td_title;
  CButton m_save_button;
  CEdit m_names_edit;
  CStatic m_midification_flag;
  std::auto_ptr<CTDContextMenuManager> mp_ContextMenuManager;

  //обработчики событий (делегаты)
  EventWithCode m_OnMapChanged;
  EventWithHWND m_OnCloseMapWnd;
  EventWithHWND m_OnOpenMapWnd;
  EventHandler m_OnTabActivate;
  EventHandler m_OnSaveButton;
  EventHandler m_OnChangeTablesSetName;
  EventWithCode m_OnLoadTablesFrom;
  EventWithCode m_OnSaveTablesTo;
};

/////////////////////////////////////////////////////////////////////////////
