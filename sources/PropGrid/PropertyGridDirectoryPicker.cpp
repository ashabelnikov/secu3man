////////////////////////////////////////////////////////////////////////////////
//
//               Copyright (C) 2001, nabocorp
//                     All Rights Reserved
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "shlobj.h"
#include "PropertyGridDirectoryPicker.h"

#define BIF_NEWDIALOGSTYLE 0x0040

_TSTRING CPropertyGridDirectoryPicker::m_strTitle = _T("Choose a directory");

CPropertyGridDirectoryPicker::CPropertyGridDirectoryPicker()
{
}

CPropertyGridDirectoryPicker::~CPropertyGridDirectoryPicker()
{
}

int CALLBACK CPropertyGridDirectoryPicker::BrowseCallbackProc(HWND hwnd,UINT uMsg,LPARAM lp, LPARAM pData)
{
  switch(uMsg)
  {
    // If the dialog is being initialised
  case BFFM_INITIALIZED:
    {
      // Send a SetSelection message on the passed directory
      SendMessage(hwnd,BFFM_SETSELECTION,TRUE,pData);
      break;
    }
  }
  return 0;
}

bool CPropertyGridDirectoryPicker::PickDirectory(_TSTRING &directory, HWND hwnd)
{
  TCHAR pszBuffer[MAX_PATH];
  pszBuffer[0] = '\0';

  // Gets the Shell's default allocator
  LPMALLOC pMalloc;
  if (::SHGetMalloc(&pMalloc) == NOERROR)
  {
    BROWSEINFO bi;
    LPITEMIDLIST pidl;

    // Get help on BROWSEINFO struct
    bi.hwndOwner = hwnd;
    bi.pidlRoot = NULL;
    bi.pszDisplayName = pszBuffer;
    bi.lpszTitle = m_strTitle.c_str();
    bi.ulFlags = BIF_RETURNFSANCESTORS | BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE ;

    // The callback function initialises the dialog with the passed value
    bi.lpfn = BrowseCallbackProc;
    bi.lParam = LPARAM(directory.c_str());

    // This next call issues the dialog box.
    if ((pidl = ::SHBrowseForFolder(&bi)) != NULL)
    {
      // Get the full path into pszBuffer
      ::SHGetPathFromIDList(pidl, pszBuffer);
      // Free the PIDL allocated by SHBrowseForFolder.
      pMalloc->Free(pidl);
    }
    // Release the shell's allocator.
    pMalloc->Release();
  }

  // get the result
  if (_tcslen(pszBuffer) != 0)
  {
    directory = pszBuffer;
    return TRUE;
  }
  return FALSE;
}

