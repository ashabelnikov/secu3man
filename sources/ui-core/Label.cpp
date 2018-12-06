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

/** \file Label.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "Label.h"

CLabel::CLabel()
: m_hCursor(NULL)
, m_crText(GetSysColor(COLOR_WINDOWTEXT))
, m_brushColor(GetSysColor(COLOR_3DFACE))
, m_hBrush(::CreateSolidBrush(m_brushColor))
{
 ::GetObject((HFONT)GetStockObject(DEFAULT_GUI_FONT), sizeof(m_lf), &m_lf);
 m_font.CreateFontIndirect(&m_lf);
}

CLabel::~CLabel()
{
 m_font.DeleteObject();
 ::DeleteObject(m_hBrush);
}

BEGIN_MESSAGE_MAP(CLabel, Super)
 ON_WM_CTLCOLOR_REFLECT()
 ON_WM_LBUTTONDOWN()
 ON_WM_SETCURSOR()
END_MESSAGE_MAP()

HBRUSH CLabel::CtlColor(CDC* pDC, UINT nCtlColor) 
{
 COLORREF newcolor = GetSysColor(COLOR_3DFACE);
 if (m_brushColor != newcolor)
 {
  m_brushColor = newcolor;
  ::DeleteObject(m_hBrush);
  m_hBrush = CreateSolidBrush(m_brushColor);
 }

 if (CTLCOLOR_STATIC == nCtlColor)
 {
  pDC->SelectObject(&m_font);
  pDC->SetTextColor(m_crText);
  pDC->SetBkMode(TRANSPARENT);
 }

 return m_hBrush;
}

void CLabel::_ReconstructFont(void)
{
 m_font.DeleteObject();
 VERIFY(m_font.CreateFontIndirect(&m_lf));
}

void CLabel::SetLink(bool bLink)
{
 if (bLink)
  ModifyStyle(0, SS_NOTIFY);
 else
  ModifyStyle(SS_NOTIFY, 0);
}

void CLabel::OnLButtonDown(UINT nFlags, CPoint point) 
{
 CString strLink;

 GetWindowText(strLink);
 ShellExecute(NULL, _T("open"), strLink, NULL, NULL, SW_SHOWNORMAL);
		
 CStatic::OnLButtonDown(nFlags, point);
}

BOOL CLabel::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
 if (m_hCursor)
 {
  ::SetCursor(m_hCursor);
  return true;
 }

 return CStatic::OnSetCursor(pWnd, nHitTest, message);
}

void CLabel::SetText(const _TSTRING& strText)
{
 SetWindowText(strText.c_str());
}

void CLabel::SetTextColor(COLORREF crText)
{
 m_crText = crText;
 RedrawWindow();
}

void CLabel::SetFontUnderline(bool bSet)
{	
 m_lf.lfUnderline = bSet;
 _ReconstructFont();
 RedrawWindow();
}

void CLabel::SetLinkCursor(HCURSOR hCursor)
{
 m_hCursor = hCursor;
}
