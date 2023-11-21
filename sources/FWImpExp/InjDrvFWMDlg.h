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

/** \file InjDrvFWMDlg.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include <vector>
#include "common/FastDelegate.h"
#include "common/UnicodeSupport.h"

class CToolTipCtrlEx;
class CLabel;

//Note! order in this enum must correspond to order in the related combobox
//See CFirmwareMasterDlg::FillProcCombo()
enum ProcId
{
 PROC_328PB = 0,
 PROC_328,
};

//constants for check boxes
enum FwmFlag
{
 FWM_INPINV = 0,
 FWM_FLBINV = 1,
 FWM_PWMINV = 2,
 FWM_PWCORR = 3,
 FWM_NR_OF_FLAGS = 4
};

//view
class CInjDrvFWMDlg : public CDialog
{
  typedef CDialog Super;
  typedef fastdelegate::FastDelegate0<> EventHandler;
  typedef fastdelegate::FastDelegate1<int> EventWithCode;

 public:
  CInjDrvFWMDlg(CWnd* pParent = NULL);   // standard constructor
  static const UINT IDD;

  void setOnOkButton(EventHandler OnFunction) {m_OnOkButton = OnFunction;};
  void setOnCancelButton(EventHandler OnFunction) {m_OnCancelButton = OnFunction;};
  void setOnActivate(EventHandler OnActivate) {m_OnActivate = OnActivate;};
  void setOnChangeFwmCheck(EventWithCode OnChange) {m_OnChangeFwmCheck = OnChange;};
  void setOnSaveButton(EventHandler OnFunction) {m_OnSaveButton = OnFunction;};
  void setOnLoadButton(EventHandler OnFunction) {m_OnLoadButton = OnFunction;};
  void setOnFuseLink(EventHandler OnFunction) {m_OnFuseLink = OnFunction;};
  void setOnChecksLink(EventHandler OnFunction) {m_OnChecksLink = OnFunction;};

  void FillProcCombo(void);
  void SetProcCombo(int id);
  int GetProcCombo(void);

  void SetFwmFlag(FwmFlag i_flag_type, bool i_state);
  bool GetFwmFlag(FwmFlag i_flag_type);
  void EnableFwmFlag(FwmFlag i_flag_type, bool i_enable);

  virtual INT_PTR DoModal();

    // Implementation
 protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  virtual void OnOK();
  virtual void OnCancel();
  virtual BOOL OnInitDialog();
  afx_msg void OnDestroy();
  afx_msg LRESULT OnKickIdle(WPARAM, LPARAM);
  afx_msg void OnFwmCheck(UINT);
  afx_msg void OnSave();
  afx_msg void OnLoad();
  DECLARE_MESSAGE_MAP()

  void OnFuseLinkClick(void);
  void OnChecksLinkClick(void);

 private:
  EventHandler m_OnOkButton;
  EventHandler m_OnSaveButton;
  EventHandler m_OnLoadButton;
  EventHandler m_OnCancelButton;
  EventHandler m_OnActivate;
  EventWithCode m_OnChangeFwmCheck;
  EventHandler m_OnFuseLink;
  EventHandler m_OnChecksLink;

  CComboBox m_proc_combo;

  CButton m_fwm_checks[FWM_NR_OF_FLAGS];
  std::auto_ptr<CToolTipCtrlEx> mp_ttc;
  std::auto_ptr<CLabel> mp_FuseLink;
  std::auto_ptr<CLabel> mp_ChecksLink;

  bool m_fwm_flags[FWM_NR_OF_FLAGS];
  int m_proc_idx;
};
