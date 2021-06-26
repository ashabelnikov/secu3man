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

/** \file GridModeEditorInjDlg.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include <memory>
#include "common/FastDelegate.h"
#include "common/UnicodeSupport.h"
#include "ui-core/TabController.h"
#include "DynamicValues.h"

class CGMEInjVEDlg;
class CGMEInjAFRDlg;
class CGMEInjITDlg;
class CGMEInjIRegDlg;
class CGMEInjEnrDlg;
class CGMEInjOtherDlg;
class CGMEInjPwm1Dlg;
class CGMEInjPwm2Dlg;

class AFX_EXT_CLASS CGridModeEditorInjDlg : public CDialog
{
  typedef CDialog Super;
  typedef fastdelegate::FastDelegate0<> EventHandler;
  typedef fastdelegate::FastDelegate0<bool> EventResult;
  typedef fastdelegate::FastDelegate1<int> EventWithCode;
  typedef fastdelegate::FastDelegate2<HWND, int> EventWithHWND;

 public:
  CGridModeEditorInjDlg(CWnd* pParent = NULL);   // standard constructor
 ~CGridModeEditorInjDlg();
  static const UINT IDD;

  void BindMaps(float* pVE, float* pAFR, float* pIT, float* pIdlc, float* pIdlr, float* pITRPM, float* pRigid, float* pIACC, float* pIACCW, float* pAftstr, float* pWrmp, float* pAETPS, float* pAERPM, float* pCrnk, float* pDead, float* pEGOCrv, float* pIATCLT, float* pTpsswt, float* pAtsc, float* pGtsc, float* pGpsc, float* pPwm1, float* pPwm2, float* pIACMAT, float* pVE2);
  void BindRPMGrid(float* pGrid);
  void BindCLTGrid(float* pGrid);
  void BindLoadGrid(float* pGrid, float* pGrid2);
  void UpdateView(bool axisLabels = false);
  void UpdateDialogControls(void);

  void SetDynamicValues(const TablDesk::DynVal& dv);

  void SetLoadAxisCfg(float minVal, float maxVal, bool useTable, bool forceUpdate = false);

  void SetITMode(int mode);
  int GetITMode(void) const;

  void SetActiveVEMap(int vemapid);
  int GetActiveVEMap(void) const;

  void SetSplitAngMode(bool mode);

 public:
  void setOnChangeSettings(EventHandler OnCB);
  void setIsAllowed(EventResult IsFunction);
  void setOnMapChanged(EventWithCode OnFunction);
  void setOnOpenMapWnd(EventWithHWND OnFunction); 
  void setOnCloseMapWnd(EventWithHWND OnFunction);

 // Implementation
 protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  virtual BOOL OnInitDialog();
  afx_msg void OnClose();
  afx_msg void OnUpdateControls(CCmdUI* pCmdUI);
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
  DECLARE_MESSAGE_MAP()

 private:
  void OnChangeVE(void);
  void OnChangeVE2(void);
  void OnChangeAFR(void);
  void OnChangeIT(void);
  void OnChangeIReg(int mapId);
  void OnChangeEnr(int mapId);
  void OnChangeOther(int mapId);
  void OnChangePwm1(void);
  void OnChangePwm2(void);

  EventResult   m_IsAllowed;
  EventWithCode m_OnMapChanged;
  EventWithHWND m_OnOpenMapWnd;
  EventWithHWND m_OnCloseMapWnd;

  CTabController m_tab_control;
  std::auto_ptr<CGMEInjVEDlg> m_pVEPageDlg;
  std::auto_ptr<CGMEInjAFRDlg> m_pAFRPageDlg;
  std::auto_ptr<CGMEInjITDlg> m_pITPageDlg;
  std::auto_ptr<CGMEInjIRegDlg> m_pIRegPageDlg;
  std::auto_ptr<CGMEInjEnrDlg> m_pEnrPageDlg;
  std::auto_ptr<CGMEInjOtherDlg> m_pOtherPageDlg;
  std::auto_ptr<CGMEInjPwm1Dlg> m_pPwm1PageDlg;
  std::auto_ptr<CGMEInjPwm2Dlg> m_pPwm2PageDlg;

  float m_ldaxMin;
  float m_ldaxMax;
  bool m_ldaxNeedsUpdate;
  bool m_ldaxUseTable;
  float m_baro_press;
  std::vector<float> m_work_map_load_slots;
  std::vector<float> m_ve2_map_load_slots;
  float* mp_lodGrid;

  friend class CAutoTuneController;

  int m_pwm1TabIdx;
  bool m_initialized;
};
