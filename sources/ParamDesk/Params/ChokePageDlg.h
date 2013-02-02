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
#include <string>
#include "io-core/SECU3IO.h"
#include "common/ParamPageEvents.h"
#include "ui-core/EditEx.h"
#include "ui-core/SpinButtonCtrlEx.h"
#include "ui-core/TabDialog.h"

class CToolTipCtrlEx;

class CChokePageDlg : public CTabDialog, public ParamPageEvents
{
  typedef CTabDialog Super;

 public:
  CChokePageDlg(CWnd* pParent = NULL);   // standard constructor
  virtual LPCTSTR GetDialogID(void) const;
  static const UINT IDD;

  void Enable(bool enable);
  bool IsEnabled(void);

  void GetValues(SECU3IO::ChokePar* o_values);
  void SetValues(const SECU3IO::ChokePar* i_values);

  void EnableChokeTesting(bool enable);

 // Implementation
 protected:
  virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV support
  virtual BOOL OnInitDialog();
  afx_msg void OnChangePdChokeSMStepsNumEdit();
  afx_msg void OnUpdateControls(CCmdUI* pCmdUI);
  afx_msg void OnUpdateChokeTestBtn(CCmdUI* pCmdUI);
  afx_msg void OnSMTestButton();
  DECLARE_MESSAGE_MAP()

 private:
  SECU3IO::ChokePar m_params;
  bool m_enabled;
  bool m_choketst_enabled;

  CSpinButtonCtrlEx m_sm_steps_num_spin;
  CEditEx m_sm_steps_num_edit;
  CButton m_sm_test_check;

  std::auto_ptr<CToolTipCtrlEx> mp_ttc;
};
