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

/** \file WelcomeDlg.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "resource.h"
#include "WelcomeDlg.h"
#include "ui-core/Label.h"
#include "common/Dll.h"

const UINT CWelcomeDlg::IDD = IDD_WELCOME;

/////////////////////////////////////////////////////////////////////////////
// CWelcomeDlg dialog

BEGIN_MESSAGE_MAP(CWelcomeDlg, Super)
END_MESSAGE_MAP()

CWelcomeDlg::CWelcomeDlg(CWnd* pParent /*=NULL*/)
: Super(CWelcomeDlg::IDD, pParent)
, mp_secu3orgLink(new CLabel)
, mp_vkgLink(new CLabel)
, mp_fbgLink(new CLabel)
{
 //empty
}

CWelcomeDlg::~CWelcomeDlg()
{
 //empty
}

void CWelcomeDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);
 DDX_Control(pDX, IDC_W_SECU3ORG_LINK, *mp_secu3orgLink);
 DDX_Control(pDX, IDC_W_VKG_LINK, *mp_vkgLink);
 DDX_Control(pDX, IDC_W_FBG_LINK, *mp_fbgLink);
 DDX_Control(pDX, IDC_BITMAP0002, m_secu3orgPic);
 DDX_Control(pDX, IDC_BITMAP0003, m_vkgPic);
 DDX_Control(pDX, IDC_BITMAP0004, m_fbgPic);
}

/////////////////////////////////////////////////////////////////////////////
// CWelcomeDlg message handlers

void CWelcomeDlg::OnOK()
{
 Super::OnOK();
}

BOOL CWelcomeDlg::OnInitDialog()
{
 Super::OnInitDialog();

 //init HTTP link
 mp_secu3orgLink->SetLink(true);
 mp_secu3orgLink->SetTextColor(RGB(0, 0, 255));
 mp_secu3orgLink->SetFontUnderline(true);

 mp_secu3orgLink->SetLinkCursor((HCURSOR)LoadImage(DLL::GetModuleHandle(), MAKEINTRESOURCE(IDC_CURSOR_HAND), IMAGE_CURSOR, 0, 0, LR_SHARED));

 mp_vkgLink->SetLink(true);
 mp_vkgLink->SetTextColor(RGB(0, 0, 255));
 mp_vkgLink->SetFontUnderline(true);
 mp_vkgLink->SetLinkCursor((HCURSOR)LoadImage(DLL::GetModuleHandle(), MAKEINTRESOURCE(IDC_CURSOR_HAND), IMAGE_CURSOR, 0, 0, LR_SHARED));

 mp_fbgLink->SetLink(true);
 mp_fbgLink->SetTextColor(RGB(0, 0, 255));
 mp_fbgLink->SetFontUnderline(true);
 mp_fbgLink->SetLinkCursor((HCURSOR)LoadImage(DLL::GetModuleHandle(), MAKEINTRESOURCE(IDC_CURSOR_HAND), IMAGE_CURSOR, 0, 0, LR_SHARED));

 m_secu3orgBmp.LoadBitmap(IDB_BITMAP0002);

 //set bitmap background
 CDC memDC;
 memDC.CreateCompatibleDC(GetDC());
 memDC.SelectObject(&m_secu3orgBmp);
 BITMAP bmp;
 m_secu3orgBmp.GetBitmap(&bmp);
 COLORREF bk_color = GetSysColor(COLOR_BTNFACE);
 for(int i = 0; i < bmp.bmWidth; ++i)
 {
  for(int j = 0; j < bmp.bmHeight; ++j)
  {
   COLORREF p = memDC.GetPixel(i,j);
   if (p > RGB(245,245,245)) //background color of original image is white
    memDC.SetPixel(i,j, bk_color);
  }
 }

 m_secu3orgPic.SetBitmap(m_secu3orgBmp);
 m_vkgBmp.LoadBitmap(IDB_BITMAP0003);
 m_vkgPic.SetBitmap(m_vkgBmp);
 m_fbgBmp.LoadBitmap(IDB_BITMAP0004);
 m_fbgPic.SetBitmap(m_fbgBmp);

 UpdateData(FALSE);
 return TRUE;  // return TRUE unless you set the focus to a control
}
