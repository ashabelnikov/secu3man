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
#include <algorithm>
#include <limits>
#include <math.h>
#include <numeric>
#include "Application/CommunicationManager.h"
#include "common/fastdelegate.h"
#include "common/MathHelpers.h"
#include "FirmwareTabController.h"
#include "KnockChannelTabController.h"
#include "MainFrame/StatusBarManager.h"
#include "ParamDesk/Params/KnockPageDlg.h"
#include "TabControllersCommunicator.h"
#include "TabDialogs/KnockChannelTabDlg.h"

using namespace fastdelegate;
using namespace SECU3IO;

#define EHKEY _T("KnockChanCntr")
#define TIMER_DIV_VAL 5
#define TIMER_PERIOD 100

const BYTE default_context = SENSOR_DAT;
const BYTE kparams_context = KNOCK_PAR;
const BYTE attnmap_context = ATTTAB_PAR;

namespace {
void UpdateMap(size_t* map, int* flag, const SepTabPar* data)
{
 int address = data->address;
 for(size_t i = 0; i < data->data_size; ++i)
 {
  flag[address] = 1; //set flag
  map [address] = MathHelpers::Round(data->table_data[i]); //save data
  ++address;
 }
}
void FindMinMaxWithBackTransformation(const std::vector<float>& array, std::vector<std::vector<float> >& exclude, const std::vector<size_t>& gain, size_t size, float& min, float& max)
{
 min = FLT_MAX; //5V is maximum
 max = 0;       //0V is minimum
 for(size_t i = 0; i < size; ++i)
 {
  float value = array[i] / SECU3IO::hip9011_attenuator_gains[gain[i]];
  if (0==exclude[i].size())
   continue; //exclude values which have no statistics
  if (value < min)
   min = value;
  if (value > max)
   max = value;
 }
}
}

CKnockChannelTabController::CKnockChannelTabController(CKnockChannelTabDlg* ip_view, CCommunicationManager* ip_comm, CStatusBarManager* ip_sbar)
: mp_view(ip_view)
, mp_comm(ip_comm)
, mp_sbar(ip_sbar)
, m_operation_state(-1)
, m_packet_processing_state(PPS_READ_MONITOR_DATA)
, m_parameters_changed(false)
, m_k_desired_level(1.0f)
, m_currentRPM(0)
, m_params_changes_timer_div(TIMER_DIV_VAL)
{
 m_rdAttenMap.resize(CKnockChannelTabDlg::RPM_KNOCK_SIGNAL_POINTS, 0);
 m_rdAttenMapFlags.resize(CKnockChannelTabDlg::RPM_KNOCK_SIGNAL_POINTS, false);

 mp_view->setOnSaveParameters(MakeDelegate(this,&CKnockChannelTabController::OnSaveParameters));
 mp_view->mp_knock_parameters_dlg->setFunctionOnChange(MakeDelegate(this,&CKnockChannelTabController::OnParametersChange));
 mp_view->setOnCopyToAttenuatorTable(MakeDelegate(this,&CKnockChannelTabController::OnCopyToAttenuatorTable));
 mp_view->setOnClearFunction(MakeDelegate(this,&CKnockChannelTabController::OnClearFunction));

 _InitializeRPMKnockFunctionBuffer();
}

CKnockChannelTabController::~CKnockChannelTabController()
{
 //empty
}

//изменились настройки программы!
void CKnockChannelTabController::OnSettingsChanged(void)
{
 //включаем необходимый для данного контекста коммуникационный контроллер
 mp_comm->SwitchOn(CCommunicationManager::OP_ACTIVATE_APPLICATION, true);
}

