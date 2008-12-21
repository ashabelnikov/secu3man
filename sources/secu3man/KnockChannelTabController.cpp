/****************************************************************
 *       SECU-3  - An open source, free engine control unit
 *    Designed by Alexey A. Shabelnikov. Ukraine, Gorlovka 2007.
 *       Microprocessors systems - design & programming.
 *    contacts:
 *              http://secu-3.narod.ru
 *              ICQ: 405-791-931
 ****************************************************************/

#include "stdafx.h"
#include "KnockChannelTabController.h"
#include "common\fastdelegate.h"

using namespace fastdelegate;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CKnockChannelTabController::CKnockChannelTabController(CKnockChannelTabDlg* i_view, CCommunicationManager* i_comm, CStatusBarManager* i_sbar)
: m_view(NULL)
, m_comm(NULL)
, m_sbar(NULL)
{
  //инициализируем указатели на вспомогательные объекты
  m_view = i_view;
  m_comm = i_comm;
  m_sbar = i_sbar;

  m_view->setOnSaveParameters(MakeDelegate(this,&CKnockChannelTabController::OnSaveParameters));
}


CKnockChannelTabController::~CKnockChannelTabController()
{  
}

//from MainTabController
void CKnockChannelTabController::OnActivate(void)
{
}

//from MainTabController
void CKnockChannelTabController::OnDeactivate(void)
{
}

bool CKnockChannelTabController::OnClose(void)
{
  return true;
}

void CKnockChannelTabController::OnSaveParameters(void)
{

}

