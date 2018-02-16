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

/** \file ddx_helpers.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#ifdef DDX_HELPERS_EXPORTS
 #define DDX_HELPERS_API __declspec(dllexport)
#else
 #define DDX_HELPERS_API __declspec(dllimport)
#endif

void DDX_HELPERS_API DDX_Check_UCHAR(CDataExchange* pDX, int nIDC, UCHAR& value);
void DDX_HELPERS_API DDX_Check_bool(CDataExchange* pDX, int nIDC, bool& value);
void DDX_HELPERS_API DDX_CBIndex_UCHAR(CDataExchange* pDX, int nIDC, UCHAR& index);
void DDX_HELPERS_API DDX_CBIndex_int(CDataExchange* pDX, int nIDC, int& index);
void DDX_HELPERS_API DDX_CBIndex_bool(CDataExchange* pDX, int nIDC, bool& index);
void DDX_HELPERS_API DDX_Radio_UCHAR(CDataExchange* pDX, int nIDC, UCHAR& value);
void DDX_HELPERS_API DDX_Radio_bool(CDataExchange* pDX, int nIDC, bool& value);
void DDX_HELPERS_API DDX_Text_Fmt(CDataExchange* pDX, int nIDC, float& value, LPCTSTR OutFmt=_T("%f"), LPCTSTR InFmt=_T("%f"));
void DDX_HELPERS_API DDX_Text_Fmt(CDataExchange* pDX, int nIDC, int& value, LPCTSTR OutFmt=_T("%d"), LPCTSTR InFmt=_T("%d"));