//from MainTabController
void CKnockChannelTabController::OnActivate(void)
{
 mp_comm->m_pAppAdapter->AddEventHandler(this,EHKEY);
 mp_comm->setOnSettingsChanged(MakeDelegate(this,&CKnockChannelTabController::OnSettingsChanged));

 //включаем необходимый для данного контекста коммуникационный контроллер
 mp_comm->SwitchOn(CCommunicationManager::OP_ACTIVATE_APPLICATION);

 //запускаем таймер по которому будет ограничиваться частота посылки данных в SECU-3, дополнительно используеься программный делитель частоты на 5
 m_params_changes_timer.SetTimer(this,&CKnockChannelTabController::OnParamsChangesTimer, TIMER_PERIOD);

 //симулируем изменение состояния для обновления контроллов, так как OnConnection вызывается только если
 //сбрывается или разрывается принудительно (путем деактивации коммуникационного контроллера)
 bool online_status = mp_comm->m_pControlApp->GetOnlineStatus();
 OnConnection(online_status);

 //запрещаем кнопку если прошивка не открыта на вкладке "Данные прошивки"
 CFirmwareTabController* p_controller = static_cast<CFirmwareTabController*>
 (TabControllersCommunicator::GetInstance()->GetReference(TCC_FIRMWARE_TAB_CONTROLLER));
 mp_view->EnableCopyToAttenuatorTableButton(p_controller->IsFirmwareOpened());
 mp_view->EnableClearFunctionButton(true);

 //восстанавливаем желаемый уровень сигнала
 mp_view->SetDesiredLevel(m_k_desired_level);
}

//from MainTabController
void CKnockChannelTabController::OnDeactivate(void)
{
 m_params_changes_timer.KillTimer();
 mp_comm->m_pAppAdapter->RemoveEventHandler(EHKEY);
 mp_sbar->SetInformationText(_T(""));

 //сохраняем желаемый уровень сигнала
 m_k_desired_level = mp_view->GetDesiredLevel();
}

void CKnockChannelTabController::OnPacketReceived(const BYTE i_descriptor, SECU3IO::SECU3Packet* ip_packet)
{
 //особый случай: пришел пакет с нотификацонным кодом
 if (i_descriptor == OP_COMP_NC)
 {
  const OPCompNc* p_ndata = (OPCompNc*)ip_packet;
  switch(p_ndata->opcode)
  {
   case OPCODE_EEPROM_PARAM_SAVE: //параметры были сохранены
    mp_sbar->SetInformationText(MLL::LoadString(IDS_KC_PARAMETERS_HAS_BEEN_SAVED));
    return;
   case OPCODE_SAVE_TABLSET:      //таблицы были сохранены
    mp_sbar->SetInformationText(MLL::LoadString(IDS_PM_TABLSET_HAS_BEEN_SAVED));    
    return;
  }
 }

 //обработка приходящих пакетов в зависимости от текущего режима
 switch(m_packet_processing_state)
 {
  case PPS_READ_NECESSARY_PARAMETERS:  //чтение указанных параметров
   if (ReadNecessaryParametersFromSECU(i_descriptor, ip_packet))
    StartReadingAttenuatorMap();
   break;

  case PPS_READ_ATTENUATOR_MAP: //чтение таблицы аттенюатора
   if (ReadAttenuatorMapFromSECU(i_descriptor, ip_packet))
   {
    m_packet_processing_state = PPS_BEFORE_READ_MONITOR_DATA;
   
    //Вся конфигурация прочитана - можно разрешать панели
    bool state = mp_comm->m_pControlApp->GetOnlineStatus();
    mp_view->mp_knock_parameters_dlg->Enable(state);
    mp_view->EnableAll(state);
   }
   break;

  case PPS_BEFORE_READ_MONITOR_DATA: //в этот режим мы попадаем только один раз
   if (i_descriptor!=default_context)
    mp_comm->m_pControlApp->ChangeContext(default_context); //!!!
   else
   {
    _HandleSample((SensorDat*)(ip_packet), true);
    m_packet_processing_state = PPS_READ_MONITOR_DATA;
   }
   break;

  case PPS_READ_MONITOR_DATA:  //получение данных для монитора
   if (i_descriptor!=default_context)
    mp_comm->m_pControlApp->ChangeContext(default_context); //!!!
   else
    _HandleSample((SensorDat*)(ip_packet), false);
   break;
 }//switch
}

void CKnockChannelTabController::OnConnection(const bool i_online)
{
 int state;
 ASSERT(mp_sbar);

 if (i_online) //перешли в онлайн
 {
  state = CStatusBarManager::STATE_ONLINE;
  StartReadingNecessaryParameters();
 }
 else
 {
  state = CStatusBarManager::STATE_OFFLINE;
 }

 if (i_online==false)
 { //здесь только запрещаем, разрешим в другом месте после выполнения подготовительных операций
  mp_view->mp_knock_parameters_dlg->Enable(i_online);
  mp_view->EnableAll(i_online);
 }

 mp_sbar->SetConnectionState(state);
 //показываем или прячем вертикальную линию отображающую обороты на графике
 mp_view->SetRPMVisibility(i_online);
}

