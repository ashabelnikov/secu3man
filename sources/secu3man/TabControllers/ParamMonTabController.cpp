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
#include <limits>
#include "about/secu-3about.h"
#include "Application/CommunicationManager.h"
#include "common/FastDelegate.h"
#include "io-core/bitmask.h"
#include "io-core/FirmwareMapsDataHolder.h"
#include "MainFrame/StatusBarManager.h"
#include "MIDesk/MIDeskDlg.h"
#include "MIDesk/RSDeskDlg.h"
#include "ParamDesk/Tables/TablesDeskDlg.h"
#include "ParamDesk/Params/ParamDeskDlg.h"
#include "PMInitDataCollector.h"
#include "PMMonitorController.h"
#include "PMParamsController.h"
#include "PMTablesController.h"
#include "Settings/ISettingsData.h"
#include "TabDialogs/ParamMonTabDlg.h"

using namespace fastdelegate;
using namespace SECU3IO;
#undef max

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
, mp_parcntr(new CPMParamsController(ip_view->mp_ParamDeskDlg.get(), ip_comm, ip_sbar, MakeDelegate(this, &CParamMonTabController::OnPDRequestsDataCollection), MakeDelegate(this, &CParamMonTabController::OnParametersChanged), ip_settings))
, mp_tabcntr(new CPMTablesController(ip_view->mp_TablesDeskDlg.get(), ip_comm, ip_sbar, ip_settings))
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

 mp_view->mp_MIDeskDlg->setOnMISettingsChanged(MakeDelegate(this, &CParamMonTabController::OnMIDeskSettingsChanged));
 mp_tabcntr->setOnChangeSettings(MakeDelegate(this, &CParamMonTabController::_OnChangeSettings));
}

CParamMonTabController::~CParamMonTabController()
{
 //empty
}

//изменились настройки программы!
void CParamMonTabController::OnSettingsChanged(int action)
{
 if (action == 2)
 {
  mp_view->EnableMakingChartsChildren(mp_settings->GetChildCharts());
  return;
 }

 if (action ==3)
 {
  mp_view->EnableToggleMapWnd(mp_settings->GetToggleMapWnd());
  return; 
 }

 //включаем необходимый для данного контекста коммуникационный контроллер
 if (action != 1)
 { //will be skipped if only ExFixtures check has been changed (see MainFrameController::OnAppSwitchDashboards() for more info)
  mp_comm->SwitchOn(CCommunicationManager::OP_ACTIVATE_APPLICATION, true);
  mp_moncntr->OnSettingsChanged();
 }

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
 mp_view->EnableToggleMapWnd(mp_settings->GetToggleMapWnd());

 m_one_shot_timer.SetTimer(this, &CParamMonTabController::_OnOneShotTimer, 0);
}

void CParamMonTabController::_OnOneShotTimer(void)
{
 m_one_shot_timer.KillTimer();
 if (mp_settings->GetParamMonVert() != std::numeric_limits<int>::max())
  mp_view->SetSplitterPos(mp_settings->GetParamMonVert());
}

