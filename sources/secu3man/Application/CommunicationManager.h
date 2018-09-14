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

/** \file CommunicationManager.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include "common/FastDelegate.h"
#include "common/unicodesupport.h"
#include "io-core/BootLoader.h"
#include "io-core/bootloaderadapter.h"
#include "io-core/ccomport.h"
#include "io-core/ControlApp.h"
#include "io-core/controlappadapter.h"

class ISettingsData;

class CCommunicationManager
{
  typedef fastdelegate::FastDelegate0<> EventHandler;

 public:
  CCommunicationManager();
  virtual ~CCommunicationManager();

  bool Init(void);
  bool Terminate(void);
  void setOnSettingsChanged(EventHandler i_OnSettingsChanged);

  enum
  {
   OP_ACTIVATE_APPLICATION,
   OP_ACTIVATE_BOOTLOADER,
   OP_DEACTIVATE_ALL
  };
  //включение указанного коммуникационного контроллера. Если контроллер уже включен,
  //то функция ничего не делает. Для принудительной переинициализации контроллера необходимо
  //установить флаг i_force_reinit в true.
  void SwitchOn(size_t i_cntr, bool i_force_reinit = false);

  CControlAppAdapter* m_pAppAdapter;
  CControlApp*        m_pControlApp;
  CBootLoader*        m_pBootLoader;
  CBootLoaderAdapter* m_pBldAdapter;

 private:
  ISettingsData* m_pSettings;
  CComPort*      m_pComPort;

  const int m_recv_buff_size;
  const int m_send_buff_size;

  EventHandler m_OnSettingsChanged;
};
