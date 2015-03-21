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

/** \file FunSetPageDlg.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include <vector>
#include "common/unicodesupport.h"
#include "io-core/SECU3IO.h"
#include "common/ParamPageEvents.h"
#include "common/ParamTabBaseDlg.h"
#include "ui-core/EditEx.h"
#include "ui-core/SpinButtonCtrlEx.h"
#include "ui-core/TabDialog.h"

class CToolTipCtrlEx;

class CFunSetPageDlg : public CParamTabBaseDlg, public ParamPageEvents
{
  typedef CParamTabBaseDlg Super;

 public:
  CFunSetPageDlg(CWnd* pParent = NULL); // standard constructor
  virtual LPCTSTR GetDialogID(void) const;
  static const UINT IDD;

  void Enable(bool enable);
  bool IsEnabled(void);

  void FillCBByFunNames(void);
  std::vector<_TSTRING>& AccessFunNames(void);

  void GetValues(SECU3IO::FunSetPar* o_values);
  void SetValues(const SECU3IO::FunSetPar* i_values);

 // Implementation
 protected:
  virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV support
  virtual BOOL OnInitDialog();
  afx_msg void OnChangeData();
  afx_msg void OnMapCalcButton();
  afx_msg void OnUpdateControls(CCmdUI* pCmdUI);
  DECLARE_MESSAGE_MAP()

 private:
  SECU3IO::FunSetPar m_params;
  std::vector<_TSTRING> m_fun_names;
  bool m_enabled;

  CComboBox m_gas_maps_combo;
  CComboBox m_benzin_maps_combo;
  CSpinButtonCtrlEx m_press_swing_spin;
  CEditEx m_press_swing_edit;
  CSpinButtonCtrlEx m_map_grad_spin;
  CEditEx m_map_grad_edit;
  CSpinButtonCtrlEx m_map_curve_offset_spin;
  CEditEx m_map_curve_offset_edit;
  CSpinButtonCtrlEx m_map_curve_gradient_spin;
  CEditEx m_map_curve_gradient_edit;
  CSpinButtonCtrlEx m_tps_curve_offset_spin;
  CEditEx m_tps_curve_offset_edit;
  CSpinButtonCtrlEx m_tps_curve_gradient_spin;
  CEditEx m_tps_curve_gradient_edit;
  CBitmapButton m_calc_map_btn;

  std::auto_ptr<CToolTipCtrlEx> mp_ttc;
};
