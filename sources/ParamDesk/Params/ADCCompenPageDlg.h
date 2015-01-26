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

#include <memory>
#include "io-core/SECU3IO.h"
#include "common/ParamPageEvents.h"
#include "common/ParamTabBaseDlg.h"
#include "ui-core/EditEx.h"
#include "ui-core/SpinButtonCtrlEx.h"
#include "ui-core/TabDialog.h"

class CToolTipCtrlEx;

class CADCCompenPageDlg : public CParamTabBaseDlg, public ParamPageEvents
{
  typedef CParamTabBaseDlg Super;

 public:
  CADCCompenPageDlg(CWnd* pParent = NULL); // standard constructor
 ~CADCCompenPageDlg();
  virtual LPCTSTR GetDialogID(void) const;
  static const UINT IDD;

  void Enable(bool enable);
  bool IsEnabled(void);

  void GetValues(SECU3IO::ADCCompenPar* o_values);
  void SetValues(const SECU3IO::ADCCompenPar* i_values);

  void EnableSECU3TItems(bool i_enable);

 // Implementation
 protected:
  virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV support
  virtual BOOL OnInitDialog();
  afx_msg void OnChangeEdit();
  afx_msg void OnUpdateControls(CCmdUI* pCmdUI);
  afx_msg void OnUpdateControlsSECU3T(CCmdUI* pCmdUI);
  DECLARE_MESSAGE_MAP()

 private:
  SECU3IO::ADCCompenPar m_params;
  bool m_enabled;
  bool m_enable_secu3t_features;

  CSpinButtonCtrlEx m_map_factor_spin;
  CSpinButtonCtrlEx m_map_correction_spin;
  CEditEx m_map_factor_edit;
  CEditEx m_map_correction_edit;

  CSpinButtonCtrlEx m_ubat_factor_spin;
  CSpinButtonCtrlEx m_ubat_correction_spin;
  CEditEx m_ubat_factor_edit;
  CEditEx m_ubat_correction_edit;

  CSpinButtonCtrlEx m_temp_factor_spin;
  CSpinButtonCtrlEx m_temp_correction_spin;
  CEditEx m_temp_factor_edit;
  CEditEx m_temp_correction_edit;

  CSpinButtonCtrlEx m_tps_factor_spin;
  CSpinButtonCtrlEx m_tps_correction_spin;
  CEditEx m_tps_factor_edit;
  CEditEx m_tps_correction_edit;

  CSpinButtonCtrlEx m_ai1_factor_spin;
  CSpinButtonCtrlEx m_ai1_correction_spin;
  CEditEx m_ai1_factor_edit;
  CEditEx m_ai1_correction_edit;

  CSpinButtonCtrlEx m_ai2_factor_spin;
  CSpinButtonCtrlEx m_ai2_correction_spin;
  CEditEx m_ai2_factor_edit;
  CEditEx m_ai2_correction_edit;
  
  std::auto_ptr<CToolTipCtrlEx> mp_ttc;
};
