/* SECU-3  - An open source, free engine control unit
   Copyright (C) 2007 Alexey A. Shabelnikov. Ukraine, Gorlovka

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
              http://secu-3.narod.ru
              email: secu-3@yandex.ru
*/

#pragma once

#include <memory>

class CChildView;
class CMainFrame;
class CMainTabManager;

class CChildViewManager  
{
 public:
  CChildViewManager();
  virtual ~CChildViewManager();

  bool Init(CMainFrame* i_pMainFrame);

  void OnAfterCreate(void);

  virtual bool OnClose(void);
  virtual bool OnAskFullScreen(void);
  virtual void OnFullScreen(bool i_what);

 private:
  std::auto_ptr<CMainTabManager> mp_MainTabManager;
  std::auto_ptr<CChildView> mp_wndView;
};
