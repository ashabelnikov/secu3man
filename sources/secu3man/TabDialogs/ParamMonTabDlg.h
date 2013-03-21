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
#include "common/FastDelegate.h"
#include "ui-core/TabDialog.h"

class CCEDeskDlg;
class CMIDeskDlg;
class CParamDeskDlg;
class CRSDeskDlg;
class CTablesDeskDlg;
class CToolTipCtrlEx;

// CParamMonTabDlg dialog
class CParamMonTabDlg : public CTabDialog
{
  typedef CTabDialog Super;
  typedef fastdelegate::FastDelegate0<> EventHandler;
 public:
  CParamMonTabDlg(CWnd* pParent = NULL);   // standard constructor
  static const UINT IDD;
  virtual LPCTSTR GetDialogID(void) const;

  bool GetRawSensorsCheckState(void);
  bool GetEditTablesCheckState(void);

  void EnableEditTablesCheck(bool enable);

  void MakePDFloating(bool i_floating);
  void EnlargeMonitor(bool i_enlarge);

  void setOnRawSensorsCheck(EventHandler i_Function);
  void setOnEditTablesCheck(EventHandler i_Function);

  std::auto_ptr<CCEDeskDlg> mp_CEDeskDlg;
  std::auto_ptr<CMIDeskDlg> mp_MIDeskDlg;
  std::auto_ptr<CRSDeskDlg> mp_RSDeskDlg;
  std::auto_ptr<CParamDeskDlg> mp_ParamDeskDlg;
  std::auto_ptr<CTablesDeskDlg> mp_TablesDeskDlg;

 // Implementation
 protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  virtual BOOL OnInitDialog();
  afx_msg void OnPmShowRawSensors();
  afx_msg void OnPmEditTables();
  DECLARE_MESSAGE_MAP()

 private:
  void _ResizeRect(const CRect& i_external, CRect& io_victim);

  CButton m_raw_sensors_check;
  CButton m_edit_tables_check;
  CStatic m_save_note_text;

  EventHandler m_OnRawSensorsCheck;
  EventHandler m_OnEditTablesCheck;

  CRect m_original_mi_rect;
  CRect m_original_rs_rect;
  CRect m_original_ce_rect;
  CPoint m_original_check_pos;
  CPoint m_original_button_pos;
  bool floating;

  std::auto_ptr<CToolTipCtrlEx> mp_ttc;
};
