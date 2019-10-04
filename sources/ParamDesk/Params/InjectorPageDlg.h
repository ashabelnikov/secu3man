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

/** \file InjectorPageDlg.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include <memory>
#include <vector>
#include "io-core/SECU3IO.h"
#include "common/ParamPageEvents.h"
#include "common/ParamTabBaseDlg.h"
#include "ui-core/EditEx.h"
#include "ui-core/SpinButtonCtrlEx.h"
#include "ui-core/TabDialog.h"

class CToolTipCtrlEx;
class CWndScroller;

class CInjectorPageDlg : public CParamTabBaseDlg, public ParamPageEvents
{
  typedef CParamTabBaseDlg Super;

 public:
  CInjectorPageDlg(CWnd* pParent = NULL);   // standard constructor
 ~CInjectorPageDlg();
  virtual LPCTSTR GetDialogID(void) const;
  static const UINT IDD;

  void Enable(bool enable);
  bool IsEnabled(void);
  void EnableSECU3TItems(bool i_enable);

  void GetValues(SECU3IO::InjctrPar* o_values);
  void SetValues(const SECU3IO::InjctrPar* i_values);

  void SetITEdMode(int mode);

 // Implementation
 protected:
  virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV support
  virtual BOOL OnInitDialog();
  afx_msg void OnDestroy();
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnChangeData();
  afx_msg void OnChangeDataInjCfg();
  afx_msg void OnChangeDataInjCfg_g();
  afx_msg void OnInjUseTimingMap();
  afx_msg void OnUpdateControls(CCmdUI* pCmdUI);
  afx_msg void OnUpdateInjTiming(CCmdUI* pCmdUI);
  afx_msg void OnUpdateInjTiming_g(CCmdUI* pCmdUI);
  afx_msg void OnUpdateControlsSECU3i(CCmdUI* pCmdUI);
  DECLARE_MESSAGE_MAP()

 private:
  void _FillInjCfgComboBox(int fi);
  int  _GetInjCfgComboBoxSelection(int fi);
  void _SetInjCfgComboBoxSelection(int i_sel, int fi);
  std::vector<std::pair<int, _TSTRING> > m_injcfgs;

  void _FillSqrNumComboBox(int fi);
  int  _GetSqrNumComboBoxSelection(int fi);
  void _SetSqrNumComboBoxSelection(int i_sel, int fi);
  std::vector<std::pair<int, _TSTRING> > m_sqrnum[2];

  void _FillAngleSpecsComboBox(int fi);
  int  _GetAngleSpecsComboBoxSelection(int fi);
  void _SetAngleSpecsComboBoxSelection(int i_sel, int fi);
  std::vector<std::pair<int, _TSTRING> > m_anglespecs;

  void _GetITModeRange(float& y1, float& y2);

 private:
  SECU3IO::InjctrPar m_params;
  bool m_enabled;
  bool m_enable_secu3t_features;

  CSpinButtonCtrlEx m_cyldisp_spin;
  CEditEx m_cyldisp_edit;

  CSpinButtonCtrlEx m_flowrate_spin[2];
  CEditEx m_flowrate_edit[2];
  CComboBox m_injcfg_combo[2];
  CComboBox m_sqrnum_combo[2];
  CComboBox m_injas_combo[2];
  CSpinButtonCtrlEx m_inj_timing_spin[2];
  CEditEx m_inj_timing_edit[2];
  CSpinButtonCtrlEx m_inj_timing_crk_spin[2];
  CEditEx m_inj_timing_crk_edit[2];
  CButton m_inj_usetimingmap_check[2];
  CSpinButtonCtrlEx m_fff_const_spin;
  CEditEx m_fff_const_edit;
  CButton m_inj_useaddcorrs_check;
  CButton m_inj_useairden_check;
  CButton m_inj_usediffpress_check;

  CSpinButtonCtrlEx m_min_pw_spin[2];
  CEditEx m_min_pw_edit[2];

  float m_fuel_density[2];  //fuel density (g/cc)

  std::auto_ptr<CToolTipCtrlEx> mp_ttc;
  std::auto_ptr<CWndScroller> mp_scr;
  bool m_ovf_msgbox;

  CFont m_boldDlgFont;
  int m_itmode;
};
