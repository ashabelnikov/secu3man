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

/** \file TDContextMenuManager.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "Resources/resource.h"
#include "TDContextMenuManager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTDContextMenuManager::CTDContextMenuManager(CWnd* pWnd /*= NULL*/)
: m_pWnd(pWnd)
{
 //empty
}

CTDContextMenuManager::~CTDContextMenuManager()
{
 //empty
}

void CTDContextMenuManager::Attach(CWnd* pWnd)
{
 ASSERT(pWnd);
 m_pWnd = pWnd;
}

//Fill menu with items and submenus
void CTDContextMenuManager::CreateContent(void)
{
 ///////////////////////////////////////////////////////////////////////////
 //Popup menu "Загрузить из"
 m_LoadMenu.CreatePopupMenu();
 ///////////////////////////////////////////////////////////////////////////
 //Popup menu "Сохранить в"
 m_SaveMenu.CreatePopupMenu();
 ///////////////////////////////////////////////////////////////////////////
 m_ParentMenu.CreatePopupMenu();

 ///////////////////////////////////////////////////////////////////////////
 m_ParentMenu.AppendMenu(MF_POPUP | MF_STRING, (UINT)m_LoadMenu.m_hMenu, MLL::LoadString(IDS_TD_LOAD_TABLES_POPUP_MENU));
 m_ParentMenu.AppendMenu(MF_POPUP | MF_STRING, (UINT)m_SaveMenu.m_hMenu, MLL::LoadString(IDS_TD_SAVE_TABLES_POPUP_MENU));
 ///////////////////////////////////////////////////////////////////////////

 m_ParentMenu.AppendMenu(MF_STRING, IDM_TD_IMPORT_FROM_S3F, MLL::LoadString(IDS_TD_IMPORT_FROM_S3F));
 m_ParentMenu.AppendMenu(MF_STRING, IDM_TD_EXPORT_TO_S3F, MLL::LoadString(IDS_TD_EXPORT_TO_S3F));
}

//show context menu
void CTDContextMenuManager::TrackPopupMenu(int x, int y)
{
 ASSERT(m_pWnd);
 m_ParentMenu.TrackPopupMenu(TPM_LEFTALIGN, x, y, m_pWnd);
}

void CTDContextMenuManager::TrackPopupMenuSave(int x, int y)
{
 ASSERT(m_pWnd);
 m_SaveMenu.TrackPopupMenu(TPM_LEFTALIGN, x, y, m_pWnd);
}

void CTDContextMenuManager::EnableMenuItems(bool i_enable)
{
 UINT state = i_enable ? MF_ENABLED : MF_GRAYED;
 size_t i, count = m_LoadMenu.GetMenuItemCount();
 for(i = 0; i < count; i++)
  m_LoadMenu.EnableMenuItem(m_LoadMenu.GetMenuItemID(i), state);
 count = m_SaveMenu.GetMenuItemCount();
 for(i = 0; i < count; i++)
  m_SaveMenu.EnableMenuItem(m_SaveMenu.GetMenuItemID(i), state);
}

void CTDContextMenuManager::SetLoadPopupMenuNames(const std::vector<_TSTRING>& i_names, int sep_index)
{
 size_t i, count = m_LoadMenu.GetMenuItemCount();
 for(i = 0; i < count; ++i)
  m_LoadMenu.DeleteMenu(IDM_TD_LOAD_NAMES_RESERVED0 + i, MF_BYCOMMAND);
 m_LoadMenu.DeleteMenu(0, MF_BYPOSITION); //remove separator
 
 for(i = 0; i < i_names.size(); ++i)
 {
  if (i == sep_index) //check when we need to add separator
   m_LoadMenu.AppendMenu(MF_SEPARATOR);
  m_LoadMenu.AppendMenu(MF_STRING, IDM_TD_LOAD_NAMES_RESERVED0 + i, i_names[i].c_str());
 }
}

void CTDContextMenuManager::SetSavePopupMenuNames(const std::vector<_TSTRING>& i_names)
{
 size_t i, count = m_SaveMenu.GetMenuItemCount();
 for(i = 0; i < count; ++i)
  m_SaveMenu.DeleteMenu(IDM_TD_SAVE_NAMES_RESERVED0 + i, MF_BYCOMMAND);
 
 for(i = 0; i < i_names.size(); ++i)
  m_SaveMenu.AppendMenu(MF_STRING, IDM_TD_SAVE_NAMES_RESERVED0 + i, i_names[i].c_str());
}
