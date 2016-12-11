
#pragma once

#ifdef _USRDLL
#undef _USRDLL        //CMake adds _USRDLL symbol to Extensions DLLs projects...
#pragma message("WARNING! _USRDLL symbol shouldn't be defined for Extension DLLs!")
#endif

#define VC_EXTRALEAN  // Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE classes
#include <afxodlgs.h>       // MFC OLE dialog classes
#include <afxdisp.h>        // MFC Automation classes
#endif // _AFX_NO_OLE_SUPPORT

#include <afxdtctl.h>       // MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>         // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

//------------------------------------------------------
#ifdef _DEBUG  //'identifier' : identifier was truncated in the debug information
  #pragma warning (disable: 4786)
#endif
