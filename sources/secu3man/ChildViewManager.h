
#pragma once

#include "ChildView.h"
#include "MainFrame.h"

#include "MainTabManager.h"
#include "ChildViewController.h"

class CChildViewManager  
{
public:
    CChildView  m_wndView;

	CMainTabManager* m_pMainTabManager;
	CChildViewController m_ChildViewController;

	bool Init(CMainFrame* i_pMainFrame);
	void OnAfterCreate(void);

	CChildViewManager();
	virtual ~CChildViewManager();

};

