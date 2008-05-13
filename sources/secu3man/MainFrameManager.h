
#pragma once


#include "ChildView.h"
#include "MainFrame.h"
#include "ChildViewManager.h"


class CMainFrameManager  
{
  public:
	CMainFrame* m_pMainFrame;

	CChildViewManager* m_pChildViewManager;
	CStatusBarManager  m_StatusBarManager;

    bool Init(CWnd* &o_pMainWnd);

	CMainFrameManager();
	virtual ~CMainFrameManager();
};

