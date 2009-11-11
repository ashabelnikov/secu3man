/****************************************************************
 *       SECU-3  - An open source, free engine control unit
 *    Designed by Alexey A. Shabelnikov. Ukraine, Gorlovka 2008.
 *       Microprocessors systems - design & programming.
 *    contacts:
 *              http://secu-3.narod.ru
 *              ICQ: 405-791-931
 ****************************************************************/

#include "stdafx.h"
#include <afxdllx.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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
