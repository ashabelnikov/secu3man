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

/** \file CheckEngineTabController.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "Resources/resource.h"
#include "CheckEngineTabController.h"

#include <algorithm>
#include <limits>
#include "Application/CommunicationManager.h"
#include "common/FastDelegate.h"
#include "common/MathHelpers.h"
#include "io-core/bitmask.h"
#include "io-core/ce_errors.h"
#include "MainFrame/StatusBarManager.h"
#include "TabDialogs/CheckEngineTabDlg.h"
#include "Settings/ISettingsData.h"
#include "CEErrorIdStr.h"
#include "io-core/FirmwareMapsDataHolder.h"

using namespace fastdelegate;
using namespace SECU3IO;
#undef max

#define EHKEY _T("CheckEngineCntr")

const BYTE default_context = SENSOR_DAT;
const BYTE fwinfo_context = FWINFO_DAT;
const BYTE ltftmap_context = LTFT_DAT;
const BYTE rpmgrid_context = RPMGRD_PAR;
const BYTE lodgrid_context = LODGRD_PAR;
const BYTE funset_context = FUNSET_PAR;

namespace {
void UpdateMap(float* map, int* flag, const SepTabPar* data)
{
 int address = data->address;
 for(size_t i = 0; i < data->data_size; ++i)
 {
  flag[address] = 1; //set flag
  map [address] = data->table_data[i]; //save data
  ++address;
 }
}
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//We only save value of this pointer (we do not access members), so we can ignore warning.
#pragma warning( disable : 4355 ) // : 'this' : used in base member initializer list

CCheckEngineTabController::CCheckEngineTabController(CCheckEngineTabDlg* i_view, CCommunicationManager* i_comm, CStatusBarManager* i_sbar, ISettingsData* ip_settings)
: m_view(NULL)
, m_comm(NULL)
, m_sbar(NULL)
, mp_settings(ip_settings)
, m_real_time_errors_mode(false)
, mp_errors_ids(new CEErrorIdStr())
, m_autoCETmr(this, &CCheckEngineTabController::OnReadSavedErrors)
, m_pending_ce_autoreading(false)
, m_packet_processing_state(PPS_READ_MONITOR_DATA)
, m_operation_state(-1)
, m_fw_options(0)
, m_ldaxMinVal(0)
, m_ldaxBaro(false)
, m_baroPrev(std::numeric_limits<float>::max())
{
 //инициализируем указатели на вспомогательные объекты
 m_view = i_view;
 m_comm = i_comm;
 m_sbar = i_sbar;

 m_view->setOnRealTimeErrors(MakeDelegate(this,&CCheckEngineTabController::OnRealTimeErrors));
 m_view->setOnReadSavedErrors(MakeDelegate(this,&CCheckEngineTabController::OnReadSavedErrors));
 m_view->setOnWriteSavedErrors(MakeDelegate(this,&CCheckEngineTabController::OnWriteSavedErrors));
 m_view->setOnListSetAllErrors(MakeDelegate(this,&CCheckEngineTabController::OnListSetAllErrors));
 m_view->setOnListClearAllErrors(MakeDelegate(this,&CCheckEngineTabController::OnListClearAllErrors));
 m_view->setOnTrimtabButton(MakeDelegate(this, &CCheckEngineTabController::OnTrimtabButton));
 m_view->setOnTrimtabReadButton(MakeDelegate(this, &CCheckEngineTabController::OnTrimtabReadButton));
 m_view->setOnTrimtabResetButton(MakeDelegate(this, &CCheckEngineTabController::OnTrimtabResetButton));
 m_view->setOnTrimtabSaveButton(MakeDelegate(this, &CCheckEngineTabController::OnTrimtabSaveButton));

 m_view->SetLocale(mp_settings->GetInterfaceLanguage());

 std::fill(m_rpmGrid, m_rpmGrid+F_WRK_POINTS_F, .0f);
 std::fill(m_loadGrid, m_loadGrid+F_WRK_POINTS_L, .0f);
 std::fill(m_trimTab, m_trimTab+(F_WRK_POINTS_L*F_WRK_POINTS_F), .0f);

 m_view->BindMaps(m_trimTab);
 m_view->BindRPMGrid(m_rpmGrid);
 m_view->BindLoadGrid(m_loadGrid);

 m_rdLTFTMapFlags.resize(F_WRK_POINTS_L*F_WRK_POINTS_F, 0);
 m_ltft_load_slots.reserve(32);
 m_ltft_load_slots = MathHelpers::BuildGridFromRange(1.0f, 16.0f, 16, true);  //<--reverse order
}

CCheckEngineTabController::~CCheckEngineTabController()
{
 //empty
}

//изменились настройки программы!
void CCheckEngineTabController::OnSettingsChanged(int action)
{
 //включаем необходимый для данного контекста коммуникационный контроллер
 VERIFY(m_comm->SwitchOn(CCommunicationManager::OP_ACTIVATE_APPLICATION, true));
}

//from MainTabController
void CCheckEngineTabController::OnActivate(void)
{
 PPS_SetOperation(PPS_READ_FWOPTIONS);
 m_baroPrev = std::numeric_limits<float>::max();

 //заполняем список кодами ошибок
 const CEErrorIdStr::ErrorsIDContainer errors_ids = mp_errors_ids->Get();
 CEErrorIdStr::ErrorsIDContainer::const_iterator it = errors_ids.begin();
 for(; it != errors_ids.end(); ++it)
 {
  size_t found = 0;
  for(size_t i = 0; i < SECU3_CE_ERRCODES_COUNT; ++i)
   if (secu3_ce_error_codes[i].first==(*it).first)
    found = i;
  if (found < SECU3_CE_ERRCODES_COUNT)
  {
   TCHAR str[10];
   _stprintf(str, _T("%d"), secu3_ce_error_codes[found].second);
   m_view->AppendErrorsList((*it).first, _TSTRING(str), (*it).second.first, false);
  }
  else
   ASSERT(0);
 }

 m_view->EnableRWButtons(true);

 m_comm->m_pAppAdapter->AddEventHandler(this,EHKEY);
 m_comm->setOnSettingsChanged(MakeDelegate(this,&CCheckEngineTabController::OnSettingsChanged));

 m_real_time_errors_mode = false;

 //включаем необходимый для данного контекста коммуникационный контроллер
 VERIFY(m_comm->SwitchOn(CCommunicationManager::OP_ACTIVATE_APPLICATION));

 //симулируем изменение состояния для обновления контроллов, так как OnConnection вызывается только если
 //сбрывается или разрывается принудительно (путем деактивации коммуникационного контроллера)
 bool online_status = m_comm->m_pControlApp->GetOnlineStatus();
 OnConnection(online_status);

 //read CE error from SECU-3 after opening of tab if allowed
 if (mp_settings->GetAutoCERead())
  m_pending_ce_autoreading = true;
}

//from MainTabController
void CCheckEngineTabController::OnDeactivate(void)
{
 m_comm->m_pAppAdapter->RemoveEventHandler(EHKEY);
 m_sbar->SetInformationText(_T(""));
 m_pending_ce_autoreading = false;

 m_view->EnableTrimtab(false);
 PPS_SetOperation(PPS_READ_FWOPTIONS, true);
}

void CCheckEngineTabController::OnPacketReceived(const BYTE i_descriptor, SECU3IO::SECU3Packet* ip_packet)
{
 //особый случай: пришел пакет с нотификацонным кодом
 if (i_descriptor == OP_COMP_NC)
 {
  const OPCompNc* p_ndata = (OPCompNc*)ip_packet;
  switch(p_ndata->opcode)
  {
   case OPCODE_CE_SAVE_ERRORS: //Посланные ранее коды ошибок были сохранены в EEPROM
    m_sbar->SetInformationText(MLL::LoadString(IDS_ERROR_CODES_SAVED_SUCCESSFULLY));
    return;
   case OPCODE_EEPROM_PARAM_SAVE: //параметры были сохранены
    m_sbar->SetInformationText(MLL::LoadString(IDS_PM_PARAMS_HAS_BEEN_SAVED));
    return;
   case OPCODE_SAVE_TABLSET: //таблицы были сохранены
    m_sbar->SetInformationText(MLL::LoadString(IDS_PM_TABLSET_HAS_BEEN_SAVED));    
    return;
   case OPCODE_RESET_LTFT: //LTFT table had been reset
    m_sbar->SetInformationText(MLL::LoadString(IDS_CE_LTFT_HAS_BEEN_RESET));
    std::fill(m_trimTab, m_trimTab + 256, .0f);
    m_view->UpdateView();
    return;
   case OPCODE_SAVE_LTFT: //LTFT table had been saved
    m_sbar->SetInformationText(MLL::LoadString(IDS_CE_LTFT_HAS_BEEN_SAVED));
    return;
  }
 }

 //обработка приходящих пакетов в зависимости от текущего режима
 switch(m_packet_processing_state)
 {
  case PPS_READ_FWOPTIONS:  //read firmware options
   if (PPS_ReadFWOptions(i_descriptor, ip_packet))
   {
    if (m_view->GetTrimtabButtonState())
     m_packet_processing_state = PPS_READ_RPMGRID;      //continue reading rest of data
    else
     m_packet_processing_state = PPS_READ_MONITOR_DATA; //button is not pushed, got to monitor mode
   }
   break;
  case PPS_READ_RPMGRID:
   if (PPS_ReadRPMGrid(i_descriptor, ip_packet))
    m_packet_processing_state = PPS_READ_LODGRID;
   else
    break;
  case PPS_READ_LODGRID:
   if (PPS_ReadLodGrid(i_descriptor, ip_packet))
    m_packet_processing_state = PPS_READ_FUNSET;
   else
    break;
  case PPS_READ_FUNSET:
   if (PPS_ReadFunset(i_descriptor, ip_packet))
   {
    m_comm->m_pControlApp->ChangeContext(ltftmap_context);
    m_packet_processing_state = PPS_READ_LTFT_MAP;
   }
   else
    break;
  case PPS_READ_LTFT_MAP:
   if (PPS_ReadLTFTMap(i_descriptor, ip_packet))
   {
    m_packet_processing_state = PPS_READ_MONITOR_DATA;
    m_view->UpdateView(true);
    m_view->EnableTrimtab(true);
   }
   else
    break;
  case PPS_READ_MONITOR_DATA:  //получение данных для монитора
   if (i_descriptor!=default_context)
    m_comm->m_pControlApp->ChangeContext(default_context); //set to default context
   else
   {
    SensorDat* sd = (SensorDat*)(ip_packet);    
    if (m_baroPrev!=sd->baro_press && m_ldaxBaro) //update upper pressure value here if MAP(baro) is selected as load and baro value is changed
    {
     m_ltft_load_slots = MathHelpers::BuildGridFromRange(m_ldaxMinVal, sd->baro_press, 16, true); //<-- reverse order
     m_view->BindLoadGrid(&m_ltft_load_slots[0]);  
     m_view->UpdateView(true);
     m_baroPrev = sd->baro_press; //update cached value
    }
    m_view->SetArguments(sd->frequen, sd->load, sd->strt_use);
   }
   break;
 }//switch

 if (i_descriptor == CE_ERR_CODES && m_real_time_errors_mode)
 { //пришел пакет содержащий коды ошибок (в реальном времени)
  CEErrors* errors = reinterpret_cast<CEErrors*>(ip_packet);
  _SetErrorsToList(errors);
 }

 if (i_descriptor == CE_SAVED_ERR)
 { //приняли ошибки прочитанные из EEPROM
  CEErrors* errors = reinterpret_cast<CEErrors*>(ip_packet);
  m_sbar->SetInformationText(MLL::LoadString(IDS_ERROR_CODES_READ_SUCCESSFULLY));
  _SetErrorsToList(errors);
 }

 if (i_descriptor == SENSOR_DAT && m_pending_ce_autoreading)
 {
  m_pending_ce_autoreading = false;
  m_autoCETmr.SetTimer(100, true); //one shot timer
 }
}


void CCheckEngineTabController::OnConnection(const bool i_online)
{
 int state;
 ASSERT(m_sbar);

 if (i_online) //перешли в онлайн
 {
  state = CStatusBarManager::STATE_ONLINE;
  //Если установлен чекбокс режима реального времени, то меняем контекст
  m_comm->m_pControlApp->ChangeContext(m_real_time_errors_mode ? CE_ERR_CODES : default_context);
  m_view->EnableAll(true);
 }
 else
 {
  state = CStatusBarManager::STATE_OFFLINE;
 }

 //здесь мы можем только запрещать панели, а разрешать их будем только тогда, когда прочитана конфигурация
 if (i_online==false)
 {
  m_view->EnableAll(false);
  m_view->EnableTrimtab(false);
  PPS_SetOperation(PPS_READ_FWOPTIONS, true);
  m_baroPrev = std::numeric_limits<float>::max();
 }

 m_sbar->SetConnectionState(state);
}

//This method called when framework ask to close application
bool CCheckEngineTabController::OnClose(void)
{
 return true;
}

void CCheckEngineTabController::OnCloseNotify(void)
{
  //empty
}

void CCheckEngineTabController::OnFullScreen(bool i_what)
{
 //empty
}

bool CCheckEngineTabController::OnAskChangeTab(void)
{
 return true; //always allows to change tab
}

void CCheckEngineTabController::OnSaveSettings(void)
{
 //empty
}

//посльзователь "дернул" чек бокс "Считывать ошибки в реальном времени".
void CCheckEngineTabController::OnRealTimeErrors(void)
{
 bool checked  = m_view->GetRealTimeErrorsCheck();
 m_real_time_errors_mode = checked;
 m_view->EnableRWButtons(!checked);

 if (checked)
 {
  m_sbar->SetInformationText("");
  m_comm->m_pControlApp->ChangeContext(CE_ERR_CODES);  //change context!
 }
 else
 {
  //устанавливаем контекст по умолчанию
  m_comm->m_pControlApp->ChangeContext(default_context);
 }
}

//Посылка команды которая заставит SECU-3 прочитать коды ошибок из EEPROM
//и передать их в менеджер (чек боксы в списке)
void CCheckEngineTabController::OnReadSavedErrors(void)
{
 m_sbar->SetInformationText(MLL::LoadString(IDS_READING_ERROR_CODES_FROM_EEPROM));
 OPCompNc packet_data;
 packet_data.opcode = OPCODE_CE_SAVE_ERRORS;
 m_comm->m_pControlApp->SendPacket(OP_COMP_NC,&packet_data);
}

//Чтение из списка состояния чек боксов и передача пакета с кодами ошибок в SECU-3.
//После получения паета SECU-3 сохранит коды ошибок в EEPROM.
void CCheckEngineTabController::OnWriteSavedErrors(void)
{
 m_sbar->SetInformationText(MLL::LoadString(IDS_WRITING_ERROR_CODES_TO_EEPROM));
 CEErrors packet_data;
 _GetErrorsFromList(&packet_data);
 m_comm->m_pControlApp->SendPacket(CE_SAVED_ERR,&packet_data);
}

//установка всех чек боксов в списке
void CCheckEngineTabController::OnListSetAllErrors(void)
{
 const CEErrorIdStr::ErrorsIDContainer errors_ids = mp_errors_ids->Get();
 CEErrorIdStr::ErrorsIDContainer::const_iterator it = errors_ids.begin();
 for(; it != errors_ids.end(); ++it)
  m_view->SetErrorState((*it).first, true);
}

//очистка всех чек боксов в списке
void CCheckEngineTabController::OnListClearAllErrors(void)
{
 const CEErrorIdStr::ErrorsIDContainer errors_ids = mp_errors_ids->Get();
 CEErrorIdStr::ErrorsIDContainer::const_iterator it = errors_ids.begin();
 for(; it != errors_ids.end(); ++it)
  m_view->SetErrorState((*it).first, false);
}

//Перекачивает битики кодов ошибок из структуры данных в чек боксы списка
void CCheckEngineTabController::_SetErrorsToList(const CEErrors* ip_errors)
{
 CEErrorIdStr::ErrorsIDContainer errors_ids = mp_errors_ids->Get();
 CEErrorIdStr::ErrorsIDContainer::iterator it = errors_ids.begin();
 for(; it != errors_ids.end(); ++it)
 {
  bool state = CHECKBIT32(ip_errors->flags, ((*it).first));

  if (m_view->GetInertShow() && m_real_time_errors_mode)
  {
   DWORD current = ::GetTickCount();
   if (state)
    (*it).second.second = current;

   if ((current - (*it).second.second) < 500 )
    m_view->SetErrorState((*it).first, 1);
   else
    m_view->SetErrorState((*it).first, 0);
  }
  else
    m_view->SetErrorState((*it).first, state);
 }
}

//Перекачивает состояния чекбоксов списка в битики структуры данных
void CCheckEngineTabController::_GetErrorsFromList(SECU3IO::CEErrors* op_errors)
{
 op_errors->flags = 0;
 const CEErrorIdStr::ErrorsIDContainer errors_ids = mp_errors_ids->Get();
 CEErrorIdStr::ErrorsIDContainer::const_iterator it = errors_ids.begin();
 for(; it != errors_ids.end(); ++it)
  WRITEBIT32(op_errors->flags, ((*it).first), m_view->GetErrorState((*it).first));
}

void CCheckEngineTabController::OnTrimtabButton(void)
{
 if (m_view->GetTrimtabButtonState() && CHECKBIT32(m_fw_options, COPT_FUEL_INJECT))
 {
  if (std::find(m_rdLTFTMapFlags.begin(), m_rdLTFTMapFlags.end(), 0) != m_rdLTFTMapFlags.end())
  { //start new reading only if cache is not up to date
   PPS_SetOperation(PPS_READ_RPMGRID, false);
  }
 }
}

void CCheckEngineTabController::OnTrimtabReadButton(void)
{
 PPS_SetOperation(PPS_READ_LTFT_MAP, true);
}

void CCheckEngineTabController::OnTrimtabResetButton(void)
{
 m_sbar->SetInformationText(MLL::LoadString(IDS_CE_RESETTING_LTFT));
 OPCompNc packet_data;
 packet_data.opcode = OPCODE_RESET_LTFT;
 packet_data.opdata = OPDATA_RESET_LTFT;
 m_comm->m_pControlApp->SendPacket(OP_COMP_NC, &packet_data);
}

//return true when operation of state machine is finished
//set m_operation_state = 0 for starting
bool CCheckEngineTabController::PPS_ReadFWOptions(const BYTE i_descriptor, const void* i_packet_data)
{
 m_sbar->SetInformationText(MLL::LoadString(IDS_PM_READING_FWC_OPTIONS));

 if (i_descriptor != fwinfo_context)
 {
  SECU3IO::OPCompNc packet_data;
  packet_data.opcode = SECU3IO::OPCODE_READ_FW_SIG_INFO;
  m_comm->m_pControlApp->SendPacket(OP_COMP_NC, &packet_data);
 }
 else
 {//тот что надо!
  const FWInfoDat* fw_info = (const FWInfoDat*)i_packet_data;
  m_fw_options = fw_info->options; //save FW options
  m_sbar->SetInformationText(MLL::LoadString(IDS_PM_READY));
  return true; //operations were completed
 }

 return false; //КА продолжает работу
}

//return true when operation of state machine is finished
//set m_operation_state = 0 for starting
bool CCheckEngineTabController::PPS_ReadRPMGrid(const BYTE i_descriptor, const void* i_packet_data)
{
 m_sbar->SetInformationText(MLL::LoadString(IDS_PM_READING_RPMGRD));

 if (i_descriptor != rpmgrid_context)
  m_comm->m_pControlApp->ChangeContext(rpmgrid_context);
 else
 {//save RPM grid
  const SepTabPar* data = (const SepTabPar*)i_packet_data;
  memcpy(m_rpmGrid, data->table_data, F_RPM_SLOTS*sizeof(float)); //save RPM grid
  return true;
 }

 return false; //КА продолжает работу
}

//return true when operation of state machine is finished
//set m_operation_state = 0 for starting
bool CCheckEngineTabController::PPS_ReadLodGrid(const BYTE i_descriptor, const void* i_packet_data)
{
 m_sbar->SetInformationText(MLL::LoadString(IDS_PM_READING_LODGRD));

 if (i_descriptor != lodgrid_context)
  m_comm->m_pControlApp->ChangeContext(lodgrid_context);
 else
 {
  const SepTabPar* data = (const SepTabPar*)i_packet_data;
  memcpy(m_loadGrid, data->table_data, F_LOAD_SLOTS*sizeof(float)); //save load grid
  return true;
 }

 return false; //КА продолжает работу
}

//return true when operation of state machine is finished
//set m_operation_state = 0 for starting
bool CCheckEngineTabController::PPS_ReadLTFTMap(const BYTE i_descriptor, const void* i_packet_data)
{
 switch(m_operation_state)
 {
  case 0: //ожидаем пока не будут получены все фрагменты таблицы   
   m_sbar->SetInformationText(MLL::LoadString(IDS_CE_READING_LTFTMAP));
   if (i_descriptor!=ltftmap_context)
    m_comm->m_pControlApp->ChangeContext(ltftmap_context);
   else
   { //clear acquisition flags and save received piece of data
    std::fill(m_rdLTFTMapFlags.begin(), m_rdLTFTMapFlags.end(), 0);
    const SepTabPar* data = (const SepTabPar*)i_packet_data;
    UpdateMap(&m_trimTab[0], &m_rdLTFTMapFlags[0], data);
    m_operation_state = 1; //next state
   }   
   break;
  case 1:
   {
    if (i_descriptor != ltftmap_context)
    {
     m_operation_state = 0;
     break;
    }
    //update chache and perform checking
    const SepTabPar* data = (const SepTabPar*)i_packet_data;
    UpdateMap(&m_trimTab[0], &m_rdLTFTMapFlags[0], data);
    if (std::find(m_rdLTFTMapFlags.begin(), m_rdLTFTMapFlags.end(), 0) == m_rdLTFTMapFlags.end())
    { //cache is already up to date
     m_operation_state = -1; //all operations have been completed
     m_sbar->SetInformationText(MLL::LoadString(IDS_PM_READY));
     return true; //работа КА завершена
    }
   }
   break;
 } //switch

 return false; //КА продолжает работу
}

//return true when operation of state machine is finished
//set m_operation_state = 0 for starting
bool CCheckEngineTabController::PPS_ReadFunset(const BYTE i_descriptor, const void* i_packet_data)
{
 m_sbar->SetInformationText(MLL::LoadString(IDS_PM_READING_PARAMS));

 if (i_descriptor != funset_context)
  m_comm->m_pControlApp->ChangeContext(funset_context);
 else
 {//save read data
  const FunSetPar* data = (const FunSetPar*)i_packet_data;

  if (data->use_load_grid)
  {
   m_view->BindLoadGrid(m_loadGrid); //just use load grid from firmware
   m_ldaxBaro = false;
  }
  else
  {
   if (data->load_src_cfg == 1)
   {
    m_ldaxMinVal = data->map_lower_pressure;
    m_ldaxBaro = true; //use dynamic upper pressure because MAP(baro) load is selected. Grid will be updated later
   }
   else
   { //just use lower and upper static limits
    m_ltft_load_slots = MathHelpers::BuildGridFromRange(data->map_lower_pressure, data->map_upper_pressure, 16, true); //<-- reverse order
    m_view->BindLoadGrid(&m_ltft_load_slots[0]);
    m_ldaxBaro = false;
   }
  }

  return true;
 }

 return false; //SM continue to work
}

void CCheckEngineTabController::PPS_SetOperation(int pps, bool clear_ltft_chache /*= false*/)
{
 if (clear_ltft_chache)
  std::fill(m_rdLTFTMapFlags.begin(), m_rdLTFTMapFlags.end(), 0);
 m_packet_processing_state = pps;
 m_operation_state = 0;
}

void CCheckEngineTabController::OnTrimtabSaveButton(void)
{
 m_sbar->SetInformationText(MLL::LoadString(IDS_CE_SAVING_LTFT));
 OPCompNc packet_data;
 packet_data.opcode = OPCODE_SAVE_LTFT;
 packet_data.opdata = 0;
 m_comm->m_pControlApp->SendPacket(OP_COMP_NC, &packet_data);
}
