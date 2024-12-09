/* Names of modules
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

/** \file ModuleName.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include <shlwapi.h>
#include "UnicodeSupport.h"

namespace ModuleName {

 static LPCTSTR about     =  _T("about.dll");
 static LPCTSTR chartxd   =  _T("chartxd.dll");
 static LPCTSTR fwimpexp  =  _T("fwimpexp.dll");
 static LPCTSTR hexutils  =  _T("hexutils.dll");
 static LPCTSTR iocore    =  _T("io-core.dll");
 static LPCTSTR midesk    =  _T("midesk.dll");
 static LPCTSTR paramdesk =  _T("paramdesk.dll");
 static LPCTSTR tabldesk  =  _T("tabldesk.dll");
 static LPCTSTR uicore    =  _T("ui-core.dll");
 static LPCTSTR secu3man  =  _T("secu3man.exe");

 static _TSTRING GetExecDirPath(const _TSTRING& file = _T(""))
 {
  TCHAR szDirectory[MAX_PATH] = _T("");
  //get path where resides executable file of this proces
  HMODULE hModule = GetModuleHandle(NULL);
  ASSERT(hModule);
  GetModuleFileName(hModule, szDirectory, MAX_PATH-1);
  VERIFY(PathRemoveFileSpec(szDirectory));
  _TSTRING path = szDirectory;   
  if (path.size() && file.size())
  {
   if (path[path.size()-1] != _T('\\')) //if root dir, then '\' already exist
    path+=_T("\\");
  }
  return path + file;  
 }

 static void AddSlashToPath(_TSTRING& path)
 {
  if (path.size())
  {
   TCHAR last_char = path[path.size()-1];
   if (last_char != _T('\\')) //if root dir, then '\' already exist
    path+=_T("\\");
  }
 }

} //ns
