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

/** \file TablesDeskDlg.h
 * \author Alexey A. Shabelnikov
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
  virtual void TransformValues(void);
  virtual void SetReadOnlyTablesSetName(bool readonly);
  virtual void SetModificationFlag(bool value);
  virtual void MakeChartsChildren(bool children);
  virtual void EnableToggleMapWnd(bool toggle);
  virtual void SetFunctionsNames(const std::vector<_TSTRING>& i_fwnames, const std::vector<_TSTRING>& i_eenames, int sep_index); //Set names of read-only and read/write tables
  virtual void SetRPMGrid(const float* values);
  virtual void SetCLTGrid(const float* values);
  virtual void SetLoadGrid(const float* values);
  virtual void CloseAllCharts(void);

  //events
  virtual void setOnMapChanged(EventWithCode OnFunction);
  virtual void setOnCloseMapWnd(EventWithHWND OnFunction);
  virtual void setOnOpenMapWnd(EventWithHWND OnFunction);
  virtual void setOnSaveButton(EventHandler OnFunction);
  virtual void setOnChangeTablesSetName(EventHandler OnFunction);
  virtual void setOnLoadTablesFrom(EventWithCode OnFunction);
  virtual void setOnSaveTablesTo(EventWithCode OnFunction);
  virtual void setOnImportFromS3F(EventHandler OnFunction);
  virtual void setOnExportToS3F(EventHandler OnFunction);

  virtual void SetTablesSetName(const _TSTRING& name);
  virtual _TSTRING GetTablesSetName(void) const;
  virtual float* GetMap(int i_mapType, bool i_original);

  virtual void SetDynamicValues(int rpm, float temp, int air_flow, float adv_ang, float knock_retard, bool knkret_use,
   float strt_aalt, bool strt_use, float idle_aalt, bool idle_use, float work_aalt, bool work_use, float temp_aalt, bool temp_use,
   float airt_aalt, bool airt_use, float idlreg_aac, bool idlreg_use, float octan_aac, bool octan_use, float tps, float iac_pos,
   int tpsdot, float voltage, float add_i1, float tmp2, float baro_press, float load, float afr, bool acceleration, bool ie, float air_temp,
   float rigid_arg, bool rigid_use, float map2, int rxlaf, bool aftstr_enr, int mapdot);
  virtual void EnableAdvanceAngleIndication(bool i_enable);
  //-------------------------------------------------------------

  void OnShow(bool show);

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
  afx_msg void OnRangeCmdsLoad(UINT nID);
  afx_msg void OnRangeCmdsSave(UINT nID);
  afx_msg void OnImportFromS3F(void);
  afx_msg void OnExportToS3F(void);
  afx_msg void OnEditKillFocus();
  afx_msg void OnChangeTablesSetName();
  virtual BOOL PreTranslateMessage(MSG* pMsg);
  afx_msg void OnSize(UINT nType, int cx, int cy);
  DECLARE_MESSAGE_MAP()

 private:
  //events from CTablesPageDlg->CButtonsPanel
  void OnMapChanged(int i_mapType);
  void OnCloseMapWnd(HWND i_hwnd, int i_mapType);
  void OnOpenMapWnd(HWND i_hwnd, int i_mapType);
  void OnWndActivation(HWND i_hwnd, long cmd);

  //from CButtonsPanel
  bool IsAllowed(void);

  bool m_enabled;
  bool m_tsneb_readonly;
  bool m_lock_enchange;
  bool m_lock_killfocus;

  CStatic m_td_title;
  CButton m_save_button;
  CEdit m_names_edit;
  CString m_names_text;
  CStatic m_midification_flag;
  std::auto_ptr<CTDContextMenuManager> mp_ContextMenuManager;
  std::auto_ptr<class CToolTipCtrlEx> mp_ttc;

  //event handlers (delegates)
  EventWithCode m_OnMapChanged;
  EventWithHWND m_OnCloseMapWnd;
  EventWithHWND m_OnOpenMapWnd;
  EventHandler m_OnSaveButton;
  EventHandler m_OnChangeTablesSetName;
  EventWithCode m_OnLoadTablesFrom;
  EventWithCode m_OnSaveTablesTo;
  EventHandler m_OnImportFromS3F;
  EventHandler m_OnExportToS3F;
  CSize m_bpLRMargin;
};

/////////////////////////////////////////////////////////////////////////////