void CKnockChannelTabController::StartReadingNecessaryParameters(void)
{
 mp_comm->m_pControlApp->ChangeContext(kparams_context);  //change context!
 m_packet_processing_state = PPS_READ_NECESSARY_PARAMETERS;
 m_operation_state = 0;
}

//возвращает true когда работа автомата завершена
//m_operation_state = 0 для запуска
bool CKnockChannelTabController::ReadNecessaryParametersFromSECU(const BYTE i_descriptor, const void* i_packet_data)
{
 mp_sbar->SetInformationText(MLL::LoadString(IDS_KC_READING_PARAMETERS));

 switch(m_operation_state)
 {
  case 0:  //ожидаем пакета с указанными нами параметрами
  {
   if (i_descriptor!=kparams_context)
   {
    mp_comm->m_pControlApp->ChangeContext(kparams_context); //!!!
   }
   else
   {//тот что надо!
    mp_view->mp_knock_parameters_dlg->SetValues((SECU3IO::KnockPar*)i_packet_data);

    //процесс инициализации окончен
    m_operation_state = -1; //останов КА - операции выполнены
    mp_sbar->SetInformationText(MLL::LoadString(IDS_KC_READY));
    return true; //операции выполнены
   }
  }
  break;
 }//switch

 return false; //КА продолжает работу
}

void CKnockChannelTabController::StartReadingAttenuatorMap(void)
{
 mp_comm->m_pControlApp->ChangeContext(attnmap_context);  //change context!
 m_packet_processing_state = PPS_READ_ATTENUATOR_MAP;
 m_operation_state = 0;
}

//возвращает true когда работа автомата завершена
//m_operation_state = 0 для запуска
bool CKnockChannelTabController::ReadAttenuatorMapFromSECU(const BYTE i_descriptor, const void* i_packet_data)
{
 switch(m_operation_state)
 {
  case 0: //ожидаем пока не будут получены все фрагменты таблицы   
   mp_sbar->SetInformationText(MLL::LoadString(IDS_KC_READING_ATTENUATOR_MAP));
   if (i_descriptor!=attnmap_context)
    mp_comm->m_pControlApp->ChangeContext(attnmap_context);
   else
   { //clear acquisition flags and save received piece of data
    std::fill(m_rdAttenMapFlags.begin(), m_rdAttenMapFlags.end(), 0);
    const SepTabPar* data = (const SepTabPar*)i_packet_data;
    UpdateMap(&m_rdAttenMap[0], &m_rdAttenMapFlags[0], data);
    m_operation_state = 1; //next state
   }   
   break;
  case 1:
   {
    if (i_descriptor != attnmap_context)
    {
     m_operation_state = 0;
     break;
    }

    //update chache and perform checking
    const SepTabPar* data = (const SepTabPar*)i_packet_data;
    UpdateMap(&m_rdAttenMap[0], &m_rdAttenMapFlags[0], data);
    if (std::find(m_rdAttenMapFlags.begin(), m_rdAttenMapFlags.end(), 0) == m_rdAttenMapFlags.end())
    { //cache is already up to date (все фрагменты таблицы прочитаны)
     m_operation_state = -1; //останов КА - операции выполнены
     mp_sbar->SetInformationText(MLL::LoadString(IDS_PM_READY));
     return true; //работа КА завершена
    }
   }
   break;
 } //switch

 return false; //КА продолжает работу
}

bool CKnockChannelTabController::OnClose(void)
{
 return true;
}

bool CKnockChannelTabController::OnAskFullScreen(void)
{
 return false;
}

void CKnockChannelTabController::OnFullScreen(bool i_what, const CRect& i_rect)
{
 //empty
}

void CKnockChannelTabController::OnSaveParameters(void)
{
 mp_sbar->SetInformationText(MLL::LoadString(IDS_KC_WRITING_PARAMETERS));
 OPCompNc packet_data;
 packet_data.opcode = OPCODE_EEPROM_PARAM_SAVE;
 mp_comm->m_pControlApp->SendPacket(OP_COMP_NC,&packet_data);
}

void CKnockChannelTabController::OnParametersChange(void)
{
 m_parameters_changed = true;
}

