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

/** \file DevDiagnostTabDlg.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include <memory>
#include "common/FastDelegate.h"
#include "io-core/SECU3IO.h"
#include "ui-core/TabDialog.h"

class CDiagnostContextMenuManager;
class COscillCtrl;
class CDevDiagInpsDlg;
class CDevDiagOutsDlg;

class CDevDiagnostTabDlg : public CTabDialog
{
  typedef CTabDialog Super;
  typedef fastdelegate::FastDelegate0<> EventHandler;
  typedef fastdelegate::FastDelegate1<bool> EventFlag;
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
  void setOnEnableBLDETesting(EventFlag OnFunction);
  void setOnEnableTACHOTesting(EventFlag OnFunction);

  //Set values of inputs (digital and analog)
  void SetInputValues(const SECU3IO::DiagInpDat* i_values);
  void SetEnterButtonCaption(const _TSTRING& i_text);
  void SetEnterButton(bool i_state);
  void SetOutputValue(int id, bool state);

  void EnableDiagControls(bool i_enable);
  void EnableEnterButton(bool i_enable);
  void EnableSECU3TFeatures(bool i_enable);
  void EnableExtraIO(bool i_enable);
  void EnableBLDETesting(bool i_enable);
  void EnableTACHOTesting(bool i_enable);
  bool IsBLDETestingEnabled(void);
  bool IsTACHOTestingEnabled(void);

  void EnableStartAutoTstMenuItem(bool i_enable);
  void EnableStopAutoTstMenuItem(bool i_enable);

  //IDs for outputs (SECU-3T and SECU-3i)
  enum 
  {
   OID_IGN_OUT1 = 0,
   OID_IGN_O1 = 0,

   OID_IGN_OUT2 = 1,
   OID_IGN_O2 = 1,

   OID_IGN_OUT3 = 2,
   OID_IGN_O3 = 2,

   OID_IGN_OUT4 = 3,
   OID_IGN_O4 = 3,

   OID_IE = 4,
   OID_IGN_O5 = 4,

   OID_FE = 5,
   OID_ECFO = 5,

   OID_ECF = 6,
   OID_INJ_O1 = 6,

   OID_CE = 7,
   OID_INJ_O2 = 7,

   OID_ST_BLOCK = 8,
   OID_INJ_O3 = 8,

   OID_ADD_IO1 = 9,
   OID_INJ_O4 = 9,

   OID_ADD_IO2 = 10,
   OID_INJ_O5 = 10,

   OID_BL = 11,       //3-state output

   OID_DE = 12,       //3-state output
   
   OID_STBL_O = 13,

   OID_CEL_O = 14,

   OID_FPMP_O = 15,

   OID_PWRR_O = 16,

   OID_EVAP_O = 17,

   OID_O2SH_O = 18,

   OID_COND_O = 19,

   OID_ADD_O2 = 20,

   OID_TACH_O = 21,   //special

   OID_SECU3T_NUM = 13,
   OID_SECU3i_NUM = 22
  };

 protected:
  virtual BOOL OnInitDialog();
  virtual void DoDataExchange(CDataExchange* pDX);
  afx_msg void OnUpdateDiagControls(CCmdUI* pCmdUI);
  afx_msg void OnUpdateEnterButton(CCmdUI* pCmdUI);
  afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
  afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
  afx_msg void OnEnterButton();
  afx_msg void OnTimer(UINT nIDEvent);
  afx_msg void OnDestroy();
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnStartOutputsAutoTesting();
  afx_msg void OnStopOutputsAutoTesting();
  afx_msg void OnEnableBLDETesting();
  afx_msg void OnEnableTACHOTesting();
  DECLARE_MESSAGE_MAP()

 private:
  void OnOutputCheckToggle(int output_id, bool state);

  void _InitializeOscilloscopeControls(void);

  std::auto_ptr<COscillCtrl> mp_OScopeCtrl1;
  std::auto_ptr<COscillCtrl> mp_OScopeCtrl2;
  std::auto_ptr<CDiagnostContextMenuManager> mp_ContextMenuManager;
  std::auto_ptr<CDevDiagInpsDlg> mp_inpsDlg;
  std::auto_ptr<CDevDiagOutsDlg> mp_outsDlg;

  CButton m_enter_button;
  CStatic m_outputs_group;
  CStatic m_inputs_group;
  CStatic m_warning_text;
  EventOutputToggle m_on_output_check;
  EventHandler m_on_enter_button;
  EventHandler m_on_start_outauto_tst;
  EventHandler m_on_stop_outauto_tst;
  EventFlag m_on_enable_blde_tst;
  EventFlag m_on_enable_tacho_tst;

  bool m_enable_diag_controls;
  bool m_enable_enter_button;
  bool m_enable_blde_testing;
  bool m_enable_tacho_testing;
  bool m_start_autotst_enabled;
  bool m_stop_autotst_enabled;
  
  bool m_initialized;
};
