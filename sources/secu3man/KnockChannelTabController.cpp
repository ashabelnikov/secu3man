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

#define EHKEY _T("KnockChanCntr")

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
 //na
}

//изменились настройки программы!
void CKnockChannelTabController::OnSettingsChanged(void)
{
  //включаем необходимый для данного контекста коммуникационный контроллер
  m_comm->SwitchOn(CCommunicationManager::OP_ACTIVATE_APPLICATION, true);   
}

//from MainTabController
void CKnockChannelTabController::OnActivate(void)
{
 m_comm->m_pAppAdapter->AddEventHandler(this,EHKEY); 
 m_comm->setOnSettingsChanged(MakeDelegate(this,&CKnockChannelTabController::OnSettingsChanged));

 //включаем необходимый для данного контекста коммуникационный контроллер
 m_comm->SwitchOn(CCommunicationManager::OP_ACTIVATE_APPLICATION);

 //симулируем изменение состояния для обновления контроллов, так как OnConnection вызывается только если
 //сбрывается или разрывается принудительно (путем деактивации коммуникационного контроллера)
 bool online_status = m_comm->m_pControlApp->GetOnlineStatus();
 OnConnection(online_status);
}

//from MainTabController
void CKnockChannelTabController::OnDeactivate(void)
{
 m_comm->m_pAppAdapter->RemoveEventHandler(EHKEY);
 m_sbar->SetInformationText(_T(""));
}

void CKnockChannelTabController::OnPacketReceived(const BYTE i_descriptor, SECU3IO::SECU3Packet* ip_packet)
{

}

void CKnockChannelTabController::OnConnection(const bool i_online)
{
 int state;
  ASSERT(m_sbar);

  if (i_online) //перешли в онлайн
  {
	state = CStatusBarManager::STATE_ONLINE;
  }
  else
  {
	state = CStatusBarManager::STATE_OFFLINE;  
  }
 
  m_sbar->SetConnectionState(state);
}

bool CKnockChannelTabController::OnClose(void)
{
  return true;
}

void CKnockChannelTabController::OnSaveParameters(void)
{

}

