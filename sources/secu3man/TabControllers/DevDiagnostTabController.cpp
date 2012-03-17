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
#include "Resources/resource.h"
#include "DevDiagnostTabController.h"

#include "Application/CommunicationManager.h"
#include "common/FastDelegate.h"
#include "TabDialogs/DevDiagnostTabDlg.h"
#include "io-core/ufcodes.h"
#include "MainFrame/StatusBarManager.h"
#include "Settings/ISettingsData.h"

using namespace fastdelegate;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define EHKEY _T("DevDiagCntr")

CDevDiagnostTabController::CDevDiagnostTabController(CDevDiagnostTabDlg* i_view, CCommunicationManager* i_comm, CStatusBarManager* i_sbar, ISettingsData* ip_settings)
: m_view(NULL)
, m_comm(NULL)
, m_sbar(NULL)
{
 //инициализируем указатели на вспомогательные объекты
 m_view = i_view;
 m_comm = i_comm;
 m_sbar = i_sbar;
 mp_settings = ip_settings;

 //устанавливаем делегаты (обработчики событий от представления)
 m_view->setOnOutputToggle(MakeDelegate(this,&CDevDiagnostTabController::OnOutputToggle));
 m_view->setOnEnterButton(MakeDelegate(this,&CDevDiagnostTabController::OnEnterButton));
}

CDevDiagnostTabController::~CDevDiagnostTabController()
{
 //na
}

//изменились настройки программы!
void CDevDiagnostTabController::OnSettingsChanged(void)
{
 //включаем необходимый для данного контекста коммуникационный контроллер
 m_comm->SwitchOn(CCommunicationManager::OP_ACTIVATE_APPLICATION, true);

}

//from MainTabController
void CDevDiagnostTabController::OnActivate(void)
{
 //////////////////////////////////////////////////////////////////
 //Подключаем контроллер к потоку данных от SECU-3
 m_comm->m_pAppAdapter->AddEventHandler(this,EHKEY);
 m_comm->setOnSettingsChanged(MakeDelegate(this,&CDevDiagnostTabController::OnSettingsChanged));
 //////////////////////////////////////////////////////////////////

 //включаем необходимый для данного контекста коммуникационный контроллер
 m_comm->SwitchOn(CCommunicationManager::OP_ACTIVATE_APPLICATION);

 //симулируем изменение состояния для обновления контроллов, так как OnConnection вызывается только если
 //сбрывается или разрывается принудительно (путем деактивации коммуникационного контроллера)
 bool online_status = m_comm->m_pControlApp->GetOnlineStatus();
 OnConnection(online_status);
}

//from MainTabController
void CDevDiagnostTabController::OnDeactivate(void)
{
 m_comm->m_pAppAdapter->RemoveEventHandler(EHKEY);
 m_sbar->SetInformationText(_T(""));
}

//hurrah!!! получен пакет от SECU-3
void CDevDiagnostTabController::OnPacketReceived(const BYTE i_descriptor, SECU3IO::SECU3Packet* ip_packet)
{
 //особый случай: пришел пакет с нотификацонным кодом
 if (i_descriptor == OP_COMP_NC)
 {
  const SECU3IO::OPCompNc* p_ndata = (SECU3IO::OPCompNc*)ip_packet;
  switch(p_ndata->opcode)
  {
   case SECU3IO::OPCODE_EEPROM_PARAM_SAVE: //параметры были сохранены
    m_sbar->SetInformationText(MLL::LoadString(IDS_PM_PARAMS_HAS_BEEN_SAVED));
    return;
   case SECU3IO::OPCODE_SAVE_TABLSET:      //таблицы были сохранены
    m_sbar->SetInformationText(MLL::LoadString(IDS_PM_TABLSET_HAS_BEEN_SAVED));    
    return;
  }
 }
}

//состояние подключения изменилось!
void CDevDiagnostTabController::OnConnection(const bool i_online)
{
 int state;
 ASSERT(m_sbar);

 if (i_online) //перешли в онлайн
  state = CStatusBarManager::STATE_ONLINE;
 else
  state = CStatusBarManager::STATE_OFFLINE;

 m_view->EnableEnterButton(i_online);
 m_sbar->SetConnectionState(state);
}

bool CDevDiagnostTabController::OnClose(void)
{
 return true;
}

bool CDevDiagnostTabController::OnAskFullScreen(void)
{
 return false;
}

void CDevDiagnostTabController::OnFullScreen(bool i_what, const CRect& i_rect)
{
 //empty
}

void CDevDiagnostTabController::OnOutputToggle(int output_id, bool state)
{

}

void CDevDiagnostTabController::OnEnterButton(bool state)
{

}
