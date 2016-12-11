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

/** \file FirmwareContextMenuManager.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include <vector>

class CFirmwareModeContextMenuManager
{
 public:
  CFirmwareModeContextMenuManager(CWnd* pWnd = NULL);
  virtual ~CFirmwareModeContextMenuManager();

  void Attach(CWnd* pWnd);
  void CreateContent(void);
  void TrackPopupMenu(int x, int y, bool topAlign = true);
  void EnableBLMenuItems(bool i_enable);
  void EnableAppMenuItems(bool i_enable);

  //Надо вызывать когда окну приходит WM_INITMENUPOPUP
  void OnInitMenuPopup(CMenu* pMenu, UINT nIndex, BOOL bSysMenu);

 private:
  CWnd* m_pWnd;        //Identifies the window that owns the pop-up menu
  CMenu m_ParentMenu;  //Top level menu
  CMenu m_ImportMenu;
  CMenu m_ExportMenu;

  std::vector<UINT> m_bl_menu_items_IDs;
  std::vector<UINT> m_app_menu_items_IDs;
};
