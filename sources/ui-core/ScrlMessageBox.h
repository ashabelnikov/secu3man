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

/** \file ScrlMessageBox.h
 * \author Alexey A. Shabelnikov
 */

#pragma once
#include "common/unicodesupport.h"

class AFX_EXT_CLASS CScrlMessageBox : public CDialog
{
 typedef CDialog Super;

 public:
  CScrlMessageBox(CWnd* pParentWnd, const _TSTRING& caption, const _TSTRING& text, LPCSTR icon);

 protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  virtual BOOL OnInitDialog();
  afx_msg void OnPaint();
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
  DECLARE_MESSAGE_MAP()

  void _AlignControls(int cx, int cy);

 private:
  CEdit m_edit;
  const _TSTRING m_caption;
  const _TSTRING m_text;
  LPCSTR m_icon;
  HICON m_hIcon;
  bool m_initialized;
  CSize m_createSize;
};
