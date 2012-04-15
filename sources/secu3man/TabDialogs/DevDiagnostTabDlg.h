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
#include "io-core/SECU3IO.h"
#include "ui-core/TabDialog.h"

class CDiagnostContextMenuManager;
class COScopeCtrl;

class CDevDiagnostTabDlg : public CTabDialog
{
  typedef CTabDialog Super;
  typedef fastdelegate::FastDelegate0<> EventHandler;
  typedef fastdelegate::FastDelegate2<int, bool> EventOutputToggle;

 public:
  CDevDiagnostTabDlg(CWnd* pParent = NULL);
  static const UINT IDD;
  virtual LPCTSTR GetDialogID(void) const;

  //установка обработчиков событий от элементов управления
  void setOnOutputToggle(EventOutputToggle OnFunction);
  void setOnEnterButton(EventHandler OnFunction);
  void setOnStartOutAutoTesting(EventHandler OnFunction);
  void setOnStopOutAutoTesting(EventHandler OnFunction);

  //Set values of inputs (digital and analog)
  void SetInputValues(const SECU3IO::DiagInpDat* i_values);
  void SetEnterButtonCaption(const _TSTRING& i_text);
  void SetEnterButton(bool i_state);
  void SetOutputValue(int id, bool state);

  void EnableDiagControls(bool i_enable);
  void EnableEnterButton(bool i_enable);
  void EnableSECU3TFeatures(bool i_enable);

  //IDs for outputs
  enum 
  {
   OID_IGN_OUT1 = 0,
   OID_IGN_OUT2,
   OID_IGN_OUT3,
   OID_IGN_OUT4,
   OID_IE,
   OID_FE,
   OID_ECF,
   OID_CE,
   OID_ST_BLOCK,
   OID_ADD_IO1,
   OID_ADD_IO2,
  };

 protected:
  virtual BOOL OnInitDialog();
  virtual void DoDataExchange(CDataExchange* pDX);
  afx_msg void OnUpdateDiagControls(CCmdUI* pCmdUI);
  afx_msg void OnUpdateEnterButton(CCmdUI* pCmdUI);
  afx_msg void OnOutputCheckToggle(UINT nID);
  afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
  afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
  afx_msg void OnEnterButton();
  afx_msg void OnTimer(UINT nIDEvent);
  afx_msg void OnDestroy();
  afx_msg void OnStartOutputsAutoTesting();
  afx_msg void OnStopOutputsAutoTesting();
  DECLARE_MESSAGE_MAP()

 private:
  void _InitializeOscilloscopeControls(void);

  std::auto_ptr<COScopeCtrl> mp_OScopeCtrl1;
  std::auto_ptr<COScopeCtrl> mp_OScopeCtrl2;
  std::auto_ptr<CDiagnostContextMenuManager> mp_ContextMenuManager;

  CButton m_enter_button;
  EventOutputToggle m_on_output_check;
  EventHandler m_on_enter_button;
  EventHandler m_on_start_outauto_tst;
  EventHandler m_on_stop_outauto_tst;

  bool m_enable_diag_controls;
  bool m_enable_enter_button;
  bool m_enable_secu3t_features;
  SECU3IO::DiagInpDat m_inputValues;
  CFont m_textFont;
};
