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

using namespace fastdelegate;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMainFrameManager::CMainFrameManager()
{
  m_pChildViewManager = new CChildViewManager();
}

CMainFrameManager::~CMainFrameManager()
{
  delete m_pChildViewManager; 
  //do not delete m_pMainFrame!
}

bool CMainFrameManager::Init(CWnd* &o_pMainWnd)
{
  m_pMainFrame = new CMainFrame();
  
  // create and load the frame with its resources
  m_pMainFrame->LoadFrame(IDR_MAINFRAME,WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE,NULL,NULL);

  HICON  hFrameIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
  m_pMainFrame->SetIcon(hFrameIcon,TRUE);

  o_pMainWnd = m_pMainFrame;

  //инициализируем менеджер StatusBar-a
  m_StatusBarManager.Create(m_pMainFrame);
  m_StatusBarManager.AddContent();

  //инициализируем иерархию менеджеров UI
  m_pChildViewManager->Init(m_pMainFrame);

 // The one and only window has been initialized, so show and update it.
  m_pMainFrame->ShowWindow(SW_SHOW);
  m_pMainFrame->UpdateWindow();

  m_pChildViewManager->OnAfterCreate();

  m_pMainFrame->setFunctionOnClose(MakeDelegate(this,&CMainFrameManager::OnClose));

  return true;
}


bool CMainFrameManager::OnClose(void)
{
 return m_pChildViewManager->OnClose();
}