//from MainTabController
void CParamMonTabController::OnDeactivate(void)
{
 m_one_shot_timer.KillTimer();
 mp_comm->m_pAppAdapter->RemoveEventHandler(EHKEY);

 //deactivate children controllers
 mp_moncntr->OnDeactivate();
 mp_parcntr->OnDeactivate();
 mp_tabcntr->OnDeactivate();
 
 mp_sbar->SetInformationText(_T(""));

 mp_settings->SetParamMonVert(mp_view->GetSplitterPos());
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

 mp_moncntr->OnConnection(i_online);

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
 mp_tabcntr->OnClose();
 mp_settings->SetParamMonVert(mp_view->GetSplitterPos());
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

void CParamMonTabController::OnSaveSettings(void)
{
 mp_tabcntr->OnSaveSettings();
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

void CParamMonTabController::OnParametersChanged(const BYTE i_descriptor, const void* ip_packet)
{
 if (i_descriptor == FUNSET_PAR)
 {
  mp_tabcntr->OnFunSetChanged((const FunSetPar*)ip_packet);
 }
}

void CParamMonTabController::_StartScenario(const std::vector<CPMStateMachineState*>& scenario)
{
 m_state_machine = scenario;
 m_current_state = m_state_machine.begin();
 (*m_current_state)->StartDataCollection();
}

void CParamMonTabController::ConfigureIndicators(void)
{
 //indicators
 int idx = (int)mp_settings->GetShowExFixtures();
 IndicatorsCfg cfg0;
 mp_settings->GetIndicatorsConfig(cfg0);
 mp_view->mp_MIDeskDlg->SetIndicatorsCfg(cfg0.m_optIndHeightPercent[idx], cfg0.m_optIndRows[idx], cfg0.m_optIndGas_v[idx], cfg0.m_optIndCarb[idx], cfg0.m_optIndIdleValve[idx], cfg0.m_optIndPowerValve[idx], cfg0.m_optIndStBlock[idx], cfg0.m_optIndAE[idx], cfg0.m_optIndCoolingFan[idx], cfg0.m_optIndCE[idx], cfg0.m_optIndFCRevLim[idx], cfg0.m_optIndFloodClear[idx], cfg0.m_optIndSysLocked[idx], cfg0.m_optIndIgn_i[idx], cfg0.m_optIndCond_i[idx], cfg0.m_optIndEpas_i[idx]);

 mp_view->mp_MIDeskDlg->SetIndicatorsDragNDrop(mp_settings->GetIndicatorsDragNDrop());

 //meters
 MetersCfg cfg1[2];
 mp_settings->GetMetersConfig(cfg1);
 mp_view->mp_MIDeskDlg->SetMetersCfg(cfg1[idx].m_optMetRows, cfg1[idx].m_optMetRPM, cfg1[idx].m_optMetMAP, cfg1[idx].m_optMetVBat, cfg1[idx].m_optMetIgnTim, cfg1[idx].m_optMetCLT, cfg1[idx].m_optMetAddI1, cfg1[idx].m_optMetAddI2,
  cfg1[idx].m_optMetInjPW, cfg1[idx].m_optMetIAT, cfg1[idx].m_optMetEGOCorr, cfg1[idx].m_optMetTPS, cfg1[idx].m_optMetAirFlow, cfg1[idx].m_optMetVehicleSpeed, cfg1[idx].m_optMetTPSDot, cfg1[idx].m_optMetMAP2, cfg1[idx].m_optMetMAPD,
  cfg1[idx].m_optMetTmp2, cfg1[idx].m_optMetFuelConsum, cfg1[idx].m_optMetKnockRetard, cfg1[idx].m_optMetKnockGraph, cfg1[idx].m_optMetSensAFR, cfg1[idx].m_optMetChokePos, cfg1[idx].m_optMetGDPos, cfg1[idx].m_optMetSynLoad, cfg1[idx].m_optMetInjTimB, cfg1[idx].m_optMetInjTimE, cfg1[idx].m_optMetFuelConsumF, cfg1[idx].m_optMetGrts,
  mp_settings->GetTitleFontSize(), mp_settings->GetValueFontSize(), mp_settings->GetPaneFontSize(), mp_settings->GetLabelFontSize());

 mp_view->mp_MIDeskDlg->SetMetersDragNDrop(mp_settings->GetMetersDragNDrop());
}

void CParamMonTabController::OnMIDeskSettingsChanged(void)
{
 int idx = (int)mp_settings->GetShowExFixtures();

 //indicators
 IndicatorsCfg cfg0;
 mp_settings->GetIndicatorsConfig(cfg0);

 float IndHeightPercent; int IndRows; CMIDeskDlg::IndCfg_t IndGas_v, IndCarb, IndIdleValve, IndPowerValve, IndStBlock, IndAE,
 IndCoolingFan, IndCE, IndFCRevLim, IndFloodClear, IndSysLocked, IndIgn_i, IndCond_i, IndEpas_i;

 mp_view->mp_MIDeskDlg->GetIndicatorsCfg(IndHeightPercent, IndRows, IndGas_v, IndCarb, IndIdleValve, IndPowerValve, IndStBlock, IndAE, IndCoolingFan, IndCE, IndFCRevLim, IndFloodClear, IndSysLocked, IndIgn_i, IndCond_i, IndEpas_i);

 cfg0.m_optIndHeightPercent[idx] = IndHeightPercent, cfg0.m_optIndRows[idx] = IndRows, cfg0.m_optIndGas_v[idx] = IndGas_v, cfg0.m_optIndCarb[idx] = IndCarb, cfg0.m_optIndIdleValve[idx] = IndIdleValve, cfg0.m_optIndPowerValve[idx] = IndPowerValve, cfg0.m_optIndStBlock[idx] = IndStBlock, cfg0.m_optIndAE[idx] = IndAE, cfg0.m_optIndCoolingFan[idx] = IndCoolingFan, cfg0.m_optIndCE[idx] = IndCE, cfg0.m_optIndFCRevLim[idx] = IndFCRevLim, cfg0.m_optIndFloodClear[idx] = IndFloodClear, cfg0.m_optIndSysLocked[idx] = IndSysLocked, cfg0.m_optIndIgn_i[idx] = IndIgn_i, cfg0.m_optIndCond_i[idx] = IndCond_i, cfg0.m_optIndEpas_i[idx] = IndEpas_i;

 for (int ii = 0; ii < 2; ++ii) //copy colors, redundant because we have single color set in ini file
  cfg0.m_optIndGas_v[ii].second = IndGas_v.second, cfg0.m_optIndCarb[ii].second = IndCarb.second, cfg0.m_optIndIdleValve[ii].second = IndIdleValve.second, cfg0.m_optIndPowerValve[ii].second = IndPowerValve.second, cfg0.m_optIndStBlock[ii].second = IndStBlock.second, cfg0.m_optIndAE[ii].second = IndAE.second, cfg0.m_optIndCoolingFan[ii].second = IndCoolingFan.second, cfg0.m_optIndCE[ii].second = IndCE.second, cfg0.m_optIndFCRevLim[ii].second = IndFCRevLim.second, cfg0.m_optIndFloodClear[ii].second = IndFloodClear.second, cfg0.m_optIndSysLocked[ii].second = IndSysLocked.second, cfg0.m_optIndIgn_i[ii].second = IndIgn_i.second, cfg0.m_optIndCond_i[ii].second = IndCond_i.second, cfg0.m_optIndEpas_i[ii].second = IndEpas_i.second;

 mp_settings->SetIndicatorsConfig(cfg0);

 mp_settings->SetIndicatorsDragNDrop(mp_view->mp_MIDeskDlg->GetIndicatorsDragNDrop());

 //meters
 MetersCfg cfg1[2];
 mp_settings->GetMetersConfig(cfg1);

 int  TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize;

 mp_view->mp_MIDeskDlg->GetMetersCfg(cfg1[idx].m_optMetRows, cfg1[idx].m_optMetRPM, cfg1[idx].m_optMetMAP, cfg1[idx].m_optMetVBat, cfg1[idx].m_optMetIgnTim, cfg1[idx].m_optMetCLT, cfg1[idx].m_optMetAddI1, cfg1[idx].m_optMetAddI2,
  cfg1[idx].m_optMetInjPW, cfg1[idx].m_optMetIAT, cfg1[idx].m_optMetEGOCorr, cfg1[idx].m_optMetTPS, cfg1[idx].m_optMetAirFlow, cfg1[idx].m_optMetVehicleSpeed, cfg1[idx].m_optMetTPSDot, cfg1[idx].m_optMetMAP2,
  cfg1[idx].m_optMetMAPD, cfg1[idx].m_optMetTmp2, cfg1[idx].m_optMetFuelConsum, cfg1[idx].m_optMetKnockRetard, cfg1[idx].m_optMetKnockGraph, cfg1[idx].m_optMetSensAFR, cfg1[idx].m_optMetChokePos,
  cfg1[idx].m_optMetGDPos, cfg1[idx].m_optMetSynLoad, cfg1[idx].m_optMetInjTimB, cfg1[idx].m_optMetInjTimE, cfg1[idx].m_optMetFuelConsumF, cfg1[idx].m_optMetGrts, TitleFontSize, ValueFontSize, PaneFontSize, LabelFontSize);

 mp_settings->SetMetersConfig(cfg1);
 mp_settings->SetTitleFontSize(TitleFontSize);
 mp_settings->SetValueFontSize(ValueFontSize);
 mp_settings->SetPaneFontSize(PaneFontSize);
 mp_settings->SetLabelFontSize(LabelFontSize);

 mp_settings->SetMetersDragNDrop(mp_view->mp_MIDeskDlg->GetMetersDragNDrop());
}

void CParamMonTabController::_OnChangeSettings(void)
{
 mp_view->mp_MIDeskDlg->SetITMode(mp_settings->GetITEdMode());
 mp_view->mp_ParamDeskDlg->SetITEdMode(mp_settings->GetITEdMode());
}
