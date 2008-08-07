
#pragma once

#include "ChildView.h"
#include "MainFrame.h"

#include "MainTabManager.h"

class CChildViewManager  
{
public:
    CChildView  m_wndView;

	CMainTabManager* m_pMainTabManager;

	bool Init(CMainFrame* i_pMainFrame);
	void OnAfterCreate(void);

	CChildViewManager();
	virtual ~CChildViewManager();

	virtual bool OnClose(void);
};

