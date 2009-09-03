
#pragma once

#include "ITabController.h"
#include "io-core/ControlApp.h"
#include "io-core/BootLoader.h"
#include "io-core/ControlAppAdapter.h"
#include "io-core/BootLoaderAdapter.h"
#include "common/ObjectTimer.h"

class CFirmwareDataMediator;
class CFirmwareTabDlg;
class CCommunicationManager;
class CStatusBarManager;

class CFirmwareTabController : public ITabController, private IAPPEventHandler, private IBLDEventHandler 
{
  public:
	CFirmwareTabController(CFirmwareTabDlg* i_view, CCommunicationManager* i_comm, CStatusBarManager* i_sbar);
	virtual ~CFirmwareTabController();

	bool IsFirmwareOpened(void);

	void SetAttenuatorMap(const float* i_values);

  private:
    CFirmwareTabDlg*  m_view;
	CCommunicationManager* m_comm;
	CStatusBarManager*  m_sbar;
	CFirmwareDataMediator* m_fwdm;

    //from ITabController
    virtual void OnActivate(void);
	virtual void OnDeactivate(void);
	virtual bool OnClose(void);

	//from IAPPEventHandler:
    virtual void OnPacketReceived(const BYTE i_descriptor, SECU3IO::SECU3Packet* ip_packet);
    virtual void OnConnection(const bool i_online);

	//from IBLDEventHandler
    virtual void OnUpdateUI(IBLDEventHandler::poolUpdateUI* ip_data);  
    virtual void OnBegin(const int opcode,const int status);
    virtual void OnEnd(const int opcode,const int status);    

	void OnSettingsChanged(void);


    CString GenerateErrorStr(void);
	void SaveEEPROMToFile(const BYTE* p_data, const int size);
	bool SaveFLASHToFile(const BYTE* p_data, const int size, CString* o_file_name = NULL, bool calculate_and_place_crc16 = false);
    bool LoadEEPROMFromFile(BYTE* p_data, const int size);
    bool LoadFLASHFromFile(BYTE* p_data, const int size, CString* o_file_name = NULL);

	bool CheckChangesAskAndSaveFirmware(void);
    void SetViewFirmwareValues(void);
	void SetViewChartsValues(void);

	void PrepareOnLoadFLASH(const BYTE* i_buff,const _TSTRING& i_file_name);
	void CFirmwareTabController::StartWritingOfFLASHFromBuff(BYTE* io_buff);

	void _OnReadFlashToFile(void);

	///////////context menu and other event handlers/////////////////////
    void OnBootLoaderInfo(void);
    void OnReadEepromToFile(void);
    void OnWriteEepromFromFile(void);
    void OnReadFlashToFile(void);
    void OnWriteFlashFromFile(void);
	void OnOpenFlashFromFile(void);
	void OnSaveFlashToFile(void);
	void OnImportDataFromAnotherFW(void);
	void OnReadFlashFromSECU(void);
	void OnWriteFlashToSECU(void);
	void OnImportDataFromSECU3(void);
	void OnImportMapsFromMPSZ(void);
	void OnExportMapsToMPSZ(void);
	void OnWirmwareInfo(void);

	void OnFWInformationTextChanged(void);
	void OnMapChanged(int i_type);
	void OnFunSetSelectionChanged(int i_selected_index);
	void OnFunSetNamechanged(int i_index_of_item, CString i_new_name);
	///////////////////////////////////////////////////////////	
    void OnBLStartedEmergency(void);

	void OnModificationCheckTimer(void);
	void OnParamDeskTabActivate(void);
    void OnParamDeskChangeInTab(void);

  private: //функции - обертки
	bool IsBLStartedEmergency(void);
    bool StartBootLoader(void);
	bool ExitBootLoader(void);

  private:
	BYTE m_bl_data[65536];
	BYTE m_code_for_merge_with_overhead[65536];
	bool m_bl_started_emergency;
	int  m_current_funset_index;
	CObjectTimer<CFirmwareTabController> m_modification_check_timer;


    enum EReadFlashMode
	{
	 MODE_RD_FLASH_TO_FILE,
     MODE_RD_FLASH_TO_BUFF_FOR_LOAD,
     MODE_RD_FLASH_FOR_IMPORT_DATA,
	 MODE_RD_FLASH_TO_BUFF_MERGE_DATA
	};

	int  m_bl_read_flash_mode;
	int  m_lastSel;                  //хранит номер вкладки панели параметров которая была выбрана в последний раз
};

