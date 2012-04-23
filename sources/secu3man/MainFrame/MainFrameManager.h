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
              http://secu-3.org
              email: shabelnikov@secu-3.org
*/

#pragma once

class CChildViewManager;
class CMainFrame;
class CStatusBarManager;
class MainFrameController;

class CMainFrameManager
{
 public:
  CMainFrameManager();
  virtual ~CMainFrameManager();

  //Создает пустое главное окно и возвращает указатель не него
  CMainFrame* GreateMainWindow(void);

  //инициализирует содержимое окна и дочерние контроллеры
  bool Init(CWnd* &o_pMainWnd);

  //возвращает менеджер статусной строки (в нижней части главного окна)
  CStatusBarManager* GetStatusBarManager(void) const;

 private:
  CMainFrame* m_pMainFrame;	//view
  MainFrameController* m_pMainFrameController; //controller
  CChildViewManager* m_pChildViewManager;
  CStatusBarManager* m_pStatusBarManager;
};
