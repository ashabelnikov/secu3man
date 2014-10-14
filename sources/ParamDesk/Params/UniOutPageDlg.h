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

#include <map>
#include <memory>
#include <string>
#include "io-core/SECU3IO.h"
#include "common/ParamPageEvents.h"
#include "ui-core/EditEx.h"
#include "ui-core/SpinButtonCtrlEx.h"
#include "ui-core/TabDialog.h"

class CToolTipCtrlEx;
class CWndScroller;

class CUniOutPageDlg : public CTabDialog, public ParamPageEvents
{
  typedef CTabDialog Super;

 public:
  CUniOutPageDlg(CWnd* pParent = NULL);   // standard constructor
  virtual LPCTSTR GetDialogID(void) const;
  static const UINT IDD;

  void Enable(bool enable);
  bool IsEnabled(void);

  void GetValues(SECU3IO::UniOutPar* o_values);
  void SetValues(const SECU3IO::UniOutPar* i_values);

 // Implementation
 protected:
  virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV support
  virtual BOOL OnInitDialog();
  afx_msg void OnDestroy();
  afx_msg void OnUpdateControls(CCmdUI* pCmdUI);
  afx_msg void OnUpdateControlsOut1Con2(CCmdUI* pCmdUI);
  afx_msg void OnUpdateControlsOut2Con2(CCmdUI* pCmdUI);
  afx_msg void OnUpdateControlsOut3Con2(CCmdUI* pCmdUI);
  afx_msg void OnChangeInvFlags(void);
  afx_msg void OnChangeDataOut1Con1(void);
  afx_msg void OnChangeDataOut1Con2(void);
  afx_msg void OnChangeDataOut2Con1(void);
  afx_msg void OnChangeDataOut2Con2(void);
  afx_msg void OnChangeDataOut3Con1(void);
  afx_msg void OnChangeDataOut3Con2(void);
  afx_msg void OnChangeDataLFCombo(void);
  afx_msg void OnChangeDataLF12Combo(void);
  afx_msg void OnChangeData(void);
  DECLARE_MESSAGE_MAP()

 private:
  void _FillConditionComboBoxes(void);
  void _FillLogicFuncComboBoxes(void);
  void _FillLogicFunc12ComboBox(void);
  void _SetCondInputFormat(int outIndex, bool cond, bool setdefval = true);
  void _SetCondComboBoxesSel(void);
  void _GetCondComboBoxesSel(void);
  void _SetLogicFuncComboBoxesSel(void);
  void _GetLogicFuncComboBoxesSel(void);
  void _SetLogicFunc12ComboBoxSel(void);
  void _GetLogicFunc12ComboBoxSel(void);

  SECU3IO::UniOutPar m_params;
  bool m_enabled;

  struct Out
  {
   CComboBox cond1_combo;
   CComboBox cond2_combo;
   CComboBox lf_combo;
   CButton inv1_check;
   CButton inv2_check;

   CSpinButtonCtrlEx on_thrd_1_spin;
   CEditEx on_thrd_1_edit;
   CSpinButtonCtrlEx off_thrd_1_spin;
   CEditEx off_thrd_1_edit;

   CSpinButtonCtrlEx on_thrd_2_spin;
   CEditEx on_thrd_2_edit;
   CSpinButtonCtrlEx off_thrd_2_spin;
   CEditEx off_thrd_2_edit;
  }m_out[SECU3IO::UNI_OUTPUT_NUM];

  CComboBox lf12_combo;

  struct CondFmt
  {
   CondFmt(int i_textlim, int i_decplaces, float i_min, float i_max, float i_delta, float i_defval_on, float i_defval_off, _TSTRING i_condstr, bool i_sign)
   : textlim(i_textlim), decplaces(i_decplaces), minimum(i_min), maximum(i_max), delta(i_delta), defval_on(i_defval_on)
   , defval_off(i_defval_off), condstr(i_condstr), sign(i_sign) {}
   int textlim, decplaces;
   float minimum, maximum, delta;
   float defval_on, defval_off;
   _TSTRING condstr;
   bool sign;
  };

  std::map<int, CondFmt> m_condFmt;
  std::map<int, _TSTRING> m_lf_str;

  std::auto_ptr<CWndScroller> mp_scr;
  std::auto_ptr<CToolTipCtrlEx> mp_ttc;
};
