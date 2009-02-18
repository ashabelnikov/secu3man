
#pragma once

#include "io-core/ccomport.h"
#include "io-core/BootLoader.h"
#include "io-core/ControlApp.h"
#include "AppSettingsManager.h"
#include "common/FastDelegate.h"
#include "common/unicodesupport.h"
#include "io-core/controlappadapter.h"
#include "io-core/bootloaderadapter.h"


class CCommunicationManager  
{
  public:
    typedef fastdelegate::FastDelegate0<> EventHandler;
    
	CCommunicationManager();
	virtual ~CCommunicationManager();

    bool Init(void);
	void OnAfterCreate(void);
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
	
    CAppSettingsManager* m_pSettings;
    CComPort*            m_pComPort;
    CBootLoader*         m_pBootLoader;
    CControlApp*         m_pControlApp;
	CControlAppAdapter*  m_pAppAdapter;
	CBootLoaderAdapter*  m_pBldAdapter;	
    ///////////////////////////////////////////////////

  private:
    const int m_recv_buff_size;
	const int m_send_buff_size;

	EventHandler m_OnSettingsChanged;
};

