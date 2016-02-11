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

/** \file CWelcomeDlg.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include <memory>
#include "common/UnicodeSupport.h"

class CLabel;

class CWelcomeDlg : public CDialog
{
  typedef CDialog Super;

 public:
  CWelcomeDlg(CWnd* pParent = NULL);   // standard constructor
 ~CWelcomeDlg();
  static const UINT IDD;

 // Implementation
 protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  virtual void OnOK();
  virtual BOOL OnInitDialog();
  DECLARE_MESSAGE_MAP()

  CStatic m_secu3orgPic;
  CBitmap m_secu3orgBmp;
  CStatic m_vkgPic;
  CBitmap m_vkgBmp;
  CStatic m_fbgPic;
  CBitmap m_fbgBmp;

 private:
  std::auto_ptr<CLabel> mp_secu3orgLink;
  std::auto_ptr<CLabel> mp_vkgLink;
  std::auto_ptr<CLabel> mp_fbgLink;
};
