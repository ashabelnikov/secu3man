/* Common utilities - language layer
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
             e-mail: shabelnikov-stc@mail.ru
             Microprocessor systems - design & programming.
*/

#pragma once

#include "UnicodeSupport.h"

class MLL //Multy Language Layer
{
 public:

  //std version
  static _TSTRING GetString(UINT i_id)
  {
#define SIZE 1024
   TCHAR buffer[SIZE];
   HINSTANCE hInst = GetModuleBaseAddress();
   int real_size = ::LoadString(hInst, i_id, buffer, SIZE);
   ASSERT(real_size < (SIZE - 1));
   ASSERT(real_size);
   return _TSTRING(buffer);
#undef SIZE
  }

  //MFC version
  static CString LoadString(UINT i_id)
  {
   return GetString(i_id).c_str();
  }

 private:
  // returns the HMODULE that contains this function
  static HMODULE GetModuleBaseAddress()
  {
   MEMORY_BASIC_INFORMATION mbi;
   return ((::VirtualQuery(GetModuleBaseAddress, &mbi, sizeof(mbi)) != 0) ?
   (HMODULE) mbi.AllocationBase : NULL);
  }

};
