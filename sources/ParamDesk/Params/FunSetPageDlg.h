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

/** \file FunSetPageDlg.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include <vector>
#include "common/unicodesupport.h"
#include "common/objecttimer.h"
#include "io-core/SECU3IO.h"
#include "common/ParamPageEvents.h"
#include "common/ParamTabBaseDlg.h"
#include "ui-core/EditEx.h"
#include "ui-core/SpinButtonCtrlEx.h"
#include "ui-core/TabDialog.h"

class CToolTipCtrlEx;
class CWndScroller;

class CFunSetPageDlg : public CParamTabBaseDlg, public ParamPageEvents
{
  typedef CParamTabBaseDlg Super;
  typedef fastdelegate::FastDelegate1<int> EventWithCode;

 public:
  CFunSetPageDlg(CWnd* pParent = NULL, bool m_tps_learning = true); // standard constructor
  virtual LPCTSTR GetDialogID(void) const;
  static const UINT IDD;

  void Enable(bool enable);
  bool IsEnabled(void);
  //true - SECU-3T, false - SECU-3i
  void EnableSECU3TItems(bool i_enable);
  void EnableFuelInjection(bool i_enable);

  void FillCBByFunNames(void);
  std::vector<_TSTRING>& AccessFunNames(void);

  void GetValues(SECU3IO::FunSetPar* o_values);
  void SetValues(const SECU3IO::FunSetPar* i_values);

  void FillCBByLoadOpts(void);

  void setOnTPSLearning(EventWithCode OnCB) {m_OnTPSLearning = OnCB;}

 // Implementation
 protected:
  virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV support
  virtual BOOL OnInitDialog();
  afx_msg void OnDestroy();
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnChangeData();
  afx_msg void OnChangeDataLdaxGrid();
  afx_msg void OnChangeDataLoadSrc();
  afx_msg void OnMapCalcButton();
  afx_msg void OnMap2CalcButton();
  afx_msg void OnTpsCalcButton();
  afx_msg void OnChangeVE2MF();
  afx_msg void OnUpdateControls(CCmdUI* pCmdUI);
  afx_msg void OnUpdateControlsLower(CCmdUI* pCmdUI);
  afx_msg void OnUpdateControlsUpper(CCmdUI* pCmdUI);
  afx_msg void OnUpdateControlsSECU3i(CCmdUI* pCmdUI);
  afx_msg void OnUpdateControlsFuelInject(CCmdUI* pCmdUI);
  afx_msg void OnUpdateControlsTPSLearning(CCmdUI* pCmdUI);
  DECLARE_MESSAGE_MAP()

 private:
  void UpdateLoadAxisUnits(void);
  void OnTPsLearningPushTimer(void);
  void OnTPsLearningReleaseTimer(void);
  SECU3IO::FunSetPar m_params;
  std::vector<_TSTRING> m_fun_names;
  bool m_enabled;
  bool m_enable_secu3t_features;
  bool m_fuel_injection;
  bool m_tps_learning;

  CComboBox m_gas_maps_combo;
  CComboBox m_benzin_maps_combo;
  CComboBox m_load_src_combo;
  CComboBox m_gas_uni_combo;
  CComboBox m_benzin_uni_combo;
  CComboBox m_barocorr_type_combo;
  CComboBox m_ve2mf_combo;
  CComboBox m_gas_v_uni_combo;
  CSpinButtonCtrlEx m_press_swing_spin;
  CEditEx m_press_swing_edit;
  CSpinButtonCtrlEx m_map_grad_spin;
  CEditEx m_map_grad_edit;
  CSpinButtonCtrlEx m_map_curve_offset_spin;
  CEditEx m_map_curve_offset_edit;
  CSpinButtonCtrlEx m_map_curve_gradient_spin;
  CEditEx m_map_curve_gradient_edit;
  CSpinButtonCtrlEx m_tps_curve_offset_spin;
  CEditEx m_tps_curve_offset_edit;
  CSpinButtonCtrlEx m_tps_curve_gradient_spin;
  CEditEx m_tps_curve_gradient_edit;
  CBitmapButton m_calc_map_btn;
  CBitmapButton m_calc_map2_btn;
  CBitmapButton m_calc_tps_btn;
  CStatic m_lolo_unit;
  CStatic m_hilo_unit;
  CButton m_use_ldax_grid_check;

  CSpinButtonCtrlEx m_map2_curve_offset_spin;
  CEditEx m_map2_curve_offset_edit;
  CSpinButtonCtrlEx m_map2_curve_gradient_spin;
  CEditEx m_map2_curve_gradient_edit;

  std::auto_ptr<CToolTipCtrlEx> mp_ttc;
  std::auto_ptr<CWndScroller> mp_scr;  

  EventWithCode m_OnTPSLearning;
  CObjectTimer<CFunSetPageDlg> m_tpsl_tmr;  
  float m_tpsl_push_value;
  float m_tpsl_release_value;
};
