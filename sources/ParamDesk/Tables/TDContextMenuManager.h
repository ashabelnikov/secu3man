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

/** \file TDContextMenuManager.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include <vector>

class CTDContextMenuManager
{
 public:
  CTDContextMenuManager(CWnd* pWnd = NULL);
  virtual ~CTDContextMenuManager();

  void Attach(CWnd* pWnd);
  void CreateContent(void);
  void TrackPopupMenu(int x, int y);
  void TrackPopupMenuSave(int x, int y);
  void EnableMenuItems(bool i_enable);
  void SetLoadPopupMenuNames(const std::vector<_TSTRING>& i_names, int sep_index);
  void SetSavePopupMenuNames(const std::vector<_TSTRING>& i_names);

 private:
  CWnd* m_pWnd;        //Identifies the window that owns the pop-up menu
  CMenu m_ParentMenu;  //Top level menu
  CMenu m_LoadMenu;    //Popup menu for load tables
  CMenu m_SaveMenu;    //Popup menu for save tables
};
