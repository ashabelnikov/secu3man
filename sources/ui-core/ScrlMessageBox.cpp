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

/** \file ScrlMessageBox.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "resource.h"
#include "ScrlMessageBox.h"
#include "common/GDIHelpers.h"
#include "common/DPIAware.h"

BEGIN_MESSAGE_MAP(CScrlMessageBox, Super)
 ON_WM_PAINT()
 ON_WM_SIZE()
 ON_WM_GETMINMAXINFO()
END_MESSAGE_MAP()

void CScrlMessageBox::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);
 DDX_Control(pDX, IDC_SCRLMSGBOX_EDIT, m_edit);
}

CScrlMessageBox::CScrlMessageBox(CWnd* pParentWnd, const _TSTRING& caption, const _TSTRING& text, LPCSTR icon)
: Super(IDD_SCRLMSGBOX, pParentWnd)
, m_caption(caption)
, m_text(text)
, m_icon(icon)
, m_initialized(false)
{
 m_hIcon = LoadIcon(NULL, m_icon);
}

BOOL CScrlMessageBox::OnInitDialog()
{
 Super::OnInitDialog();

 SetWindowText(m_caption.c_str());
 m_edit.SetWindowText(m_text.c_str());

 CRect rc;
 GetWindowRect(&rc);
 m_createSize = rc.Size();

 m_initialized = true;

 GetClientRect(&rc);
 _AlignControls(rc.Width(), rc.Height());

 UpdateData(FALSE);
 return TRUE;  // return TRUE unless you set the focus to a control
}

void CScrlMessageBox::OnPaint() 
{
 CPaintDC dc(this); // device context for painting

 dc.DrawIcon(0, 5, m_hIcon);
}

void CScrlMessageBox::OnSize(UINT nType, int cx, int cy)
{
 Super::OnSize(nType, cx, cy);

 if (m_initialized)
 {
  _AlignControls(cx, cy);
 }
}

void CScrlMessageBox::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
 if (m_initialized)
 {
  lpMMI->ptMinTrackSize.x = m_createSize.cx;
  lpMMI->ptMinTrackSize.y = m_createSize.cy;

  lpMMI->ptMaxTrackSize.x = m_createSize.cx;
  lpMMI->ptMaxTrackSize.y = (LONG)(m_createSize.cy * 1.6f);
 }
}

void CScrlMessageBox::_AlignControls(int cx, int cy)
{
 DPIAware dpi;

 CRect rcok = GDIHelpers::GetChildWndRect(GetDlgItem(IDOK));
 rcok.MoveToY(cy - rcok.Height() - dpi.ScaleY(3));
 GetDlgItem(IDOK)->MoveWindow(rcok);

 CRect rced = GDIHelpers::GetChildWndRect(&m_edit);
 rced.bottom = rcok.top - dpi.ScaleY(3);
 m_edit.MoveWindow(rced);
}
