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

#include <string>
#include "io-core/SECU3IO.h"
#include "common/ParamPageEvents.h"
#include "ui-core/EditEx.h"
#include "ui-core/SpinButtonCtrlEx.h"
#include "ui-core/TabDialog.h"

class CStarterPageDlg : public CTabDialog, public ParamPageEvents
{
  typedef CTabDialog Super;

 public:
  CStarterPageDlg(CWnd* pParent = NULL);   // standard constructor
  virtual LPCTSTR GetDialogID(void) const;
  static const UINT IDD;

  void Enable(bool enable);
  bool IsEnabled(void);

  void GetValues(SECU3IO::StartrPar* o_values);
  void SetValues(const SECU3IO::StartrPar* i_values);

 // Implementation
 protected:
  virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV support
  virtual BOOL OnInitDialog();
  afx_msg void OnChangePdStarterOffRpmEdit();
  afx_msg void OnChangePdStarterSmapAbandonRpmEdit();
  afx_msg void OnUpdateControls(CCmdUI* pCmdUI);
  DECLARE_MESSAGE_MAP()

 private:
  SECU3IO::StartrPar m_params;
  bool m_enabled;

  CSpinButtonCtrlEx m_smap_abandon_rpm_spin;
  CSpinButtonCtrlEx m_starter_off_rpm_spin;
  CEditEx m_smap_abandon_rpm_edit;
  CEditEx m_starter_off_rpm_edit;
};
