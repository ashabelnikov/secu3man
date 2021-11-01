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

/** \file HatchDispCtrl.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "HatchDispCtrl.h"
#include "common/gdihelpers.h"

CHatchDispCtrl::CHatchDispCtrl()
: m_range(0)
, m_hatches(256)
, m_width(0)
{
 m_COLOR_3DFACE = GetSysColor(COLOR_3DFACE);
}

CHatchDispCtrl::~CHatchDispCtrl()
{
 m_bitmap.DeleteObject();
 m_memDC.DeleteDC();
}

BEGIN_MESSAGE_MAP(CHatchDispCtrl, Super)
 ON_WM_PAINT()
END_MESSAGE_MAP()

void CHatchDispCtrl::Init(void)
{
 CRect rc;
 GetClientRect(&rc);

 m_memDC.DeleteDC();
 m_memDC.CreateCompatibleDC(GetDC());
 m_bitmap.DeleteObject();
 m_bitmap.CreateCompatibleBitmap(GetDC(), rc.Width(), rc.Height());
 m_memDC.SelectObject(&m_bitmap);

 m_hatches.resize(rc.Width());
 std::fill(m_hatches.begin(), m_hatches.end(), 0);

 m_width = rc.Width(); //save width for further use
}

void CHatchDispCtrl::SetRange(unsigned long range)
{
 m_range = range;
}

void CHatchDispCtrl::DrawHatch(unsigned long pos, COLORREF color)
{
 if (pos > m_range)
  return;

 int index = ((m_width-1) * pos) / m_range;

 m_hatches[index] = color;
}

void CHatchDispCtrl::DrawBitmap(void)
{
 CRect rc;
 GetClientRect(&rc);

 m_memDC.FillSolidRect(0,0, rc.Width(), rc.Height(), GetSysColor(COLOR_BTNFACE));

 //draw hatches
 for(size_t i = 0; i < m_hatches.size(); ++i)
 {
  if (0 != m_hatches[i])
  {
   CPen pen(PS_SOLID, 1, m_hatches[i]);
   m_memDC.SelectObject(&pen);
   m_memDC.MoveTo(i, 0);
   m_memDC.LineTo(i, rc.Height());
  }
 }

 Invalidate();
}

void CHatchDispCtrl::OnPaint()
{
 if (m_COLOR_3DFACE != GetSysColor(COLOR_3DFACE))
 {
  m_COLOR_3DFACE = GetSysColor(COLOR_3DFACE);
  DrawBitmap(); //redraw bitmap
 }

 CPaintDC dc(this);
 CRect rc;
 GetClientRect(&rc);

 //copy the resulting bitmap to the destination DC
 //if window is disabled, then just fill rectangle with background color
 if (IsWindowEnabled())
 {
  dc.BitBlt(0, 0, rc.Width(), rc.Height(), &m_memDC, 0, 0, SRCCOPY);
 }
 else
  dc.FillSolidRect(0,0, rc.Width(), rc.Height(), GetSysColor(COLOR_BTNFACE));
}
