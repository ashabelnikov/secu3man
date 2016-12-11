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

/** \file PMParamsController.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include <vector>
#include "common/FastDelegate.h"
#include "common/ObjectTimer.h"
#include "PMControllerBase.h"

class CCommunicationManager;
class CParamDeskDlg;
class CStatusBarManager;

class CPMParamsController : public CPMControllerBase<CParamDeskDlg>
{
  typedef CPMControllerBase<VIEW> Super;
  typedef fastdelegate::FastDelegate0<> EventHandler;

 public:
  CPMParamsController(VIEW* ip_view, CCommunicationManager* ip_comm, CStatusBarManager* ip_sbar, EventHandler RequestDataCollection);
  virtual ~CPMParamsController();

  //начало работы контроллера
  virtual void OnActivate(void);

  //конец работы контроллера
  virtual void OnDeactivate(void);

  virtual void Enable(bool state);

  virtual void StartDataCollection(void);

  //Чтение всех необходимых данных после коннекта (конечный автомат)
  //возвращает true когда все данные прочитаны  
  virtual bool CollectData(const BYTE i_descriptor, const void* i_packet_data);

  void SetFunctionsNames(const std::vector<_TSTRING>& i_names);

  //применение опций прошивки
  void ApplyFWOptions(DWORD opt);

 private:
  //Events from view
  void OnParamDeskTabActivate(void);
  void OnParamDeskChangeInTab(void);
  void OnPDSaveButton(void);
  //from m_pd_changes_timer
  void OnParamDeskChangesTimer(void);

  CCommunicationManager* mp_comm;
  CStatusBarManager* mp_sbar;

  CObjectTimer<CPMParamsController> m_pd_changes_timer; 
  EventHandler m_RequestDataCollection;
  bool m_parameters_changed;       //этот флаг устанавливается при изменении на любой из вкладок параметров и сбрасывается после посылки измененных данных в SECU
  int  m_lastSel;                  //хранит номер вкладки панели параметров которая была выбрана в последний раз
};
