// TablDesk.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include <afxdllx.h>

static AFX_EXTENSION_MODULE TablDeskDLL = { NULL, NULL };

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
 UNREFERENCED_PARAMETER(lpReserved);

 if (dwReason == DLL_PROCESS_ATTACH)
 {
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

