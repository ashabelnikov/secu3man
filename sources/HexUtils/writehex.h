/* Intel Hex writer (binary to hex)
   Copyright (C) 2005 Alexey A. Shabelnikov. Ukraine, Kiev

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

/** \file writehex.h
 * \author Alexey A. Shabelnikov
 */

#pragma once
#include <vector>

#ifdef HEXUTILS_EXPORTS
 #define HEXUTILS_API __declspec(dllexport)
#else
 #define HEXUTILS_API __declspec(dllimport)
#endif

HEXUTILS_API bool WriteHexFile(FILE* fout, const std::vector<BYTE>& code);
