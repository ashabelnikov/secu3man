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
#include "CheckEngineTabController.h"

#include "Application/CommunicationManager.h"
#include "common/FastDelegate.h"
#include "io-core/ce_errors.h"
#include "MainFrame/StatusBarManager.h"
#include "TabDialogs/CheckEngineTabDlg.h"

using namespace fastdelegate;
using namespace SECU3IO;

#define EHKEY _T("CheckEngineCntr")

const BYTE default_context = SENSOR_DAT;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCheckEngineTabController::CCheckEngineTabController(CCheckEngineTabDlg* i_view, CCommunicationManager* i_comm, CStatusBarManager* i_sbar)
: m_view(NULL)
, m_comm(NULL)
, m_sbar(NULL)
, m_real_time_errors_mode(false)
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
 //наполняем карту которая будет содержать ошибки отображаемые в списке. Номер бита закодированной ошибки
 //одновременно выступает ключом в этой карте и ID-шкой для идентификации элементов списка.
 DWORD v = 0;
 m_errors_ids.insert(ErrorsIDContainer::value_type(ECUERROR_CKPS_MALFUNCTION, std::make_pair(MLL::GetString(IDS_ECUERROR_CKPS_MALFUNCTION), v)));
 m_errors_ids.insert(ErrorsIDContainer::value_type(ECUERROR_EEPROM_PARAM_BROKEN, std::make_pair(MLL::GetString(IDS_ECUERROR_EEPROM_PARAM_BROKEN), v)));
 m_errors_ids.insert(ErrorsIDContainer::value_type(ECUERROR_PROGRAM_CODE_BROKEN, std::make_pair(MLL::GetString(IDS_ECUERROR_PROGRAM_CODE_BROKEN), v)));
 m_errors_ids.insert(ErrorsIDContainer::value_type(ECUERROR_KSP_CHIP_FAILED, std::make_pair(MLL::GetString(IDS_ECUERROR_KSP_CHIP_FAILED), v)));
 m_errors_ids.insert(ErrorsIDContainer::value_type(ECUERROR_KNOCK_DETECTED, std::make_pair(MLL::GetString(IDS_ECUERROR_KNOCK_DETECTED), v)));
 m_errors_ids.insert(ErrorsIDContainer::value_type(ECUERROR_MAP_SENSOR_FAIL, std::make_pair(MLL::GetString(IDS_ECUERROR_MAP_SENSOR_FAIL), v)));
 m_errors_ids.insert(ErrorsIDContainer::value_type(ECUERROR_TEMP_SENSOR_FAIL, std::make_pair(MLL::GetString(IDS_ECUERROR_TEMP_SENSOR_FAIL), v)));
 m_errors_ids.insert(ErrorsIDContainer::value_type(ECUERROR_VOLT_SENSOR_FAIL, std::make_pair(MLL::GetString(IDS_ECUERROR_VOLT_SENSOR_FAIL), v)));
 m_errors_ids.insert(ErrorsIDContainer::value_type(ECUERROR_DWELL_CONTROL, std::make_pair(MLL::GetString(IDS_ECUERROR_DWELL_CONTROL), v)));
 m_errors_ids.insert(ErrorsIDContainer::value_type(ECUERROR_CAMS_MALFUNCTION, std::make_pair(MLL::GetString(IDS_ECUERROR_CAMS_MALFUNCTION), v))); 
 m_errors_ids.insert(ErrorsIDContainer::value_type(ECUERROR_TPS_SENSOR_FAIL, std::make_pair(MLL::GetString(IDS_ECUERROR_TPS_SENSOR_FAIL), v))); 
}

CCheckEngineTabController::~CCheckEngineTabController()
{
 //empty
}

//изменились настройки программы!
void CCheckEngineTabController::OnSettingsChanged(void)
{
 //включаем необходимый для данного контекста коммуникационный контроллер
 m_comm->SwitchOn(CCommunicationManager::OP_ACTIVATE_APPLICATION, true);
}

//from MainTabController
void CCheckEngineTabController::OnActivate(void)
{
 //заполняем список кодами ошибок
 ErrorsIDContainer::const_iterator it = m_errors_ids.begin();
 for(; it != m_errors_ids.end(); ++it)
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
 m_comm->SwitchOn(CCommunicationManager::OP_ACTIVATE_APPLICATION);

 //симулируем изменение состояния для обновления контроллов, так как OnConnection вызывается только если
 //сбрывается или разрывается принудительно (путем деактивации коммуникационного контроллера)
 bool online_status = m_comm->m_pControlApp->GetOnlineStatus();
 OnConnection(online_status);
}

//from MainTabController
void CCheckEngineTabController::OnDeactivate(void)
{
 m_comm->m_pAppAdapter->RemoveEventHandler(EHKEY);
 m_sbar->SetInformationText(_T(""));
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
  }
 }

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
 }

 m_sbar->SetConnectionState(state);
}

//Контроллеру говорят что приложение хотят закрыть и спрашивают, можно ли это сделать.
bool CCheckEngineTabController::OnClose(void)
{
 return true;
}

bool CCheckEngineTabController::OnAskFullScreen(void)
{
 return false;
}

void CCheckEngineTabController::OnFullScreen(bool i_what, const CRect& i_rect)
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
 ErrorsIDContainer::const_iterator it = m_errors_ids.begin();
 for(; it != m_errors_ids.end(); ++it)
  m_view->SetErrorState((*it).first, true);
}

//очистка всех чек боксов в списке
void CCheckEngineTabController::OnListClearAllErrors(void)
{
 ErrorsIDContainer::const_iterator it = m_errors_ids.begin();
 for(; it != m_errors_ids.end(); ++it)
  m_view->SetErrorState((*it).first, false);
}

//Перекачивает битики кодов ошибок из структуры данных в чек боксы списка
void CCheckEngineTabController::_SetErrorsToList(const CEErrors* ip_errors)
{
 ErrorsIDContainer::iterator it = m_errors_ids.begin();
 for(; it != m_errors_ids.end(); ++it)
 {
  DWORD bit = 1 << ((*it).first);
  bool state = ((bit & ip_errors->flags)!=0) ? true : false;

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
 ErrorsIDContainer::const_iterator it = m_errors_ids.begin();
 for(; it != m_errors_ids.end(); ++it)
 {
  DWORD value = m_view->GetErrorState((*it).first) ? 0x00000001 : 0x00000000;
  DWORD bit = value << ((*it).first);
  op_errors->flags|= bit;
 }
}
