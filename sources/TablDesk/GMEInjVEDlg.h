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

/** \file GMEInjVEDlg.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include "common/FastDelegate.h"
#include "ui-core/TabDialog.h"
#include "ui-core/MapEditorCtrl.h"

class CToolTipCtrlEx;

class CGMEInjVEDlg : public CTabDialog
{
  typedef CTabDialog Super;
  typedef fastdelegate::FastDelegate0<> EventHandler;
  typedef fastdelegate::FastDelegate0<bool> EventResult;

 public:
  CGMEInjVEDlg(CWnd* pParent = NULL);   // standard constructor
 ~CGMEInjVEDlg();
  static const UINT IDD;

  void BindMaps(float* pVE);
  void BindRPMGrid(float* pGrid);
  void BindLoadGrid(const float* pGrid, bool updateLabels = false);
  void setOnChange(EventHandler OnCB);
  void UpdateView(bool axisLabels = false);

  void SetArguments(int rpm, int air_flow, bool strt_use, float load);

  //Used by CAutoTuneController:
  void BindLamDelMap(float* p_LamDelMap, float* p_rpmGridLD, float* p_loadGridLD);
  void BindCelWgtMap(float* p_CelWgtMap);
  void BindCelBlkMap(bool *p_CelBlkMap);
  void setOnViewActivate(EventHandler OnCB);
  void setOnStrStp(EventHandler OnCB);
  void setOnRstStt(EventHandler OnCB);
  void setIsReady(EventResult OnCB);
  void setOnChangeLamDel(EventHandler OnCB);
  void SetStatusText(const _TSTRING& str);
  void SetStrStpCaption(const _TSTRING& str);
  void UpdateCelWgtMapCell(int l, int r);
  void setOnSmooth(EventHandler OnCB);
  bool GetBlkAllCheck(void);
  bool GetFinishCheck(void);
  bool GetRstAllCheck(void);
  std::pair<int, int> GetVESelection(void);

 public:

 // Implementation
 protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  virtual BOOL OnInitDialog();
  afx_msg void OnUpdateControls(CCmdUI* pCmdUI);
  afx_msg void OnUpdateControlsAutoTune(CCmdUI* pCmdUI);
  afx_msg void OnUpdateControlsAutoTune1(CCmdUI* pCmdUI);
  afx_msg void OnUpdateControlsAutoTune2(CCmdUI* pCmdUI);
  afx_msg void OnLamDelButton();
  afx_msg void OnCelWgtButton();
  afx_msg void OnStrStpButton();
  afx_msg void OnRstSttButton();
  afx_msg void OnCelBlkButton();
  afx_msg void OnSmoothButton();
  DECLARE_MESSAGE_MAP()

  virtual LPCTSTR GetDialogID(void) const;

  void OnChangeLamDel(void);
  void OnSelChangeVE(void);

 private:
  CButton m_lamdel_button;
  CButton m_celwgt_button;
  CButton m_strstp_button;
  CButton m_rststt_button;
  CButton m_celblk_button;
  CButton m_smooth_button;
  CButton m_rstall_check;
  CButton m_blkall_check;
  CButton m_finish_check;

  CStatic m_status_text;
  _TSTRING m_status_cache;

  CMapEditorCtrl m_ve_map;
  CMapEditorCtrl m_lamdel_map;
  CMapEditorCtrl m_celwgt_map;
  CFont m_font;

  float* mp_VEMap;
  float* mp_rpmGrid;
  const float* mp_loadGrid;

  float* mp_LamDelMap;
  float* mp_rpmGridLD;
  float* mp_loadGridLD;
  float* mp_CelWgtMap;
  bool* mp_CelBlkMap;

  EventHandler m_OnViewActivate;
  EventHandler m_OnStrStp;
  EventHandler m_OnRstStt;
  EventHandler m_OnChangeLamDel;
  EventResult  m_IsReady; //if this CB is not initialized all autotune related controls will be disabled
  EventHandler m_OnSmooth;

  std::auto_ptr<CToolTipCtrlEx> mp_ttc;
};
