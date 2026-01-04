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

/** \file secu-3about.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "resource.h"
#include "secu-3about.h"
#include <algorithm>
#include <map>

#include "STCAbout.h"
#include "../common/unicodesupport.h"
#include "../common/ModuleName.h"
#include "Version.h"
#include "WelcomeDlg.h"

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

void ABOUT_API DisplayWelcome(void)
{
 CWelcomeDlg dialog;
 dialog.DoModal();
}

bool CheckAppTitle(CWnd* p_wnd)
{
 CString str;
 p_wnd->GetWindowText(str);
 TCHAR data[] = {'S','E','C','U','-','3',' ','M','a','n','a','g','e','r','\0'};
 if (str.Find(data) < 0)
 {
  CheckAppTitle(p_wnd);
  return false;
 }

 TCHAR ldstr[100+1];
 LoadString(GetModuleHandle(ModuleName::secu3man), 4152/*IDS_APP_TITLE*/, ldstr, 100);
 if (str.Find(data) < 0)
 {
  CheckAppTitle(p_wnd);
  return false;
 }

 return true;
}

bool ABOUT_API CheckAppLogo(void)
{
 TCHAR s[8+1];
 LoadString(GetModuleHandle(ModuleName::secu3man), 4153/*IDS_STATUS_BAR_LOGO*/, s, 8);
 if (s[0]!='S'||s[1]!='E'||s[2]!='C'||s[3]!='U'||s[4]!='-'||s[5]!='3')
  return false;

 return true;
}

bool ABOUT_API CheckAppUrl(const _TSTRING& str)
{
 if (NULL==_tcsstr(str.c_str(), _T("http://secu-3.org")))
  return false;

 return true;
}

bool ABOUT_API CheckVersion(void)
{
 std::vector<_TSTRING> mods;
 mods.push_back(ModuleName::about);
 mods.push_back(ModuleName::chartxd);
 mods.push_back(ModuleName::fwimpexp);
 mods.push_back(ModuleName::hexutils);
 mods.push_back(ModuleName::iocore);
 mods.push_back(ModuleName::midesk);
 mods.push_back(ModuleName::paramdesk);
 mods.push_back(ModuleName::tabldesk);
 mods.push_back(ModuleName::uicore);
 mods.push_back(ModuleName::secu3man);

 std::vector<BYTE> data;
 for (size_t i = 0; i < mods.size(); ++i)
 {
  if (!GetVersionData(mods[i].c_str(), data))
   return false;

  BYTE data1[] = {'S',0,'E',0,'C',0,'U',0,'-',0,'3',0,'.',0,'O',0,'R',0,'G',0};
  std::vector<BYTE>::iterator it = std::search(data.begin(), data.end(), data1, data1+20);
  if (it==data.end())
   return false;

  BYTE data2[] = {'S',0,'E',0,'C',0,'U',0,'-',0,'3',0,' ',0,'M',0,'a',0,'n',0,'a',0,'g',0,'e',0,'r',0};
  it = std::search(data.begin(), data.end(), data2, data2+28);
  if (it==data.end())
   return false;
 }

 return true;
}
