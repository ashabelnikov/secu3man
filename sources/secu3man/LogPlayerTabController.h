
#pragma once

#include "ITabController.h"
#include "common/unicodesupport.h"
#include "io-core/ControlAppAdapter.h"
#include "common/ObjectTimer.h"

class CLogPlayerTabDlg;
class CCommunicationManager;
class CStatusBarManager;
class LogReader;

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
    void OnOpenFileButton(void);
	void OnPlayButton(void);
	void OnNextButton(void);
	void OnPrevButton(void);
	void OnTimeFactorCombo(size_t i_factor_code);
	void OnSliderMoved(UINT i_nSBCode, unsigned long i_nPos);

    void OnTimer(void);

  private:

	//<factor id, <name, value> >
	std::map<size_t, std::pair<_TSTRING, float> > m_time_factors;

	std::auto_ptr<LogReader> mp_log_reader;

    CObjectTimer<CLogPlayerTabController> m_timer;

    float m_position;
};


