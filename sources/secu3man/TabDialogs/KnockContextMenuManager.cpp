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

/** \file KnockContextMenuManager.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "Resources/resource.h"
#include "KnockContextMenuManager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CKnockContextMenuManager::CKnockContextMenuManager(CWnd* pWnd /*= NULL*/)
: m_pWnd(pWnd)
{
 //empty
}

CKnockContextMenuManager::~CKnockContextMenuManager()
{
 //empty
}

void CKnockContextMenuManager::Attach(CWnd* pWnd)
{
 ASSERT(pWnd);
 m_pWnd = pWnd;
}

//Fill menu with items and submenus
void CKnockContextMenuManager::CreateContent(void)
{
 m_ParentMenu.CreatePopupMenu();
 m_ParentMenu.AppendMenu(MF_STRING,IDM_KC_LIST_RESET_POINTS,MLL::LoadString(IDS_KC_LIST_RESET_POINTS));
 m_ParentMenu.AppendMenu(MF_STRING, IDM_KC_LIST_NEIGHBOUR_MIDDLE, MLL::LoadString(IDS_KC_LIST_NEIGHBOUR_MIDDLE));
 m_ParentMenu.AppendMenu(MF_STRING, IDM_KC_LIST_SIGMA_FILTER, MLL::LoadString(IDS_KC_LIST_SIGMA_FILTER));
 m_ParentMenu.AppendMenu(MF_SEPARATOR);
 m_ParentMenu.AppendMenu(MF_STRING,IDM_KC_LIST_LOAD_POINTS,MLL::LoadString(IDS_KC_LIST_LOAD_POINTS));
 m_ParentMenu.AppendMenu(MF_STRING,IDM_KC_LIST_SAVE_POINTS,MLL::LoadString(IDS_KC_LIST_SAVE_POINTS));
}

//показывает контекстное меню
void CKnockContextMenuManager::TrackPopupMenu(int x, int y)
{
 ASSERT(m_pWnd);
 m_ParentMenu.TrackPopupMenu(TPM_LEFTALIGN,x,y,m_pWnd);
}
