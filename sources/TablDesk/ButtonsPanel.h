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

/** \file ButtonsPanel.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include <memory>
#include "common/FastDelegate.h"
#include "GridModeEditorIgnDlg.h"
#include "GridModeEditorInjDlg.h"

class CWndScroller;
class CAutoTuneController;

/////////////////////////////////////////////////////////////////////////////
// CButtonsPanel dialog

class AFX_EXT_CLASS CButtonsPanel : public CDialog
{
  typedef CDialog Super;

 public:
  typedef fastdelegate::FastDelegate0<> EventHandler;
  typedef fastdelegate::FastDelegate1<int> EventWithCode;
  typedef fastdelegate::FastDelegate0<bool> EventResult;
  typedef fastdelegate::FastDelegate2<HWND, int> EventWithHWND;
  typedef fastdelegate::FastDelegate2<HWND, long> EventWithHWNDLong;

  CButtonsPanel(UINT dialog_id, CWnd* pParent = NULL, bool enableAutoTune = false);   // standard constructor
  /*static*/ const UINT IDD;
  ~CButtonsPanel();

  //note: use wnd_insert_after parameter to fix tab order!
  void SetPosition(int x_pos, int y_pos, CWnd* wnd_insert_after = NULL);

  //returns NULL if corresponding window wasn't opened
  virtual HWND GetMapWindow(int wndType);

  //Read/Write access
  float* GetStartMap(bool i_original);
  float* GetIdleMap(bool i_original);
  float* GetWorkMap(bool i_original);
  float* GetTempMap(bool i_original);
  float* GetVEMap(bool i_original);
  float* GetAFRMap(bool i_original);
  float* GetCrnkMap(bool i_original);
  float* GetWrmpMap(bool i_original);
  float* GetDeadMap(bool i_original);
  float* GetIdlrMap(bool i_original);
  float* GetIdlcMap(bool i_original);
  float* GetAETPSMap(bool i_original);
  float* GetAERPMMap(bool i_original);
  float* GetAftstrMap(bool i_original);
  float* GetITMap(bool i_original);
  float* GetITRPMMap(bool i_original);
  float* GetRigidMap(bool i_original);
  float* GetEGOCurveMap(bool i_original);
  float* GetIACCMap(bool i_original);
  float* GetIACCWMap(bool i_original);
  float* GetIATCLTMap(bool i_original);
  float* GetTpsswtMap(bool i_original);
  float* GetGtscMap(bool i_original);
  float* GetGpscMap(bool i_original);
  float* GetAtscMap(bool i_original);

  float* GetRPMGrid(void);

  //updates all opened charts
  virtual void UpdateOpenedCharts(void);

  //used for grid mode editing window
  void EnableAdvanceAngleIndication(bool i_enable);

  void SetDynamicValues(const TablDesk::DynVal& dv);

  void SetLoadAxisCfg(float minVal, float maxVal, int loadSrc);

  virtual void EnableFuelInjection(bool i_enable);

  virtual void EnableGasdose(bool i_enable);

  virtual void EnableCarbAfr(bool i_enable);

  virtual void EnableChokeOp(bool enable);

  void EnableGasCorr(bool i_enable);

  float* GetLamDelMap(int id); // 0 - map, 1 - rpm bins, 2 - load bins  
  void SetAFRError(float afrError);
  void SetStatSize(int statSize);
  void SetAutoBlockThrd(int thrd);
  bool* GetBlockedCells(void);
  void SetGrowingMode(bool growing);
  void SetMinAFR(float afr);
  void SetMaxAFR(float afr);
  void SetMinDistThrd(int thrd);
  void SetMinTPS(float tps);
  void SetMaxTPS(float tps);
  void SetCLTThrd(float clt);

  void SetMapEditorSettings(int gradSat, int gradBrt, bool bold);

  void SetITEdMode(int mode);
  int GetITEdMode(void) const;

  virtual void MakeChartsChildren(bool children);

  void EnableToggleMapWnd(bool toggle);

  virtual void CloseCharts(void);

  virtual void ShowOpenedCharts(bool i_show);

 public: //установка обработчиков событий
  void setOnChangeSettings(EventHandler OnCB);
  void setOnMapChanged(EventWithCode OnFunction);
  void setOnCloseMapWnd(EventWithHWND OnFunction);
  void setOnOpenMapWnd(EventWithHWND OnFunction); 
  void setIsAllowed(EventResult IsFunction);
  void setOnWndActivation(EventWithHWNDLong OnFunction); 

