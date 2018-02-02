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

/** \file ChokePageDlg.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include <memory>
#include <string>
#include "io-core/SECU3IO.h"
#include "common/ParamPageEvents.h"
#include "common/ParamTabBaseDlg.h"
#include "ui-core/EditEx.h"
#include "ui-core/SpinButtonCtrlEx.h"
#include "ui-core/TabDialog.h"

class CToolTipCtrlEx;
class CWndScroller;

class CChokePageDlg : public CParamTabBaseDlg, public ParamPageEvents
{
  typedef CParamTabBaseDlg Super;

 public:
  CChokePageDlg(CWnd* pParent = NULL);   // standard constructor
  virtual LPCTSTR GetDialogID(void) const;
  static const UINT IDD;

  void Enable(bool enable);
  bool IsEnabled(void);

  void GetValues(SECU3IO::ChokePar* o_values);
  void SetValues(const SECU3IO::ChokePar* i_values);
  void LockUIUpdate(bool lock);

  void EnableChokeTesting(bool enable);
  void EnableChokeManPos(bool enable);
  void EnableFuelInjection(bool enable);

 // Implementation
 protected:
  virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV support
  virtual BOOL OnInitDialog();
  afx_msg void OnDestroy();
  afx_msg void OnChangePdChokeSMStepsNumEdit();
  afx_msg void OnChangeData();
  afx_msg void OnUpdateControls(CCmdUI* pCmdUI);
  afx_msg void OnUpdateControlsCarb(CCmdUI* pCmdUI);
  afx_msg void OnUpdateChokeTestBtn(CCmdUI* pCmdUI);
  afx_msg void OnUpdateChokeManPosBtn(CCmdUI* pCmdUI);
  afx_msg void OnSMTestButton();
  afx_msg void OnManualDeltapos(NMHDR* pNMHDR, LRESULT* pResult);
  DECLARE_MESSAGE_MAP()

 private:
  SECU3IO::ChokePar m_params;
  bool m_enabled;
  bool m_choketst_enabled;
  bool m_chokemanpos_enabled;
  bool m_fuel_injection;
  bool m_lock_ui_update;

  CSpinButtonCtrlEx m_sm_steps_num_spin;
  CEditEx m_sm_steps_num_edit;
  CButton m_sm_test_check;
  CSpinButtonCtrl m_man_ctrl_spin;

  CSpinButtonCtrlEx m_rpmreg_if_spin;
  CEditEx m_rpmreg_if_edit;

  CSpinButtonCtrlEx m_strt_add_tm1_spin;
  CEditEx m_strt_add_tm1_edit;

  CSpinButtonCtrlEx m_strt_add_tm2_spin;
  CEditEx m_strt_add_tm2_edit;

  CButton m_offstrtadd_ongas_check;
  CButton m_offrpmreg_ongas_check;
  CButton m_usethrottle_pos_check;
  CButton m_sm_maxfreqinit_check;

  CComboBox m_sm_freq_combo;

  std::auto_ptr<CWndScroller> mp_scr;
  std::auto_ptr<CToolTipCtrlEx> mp_ttc;
};
