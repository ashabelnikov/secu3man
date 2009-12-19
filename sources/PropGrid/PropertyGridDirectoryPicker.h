////////////////////////////////////////////////////////////////////////////////
//
//               Copyright (C) 2001, nabocorp
//                     All Rights Reserved
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "common/unicodesupport.h"

class CPropertyGridDirectoryPicker
{
 public:
  CPropertyGridDirectoryPicker();
  virtual ~CPropertyGridDirectoryPicker();

  static bool PickDirectory(_TSTRING&, HWND hwnd);
  static _TSTRING m_strTitle;

 protected:
  static int CALLBACK BrowseCallbackProc(HWND hwnd,UINT uMsg,LPARAM lp, LPARAM pData);
};
