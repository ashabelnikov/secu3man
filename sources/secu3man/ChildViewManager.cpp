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

#include "MainTabManager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CChildViewManager::CChildViewManager()
{
  m_pMainTabManager = new CMainTabManager;
}

CChildViewManager::~CChildViewManager()
{
  delete m_pMainTabManager;
  m_pMainTabManager = NULL;
}

bool CChildViewManager::Init(CMainFrame* i_pMainFrame)
{
  ASSERT(i_pMainFrame);
  ASSERT(m_pMainTabManager);
  // create a view to occupy the client area of the frame
  if (!m_wndView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
	CRect(0, 0, 0, 0), i_pMainFrame, AFX_IDW_PANE_FIRST, NULL))
  {
	TRACE0("Failed to create view window\n");
	return false;
  }

  i_pMainFrame->SetView(&m_wndView);

  m_pMainTabManager->Init(&m_wndView);

  return true;
}

void CChildViewManager::OnAfterCreate(void)
{
  m_pMainTabManager->OnAfterCreate();
}

bool CChildViewManager::OnClose(void)
{
 return m_pMainTabManager->OnClose();
}
