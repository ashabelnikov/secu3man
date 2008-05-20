
#pragma once

#include "io-core/ccomport.h"
#include "io-core/BootLoader.h"
#include "io-core/ControlApp.h"
#include "AppSettingsManager.h"
#include "common/FastDelegate.h"
#include "common/unicodesupport.h"


class CCommunicationManager  
{
  public:
    typedef fastdelegate::FastDelegate0<> EventHandler;
    /////////////////////////////////
	CCommunicationManager();
	virtual ~CCommunicationManager();

    bool Init(void);
	bool Terminate(void);
	void SetOnSettingsChanged(EventHandler OnSettingsChanged) {m_OnSettingsChanged = OnSettingsChanged;};

    CAppSettingsManager* m_pSettings;

    CComPort*    m_pComPort;
    CBootLoader* m_pBootLoader;
    CControlApp* m_pControlApp;
    ///////////////////////////////

  private:
    const int m_recv_buff_size;
	const int m_send_buff_size;

	EventHandler m_OnSettingsChanged;
};

