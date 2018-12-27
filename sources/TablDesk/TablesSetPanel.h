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
#include "ButtonsPanel.h"
#include "common/FastDelegate.h"
#include "common/unicodesupport.h"
#include "ui-core/ListCtrlEx.h"

class CToolTipCtrlEx;

/////////////////////////////////////////////////////////////////////////////
// CTablesSetPanel dialog

class AFX_EXT_CLASS CTablesSetPanel : public CButtonsPanel
{
  typedef CButtonsPanel Super;
  typedef fastdelegate::FastDelegate2<int,CString> EventWithCodeAndString;
  typedef fastdelegate::FastDelegate2<int,float> EventWithCodeAndFloat;
  typedef fastdelegate::FastDelegate0<> EventHandler;

 public:
  CTablesSetPanel(CWnd* pParent = NULL);   // standard constructor
  static const UINT IDD;

  void SetFunSetListBox(std::vector<_TSTRING> i_list_of_names);
  void SetFunSetListBoxSelection(int i_selected_index);

  //Access content of maps
  float* GetAttenuatorMap(bool i_original);
  float* GetDwellCntrlMap(bool i_original);
  float* GetCTSCurveMap(bool i_original);
  float* GetATSCurveMap(bool i_original);
  float* GetATSAACMap(bool i_original);
  void SetCTSXAxisEdits(float i_begin, float i_end);
  void SetATSXAxisEdits(float i_begin, float i_end);
  float* GetGasdosePosMap(bool i_original);
  float* GetBarocorrMap(bool i_original);
  float* GetManIgntimMap(bool i_original);
  float* GetTmp2CurveMap(bool i_original);
  float* GetCrkTempMap(bool i_original);
  float* GetEHPauseMap(bool i_original);

  //returns NULL if corresponding window wasn't opened
  virtual HWND GetMapWindow(int wndType);

  virtual void UpdateOpenedCharts(void);

  void EnableDwellControl(bool enable);
  void EnableCTSCurve(bool enable);
  virtual void EnableChokeOp(bool enable);
  virtual void EnableFuelInjection(bool i_enable);
  virtual void EnableGasdose(bool enable);
  void EnableTmp2Curve(bool enable);

  virtual void CloseCharts(void);

  virtual void ShowOpenedCharts(bool i_show);

 public: //set event handlers
  void setOnFunSetSelectionChanged(EventWithCode OnFunction);
  void setOnFunSetNamechanged(EventWithCodeAndString OnFunction);
  void setOnCTSXAxisEditChanged(EventWithCodeAndFloat OnFunction);
  void setOnATSXAxisEditChanged(EventWithCodeAndFloat OnFunction);
  void setOnRPMGridEditButton(EventHandler OnFunction);
  void setOnCESettingsButton(EventHandler OnFunction);

// Implementation
 protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  virtual BOOL OnInitDialog();
  afx_msg void OnDestroy();
  afx_msg void OnViewAttenuatorMap();
  afx_msg void OnViewDwellCntrlMap();
  afx_msg void OnViewCTSCurveMap();
  afx_msg void OnDwellCalcButton();
  afx_msg void OnViewATSCurveMap();
  afx_msg void OnViewATSAACMap();
  afx_msg void OnRPMGridButton();
  afx_msg void OnCESettingsButton();
  afx_msg void OnViewGasdosePosMap();
  afx_msg void OnViewBarocorrMap();
  afx_msg void OnViewManIgntimMap();
  afx_msg void OnViewTmp2CurveMap();
  afx_msg void OnViewCrkTempMap();
  afx_msg void OnViewEHPauseMap();
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
  afx_msg void OnUpdateControls(CCmdUI* pCmdUI);
  afx_msg void OnChangeFunsetList(NMHDR* pNMHDR, LRESULT* pResult);
  afx_msg void OnEndLabelEditFunsetList(NMHDR* pNMHDR, LRESULT* pResult);
  DECLARE_MESSAGE_MAP()

  virtual bool IsAllowed(void);

 private:
  CListCtrlEx m_funset_listbox;
  CButton   m_view_attenuator_map_btn;
  CButton   m_view_dwellcntrl_map_btn;
  CButton   m_view_cts_curve_map_btn;
  CButton   m_view_ats_curve_map_btn;
  CButton   m_view_ats_aac_map_btn;
  CButton   m_view_gasdose_map_btn;
  CButton   m_edit_cesettings_btn;
  CButton   m_view_barocorr_map_btn;
  CButton   m_view_manigntim_map_btn;
  CButton   m_view_tmp2_curve_map_btn;
  CButton   m_view_crktemp_map_btn;
  CButton   m_view_eh_pause_map_btn;
  CBitmapButton m_calc_dwell_btn;
  CBitmapButton m_rpm_grid_btn;

