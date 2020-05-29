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

/** \file RPMGridEditDlg.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include <vector>
#include "common/FastDelegate.h"
#include "common/UnicodeSupport.h"

class CEditEx;
class CToolTipCtrlEx;

class AFX_EXT_CLASS CRPMGridEditDlg : public CDialog
{
  typedef CDialog Super;
  typedef fastdelegate::FastDelegate1<int> EventHandler;
  typedef fastdelegate::FastDelegate3<int, size_t, float> EventOnChange;
  typedef fastdelegate::FastDelegate0<bool> EventResult;

 public:
  CRPMGridEditDlg(CWnd* pParent = NULL);   // standard constructor
 ~CRPMGridEditDlg();
  static const UINT IDD;

  void SetValues(int mode, const float* ip_values);
  void GetValues(int mode, float* op_values);
  float GetValue(int mode, size_t index);
  void setOnChange(EventOnChange OnFunction);
  void setOnLoadDefVal(EventHandler OnFunction);
  void setIsOkEnabled(EventResult OnFunction);
  void SetErrMessage(int mode, const _TSTRING& str);
  void SetItemError(int mode, size_t index, bool i_error);

  virtual INT_PTR DoModal();

 // Implementation
 protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  virtual void OnOK();
  virtual void OnCancel();
  virtual BOOL OnInitDialog();
  afx_msg LRESULT OnKickIdle(WPARAM, LPARAM);
  afx_msg void OnChangeEdit(UINT nID);
  afx_msg void OnChangeEdit1(UINT nID);
  afx_msg void OnLoadDefValBtn();
  afx_msg void OnLoadDefValBtn1();
  afx_msg void OnUpdateOkButton(CCmdUI* pCmdUI);
  afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd *pWnd, UINT nCtlColor);
  DECLARE_MESSAGE_MAP()

 private:
  std::auto_ptr<CToolTipCtrlEx> mp_ttc;
  CStatic m_errMsg[2];
  std::vector<CEditEx*> m_edits[2];
  std::vector<float> m_values[2];
  std::vector<bool> m_errflags[2];

  EventOnChange m_onChange;
  EventHandler m_onLoadDefVal;
  EventResult m_isOkEnabled;
  CBrush m_redBrush;
};
