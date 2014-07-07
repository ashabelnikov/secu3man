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

#pragma once

#include <memory>
#include "ui-core/DialogWithAccelerators.h"

class CToolTipCtrlEx;

/////////////////////////////////////////////////////////////////////////////
// CLMDeskDlg dialog

class AFX_EXT_CLASS CLMDeskDlg : public CModelessDialog
{
  typedef CModelessDialog Super;

 public:
  CLMDeskDlg(CWnd* pParent = NULL);   // standard constructor
  static const UINT IDD;

  void Show(bool show);
  void Enable(bool enable);
  void SetValues(bool k1, bool k2, bool k3);

  //изменение размеров окна
  void Resize(const CRect& i_rect);

  // Implementation
 protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  virtual BOOL OnInitDialog();
  afx_msg HBRUSH OnCtlColor( CDC* pDC, CWnd* pWnd, UINT nCtlColor );
  DECLARE_MESSAGE_MAP()

 private:
  enum { KEY_COUNT = 3 };           //number of marker keys

  int  m_enabled;
  CStatic m_key_text[KEY_COUNT];    //controls
  _TSTRING m_key_str[KEY_COUNT];    //strings
  bool m_key_flags[KEY_COUNT];      //state flags
  COLORREF m_key_colors[KEY_COUNT]; //colors
  std::auto_ptr<CToolTipCtrlEx> mp_ttc;
};

/////////////////////////////////////////////////////////////////////////////