 private:
  EventWithCode m_OnFunSetSelectionChanged;
  EventWithCodeAndString m_OnFunSetNamechanged;
  EventWithCodeAndFloat m_OnCTSXAxisEditChanged;
  EventWithCodeAndFloat m_OnATSXAxisEditChanged;
  EventHandler m_OnRPMGridEditButton;
  EventHandler m_OnCESettingsButton;

  static void __cdecl OnChangeAttenuatorTable(void* i_param);
  static void __cdecl OnCloseAttenuatorTable(void* i_param);
  static void __cdecl OnWndActivationAttenuatorTable(void* i_param, long cmd);

  static void __cdecl OnChangeDwellCntrlTable(void* i_param);
  static void __cdecl OnCloseDwellCntrlTable(void* i_param);
  static void __cdecl OnWndActivationDwellCntrlTable(void* i_param, long cmd);

  static void __cdecl OnChangeCTSCurveTable(void* i_param);
  static void __cdecl OnCloseCTSCurveTable(void* i_param);
  static void __cdecl OnWndActivationCTSCurveTable(void* i_param, long cmd);

  static void __cdecl OnGetYAxisLabel(LPTSTR io_label_string, int index, void* i_param);
  static void __cdecl OnGetXAxisLabel(LPTSTR io_label_string, int index, void* i_param);
  static void __cdecl OnChangeCTSXAxisEdit(void* i_param, int type, float value);
  static void __cdecl OnChangeATSXAxisEdit(void* i_param, int type, float value);

  static void __cdecl OnChangeATSCurveTable(void* i_param);
  static void __cdecl OnCloseATSCurveTable(void* i_param);
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

  bool m_dwellcntrl_enabled;
  bool m_cts_curve_enabled;
  bool m_tmp2_curve_enabled;

  int m_attenuator_map_chart_state;
  int m_dwellcntrl_map_chart_state;
  int m_cts_curve_map_chart_state;
  int m_ats_curve_map_chart_state;
  int m_ats_aac_map_chart_state;
  int m_gasdose_map_chart_state;
  int m_barocorr_map_chart_state;
  int m_manigntim_map_chart_state;
  int m_tmp2_curve_map_chart_state;
  int m_crktemp_map_chart_state;
  int m_eh_pause_map_chart_state;

  HWND m_attenuator_map_wnd_handle;
  HWND m_dwellcntrl_map_wnd_handle;
  HWND m_cts_curve_map_wnd_handle;
  HWND m_ats_curve_map_wnd_handle;
  HWND m_ats_aac_map_wnd_handle;
  HWND m_gasdose_map_wnd_handle;
  HWND m_barocorr_map_wnd_handle;
  HWND m_manigntim_map_wnd_handle;
  HWND m_tmp2_curve_map_wnd_handle;
  HWND m_crktemp_map_wnd_handle;
  HWND m_eh_pause_map_wnd_handle;

  float m_attenuator_table_slots[128];

  float m_attenuator_map_active[128];
  float m_attenuator_map_original[128];

  float m_dwellcntrl_map_active[32];
  float m_dwellcntrl_map_original[32];

  float m_cts_curve_map_active[16];
  float m_cts_curve_map_original[16];

  float m_cts_curve_x_axis_limits[2];

  float m_ats_curve_map_active[16];
  float m_ats_curve_map_original[16];

  float m_ats_curve_x_axis_limits[2];

  float m_ats_aac_map_active[16];
  float m_ats_aac_map_original[16];

  float m_gasdose_map_active[16][16];
  float m_gasdose_map_original[16][16];

  float m_barocorr_map_active[9+2];
  float m_barocorr_map_original[9+2];

  float m_manigntim_map_active[16];
  float m_manigntim_map_original[16];

  float m_tmp2_curve_map_active[16+2];
  float m_tmp2_curve_map_original[16+2];

  float m_crktemp_map_active[16];
  float m_crktemp_map_original[16];

  float m_eh_pause_map_active[32];
  float m_eh_pause_map_original[32];

  std::auto_ptr<CToolTipCtrlEx> mp_ttc;
};
