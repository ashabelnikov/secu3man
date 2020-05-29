/* SECU-3  - An open source, free engine control unit
   Copyright (C) 2007 Alexey A. Shabelnikov. Ukraine, Kiev

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

   contacts:
              http://secu-3.org
              email: shabelnikov@secu-3.org
*/

/** \file FileDialogEx.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

class AFX_EXT_CLASS CFileDialogEx : public CFileDialog
{
  typedef CFileDialog Super;

 public:
  CFileDialogEx(BOOL bOpenFileDialog,
                LPCTSTR lpszDefExt = NULL,
                LPCTSTR lpszFileName = NULL,
                DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
                LPCTSTR lpszFilter = NULL,
                CWnd* pParentWnd = NULL,
                DWORD dwSize = 0);

  virtual ~CFileDialogEx();

  virtual INT_PTR DoModal();
};
