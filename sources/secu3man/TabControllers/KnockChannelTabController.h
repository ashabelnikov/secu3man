#pragma once

#include "common\ObjectTimer.h"
#include "common\unicodesupport.h"
#include "io-core\ControlAppAdapter.h"
#include "io-core\ufcodes.h"
#include "TabsManagement\ITabController.h"

class CCommunicationManager;
class CKnockChannelTabDlg;
class CStatusBarManager;

class CKnockChannelTabController : public ITabController, private IAPPEventHandler
{
 public:
  CKnockChannelTabController(CKnockChannelTabDlg* i_view, CCommunicationManager* i_comm, CStatusBarManager* i_sbar);
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

  void OnParamsChangesTimer(void);

  enum //Packet Processing States
  {
   PPS_READ_NECESSARY_PARAMETERS = 0,
   PPS_BEFORE_READ_MONITOR_DATA = 1,
   PPS_READ_MONITOR_DATA = 2
  };

  void StartReadingNecessaryParameters(void); 
  bool ReadNecessaryParametersFromSECU(const BYTE i_descriptor, const void* i_packet_data);
  void _HandleSample(SECU3IO::SensorDat* p_packet, bool i_first_time);

  void _PerformAverageOfRPMKnockFunctionValues(std::vector<float> &o_function);
  void _InitializeRPMKnockFunctionBuffer(void);

  CKnockChannelTabDlg*  m_view;
  CCommunicationManager* m_comm;
  CStatusBarManager*  m_sbar;
  CControlAppAdapter* m_pAdapter;
 
 private:
  int  m_packet_processing_state;  //хранит код текущей операции, если никаких других операций не выполняется то должна выполнятся PPS_READ_MONITOR_DATA
  int  m_operation_state;          //хранит состояние конечных автоматов конкретной операции, если -1 -значит КА остановлен
  bool m_parameters_changed;       //этот флаг устанавливается параметров ДД и сбрасывается после посылки измененных данных в SECU
  CObjectTimer<CKnockChannelTabController> m_params_changes_timer;

  //Буфер содержащий значения функции сигнала детонации от оборотов. Каждое значение функции
  //может состоять из множества (но ограниченного) значений для усреднения.
  enum {RPM_KNOCK_SAMPLES_PER_POINT = 16};
  std::vector<std::vector<float> > m_rpm_knock_signal;
  std::vector<size_t> m_rpm_knock_signal_ii;

  //сохраняет желаемый уровень сигнала ДД когда контроллер деактивируется
  float m_k_desired_level;
};
