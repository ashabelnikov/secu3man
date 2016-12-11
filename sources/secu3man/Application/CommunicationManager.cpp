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

/** \file CommunicationManager.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "CommunicationManager.h"
#include "io-core/CComPort.h"
#include "io-core/Bootloader.h"
#include "io-core/LogWriter.h"
#include "ISECU3Man.h"
#include "Settings/AppSettingsManager.h"
#include "Settings/ISettingsData.h"

CCommunicationManager::CCommunicationManager()
: m_pComPort(NULL)
, m_pBootLoader(NULL)
, m_pControlApp(NULL)
, m_recv_buff_size(4096)  //<--Note: some shitty drivers (e.g. SiLabs) don't accept large buffers
, m_send_buff_size(4096)
, m_pSettings(NULL)  //external!
, m_OnSettingsChanged(NULL)
, m_pAppAdapter(NULL)
, m_pBldAdapter(NULL)
{
 m_pSettings = ISECU3Man::GetSECU3Man()->GetAppSettingsManager()->GetSettings();

 m_pComPort    = new CComPort(_T("COM1"), m_recv_buff_size,m_send_buff_size);
 m_pBootLoader = new CBootLoader();
 m_pControlApp = new CControlApp();
 m_pAppAdapter = new CControlAppAdapter();
 m_pBldAdapter = new CBootLoaderAdapter();
}

CCommunicationManager::~CCommunicationManager()
{
 delete m_pComPort;
 delete m_pBootLoader;
 delete m_pControlApp;
 delete m_pAppAdapter;
 delete m_pBldAdapter;
}


bool CCommunicationManager::Terminate(void)
{
 //завершаем работу контроллеров интерфейсов
 m_pControlApp->Terminate();
 m_pBootLoader->Terminate();
 m_pComPort->Terminate();

 return true;
}


bool CCommunicationManager::Init(void)
{
 ASSERT(m_pSettings);  //ты забыл мен€ инициализировать!
 ASSERT(m_pBootLoader);
 ASSERT(m_pControlApp);
 ASSERT(m_pAppAdapter);
 ASSERT(m_pBldAdapter);

 bool status = true;

 m_pBootLoader->SetPlatformParameters(PlatformParamHolder(m_pSettings->GetECUPlatformType()));

 //останавливаем работу контроллеров интерфейсов, конкретный контроллер будет активирован
 //позже - в зависимости от текущего контекста программы
 m_pControlApp->SwitchOn(false);
 m_pBootLoader->SwitchOn(false);

 //дл€ того чтобы можно было применить новые настройки к порту, необходимо его заново открыть
 m_pComPort->Terminate();

 //Set protocol data encoding/decoding mode (Hex or Bin)
 m_pControlApp->SetProtocolDataMode(m_pSettings->GetHexDataMode());
 //Set parameters for speed sensor calculations
 m_pControlApp->SetNumPulsesPer1Km(m_pSettings->GetNumPulsesPer1Km());
 //Set clock frequency (16 or 20 mHz)
 m_pControlApp->SetQuartzFrq((EP_ATMEGA644==m_pSettings->GetECUPlatformType()) ? 20000000 : 16000000);

 try
 {
  //на скорость переданную в эту функцию не нужно обращать внимани€ (она будет установлена конкретным контроллером интрерфейса)
  m_pComPort->Initialize(_TSTRING(m_pSettings->GetPortName()), 9600, NOPARITY, ONESTOPBIT, false, true);
 }
 catch(CComPort::xInitialize e)
 {
  //Say to user that we was not able to open specified COM-port
  if (m_pSettings->GetCOMPortBother())
   AfxMessageBox(e.GetDetailStr());
  status = false;
 }

 m_pComPort->Purge();

 //Initialize interface controllers
 try
 {
  m_pControlApp->Initialize(m_pComPort,m_pSettings->GetBaudRateApplication(),500);
 }
 catch(CControlApp::xThread)
 {
  ASSERT(0); //because, strange situation!
  status = false;
 }

 try
 {
  m_pBootLoader->Initialize(m_pComPort,m_pSettings->GetBaudRateBootloader());
 }
 catch (CBootLoader::xThread)
 {
  ASSERT(0);
  status = false;
 }

 //Initialize communication controllers' adapters
 CWnd* pParent = AfxGetApp()->m_pMainWnd;
 VERIFY(m_pAppAdapter->Create(pParent));
 VERIFY(m_pBldAdapter->Create(pParent));

 //Link adapters with communication controllers
 m_pControlApp->SetEventHandler(m_pAppAdapter);
 m_pBootLoader->SetEventHandler(m_pBldAdapter);

 //оповещаем объект слушател€ об изменении настроек. ѕолучатель этого оповещени€ должен активировать
 //необходимый ему коммуникационный контроллер.
 if (m_OnSettingsChanged)
  m_OnSettingsChanged();

 return status;
}

//активирование указанного коммуникационного контроллера или деактивирование всех контроллеров
void CCommunicationManager::SwitchOn(size_t i_cntr, bool i_force_reinit  /* = false */)
{
 _ASSERTE(!(m_pControlApp->GetWorkState() && m_pBootLoader->GetWorkState()));

 switch(i_cntr)
 {
  case OP_ACTIVATE_APPLICATION:
   m_pBootLoader->SwitchOn(false, i_force_reinit);
   m_pControlApp->SwitchOn(true, i_force_reinit);
   break;

  case OP_ACTIVATE_BOOTLOADER:
   m_pControlApp->SwitchOn(false, i_force_reinit);
   m_pBootLoader->SwitchOn(true, i_force_reinit);
   break;

  case OP_DEACTIVATE_ALL: //deactivate all
   m_pControlApp->SwitchOn(false, i_force_reinit);
   m_pBootLoader->SwitchOn(false, i_force_reinit);
   break;

  default:
   _ASSERTE(0);
   break;
 }
}

void CCommunicationManager::setOnSettingsChanged(EventHandler i_OnSettingsChanged)
{
 m_OnSettingsChanged = i_OnSettingsChanged;
}
