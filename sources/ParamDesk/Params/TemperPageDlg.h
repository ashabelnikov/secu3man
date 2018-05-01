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

/** \file TemperPageDlg.h
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

class CTemperPageDlg : public CParamTabBaseDlg, public ParamPageEvents
{
  typedef CParamTabBaseDlg Super;

 public:
  CTemperPageDlg(CWnd* pParent = NULL); // standard constructor
  virtual LPCTSTR GetDialogID(void) const;
  static const UINT IDD;

  void Enable(bool enable);
  bool IsEnabled(void);

  void GetValues(SECU3IO::TemperPar* o_values);
  void SetValues(const SECU3IO::TemperPar* i_values);

  void EnableUseVentPwm(bool enable);
  void EnableUseCTSCurveMap(bool enable);

  void EnableSECU3TItems(bool i_enable);
  void EnableFuelInjection(bool enable);

 // Implementation
 protected:
  virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV support
  virtual BOOL OnInitDialog();
  afx_msg void OnDestroy();
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnChangePdTemperVentOnThresholdEdit();
  afx_msg void OnChangePdTemperVentOffThresholdEdit();
  afx_msg void OnChangeData();
  afx_msg void OnPdTemperUseTempSensor();
  afx_msg void OnPdTemperUseVentPwm();
  afx_msg void OnPdTemperUseCurveMap();
  afx_msg void OnUpdateControls(CCmdUI* pCmdUI);
  afx_msg void OnUpdateVentOff(CCmdUI* pCmdUI);
  afx_msg void OnUpdateUseVentPwm(CCmdUI* pCmdUI);
  afx_msg void OnUpdateUseCurveMap(CCmdUI* pCmdUI);
  afx_msg void OnUpdateControlsSECU3iFI(CCmdUI* pCmdUI);
  afx_msg void OnUpdateFuelInjectionControls(CCmdUI* pCmdUI);
  DECLARE_MESSAGE_MAP()

 private:
  std::auto_ptr<class CToolTipCtrlEx> mp_ttc;
  std::auto_ptr<CWndScroller> mp_scr;

  SECU3IO::TemperPar m_params;
  bool m_enabled;
  bool m_use_vent_pwm_enabled;
  bool m_use_curve_map_enabled;
  bool m_enable_secu3t_features;
  bool m_fuel_injection;

  CButton m_use_temp_sensor;
  CButton m_use_vent_pwm;
  CButton m_use_curve_map;
  CSpinButtonCtrlEx m_vent_on_threshold_spin;
  CEditEx m_vent_on_threshold_edit;
  CSpinButtonCtrlEx m_vent_off_threshold_spin;
  CEditEx m_vent_off_threshold_edit;
  CSpinButtonCtrlEx m_vent_pwmfrq_spin;
  CEditEx m_vent_pwmfrq_edit;
  CSpinButtonCtrlEx m_cond_pvt_on_spin;
  CEditEx m_cond_pvt_on_edit;
  CSpinButtonCtrlEx m_cond_pvt_off_spin;
  CEditEx m_cond_pvt_off_edit;
  CSpinButtonCtrlEx m_cond_min_rpm_spin;
  CEditEx m_cond_min_rpm_edit;
  CSpinButtonCtrlEx m_vent_timer_spin;
  CEditEx m_vent_timer_edit;
};
