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

/** \file RSDeskDlg.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include <memory>
#include "IRSView.h"
#include "ui-core/DialogWithAccelerators.h"

class CWndScroller;

/////////////////////////////////////////////////////////////////////////////
// CRSDeskDlg dialog

class AFX_EXT_CLASS CRSDeskDlg : public CModelessDialog, public IRSView
{
  typedef CModelessDialog Super;

 public:
  CRSDeskDlg(CWnd* pParent = NULL);   // standard constructor
 ~CRSDeskDlg();
  static const UINT IDD;

  //--------interface implementation---------------
  virtual void Show(bool show);
  virtual void Enable(bool enable);
  virtual void SetValues(const SECU3IO::RawSensDat* i_values);
  virtual void GetValues(SECU3IO::RawSensDat* o_values);
  virtual void EnableSECU3TItems(bool i_enable);
  virtual void EnableExtraIO(bool i_enable);
  virtual void EnableSpiAdc(bool i_enable);
  //-----------------------------------------------

  //изменение размеров окна
  void Resize(const CRect& i_rect);

  // Implementation
 protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  virtual BOOL OnInitDialog();
  afx_msg void OnDestroy();
  afx_msg void OnSize(UINT nType, int cx, int cy);
  DECLARE_MESSAGE_MAP()

  void updateScrollerSize(void);

 private:
  int  m_enabled;
  bool m_enable_secu3t_features;
  bool m_enable_extraio;
  bool m_enable_spiadc;
  bool m_was_initialized;

  struct Input
  {
   Input() : value(.0f) {}
   void StoreRects(void);
   void Scale(float Xf, float Yf);
   CStatic ctrl[3]; //field, caption, unit
   CRect rect[3];   //field, caption, unit
   float value;
  };

  Input m_map_inp;
  Input m_ubat_inp;
  Input m_temp_inp;
  Input m_knock_inp;
  Input m_tps_inp;
  Input m_add_i1_inp;
  Input m_add_i2_inp;
  Input m_add_i3_inp;
  Input m_add_i4_inp;
  //from MCP3204
  Input m_add_i5_inp;
  Input m_add_i6_inp;
  Input m_add_i7_inp;
  Input m_add_i8_inp;

  CFont m_fieldFont;
  CRect m_origRect;
  
  std::auto_ptr<CWndScroller> mp_scr;
};

/////////////////////////////////////////////////////////////////////////////
