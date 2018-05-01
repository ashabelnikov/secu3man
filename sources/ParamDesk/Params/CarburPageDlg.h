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

/** \file CarburPageDlg.h
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

class CToolTipCtrlEx;
class CWndScroller;

class CCarburPageDlg : public CParamTabBaseDlg, public ParamPageEvents
{
  typedef CParamTabBaseDlg Super;

 public:
  CCarburPageDlg(CWnd* pParent = NULL); // standard constructor
  virtual LPCTSTR GetDialogID(void) const;
  static const UINT IDD;

  void Enable(bool enable);
  bool IsEnabled(void);
  void EnableFuelInjection(bool i_enable);
  void EnableGasdose(bool i_enable);

  void GetValues(SECU3IO::CarburPar* o_values);
  void SetValues(const SECU3IO::CarburPar* i_values);

 // Implementation
 protected:
  virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV support
  virtual BOOL OnInitDialog();
  afx_msg void OnDestroy();
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnChangeData();
  afx_msg void OnUpdateControls(CCmdUI* pCmdUI);
  afx_msg void OnUpdateGasControls(CCmdUI* pCmdUI);
  afx_msg void OnUpdateInjGasControls(CCmdUI* pCmdUI);
  afx_msg void OnUpdateFuelInjectionControls(CCmdUI* pCmdUI);
  afx_msg void OnUpdateCarburetorControls(CCmdUI* pCmdUI);
  DECLARE_MESSAGE_MAP()

 private:
  SECU3IO::CarburPar m_params;
  bool m_enabled;
  bool m_fuel_injection;
  bool m_gasdose;

  CButton m_inverse_throttle_switch;
  CSpinButtonCtrlEx m_shutoff_lo_threshold_spin;
  CEditEx m_shutoff_lo_threshold_edit;
  CSpinButtonCtrlEx m_shutoff_hi_threshold_spin;
  CEditEx m_shutoff_hi_threshold_edit;
  CSpinButtonCtrlEx m_epm_on_threshold_spin;
  CEditEx m_epm_on_threshold_edit;
  CSpinButtonCtrlEx m_shutoff_lo_threshold_spin_g;
  CEditEx m_shutoff_lo_threshold_edit_g;
  CSpinButtonCtrlEx m_shutoff_hi_threshold_spin_g;
  CEditEx m_shutoff_hi_threshold_edit_g;
  CSpinButtonCtrlEx m_shutoff_delay_spin;
  CEditEx m_shutoff_delay_edit;
  CSpinButtonCtrlEx m_tps_threshold_spin;
  CEditEx m_tps_threshold_edit;
  CSpinButtonCtrlEx m_fuelcut_map_thrd_spin;
  CEditEx m_fuelcut_map_thrd_edit;
  CSpinButtonCtrlEx m_fuelcut_cts_thrd_spin;
  CEditEx m_fuelcut_cts_thrd_edit;
  CSpinButtonCtrlEx m_revlim_lo_threshold_spin;
  CEditEx m_revlim_lo_threshold_edit;
  CSpinButtonCtrlEx m_revlim_hi_threshold_spin;
  CEditEx m_revlim_hi_threshold_edit;

  std::auto_ptr<CWndScroller> mp_scr;
  std::auto_ptr<CToolTipCtrlEx> mp_ttc;
};
