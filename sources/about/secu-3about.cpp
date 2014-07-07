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

#include "stdafx.h"
#include "resource.h"
#include "secu-3about.h"

#include "STCAbout.h"
#include "../common/unicodesupport.h"
#include "Version.h"

extern HINSTANCE hInstance;  //DLLs hInstance


namespace {
 CString BuildSoftwareInfoStr(void)
 {
  USES_CONVERSION;
  CString string;
  CString templ = MLL::LoadString(IDS_SOFTWARE_INFO);
  int major = 0, minor = 0;
  GetVersionInfo(NULL, major, minor);
  string.Format(templ, major, minor, A2T(__DATE__)); //compiler uses only ASCII format... 
  return string;
 }
}

void ABOUT_API DisplayAbout(CWnd* i_pParent)
{
 AboutStc(i_pParent, hInstance, (LPCTSTR)IDB_BITMAP0001, (LPCTSTR)IDR_RGN0001,
  BuildSoftwareInfoStr(), MLL::LoadString(IDS_AUTHOR_INFO));
}

//timeToshow - How long show splash screen, in milliseconds.
void ABOUT_API DisplaySplash(int timeToShow)
{
 AboutStc(CWnd::GetDesktopWindow(), hInstance, (LPCTSTR)IDB_BITMAP0001, (LPCTSTR)IDR_RGN0001,
  BuildSoftwareInfoStr(), MLL::LoadString(IDS_AUTHOR_INFO), true, timeToShow);
}
