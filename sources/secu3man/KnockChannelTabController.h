#pragma once

#include "ITabController.h"
#include "io-core/ControlAppAdapter.h"
#include "io-core/ufcodes.h"
#include "common/ObjectTimer.h"
#include "common/unicodesupport.h"

class CKnockChannelTabDlg;
class CCommunicationManager;
class CStatusBarManager;

class CKnockChannelTabController : public ITabController, private IAPPEventHandler  
{
  public:
	CKnockChannelTabController(CKnockChannelTabDlg* i_view, CCommunicationManager* i_comm, CStatusBarManager* i_sbar);
	virtual ~CKnockChannelTabController();

  private:
    CKnockChannelTabDlg*  m_view;
	CCommunicationManager* m_comm;
	CStatusBarManager*  m_sbar;
    CControlAppAdapter* m_pAdapter;
	
	//from IAPPEventHandler:
    virtual void OnPacketReceived(const BYTE i_descriptor, SECU3IO::SECU3Packet* ip_packet);           
    virtual void OnConnection(const bool i_online);

	//настройки программы изменились
    void OnSettingsChanged(void);

    //по€вление/закрытие вкладки Check Engine
    virtual void OnActivate(void);
	virtual void OnDeactivate(void);
	virtual bool OnClose(void);

	//событи€
	void OnSaveParameters(void);
	void OnParametersChange(void);

	void OnParamsChangesTimer(void);

    enum //Packet Processing States
	{
     PPS_READ_NECESSARY_PARAMETERS = 0,
	 PPS_BEFORE_READ_MONITOR_DATA = 1,
	 PPS_READ_MONITOR_DATA = 2
	};

    void StartReadingNecessaryParameters(void); 
    bool ReadNecessaryParametersFromSECU(const BYTE i_descriptor, const void* i_packet_data);

    int  m_packet_processing_state;  //хранит код текущей операции, если никаких других операций не выполн€етс€ то должна выполн€тс€ PPS_READ_MONITOR_DATA
	int  m_operation_state;          //хранит состо€ние конечных автоматов конкретной операции, если -1 -значит  ј остановлен
	bool m_parameters_changed;       //этот флаг устанавливаетс€ параметров ƒƒ и сбрасываетс€ после посылки измененных данных в SECU
	CObjectTimer<CKnockChannelTabController> m_params_changes_timer;
};
