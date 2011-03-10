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

#pragma once

#include "common\MathHelpers.h"

struct MIHelpers
{

 //Получает координаты окна в системе координат родительского окна
 static CRect GetChildWndRect(CWnd* ip_wnd)
 {
  ASSERT(ip_wnd);
  CRect rect;
  ip_wnd->GetWindowRect(rect);
  ip_wnd->GetParent()->ScreenToClient(rect);
  return rect;
 }

 //Масштабирует все точки прямоугольника
 static void ScaleRect(CRect& io_rect, float i_x_factor, float i_y_factor)
 {
  io_rect.left = MathHelpers::Round(io_rect.left * i_x_factor);
  io_rect.right = MathHelpers::Round(io_rect.right * i_x_factor);
  io_rect.top = MathHelpers::Round(io_rect.top * i_y_factor);
  io_rect.bottom = MathHelpers::Round(io_rect.bottom * i_y_factor);
 }

 static void CalcRectToRectRatio(const CRect& i_dividend, const CRect& i_divisor, float& o_x_factor, float& o_y_factor)
 {
  o_x_factor = ((float)i_dividend.Width()) / (float)i_divisor.Width();
  o_y_factor = ((float)i_dividend.Height()) / (float)i_divisor.Height();
 }

};
