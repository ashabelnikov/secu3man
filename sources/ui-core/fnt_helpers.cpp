 /****************************************************************
 *
 *  Created by Alexey A. Shabelnikov. Ukraine, Gorlovka 2008.
 *   ICQ: 405-791-931. e-mail: shabelnikov-stc@mail.ru
 *  Microprocessors systems - design & programming.
 *
 *****************************************************************/

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
