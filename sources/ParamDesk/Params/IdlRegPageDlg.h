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

/** \file IdlRegPageDlg.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include <memory>
#include "io-core/SECU3IO.h"
#include "common/ParamPageEvents.h"
#include "common/ParamTabBaseDlg.h"
#include "ui-core/EditEx.h"
#include "ui-core/SpinButtonCtrlEx.h"
#include "ui-core/TabDialog.h"

class CWndScroller;

class CToolTipCtrlEx;

class CIdlRegPageDlg : public CParamTabBaseDlg, public ParamPageEvents
{
  typedef CParamTabBaseDlg Super;

 public:
  CIdlRegPageDlg(CWnd* pParent = NULL); // standard constructor
  virtual LPCTSTR GetDialogID(void) const;
  static const UINT IDD;

  void Enable(bool enable);
  bool IsEnabled(void);
  void EnableFuelInjection(bool i_enable);

  void GetValues(SECU3IO::IdlRegPar* o_values);
  void SetValues(const SECU3IO::IdlRegPar* i_values);

 // Implementation
 protected:
  virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV support
  virtual BOOL OnInitDialog();
  afx_msg void OnDestroy();
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnChangeData();
  afx_msg void OnChangeDataThrass();
  afx_msg void OnUpdateControls(CCmdUI* pCmdUI);
  afx_msg void OnUpdateFuelInjectionControls(CCmdUI* pCmdUI);
  afx_msg void OnUpdateFuelInjectionControlsAdd(CCmdUI* pCmdUI);
  DECLARE_MESSAGE_MAP()

 private:
  std::auto_ptr<CWndScroller> mp_scr;
  std::auto_ptr<CToolTipCtrlEx> mp_ttc;

  SECU3IO::IdlRegPar m_params;
  bool m_enabled;
  bool m_fuel_injection;

  CButton m_use_regulator;
  CButton m_use_regongas;
  CButton m_use_closedloop;
  CButton m_preg_mode_check;
  CButton m_use_claicongas_check;
  CButton m_use_thrassmap_check;
  CSpinButtonCtrlEx m_goal_rpm_spin;
  CEditEx m_goal_rpm_edit;
  CSpinButtonCtrlEx m_factor_pos_spin;
  CEditEx m_factor_pos_edit;
  CSpinButtonCtrlEx m_factor_neg_spin;
  CEditEx m_factor_neg_edit;
  CSpinButtonCtrlEx m_dead_band_rpm_spin;
  CEditEx m_dead_band_rpm_edit;
  CSpinButtonCtrlEx m_restriction_min_spin;
  CEditEx m_restriction_min_edit;
  CSpinButtonCtrlEx m_restriction_max_spin;
  CEditEx m_restriction_max_edit;
  CSpinButtonCtrlEx m_turn_on_temp_spin;
  CEditEx m_turn_on_temp_edit;
  //closed loop parameters
  CSpinButtonCtrlEx m_idltorunadd_spin;
  CEditEx m_idltorunadd_edit;
  CSpinButtonCtrlEx m_rpmonrunadd_spin;
  CEditEx m_rpmonrunadd_edit;
  CSpinButtonCtrlEx m_idlregp_spin;
  CEditEx m_idlregp_edit;
  CSpinButtonCtrlEx m_idlregi_spin;
  CEditEx m_idlregi_edit;
  CSpinButtonCtrlEx m_idlregpm_spin;
  CEditEx m_idlregpm_edit;
  CSpinButtonCtrlEx m_idlregim_spin;
  CEditEx m_idlregim_edit;
  CSpinButtonCtrlEx m_coeffthrd1_spin;
  CEditEx m_coeffthrd1_edit;
  CSpinButtonCtrlEx m_coeffthrd2_spin;
  CEditEx m_coeffthrd2_edit;
  CSpinButtonCtrlEx m_intrpmlim_spin;
  CEditEx m_intrpmlim_edit;
  CSpinButtonCtrlEx m_mapvalue_spin;
  CEditEx m_mapvalue_edit;
  CSpinButtonCtrlEx m_iacminpos_spin;
  CEditEx m_iacminpos_edit;
  CSpinButtonCtrlEx m_iacmaxpos_spin;
  CEditEx m_iacmaxpos_edit;
  CSpinButtonCtrlEx m_iac_deadband_spin;
  CEditEx m_iac_deadband_edit;
};
