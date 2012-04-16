/* SECU-3  - An open source, free engine control unit
   Copyright (C) 2007 Alexey A. Shabelnikov. Ukraine, Gorlovka

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

#include "IRemapDeskView.h"
#include "ui-core/DialogWithAccelerators.h"

/////////////////////////////////////////////////////////////////////////////
// CIORemapping dialog

class AFX_EXT_CLASS CIORemappingDlg : public CModelessDialog, public IRemapDeskView
{
  typedef CModelessDialog Super;

 public:
  CIORemappingDlg(CWnd* pParent = NULL);   // standard constructor
  static const UINT IDD;

  void SetPosition(int x_pos, int y_pos, CWnd* wnd_insert_after = NULL);

  //----------interface implementation---------------------------
  virtual bool IsEnabled(void);
  virtual void Enable(bool enable);
  virtual void Show(bool show);

  virtual bool AddItem(FWDM::IOSid iosId, FWDM::IOPid iopId, const _TSTRING& i_text);
  virtual bool SelectItem(FWDM::IOSid iosId, FWDM::IOPid iopId);
  virtual FWDM::IOPid GetSelection(FWDM::IOSid iosId) const;
  virtual void setOnItemSelected(EventItemSel OnFunction);
  virtual void EnableSECU3TItems(bool i_enable);
  //-------------------------------------------------------------

  // Implementation
 protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  virtual BOOL OnInitDialog();
  afx_msg void OnUpdateControls(CCmdUI* pCmdUI);
  afx_msg void OnChangeSelection(UINT nID);
  DECLARE_MESSAGE_MAP()

 private:
  bool m_enabled;
  bool m_enable_secu3t_features;

  EventItemSel m_OnItemSel;
  CComboBox m_iorcb[FWDM::IOS_COUNT];
};

/////////////////////////////////////////////////////////////////////////////
