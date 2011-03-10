/****************************************************************
 *
 *  Created by Alexey A. Shabelnikov. Ukraine, Gorlovka 2008.
 *   ICQ: 405-791-931. e-mail: shabelnikov-stc@mail.ru
 *  Microprocessors systems - design & programming.
 *
 *****************************************************************/

#include "StdAfx.h"
#include "HeaderCtrlEx.h"

/////////////////////////////////////////////////////////////////////////////
// CHeaderCtrlEx

CHeaderCtrlEx::CHeaderCtrlEx()
{
 m_text_color = GetSysColor(COLOR_BTNTEXT);
}

CHeaderCtrlEx::~CHeaderCtrlEx()
{
 //na
}

BEGIN_MESSAGE_MAP(CHeaderCtrlEx, CHeaderCtrl)
END_MESSAGE_MAP()

BOOL CHeaderCtrlEx::Init(CHeaderCtrl *pHeader)
{
 _ASSERTE(pHeader && pHeader->GetSafeHwnd());
 if (!SubclassWindow(pHeader->GetSafeHwnd()))
 {
  OutputDebugString(_T("Unable to subclass existing header!\n"));
  return FALSE;
 }

 // Iterate through the items and attach the image list
 HDITEM hdi;
 ZeroMemory(&hdi,sizeof(HDITEM));
 for (int i=0; i < GetItemCount(); i++)
 {
  GetItem(i, &hdi);
  hdi.fmt |=  HDF_OWNERDRAW;
  hdi.mask |= HDI_FORMAT;
  SetItem( i, &hdi );
 }

 return TRUE;
}


void CHeaderCtrlEx::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
 // This code only works with header controls.
 ASSERT(lpDrawItemStruct->CtlType == ODT_HEADER);

 HDITEM hdi;
 TCHAR  lpBuffer[256];

 hdi.mask = HDI_TEXT;
 hdi.pszText = lpBuffer;
 hdi.cchTextMax = 256;

 GetItem(lpDrawItemStruct->itemID, &hdi);

 // Draw the button frame.
 ::DrawFrameControl(lpDrawItemStruct->hDC, &lpDrawItemStruct->rcItem, DFC_BUTTON, DFCS_BUTTONPUSH);

 // Draw the items text using the text color red.
 COLORREF crOldColor = ::SetTextColor(lpDrawItemStruct->hDC, m_text_color);
 ::DrawText(lpDrawItemStruct->hDC, lpBuffer, _tcsclen(lpBuffer),
  &lpDrawItemStruct->rcItem, DT_SINGLELINE|DT_VCENTER|DT_CENTER);
 ::SetTextColor(lpDrawItemStruct->hDC, crOldColor);
}

void CHeaderCtrlEx::SetTextColor(COLORREF i_color)
{
 m_text_color = i_color;
 Invalidate();
}
