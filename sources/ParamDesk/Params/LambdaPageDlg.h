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

/** \file LambdaPageDlg.h
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

class CLambdaPageDlg : public CParamTabBaseDlg, public ParamPageEvents
{
  typedef CParamTabBaseDlg Super;

 public:
  CLambdaPageDlg(CWnd* pParent = NULL);   // standard constructor
 ~CLambdaPageDlg();
  virtual LPCTSTR GetDialogID(void) const;
  static const UINT IDD;

  void Enable(bool enable);
  bool IsEnabled(void);

  void GetValues(SECU3IO::LambdaPar* o_values);
  void SetValues(const SECU3IO::LambdaPar* i_values);

 // Implementation
 protected:
  virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV support
  virtual BOOL OnInitDialog();
  afx_msg void OnDestroy();
  afx_msg void OnChangeData();
  afx_msg void OnUpdateControls(CCmdUI* pCmdUI);
  DECLARE_MESSAGE_MAP()

 private:
  SECU3IO::LambdaPar m_params;
  bool m_enabled;

  CComboBox m_senstype_combo;
  CSpinButtonCtrlEx m_strperstp_spin;
  CEditEx m_strperstp_edit;
  CSpinButtonCtrlEx m_stepsize_p_spin;
  CEditEx m_stepsize_p_edit;
  CSpinButtonCtrlEx m_stepsize_m_spin;
  CEditEx m_stepsize_m_edit;
  CSpinButtonCtrlEx m_corrlimit_p_spin;
  CEditEx m_corrlimit_p_edit;
  CSpinButtonCtrlEx m_corrlimit_m_spin;
  CEditEx m_corrlimit_m_edit;
  CSpinButtonCtrlEx m_swtpoint_spin;
  CEditEx m_swtpoint_edit;
  CSpinButtonCtrlEx m_tempthrd_spin;
  CEditEx m_tempthrd_edit;
  CSpinButtonCtrlEx m_rpmthrd_spin;
  CEditEx m_rpmthrd_edit;
  CSpinButtonCtrlEx m_activdelay_spin;
  CEditEx m_activdelay_edit;
  CSpinButtonCtrlEx m_deadband_spin;
  CEditEx m_deadband_edit;

  std::auto_ptr<CToolTipCtrlEx> mp_ttc;
  std::auto_ptr<CWndScroller> mp_scr;
};
