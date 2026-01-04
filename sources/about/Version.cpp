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

/** \file Version.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "version.h"
#include <vector>
#include "../common/Dll.h"

bool GetVersionInfo(LPCTSTR filename, int &major, int &minor)
{
 DWORD   verBufferSize;
 TCHAR   filenameBuff[2048+1];

 if (!filename)
 {
  DWORD size = GetModuleFileName(DLL::GetModuleHandle(), filenameBuff, 2048);
  if (!size || size==ERROR_INSUFFICIENT_BUFFER)
   return false;
  filename = filenameBuff;
 }

 //get the size of the version info block in the file and allocate memory for it
 verBufferSize = GetFileVersionInfoSize(filename, NULL);
 std::vector<BYTE> verBuffer(verBufferSize, 0);

 if(verBufferSize > 0)
 {
  //get the version block from the file
  if(TRUE == GetFileVersionInfo(filename, NULL, verBufferSize, &verBuffer[0]))
  {
   UINT length;
   VS_FIXEDFILEINFO *verInfo = NULL;

   //query the version information for neutral language
   if (TRUE == VerQueryValue(&verBuffer[0], _T("\\"), reinterpret_cast<LPVOID*>(&verInfo), &length))
   {
    //pull the version values.
    major = HIWORD(verInfo->dwProductVersionMS);
    minor = LOWORD(verInfo->dwProductVersionMS);
    return true;
   }
  }
 }
 return false;
}

bool GetProductVersion(int &major, int &minor)
{
 return GetVersionInfo(NULL, major, minor);
}

bool GetVersionData(LPCTSTR filename, std::vector<BYTE>& o_data)
{
 //get the size of the version info block in the file and allocate memory for it
 DWORD verBufferSize = GetFileVersionInfoSize(filename, NULL);
 std::vector<BYTE> verBuffer(verBufferSize, 0);

 if(verBufferSize > 0)
 {
  //get the version block from the file
  if(TRUE == GetFileVersionInfo(filename, NULL, verBufferSize, &verBuffer[0]))
  {
    o_data = verBuffer; //store result
    return true;
  }
 }
 return false;
}
