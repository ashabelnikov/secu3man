
/****************************************************************
 *
 *  Created by Alexey A. Shabelnikov. Ukraine, Gorlovka 2009. 
 *   ICQ: 405-791-931. e-mail: shabelnikov-stc@mail.ru
 *  Microprocessors systems - design & programming.
 *
 *****************************************************************/

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
