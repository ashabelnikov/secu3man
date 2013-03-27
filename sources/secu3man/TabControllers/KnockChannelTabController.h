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

#pragma once

#include "common/ObjectTimer.h"
#include "common/unicodesupport.h"
#include "io-core/ControlAppAdapter.h"
#include "io-core/ufcodes.h"
#include "TabsManagement/ITabController.h"

class CCommunicationManager;
class CKnockChannelTabDlg;
class CStatusBarManager;

class CKnockChannelTabController : public ITabController, private IAPPEventHandler
{
 public:
  CKnockChannelTabController(CKnockChannelTabDlg* ip_view, CCommunicationManager* ip_comm, CStatusBarManager* ip_sbar);
  virtual ~CKnockChannelTabController();

 private:
  //from IAPPEventHandler:
  virtual void OnPacketReceived(const BYTE i_descriptor, SECU3IO::SECU3Packet* ip_packet);
  virtual void OnConnection(const bool i_online);

  //настройки программы изменились
  void OnSettingsChanged(void);

  //появление/закрытие вкладки Check Engine
  virtual void OnActivate(void);
  virtual void OnDeactivate(void);

  virtual bool OnClose(void);
  virtual bool OnAskFullScreen(void);
  virtual void OnFullScreen(bool i_what, const CRect& i_rect);

  //события
  void OnSaveParameters(void);
  void OnParametersChange(void);
  void OnCopyToAttenuatorTable(void);
  void OnClearFunction(void);
  void OnResetPoints(const std::vector<int>& pointIndexes);
  void OnLoadPoints(void);
  void OnSavePoints(void);

  void OnParamsChangesTimer(void);

  enum //Packet Processing States
  {
   PPS_READ_NECESSARY_PARAMETERS = 0,
   PPS_READ_ATTENUATOR_MAP = 1,
   PPS_BEFORE_READ_MONITOR_DATA = 2,
   PPS_READ_MONITOR_DATA = 3
  };

  void StartReadingNecessaryParameters(void);
  bool ReadNecessaryParametersFromSECU(const BYTE i_descriptor, const void* i_packet_data);
  void StartReadingAttenuatorMap(void);
  bool ReadAttenuatorMapFromSECU(const BYTE i_descriptor, const void* i_packet_data);
  void _HandleSample(SECU3IO::SensorDat* p_packet, bool i_first_time);

  void _PerformAverageOfRPMKnockFunctionValues(std::vector<float> &o_function);
  void _InitializeRPMKnockFunctionBuffer(void);
  void _InitializeRPMKnockFunctionBuffer(const std::vector<int>& pointIndexes);

  CKnockChannelTabDlg*  mp_view;
  CCommunicationManager* mp_comm;
  CStatusBarManager*  mp_sbar;
  CControlAppAdapter* mp_pAdapter;

 private:
  int  m_packet_processing_state;  //хранит код текущей операции, если никаких других операций не выполняется то должна выполнятся PPS_READ_MONITOR_DATA
  int  m_operation_state;          //хранит состояние конечных автоматов конкретной операции, если -1 -значит КА остановлен
  bool m_parameters_changed;       //этот флаг устанавливается параметров ДД и сбрасывается после посылки измененных данных в SECU
  CObjectTimer<CKnockChannelTabController> m_params_changes_timer;
  int m_params_changes_timer_div;

  //Буфер содержащий значения функции сигнала детонации от оборотов. Каждое значение функции
  //может состоять из множества (но ограниченного) значений для усреднения.
  enum {RPM_KNOCK_SAMPLES_PER_POINT = 16};
  std::vector<std::vector<float> > m_rpm_knock_signal;
  std::vector<size_t> m_rpm_knock_signal_ii;

  //сохраняет желаемый уровень сигнала ДД когда контроллер деактивируется
  float m_k_desired_level;

  //сохраняет текущие обороты для отображения в виде вертикальной линии на графике
  int m_currentRPM;
 
  //Массив для хранения таблицы аттенюатора прочитанной из SECU-3 и массив с флагами
  std::vector<size_t> m_rdAttenMap;
  std::vector<int> m_rdAttenMapFlags;
};
