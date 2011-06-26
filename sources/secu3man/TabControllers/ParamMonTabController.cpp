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

#include "stdafx.h"
#include "Resources/resource.h"
#include "ParamMonTabController.h"

#include <map>
#include <algorithm>
#include "Application/CommunicationManager.h"
#include "common/FastDelegate.h"
#include "MainFrame/StatusBarManager.h"
#include "MIDesk/MIDeskDlg.h"
#include "MIDesk/RSDeskDlg.h"
#include "PMMonitorController.h"
#include "PMParamsController.h"
#include "PMTablesController.h"
#include "PMInitDataCollector.h"
#include "Settings/ISettingsData.h"
#include "TabDialogs/ParamMonTabDlg.h"

using namespace fastdelegate;
using namespace std;
using namespace SECU3IO;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define EHKEY _T("ParamMonCntr")

#pragma warning( disable : 4355 ) // : 'this' : used in base member initializer list

CParamMonTabController::CParamMonTabController(CParamMonTabDlg* i_view, CCommunicationManager* i_comm, CStatusBarManager* i_sbar, ISettingsData* ip_settings)
: m_view(i_view)
, m_comm(i_comm)
, m_sbar(i_sbar)
, mp_settings(ip_settings)
, mp_idccntr(new CPMInitDataCollector(i_comm, i_sbar))
, mp_parcntr(new CPMParamsController(i_view->mp_ParamDeskDlg.get(), i_comm, i_sbar, MakeDelegate(this, &CParamMonTabController::OnPDRequestsDataCollection)))
, mp_tabcntr(new CPMTablesController(i_view->mp_TablesDeskDlg.get(), i_comm, i_sbar))
, mp_moncntr(new CPMMonitorController(i_view->mp_MIDeskDlg.get(),i_view->mp_RSDeskDlg.get(), i_comm, i_sbar, ip_settings))
, m_current_state(m_state_machine.end())
{
 //сценарий: сбор данных-->чтение параметров-->мониторинг
 m_scenario1.push_back(mp_idccntr.get());
 m_scenario1.push_back(mp_parcntr.get());
 m_scenario1.push_back(mp_moncntr.get());

 //сценарий: сбор данных-->чтение таблиц-->чтение параметров-->мониторинг
 m_scenario2.push_back(mp_idccntr.get());
 m_scenario2.push_back(mp_tabcntr.get());
 m_scenario2.push_back(mp_parcntr.get());
 m_scenario2.push_back(mp_moncntr.get());

 //сценарий: чтение таблиц-->мониторинг
 m_scenario3.push_back(mp_tabcntr.get());
 m_scenario3.push_back(mp_moncntr.get());

 //сценарий: чтение параметров-->мониторинг
 m_scenario4.push_back(mp_parcntr.get());
 m_scenario4.push_back(mp_moncntr.get());

 //Устанавливаем обработчики событий от view
 m_view->setOnRawSensorsCheck(MakeDelegate(this, &CParamMonTabController::OnRawSensorsCheckBox));
 m_view->setOnEditTablesCheck(MakeDelegate(this, &CParamMonTabController::OnEditTablesCheckBox));
}

CParamMonTabController::~CParamMonTabController()
{
 //empty
}

void CParamMonTabController::StartScenario(const std::vector<CPMStateMachineState*>& scenario)
{
 m_state_machine = scenario;
 m_current_state = m_state_machine.begin();
 (*m_current_state)->StartDataCollection();
}

//изменились настройки программы!
void CParamMonTabController::OnSettingsChanged(void)
{
 //включаем необходимый для данного контекста коммуникационный контроллер
 m_comm->SwitchOn(CCommunicationManager::OP_ACTIVATE_APPLICATION, true);
 mp_moncntr->OnSettingsChanged();
 m_view->Invalidate();
}

void CParamMonTabController::OnPDRequestsDataCollection()
{
 StartScenario(m_scenario4);
}

//from MainTabController
void CParamMonTabController::OnActivate(void)
{
 //activate children controllers
 mp_moncntr->OnActivate();
 mp_moncntr->ShowRawSensors(m_view->GetRawSensorsCheckState());
 mp_parcntr->OnActivate();
 mp_tabcntr->OnActivate();

 //////////////////////////////////////////////////////////////////
 //Подключаем контроллер к потоку данных от SECU-3
 m_comm->m_pAppAdapter->AddEventHandler(this,EHKEY);
 m_comm->setOnSettingsChanged(MakeDelegate(this, &CParamMonTabController::OnSettingsChanged));
 //////////////////////////////////////////////////////////////////

 //включаем необходимый для данного контекста коммуникационный контроллер
 m_comm->SwitchOn(CCommunicationManager::OP_ACTIVATE_APPLICATION);

 //симулируем изменение состояния для обновления контроллов, так как OnConnection вызывается только если
 //сбрывается или разрывается принудительно (путем деактивации коммуникационного контроллера)
 OnConnection(m_comm->m_pControlApp->GetOnlineStatus());
}