// Implementation
 protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  virtual BOOL OnInitDialog();
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnViewStartMap();
  afx_msg void OnViewIdleMap();
  afx_msg void OnViewWorkMap();
  afx_msg void OnViewTempMap();
  afx_msg void OnViewVEMap();
  afx_msg void OnViewAFRMap();
  afx_msg void OnViewCrnkMap();
  afx_msg void OnViewWrmpMap();
  afx_msg void OnViewDeadMap();
  afx_msg void OnViewIdlrMap();
  afx_msg void OnViewIdlcMap();
  afx_msg void OnViewAETPSMap();
  afx_msg void OnViewAERPMMap();
  afx_msg void OnViewAftstrMap();
  afx_msg void OnViewITMap();
  afx_msg void OnViewITRPMMap();
  afx_msg void OnViewRigidMap();
  afx_msg void OnViewEGOCrvMap();
  afx_msg void OnViewIACCMap();
  afx_msg void OnViewIACCWMap();
  afx_msg void OnViewIATCLTMap();
  afx_msg void OnViewTpsswtMap();
  afx_msg void OnViewGtscMap();
  afx_msg void OnViewGpscMap();
  afx_msg void OnViewAtscMap();

  afx_msg void OnGridModeEditingIgn();
  afx_msg void OnGridModeEditingInj();
  afx_msg void OnUpdateViewStartMap(CCmdUI* pCmdUI);
  afx_msg void OnUpdateViewIdleMap(CCmdUI* pCmdUI);
  afx_msg void OnUpdateViewWorkMap(CCmdUI* pCmdUI);
  afx_msg void OnUpdateViewTempMap(CCmdUI* pCmdUI);
  afx_msg void OnUpdateViewVEMap(CCmdUI* pCmdUI);
  afx_msg void OnUpdateViewAFRMap(CCmdUI* pCmdUI);
  afx_msg void OnUpdateViewCrnkMap(CCmdUI* pCmdUI);
  afx_msg void OnUpdateViewWrmpMap(CCmdUI* pCmdUI);
  afx_msg void OnUpdateViewDeadMap(CCmdUI* pCmdUI);
  afx_msg void OnUpdateViewIdlrMap(CCmdUI* pCmdUI);
  afx_msg void OnUpdateViewIdlcMap(CCmdUI* pCmdUI);
  afx_msg void OnUpdateViewAETPSMap(CCmdUI* pCmdUI);
  afx_msg void OnUpdateViewAERPMMap(CCmdUI* pCmdUI);
  afx_msg void OnUpdateViewAftstrMap(CCmdUI* pCmdUI);
  afx_msg void OnUpdateViewITMap(CCmdUI* pCmdUI);
  afx_msg void OnUpdateViewITRPMMap(CCmdUI* pCmdUI);
  afx_msg void OnUpdateViewRigidMap(CCmdUI* pCmdUI);
  afx_msg void OnUpdateViewEGOCrvMap(CCmdUI* pCmdUI);
  afx_msg void OnUpdateViewIACCMap(CCmdUI* pCmdUI);
  afx_msg void OnUpdateViewIACCWMap(CCmdUI* pCmdUI);
  afx_msg void OnUpdateViewIATCLTMap(CCmdUI* pCmdUI);
  afx_msg void OnUpdateViewTpsswtMap(CCmdUI* pCmdUI);
  afx_msg void OnUpdateViewGtscMap(CCmdUI* pCmdUI);
  afx_msg void OnUpdateViewGpscMap(CCmdUI* pCmdUI);
  afx_msg void OnUpdateViewAtscMap(CCmdUI* pCmdUI);
  afx_msg void OnUpdateGridModeEditingIgn(CCmdUI* pCmdUI);
  afx_msg void OnUpdateGridModeEditingInj(CCmdUI* pCmdUI);
  afx_msg void OnTimer(UINT nIDEvent);
  afx_msg void OnDestroy();
  DECLARE_MESSAGE_MAP()

  void _EnableCharts(bool enable);

  //may be overloaded to change behaviour
  virtual bool IsAllowed(void);

  EventHandler  m_OnChangeSettings;
  EventWithCode m_OnMapChanged;
  EventWithHWND m_OnCloseMapWnd;
  EventWithHWND m_OnOpenMapWnd;
  EventResult   m_IsAllowed;
  EventWithHWNDLong m_OnWndActivation;

  int m_scrl_view;
 private:
  void _MakeWindowChild(HWND hwnd, bool child);
  void _GetITModeRange(float& y1, float& y2);

  std::auto_ptr<CWndScroller> mp_scr;

  CButton m_view_work_map_btn;
  CButton m_view_temp_map_btn;
  CButton m_view_start_map_btn;
  CButton m_view_idle_map_btn;
  CButton m_view_ve_map_btn;
  CButton m_view_afr_map_btn;
  CButton m_view_crnk_map_btn;
  CButton m_view_wrmp_map_btn;
  CButton m_view_dead_map_btn;
  CButton m_view_idlr_map_btn;
  CButton m_view_idlc_map_btn;
  CButton m_view_aetps_map_btn;
  CButton m_view_aerpm_map_btn;
  CButton m_view_aftstr_map_btn;
  CButton m_view_it_map_btn;
  CButton m_view_itrpm_map_btn;
  CButton m_view_rigid_map_btn;
  CButton m_view_egocrv_map_btn;
  CButton m_view_iacc_map_btn;
  CButton m_view_iaccw_map_btn;
  CButton m_view_iatclt_map_btn;
  CButton m_view_tpsswt_map_btn;
  CButton m_view_gtsc_map_btn;
  CButton m_view_gpsc_map_btn;
  CButton m_view_atsc_map_btn;

  CButton m_grid_mode_editing_ign_check;
  CButton m_grid_mode_editing_inj_check;
  std::auto_ptr<CGridModeEditorIgnDlg> mp_gridModeEditorIgnDlg;
  std::auto_ptr<CGridModeEditorInjDlg> mp_gridModeEditorInjDlg;
  std::auto_ptr<CAutoTuneController> mp_autoTuneCntr;

  void OnChangeSettingsGME(void);

  static void __cdecl OnChangeStartMap(void* i_param);
  static void __cdecl OnCloseStartMap(void* i_param);
  static void __cdecl OnChangeIdleMap(void* i_param);
  static void __cdecl OnCloseIdleMap(void* i_param);
  static void __cdecl OnChangeWorkMap(void* i_param);
  static void __cdecl OnCloseWorkMap(void* i_param);
  static void __cdecl OnChangeTempMap(void* i_param);
  static void __cdecl OnCloseTempMap(void* i_param);
  static float __cdecl OnValueTransformITMap(void* i_param, float source, int direction);
