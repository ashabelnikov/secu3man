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
#include <memory>
#include "common/FastDelegate.h"
#include "common/UnicodeSupport.h"
#include "ui-core/EditEx.h"
#include "ui-core/SpinButtonCtrlEx.h"
#include "ui-core/UpdatableDialog.h"

class CWndScroller;
class CToolTipCtrlEx;

struct CESettingsData;

class AFX_EXT_CLASS CCESettingsDlg : public CModelessUpdatableDialog
{
  typedef CModelessUpdatableDialog Super;

 public:
  CCESettingsDlg(CWnd* pParent = NULL);   // standard constructor
 ~CCESettingsDlg();
  static const UINT IDD;

  void SetValues(const CESettingsData& i_data);
  void GetValues(CESettingsData& o_data);
  void EnableSECU3TItems(bool i_enable);
  void EnableExtraIO(bool i_enable);
  void SetWndPosition(int x, int y);
  const CPoint& GetWndPosition(void);
  void SetPosition(int x_pos, int y_pos, CWnd* wnd_insert_after = NULL);

 // Implementation
 protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  virtual BOOL OnInitDialog();
  afx_msg void OnDestroy();
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnUpdateSECU3i(CCmdUI* pCmdUI);
  afx_msg void OnUpdateSECU3iEx(CCmdUI* pCmdUI);
  afx_msg void OnUpdateEmValue(CCmdUI* pCmdUI);
  afx_msg void OnEMCheck();
  DECLARE_MESSAGE_MAP()

 private:
  void _UpdateScrlViewSize(void);
  CESettingsData* mp_data;
  std::auto_ptr<CWndScroller> mp_scr;
  bool m_enable_secu3t_features;
  bool m_enable_extraio;

  CEditEx m_map_v_min_edit;
  CSpinButtonCtrlEx m_map_v_min_spin;
  CEditEx m_map_v_max_edit;
  CSpinButtonCtrlEx m_map_v_max_spin;
  CEditEx m_map_v_em_edit;
  CSpinButtonCtrlEx m_map_v_em_spin;
  CButton m_map_v_useem_check;

  CEditEx m_vbat_v_min_edit;
  CSpinButtonCtrlEx m_vbat_v_min_spin;
  CEditEx m_vbat_v_max_edit;
  CSpinButtonCtrlEx m_vbat_v_max_spin;
  CEditEx m_vbat_v_em_edit;
  CSpinButtonCtrlEx m_vbat_v_em_spin;
  CButton m_vbat_v_useem_check;

  CEditEx m_cts_v_min_edit;
  CSpinButtonCtrlEx m_cts_v_min_spin;
  CEditEx m_cts_v_max_edit;
  CSpinButtonCtrlEx m_cts_v_max_spin;
  CEditEx m_cts_v_em_edit;
  CSpinButtonCtrlEx m_cts_v_em_spin;
  CButton m_cts_v_useem_check;

  CEditEx m_ks_v_min_edit;
  CSpinButtonCtrlEx m_ks_v_min_spin;
  CEditEx m_ks_v_max_edit;
  CSpinButtonCtrlEx m_ks_v_max_spin;
  CEditEx m_ks_v_em_edit;
  CSpinButtonCtrlEx m_ks_v_em_spin;
  CButton m_ks_v_useem_check;

  CEditEx m_tps_v_min_edit;
  CSpinButtonCtrlEx m_tps_v_min_spin;
  CEditEx m_tps_v_max_edit;
  CSpinButtonCtrlEx m_tps_v_max_spin;
  CEditEx m_tps_v_em_edit;
  CSpinButtonCtrlEx m_tps_v_em_spin;
  CButton m_tps_v_useem_check;

  CEditEx m_add_i1_v_min_edit;
  CSpinButtonCtrlEx m_add_i1_v_min_spin;
  CEditEx m_add_i1_v_max_edit;
  CSpinButtonCtrlEx m_add_i1_v_max_spin;
  CEditEx m_add_i1_v_em_edit;
  CSpinButtonCtrlEx m_add_i1_v_em_spin;
  CButton m_add_i1_v_useem_check;

  CEditEx m_add_i2_v_min_edit;
  CSpinButtonCtrlEx m_add_i2_v_min_spin;
  CEditEx m_add_i2_v_max_edit;
  CSpinButtonCtrlEx m_add_i2_v_max_spin;
  CEditEx m_add_i2_v_em_edit;
  CSpinButtonCtrlEx m_add_i2_v_em_spin;
  CButton m_add_i2_v_useem_check;

