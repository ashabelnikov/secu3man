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

class CSecurPageDlg : public CTabDialog, public ParamPageEvents
{
  typedef CTabDialog Super;

 public:
  CSecurPageDlg(CWnd* pParent = NULL);   // standard constructor
  virtual LPCTSTR GetDialogID(void) const;
  static const UINT IDD;

  void Enable(bool enable);
  bool IsEnabled(void);

  void GetValues(SECU3IO::SecurPar* o_values);
  void SetValues(const SECU3IO::SecurPar* i_values);

  void EnableBTNameAndPass(bool enable);

 // Implementation
 protected:
  virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV support
  virtual BOOL OnInitDialog();
  afx_msg void OnChangeData();
  afx_msg void OnChangeDataApply();
  afx_msg void OnChangeDataUseBtCheck();
  afx_msg void OnChangeDataNamePass();
  afx_msg void OnUpdateControls(CCmdUI* pCmdUI);
  afx_msg void OnUpdateNameAndPass(CCmdUI* pCmdUI);
  afx_msg void OnUpdateApplyButton(CCmdUI* pCmdUI);
  DECLARE_MESSAGE_MAP()

 private:
  SECU3IO::SecurPar m_params;
  _TSTRING m_bt_name;
  _TSTRING m_bt_pass;
  bool m_enabled;
  bool m_namepass_enabled;
  
  CEditEx m_bt_name_edit;
  CStatic m_bt_name_caption;

  CEditEx m_bt_pass_edit;
  CStatic m_bt_pass_caption;

  CButton m_bt_apply_button;
  CButton m_bt_use_check;
  CButton m_imm_use_check;

  std::auto_ptr<CToolTipCtrlEx> mp_ttc;
};