protected:
  static void __cdecl OnGetXAxisLabelRPM(LPTSTR io_label_string, int index, void* i_param);
private:
  static void __cdecl OnWndActivationStartMap(void* i_param, long cmd);
  static void __cdecl OnWndActivationIdleMap(void* i_param, long cmd);
  static void __cdecl OnWndActivationWorkMap(void* i_param, long cmd);
  static void __cdecl OnWndActivationTempMap(void* i_param, long cmd);

  static void __cdecl OnChangeVEMap(void* i_param);
  static void __cdecl OnCloseVEMap(void* i_param);
  static void __cdecl OnChangeAFRMap(void* i_param);
  static void __cdecl OnCloseAFRMap(void* i_param);
  static void __cdecl OnChangeCrnkMap(void* i_param);
  static void __cdecl OnCloseCrnkMap(void* i_param);
  static void __cdecl OnChangeWrmpMap(void* i_param);
  static void __cdecl OnCloseWrmpMap(void* i_param);
  static void __cdecl OnChangeDeadMap(void* i_param);
  static void __cdecl OnCloseDeadMap(void* i_param);
  static void __cdecl OnChangeIdlrMap(void* i_param);
  static void __cdecl OnCloseIdlrMap(void* i_param);
  static void __cdecl OnChangeIdlcMap(void* i_param);
  static void __cdecl OnCloseIdlcMap(void* i_param);
  static void __cdecl OnChangeAETPSMap(void* i_param);
  static void __cdecl OnCloseAETPSMap(void* i_param);
  static void __cdecl OnChangeAERPMMap(void* i_param);
  static void __cdecl OnCloseAERPMMap(void* i_param);
  static void __cdecl OnChangeAftstrMap(void* i_param);
  static void __cdecl OnCloseAftstrMap(void* i_param);
  static void __cdecl OnChangeITMap(void* i_param);
  static void __cdecl OnCloseITMap(void* i_param);
  static void __cdecl OnChangeITRPMMap(void* i_param);
  static void __cdecl OnCloseITRPMMap(void* i_param);
  static void __cdecl OnChangeRigidMap(void* i_param);
  static void __cdecl OnCloseRigidMap(void* i_param);
  static void __cdecl OnChangeEGOCrvMap(void* i_param);
  static void __cdecl OnCloseEGOCrvMap(void* i_param);
  static void __cdecl OnChangeEGOCrvXAxisEdit(void* i_param, int i_type, float i_value);
  static void __cdecl OnChangeIACCMap(void* i_param);
  static void __cdecl OnCloseIACCMap(void* i_param);
  static void __cdecl OnChangeIACCXAxisEdit(void* i_param, int i_type, float i_value);
  static void __cdecl OnChangeIACCWMap(void* i_param);
  static void __cdecl OnCloseIACCWMap(void* i_param);
  static void __cdecl OnChangeIACCWXAxisEdit(void* i_param, int i_type, float i_value);
  static void __cdecl OnChangeIATCLTMap(void* i_param);
  static void __cdecl OnCloseIATCLTMap(void* i_param);
  static void __cdecl OnChangeIATCLTXAxisEdit(void* i_param, int i_type, float i_value);
  static void __cdecl OnChangeTpsswtMap(void* i_param);
  static void __cdecl OnCloseTpsswtMap(void* i_param);
  static void __cdecl OnChangeGtscMap(void* i_param);
  static void __cdecl OnCloseGtscMap(void* i_param);
  static void __cdecl OnChangeGpscMap(void* i_param);
  static void __cdecl OnCloseGpscMap(void* i_param);
  static void __cdecl OnChangeGpscXAxisEdit(void* i_param, int i_type, float i_value);
  static void __cdecl OnChangeAtscMap(void* i_param);
  static void __cdecl OnCloseAtscMap(void* i_param);

  static void __cdecl OnWndActivationVEMap(void* i_param, long cmd);
  static void __cdecl OnWndActivationAFRMap(void* i_param, long cmd);
  static void __cdecl OnWndActivationCrnkMap(void* i_param, long cmd);
  static void __cdecl OnWndActivationWrmpMap(void* i_param, long cmd);
  static void __cdecl OnWndActivationDeadMap(void* i_param, long cmd);
  static void __cdecl OnWndActivationIdlrMap(void* i_param, long cmd);
  static void __cdecl OnWndActivationIdlcMap(void* i_param, long cmd);
  static void __cdecl OnWndActivationAETPSMap(void* i_param, long cmd);
  static void __cdecl OnWndActivationAERPMMap(void* i_param, long cmd);
  static void __cdecl OnWndActivationAftstrMap(void* i_param, long cmd);
  static void __cdecl OnWndActivationITMap(void* i_param, long cmd);
  static void __cdecl OnWndActivationITRPMMap(void* i_param, long cmd);
  static void __cdecl OnWndActivationRigidMap(void* i_param, long cmd);
  static void __cdecl OnWndActivationEGOCrvMap(void* i_param, long cmd);
  static void __cdecl OnWndActivationIACCMap(void* i_param, long cmd);
  static void __cdecl OnWndActivationIACCWMap(void* i_param, long cmd);
  static void __cdecl OnWndActivationIATCLTMap(void* i_param, long cmd);
  static void __cdecl OnWndActivationTpsswtMap(void* i_param, long cmd);
  static void __cdecl OnWndActivationGtscMap(void* i_param, long cmd);
  static void __cdecl OnWndActivationGpscMap(void* i_param, long cmd);
  static void __cdecl OnWndActivationAtscMap(void* i_param, long cmd);

  void OnGridMapChangedIgn(int mapType);
  void OnGridMapClosedIgn(HWND, int);

  void OnGridMapChangedInj(int mapType);
  void OnGridMapClosedInj(HWND, int);

  int m_charts_enabled;
  ///////////////////////////////////////////////////////
  int m_work_map_chart_state;
  int m_temp_map_chart_state;
  int m_start_map_chart_state;
  int m_idle_map_chart_state;
  int m_grid_map_state_ign;
  int m_grid_map_state_inj;
  int m_ve_map_chart_state;
  int m_afr_map_chart_state;
  int m_crnk_map_chart_state;
  int m_wrmp_map_chart_state;
  int m_dead_map_chart_state;
  int m_idlr_map_chart_state;
  int m_idlc_map_chart_state;
  int m_aetps_map_chart_state;
  int m_aerpm_map_chart_state;
  int m_aftstr_map_chart_state;
  int m_it_map_chart_state;
  int m_itrpm_map_chart_state;
  int m_rigid_map_chart_state;
  int m_egocrv_map_chart_state;
  int m_iacc_map_chart_state;
  int m_iaccw_map_chart_state;
  int m_iatclt_map_chart_state;
  int m_tpsswt_map_chart_state;
  int m_gtsc_map_chart_state;
  int m_gpsc_map_chart_state;
  int m_atsc_map_chart_state;

  HWND m_start_map_wnd_handle;
  HWND m_idle_map_wnd_handle;
  HWND m_work_map_wnd_handle;
  HWND m_temp_map_wnd_handle;
  HWND m_ve_map_wnd_handle;
  HWND m_afr_map_wnd_handle;
  HWND m_crnk_map_wnd_handle;
  HWND m_wrmp_map_wnd_handle;
  HWND m_dead_map_wnd_handle;
  HWND m_idlr_map_wnd_handle;
  HWND m_idlc_map_wnd_handle;
  HWND m_aetps_map_wnd_handle;
  HWND m_aerpm_map_wnd_handle;
  HWND m_aftstr_map_wnd_handle;
  HWND m_it_map_wnd_handle;
  HWND m_itrpm_map_wnd_handle;
  HWND m_rigid_map_wnd_handle;
  HWND m_egocrv_map_wnd_handle;
  HWND m_iacc_map_wnd_handle;
  HWND m_iaccw_map_wnd_handle;
  HWND m_iatclt_map_wnd_handle;
  HWND m_tpsswt_map_wnd_handle;
  HWND m_gtsc_map_wnd_handle;
  HWND m_gpsc_map_wnd_handle;
  HWND m_atsc_map_wnd_handle;

  float m_start_map_active[16];
  float m_start_map_original[16];
  float m_idle_map_active[16];
  float m_idle_map_original[16];
  float m_work_map_active[16][16];
  float m_work_map_original[16][16];
  float m_temp_map_active[16];
  float m_temp_map_original[16];
  float m_rpm_grid_values[16];

  float m_ve_map_active[16][16];
  float m_ve_map_original[16][16];
  float m_afr_map_active[16][16];
  float m_afr_map_original[16][16];
  float m_crnk_map_active[16];
  float m_crnk_map_original[16];
  float m_wrmp_map_active[16];
  float m_wrmp_map_original[16];
  float m_dead_map_active[32];
  float m_dead_map_original[32];
  float m_idlr_map_active[16];
  float m_idlr_map_original[16];
  float m_idlc_map_active[16];
  float m_idlc_map_original[16];
  float m_aetps_map_active[16];
  float m_aetps_map_original[16];
  float m_aerpm_map_active[8];
  float m_aerpm_map_original[8];
  float m_aftstr_map_active[16];
  float m_aftstr_map_original[16];
  float m_it_map_active[16][16];
  float m_it_map_original[16][16];
  float m_itrpm_map_active[16];
  float m_itrpm_map_original[16];
  float m_rigid_map_active[8];
  float m_rigid_map_original[8];
  float m_egocrv_map_active[16+2];
  float m_egocrv_map_original[16+2];
  float m_iacc_map_active[8+2];
  float m_iacc_map_original[8+2];
  float m_iaccw_map_active[16+2];
  float m_iaccw_map_original[16+2];
  float m_iatclt_map_active[8+2];
  float m_iatclt_map_original[8+2];
  float m_tpsswt_map_active[16];
  float m_tpsswt_map_original[16];
  float m_gtsc_map_active[16];
  float m_gtsc_map_original[16];
  float m_gpsc_map_active[9+2];
  float m_gpsc_map_original[9+2];
  float m_atsc_map_active[16];
  float m_atsc_map_original[16];

  ///////////////////////////////////////////////////////
  bool m_en_aa_indication;
  bool m_carb_afr;
  bool m_en_gas_corr;
protected:
  HWND _ChartParentHwnd(void);
  void OnOpenMapWnd(HWND i_hwnd, int i_mapType);
  void OnWndActivation(HWND i_hwnd, long cmd);

  bool m_fuel_injection;
  bool m_gasdose;
  bool m_choke_op_enabled;

  float m_ldaxMinVal;
  float m_ldaxMaxVal;
  int m_ldaxCfg;

  bool m_children_charts;

  HWND m_openedChart;
  bool m_toggleMapWnd;
  
  int m_it_mode;
};
