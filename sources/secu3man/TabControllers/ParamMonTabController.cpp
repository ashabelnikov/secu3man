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

/** \file ParamMonTabController.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "Resources/resource.h"
#include "ParamMonTabController.h"

#include <algorithm>
#include <map>
#include "about/secu-3about.h"
#include "Application/CommunicationManager.h"
#include "common/FastDelegate.h"
#include "io-core/bitmask.h"
#include "io-core/FirmwareMapsDataHolder.h"
#include "MainFrame/StatusBarManager.h"
#include "MIDesk/MIDeskDlg.h"
#include "MIDesk/RSDeskDlg.h"
#include "ParamDesk/Tables/TablesDeskDlg.h"
#include "PMInitDataCollector.h"
#include "PMMonitorController.h"
#include "PMParamsController.h"
#include "PMTablesController.h"
#include "Settings/ISettingsData.h"
#include "TabDialogs/ParamMonTabDlg.h"

using namespace fastdelegate;
using namespace SECU3IO;

#define EHKEY _T("ParamMonCntr")

//We only save value of this pointer (we do not access members), so we can ignore warning.
#pragma warning( disable : 4355 ) // : 'this' : used in base member initializer list

CParamMonTabController::CParamMonTabController(CParamMonTabDlg* ip_view, CCommunicationManager* ip_comm, CStatusBarManager* ip_sbar, ISettingsData* ip_settings)
: mp_view(ip_view)
, mp_comm(ip_comm)
, mp_sbar(ip_sbar)
, mp_settings(ip_settings)
, mp_idccntr(new CPMInitDataCollector(ip_comm, ip_sbar))
, mp_moncntr(new CPMMonitorController(ip_view->mp_MIDeskDlg.get(),ip_view->mp_RSDeskDlg.get(), ip_view->mp_CEDeskDlg.get(), ip_view->mp_TablesDeskDlg.get(), ip_comm, ip_sbar, ip_settings))
, mp_parcntr(new CPMParamsController(ip_view->mp_ParamDeskDlg.get(), ip_comm, ip_sbar, MakeDelegate(this, &CParamMonTabController::OnPDRequestsDataCollection)))
, mp_tabcntr(new CPMTablesController(ip_view->mp_TablesDeskDlg.get(), ip_comm, ip_sbar))
, m_current_state(m_state_machine.end())
{
 //=================================================================
 if (!CheckVersion() || !CheckAppMenu() || !CheckAbout() || !CheckAppLogo())
  delete this;
 //=================================================================

 //сценарий: [сбор данных]-->[чтение параметров]-->[мониторинг]
 m_scenario1.push_back(mp_idccntr.get());
 m_scenario1.push_back(mp_parcntr.get());
 m_scenario1.push_back(mp_moncntr.get());

 //сценарий: [сбор данных]-->[чтение таблиц]-->[чтение параметров]-->[мониторинг]
 m_scenario2.push_back(mp_idccntr.get());
 m_scenario2.push_back(mp_tabcntr.get());
 m_scenario2.push_back(mp_parcntr.get());
 m_scenario2.push_back(mp_moncntr.get());

 //сценарий: [чтение таблиц]-->[мониторинг]
 m_scenario3.push_back(mp_tabcntr.get());
 m_scenario3.push_back(mp_moncntr.get());

 //сценарий: [чтение параметров]-->[мониторинг]
 m_scenario4.push_back(mp_parcntr.get());
 m_scenario4.push_back(mp_moncntr.get());

 //сценарий: [сбор данных]-->[мониторинг]
 m_scenario5.push_back(mp_idccntr.get());
 m_scenario5.push_back(mp_moncntr.get());

 //Устанавливаем обработчики событий от view
 mp_view->setOnRawSensorsCheck(MakeDelegate(this, &CParamMonTabController::OnRawSensorsCheckBox));
 mp_view->setOnEditTablesCheck(MakeDelegate(this, &CParamMonTabController::OnEditTablesCheckBox));
}

CParamMonTabController::~CParamMonTabController()
{
 //empty
}

//изменились настройки программы!
void CParamMonTabController::OnSettingsChanged(void)
{
 //включаем необходимый для данного контекста коммуникационный контроллер
 mp_comm->SwitchOn(CCommunicationManager::OP_ACTIVATE_APPLICATION, true);
 mp_moncntr->OnSettingsChanged();
 ConfigureIndicators();
 mp_view->Invalidate();
}

//from MainTabController
void CParamMonTabController::OnActivate(void)
{
 //activate children controllers
 mp_moncntr->OnActivate();
 mp_moncntr->ShowRawSensors(mp_view->GetRawSensorsCheckState());
 mp_parcntr->OnActivate();
 mp_tabcntr->InvalidateCache(); //<--делаем закешированные таблицы невалидными
 mp_tabcntr->OnActivate();

 //////////////////////////////////////////////////////////////////
 //Подключаем контроллер к потоку данных от SECU-3
 mp_comm->m_pAppAdapter->AddEventHandler(this,EHKEY);
 mp_comm->setOnSettingsChanged(MakeDelegate(this, &CParamMonTabController::OnSettingsChanged));
 //////////////////////////////////////////////////////////////////

 //включаем необходимый для данного контекста коммуникационный контроллер
 mp_comm->SwitchOn(CCommunicationManager::OP_ACTIVATE_APPLICATION);

 //симулируем изменение состояния для обновления контроллов, так как OnConnection вызывается только если
 //сбрывается или разрывается принудительно (путем деактивации коммуникационного контроллера)
 OnConnection(mp_comm->m_pControlApp->GetOnlineStatus());

 ConfigureIndicators();
 mp_view->EnableMakingChartsChildren(mp_settings->GetChildCharts());
}

//from MainTabController
void CParamMonTabController::OnDeactivate(void)
{
 mp_comm->m_pAppAdapter->RemoveEventHandler(EHKEY);

 //deactivate children controllers
 mp_moncntr->OnDeactivate();
 mp_parcntr->OnDeactivate();
 mp_tabcntr->OnDeactivate();
 
 mp_sbar->SetInformationText(_T(""));
}

void CParamMonTabController::OnPacketReceived(const BYTE i_descriptor, SECU3IO::SECU3Packet* ip_packet)
{
 //особый случай: пришел пакет с нотификацонным кодом
 if (i_descriptor == OP_COMP_NC)
 {
  const OPCompNc* p_ndata = (OPCompNc*)ip_packet;
  switch(p_ndata->opcode)
  {
   case OPCODE_EEPROM_PARAM_SAVE: //параметры были сохранены
    mp_sbar->SetInformationText(MLL::LoadString(IDS_PM_PARAMS_HAS_BEEN_SAVED));
    return;
   case OPCODE_SAVE_TABLSET:      //таблицы были сохранены
    mp_sbar->SetInformationText(MLL::LoadString(IDS_PM_TABLSET_HAS_BEEN_SAVED));    
    _StartScenario(m_scenario5);
    return;
   case OPCODE_LOAD_TABLSET:      //SECU-3 tells us that new tables set has been selected:
    {
     //пользователь выбрал новый набор таблиц (в параметрах): нужно заново прочитать таблицы
     //также, он мог загрузить один из наборов таблиц через меню в окне редактора таблиц, в этом случае
     //мы тоже попадем сюда
     if (true==mp_view->GetEditTablesCheckState())
      _StartScenario(m_scenario3);
     mp_tabcntr->Enable(false);
     mp_tabcntr->InvalidateCache();
    }
    return;
  }
 }

 //Обработка машины состояний
 if (m_current_state != m_state_machine.end() && (*m_current_state)->CollectData(i_descriptor, ip_packet))
 {
  //Завершился сбор данных
  if ((*m_current_state) == mp_idccntr.get())
  {
   mp_view->EnableEditTablesCheck(CHECKBIT32(mp_idccntr->GetFWOptions(), COPT_REALTIME_TABLES));
   mp_parcntr->SetFunctionsNames(mp_idccntr->GetFNNames()); 
   mp_parcntr->ApplyFWOptions(mp_idccntr->GetFWOptions());
   mp_moncntr->ApplyFWOptions(mp_idccntr->GetFWOptions());
   mp_tabcntr->ApplyFWOptions(mp_idccntr->GetFWOptions());

   if (CHECKBIT32(mp_idccntr->GetFWOptions(), COPT_REALTIME_TABLES))
   {
    std::vector<_TSTRING> fwnames(mp_idccntr->GetFNNames().begin(), mp_idccntr->GetFNNames().begin() + TABLES_NUMBER);
    std::vector<_TSTRING> eenames(mp_idccntr->GetFNNames().begin() + TABLES_NUMBER, mp_idccntr->GetFNNames().end());
    mp_tabcntr->SetFunctionsNames(fwnames, eenames);
   }
  }
  //Завершилось чтение параметров
  else if ((*m_current_state) == mp_parcntr.get())
  {
   //конфигурация прочитана - можно разрешать панель параметров
   mp_parcntr->Enable(mp_comm->m_pControlApp->GetOnlineStatus());
  }
  //Завершилось чтение таблиц
  else if ((*m_current_state) == mp_tabcntr.get())
  {
   mp_tabcntr->Enable(mp_comm->m_pControlApp->GetOnlineStatus());  
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
 ASSERT(mp_sbar);

 if (i_online) //перешли в онлайн
 {
  state = CStatusBarManager::STATE_ONLINE;
  mp_tabcntr->InvalidateCache(); //<--делаем закешированные таблицы невалидными
  _StartScenario(mp_view->GetEditTablesCheckState() ? m_scenario2 : m_scenario1);
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

 mp_sbar->SetConnectionState(state);
}

bool CParamMonTabController::OnClose(void)
{
 return true; //отвечаем что данная вкладка готова к закрытию приложения
}

void CParamMonTabController::OnCloseNotify(void)
{
  mp_tabcntr->OnCloseNotify();
}

void CParamMonTabController::OnFullScreen(bool i_what)
{
 //not used
}

bool CParamMonTabController::OnAskChangeTab(void)
{
 return true; //always allows to change tab
}

//Событие от чекбокса переключающего режим мониторинга (приборы/сырые данные)
void CParamMonTabController::OnRawSensorsCheckBox(void)
{
 mp_moncntr->ShowRawSensors(mp_view->GetRawSensorsCheckState());
}

void CParamMonTabController::OnEditTablesCheckBox(void)
{
 //Если пользователь нажал кнопку и данные в кеше контроллера невалидные (устарели)
 //то запускаем процесс чтения таблиц
 if (true==mp_view->GetEditTablesCheckState() && false==mp_tabcntr->IsValidCache())
  _StartScenario(m_scenario3);
}

void CParamMonTabController::OnPDRequestsDataCollection()
{
 _StartScenario(m_scenario4);
}

void CParamMonTabController::_StartScenario(const std::vector<CPMStateMachineState*>& scenario)
{
 m_state_machine = scenario;
 m_current_state = m_state_machine.begin();
 (*m_current_state)->StartDataCollection();
}

void CParamMonTabController::ConfigureIndicators(void)
{
 int idx = (int)mp_settings->GetShowExFixtures();
 IndicatorsCfg cfg0;
 mp_settings->GetIndicatorsConfig(cfg0);
 mp_view->mp_MIDeskDlg->SetIndicatorsCfg(cfg0.m_optIndRows[idx], cfg0.m_optIndGas_v[idx], cfg0.m_optIndCarb[idx], cfg0.m_optIndIdleValve[idx], cfg0.m_optIndPowerValve[idx], cfg0.m_optIndStBlock[idx], cfg0.m_optIndAE[idx], cfg0.m_optIndCoolingFan[idx], cfg0.m_optIndCE[idx]);

 MetersCfg cfg1;
 mp_settings->GetMetersConfig(cfg1);
 mp_view->mp_MIDeskDlg->SetMetersCfg(cfg1.m_optMetRows[idx], cfg1.m_optMetRPM[idx], cfg1.m_optMetMAP[idx], cfg1.m_optMetVBat[idx], cfg1.m_optMetIgnTim[idx], cfg1.m_optMetCLT[idx], cfg1.m_optMetAddI1[idx], cfg1.m_optMetAddI2[idx],
  cfg1.m_optInjPW[idx], cfg1.m_optMetIAT[idx], cfg1.m_optMetEGOCorr[idx], cfg1.m_optMetTPS[idx], cfg1.m_optMetAirFlow[idx], cfg1.m_optMetVehicleSpeed[idx], cfg1.m_optMetTPSDot[idx], cfg1.m_optMetMAP2[idx], cfg1.m_optMetMAPD[idx],
  cfg1.m_optMetTmp2[idx], cfg1.m_optMetFuelConsum[idx], cfg1.m_optMetKnockRetard[idx], cfg1.m_optMetKnockGraph[idx], cfg1.m_optMetSensAFR[idx]);
}
