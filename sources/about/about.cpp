/* SECU-3  - An open source, free engine control unit
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
              email: shabelnikov@secu-3.org
*/

#include "stdafx.h"
#include <afxdllx.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

HINSTANCE hInstance;

static AFX_EXTENSION_MODULE AboutDLL = { NULL, NULL };

extern "C" int APIENTRY
DllMain(HINSTANCE i_hInstance, DWORD dwReason, LPVOID lpReserved)
{
 // Remove this if you use lpReserved
 UNREFERENCED_PARAMETER(lpReserved);

 hInstance = i_hInstance;

 if (dwReason == DLL_PROCESS_ATTACH)
 {
  TRACE0("ABOUT.DLL Initializing!\n");
		
  // Extension DLL one-time initialization
  if (!AfxInitExtensionModule(AboutDLL, hInstance))
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

  new CDynLinkLibrary(AboutDLL);
 }
 else if (dwReason == DLL_PROCESS_DETACH)
 {
  TRACE0("ABOUT.DLL Terminating!\n");
  // Terminate the library before destructors are called
  AfxTermExtensionModule(AboutDLL);
 }
 return 1;   // ok
}
