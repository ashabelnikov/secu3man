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

/** \file FirmwareContextMenuManager.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "Resources/resource.h"
#include "FirmwareContextMenuManager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFirmwareModeContextMenuManager::CFirmwareModeContextMenuManager(CWnd* pWnd /*= NULL*/)
: m_pWnd(pWnd)
{
 //empty
}

CFirmwareModeContextMenuManager::~CFirmwareModeContextMenuManager()
{
 //empty
}

void CFirmwareModeContextMenuManager::Attach(CWnd* pWnd)
{
 ASSERT(pWnd);
 m_pWnd = pWnd;
}

//Fill menu with items and submenus
void CFirmwareModeContextMenuManager::CreateContent(void)
{
 ///////////////////////////////////////////////////////////////////////////
 //Popup menu "»мпорт данных"
 m_ImportMenu.CreatePopupMenu();
 m_ImportMenu.AppendMenu(MF_STRING,IDM_IMPORT_IMPORT_FROM_MPSZ,MLL::LoadString(IDS_IMPORT_IMPORT_FROM_MPSZ));
 m_ImportMenu.AppendMenu(MF_STRING,IDM_IMPORT_IMPORT_FROM_SECU3,MLL::LoadString(IDS_IMPORT_IMPORT_FROM_SECU3));
 m_ImportMenu.AppendMenu(MF_STRING,IDM_IMPORT_IMPORT_FROM_S3F,MLL::LoadString(IDS_IMPORT_IMPORT_FROM_S3F));
 m_ImportMenu.AppendMenu(MF_SEPARATOR);
 m_ImportMenu.AppendMenu(MF_STRING,IDM_IMPORT_FW_DATA_FROM_ANOTHER_FW,MLL::LoadString(IDS_IMPORT_FW_DATA_FROM_ANOTHER_FW));
 m_ImportMenu.AppendMenu(MF_STRING,IDM_IMPORT_FW_DATA_FROM_SECU3,MLL::LoadString(IDS_IMPORT_FW_DATA_FROM_SECU3));
 m_ImportMenu.AppendMenu(MF_SEPARATOR);
 m_ImportMenu.AppendMenu(MF_STRING,IDM_IMPORT_DEF_PARAMS_FROM_EEPROM_FILE, MLL::LoadString(IDS_IMPORT_DEF_PARAMS_FROM_EEPROM_FILE));
 m_ImportMenu.AppendMenu(MF_STRING,IDM_IMPORT_TABLES_FROM_EEPROM_FILE, MLL::LoadString(IDS_IMPORT_TABLES_FROM_EEPROM_FILE));

 ///////////////////////////////////////////////////////////////////////////
 //Popup menu "Ёкспорт данных"
 m_ExportMenu.CreatePopupMenu();
 m_ExportMenu.AppendMenu(MF_STRING,IDM_EXPORT_EXPORT_TO_MPSZ,MLL::LoadString(IDS_EXPORT_EXPORT_TO_MPSZ));
 m_ExportMenu.AppendMenu(MF_STRING,IDM_EXPORT_EXPORT_TO_SECU3,MLL::LoadString(IDS_EXPORT_EXPORT_TO_SECU3));
 m_ExportMenu.AppendMenu(MF_STRING,IDM_EXPORT_EXPORT_TO_S3F,MLL::LoadString(IDS_EXPORT_EXPORT_TO_S3F));
 ///////////////////////////////////////////////////////////////////////////
 m_ParentMenu.CreatePopupMenu();

 m_ParentMenu.AppendMenu(MF_STRING,IDM_READ_FLASH,MLL::LoadString(IDS_READ_FLASH));
 m_ParentMenu.AppendMenu(MF_STRING,IDM_WRITE_FLASH,MLL::LoadString(IDS_WRITE_FLASH));
 m_ParentMenu.AppendMenu(MF_STRING,IDM_OPEN_FLASH,MLL::LoadString(IDS_OPEN_FLASH));
 m_ParentMenu.AppendMenu(MF_STRING,IDM_SAVE_FLASH,MLL::LoadString(IDS_SAVE_FLASH));

 ///////////////////////////////////////////////////////////////////////////
 m_ParentMenu.AppendMenu(MF_SEPARATOR);
 m_ParentMenu.AppendMenu(MF_POPUP | MF_STRING,(UINT)m_ImportMenu.m_hMenu,MLL::LoadString(IDS_IMPORT_POPUP_MENU));
 m_ParentMenu.AppendMenu(MF_POPUP | MF_STRING,(UINT)m_ExportMenu.m_hMenu,MLL::LoadString(IDS_EXPORT_POPUP_MENU));
 ///////////////////////////////////////////////////////////////////////////
 m_ParentMenu.AppendMenu(MF_SEPARATOR);

 m_ParentMenu.AppendMenu(MF_STRING,IDM_READ_FLASH_TO_FILE,MLL::LoadString(IDS_READ_FLASH_TO_FILE));
 m_bl_menu_items_IDs.push_back(IDM_READ_FLASH_TO_FILE);

 m_ParentMenu.AppendMenu(MF_STRING,IDM_WRITE_FLASH_FROM_FILE,MLL::LoadString(IDS_WRITE_FLASH_FROM_FILE));
 m_bl_menu_items_IDs.push_back(IDM_WRITE_FLASH_FROM_FILE);

 m_ParentMenu.AppendMenu(MF_SEPARATOR);

 m_ParentMenu.AppendMenu(MF_STRING,IDM_READ_EEPROM_TO_FILE,MLL::LoadString(IDS_READ_EEPROM_TO_FILE));
 m_bl_menu_items_IDs.push_back(IDM_READ_EEPROM_TO_FILE);

 m_ParentMenu.AppendMenu(MF_STRING,IDM_WRITE_EEPROM_FROM_FILE,MLL::LoadString(IDS_WRITE_EEPROM_FROM_FILE));
 m_bl_menu_items_IDs.push_back(IDM_WRITE_EEPROM_FROM_FILE);

 m_ParentMenu.AppendMenu(MF_STRING,IDM_RESET_EEPROM, MLL::LoadString(IDS_RESET_EEPROM));
 m_app_menu_items_IDs.push_back(IDM_RESET_EEPROM);

 m_ParentMenu.AppendMenu(MF_SEPARATOR);

 m_ParentMenu.AppendMenu(MF_STRING,IDM_READ_BOOTLOADER_SIGNATURE,MLL::LoadString(IDS_READ_BOOTLOADER_SIGNATURE));
 m_bl_menu_items_IDs.push_back(IDM_READ_BOOTLOADER_SIGNATURE);

 m_ParentMenu.AppendMenu(MF_STRING,IDM_READ_FW_SIGNATURE_INFO,MLL::LoadString(IDS_READ_FW_SIGNATURE_INFO));
 m_app_menu_items_IDs.push_back(IDM_READ_FW_SIGNATURE_INFO);

 m_ParentMenu.AppendMenu(MF_SEPARATOR);

 m_ParentMenu.AppendMenu(MF_STRING,IDM_FIRMWARE_MASTER,MLL::LoadString(IDS_FIRMWARE_MASTER));
}

//показывает контекстное меню
void CFirmwareModeContextMenuManager::TrackPopupMenu(int x, int y, bool topAlign /*= true*/)
{
 ASSERT(m_pWnd);
 m_ParentMenu.TrackPopupMenu(TPM_LEFTALIGN | (topAlign ? TPM_TOPALIGN : TPM_BOTTOMALIGN),x,y,m_pWnd);
}

void CFirmwareModeContextMenuManager::EnableBLMenuItems(bool i_enable)
{
 for(size_t i = 0; i < m_bl_menu_items_IDs.size(); i++)
 {
  UINT state = i_enable ? MF_ENABLED : MF_GRAYED;
  m_ParentMenu.EnableMenuItem(m_bl_menu_items_IDs[i],state);
 }
}

void CFirmwareModeContextMenuManager::EnableAppMenuItems(bool i_enable)
{
 for(size_t i = 0; i < m_app_menu_items_IDs.size(); i++)
 {
  UINT state = i_enable ? MF_ENABLED : MF_GRAYED;
  m_ParentMenu.EnableMenuItem(m_app_menu_items_IDs[i],state);
 }
}
