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

/** \file CESettingsDlg.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include <vector>
#include "common/FastDelegate.h"
#include "common/UnicodeSupport.h"
#include "ui-core/EditEx.h"
#include "ui-core/SpinButtonCtrlEx.h"

namespace SECU3IO { struct CESettingsData; }

class AFX_EXT_CLASS CCESettingsDlg : public CDialog
{
  typedef CDialog Super;

 public:
  CCESettingsDlg(CWnd* pParent = NULL);   // standard constructor
 ~CCESettingsDlg();
  static const UINT IDD;

  void SetValues(const SECU3IO::CESettingsData& i_data);
  void GetValues(SECU3IO::CESettingsData& o_data);

 // Implementation
 protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  virtual void OnOK();
  virtual void OnCancel();
  virtual BOOL OnInitDialog();
  afx_msg LRESULT OnKickIdle(WPARAM, LPARAM);
  afx_msg void OnUpdateOkButton(CCmdUI* pCmdUI);
  DECLARE_MESSAGE_MAP()

 private:
  SECU3IO::CESettingsData* mp_data;

  CEditEx m_map_v_min_edit;
  CSpinButtonCtrlEx m_map_v_min_spin;
  CEditEx m_map_v_max_edit;
  CSpinButtonCtrlEx m_map_v_max_spin;
  CEditEx m_map_v_em_edit;
  CSpinButtonCtrlEx m_map_v_em_spin;

  CEditEx m_vbat_v_min_edit;
  CSpinButtonCtrlEx m_vbat_v_min_spin;
  CEditEx m_vbat_v_max_edit;
  CSpinButtonCtrlEx m_vbat_v_max_spin;
  CEditEx m_vbat_v_em_edit;
  CSpinButtonCtrlEx m_vbat_v_em_spin;

  CEditEx m_cts_v_min_edit;
  CSpinButtonCtrlEx m_cts_v_min_spin;
  CEditEx m_cts_v_max_edit;
  CSpinButtonCtrlEx m_cts_v_max_spin;
  CEditEx m_cts_v_em_edit;
  CSpinButtonCtrlEx m_cts_v_em_spin;

  CEditEx m_ks_v_min_edit;
  CSpinButtonCtrlEx m_ks_v_min_spin;
  CEditEx m_ks_v_max_edit;
  CSpinButtonCtrlEx m_ks_v_max_spin;
  CEditEx m_ks_v_em_edit;
  CSpinButtonCtrlEx m_ks_v_em_spin;

  CEditEx m_tps_v_min_edit;
  CSpinButtonCtrlEx m_tps_v_min_spin;
  CEditEx m_tps_v_max_edit;
  CSpinButtonCtrlEx m_tps_v_max_spin;
  CEditEx m_tps_v_em_edit;
  CSpinButtonCtrlEx m_tps_v_em_spin;

  CEditEx m_add_i1_v_min_edit;
  CSpinButtonCtrlEx m_add_i1_v_min_spin;
  CEditEx m_add_i1_v_max_edit;
  CSpinButtonCtrlEx m_add_i1_v_max_spin;
  CEditEx m_add_i1_v_em_edit;
  CSpinButtonCtrlEx m_add_i1_v_em_spin;

  CEditEx m_add_i2_v_min_edit;
  CSpinButtonCtrlEx m_add_i2_v_min_spin;
  CEditEx m_add_i2_v_max_edit;
  CSpinButtonCtrlEx m_add_i2_v_max_spin;
  CEditEx m_add_i2_v_em_edit;
  CSpinButtonCtrlEx m_add_i2_v_em_spin;
};
