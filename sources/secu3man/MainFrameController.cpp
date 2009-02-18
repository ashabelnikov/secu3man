/****************************************************************
 *       SECU-3  - An open source, free engine control unit
 *    Designed by Alexey A. Shabelnikov. Ukraine, Gorlovka 2009.
 *       Microprocessors systems - design & programming.
 *    contacts:
 *              http://secu-3.narod.ru
 *              ICQ: 405-791-931
 ****************************************************************/

#include "stdafx.h"
#include "MainFrameController.h"
#include "MainFrame.h"
#include "common\fastdelegate.h"
#include "about\secu-3about.h"
#include "CommunicationManager.h"

using namespace fastdelegate;

MainFrameController::MainFrameController(CCommunicationManager* i_pCommunicationManager, 
	                  CAppSettingsManager* i_pAppSettingsManager, CStatusBarManager* i_pStatusBarManager, 
					  CMainFrame* ip_view /* = NULL*/)
: m_pCommunicationManager(i_pCommunicationManager)
, m_pAppSettingsManager(i_pAppSettingsManager)
, m_pStatusBarManager(i_pStatusBarManager)
, mp_view(ip_view)
{
 _ASSERTE(i_pCommunicationManager); 
 _ASSERTE(i_pAppSettingsManager); 
 _ASSERTE(i_pStatusBarManager); 
 _SetDelegates(); 
}

void MainFrameController::_SetDelegates(void)
{
 if (NULL==mp_view)
  return;
 mp_view->setOnAppAbout(MakeDelegate(this,&MainFrameController::OnAppAbout));
 mp_view->setOnAppSettings(MakeDelegate(this,&MainFrameController::OnAppSettings));
 mp_view->setOnAppBeginLog(MakeDelegate(this,&MainFrameController::OnAppBeginLog));
 mp_view->setOnAppEndLog(MakeDelegate(this,&MainFrameController::OnAppEndLog));
 mp_view->setIsBeginLoggingAllowed(MakeDelegate(this,&MainFrameController::IsBeginLoggingAllowed));
 mp_view->setIsEndLoggingAllowed(MakeDelegate(this,&MainFrameController::IsEndLoggingAllowed));
}

MainFrameController::~MainFrameController()
{
 //na
}

void MainFrameController::OnAppAbout()
{
 DisplayAbout(mp_view);
}

void MainFrameController::OnAppSettings()
{
 int result = m_pAppSettingsManager->ShowDialog();

 if (result==IDOK)
 {
  mp_view->BeginWaitCursor();
  m_pCommunicationManager->Init();
  mp_view->EndWaitCursor();
 }
}

void MainFrameController::OnAppBeginLog()
{
 m_pCommunicationManager->OnStartLogWriting();
 if (m_pCommunicationManager->IsLoggingInProcess())
  m_pStatusBarManager->SetLoggerState(CStatusBarManager::LOG_STATE_WRITING);
}

void MainFrameController::OnAppEndLog()
{
 m_pCommunicationManager->OnStopLogWriting();
 m_pStatusBarManager->SetLoggerState(CStatusBarManager::LOG_STATE_STOPPED);
}

bool MainFrameController::IsBeginLoggingAllowed(void)
{
 return !m_pCommunicationManager->IsLoggingInProcess();
}

bool MainFrameController::IsEndLoggingAllowed(void)
{
 return m_pCommunicationManager->IsLoggingInProcess();
}

void MainFrameController::SetView(CMainFrame* ip_view)
{
 _ASSERTE(ip_view);
 mp_view = ip_view;
 _SetDelegates();
}
