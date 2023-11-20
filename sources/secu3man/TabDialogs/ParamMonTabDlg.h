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

/** \file ParamMonTabDlg.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include <memory>
#include "common/FastDelegate.h"
#include "ui-core/TabDialog.h"

class CCEDeskDlg;
class CLabel;
class CMIDeskDlg;
class CParamDeskDlg;
class CRSDeskDlg;
class CTablesDeskDlg;
class CSepTablesDeskDlg;
class CToolTipCtrlEx;

// CParamMonTabDlg dialog
class CParamMonTabDlg : public CTabDialog
{
  typedef CTabDialog Super;
  typedef fastdelegate::FastDelegate0<> EventHandler;
 public:
  CParamMonTabDlg();
  virtual LPCTSTR GetDialogID(void) const;

  bool GetRawSensorsCheckState(void);
  bool GetEditTablesCheckState(void);
  bool GetEditTablesOffCheckState(void);

  void EnableEditTablesCheck(bool enable);
  void EnableEditTablesOffCheck(bool enable);
  void EnableMakingChartsChildren(bool enable);
  void EnableToggleMapWnd(bool toggle);

  void setOnRawSensorsCheck(EventHandler i_Function);
  void setOnEditTablesCheck(EventHandler i_Function);
  void setOnEditTablesOffCheck(EventHandler i_Function);

  std::auto_ptr<CCEDeskDlg> mp_CEDeskDlg;
  std::auto_ptr<CMIDeskDlg> mp_MIDeskDlg;
  std::auto_ptr<CRSDeskDlg> mp_RSDeskDlg;
  std::auto_ptr<CParamDeskDlg> mp_ParamDeskDlg;
  std::auto_ptr<CTablesDeskDlg> mp_TablesDeskDlg;
  std::auto_ptr<CSepTablesDeskDlg> mp_SeptabsDeskDlg;

  void SetSplitterPos(int pos);
  int GetSplitterPos(void) const;

 // Implementation
 protected:
  void _MoveSplitter(int x, int start_x);

  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  virtual BOOL OnInitDialog();
  afx_msg void OnDestroy();
  afx_msg void OnPmShowRawSensors();
  afx_msg void OnPmEditTables();
  afx_msg void OnPmEditTablesOff();
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnMouseMove(UINT nFlags, CPoint point);
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
  DECLARE_MESSAGE_MAP()

  virtual void OnShow(bool show);

 private:
  CButton m_raw_sensors_check;
  CButton m_edit_tables_check;
  CButton m_edit_tables_off_check;
  std::auto_ptr<CLabel> mp_secu3orgLink;

  EventHandler m_OnRawSensorsCheck;
  EventHandler m_OnEditTablesCheck;
  EventHandler m_OnEditTablesOffCheck;

  bool m_enMakeChartsChildren;
  bool m_initialized;
  int m_miMargin;
  
  bool m_moveSplitter;
  CPoint m_moveStart;
  int m_moveStrtWidthPD;
  CRect m_moveStrtRectMI;

  std::auto_ptr<CToolTipCtrlEx> mp_ttc;

  int m_splitterPos;
};
