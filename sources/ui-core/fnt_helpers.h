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

/** \file fnt_helpers.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#ifdef FNT_HELPERS_EXPORTS
 #define FNT_HELPERS_API __declspec(dllexport)
#else
 #define FNT_HELPERS_API __declspec(dllimport)
#endif

// wnd - Window to clone font from
// newFont - pointer to objects which will receive new font
// height - height of a new font
// bold - new font will be bold if true
bool FNT_HELPERS_API CloneWndFont(CWnd* wnd, CFont* newFnt, LONG height, bool bold = false);
