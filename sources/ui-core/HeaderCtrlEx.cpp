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
 //empty
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
