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

#include "stdafx.h"
#include "ToolTipCtrlEx.h"

std::list<CToolTipCtrlEx*> CToolTipCtrlEx::m_insts;
bool CToolTipCtrlEx::m_activated_all = true;

CToolTipCtrlEx::CToolTipCtrlEx()
: m_activated(false)
{
 m_insts.insert(m_insts.begin(), this);
}

CToolTipCtrlEx::~CToolTipCtrlEx()
{
 m_insts.remove(this);
}

bool CToolTipCtrlEx::AddWindow(CWnd* pWnd, const _TSTRING& text)
{
 TOOLINFO ti;
 ti.cbSize = sizeof (TOOLINFO);
 ti.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
 ti.hwnd = pWnd->GetParent ()->GetSafeHwnd ();
 ti.uId = (UINT) pWnd->GetSafeHwnd ();
 ti.hinst = AfxGetInstanceHandle ();
 ti.lpszText = const_cast<TCHAR*>(text.c_str());

 return (bool) SendMessage (TTM_ADDTOOL, 0, (LPARAM) &ti);
}

bool CToolTipCtrlEx::AddRectangle(CWnd* pWnd, const _TSTRING& text, LPCRECT pRect, UINT nIDTool)
{
 TOOLINFO ti;
 ti.cbSize = sizeof (TOOLINFO);
 ti.uFlags = TTF_SUBCLASS;
 ti.hwnd = pWnd->GetSafeHwnd ();
 ti.uId = nIDTool;
 ti.hinst = AfxGetInstanceHandle ();
 ti.lpszText = const_cast<TCHAR*>(text.c_str());
 ::CopyRect (&ti.rect, pRect);

 return (bool) SendMessage (TTM_ADDTOOL, 0, (LPARAM) &ti);
}

/*static*/ void CToolTipCtrlEx::ActivateAllTooltips(bool i_activate)
{
 m_activated_all = i_activate;
 std::list<CToolTipCtrlEx*>::iterator it;
 for(it = m_insts.begin(); it != m_insts.end(); ++it)
 {
  (*it)->Activate((*it)->m_activated && m_activated_all);
 }
}

void CToolTipCtrlEx::ActivateToolTips(bool i_activate)
{
 m_activated = i_activate;
 Activate(i_activate && m_activated_all);
}
