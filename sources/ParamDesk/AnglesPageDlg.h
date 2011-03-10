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

#include "io-core\SECU3IO.h"
#include "ParamPageEvents.h"
#include "ui-core\EditEx.h"
#include "ui-core\SpinButtonCtrlEx.h"
#include "ui-core\TabDialog.h"

class CAnglesPageDlg : public CTabDialog, public ParamPageEvents
{
  typedef CTabDialog Super;

 public:
  CAnglesPageDlg(CWnd* pParent = NULL); // standard constructor
  virtual LPCTSTR GetDialogID(void) const;
  static const UINT IDD;

  void Enable(bool enable);
  bool IsEnabled(void);

  void GetValues(SECU3IO::AnglesPar* o_values);
  void SetValues(const SECU3IO::AnglesPar* i_values);

 // Implementation
 protected:
  virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV support
  virtual BOOL OnInitDialog();
  afx_msg void OnChangeData();
  afx_msg void OnUpdateControls(CCmdUI* pCmdUI);
  DECLARE_MESSAGE_MAP()

 private:
  SECU3IO::AnglesPar m_params;
  BOOL m_enabled;

  CSpinButtonCtrlEx m_min_angle_spin;
  CSpinButtonCtrlEx m_max_angle_spin;
  CSpinButtonCtrlEx m_correction_spin;
  CSpinButtonCtrlEx m_decrease_spead_spin;
  CSpinButtonCtrlEx m_increase_spead_spin;
  CEditEx m_min_angle_edit;
  CEditEx m_max_angle_edit;
  CEditEx m_correction_edit;
  CEditEx m_decrease_spead_edit;
  CEditEx m_increase_spead_edit;
};
