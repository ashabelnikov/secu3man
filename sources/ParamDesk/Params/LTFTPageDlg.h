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

/** \file LTFTPageDlg.h
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

class CLTFTPageDlg : public CParamTabBaseDlg, public ParamPageEvents
{
  typedef CParamTabBaseDlg Super;

 public:
  CLTFTPageDlg();
 ~CLTFTPageDlg();
  virtual LPCTSTR GetDialogID(void) const;

  void Enable(bool enable);
  bool IsEnabled(void);
  void EnableFuelInjection(bool i_enable);

  void GetValues(SECU3IO::LtftPar* o_values);
  void SetValues(const SECU3IO::LtftPar* i_values);

 // Implementation
 protected:
  virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV support
  virtual BOOL OnInitDialog();
  afx_msg void OnDestroy();
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnChangeData();
  afx_msg void OnUpdateControls(CCmdUI* pCmdUI);
  DECLARE_MESSAGE_MAP()

 private:
  SECU3IO::LtftPar m_params;
  bool m_enabled;

  CComboBox m_ltft_mode_combo;
  CSpinButtonCtrlEx m_ltft_learn_clt_dw_spin;
  CEditEx m_ltft_learn_clt_dw_edit;
  CSpinButtonCtrlEx m_ltft_learn_clt_up_spin;
  CEditEx m_ltft_learn_clt_up_edit;
  CSpinButtonCtrlEx m_ltft_learn_iat_up_spin;
  CEditEx m_ltft_learn_iat_up_edit;
  CSpinButtonCtrlEx m_ltft_learn_grad_spin;
  CEditEx m_ltft_learn_grad_edit;
  CSpinButtonCtrlEx m_ltft_learn_gpa_spin;
  CEditEx m_ltft_learn_gpa_edit;
  CSpinButtonCtrlEx m_ltft_learn_gpd_spin;
  CEditEx m_ltft_learn_gpd_edit;
  CSpinButtonCtrlEx m_ltft_min_spin;
  CEditEx m_ltft_min_edit;
  CSpinButtonCtrlEx m_ltft_max_spin;
  CEditEx m_ltft_max_edit;
  CSpinButtonCtrlEx m_ltft_learn_rpm_spin[2];
  CEditEx m_ltft_learn_rpm_edit[2];
  CSpinButtonCtrlEx m_ltft_learn_load_spin[2];
  CEditEx m_ltft_learn_load_edit[2];
  CSpinButtonCtrlEx m_ltft_dead_band_spin[2];
  CEditEx m_ltft_dead_band_edit[2];

  std::auto_ptr<CToolTipCtrlEx> mp_ttc;
  std::auto_ptr<CWndScroller> mp_scr;
};
