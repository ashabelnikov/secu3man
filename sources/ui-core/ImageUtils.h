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

/** \file ImageUtils.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include "common/Unicodesupport.h"
#include <atlimage.h>


#ifdef IMAGE_UTILS_EXPORTS
 #define IMAGE_UTILS_API __declspec(dllexport)
#else
 #define IMAGE_UTILS_API __declspec(dllimport)
#endif

bool IMAGE_UTILS_API SaveScreenshot(CWnd* pWnd, bool selectFile);

bool IMAGE_UTILS_API LoadImageFromRes(HINSTANCE hinstance, LPCTSTR lpszResourceName, LPCTSTR resType, CImage* pImage);

void IMAGE_UTILS_API ResizeBitmap(CBitmap &srcBmp, CBitmap& dstBmp, int dstWidth, int dstHeight, int bltMode = HALFTONE);
