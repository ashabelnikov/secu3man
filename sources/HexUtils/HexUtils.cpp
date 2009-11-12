/****************************************************************
 *
 *  Created by Alexey A. Shabelnikov. Ukraine, Gorlovka 2005. 
 *   ICQ: 405-791-931. e-mail: shabelnikov-stc@mail.ru
 *  Microprocessors systems - design & programming.
 *
 *****************************************************************/

// HexUtils.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "HexUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

/////////////////////////////////////////////////////////////////////////////
// CHexUtilsApp

BEGIN_MESSAGE_MAP(CHexUtilsApp, CWinApp)
 //empty
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHexUtilsApp construction

CHexUtilsApp::CHexUtilsApp()
{
 //empty
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CHexUtilsApp object

CHexUtilsApp theApp;
