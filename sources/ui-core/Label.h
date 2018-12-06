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

/** \file Label.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include "common/unicodesupport.h"

class AFX_EXT_CLASS CLabel : public CStatic
{
  typedef CStatic Super;
 public:
  CLabel();
  virtual ~CLabel();

  void SetTextColor(COLORREF crText);
  void SetText(const _TSTRING& strText);
  void SetFontUnderline(bool bSet);
  void SetLink(bool bLink);
  void SetLinkCursor(HCURSOR hCursor);

 protected:
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
  afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
  DECLARE_MESSAGE_MAP()

 private:
  void _ReconstructFont(void);

  COLORREF m_brushColor;
  COLORREF m_crText;
  HBRUSH  m_hBrush;
  HCURSOR m_hCursor;
  LOGFONT m_lf;
  CFont  m_font;
};
