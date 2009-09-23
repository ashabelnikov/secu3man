
#pragma once

#include "ITabController.h"
#include "common/unicodesupport.h"
#include "io-core/ControlAppAdapter.h"


class CLogPlayerTabDlg;
class CCommunicationManager;
class CStatusBarManager;

class CLogPlayerTabController : public ITabController, private IAPPEventHandler 
{
  public:
	CLogPlayerTabController(CLogPlayerTabDlg* i_view, CCommunicationManager* i_comm, CStatusBarManager* i_sbar);
	virtual ~CLogPlayerTabController();

  private:
    CLogPlayerTabDlg*  m_view;
	CCommunicationManager* m_comm;
	CStatusBarManager*  m_sbar;    
	
    //появление/закрытие вкладки параметров и монитора
    virtual void OnActivate(void);
	virtual void OnDeactivate(void);
	virtual bool OnClose(void);

	//from IAPPEventHandler:
    virtual void OnPacketReceived(const BYTE i_descriptor, SECU3IO::SECU3Packet* ip_packet);           
    virtual void OnConnection(const bool i_online);

	void OnSettingsChanged(void);

  private:

};


