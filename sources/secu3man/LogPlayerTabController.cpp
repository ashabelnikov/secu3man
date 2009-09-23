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
#include "LogPlayerTabDlg.h"
#include "LogPlayerTabController.h"
#include "common/FastDelegate.h"
#include "io-core/ufcodes.h"
#include "CommunicationManager.h"
#include "StatusBarManager.h"

using namespace fastdelegate;
using namespace SECU3IO;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define EHKEY _T("LogPlayerCntr")


CLogPlayerTabController::CLogPlayerTabController(CLogPlayerTabDlg* i_view, CCommunicationManager* i_comm, CStatusBarManager* i_sbar)
: m_view(NULL)
, m_comm(NULL)
, m_sbar(NULL)
{
 //инициализируем указатели на вспомогательные объекты
 m_view = i_view;
 m_comm = i_comm;
 m_sbar = i_sbar;

#define _IV(id, name, value) (std::make_pair((id), std::make_pair(_TSTRING(name), (value))))
 m_time_factors.insert(_IV(0, _T("4:1"), 0.25f));
 m_time_factors.insert(_IV(1, _T("3:1"), 0.33f));
 m_time_factors.insert(_IV(2, _T("2:1"), 0.50f));
 m_time_factors.insert(_IV(3, _T("1:1"), 1.00f));
 m_time_factors.insert(_IV(4, _T("1:2"), 2.00f));
 m_time_factors.insert(_IV(5, _T("1:3"), 3.00f));
 m_time_factors.insert(_IV(6, _T("1:4"), 4.00f));
#undef _IV
}


CLogPlayerTabController::~CLogPlayerTabController()
{
 //na  
}

//изменились настройки программы!
void CLogPlayerTabController::OnSettingsChanged(void)
{
 //включаем необходимый для данного контекста коммуникационный контроллер
 m_comm->SwitchOn(CCommunicationManager::OP_ACTIVATE_APPLICATION, true);   
}


//from MainTabController
void CLogPlayerTabController::OnActivate(void)
{
 //////////////////////////////////////////////////////////////////
 //Подключаем контроллер к потоку данных от SECU-3
 m_comm->m_pAppAdapter->AddEventHandler(this,EHKEY); 
 m_comm->setOnSettingsChanged(MakeDelegate(this,&CLogPlayerTabController::OnSettingsChanged)); 
 //////////////////////////////////////////////////////////////////
 
 //включаем необходимый для данного контекста коммуникационный контроллер
 m_comm->SwitchOn(CCommunicationManager::OP_ACTIVATE_APPLICATION);

 //симулируем изменение состояния для обновления контроллов, так как OnConnection вызывается только если
 //сбрывается или разрывается принудительно (путем деактивации коммуникационного контроллера)
 bool online_status = m_comm->m_pControlApp->GetOnlineStatus();
 OnConnection(online_status);


 std::vector<_TSTRING> tf_content;
 for(size_t i = 0; i < m_time_factors.size(); ++i)
  tf_content.push_back(m_time_factors[i].first.c_str()); 
 m_view->FillTimeFactorCombo(tf_content);

 m_view->SetTimeFactor(3); //1:1
}

//from MainTabController
void CLogPlayerTabController::OnDeactivate(void)
{
 m_comm->m_pAppAdapter->RemoveEventHandler(EHKEY);
 m_sbar->SetInformationText(_T(""));
}

//hurrah!!! получен пакет от SECU-3
void CLogPlayerTabController::OnPacketReceived(const BYTE i_descriptor, SECU3IO::SECU3Packet* ip_packet)
{
 //особый случай: пришел пакет с нотификацонным кодом
 if (i_descriptor == OP_COMP_NC)
 {
  const OPCompNc* p_ndata = (OPCompNc*)ip_packet;
  switch(p_ndata->opcode)
  {
   case OPCODE_EEPROM_PARAM_SAVE:
    m_sbar->SetInformationText(MLL::LoadString(IDS_PM_PARAMS_HAS_BEEN_SAVED));
    return;
  }		
 }
}

//состояние подключения изменилось!
void CLogPlayerTabController::OnConnection(const bool i_online)
{
 int state;
 ASSERT(m_sbar);

 if (i_online) //перешли в онлайн
  state = CStatusBarManager::STATE_ONLINE;	
 else
  state = CStatusBarManager::STATE_OFFLINE;  
 
 m_sbar->SetConnectionState(state);
}


bool CLogPlayerTabController::OnClose(void)
{
 return true;
}
