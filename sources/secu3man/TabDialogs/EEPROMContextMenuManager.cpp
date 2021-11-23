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

/** \file EEPROMContextMenuManager.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "Resources/resource.h"
#include "EEPROMContextMenuManager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEEPROMContextMenuManager::CEEPROMContextMenuManager(CWnd* pWnd /*= NULL*/)
: m_pWnd(pWnd)
{
 //empty
}

CEEPROMContextMenuManager::~CEEPROMContextMenuManager()
{
 //empty
}

void CEEPROMContextMenuManager::Attach(CWnd* pWnd)
{
 ASSERT(pWnd);
 m_pWnd = pWnd;
}

//Fill menu with items and submenus
void CEEPROMContextMenuManager::CreateContent(void)
{
 m_ParentMenu.CreatePopupMenu();
 m_ParentMenu.AppendMenu(MF_STRING,IDM_EE_READ_EEPROM, MLL::LoadString(IDS_EE_READ_EEPROM));
 m_bl_menu_items_IDs.push_back(IDM_EE_READ_EEPROM);

 m_ParentMenu.AppendMenu(MF_STRING,IDM_EE_WRITE_EEPROM, MLL::LoadString(IDS_EE_WRITE_EEPROM));
 m_bl_menu_items_IDs.push_back(IDM_EE_WRITE_EEPROM);

 m_ParentMenu.AppendMenu(MF_SEPARATOR);
 m_ParentMenu.AppendMenu(MF_STRING,IDM_EE_OPEN_EEPROM, MLL::LoadString(IDS_EE_OPEN_EEPROM));
 m_ParentMenu.AppendMenu(MF_STRING,IDM_EE_SAVE_EEPROM, MLL::LoadString(IDS_EE_SAVE_EEPROM));

 m_ParentMenu.AppendMenu(MF_SEPARATOR);
 m_ParentMenu.AppendMenu(MF_STRING,IDM_EE_LOAD_GRIDS, MLL::LoadString(IDS_EE_LOAD_GRIDS));

 m_ParentMenu.AppendMenu(MF_STRING, IDM_EE_RESET_EEPROM, MLL::LoadString(IDS_RESET_EEPROM));
 m_bl_menu_items_IDs.push_back(IDM_EE_RESET_EEPROM);
}

//показывает контекстное меню
void CEEPROMContextMenuManager::TrackPopupMenu(int x, int y, bool topAlign /*= false*/)
{
 ASSERT(m_pWnd);
 m_ParentMenu.TrackPopupMenu(TPM_LEFTALIGN | (topAlign ? TPM_TOPALIGN : TPM_BOTTOMALIGN),x,y,m_pWnd);
}

void CEEPROMContextMenuManager::EnableBLMenuItems(bool i_enable)
{
 for(size_t i = 0; i < m_bl_menu_items_IDs.size(); i++)
 {
  UINT state = i_enable ? MF_ENABLED : MF_GRAYED;
  m_ParentMenu.EnableMenuItem(m_bl_menu_items_IDs[i],state);
 }
}
