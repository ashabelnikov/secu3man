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

#include <memory>
#include <string>
#include "common/fastdelegate.h"
#include "common/ParamPageEvents.h"
#include "ui-core/TabDialog.h"

class CButtonsPanel;

class CTablesPageDlg : public CTabDialog, public ParamPageEvents
{
  typedef CTabDialog Super;
  typedef fastdelegate::FastDelegate0<> EventHandler;

 public:
  CTablesPageDlg(CWnd* pParent = NULL);            // standard constructor
  virtual LPCTSTR GetDialogID(void) const;
  static const UINT IDD;

  void Enable(bool enable);
  bool IsEnabled(void);
  void SetReadOnlyTablesSetName(bool readonly);

  void SetTablesSetName(const _TSTRING& name);
  _TSTRING GetTablesSetName(void) const;

  void SetModificationFlag(bool value);

  void setOnChangeTablesSetName(EventHandler OnFunction);

  //CButtonsPanel
  std::auto_ptr<CButtonsPanel> mp_ButtonsPanel;

 // Implementation
 protected:
  virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV support
  virtual BOOL OnInitDialog();
  afx_msg void OnUpdateControls(CCmdUI* pCmdUI);
  afx_msg void OnChangeTablesSetName();
  DECLARE_MESSAGE_MAP()

 private:
  //from CButtonsPanel
  bool IsAllowed(void);

 private:
  EventHandler m_OnChangeTablesSetName;

  bool m_enabled;
  bool m_tsneb_readonly;
  CEdit m_names_edit;
  CStatic m_midification_flag;
};
