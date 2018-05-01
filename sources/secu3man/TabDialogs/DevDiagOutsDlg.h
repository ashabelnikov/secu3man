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

/** \file DevDiagOutsDlg.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include <memory>
#include "common/FastDelegate.h"
#include "io-core/SECU3IO.h"

class CWndScroller;

class CDevDiagOutsDlg : public CDialog
{
  typedef CDialog Super;
  typedef fastdelegate::FastDelegate2<int, bool> EventOutputToggle;

 public:
  CDevDiagOutsDlg(CWnd* pParent = NULL);
  static const UINT IDD;
  virtual LPCTSTR GetDialogID(void) const;

  //установка обработчиков событий от элементов управления
  void setOnOutputToggle(EventOutputToggle OnFunction);

  //Set values of inputs (digital and analog)
  void SetOutputValue(int id, bool state);

  void EnableDiagControls(bool i_enable);

  //true - SECU-3T, false - SECU-3i
  void EnableSECU3TFeatures(bool i_enable);

  void EnableBLDETesting(bool i_enable);

  //note: use wnd_insert_after parameter to fix tab order!
  void SetPosition(int x_pos, int y_pos, CWnd* wnd_insert_after = NULL);

 protected:
  virtual BOOL OnInitDialog();
  virtual void DoDataExchange(CDataExchange* pDX);
  afx_msg void OnUpdateDiagControls(CCmdUI* pCmdUI);
  afx_msg void OnOutputCheckToggle(UINT nID);
  afx_msg void OnTimer(UINT nIDEvent);
  afx_msg void OnDestroy();
  afx_msg void OnSize(UINT nType, int cx, int cy);
  DECLARE_MESSAGE_MAP()

 private:
  void _UpdateScrlViewSize(void);
  EventOutputToggle m_on_output_check;

  bool m_enable_diag_controls;
  bool m_enable_secu3t_features;
  bool m_enable_blde_testing;
  std::auto_ptr<CWndScroller> mp_scr;
};
