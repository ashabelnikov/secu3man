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

/** \file InjDrvStockDlg.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include <memory>
#include "common/FastDelegate.h"
#include "common/unicodesupport.h"
#include "ui-core/ListCtrlEx.h"

class CToolTipCtrlEx;

class AFX_EXT_CLASS CInjDrvStockDlg : public CDialog
{
  typedef CDialog Super;
  typedef fastdelegate::FastDelegate0<> EventHandler;

 public:
  CInjDrvStockDlg(CWnd* pParent = NULL);   // standard constructor
 ~CInjDrvStockDlg();
  static const UINT IDD;

  void InsertItem(const _TSTRING& value);
  int GetSelectedIndex(void);

 public: //set event handlers
  void setOnOkButton(EventHandler OnFunction) {m_OnOkButton = OnFunction;};
  void setOnCancelButton(EventHandler OnFunction) {m_OnCancelButton = OnFunction;};
  void setOnActivate(EventHandler OnActivate) {m_OnActivate = OnActivate;};

// Implementation
 protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  afx_msg LRESULT OnKickIdle(WPARAM, LPARAM);
  virtual BOOL OnInitDialog();
  afx_msg void OnDestroy();
  virtual void OnOK();
  virtual void OnCancel();
  afx_msg void OnUpdateControls(CCmdUI* pCmdUI);
  DECLARE_MESSAGE_MAP()

 private:
  CListCtrlEx m_presetsList;

 private:
  EventHandler m_OnOkButton;
  EventHandler m_OnCancelButton;
  EventHandler m_OnActivate;

  std::auto_ptr<CToolTipCtrlEx> mp_ttc;
};
