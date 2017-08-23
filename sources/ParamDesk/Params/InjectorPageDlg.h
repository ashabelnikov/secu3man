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

  void GetValues(SECU3IO::InjctrPar* o_values);
  void SetValues(const SECU3IO::InjctrPar* i_values);

 // Implementation
 protected:
  virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV support
  virtual BOOL OnInitDialog();
  afx_msg void OnDestroy();
  afx_msg void OnChangeData();
  afx_msg void OnChangeDataInjCfg();
  afx_msg void OnInjUseTimingMap();
  afx_msg void OnUpdateControls(CCmdUI* pCmdUI);
  afx_msg void OnUpdateInjTiming(CCmdUI* pCmdUI);
  DECLARE_MESSAGE_MAP()

 private:
  void _FillInjCfgComboBox(void);
  int  _GetInjCfgComboBoxSelection(void);
  void _SetInjCfgComboBoxSelection(int i_sel);
  std::vector<std::pair<int, _TSTRING> > m_injcfgs;

  void _FillSqrNumComboBox(void);
  int  _GetSqrNumComboBoxSelection(void);
  void _SetSqrNumComboBoxSelection(int i_sel);
  std::vector<std::pair<int, _TSTRING> > m_sqrnum;

  void _FillAngleSpecsComboBox(void);
  int  _GetAngleSpecsComboBoxSelection(void);
  void _SetAngleSpecsComboBoxSelection(int i_sel);
  std::vector<std::pair<int, _TSTRING> > m_anglespecs;

 private:
  SECU3IO::InjctrPar m_params;
  bool m_enabled;

  CSpinButtonCtrlEx m_cyldisp_spin;
  CEditEx m_cyldisp_edit;
  CSpinButtonCtrlEx m_flowrate_spin;
  CEditEx m_flowrate_edit;
  CComboBox m_injcfg_combo;
  CComboBox m_sqrnum_combo;
  CComboBox m_injas_combo;
  CSpinButtonCtrlEx m_inj_timing_spin;
  CEditEx m_inj_timing_edit;
  CSpinButtonCtrlEx m_inj_timing_crk_spin;
  CEditEx m_inj_timing_crk_edit;
  CButton m_inj_usetimingmap_check;

  float m_fuel_density; //fuel density (g/cc)

  std::auto_ptr<CToolTipCtrlEx> mp_ttc;
  std::auto_ptr<CWndScroller> mp_scr;
  bool m_ovf_msgbox;
};
