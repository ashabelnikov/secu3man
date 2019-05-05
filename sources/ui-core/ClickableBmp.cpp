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

/** \file ClickableBmp.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "ClickableBmp.h"

CClickableBmp::CClickableBmp()
: m_hCursor(NULL)
{
 //empty
}

CClickableBmp::~CClickableBmp()
{
 m_bitmap.DeleteObject();
}

BEGIN_MESSAGE_MAP(CClickableBmp, Super)
 ON_WM_LBUTTONDOWN()
 ON_WM_SETCURSOR()
END_MESSAGE_MAP()

void CClickableBmp::SetOnClick(const EventHandler& onClickCB)
{
 m_onClickCB = onClickCB;
}

void CClickableBmp::OnLButtonDown(UINT nFlags, CPoint point) 
{
 m_onClickCB(); //delegate
		
 CStatic::OnLButtonDown(nFlags, point);
}

BOOL CClickableBmp::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
 if (m_hCursor)
 {
  ::SetCursor(m_hCursor);
  return true;
 }

 return CStatic::OnSetCursor(pWnd, nHitTest, message);
}

void CClickableBmp::SetClickCursor(HCURSOR hCursor)
{
 m_hCursor = hCursor;
}

void CClickableBmp::SetBitmapID(UINT nIDResource)
{
 if (!m_bitmap.m_hObject) //already initialized?
 {
  m_bitmap.LoadBitmap(nIDResource);

  //set bitmap background
  CDC memDC;
  memDC.CreateCompatibleDC(GetDC());
  memDC.SelectObject(&m_bitmap);
  BITMAP bmp;
  m_bitmap.GetBitmap(&bmp);
  COLORREF bk_color = GetSysColor(COLOR_BTNFACE);
  for(int i = 0; i < bmp.bmWidth; ++i)
  {
   for(int j = 0; j < bmp.bmHeight; ++j)
   {
    COLORREF p = memDC.GetPixel(i,j);
    if (p >= RGB(192,192,192)) //background color of original image is white
     memDC.SetPixel(i,j, bk_color);
   }
  }
 }
 SetBitmap(m_bitmap);

 //set SS_NOTIFY style, otherwise control will not send events!
 ModifyStyle(0, SS_NOTIFY);
}