  CEditEx m_add_i3_v_min_edit;
  CSpinButtonCtrlEx m_add_i3_v_min_spin;
  CEditEx m_add_i3_v_max_edit;
  CSpinButtonCtrlEx m_add_i3_v_max_spin;
  CEditEx m_add_i3_v_em_edit;
  CSpinButtonCtrlEx m_add_i3_v_em_spin;
  CButton m_add_i3_v_useem_check;

  CEditEx m_add_i4_v_min_edit;
  CSpinButtonCtrlEx m_add_i4_v_min_spin;
  CEditEx m_add_i4_v_max_edit;
  CSpinButtonCtrlEx m_add_i4_v_max_spin;
  CEditEx m_add_i4_v_em_edit;
  CSpinButtonCtrlEx m_add_i4_v_em_spin;
  CButton m_add_i4_v_useem_check;

  CEditEx m_add_i5_v_min_edit;
  CSpinButtonCtrlEx m_add_i5_v_min_spin;
  CEditEx m_add_i5_v_max_edit;
  CSpinButtonCtrlEx m_add_i5_v_max_spin;
  CEditEx m_add_i5_v_em_edit;
  CSpinButtonCtrlEx m_add_i5_v_em_spin;
  CButton m_add_i5_v_useem_check;

  CEditEx m_add_i6_v_min_edit;
  CSpinButtonCtrlEx m_add_i6_v_min_spin;
  CEditEx m_add_i6_v_max_edit;
  CSpinButtonCtrlEx m_add_i6_v_max_spin;
  CEditEx m_add_i6_v_em_edit;
  CSpinButtonCtrlEx m_add_i6_v_em_spin;
  CButton m_add_i6_v_useem_check;

  CEditEx m_add_i7_v_min_edit;
  CSpinButtonCtrlEx m_add_i7_v_min_spin;
  CEditEx m_add_i7_v_max_edit;
  CSpinButtonCtrlEx m_add_i7_v_max_spin;
  CEditEx m_add_i7_v_em_edit;
  CSpinButtonCtrlEx m_add_i7_v_em_spin;
  CButton m_add_i7_v_useem_check;

  CEditEx m_add_i8_v_min_edit;
  CSpinButtonCtrlEx m_add_i8_v_min_spin;
  CEditEx m_add_i8_v_max_edit;
  CSpinButtonCtrlEx m_add_i8_v_max_spin;
  CEditEx m_add_i8_v_em_edit;
  CSpinButtonCtrlEx m_add_i8_v_em_spin;
  CButton m_add_i8_v_useem_check;

  CEditEx m_oilpress_thrd_edit;
  CSpinButtonCtrlEx m_oilpress_thrd_spin;
  CEditEx m_oilpress_timer_edit;
  CSpinButtonCtrlEx m_oilpress_timer_spin;

  std::auto_ptr<CToolTipCtrlEx> mp_ttc;
};



class AFX_EXT_CLASS CCESettingsCntr : public CDialog
{
  typedef CDialog Super;

 public:
  CCESettingsCntr(CWnd* pParent = NULL);   // standard constructor
 ~CCESettingsCntr();
  static const UINT IDD;

  void SetValues(const CESettingsData& i_data);
  void GetValues(CESettingsData& o_data);
  void EnableSECU3TItems(bool i_enable);
  void EnableExtraIO(bool i_enable);
  void SetWndPosition(int x, int y);
  const CPoint& GetWndPosition(void);
  void SetWndSize(int cx, int cy);
  const CSize& GetWndSize(void);

  virtual INT_PTR DoModal();

 // Implementation
 protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  virtual void OnOK();
  virtual void OnCancel();
  virtual BOOL OnInitDialog();
  afx_msg void OnDestroy();
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
  afx_msg LRESULT OnKickIdle(WPARAM, LPARAM);
  afx_msg void OnUpdateOkButton(CCmdUI* pCmdUI);
  DECLARE_MESSAGE_MAP()

 private:
  void _UpdateControlsPosition(int cx, int cy);
  CCESettingsDlg m_dlg;
  CPoint m_wndPos;
  CSize m_wndSize;
  CSize m_createSize;
  bool m_initialized;
  CButton m_ok_btn;
  CButton m_cancel_btn;
};
