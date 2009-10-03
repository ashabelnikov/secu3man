
#pragma once

#include "io-core/ccomport.h"
#include "io-core/BootLoader.h"
#include "io-core/ControlApp.h"
#include "common/FastDelegate.h"
#include "common/unicodesupport.h"
#include "io-core/controlappadapter.h"
#include "io-core/bootloaderadapter.h"

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
