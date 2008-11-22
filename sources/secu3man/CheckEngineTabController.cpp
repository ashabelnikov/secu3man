
#include "stdafx.h"
#include "CheckEngineTabController.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCheckEngineTabController::CCheckEngineTabController(CCheckEngineTabDlg* i_view, CCommunicationManager* i_comm, CStatusBarManager* i_sbar)
: m_view(NULL)
, m_comm(NULL)
, m_sbar(NULL)
{
  //инициализируем указатели на вспомогательные объекты
  m_view = i_view;
  m_comm = i_comm;
  m_sbar = i_sbar;
}


CCheckEngineTabController::~CCheckEngineTabController()
{  
}

//from MainTabController
void CCheckEngineTabController::OnActivate(void)
{
}

//from MainTabController
void CCheckEngineTabController::OnDeactivate(void)
{
}

bool CCheckEngineTabController::OnClose(void)
{
  return true;
}

