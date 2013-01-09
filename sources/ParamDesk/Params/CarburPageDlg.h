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

#include <memory>
#include "io-core/SECU3IO.h"
#include "common/ParamPageEvents.h"
#include "ui-core/EditEx.h"
#include "ui-core/SpinButtonCtrlEx.h"
#include "ui-core/TabDialog.h"

class CToolTipCtrlEx;

class CCarburPageDlg : public CTabDialog, public ParamPageEvents
{
  typedef CTabDialog Super;

 public:
  CCarburPageDlg(CWnd* pParent = NULL); // standard constructor
  virtual LPCTSTR GetDialogID(void) const;
  static const UINT IDD;

  void Enable(bool enable);
  bool IsEnabled(void);

  void GetValues(SECU3IO::CarburPar* o_values);
  void SetValues(const SECU3IO::CarburPar* i_values);

 // Implementation
 protected:
  virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV support
  virtual BOOL OnInitDialog();
  afx_msg void OnChangeData();
  afx_msg void OnUpdateControls(CCmdUI* pCmdUI);
  DECLARE_MESSAGE_MAP()

 private:
  SECU3IO::CarburPar m_params;
  bool m_enabled;

  CButton m_inverse_throttle_switch;
  CSpinButtonCtrlEx m_shutoff_lo_threshold_spin;
  CSpinButtonCtrlEx m_shutoff_hi_threshold_spin;
  CSpinButtonCtrlEx m_epm_on_threshold_spin;
  CSpinButtonCtrlEx m_shutoff_lo_threshold_spin_g;
  CSpinButtonCtrlEx m_shutoff_hi_threshold_spin_g;
  CSpinButtonCtrlEx m_shutoff_delay_spin;
  CSpinButtonCtrlEx m_tps_threshold_spin;
  CEditEx m_shutoff_lo_threshold_edit;
  CEditEx m_shutoff_hi_threshold_edit;
  CEditEx m_epm_on_threshold_edit;
  CEditEx m_shutoff_lo_threshold_edit_g;
  CEditEx m_shutoff_hi_threshold_edit_g;
  CEditEx m_shutoff_delay_edit;
  CEditEx m_tps_threshold_edit;

  std::auto_ptr<CToolTipCtrlEx> mp_ttc;
};
