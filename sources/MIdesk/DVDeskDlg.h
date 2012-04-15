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

#include "common/ObjectTimer.h"
#include "IDVView.h"
#include "ui-core/DialogWithAccelerators.h"

/////////////////////////////////////////////////////////////////////////////
// CDVDeskDlg dialog

class AFX_EXT_CLASS CDVDeskDlg : public CModelessDialog, public IDVView
{
  typedef CModelessDialog Super;

 public:
  CDVDeskDlg(CWnd* pParent = NULL);   // standard constructor
  static const UINT IDD;

  //--------interface implementation---------------
  virtual void Show(bool show);
  virtual void Enable(bool enable);
  virtual bool IsEnabled(void);
  virtual void SetValues(const SECU3IO::DbgvarDat* i_values);
  virtual void GetValues(SECU3IO::DbgvarDat* o_values);
  virtual void SetUpdatePeriod(unsigned int i_period);
  //-----------------------------------------------

  // Implementation
 protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  virtual BOOL OnInitDialog();
  afx_msg void OnDestroy();    //deactivate
  afx_msg void OnBaseCheck(UINT nID);
  DECLARE_MESSAGE_MAP()

  void OnUpdateTimer(void);

 private:
  CFont m_fieldFont;
  CObjectTimer<CDVDeskDlg> m_update_timer;
  unsigned int m_update_period;
  bool m_was_initialized;
  int m_enabled;

  enum {VU_SIZE = 4}; 
  struct VarUnit
  {
   int var_value;
   _TSTRING base_fmt;
   CStatic var_field;
   CStatic var_caption;
   CButton base_check;
  }m_vu[VU_SIZE];
};

/////////////////////////////////////////////////////////////////////////////
