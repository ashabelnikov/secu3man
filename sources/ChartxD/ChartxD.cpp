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

/** \file ChartxD.cpp
 * \author Alexey A. Shabelnikov
 */

#include <windows.h>
#include <TeEngine.hpp>

#pragma hdrstop

HINSTANCE hInst = NULL;

enum ELanguage
{
 IL_ENGLISH = 0,
 IL_RUSSIAN = 1
};

extern "C"
{
 void  __declspec(dllexport)  __cdecl ChartxDSetLanguage(int i_language);
 void  __declspec(dllexport)  __cdecl ChartxDShowHints(int i_show);
}

//---------------------------------------------------------------------------
void __cdecl ChartxDSetLanguage(int i_language)
{
 switch(i_language)
 {
  case IL_ENGLISH:
   ::SetThreadLocale(MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT));
   break;
  case IL_RUSSIAN:
   ::SetThreadLocale(MAKELCID(MAKELANGID(LANG_RUSSIAN, SUBLANG_ENGLISH_US), SORT_DEFAULT));
   break;
 }
}

//---------------------------------------------------------------------------
void __cdecl ChartxDShowHints(int i_show)
{
 Application->ShowHint = i_show;
}

//---------------------------------------------------------------------------
#pragma argsused
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fwdreason, LPVOID lpvReserved)
{
 hInst = hinstDLL;
 return 1;
}
