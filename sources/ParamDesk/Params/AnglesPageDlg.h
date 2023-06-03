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

/** \file AnglesPageDlg.h
 * \author Alexey A. Shabelnikov
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
class CWndScroller;

class CAnglesPageDlg : public CParamTabBaseDlg, public ParamPageEvents
{
  typedef CParamTabBaseDlg Super;

 public:
  CAnglesPageDlg();
  virtual LPCTSTR GetDialogID(void) const;

  void Enable(bool enable);
  bool IsEnabled(void);

  void GetValues(SECU3IO::AnglesPar* o_values);
  void SetValues(const SECU3IO::AnglesPar* i_values);

 // Implementation
 protected:
  virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV support
  virtual BOOL OnInitDialog();
  afx_msg void OnDestroy();
  afx_msg void OnChangeData();
  afx_msg void OnChangeDataZeroAA();
  afx_msg void OnChangeDataZeroAAOct();
  afx_msg void OnUpdateControls(CCmdUI* pCmdUI);
  afx_msg void OnSize(UINT nType, int cx, int cy);
  DECLARE_MESSAGE_MAP()

 private:
  std::auto_ptr<CWndScroller> mp_scr;
  std::auto_ptr<CToolTipCtrlEx> mp_ttc;

  SECU3IO::AnglesPar m_params;
  bool m_enabled;

  CSpinButtonCtrlEx m_min_angle_spin;
  CSpinButtonCtrlEx m_max_angle_spin;
  CSpinButtonCtrlEx m_correction_spin;
  CSpinButtonCtrlEx m_decrease_speed_spin;
  CSpinButtonCtrlEx m_increase_speed_spin;
  CEditEx m_min_angle_edit;
  CEditEx m_max_angle_edit;
  CEditEx m_correction_edit;
  CEditEx m_decrease_speed_edit;
  CEditEx m_increase_speed_edit;
  CButton m_zeroaa_check;
  CButton m_zeroaaoct_check;

  CEditEx m_shift_igntim_edit;
  CSpinButtonCtrlEx m_shift_igntim_spin;
  CButton m_always_wrkmap_check;
  CButton m_manigntim_idl_check;
};
