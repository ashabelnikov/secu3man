/* Common utilities - language layer
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
             e-mail: shabelnikov@secu-3.org
             Microprocessor systems - design & programming.
*/

/** \file DPIAware.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

//Class for working with DPI-aware issues
class DPIAware
{
 public:
  DPIAware() : m_initialized(false), m_dpiX(96), m_dpiY(96) { }

  // Get DPI of screen
  int GetDPIX() { _Init(); return m_dpiX; }
  int GetDPIY() { _Init(); return m_dpiY; }

  // Convert between raw pixels and relative pixels.
  int ScaleX(int x) { _Init(); return MulDiv(x, m_dpiX, 96); }
  int ScaleY(int y) { _Init(); return MulDiv(y, m_dpiY, 96); }

  int UnScaleX(int x) { _Init(); return MulDiv(x, 96, m_dpiX); }
  int UnScaleY(int y) { _Init(); return MulDiv(y, 96, m_dpiY); }

  int FontHeight(int pixels) { _Init(); return MulDiv(pixels, m_dpiX, 72); }

 private:
  void _Init()
  {
   if (m_initialized)
    return;
   HDC hdc = GetDC(NULL);
   if (hdc)
   {
    m_dpiX = GetDeviceCaps(hdc, LOGPIXELSX);
    m_dpiY = GetDeviceCaps(hdc, LOGPIXELSY);
    ReleaseDC(NULL, hdc);
   }
   m_initialized = true;
  }

 private:
  bool m_initialized;
  int  m_dpiX;
  int  m_dpiY;
};