//from MainTabController
void CParamMonTabController::OnDeactivate(void)
{
 m_comm->m_pAppAdapter->RemoveEventHandler(EHKEY);

 //deactivate children controllers
 mp_moncntr->OnDeactivate();
 mp_parcntr->OnDeactivate();
 mp_tabcntr->OnDeactivate();
 
 m_sbar->SetInformationText(_T(""));
}

void CParamMonTabController::OnPacketReceived(const BYTE i_descriptor, SECU3IO::SECU3Packet* ip_packet)
{
 //особый случай: пришел пакет с нотификацонным кодом
 if (i_descriptor == OP_COMP_NC)
 {
  const OPCompNc* p_ndata = (OPCompNc*)ip_packet;
  switch(p_ndata->opcode)
  {
   case OPCODE_EEPROM_PARAM_SAVE:
    m_sbar->SetInformationText(MLL::LoadString(IDS_PM_PARAMS_HAS_BEEN_SAVED));
    return;
   case OPCODE_NEW_TABLSET_SELECTED:
    if (true==m_view->GetEditTablesCheck())
      StartScenario(m_scenario3); //пользователь выбрал новый набор таблиц: нужно заново прочитать таблицы
    mp_tabcntr->Enable(false);
    mp_tabcntr->InvalidateCache();
    return;
  }
 }

 //Обработка машины состояний
 if (m_current_state != m_state_machine.end() && (*m_current_state)->CollectData(i_descriptor, ip_packet))
 {
  //Завершился сбор данных
  if ((*m_current_state) == mp_idccntr.get())
  {
   m_view->EnableEditTablesCheck((mp_idccntr->GetFWOptions() & (1 << 9)) > 0);
   mp_parcntr->SetFunctionsNames(mp_idccntr->GetFNNames());  
  }
  //Завершилось чтение параметров
  else if ((*m_current_state) == mp_parcntr.get())
  {
   //конфигурация прочитана - можно разрешать панель параметров
   mp_parcntr->Enable(m_comm->m_pControlApp->GetOnlineStatus());
  }
  //Завершилось чтение таблиц
  else if ((*m_current_state) == mp_tabcntr.get())
  {
   mp_tabcntr->Enable(m_comm->m_pControlApp->GetOnlineStatus());  
  }
  
  //переходим к следующему состоянию
  if ((m_current_state + 1) != m_state_machine.end())
  {
   ++m_current_state;
   (*m_current_state)->StartDataCollection();
  }
 }
}

//состояние подключения изменилось!
void CParamMonTabController::OnConnection(const bool i_online)
{
 int state;
 ASSERT(m_sbar);

 if (i_online) //перешли в онлайн
 {
  state = CStatusBarManager::STATE_ONLINE;
  mp_tabcntr->InvalidateCache(); //<--делаем закешированные таблицы невалидными
  StartScenario(m_view->GetEditTablesCheck() ? m_scenario2 : m_scenario1);
 }
 else
 {
  state = CStatusBarManager::STATE_OFFLINE;
 }

 //здесь мы можем только запрещать панели, а разрешать их будем только тогда, когда прочитана конфигурация
 if (i_online==false) 
 {
  mp_moncntr->Enable(i_online);
  mp_tabcntr->Enable(i_online);
  mp_parcntr->Enable(i_online);
 }

 m_sbar->SetConnectionState(state);
}

//Событие от чекбокса переключающего режим мониторинга (приборы/сырые данные)
void CParamMonTabController::OnRawSensorsCheckBox(void)
{
 mp_moncntr->ShowRawSensors(m_view->GetRawSensorsCheckState());
}

void CParamMonTabController::OnEditTablesCheckBox(void)
{
 //Если пользователь нажал кнопку и данные в кеше контроллера невалидные (устарели)
 //то запускаем процесс чтения таблиц
 if (true==m_view->GetEditTablesCheck() && false==mp_tabcntr->IsValidCache())
  StartScenario(m_scenario3);
}

bool CParamMonTabController::OnClose(void)
{
 return true; //отвечаем что данная вкладка готова к закрытию приложения
}

bool CParamMonTabController::OnAskFullScreen(void)
{
 return true; //отвечаем что полноэкранный режим для этой вкладки разрешен
}

void CParamMonTabController::OnFullScreen(bool i_what, const CRect& i_rect)
{
 //При включении полноэкранного режима ресайзим окно вкладки так чтобы оно было
 //поверх таб контрола. При выключении полноэкранного режима таб контрол сам ресайзит
 //вкладку к нужному размеру.

 if (i_what)
  m_view->MoveWindow(i_rect.left, i_rect.top, i_rect.Width(), i_rect.Height());

 m_view->MakePDFloating(i_what);
 m_view->EnlargeMonitor(i_what);
}
