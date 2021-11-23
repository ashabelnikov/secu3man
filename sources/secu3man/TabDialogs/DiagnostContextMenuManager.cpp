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

/** \file DiagnostContextMenuManager.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "Resources/resource.h"
#include "DiagnostContextMenuManager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDiagnostContextMenuManager::CDiagnostContextMenuManager(CWnd* pWnd /*= NULL*/)
: m_pWnd(pWnd)
{
 //empty
}

CDiagnostContextMenuManager::~CDiagnostContextMenuManager()
{
 //empty
}

void CDiagnostContextMenuManager::Attach(CWnd* pWnd)
{
 ASSERT(pWnd);
 m_pWnd = pWnd;
}

//Fill menu with items and submenus
void CDiagnostContextMenuManager::CreateContent(void)
{
 m_ParentMenu.CreatePopupMenu();

 m_ParentMenu.AppendMenu(MF_STRING,IDM_DEV_DIAG_START_OUTAUTO_TST,MLL::LoadString(IDS_DEV_DIAG_START_OUTAUTO_TST));
 m_ParentMenu.AppendMenu(MF_STRING,IDM_DEV_DIAG_STOP_OUTAUTO_TST,MLL::LoadString(IDS_DEV_DIAG_STOP_OUTAUTO_TST));
 m_ParentMenu.AppendMenu(MF_SEPARATOR);
 m_ParentMenu.AppendMenu(MF_STRING,IDM_DEV_DIAG_ENABLE_BLDE_TST,MLL::LoadString(IDS_DEV_DIAG_ENABLE_BLDE_TST));
 m_ParentMenu.AppendMenu(MF_SEPARATOR);
 m_ParentMenu.AppendMenu(MF_STRING,IDM_DEV_DIAG_ENABLE_TACH_O_TST,MLL::LoadString(IDS_DEV_DIAG_ENABLE_TACH_O_TST));
}

//показывает контекстное меню
void CDiagnostContextMenuManager::TrackPopupMenu(int x, int y)
{
 ASSERT(m_pWnd);
 m_ParentMenu.TrackPopupMenu(TPM_LEFTALIGN,x,y,m_pWnd);
}

CMenu& CDiagnostContextMenuManager::GetParentMenu(void)
{
 return m_ParentMenu;
}