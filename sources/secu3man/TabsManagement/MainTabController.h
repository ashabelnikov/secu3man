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

#include <vector>
#include "ui-core\ITabControllerEvent.h"

class CTabController;
class ITabController;

class CMainTabController  : public ITabControllerEvent
{
 public:
  CMainTabController();
  virtual ~CMainTabController();

  //добавляет контроллер
  void AddTabController(ITabController* i_pCntr);

  //возвращает список контроллеров
  std::vector<ITabController*>& GetControllersList(void);

  //Привязка к Tab-контролу
  void SetTabController(CTabController* i_pTabController);

  //возвращает контроллер активной вкладки
  virtual ITabController* GetActiveController() const;

 private:
  //ITabControllerEvent imp.
  virtual void OnSelchangeTabctl(void);
  virtual void OnSelchangingTabctl(void);

 private:
  std::vector<ITabController*> m_controllers_list;
  CTabController* m_pTabController;
};
