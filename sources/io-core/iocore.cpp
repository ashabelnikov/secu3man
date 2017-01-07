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

/** \file iocore.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include <afxdllx.h>

static AFX_EXTENSION_MODULE IocoreDLL = { NULL, NULL };

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
 // Remove this if you use lpReserved
 UNREFERENCED_PARAMETER(lpReserved);

 if (dwReason == DLL_PROCESS_ATTACH)
 {
  TRACE0("IOCORE.DLL Initializing!\n");

  // Extension DLL one-time initialization
  if (!AfxInitExtensionModule(IocoreDLL, hInstance))
   return 0;

  // Insert this DLL into the resource chain
  // NOTE: If this Extension DLL is being implicitly linked to by
  //  an MFC Regular DLL (such as an ActiveX Control)
  //  instead of an MFC application, then you will want to
  //  remove this line from DllMain and put it in a separate
  //  function exported from this Extension DLL.  The Regular DLL
  //  that uses this Extension DLL should then explicitly call that
  //  function to initialize this Extension DLL.  Otherwise,
  //  the CDynLinkLibrary object will not be attached to the
  //  Regular DLL's resource chain, and serious problems will
  //  result.

  new CDynLinkLibrary(IocoreDLL);
 }
 else if (dwReason == DLL_PROCESS_DETACH)
 {
  TRACE0("IOCORE.DLL Terminating!\n");
  // Terminate the library before destructors are called
  AfxTermExtensionModule(IocoreDLL);
 }
 return 1;   // ok
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
