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

/** \file GasdosePageDlg.h
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

class CGasdosePageDlg : public CParamTabBaseDlg, public ParamPageEvents
{
  typedef CParamTabBaseDlg Super;

 public:
  CGasdosePageDlg();
  virtual LPCTSTR GetDialogID(void) const;

  void Enable(bool enable);
  bool IsEnabled(void);

  void GetValues(SECU3IO::GasdosePar* o_values);
  void SetValues(const SECU3IO::GasdosePar* i_values);
  void LockUIUpdate(bool lock);

  void EnableGasdoseTesting(bool enable);
  void EnableGasdoseManPos(bool enable);

 // Implementation
 protected:
  virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV support
  virtual BOOL OnInitDialog();
  afx_msg void OnDestroy();
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnChangePdSMStepsNumEdit();
  afx_msg void OnChangeData();
  afx_msg void OnUpdateControls(CCmdUI* pCmdUI);
  afx_msg void OnUpdateGasdoseTestBtn(CCmdUI* pCmdUI);
  afx_msg void OnUpdateGasdoseManPosBtn(CCmdUI* pCmdUI);
  afx_msg void OnSMTestButton();
  afx_msg void OnManualDeltapos(NMHDR* pNMHDR, LRESULT* pResult);
  afx_msg void OnUpdateGasdoseSMSteps(CCmdUI* pCmdUI);
  DECLARE_MESSAGE_MAP()

 private:
  SECU3IO::GasdosePar m_params;
  bool m_enabled;
  bool m_gasdose_tst_enabled;
  bool m_gasdose_manpos_enabled;
  bool m_lock_ui_update;

  CSpinButtonCtrlEx m_sm_steps_num_spin;
  CEditEx m_sm_steps_num_edit;
  CButton m_sm_test_check;
  CSpinButtonCtrl m_man_ctrl_spin;

  CSpinButtonCtrlEx m_fc_closing_spin;
  CEditEx m_fc_closing_edit;

  CSpinButtonCtrlEx m_corrlimit_p_spin;
  CEditEx m_corrlimit_p_edit;
  CSpinButtonCtrlEx m_corrlimit_m_spin;
  CEditEx m_corrlimit_m_edit;

  CSpinButtonCtrlEx m_stoichval_spin;
  CEditEx m_stoichval_edit;

  CComboBox m_sm_freq_combo;

  CButton m_sm_maxfreqinit_check;

  std::auto_ptr<CToolTipCtrlEx> mp_ttc;
  std::auto_ptr<CWndScroller> mp_scr;
};
