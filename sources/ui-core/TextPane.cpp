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

/** \file TextPane.cpp
 * \author Alexey A. Shabelnikov
 */

#include "StdAfx.h"
#include "TextPane.h"

BEGIN_MESSAGE_MAP(CTextPane, Super)
 ON_WM_PAINT()
END_MESSAGE_MAP()

CTextPane::CTextPane()
{
 SetPaneFont("Arial");
}

void CTextPane::OnPaint()
{
 CPaintDC dc(this);
 CRect rc;
 GetWindowRect(rc);

 dc.SetBkMode(TRANSPARENT);

 //Save old GDI objects
 CFont * oldFont = dc.SelectObject(&m_font);
 COLORREF oldColor = dc.SetTextColor(::GetSysColor(COLOR_3DHILIGHT));

 rc.right = rc.Width();
 rc.bottom = rc.Height();
 rc.left = rc.top = 0;
 dc.FillSolidRect(rc, ::GetSysColor(COLOR_3DFACE));

 dc.DrawText(m_text.c_str(), rc + CPoint(1, 1), DT_SINGLELINE | DT_LEFT | DT_VCENTER);
 dc.SetTextColor(::GetSysColor( COLOR_3DSHADOW));
 dc.DrawText(m_text.c_str(), rc, DT_SINGLELINE | DT_LEFT | DT_VCENTER);

 // restore old GDI objects
 dc.SetTextColor(oldColor);
 dc.SelectObject(oldFont);
}

BOOL CTextPane::Create(const _TSTRING& text, DWORD dwStyle, CWnd *pParent, UINT nID)
{
 m_text = text;
 return Super::Create(AfxRegisterWndClass(CS_BYTEALIGNCLIENT), text.c_str(), dwStyle, CRect(0,0,0,0), pParent, nID);
}

void CTextPane::SetPaneText(const _TSTRING& text)
{
 m_text = text;
}

void CTextPane::SetPaneFont(const _TSTRING& name, int nHeight/* = 18*/, int nWeight/* = FW_BOLD*/, BYTE bItalic/* = true*/, BYTE bUnderline/* = false*/)
{
 if(m_font.m_hObject) {
  m_font.Detach();
 }

 m_font.CreateFont(nHeight, 0, 0, 0, nWeight, bItalic, bUnderline, 0,0,0,0,0,0, name.c_str());
}
