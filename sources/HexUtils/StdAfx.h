// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#pragma once

#define VC_EXTRALEAN  // Exclude rarely-used stuff from Windows headers

#include <windows.h>

#if (_MSC_VER <1300)
 #pragma warning( disable : 4786 ) // : identifier was truncated to '255' characters in the debug information
#endif
