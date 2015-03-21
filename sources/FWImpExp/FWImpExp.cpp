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

/** \file FWImpExp.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include <afxdllx.h>

static AFX_EXTENSION_MODULE FWImpExpDLL = { NULL, NULL };

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
 UNREFERENCED_PARAMETER(lpReserved);

 if (dwReason == DLL_PROCESS_ATTACH)
 {
  TRACE0("FWIMPEXP.DLL Initializing!\n");

  if (!AfxInitExtensionModule(FWImpExpDLL, hInstance))
   return 0;

  new CDynLinkLibrary(FWImpExpDLL);
 }
 else if (dwReason == DLL_PROCESS_DETACH)
 {
  TRACE0("FWIMPEXP.DLL Terminating!\n");
  AfxTermExtensionModule(FWImpExpDLL);
 }
 return 1;
}
