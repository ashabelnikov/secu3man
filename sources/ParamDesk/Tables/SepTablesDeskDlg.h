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

/** \file SepTablesDeskDlg.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include <map>
#include <vector>

#include "common/ParamPageEvents.h"
#include "common/unicodesupport.h"
#include "ui-core/UpdatableDialog.h"

class CSeptabsPanel;

/////////////////////////////////////////////////////////////////////////////
// CTablesDeskDlg dialog

class AFX_EXT_CLASS CSepTablesDeskDlg : public CModelessUpdatableDialog, public ParamPageEvents
{
  typedef CModelessUpdatableDialog Super;
  typedef fastdelegate::FastDelegate0<> EventHandler;
  typedef fastdelegate::FastDelegate1<int> EventWithCode;
  typedef fastdelegate::FastDelegate2<HWND, int> EventWithHWND;

 public:
  CSepTablesDeskDlg(CWnd* pParent = NULL);
  virtual ~CSepTablesDeskDlg();
  static UINT IDD;

  //use wnd_insert_after parameter to set tab order!
  void SetPosition(int x_pos, int y_pos, CWnd* wnd_insert_after = NULL);

  virtual bool IsEnabled(void);
  virtual void Enable(bool enable);
  virtual void Show(bool show);
  virtual void ShowOpenedCharts(bool i_show);  
  virtual void UpdateOpenedCharts(void);
  virtual void SetModificationFlag(bool value);
  virtual void MakeChartsChildren(bool children);
  virtual void EnableToggleMapWnd(bool toggle);

  virtual void SetRPMGrid(const float* values);
  virtual void SetCLTGrid(const float* values);
  virtual void SetLoadGrid(const float* values);
  virtual void CloseAllCharts(void);

  //events
  virtual void setOnMapChanged(EventWithCode OnFunction);
  virtual void setOnCloseMapWnd(EventWithHWND OnFunction);
  virtual void setOnOpenMapWnd(EventWithHWND OnFunction);
  virtual void setOnSaveButton(EventHandler OnFunction);

  virtual float* GetMap(int i_mapType, bool i_original);

  void OnShow(bool show);

  void EnableSaveButton(bool enable);

  //CSeptabsPanel
  std::auto_ptr<CSeptabsPanel> mp_SeptabsPanel;

 // Implementation
 protected:
  virtual void DoDataExchange(CDataExchange* pDX);// DDX/DDV support
  virtual BOOL OnInitDialog();
  afx_msg void OnDestroy();
  afx_msg void OnUpdateControls(CCmdUI* pCmdUI);
  afx_msg void OnUpdateControlsSaveButton(CCmdUI* pCmdUI);
  afx_msg void OnSaveButton();
/*afx_msg void OnSysCommand(UINT, LONG);*/

  afx_msg void OnSize(UINT nType, int cx, int cy);
  DECLARE_MESSAGE_MAP()

 private:
  //events from CSeptabsPanel
  void OnMapChanged(int i_mapType);
  void OnCloseMapWnd(HWND i_hwnd, int i_mapType);
  void OnOpenMapWnd(HWND i_hwnd, int i_mapType);
  void OnWndActivation(HWND i_hwnd, long cmd);

  //from CSeptabsPanel
  bool IsAllowed(void);

  bool m_enabled;
  bool m_enable_save_button;

  CButton m_save_button;
  CStatic m_midification_flag;
  std::auto_ptr<class CToolTipCtrlEx> mp_ttc;

  //event handlers (delegates)
  EventWithCode m_OnMapChanged;
  EventWithHWND m_OnCloseMapWnd;
  EventWithHWND m_OnOpenMapWnd;
  EventHandler m_OnSaveButton;

  CSize m_bpLRMargin;
};

/////////////////////////////////////////////////////////////////////////////
