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

/** \file SeptabsPanel.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include <map>
#include "common/FastDelegate.h"
#include "TablesPanelBase.h"

/////////////////////////////////////////////////////////////////////////////
// CSeptabsPanel dialog

class AFX_EXT_CLASS CSeptabsPanel : public CDialog, public CTablesPanelBase
{
  typedef CDialog Super;

 public:
  CSeptabsPanel(bool disable_vscroll = false, bool enable_fwconsts = false, bool enable_ceerr = false, bool enable_grids = false);
  virtual ~CSeptabsPanel();
  virtual BOOL Create(CWnd* pParentWnd = NULL);

  //note: use wnd_insert_after parameter to change tab order!
  void SetPosition(int x_pos, int y_pos, CWnd* wnd_insert_after = NULL);
  void SetPosition(const CRect& rc, CWnd* wnd_insert_after = NULL);

  void OnShow(bool show);

  //Access content of maps
  float* GetMap(int mapId, bool i_original);

  //returns NULL if corresponding window wasn't opened
  virtual HWND GetMapWindow(int wndType);

  virtual void UpdateOpenedCharts(void);

  void EnableDwellControl(bool enable);
  void EnableCTSCurve(bool enable);
  void EnableFuelInjection(bool i_enable);
  void EnableGasdose(bool enable);
  void EnableTmp2Curve(bool enable);
  void EnableGrtsCurve(bool enable);
  void EnableGrHeatDutyMap(bool enable);
  void EnablePwmIacUCoefMap(bool enable);
  void EnableAftstrStrkMap(bool enable);
  void EnableFtlsCurve(bool enable);
  void EnableEgtsCurve(bool enable);
  void EnableOpsCurve(bool enable);
  void EnableManInjPwc(bool enable);
  void EnableLambdaZones(bool enable);
  void EnableFtsCurve(bool enable);
  void EnableXtauMaps(bool enable);

  void CloseCharts(void);
  void ShowOpenedCharts(bool i_show);
  virtual void MakeChartsChildren(bool children);

  void EnableEmbedMapWnd(bool embed, const CRect& rc);

 public: //set event handlers
  void setOnRPMGridEditButton(EventHandler OnFunction);
  void setOnCESettingsButton(EventHandler OnFunction);
  void setOnFwConstsButton(EventHandler OnFunction);

// Implementation
 protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  virtual BOOL OnInitDialog();
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnDestroy();
  afx_msg void OnTimer(UINT nIDEvent);
  afx_msg void OnViewAttenuatorMap();
  afx_msg void OnViewDwellCntrlMap();
  afx_msg void OnViewCTSCurveMap();
  afx_msg void OnDwellCalcButton();
  afx_msg void OnViewATSCurveMap();
  afx_msg void OnViewATSAACMap();
  afx_msg void OnRPMGridButton();
  afx_msg void OnFwConstsButton();
  afx_msg void OnCESettingsButton();
  afx_msg void OnViewGasdosePosMap();
  afx_msg void OnViewBarocorrMap();
  afx_msg void OnViewManIgntimMap();
  afx_msg void OnViewTmp2CurveMap();
  afx_msg void OnViewCrkTempMap();
  afx_msg void OnViewEHPauseMap();
  afx_msg void OnViewCrankingThrdMap();
  afx_msg void OnViewCrankingTimeMap();
  afx_msg void OnViewSmapabanThrdMap();
  afx_msg void OnViewKnockZoneMap();
  afx_msg void OnViewLambdaZoneMap();
  afx_msg void OnViewGrtsCurveMap();
  afx_msg void OnViewGrHeatDutyMap();
  afx_msg void OnViewPwmIacUCoefMap();
  afx_msg void OnViewAftstrStrk0Map();
  afx_msg void OnViewAftstrStrk1Map();
  afx_msg void OnViewGrValDelMap();
  afx_msg void OnViewEgtsCurveMap();
  afx_msg void OnViewFtlsCurveMap();
  afx_msg void OnViewOpsCurveMap();
  afx_msg void OnViewManInjPwcMap();
  afx_msg void OnViewMAFCurveMap();
  afx_msg void OnViewFtlsCorMap();
  afx_msg void OnViewFtsCurveMap();
  afx_msg void OnViewFuelDensCorrMap();
  afx_msg void OnViewXtauXfAccMap();
  afx_msg void OnViewXtauXfDecMap();
  afx_msg void OnViewXtauTfAccMap();
  afx_msg void OnViewXtauTfDecMap();
  afx_msg void OnViewInjNonLinPMap();
  afx_msg void OnViewInjNonLinGMap();
  afx_msg void OnUpdateViewAttenuatorMap(CCmdUI* pCmdUI);
  afx_msg void OnUpdateViewDwellCntrlMap(CCmdUI* pCmdUI);
  afx_msg void OnUpdateViewCTSCurveMap(CCmdUI* pCmdUI);
  afx_msg void OnUpdateViewATSCurveMap(CCmdUI* pCmdUI);
  afx_msg void OnUpdateViewATSAACMap(CCmdUI* pCmdUI);
  afx_msg void OnUpdateViewGasdosePosMap(CCmdUI* pCmdUI);
  afx_msg void OnUpdateCESettingsButton(CCmdUI* pCmdUI);
  afx_msg void OnUpdateViewBarocorrMap(CCmdUI* pCmdUI);
  afx_msg void OnUpdateViewManIgntimMap(CCmdUI* pCmdUI);
  afx_msg void OnUpdateViewTmp2CurveMap(CCmdUI* pCmdUI);
  afx_msg void OnUpdateViewCrkTempMap(CCmdUI* pCmdUI);
  afx_msg void OnUpdateViewEHPauseMap(CCmdUI* pCmdUI);
  afx_msg void OnUpdateViewCrankingThrdMap(CCmdUI* pCmdUI);
  afx_msg void OnUpdateViewCrankingTimeMap(CCmdUI* pCmdUI);
  afx_msg void OnUpdateViewSmapabanThrdMap(CCmdUI* pCmdUI);
  afx_msg void OnUpdateViewKnockZoneMap(CCmdUI* pCmdUI);
  afx_msg void OnUpdateViewLambdaZoneMap(CCmdUI* pCmdUI);
  afx_msg void OnUpdateViewGrtsCurveMap(CCmdUI* pCmdUI);
  afx_msg void OnUpdateViewGrHeatDutyMap(CCmdUI* pCmdUI);
  afx_msg void OnUpdateViewPwmIacUCoefMap(CCmdUI* pCmdUI);
  afx_msg void OnUpdateViewAftstrStrk0Map(CCmdUI* pCmdUI);
  afx_msg void OnUpdateViewAftstrStrk1Map(CCmdUI* pCmdUI);
  afx_msg void OnUpdateViewGrValDelMap(CCmdUI* pCmdUI);
  afx_msg void OnUpdateViewFtlsCurveMap(CCmdUI* pCmdUI);
  afx_msg void OnUpdateViewFtlsCorMap(CCmdUI* pCmdUI);
  afx_msg void OnUpdateViewEgtsCurveMap(CCmdUI* pCmdUI);
  afx_msg void OnUpdateViewOpsCurveMap(CCmdUI* pCmdUI);
  afx_msg void OnUpdateViewManInjPwcMap(CCmdUI* pCmdUI);
  afx_msg void OnUpdateViewMAFCurveMap(CCmdUI* pCmdUI);
  afx_msg void OnUpdateViewFtsCurveMap(CCmdUI* pCmdUI);
  afx_msg void OnUpdateViewFuelDensCorrMap(CCmdUI* pCmdUI);
  afx_msg void OnUpdateViewXtauXfAccMap(CCmdUI* pCmdUI);
  afx_msg void OnUpdateViewXtauXfDecMap(CCmdUI* pCmdUI);
  afx_msg void OnUpdateViewXtauTfAccMap(CCmdUI* pCmdUI);
  afx_msg void OnUpdateViewXtauTfDecMap(CCmdUI* pCmdUI);
  afx_msg void OnUpdateViewInjNonLinPMap(CCmdUI* pCmdUI);
  afx_msg void OnUpdateViewInjNonLinGMap(CCmdUI* pCmdUI);
  afx_msg void OnUpdateControls(CCmdUI* pCmdUI);
  afx_msg void OnUpdateControlsFWC(CCmdUI* pCmdUI);
  afx_msg void OnUpdateControlsGRD(CCmdUI* pCmdUI);
  afx_msg void OnChangeFunsetList(NMHDR* pNMHDR, LRESULT* pResult);
  afx_msg void OnEndLabelEditFunsetList(NMHDR* pNMHDR, LRESULT* pResult);
  DECLARE_MESSAGE_MAP()

  void _EnableCharts(bool enable);

 private:
  //see SeptabsPanelEvents.cpp for implementation
  static void __cdecl OnChangeSettingsCME(void* i_param);

  static void __cdecl OnChangeAttenuatorTable(void* i_param);
  static void __cdecl OnCloseAttenuatorTable(void* i_param);
  static void __cdecl OnWndActivationAttenuatorTable(void* i_param, long cmd);
  static void __cdecl OnGetAttenuatorYAxisLabel(LPTSTR io_label_string, int index, void* i_param);

  static void __cdecl OnChangeDwellCntrlTable(void* i_param);
  static void __cdecl OnCloseDwellCntrlTable(void* i_param);
  static void __cdecl OnWndActivationDwellCntrlTable(void* i_param, long cmd);

  static void __cdecl OnChangeCTSCurveTable(void* i_param);
  static void __cdecl OnCloseCTSCurveTable(void* i_param);
  static void __cdecl OnChangeCTSXAxisEdit(void* i_param, int type, float value);
  static void __cdecl OnWndActivationCTSCurveTable(void* i_param, long cmd);

  static void __cdecl OnChangeATSCurveTable(void* i_param);
  static void __cdecl OnCloseATSCurveTable(void* i_param);
  static void __cdecl OnChangeATSXAxisEdit(void* i_param, int type, float value);
  static void __cdecl OnWndActivationATSCurveTable(void* i_param, long cmd);

  static void __cdecl OnChangeATSAACTable(void* i_param);
  static void __cdecl OnCloseATSAACTable(void* i_param);
  static void __cdecl OnWndActivationATSAACTable(void* i_param, long cmd);

  static void __cdecl OnChangeGasdoseTable(void* i_param);
  static void __cdecl OnCloseGasdoseTable(void* i_param);
  static void __cdecl OnWndActivationGasdoseTable(void* i_param, long cmd);

  static void __cdecl OnChangeBarocorrTable(void* i_param);
  static void __cdecl OnCloseBarocorrTable(void* i_param);
  static void __cdecl OnChangeBarocorrXAxisEdit(void* i_param, int i_type, float i_value);
  static void __cdecl OnWndActivationBarocorrTable(void* i_param, long cmd);

  static void __cdecl OnChangeManIgntimTable(void* i_param);
  static void __cdecl OnCloseManIgntimTable(void* i_param);
  static void __cdecl OnWndActivationManIgntimTable(void* i_param, long cmd);

  static void __cdecl OnChangeTmp2CurveTable(void* i_param);
  static void __cdecl OnCloseTmp2CurveTable(void* i_param);
  static void __cdecl OnChangeTmp2CurveXAxisEdit(void* i_param, int i_type, float i_value);
  static void __cdecl OnWndActivationTmp2CurveTable(void* i_param, long cmd);

  static void __cdecl OnChangeCrkTempTable(void* i_param);
  static void __cdecl OnCloseCrkTempTable(void* i_param);
  static void __cdecl OnWndActivationCrkTempTable(void* i_param, long cmd);

  static void __cdecl OnChangeEHPauseTable(void* i_param);
  static void __cdecl OnCloseEHPauseTable(void* i_param);
  static void __cdecl OnWndActivationEHPauseTable(void* i_param, long cmd);

  static void __cdecl OnChangeCrankingThrdMap(void* i_param);
  static void __cdecl OnCloseCrankingThrdMap(void* i_param);
  static void __cdecl OnWndActivationCrankingThrdMap(void* i_param, long cmd);

  static void __cdecl OnChangeCrankingTimeMap(void* i_param);
  static void __cdecl OnCloseCrankingTimeMap(void* i_param);
  static void __cdecl OnWndActivationCrankingTimeMap(void* i_param, long cmd);

  static void __cdecl OnChangeSmapabanThrdMap(void* i_param);
  static void __cdecl OnCloseSmapabanThrdMap(void* i_param);
  static void __cdecl OnWndActivationSmapabanThrdMap(void* i_param, long cmd);

  static void __cdecl OnChangeKnockZoneMap(void* i_param);
  static void __cdecl OnCloseKnockZoneMap(void* i_param);
  static void __cdecl OnWndActivationKnockZoneMap(void* i_param, long cmd);

  static void __cdecl OnChangeLambdaZoneMap(void* i_param);
  static void __cdecl OnCloseLambdaZoneMap(void* i_param);
  static void __cdecl OnWndActivationLambdaZoneMap(void* i_param, long cmd);

  static void __cdecl OnChangeGrtsCurveTable(void* i_param);
  static void __cdecl OnCloseGrtsCurveTable(void* i_param);
  static void __cdecl OnChangeGrtsCurveXAxisEdit(void* i_param, int i_type, float i_value);
  static void __cdecl OnWndActivationGrtsCurveTable(void* i_param, long cmd);

  static void __cdecl OnChangeGrHeatDutyTable(void* i_param);
  static void __cdecl OnCloseGrHeatDutyTable(void* i_param);
  static void __cdecl OnWndActivationGrHeatDutyTable(void* i_param, long cmd);

  static void __cdecl OnChangePwmIacUCoefMap(void* i_param);
  static void __cdecl OnClosePwmIacUCoefMap(void* i_param);
  static void __cdecl OnWndActivationPwmIacUCoefMap(void* i_param, long cmd);

  static void __cdecl OnChangeAftstrStrk0Map(void* i_param);
  static void __cdecl OnCloseAftstrStrk0Map(void* i_param);
  static void __cdecl OnWndActivationAftstrStrk0Map(void* i_param, long cmd);

  static void __cdecl OnChangeAftstrStrk1Map(void* i_param);
  static void __cdecl OnCloseAftstrStrk1Map(void* i_param);
  static void __cdecl OnWndActivationAftstrStrk1Map(void* i_param, long cmd);

  static void __cdecl OnChangeGrValDelMap(void* i_param);
  static void __cdecl OnCloseGrValDelMap(void* i_param);
  static void __cdecl OnWndActivationGrValDelMap(void* i_param, long cmd);

  static void __cdecl OnChangeFtlsCurveTable(void* i_param);
  static void __cdecl OnCloseFtlsCurveTable(void* i_param);
  static void __cdecl OnChangeFtlsCurveXAxisEdit(void* i_param, int i_type, float i_value);
  static void __cdecl OnWndActivationFtlsCurveTable(void* i_param, long cmd);

  static void __cdecl OnChangeEgtsCurveTable(void* i_param);
  static void __cdecl OnCloseEgtsCurveTable(void* i_param);
  static void __cdecl OnChangeEgtsCurveXAxisEdit(void* i_param, int i_type, float i_value);
  static void __cdecl OnWndActivationEgtsCurveTable(void* i_param, long cmd);

  static void __cdecl OnChangeOpsCurveTable(void* i_param);
  static void __cdecl OnCloseOpsCurveTable(void* i_param);
  static void __cdecl OnChangeOpsCurveXAxisEdit(void* i_param, int i_type, float i_value);
  static void __cdecl OnWndActivationOpsCurveTable(void* i_param, long cmd);

  static void __cdecl OnChangeManInjPwcTable(void* i_param);
  static void __cdecl OnCloseManInjPwcTable(void* i_param);
  static void __cdecl OnWndActivationManInjPwcTable(void* i_param, long cmd);

  static void __cdecl OnChangeMAFCurveTable(void* i_param);
  static void __cdecl OnCloseMAFCurveTable(void* i_param);
  static void __cdecl OnChangeMAFCurveXAxisEdit(void* i_param, int i_type, float i_value);
  static void __cdecl OnWndActivationMAFCurveTable(void* i_param, long cmd);

  static void __cdecl OnChangeFtlsCorTable(void* i_param);
  static void __cdecl OnCloseFtlsCorTable(void* i_param);
  static void __cdecl OnWndActivationFtlsCorTable(void* i_param, long cmd);

  static void __cdecl OnChangeFtsCurveTable(void* i_param);
  static void __cdecl OnCloseFtsCurveTable(void* i_param);
  static void __cdecl OnChangeFtsCurveXAxisEdit(void* i_param, int i_type, float i_value);
  static void __cdecl OnWndActivationFtsCurveTable(void* i_param, long cmd);

  static void __cdecl OnChangeFuelDensCorrTable(void* i_param);
  static void __cdecl OnCloseFuelDensCorrTable(void* i_param);
  static void __cdecl OnWndActivationFuelDensCorrTable(void* i_param, long cmd);

  static void __cdecl OnChangeXtauXfAccMap(void* i_param);
  static void __cdecl OnCloseXtauXfAccMap(void* i_param);
  static void __cdecl OnWndActivationXtauXfAccMap(void* i_param, long cmd);

  static void __cdecl OnChangeXtauXfDecMap(void* i_param);
  static void __cdecl OnCloseXtauXfDecMap(void* i_param);
  static void __cdecl OnWndActivationXtauXfDecMap(void* i_param, long cmd);

  static void __cdecl OnChangeXtauTfAccMap(void* i_param);
  static void __cdecl OnCloseXtauTfAccMap(void* i_param);
  static void __cdecl OnWndActivationXtauTfAccMap(void* i_param, long cmd);

  static void __cdecl OnChangeXtauTfDecMap(void* i_param);
  static void __cdecl OnCloseXtauTfDecMap(void* i_param);
  static void __cdecl OnWndActivationXtauTfDecMap(void* i_param, long cmd);

  static void __cdecl OnChangeInjNonLinPTable(void* i_param);
  static void __cdecl OnCloseInjNonLinPTable(void* i_param);
  static void __cdecl OnWndActivationInjNonLinPTable(void* i_param, long cmd);

  static void __cdecl OnChangeInjNonLinGTable(void* i_param);
  static void __cdecl OnCloseInjNonLinGTable(void* i_param);
  static void __cdecl OnWndActivationInjNonLinGTable(void* i_param, long cmd);

 private:
  EventHandler m_OnRPMGridEditButton;
  EventHandler m_OnCESettingsButton;
  EventHandler m_OnFwConstsButton;

 private:
  CBitmapButton m_calc_dwell_btn;
  CBitmapButton m_rpm_grid_btn;
  CBitmapButton m_fw_consts_btn;
  CBitmapButton m_edit_cesettings_btn;

  bool m_initialized;
  bool m_disable_vscroll;
  bool m_enable_fwconsts;
  bool m_enable_ceerr;
  bool m_enable_grids;

 private:
  bool m_dwellcntrl_enabled;
  bool m_cts_curve_enabled;
  bool m_tmp2_curve_enabled;
  bool m_grts_curve_enabled;
  bool m_grheat_duty_enabled;
  bool m_pwmiac_ucoef_enabled;
  bool m_aftstr_strk_enabled;
  bool m_ftls_curve_enabled;
  bool m_egts_curve_enabled;
  bool m_ops_curve_enabled;
  bool m_maninjpwc_enabled;
  bool m_lambdazone_enabled;
  bool m_fts_curve_enabled;
  bool m_xtau_maps_enabled;

  float m_attenuator_table_slots[128];

/*  float m_mafcurve_slots[64];*/
};
