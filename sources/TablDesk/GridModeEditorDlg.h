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

#include "common/UnicodeSupport.h"
#include "ui-core/EditEx.h"

class CGridModeEditorDlg : public CDialog
{
  typedef CDialog Super;

 public:
  CGridModeEditorDlg(CWnd* pParent = NULL);   // standard constructor
 ~CGridModeEditorDlg();
  static const UINT IDD;


 // Implementation
 protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  virtual BOOL OnInitDialog();
  DECLARE_MESSAGE_MAP()

 private:
  CEdit m_wrk_grid[16][16];
  CStatic m_wrk_map_labels[16];
};
