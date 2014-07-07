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

#include "stdafx.h"
#include "common/DPIAware.h"
#include "fnt_helpers.h"

bool FNT_HELPERS_API CloneWndFont(CWnd* ip_wnd, CFont* op_newFnt, LONG height, bool bold /*= false*/)
{
 if (!ip_wnd || !op_newFnt)
  return false;
 
 CFont* p_font = ip_wnd->GetFont();
 if (!p_font)
  return false;

 if (op_newFnt->m_hObject)
  op_newFnt->DeleteObject();

 // Create font for 96 DPI.
 LOGFONT lf;
 p_font->GetLogFont(&lf);

 // Set other LOGFONT values as appropriate.
 lf.lfHeight = -MulDiv(height, 96, 72);
 lf.lfWeight = bold ? FW_BOLD : FW_NORMAL; 
 CFont tmpFont;
 VERIFY(tmpFont.CreateFontIndirect(&lf));

 // Scale the tmHeight; create target font.
 TEXTMETRIC tm;
 CDC *pDC = ip_wnd->GetDC();
 CFont* fontOld = pDC->SelectObject(&tmpFont);
 pDC->GetTextMetrics(&tm);
 pDC->SelectObject(fontOld);
 tmpFont.DeleteObject();
 ip_wnd->ReleaseDC(pDC);
 DPIAware da;
 lf.lfHeight = da.ScaleY(tm.tmHeight);
 op_newFnt->CreateFontIndirect(&lf);
 return true;
}
