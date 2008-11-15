// HexUtils.h : main header file for the HEXUTILS DLL
//

#if !defined(AFX_HEXUTILS_H__699460D0_16CC_48F2_810E_EA4E76821C30__INCLUDED_)
#define AFX_HEXUTILS_H__699460D0_16CC_48F2_810E_EA4E76821C30__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CHexUtilsApp
// See HexUtils.cpp for the implementation of this class
//

class CHexUtilsApp : public CWinApp
{
public:
	CHexUtilsApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHexUtilsApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CHexUtilsApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HEXUTILS_H__699460D0_16CC_48F2_810E_EA4E76821C30__INCLUDED_)
