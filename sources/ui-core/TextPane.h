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

/** \file TextPane.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include "common/unicodesupport.h"

class AFX_EXT_CLASS CTextPane : public CWnd
{
  typedef CWnd Super;

 public:

  CTextPane();

  virtual BOOL Create(const _TSTRING& text, DWORD dwStyle, CWnd * pParent, UINT nID);

  void SetPaneText(const _TSTRING& test);
  
  void SetPaneFont(const _TSTRING& name, int nHeight = 18, int nWeight = FW_BOLD, BYTE bItalic = TRUE, BYTE bUnderline = FALSE);

  afx_msg void OnPaint();
  DECLARE_MESSAGE_MAP()

 private:
  CFont m_font;
  _TSTRING m_text;
};
