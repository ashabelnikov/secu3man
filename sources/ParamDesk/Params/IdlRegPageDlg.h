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

#pragma once

#include "io-core/SECU3IO.h"
#include "common/ParamPageEvents.h"
#include "ui-core/EditEx.h"
#include "ui-core/SpinButtonCtrlEx.h"
#include "ui-core/TabDialog.h"
#include <memory>

class CToolTipCtrlEx;

class CIdlRegPageDlg : public CTabDialog, public ParamPageEvents
{
  typedef CTabDialog Super;

 public:
  CIdlRegPageDlg(CWnd* pParent = NULL); // standard constructor
  virtual LPCTSTR GetDialogID(void) const;
  static const UINT IDD;

  void Enable(bool enable);
  bool IsEnabled(void);

  void GetValues(SECU3IO::IdlRegPar* o_values);
  void SetValues(const SECU3IO::IdlRegPar* i_values);

 // Implementation
 protected:
  virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV support
  virtual BOOL OnInitDialog();
  afx_msg void OnChangeData();
  afx_msg void OnUpdateControls(CCmdUI* pCmdUI);
  DECLARE_MESSAGE_MAP()

 private:
  SECU3IO::IdlRegPar m_params;
  bool m_enabled;

  CButton m_use_regulator;
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
    
  std::auto_ptr<CToolTipCtrlEx> mp_ttc;
};
