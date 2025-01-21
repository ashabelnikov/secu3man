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

/** \file ScaleEditorDlg.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include "stdafx.h"
#include <memory>
#include "common/UnicodeSupport.h"
#include "common/SettingsTypes.h"
#include "ui-core/EditEx.h"
#include "ui-core/SpinButtonCtrlEx.h"
#include "ui-core/ListCtrlEx.h"
#include "ui-core/MultiLEDCtrl.h"

class CToolTipCtrlEx;

class ScaleEditorDlg : public CDialog
{
  typedef CDialog Super;
 public:
  ScaleEditorDlg(CWnd* pParent = NULL);
 ~ScaleEditorDlg();
  static const UINT IDD;

  void SetData(const MetCfg& cfg, bool meter);
  void GetData(MetCfg& cfg) const;
  virtual INT_PTR DoModal();

 // Implementation
 protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  virtual void OnOK();
  virtual void OnCancel();
  virtual BOOL OnInitDialog();
  afx_msg LRESULT OnKickIdle(WPARAM, LPARAM);
  afx_msg void OnUpdateMetCtrls(CCmdUI* pCmdUI);
  afx_msg void OnUpdateAZDel(CCmdUI* pCmdUI);
  afx_msg void OnCustomdrawList(NMHDR*, LRESULT*);
  afx_msg void OnChangeAZList(NMHDR* pNMHDR, LRESULT* pResult);
  afx_msg void OnAZAdd();
  afx_msg void OnAZDelete();
  afx_msg void OnUpdateAZEdit(CCmdUI* pCmdUI);
  afx_msg void OnChangeData();
  afx_msg void OnChangeDataSc();
  afx_msg void OnLButtonDown(UINT, CPoint);
  DECLARE_MESSAGE_MAP()

  void AppendAZList(int index, const _TSTRING& i_start, const _TSTRING& i_end, COLORREF color);
  _TSTRING FormatValue(float val);
  _TSTRING FormatColor(COLORREF val);

 private:
  CFont m_font;
  CButton m_az_add;
  CButton m_az_del;
  CSpinButtonCtrlEx m_scale_begin_spin;
  CEditEx m_scale_begin_edit;
  CSpinButtonCtrlEx m_scale_end_spin;
  CEditEx m_scale_end_edit;
  CSpinButtonCtrlEx m_scale_tics_spin;
  CEditEx m_scale_tics_edit;
  CListCtrlEx m_az_list;
  CMultiLEDCtrl m_az_color;
  CSpinButtonCtrlEx m_az_begin_spin;
  CEditEx m_az_begin_edit;
  CSpinButtonCtrlEx m_az_end_spin;
  CEditEx m_az_end_edit;

  std::auto_ptr<CToolTipCtrlEx> mp_ttc;
  MetCfg m_cfg;
  bool m_meter;
};
