// XBrowseForFolder.h  Version 1.2
//
// Author:  Hans Dietrich
//          hdietrich@gmail.com
//
// This software is released into the public domain.  You are free to use
// it in any way you like, except that you may not sell this source code.
//
// This software is provided "as is" with no expressed or implied warranty.
// I accept no liability for any damage or loss of business that this
// software may cause.
//
///////////////////////////////////////////////////////////////////////////////


#ifdef XBROWSEFORFOLDER_EXPORTS
#define XBROWSEFORFOLDER_API __declspec(dllexport)
#else
#define XBROWSEFORFOLDER_API __declspec(dllimport)
#endif

#ifndef XBROWSEFORFOLDER_H
#define XBROWSEFORFOLDER_H

#define BIF_NEWDIALOGSTYLE    0x00000001
#define BIF_NONEWFOLDERBUTTON 0x00000002

XBROWSEFORFOLDER_API
BOOL XBrowseForFolder(HWND hWnd,
					  LPCTSTR lpszInitialFolder,
					  int nFolder,
					  LPCTSTR lpszCaption,
					  LPTSTR lpszBuf,
					  DWORD dwBufSize,
					  BOOL bEditBox = FALSE);

#endif //XBROWSEFORFOLDER_H
