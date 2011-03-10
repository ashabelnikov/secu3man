/* SECU-3  - An open source, free engine control unit
   Copyright (C) 2007 Alexey A. Shabelnikov. Ukraine, Gorlovka

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

   contacts:
              http://secu-3.org
              email: shabelnikov@secu-3.org
*/

#include "stdafx.h"
#include "Resources\resource.h"
#include "MainFrameManager.h"

#include "Application\ISECU3man.h"
#include "ChildViewManager.h"
#include "common/FastDelegate.h"
#include "MainFrameController.h"
#include "MainFrame.h"
#include "StatusBarManager.h"

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

 m_pMainFrame->addOnClose(MakeDelegate(m_pChildViewManager,&CChildViewManager::OnClose));
 m_pMainFrame->setOnAskFullScreen(MakeDelegate(m_pChildViewManager,&CChildViewManager::OnAskFullScreen));
 m_pMainFrame->setOnFullScreenNotify(MakeDelegate(m_pChildViewManager,&CChildViewManager::OnFullScreen));

 m_pChildViewManager->OnAfterCreate();
 return true;
}

CStatusBarManager* CMainFrameManager::GetStatusBarManager(void) const
{
 return m_pStatusBarManager;
}
