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

/** \file InjDriverTipDlg.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "Resources/resource.h"
#include "InjDriverTipDlg.h"
#include <atlimage.h>
#include "ui-core/ImageUtils.h"

CInjDriverTipDlg::CInjDriverTipDlg(CWnd* pParent /*=NULL*/)
: Super(IDD_INJDRV_TIP_PICTURE, pParent)
{
 //empty
}

void CInjDriverTipDlg::DoDataExchange(CDataExchange* pDX)
{
 DDX_Control(pDX, IDC_INJDRV_TIP_PIC, m_tipPic);

 Super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CInjDriverTipDlg, Super)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInjDriverTipDlg message handlers

BOOL CInjDriverTipDlg::OnInitDialog()
{
 Super::OnInitDialog();

 CRect rc;
 m_tipPic.GetWindowRect(&rc);

 //stretch picture to fit window
 CImage im;
 LoadImageFromRes(AfxGetInstanceHandle(), (LPCTSTR)IDB_INJDRV_TIP_PIC, _T("JPG"), &im);
 CDC *screenDC = GetDC();
 CDC mDC;
 mDC.CreateCompatibleDC(screenDC);
 CBitmap bmp;
 bmp.CreateCompatibleBitmap(screenDC, rc.Width(), rc.Height());
 CBitmap *pob = mDC.SelectObject(&bmp);
 mDC.SetStretchBltMode(HALFTONE);
 im.StretchBlt(mDC.GetSafeHdc(), 0, 0, rc.Width(), rc.Height(), 0, 0, im.GetWidth(), im.GetHeight(), SRCCOPY);
 mDC.SelectObject(pob);
 m_tipPic.SetBitmap((HBITMAP)bmp.Detach());
 ReleaseDC(screenDC);
 
 return TRUE;  // return TRUE unless you set the focus to a control
}
