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

/** \file ChildView.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "Resources/resource.h"
#include "ChildView.h"
#include "ui-core/TabDialog.h"
#include "about/secu-3about.h"

#define MAIN_TAB_CTRL_BITMAPS_COLOR_MASK RGB(192,192,192)

namespace {
void* align4(void* ptr)
{
 return reinterpret_cast<void*>((reinterpret_cast<ULONG>(ptr) + 3) & ~3);
}
}

/////////////////////////////////////////////////////////////////////////////
// CChildView

CChildView::CChildView()
: m_pImgList(NULL)
{
 //empty
}

CChildView::~CChildView()
{
 if (m_pImgList)
 {
  delete m_pImgList;
  m_pImgList = NULL;
 }
}

BEGIN_MESSAGE_MAP(CChildView, Super)
 ON_WM_SIZE( )
END_MESSAGE_MAP()

void CChildView::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);
 DDX_Control(pDX, IDC_MAIN_TAB_CTRL, m_tab_control);
}

void CChildView::OnSize(UINT nType, int cx, int cy)
{
 Super::OnSize(nType, cx, cy);
 if (::IsWindow(m_tab_control.m_hWnd))
 {
  m_tab_control.MoveWindow(0,0, cx, cy); //resize tab control
  m_tab_control.UpdateWindow();
 }
}

//Creates dialog from template stored in memory
bool CChildView::Create(CWnd* ip_parent)
{
 //========================================================
 if (!CheckAbout() || !CheckAppMenu()) delete this;
 //========================================================

 HGLOBAL hgbl;
 LPDLGTEMPLATE lpdt; LPDLGITEMTEMPLATE lpdi; 
 LPWORD lpw;
 LPWSTR lpwsz;
 int nchar;

 hgbl = GlobalAlloc(GMEM_ZEROINIT, 1024);
 ASSERT(hgbl);
 lpdt = (LPDLGTEMPLATE)GlobalLock(hgbl);

 // Define a dialog box.
 lpdt->style = WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | DS_SETFONT | DS_CONTROL | WS_CLIPCHILDREN;
 lpdt->dwExtendedStyle = WS_EX_CONTROLPARENT | WS_EX_CLIENTEDGE;
 lpdt->cdit = 1; // number of controls
 lpdt->x  = 0;   //<--any default sizes,
 lpdt->y  = 0;   //framework will then set correct sizes.
 lpdt->cx = 0;
 lpdt->cy = 0;

 lpw = (LPWORD) (lpdt + 1);
 *lpw++ = 0;   // no menu
 *lpw++ = 0;   // predefined dialog box class (by default)

 lpwsz = (LPWSTR) lpw;
 nchar = MultiByteToWideChar (CP_ACP, 0, "View", -1, lpwsz, 50);
 lpw   += nchar;

 *lpw++ = 8; //font size

 lpwsz = (LPWSTR) lpw;
 nchar = MultiByteToWideChar (CP_ACP, 0, "MS Sans Serif", -1, lpwsz, 100);
 lpw   += nchar;

 //Define a tab control
 lpdi = reinterpret_cast<DLGITEMTEMPLATE*>(align4(lpw));
 lpdi->style = WS_CHILD | WS_TABSTOP | WS_VISIBLE | TCS_BOTTOM | TCS_OWNERDRAWFIXED;
 lpdi->dwExtendedStyle = 0;
 lpdi->x = 0;
 lpdi->y = 0;
 lpdi->cx = 0;
 lpdi->cy = 0;
 lpdi->id = IDC_MAIN_TAB_CTRL;

 lpw = reinterpret_cast<WORD*>(lpdi + 1);
 //class
 lpwsz = (LPWSTR) lpw;
 nchar = MultiByteToWideChar (CP_ACP, 0, WC_TABCONTROLA, -1, lpwsz, 100);
 lpw   += nchar;
 //title
 lpwsz = (LPWSTR) lpw;
 nchar = MultiByteToWideChar (CP_ACP, 0, "TabCtrl", -1, lpwsz, 100);
 lpw   += nchar;
 *lpw++ = 0;          // No creation data

 GlobalUnlock(hgbl);
 BOOL result = CreateIndirect((LPDLGTEMPLATE) hgbl, ip_parent);
 GlobalFree(hgbl);

 //Init image list
 m_pImgList = new CImageList();
 m_pImgList->Create(16, 16, ILC_COLOR24|ILC_MASK, 0, 0);
 CBitmap bitmap;
 bitmap.LoadBitmap((LPCTSTR)IDB_MAIN_TAB_CTRL_BITMAPS);
 m_pImgList->Add(&bitmap, MAIN_TAB_CTRL_BITMAPS_COLOR_MASK);

 //Tune a tab control
 m_tab_control.SetResourceModule(::GetModuleHandle(NULL));
 m_tab_control.SetImageList(m_pImgList);
 return result;
}

void CChildView::AddView(const _TSTRING& title, CTabDialog* ip_dlg, const int nImage)
{
 ASSERT(ip_dlg);
 m_tab_control.AddPage(title.c_str(), ip_dlg, nImage);
}

void CChildView::SetEventListener(ITabControllerEvent* ip_listener)
{
 ASSERT(ip_listener);
 m_tab_control.SetEventListener(ip_listener);
}

int CChildView::GetCurSel(void) const
{
 return m_tab_control.GetCurSel();
}

void CChildView::SetCurSel(int index)
{
 m_tab_control.SetCurSel(index);
}

void CChildView::SetFocus(bool curPage /*=false*/)
{
 if (!curPage)
  m_tab_control.SetFocus();
 else
 {
  CTabDialog* p_dlg = m_tab_control.GetCurrentPage();
  if (p_dlg)
   p_dlg->SetFocus();
 }
}

void CChildView::EnableTabControl(bool i_enable)
{
 m_tab_control.EnableWindow(i_enable);
}

void CChildView::EnableTab(int idx, bool i_enable)
{
 m_tab_control.EnableItem(idx, i_enable);
}
