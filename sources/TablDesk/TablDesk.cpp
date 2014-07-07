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

// TablDesk.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include <afxdllx.h>

static AFX_EXTENSION_MODULE TablDeskDLL = { NULL, NULL };

void RegisterOwnClasses(HINSTANCE hInstance)
{
 WNDCLASS wc;

 // Get the info for this class. #32770 is the default class name for dialogs boxes.
 ::GetClassInfo(hInstance, _T("#32770"), &wc);

 // Change the name of the class.
 wc.lpszClassName = _T("SECU-3GridModeEditWnd");

 // Register this class so that MFC can use it.
 AfxRegisterClass(&wc);	
}

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
 UNREFERENCED_PARAMETER(lpReserved);

 if (dwReason == DLL_PROCESS_ATTACH)
 {
  RegisterOwnClasses(hInstance);

  TRACE0("TABLDESK.DLL Initializing!\n");
  if (!AfxInitExtensionModule(TablDeskDLL, hInstance))
   return 0;
  new CDynLinkLibrary(TablDeskDLL);
 }
 else if (dwReason == DLL_PROCESS_DETACH)
 {
  TRACE0("TABLDESK.DLL Terminating!\n");
  AfxTermExtensionModule(TablDeskDLL);
 }
 return 1;
}