//передача пакетов с параметрами в SECU будет происходить не чаще чем вызов этого обработчика
void CKnockChannelTabController::OnParamsChangesTimer(void)
{
 if (m_params_changes_timer_div)
  --m_params_changes_timer_div;
 else
 {
  m_params_changes_timer_div = TIMER_DIV_VAL;
  if (m_parameters_changed)
  {
   //получаем данные от view и сохраняем их во временный буфер
   SECU3IO::KnockPar packet_data;
   mp_view->mp_knock_parameters_dlg->GetValues(&packet_data);

   //послали измененные пользователем данные (эта операция блокирует поток, поэтому за данные в стеке можно не беспокоиться)
   mp_comm->m_pControlApp->SendPacket(kparams_context, &packet_data);

   m_parameters_changed = false; //обработали событие - сбрасываем признак
  }

  //перекачиваем данные из буфера в график на представлении
  std::vector<float> values;
  _PerformAverageOfRPMKnockFunctionValues(values);
 
  float min, max;
  FindMinMaxWithBackTransformation(values, m_rpm_knock_signal, m_rdAttenMap, values.size(), min, max);
  float dlev = mp_view->GetDesiredLevel();
  if (dlev < 0.1f) dlev = 0.1f;
  if (min < 0.1f)  min = 0.1f;
  if (max < 0.1f)  max = 0.1f;

  float gate_ratio = SECU3IO::hip9011_attenuator_gains[0] / SECU3IO::hip9011_attenuator_gains[SECU3IO::GAIN_FREQUENCES_SIZE-1];
  bool level_ok = ((max / min) < gate_ratio) && 
                  ((min < dlev) ? ((dlev/min) < 2.0f) : true) && ((min >= dlev) ? ((min/dlev) < 9.0f) : true) &&
                  ((max < dlev) ? ((dlev/max) < 2.0f) : true) && ((max >= dlev) ? ((max/dlev) < 9.0f) : true);

  mp_view->SetDesiredLevelColor(level_ok);
  mp_view->SetRPMKnockSignal(values);
  if (mp_view->GetDLSMCheckboxState())
  { //automatic mode
   float dlev_max = min * 2.0f, dlev_min = max / 9.0f;  
   mp_view->SetDesiredLevel((dlev_max + dlev_min) / 2.0f);   
  }
 }

 //обновляем значение оборотов (отображается верт. линией на графике)
 mp_view->SetRPMValue(m_currentRPM);
}

void CKnockChannelTabController::_HandleSample(SECU3IO::SensorDat* p_packet, bool i_first_time)
{
 //добавляем новое значение в осциллограф
 mp_view->AppendPoint(p_packet->knock_k);

 //обновляем новым значением буфер функции сигнала ДД от оборотов
 //1. Вычисляем индекс в массиве. 200 - обороты в начале шкалы, 60 - шаг по оборотам.
 //2. Если ячейка функции не заполнена значениями - добавляем значение. Если ячейка функции
 //заполнена значениями, то добавляем новое значение поверх в соответствии с текущим индексом.
 int index_unchecked = MathHelpers::Round((p_packet->frequen - 200.f) / 60.f);
 if (index_unchecked < 0)
  index_unchecked = 0;
 if (index_unchecked > (CKnockChannelTabDlg::RPM_KNOCK_SIGNAL_POINTS - 1))
  index_unchecked = (CKnockChannelTabDlg::RPM_KNOCK_SIGNAL_POINTS - 1);
 size_t index = (size_t)index_unchecked;

 if (m_rpm_knock_signal[index].size() < RPM_KNOCK_SAMPLES_PER_POINT)
   m_rpm_knock_signal[index].push_back(p_packet->knock_k);
 else
 {
  size_t &ii = m_rpm_knock_signal_ii[index];
  m_rpm_knock_signal[index][ii] = p_packet->knock_k;
  ii = ii < (RPM_KNOCK_SAMPLES_PER_POINT - 1) ? ii + 1 : 0;
 }

 //сохраняем текущее значение оборотов
 m_currentRPM = p_packet->frequen;
}

