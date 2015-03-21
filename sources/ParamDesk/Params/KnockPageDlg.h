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

/** \file KnockPageDlg.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include <memory>
#include "io-core/SECU3IO.h"
#include "../common/ParamPageEvents.h" //because included externally
#include "../common/ParamTabBaseDlg.h" //because included externally
#include "ui-core/EditEx.h"
#include "ui-core/SpinButtonCtrlEx.h"
#include "ui-core/TabDialog.h"

class CToolTipCtrlEx;
class CWndScroller;

class AFX_EXT_CLASS CKnockPageDlg : public CParamTabBaseDlg, public ParamPageEvents
{
  typedef CParamTabBaseDlg Super;

 public:
  CKnockPageDlg(CWnd* pParent = NULL); // standard constructor
 ~CKnockPageDlg();
  virtual LPCTSTR GetDialogID(void) const;
  static const UINT IDD;

  void Enable(bool enable);
  bool IsEnabled(void);

  void GetValues(SECU3IO::KnockPar* o_values);
  void SetValues(const SECU3IO::KnockPar* i_values);

  void ForceOnChangeNotify(void);

 // Implementation
 protected:
  virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV support
  virtual BOOL OnInitDialog();
  afx_msg void OnChangeData();
  afx_msg void OnUpdateControls(CCmdUI* pCmdUI);
  afx_msg void OnDestroy();
  DECLARE_MESSAGE_MAP()

 private:
  SECU3IO::KnockPar m_params;
  bool m_enabled;

  std::auto_ptr<CWndScroller> mp_scr;
  std::auto_ptr<CToolTipCtrlEx> mp_ttc;

  CComboBox m_use_knock_channel_combo;
  CComboBox m_bpf_frequency_combo;
  CComboBox m_integrator_const_combo;//us

  CEditEx m_wnd_begin_angle_edit;//deg.
  CEditEx m_wnd_end_angle_edit;  //deg.
  CEditEx m_retard_step_edit;    //deg.
  CEditEx m_advance_step_edit;   //deg.
  CEditEx m_max_retard_edit;     //deg.
  CEditEx m_threshold_edit;      //voltage
  CEditEx m_recovery_delay_edit; //in work cycles

  CSpinButtonCtrlEx m_wnd_begin_angle_spin;
  CSpinButtonCtrlEx m_wnd_end_angle_spin;
  CSpinButtonCtrlEx m_retard_step_spin;
  CSpinButtonCtrlEx m_advance_step_spin;
  CSpinButtonCtrlEx m_max_retard_spin;
  CSpinButtonCtrlEx m_threshold_spin;
  CSpinButtonCtrlEx m_recovery_delay_spin;
};
