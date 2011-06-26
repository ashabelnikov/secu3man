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

#include "common/FastDelegate.h"
#include "common/unicodesupport.h"

//Интерфейс для взаимодействия с панелью редактирования таблиц (представлением)
//Особенность: Любой из следующих методов интерфейса может быть вызван независимо
//от того - видна вкладка или нет (кроме SetCurSel()).

class ITablesDeskView
{
 public:
  typedef fastdelegate::FastDelegate0<> EventHandler;
  typedef fastdelegate::FastDelegate2<int, int> EventWith2Codes;
  typedef fastdelegate::FastDelegate1<int> EventWithCode;
  typedef fastdelegate::FastDelegate2<HWND, int> EventWithHWND;

  //Common
  virtual bool IsEnabled(void) = 0;                               //Возвращает true если представление разрешено
  virtual void Enable(bool enable) = 0;                           //разрешить/запретить представление
  virtual void Show(bool show) = 0;                               //показать/спрятать контент представления
  virtual void ShowSaveButton(bool i_show) = 0;                   //показать/cпрятать кнопку "сохранить"
  virtual void UpdateOpenedCharts(void) = 0;                      //update opened charts
  virtual void SetReadOnlyTablesSetName(bool readonly) = 0;       //enable/disable tables set's name edit box

  //events
  virtual void setOnMapChanged(EventWith2Codes OnFunction) = 0;   //обработцик будет вызываться при изменении в любой из таблиц
  virtual void setOnCloseMapWnd(EventWithHWND OnFunction) = 0;    //обработцик будет вызываться при закрытии окна с таблицей
  virtual void setOnOpenMapWnd(EventWithHWND OnFunction) = 0;     //обработцик будет вызываться при открытии окна с таблицей
  virtual void setOnTabActivate(EventHandler OnFunction) = 0;     //обработцик будет вызываться при появлении вкладки
  virtual void setOnSaveButton(EventHandler OnFunction) = 0;      //обработчик будет вызываться при нажатии кнопки "сохранить"
  virtual void setOnChangeTablesSetName(EventWithCode OnFunction) = 0;//обработчик будет вызываться при нажатии кнопки "сохранить"

  //Current selection of tab control. Вызывать только для активной панели!!!
  virtual bool SetCurSel(int sel) = 0;                            //выбрать указанную вкладку по идентификатору (идентификатор - индекс)
  virtual int GetCurSel(void) = 0;                                //получить идентификатор выбранной вкладки (идентификатор - индекс)

  //Data access
  virtual void SetTablesSetName(const _TSTRING& name) = 0;        //set name to edit box 
  virtual _TSTRING GetTablesSetName(void) const = 0;              //get name from edit box
  virtual float* GetMap(int i_mapType, bool i_original) = 0;      //r/w access to specified table  
};
