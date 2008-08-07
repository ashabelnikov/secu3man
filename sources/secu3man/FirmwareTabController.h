
#pragma once

#include "ITabController.h"
#include "CommunicationManager.h"
#include "io-core/ControlApp.h"
#include "io-core/BootLoader.h"
#include "io-core/ControlAppAdapter.h"
#include "io-core/BootLoaderAdapter.h"
#include "StatusBarManager.h"

class CFirmwareDataMediator;

class CFirmwareTabController : public ITabController, private IAPPEventHandler, private IBLDEventHandler 
{
  public:
	CFirmwareTabController(CFirmwareTabDlg* i_view, CCommunicationManager* i_comm, CStatusBarManager* i_sbar);
	virtual ~CFirmwareTabController();

  private:
    CFirmwareTabDlg*  m_view;
	CCommunicationManager* m_comm;
	CStatusBarManager*  m_sbar;
	CFirmwareDataMediator* m_fwdm;

    CControlAppAdapter* m_pAppAdapter;
    CBootLoaderAdapter* m_pBldAdapter;

    //from ITabController
    virtual void OnActivate(void);
	virtual void OnDeactivate(void);
	virtual bool OnClose(void);


	//from IAPPEventHandler:
    virtual void OnPacketReceived(const BYTE i_descriptor, const void* i_packet_data);
    virtual void OnConnection(const bool i_online);

	//from IBLDEventHandler
    virtual void OnUpdateUI(const int opcode,const int total,const int current);  
    virtual void OnBegin(const int opcode,const int status);
    virtual void OnEnd(const int opcode,const int status);    

	void OnSettingsChanged(void);


    CString GenerateErrorStr(void);
	void SaveEEPROMToFile(const BYTE* p_data, const int size);
	void SaveFLASHToFile(const BYTE* p_data, const int size);
    bool LoadEEPROMFromFile(BYTE* p_data, const int size);
    bool LoadFLASHFromFile(BYTE* p_data, const int size);

    void SetViewFirmwareValues(void);

	///////////context menu event handlers/////////////////////
    void OnBootLoaderInfo(void);
    void OnReadEepromToFile(void);
    void OnWriteEepromFromFile(void);
    void OnReadFlashToFile(void);
    void OnWriteFlashFromFile(void);
	void OnOpenFlashFromFile(void);
	void OnSaveFlashToFile(void);
	void OnFWInformationTextChanged(void);
	///////////////////////////////////////////////////////////
	bool IsFirmwareOpened(void);
    void OnBLStartedEmergency(void);

  private: //функции - обертки
	bool IsBLStartedEmergency(void);
    bool StartBootLoader(void);
	bool ExitBootLoader(void);

  private:
	BYTE m_bl_data[65536];	 
	bool m_bl_started_emergency;
};

