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

/** \file IParamDeskView.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include "common/FastDelegate.h"
#include "common/unicodesupport.h"
#include "IDeskView.h"

//This interface intended for communication with panel of parameters.
//Each of these methods can be called separatelly and even if panel is not visible (except SetCurSel()).
//Type of data and type of tab is described by descriptor.

class IParamDeskView : public IDeskView
{
 public:
  virtual bool SetValues(BYTE i_descriptor, const void* i_values) = 0;  //put data into specified tab
  virtual bool GetValues(BYTE i_descriptor, void* o_values) = 0;        //get data from specified tab
  virtual bool LockUIUpdate(BYTE i_descriptor, bool lock) = 0;
  virtual void ShowSaveButton(bool i_show) = 0;

  //установка/получение имен семейств характеристик (для вкладки FunSet...)
  //вектор содержит набор имен семейств характеристик хранимых в прошивке SECU-3
  virtual void SetFunctionsNames(const std::vector<_TSTRING>& i_names) = 0;
  virtual const std::vector<_TSTRING>& GetFunctionsNames(void) = 0;

  virtual void EnableIgnitionCogs(bool enable) = 0;
  virtual void EnableUseVentPwm(bool enable) = 0;
  virtual void EnableUseCTSCurveMap(bool enable) = 0;
  virtual void EnableHallOutputParams(bool enable) = 0;
  virtual void SetMaxCylinders(int number) = 0;
  virtual void EnableOddCylinders(bool enable) = 0;
  virtual void EnableSECU3TItems(bool i_enable) = 0;
  virtual void EnableExtraIO(bool i_enable) = 0;
  virtual void EnableChokeTesting(bool i_enable) = 0;
  virtual void EnableChokeManPos(bool i_enable) = 0;
  virtual void EnableCKPSItems(bool i_enable) = 0;
  virtual void EnableBTNameAndPass(bool i_enable) = 0;
  virtual void EnableInputsMerging(bool i_enable) = 0;
  virtual void EnableFuelInjection(bool i_enable) = 0;
  virtual void EnableLambda(bool i_enable) = 0;
  virtual void EnableGasdoseTesting(bool i_enable) = 0;
  virtual void EnableGasdoseManPos(bool i_enable) = 0;
  virtual void EnableGasdose(bool i_enable) = 0;
  virtual void EnableChoke(bool i_enable) = 0;
  virtual void EnableHallWndWidth(bool enable) = 0;
  virtual void EnableChokeCtrls(bool i_enable) = 0;

  virtual BYTE GetCurrentDescriptor(void) = 0; //obtains descriptor corresponding to current (displaying) tab

  virtual void SetOnTabActivate(EventHandler OnTabActivate) = 0; //Will be called when tab become active
  virtual void SetOnChangeInTab(EventHandler OnChangeInTab) = 0; //обработчик будет вызываться при изменении пользователем данных вкладки
  virtual void SetOnSaveButton(EventHandler OnSaveButton) = 0; //обработчик будет вызываться при нажатии кнопки "сохранить"

  //call only for active panel
  virtual bool SetCurSel(int sel) = 0;
  virtual int GetCurSel(void) = 0;
};
