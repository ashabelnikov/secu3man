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

/** \file FirmwareMasterDlg.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include <vector>
#include "common/FastDelegate.h"
#include "common/UnicodeSupport.h"

class CToolTipCtrlEx;

//Note! order in this enum must correspond to order in the related combobox
//See CFirmwareMasterDlg::FillUnitCombo()
enum UnitId
{
 UNIT_SECU3I = 0,
 UNIT_SECU3T,
 UNIT_SECU3TR6,
 UNIT_SECU3L,
 UNIT_SECU3M
};

//Note! order in this enum must correspond to order in the related combobox
//See CFirmwareMasterDlg::FillSyncCombo()
enum SyncId
{
 SYNC_TRWLCRAN = 0,
 SYNC_TRWLCAMS,
 SYNC_DISTHALL,
 SYNC_NP1CRAN
};

//constants for check boxes
enum FwmFlag
{
  FWM_DWELL = 0,
  FWM_PHIGN = 1,
  FWM_INJ = 2,
  FWM_TPIC = 3,
  FWM_FPWM = 4,
  FWM_2CHIG = 5,
  FWM_OBD = 6,
  FWM_CAFR = 7,
  FWM_NR_OF_FLAGS = 8
};

//view
class CFirmwareMasterDlg : public CDialog
{
  typedef CDialog Super;
  typedef fastdelegate::FastDelegate0<> EventHandler;
  typedef fastdelegate::FastDelegate1<int> EventWithCode;

 public:
  CFirmwareMasterDlg(CWnd* pParent = NULL);   // standard constructor
  static const UINT IDD;

  void setOnOkButton(EventHandler OnFunction) {m_OnOkButton = OnFunction;};
  void setOnCancelButton(EventHandler OnFunction) {m_OnCancelButton = OnFunction;};
  void setOnActivate(EventHandler OnActivate) {m_OnActivate = OnActivate;};
  void setOnChangeFwmCheck(EventWithCode OnChange) {m_OnChangeFwmCheck = OnChange;};
  void setOnChangeUnit(EventHandler OnChange) {m_OnChangeUnit = OnChange;};
  void setOnChangeSync(EventHandler OnChange) {m_OnChangeSync = OnChange;};

  void FillUnitCombo(void);
  void FillSyncCombo(void);

  void SetUnitCombo(int id);
  void SetSyncCombo(int id);

  int GetUnitCombo(void);
  int GetSyncCombo(void);

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
  afx_msg void OnChangeUnitCombo();
  afx_msg void OnChangeSyncCombo();
  DECLARE_MESSAGE_MAP()

 private:
  EventHandler m_OnOkButton;
  EventHandler m_OnCancelButton;
  EventHandler m_OnActivate;
  EventWithCode m_OnChangeFwmCheck;
  EventHandler m_OnChangeUnit;
  EventHandler m_OnChangeSync;

  CComboBox m_unit_combo;
  CComboBox m_sync_combo;
  CButton m_fwm_checks[FWM_NR_OF_FLAGS];
  std::auto_ptr<CToolTipCtrlEx> mp_ttc;

  int m_unit_idx;
  int m_sync_idx;
  bool m_fwm_flags[FWM_NR_OF_FLAGS];
};
