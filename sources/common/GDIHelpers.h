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

/** \file GDIHelpers.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include "common/MathHelpers.h"

struct GDIHelpers
{

 //Obtains window coordinates relatively to parent's window coordinates
 static CRect GetChildWndRect(CWnd* ip_wnd)
 {
  ASSERT(ip_wnd);
  CRect rect;
  ip_wnd->GetWindowRect(rect);
  ip_wnd->GetParent()->ScreenToClient(rect);
  return rect;
 }

 //Obtains window coordinates relatively to parent's window coordinates (version for control ID)
 static CRect GetChildWndRect(CWnd* p_parent, UINT nCtrlID)
 {
  ASSERT(p_parent);
  CWnd* p_wnd = p_parent->GetDlgItem(nCtrlID);
  ASSERT(p_wnd);
  CRect rect;
  p_wnd->GetWindowRect(rect);
  p_parent->ScreenToClient(rect);
  return rect;
 }

 //Scales all rectangle points
 static void ScaleRect(CRect& io_rect, float i_x_factor, float i_y_factor)
 {
  io_rect.left = MathHelpers::Round(io_rect.left * i_x_factor);
  io_rect.right = MathHelpers::Round(io_rect.right * i_x_factor);
  io_rect.top = MathHelpers::Round(io_rect.top * i_y_factor);
  io_rect.bottom = MathHelpers::Round(io_rect.bottom * i_y_factor);
 }

 //Calculates width and height ratios between two rects
 static void CalcRectToRectRatio(const CRect& i_dividend, const CRect& i_divisor, float& o_x_factor, float& o_y_factor)
 {
  o_x_factor = ((float)i_dividend.Width()) / (float)i_divisor.Width();
  o_y_factor = ((float)i_dividend.Height()) / (float)i_divisor.Height();
 }

 //Resize and center rect
 static void ResizeAndCenterRect(const CRect& i_external, CRect& io_victim)
 {
  float Xf = ((float)i_external.Width()) / io_victim.Width();
  float Yf = ((float)i_external.Height()) / io_victim.Height();
  float factor = min(Xf, Yf);
  //scale
  io_victim.right = MathHelpers::Round((io_victim.Width() * factor));
  io_victim.bottom = MathHelpers::Round((io_victim.Height() * factor));
  io_victim.left = 0;
  io_victim.top = 0;
  //center
  CPoint center_external = i_external.CenterPoint();
  CPoint center_victim = io_victim.CenterPoint();
  io_victim.OffsetRect(center_external - center_victim);
 }

 //get screen resolution
 static CRect GetScreenRect(CWnd* p_wnd)
 {
  CDC* pDC = p_wnd->GetDC();
  int x_resolution = pDC->GetDeviceCaps(HORZRES);
  int y_resolution = pDC->GetDeviceCaps(VERTRES);
  return CRect(0, 0, x_resolution, y_resolution);
 }

 static COLORREF HSBToRGB(int index, int saturation, int brightness)
 {
  if (index > 767)
   index = 767;
  if (index < 0)
   index = 0;

  int index_m = index % 256;
  int r, g, b;

  if (index < 256) //0...255
  {
   r = 255 - index_m;
   g = index_m;
   b = 0;
  }
  else if (index < 512) //256...511
  {
   r = 0;
   g = 255 - index_m;
   b = index_m;
  }
  else //512...767
  {
   r = index_m;
   g = 0;
   b = 255 - index_m;
  }

  //apply saturation
  r = ((r * saturation) / 255) + (255 - saturation);
  g = ((g * saturation) / 255) + (255 - saturation);
  b = ((b * saturation) / 255) + (255 - saturation);

  //apply britness
  r = (r * brightness) / 255;
  g = (g * brightness) / 255;
  b = (b * brightness) / 255;

  return RGB(r, g, b);
 }

 static std::vector<COLORREF> GenerateGradientList(int start, int end, int items, int saturation, int britness)
 {
  if (end > 511) end = 511;
  if (start < 0) start = 0;
  std::vector<COLORREF> colors;
  for (int i = 0; i < items; i++)
  {   
   int value = MathHelpers::Round(((float)(end - start) * i) / items);
   value += start;
   colors.push_back(HSBToRGB(511 - value, saturation, britness));
  }
  return colors;
 }
};
