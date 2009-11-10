 /****************************************************************
 *       SECU-3  - An open source, free engine control unit
 *    Designed by Alexey A. Shabelnikov. Ukraine, Gorlovka 2007.
 *       Microprocessors systems - design & programming.
 *    contacts:
 *              http://secu-3.narod.ru
 *              ICQ: 405-791-931
 ****************************************************************/

#include "stdafx.h"
#include "ChildViewManager.h"

#include "ChildView.h"
#include "MainFrame.h"
#include "TabsManagement\MainTabManager.h"

CChildViewManager::CChildViewManager()
: mp_wndView(new CChildView)
, mp_MainTabManager(new CMainTabManager)
{
 //na   
}

CChildViewManager::~CChildViewManager()
{
 //na
}

bool CChildViewManager::Init(CMainFrame* i_pMainFrame)
{
 ASSERT(i_pMainFrame);
 ASSERT(mp_MainTabManager.get());
 // create a view to occupy the client area of the frame
 if (!mp_wndView->Create(i_pMainFrame))
 {
  TRACE0("Failed to create view window\n");
  return false;
 }
 //Set standard ID for this window can be attached to framework.
 mp_wndView->SetDlgCtrlID(AFX_IDW_PANE_FIRST);

 i_pMainFrame->SetView(mp_wndView.get());
 mp_MainTabManager->Init(mp_wndView.get());
 return true;
}

void CChildViewManager::OnAfterCreate(void)
{
 mp_MainTabManager->OnAfterCreate();
}

bool CChildViewManager::OnClose(void)
{
 return mp_MainTabManager->OnClose();
}

bool CChildViewManager::OnAskFullScreen(void)
{
 return mp_MainTabManager->OnAskFullScreen();
}

void CChildViewManager::OnFullScreen(bool i_what)
{
 mp_MainTabManager->OnFullScreen(i_what);
}
