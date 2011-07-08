/* SECU-3  - An open source, free engine control unit
   Copyright (C) 2007 Alexey A. Shabelnikov. Ukraine, Gorlovka

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

#pragma once

#include "io-core/SECU3IO.h"
#include "common/ParamPageEvents.h"
#include "ui-core/EditEx.h"
#include "ui-core/SpinButtonCtrlEx.h"
#include "ui-core/TabDialog.h"

class CTemperPageDlg : public CTabDialog, public ParamPageEvents
{
  typedef CTabDialog Super;

 public:
  CTemperPageDlg(CWnd* pParent = NULL); // standard constructor
  virtual LPCTSTR GetDialogID(void) const;
  static const UINT IDD;

  void Enable(bool enable);
  bool IsEnabled(void);

  void GetValues(SECU3IO::TemperPar* o_values);
  void SetValues(const SECU3IO::TemperPar* i_values);

  void EnableUseVentPwm(bool enable);

 // Implementation
 protected:
  virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV support
  virtual BOOL OnInitDialog();
  afx_msg void OnChangePdTemperVentOnThresholdEdit();
  afx_msg void OnChangePdTemperVentOffThresholdEdit();
  afx_msg void OnPdTemperUseTempSensor();
  afx_msg void OnPdTemperUseVentPwm();
  afx_msg void OnUpdateControls(CCmdUI* pCmdUI);
  afx_msg void OnUpdateVentOff(CCmdUI* pCmdUI);
  afx_msg void OnUpdateUseVentPwm(CCmdUI* pCmdUI);
  DECLARE_MESSAGE_MAP()

 private:
  SECU3IO::TemperPar m_params;
  bool m_enabled;
  bool m_use_vent_pwm_enabled;

  CButton m_use_temp_sensor;
  CButton m_use_vent_pwm;
  CSpinButtonCtrlEx m_vent_on_threshold_spin;
  CSpinButtonCtrlEx m_vent_off_threshold_spin;
  CEditEx m_vent_off_threshold_edit;
  CEditEx m_vent_on_threshold_edit;
};
