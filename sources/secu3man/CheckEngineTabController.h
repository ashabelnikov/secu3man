#pragma once

#include "ITabController.h"
#include "CheckEngineTabDlg.h"
#include "CommunicationManager.h"
#include "StatusBarManager.h"
#include "io-core/ControlApp.h"
#include "io-core/ControlAppAdapter.h"
#include "io-core/ufcodes.h"
#include "common/ObjectTimer.h"
#include "common/unicodesupport.h"


class CCheckEngineTabController : public ITabController 
{
  public:
	CCheckEngineTabController(CCheckEngineTabDlg* i_view, CCommunicationManager* i_comm, CStatusBarManager* i_sbar);
	virtual ~CCheckEngineTabController();

  private:
    CCheckEngineTabDlg*  m_view;
	CCommunicationManager* m_comm;
	CStatusBarManager*  m_sbar;
    CControlAppAdapter* m_pAdapter;
	
    //появление/закрытие вкладки Check Engine
    virtual void OnActivate(void);
	virtual void OnDeactivate(void);
	virtual bool OnClose(void);
};

