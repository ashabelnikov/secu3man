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

/** \file HatchDispCtrl.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include "common/fastdelegate.h"
#include <vector>

class AFX_EXT_CLASS CHatchDispCtrl : public CStatic
{
  typedef CStatic Super;

 public:
  CHatchDispCtrl();
  virtual ~CHatchDispCtrl();

  void Init(void);
  void SetRange(unsigned long range);
  void DrawHatch(unsigned long pos, COLORREF color);
  void DrawBitmap(void);

 protected:
  afx_msg void OnPaint();
  DECLARE_MESSAGE_MAP()

 private:
  COLORREF m_COLOR_3DFACE;     // for testing changing system colors
  CBitmap m_bitmap;
  CDC m_memDC;
  unsigned long m_range;
  std::vector<COLORREF> m_hatches;
  int m_width;
};
