 /****************************************************************
 *       SECU-3  - An open source, free engine control unit
 *    Designed by Alexey A. Shabelnikov. Ukraine, Gorlovka 2007.
 *       Microprocessors systems - design & programming.
 *    contacts:
 *              http://secu-3.narod.ru
 *              ICQ: 405-791-931
 ****************************************************************/

#include "stdafx.h"
#include "resource.h"
#include "MainFrameManager.h"
#include "ChildViewManager.h"
#include "MainFrameController.h"
#include "ISECU3man.h"
#include "MainFrame.h"
#include "StatusBarManager.h"
#include "common/FastDelegate.h"

using namespace fastdelegate;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMainFrameManager::CMainFrameManager()
{
 m_pMainFrame = new CMainFrame();
 m_pChildViewManager = new CChildViewManager();
 m_pStatusBarManager = new CStatusBarManager();

 ISECU3Man* p_secu3man = ISECU3Man::GetSECU3Man();
 m_pMainFrameController = new MainFrameController(p_secu3man->GetCommunicationManager(),
  p_secu3man->GetAppSettingsManager(), m_pStatusBarManager, p_secu3man->GetLogWriter(), m_pMainFrame);
}

CMainFrameManager::~CMainFrameManager()
{
 delete m_pChildViewManager; 
 delete m_pMainFrameController;
 delete m_pStatusBarManager;
 //do not delete m_pMainFrame!
}

CMainFrame* CMainFrameManager::GreateMainWindow(void)
{
 _ASSERTE(m_pMainFrame);
 // create and load the frame with its resources
 m_pMainFrame->LoadFrame(IDR_MAINFRAME,WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE,NULL,NULL);

 HICON  hFrameIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
 m_pMainFrame->SetIcon(hFrameIcon,TRUE);

 return m_pMainFrame;
}

bool CMainFrameManager::Init(CWnd* &o_pMainWnd)
{    
 _ASSERTE(m_pMainFrame);

 //инициализируем менеджер StatusBar-a
 m_pStatusBarManager->Create(m_pMainFrame);
 m_pStatusBarManager->AddContent();

 //инициализируем иерархию менеджеров UI
 m_pChildViewManager->Init(m_pMainFrame);

 // The one and only window has been initialized, so show and update it.
 m_pMainFrame->ShowWindow(SW_SHOW);
 m_pMainFrame->UpdateWindow();

 m_pMainFrame->setOnClose(MakeDelegate(m_pChildViewManager,&CChildViewManager::OnClose));
 m_pMainFrame->setOnAskFullScreen(MakeDelegate(m_pChildViewManager,&CChildViewManager::OnAskFullScreen));
 m_pMainFrame->setOnFullScreenNotify(MakeDelegate(m_pChildViewManager,&CChildViewManager::OnFullScreen));

 m_pChildViewManager->OnAfterCreate();
 return true;
}

CStatusBarManager* CMainFrameManager::GetStatusBarManager(void) const
{
 return m_pStatusBarManager;
}
