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

#include "IRSView.h"
#include "ui-core\DialogWithAccelerators.h"

/////////////////////////////////////////////////////////////////////////////
// CRSDeskDlg dialog

class AFX_EXT_CLASS CRSDeskDlg : public CModelessDialog, public IRSView
{
  typedef CModelessDialog Super;

 public:
  CRSDeskDlg(CWnd* pParent = NULL);   // standard constructor
  static const UINT IDD;

  //--------interface implementation---------------
  virtual void Show(bool show);
  virtual void Enable(bool enable);
  virtual void SetValues(const SECU3IO::RawSensDat* i_values);
  virtual void GetValues(SECU3IO::RawSensDat* o_values);
  //-----------------------------------------------

  //изменение размеров окна
  void Resize(const CRect& i_rect);

  // Implementation
 protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  virtual BOOL OnInitDialog();
  DECLARE_MESSAGE_MAP()

 private:
  float m_map_value;
  float m_ubat_value;
  float m_temp_value;
  float m_knock_value;

  CStatic m_map_field;
  CStatic m_ubat_field;
  CStatic m_temp_field;
  CStatic m_knock_field;

  CStatic m_map_caption;
  CStatic m_ubat_caption;
  CStatic m_temp_caption;
  CStatic m_knock_caption;

  CStatic m_map_unit;
  CStatic m_ubat_unit;
  CStatic m_temp_unit;
  CStatic m_knock_unit;
};

/////////////////////////////////////////////////////////////////////////////
