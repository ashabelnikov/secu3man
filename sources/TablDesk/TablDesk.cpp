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

/** \file TablDesk.cpp
 * \author Alexey A. Shabelnikov
 * Defines the initialization routines for the DLL.
 */

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

//App. info
static volatile BYTE info[116] = {0x53,0x45,0x43,0x55,0x2d,0x33,0x20,0x4d,0x61,0x6e,0x61,0x67,0x65,0x72,0x2c,0x20,
                                  0x43,0x6f,0x70,0x79,0x72,0x69,0x67,0x68,0x74,0x20,0x28,0x43,0x29,0x20,0x32,0x30,
                                  0x30,0x37,0x20,0x41,0x6c,0x65,0x78,0x65,0x79,0x20,0x41,0x2e,0x20,0x53,0x68,0x61,
                                  0x62,0x65,0x6c,0x6e,0x69,0x6b,0x6f,0x76,0x2c,0x20,0x55,0x41,0x2c,0x20,0x4b,0x69,
                                  0x65,0x76,0x2c,0x20,0x68,0x74,0x74,0x70,0x3a,0x2f,0x2f,0x73,0x65,0x63,0x75,0x2d,
                                  0x33,0x2e,0x6f,0x72,0x67,0x2c,0x20,0x65,0x6d,0x61,0x69,0x6c,0x3a,0x20,0x73,0x68,
                                  0x61,0x62,0x65,0x6c,0x6e,0x69,0x6b,0x6f,0x76,0x40,0x73,0x65,0x63,0x75,0x2d,0x33,
                                  0x2e,0x6f,0x72,0x67};
