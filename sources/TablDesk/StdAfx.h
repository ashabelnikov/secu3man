
#pragma once

#ifdef _USRDLL
#undef _USRDLL //CMake adds _USRDLL symbol to Extensions DLLs projects...
#pragma message("WARNING! _USRDLL symbol shouldn't be defined for Extension DLLs!")
#endif

#define VC_EXTRALEAN  // Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>       // MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>         // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

//---------------------------------------------------------
#ifdef _DEBUG  //'identifier' : identifier was truncated in the debug information
 #pragma warning (disable: 4786)
#endif

//warning C4800: 'int' : forcing value to bool 'true' or 'false' (performance warning)
#pragma warning( disable : 4800 )

#include <atlconv.h>
#include "common\LangLayer.h"
