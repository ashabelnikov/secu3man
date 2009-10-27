/****************************************************************
 *       SECU-3  - An open source, free engine control unit
 *    Designed by Alexey A. Shabelnikov. Ukraine, Gorlovka 2007.
 *       Microprocessors systems - design & programming.
 *    contacts:
 *              http://secu-3.narod.ru
 *              ICQ: 405-791-931
 ****************************************************************/

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