void CKnockChannelTabController::_PerformAverageOfRPMKnockFunctionValues(std::vector<float> &o_function)
{
 o_function.clear();
 std::vector<std::vector<float> >::const_iterator it = m_rpm_knock_signal.begin();
 for(; it != m_rpm_knock_signal.end(); ++it)
 {
  float value = std::accumulate((it)->begin(), (it)->end(), 0.f);
  value = it->size() ? value / it->size() : 0; //avoid divide by zero!
  o_function.push_back(value);
 }
}

void CKnockChannelTabController::_InitializeRPMKnockFunctionBuffer(void)
{
 m_rpm_knock_signal.clear();
 m_rpm_knock_signal_ii.clear();
 for(size_t i = 0; i < CKnockChannelTabDlg::RPM_KNOCK_SIGNAL_POINTS; ++i)
 {
  m_rpm_knock_signal.push_back(std::vector<float>());
  m_rpm_knock_signal_ii.push_back(0);
 }
}

void CKnockChannelTabController::OnCopyToAttenuatorTable(void)
{
 CFirmwareTabController* p_controller = static_cast<CFirmwareTabController*>
 (TabControllersCommunicator::GetInstance()->GetReference(TCC_FIRMWARE_TAB_CONTROLLER));

 //получили выборки усредненнго сигнала шума по оборотам
 std::vector<float> values;
 _PerformAverageOfRPMKnockFunctionValues(values);

 //проводим сглаживание функции (скользящее окно 5 выборок), важный момент заключается в том, что нужно
 //сглаживать только те точки, для которых есть статистика, иначе будут захватыватся нулевые точки.
 float function_out[CKnockChannelTabDlg::RPM_KNOCK_SIGNAL_POINTS];
 float function_inp[CKnockChannelTabDlg::RPM_KNOCK_SIGNAL_POINTS];
 std::copy(values.begin(), values.end(), function_inp);
 std::copy(values.begin(), values.end(), function_out); 
 for(size_t begin = 0; begin < m_rpm_knock_signal.size(); ++begin)
  if (m_rpm_knock_signal[begin].size() > 0)
   break;  
 for(size_t end = m_rpm_knock_signal.size() - 1; end > begin; --end)
  if (m_rpm_knock_signal[end].size() > 0)
   break;
 if (begin < m_rpm_knock_signal.size())
  VERIFY(MathHelpers::Smooth1D(function_inp + begin, function_out + begin, end - begin, 5));

 //получили значение желаемого уровеня сигнала
 float level = mp_view->GetDesiredLevel();

 //подготавливаем текущую таблицу аттенюатора
 float array[CKnockChannelTabDlg::RPM_KNOCK_SIGNAL_POINTS];
 for(size_t i = 0; i < CKnockChannelTabDlg::RPM_KNOCK_SIGNAL_POINTS; ++i)
  array[i] = static_cast<float>(m_rdAttenMap[i]);

 //проводим расчет коэффициентов усиления и записываем их в таблицу аттенюатора
 //Коэфф. усиления для каждой выборки расчитывается исходя их разницы значения
 //функции и значения желаемого уровня сигнала.
 for(size_t i = 0; i < CKnockChannelTabDlg::RPM_KNOCK_SIGNAL_POINTS; ++i)
 {
  //ищем ближайший коэффициент усиления в таблице коэффициентов усиления
  size_t new_gain_index = 0;
  float smaller_diff = FLT_MAX;
  for(size_t j = 0; j < SECU3IO::GAIN_FREQUENCES_SIZE; ++j)
  {
   float gain = SECU3IO::hip9011_attenuator_gains[j];
   float old_gain = SECU3IO::hip9011_attenuator_gains[m_rdAttenMap[i]];
   float diff = fabs(gain * function_out[i] - level * old_gain);
   if (diff < smaller_diff)
   {
    smaller_diff = diff;
    new_gain_index = j;
   }
  }

  //Изменяем коэффициент усиления только для тех точек, для которых была получена статистика
  if (m_rpm_knock_signal[i].size() > 0)
    array[i] = (float)new_gain_index;
 }

 //устанавливаем обновленную таблицу аттенюатора
 p_controller->SetAttenuatorMap(array);
}

void CKnockChannelTabController::OnClearFunction(void)
{
 _InitializeRPMKnockFunctionBuffer();
 //перекачиваем данные из буфера в график на представлении
 std::vector<float> values;
 _PerformAverageOfRPMKnockFunctionValues(values);
 mp_view->SetRPMKnockSignal(values);
}
