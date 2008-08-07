
#pragma once

#include <map>
#include "ITabController.h"
#include "ParamMonTabDlg.h"
#include "CommunicationManager.h"
#include "StatusBarManager.h"
#include "io-core/ControlApp.h"
#include "io-core/ControlAppAdapter.h"
#include "io-core/ufcodes.h"
#include "common/ObjectTimer.h"
#include "common/unicodesupport.h"


class CParamMonTabController : public ITabController, private IAPPEventHandler 
{
  public:
	CParamMonTabController(CParamMonTabDlg* i_view, CCommunicationManager* i_comm, CStatusBarManager* i_sbar);
	virtual ~CParamMonTabController();

  private:
    CParamMonTabDlg*  m_view;
	CCommunicationManager* m_comm;
	CStatusBarManager*  m_sbar;
    CControlAppAdapter* m_pAdapter;
	
	CObjectTimer<CParamMonTabController> m_pd_changes_timer;

    //события от панели параметров
    void OnParamDeskTabActivate(void); 
    void OnParamDeskChangeInTab(void);   

    //появление/закрытие вкладки параметров и монитора
    virtual void OnActivate(void);
	virtual void OnDeactivate(void);
	virtual bool OnClose(void);

	//from IAPPEventHandler:
    virtual void OnPacketReceived(const BYTE i_descriptor, const void* i_packet_data);
    virtual void OnConnection(const bool i_online);

	void OnSettingsChanged(void);
    void OnParamDeskChangesTimer(void);


    enum //Packet Processing States
	{
	 PPS_COLLECT_INITIAL_DATA = 0,
     PPS_READ_NECESSARY_PARAMETERS = 1,
	 PPS_BEFORE_READ_MONITOR_DATA = 2,
	 PPS_READ_MONITOR_DATA = 3
	};

	////////////////////internal//////////////////////////
    bool CollectInitialDataFromSECU(const BYTE i_descriptor, const void* i_packet_data);
	void StartCollectingInitialData(void); 
    //////////////////////////////////////////////////////
    bool ReadNecessaryParametersFromSECU(const BYTE i_descriptor, const void* i_packet_data);
	void StartReadingNecessaryParameters(void);   
	//////////////////////////////////////////////////////

	void OnRawSensorsCheckBox(void);

  private:
    int  m_packet_processing_state;  //хранит код текущей операции, если никаких других операций не выполняется то должна выполнятся PPS_READ_MONITOR_DATA
	int  m_operation_state;          //хранит состояние конечных автоматов конкретной операции, если -1 -значит КА остановлен

	int  m_lastSel;                  //хранит номер вкладки панели параметров которая была выбрана в последний раз
	bool m_parameters_changed;       //этот флаг устанавливается при изменении на любой из вкладок параметров и сбрасывается после посылки измененных данных в SECU

};


