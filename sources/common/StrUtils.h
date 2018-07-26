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

/** \file StrUtils.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include <vector>
#include "UnicodeSupport.h"

class StrUtils
{
 public:
  static std::vector<_TSTRING> TokenizeStr(const TCHAR *s, TCHAR c = _T(' '))
  {
   std::vector<_TSTRING> result;
   if (0 == *s)
    return result;
   do
   {
    const TCHAR *begin = s;
    while(*s && *s != c)
     s++;
    result.push_back(_TSTRING(begin, s));    
   }while(*s++);
   return result;
  }
};
