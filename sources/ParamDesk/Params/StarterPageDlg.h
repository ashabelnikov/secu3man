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

/** \file StarterPageDlg.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include <string>
#include <memory>
#include "io-core/SECU3IO.h"
#include "common/ParamPageEvents.h"
#include "common/ParamTabBaseDlg.h"
#include "ui-core/EditEx.h"
#include "ui-core/SpinButtonCtrlEx.h"
#include "ui-core/TabDialog.h"

class CToolTipCtrlEx;
class CWndScroller;

class CStarterPageDlg : public CParamTabBaseDlg, public ParamPageEvents
{
  typedef CParamTabBaseDlg Super;

 public:
  CStarterPageDlg(CWnd* pParent = NULL);   // standard constructor
  virtual LPCTSTR GetDialogID(void) const;
  static const UINT IDD;

  void Enable(bool enable);
  bool IsEnabled(void);

  void GetValues(SECU3IO::StartrPar* o_values);
  void SetValues(const SECU3IO::StartrPar* i_values);

  void EnableFuelInjection(bool i_enable);
  void EnableGasdose(bool i_enable);

 // Implementation
 protected:
  virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV support
  virtual BOOL OnInitDialog();
  afx_msg void OnDestroy();
  afx_msg void OnChangeData();
  afx_msg void OnUpdateControls(CCmdUI* pCmdUI);
  afx_msg void OnUpdateFuelInjectionControls(CCmdUI* pCmdUI);
  afx_msg void OnUpdateInjGasControls(CCmdUI* pCmdUI);
  DECLARE_MESSAGE_MAP()

 private:
  SECU3IO::StartrPar m_params;
  bool m_enabled;
  bool m_fuel_injection;
  bool m_gasdose;

  CSpinButtonCtrlEx m_smap_abandon_rpm_spin;
  CEditEx m_smap_abandon_rpm_edit;
  CSpinButtonCtrlEx m_starter_off_rpm_spin;
  CEditEx m_starter_off_rpm_edit;
  CSpinButtonCtrlEx m_cranktoruntime_spin;
  CEditEx m_cranktoruntime_edit;
  CSpinButtonCtrlEx m_aftstrstr_spin;
  CEditEx m_aftstrstr_edit;
  CSpinButtonCtrlEx m_primecold_spin;
  CEditEx m_primecold_edit;
  CSpinButtonCtrlEx m_primehot_spin;
  CEditEx m_primehot_edit;
  CSpinButtonCtrlEx m_primedelay_spin;
  CEditEx m_primedelay_edit;
  CSpinButtonCtrlEx m_fldclrtps_spin;
  CEditEx m_fldclrtps_edit;

  std::auto_ptr<CToolTipCtrlEx> mp_ttc;
  std::auto_ptr<CWndScroller> mp_scr;
};
