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

/** \file AccelEnrPageDlg.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include <string>
#include <memory>
#include "io-core/SECU3IO.h"
#include "common/ParamPageEvents.h"
#include "common/ParamTabBaseDlg.h"
#include "ui-core/EditEx.h"
#include "ui-core/SpinButtonCtrlEx.h"
#include "ui-core/TabDialog.h"

class CToolTipCtrlEx;
class CWndScroller;

class CAccelEnrPageDlg : public CParamTabBaseDlg, public ParamPageEvents
{
  typedef CParamTabBaseDlg Super;

 public:
  CAccelEnrPageDlg(CWnd* pParent = NULL);   // standard constructor
  virtual LPCTSTR GetDialogID(void) const;
  static const UINT IDD;

  void Enable(bool enable);
  bool IsEnabled(void);
  void EnableFuelInjection(bool i_enable);

  void GetValues(SECU3IO::AccelPar* o_values);
  void SetValues(const SECU3IO::AccelPar* i_values);

 // Implementation
 protected:
  virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV support
  virtual BOOL OnInitDialog();
  afx_msg void OnChangeData();
  afx_msg void OnChangeDataAEType();
  afx_msg void OnUpdateControls(CCmdUI* pCmdUI);
  afx_msg void OnUpdateControlsAET(CCmdUI* pCmdUI);
  afx_msg void OnUpdateControlsXtau(CCmdUI* pCmdUI);
  afx_msg void OnUpdateFuelInjectionControls(CCmdUI* pCmdUI);
  afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
  afx_msg void OnDestroy();
  afx_msg void OnSize(UINT nType, int cx, int cy);
  DECLARE_MESSAGE_MAP()

 private:
  void _UpdateEnrBlendValue(void);

 private:
  SECU3IO::AccelPar m_params;
  bool m_enabled;
  bool m_fuel_injection;

  CSpinButtonCtrlEx m_tpsdot_thrd_spin;
  CEditEx m_tpsdot_thrd_edit;
  CSpinButtonCtrlEx m_coldacc_mult_spin;
  CEditEx m_coldacc_mult_edit;
  CSpinButtonCtrlEx m_decaytime_spin;
  CEditEx m_decaytime_edit;
  CSpinButtonCtrlEx m_aetime_spin;
  CEditEx m_aetime_edit;
  CSliderCtrl m_enrblend_slider;
  CSpinButtonCtrlEx m_mapdot_thrd_spin;
  CEditEx m_mapdot_thrd_edit;
  CStatic m_enrblend_value;
  CComboBox m_aetype_combo;

  CComboBox m_wwmtype_combo;
  CSpinButtonCtrlEx m_xtau_s_thrd_spin;
  CEditEx m_xtau_s_thrd_edit;
  CSpinButtonCtrlEx m_xtau_f_thrd_spin;
  CEditEx m_xtau_f_thrd_edit;

  std::auto_ptr<CToolTipCtrlEx> mp_ttc;
  std::auto_ptr<CWndScroller> mp_scr;
};
