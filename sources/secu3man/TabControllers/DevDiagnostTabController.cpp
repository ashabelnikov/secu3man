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
#include "io-core/ufcodes.h"
#include "MainFrame/StatusBarManager.h"
#include "PMInitDataCollector.h"
#include "Settings/ISettingsData.h"
#include "TabDialogs/DevDiagnostTabDlg.h"

using namespace fastdelegate;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define EHKEY _T("DevDiagCntr")

CDevDiagnostTabController::CDevDiagnostTabController(CDevDiagnostTabDlg* ip_view, CCommunicationManager* ip_comm, CStatusBarManager* ip_sbar, ISettingsData* ip_settings)
: mp_view(ip_view)
, mp_comm(ip_comm)
, mp_sbar(ip_sbar)
, mp_settings(ip_settings)
, mp_idccntr(new CPMInitDataCollector(ip_comm, ip_sbar))
, m_comm_state(0) //for state machine
, m_diagnost_mode_active(false) //indicates that we are currently in diagnostic mode
{
 //устанавливаем делегаты (обработчики событий от представления)
 mp_view->setOnOutputToggle(MakeDelegate(this,&CDevDiagnostTabController::OnOutputToggle));
 mp_view->setOnEnterButton(MakeDelegate(this,&CDevDiagnostTabController::OnEnterButton));

 memset(&m_outputs, 0, sizeof(SECU3IO::DiagOutDat));
 m_outputs_map.insert(std::make_pair(CDevDiagnostTabDlg::OID_IGN_OUT1, &m_outputs.ign_out1));
 m_outputs_map.insert(std::make_pair(CDevDiagnostTabDlg::OID_IGN_OUT2, &m_outputs.ign_out2));
 m_outputs_map.insert(std::make_pair(CDevDiagnostTabDlg::OID_IGN_OUT3, &m_outputs.ign_out3));
 m_outputs_map.insert(std::make_pair(CDevDiagnostTabDlg::OID_IGN_OUT4, &m_outputs.ign_out4));
 m_outputs_map.insert(std::make_pair(CDevDiagnostTabDlg::OID_IE, &m_outputs.ie));
 m_outputs_map.insert(std::make_pair(CDevDiagnostTabDlg::OID_FE, &m_outputs.fe));
 m_outputs_map.insert(std::make_pair(CDevDiagnostTabDlg::OID_ECF, &m_outputs.ecf));
 m_outputs_map.insert(std::make_pair(CDevDiagnostTabDlg::OID_CE, &m_outputs.ce));
 m_outputs_map.insert(std::make_pair(CDevDiagnostTabDlg::OID_ST_BLOCK, &m_outputs.st_block));
 m_outputs_map.insert(std::make_pair(CDevDiagnostTabDlg::OID_ADD_IO1, &m_outputs.add_io1));
 m_outputs_map.insert(std::make_pair(CDevDiagnostTabDlg::OID_ADD_IO2, &m_outputs.add_io2));
}

CDevDiagnostTabController::~CDevDiagnostTabController()
{
 //na
}

//изменились настройки программы!
void CDevDiagnostTabController::OnSettingsChanged(void)
{
 //включаем необходимый для данного контекста коммуникационный контроллер
 mp_comm->SwitchOn(CCommunicationManager::OP_ACTIVATE_APPLICATION, true);
}

//from MainTabController
void CDevDiagnostTabController::OnActivate(void)
{
 memset(&m_outputs, 0, sizeof(SECU3IO::DiagOutDat)); //Outputs = 0
 m_comm_state = 0;
 m_diagnost_mode_active = false;
 mp_view->EnableEnterButton(false);
 mp_view->EnableDiagControls(false);
 mp_view->SetEnterButtonCaption(MLL::GetString(IDS_DEV_DIAG_ENTRCHK_CAPTION_ENTER));

 //////////////////////////////////////////////////////////////////
 //Подключаем контроллер к потоку данных от SECU-3
 mp_comm->m_pAppAdapter->AddEventHandler(this,EHKEY);
 mp_comm->setOnSettingsChanged(MakeDelegate(this,&CDevDiagnostTabController::OnSettingsChanged));
 //////////////////////////////////////////////////////////////////

 //включаем необходимый для данного контекста коммуникационный контроллер
 mp_comm->SwitchOn(CCommunicationManager::OP_ACTIVATE_APPLICATION);

 //симулируем изменение состояния для обновления контроллов, так как OnConnection вызывается только если
 //сбрывается или разрывается принудительно (путем деактивации коммуникационного контроллера)
 bool online_status = mp_comm->m_pControlApp->GetOnlineStatus();
 OnConnection(online_status);
}

//from MainTabController
void CDevDiagnostTabController::OnDeactivate(void)
{
 //This command will make SECU-3 to leave diagnostic mode
 if (true==m_diagnost_mode_active)
 {
  SECU3IO::OPCompNc packet_data;
  packet_data.opcode = SECU3IO::OPCODE_DIAGNOST_LEAVE;
  packet_data.opdata = 0;
  mp_comm->m_pControlApp->SendPacket(OP_COMP_NC, &packet_data);
 }

 mp_comm->m_pAppAdapter->RemoveEventHandler(EHKEY);
 mp_sbar->SetInformationText(_T(""));
}

//Получен пакет от SECU-3
void CDevDiagnostTabController::OnPacketReceived(const BYTE i_descriptor, SECU3IO::SECU3Packet* ip_packet)
{
 //особый случай: пришел пакет с нотификацонным кодом
 if (i_descriptor == OP_COMP_NC)
 {
  const SECU3IO::OPCompNc* p_ndata = (SECU3IO::OPCompNc*)ip_packet;
  switch(p_ndata->opcode)
  {
   case SECU3IO::OPCODE_EEPROM_PARAM_SAVE: //параметры были сохранены
    mp_sbar->SetInformationText(MLL::LoadString(IDS_PM_PARAMS_HAS_BEEN_SAVED));
    return;
   case SECU3IO::OPCODE_SAVE_TABLSET:      //таблицы были сохранены
    mp_sbar->SetInformationText(MLL::LoadString(IDS_PM_TABLSET_HAS_BEEN_SAVED));    
    return;
   case SECU3IO::OPCODE_DIAGNOST_ENTER:    //Confirmation: Entered diagnostic mode
    mp_view->SetEnterButtonCaption(MLL::GetString(IDS_DEV_DIAG_ENTRCHK_CAPTION_LEAVE));
    m_diagnost_mode_active = true;
    return;
   case SECU3IO::OPCODE_DIAGNOST_LEAVE:    //Confirmation: Left diagnostic mode
    mp_view->SetEnterButtonCaption(MLL::GetString(IDS_DEV_DIAG_ENTRCHK_CAPTION_ENTER));
    m_diagnost_mode_active = false;
    m_comm_state = 2; //<--wait before user enters again
    return;
  }
 }

 switch(m_comm_state)
 {
  case 0:
   if (i_descriptor == DBGVAR_DAT)
    break; //skip debug packets
   if (i_descriptor == DIAGINP_DAT && ((mp_idccntr->GetFWOptions() & (1 << SECU3IO::COPT_DIAGNOSTICS)) > 0))
   {//skip initial procedures if we are already in diagnostic mode
    m_comm_state = 2;
    m_diagnost_mode_active = true;
    mp_view->SetEnterButtonCaption(MLL::GetString(IDS_DEV_DIAG_ENTRCHK_CAPTION_LEAVE));
    mp_view->EnableEnterButton(true);//enabled
    mp_view->SetEnterButton(true);   //checked
    //Set correct states of outputs
    mp_comm->m_pControlApp->SendPacket(DIAGOUT_DAT, &m_outputs);
    break;
   }
   else
   { //device is not in diagnostic mode
    mp_view->EnableEnterButton(false);
    mp_view->SetEnterButton(false);
    mp_view->SetEnterButtonCaption(MLL::GetString(IDS_DEV_DIAG_ENTRCHK_CAPTION_ENTER));
   }

   //Start collecting initial data
   mp_idccntr->StartDataCollection();
   m_comm_state = 1;
   break;
  case 1:
   //Collect initial data
   if (mp_idccntr->CollectData(i_descriptor, ip_packet))
   {
    mp_view->EnableEnterButton((mp_idccntr->GetFWOptions() & (1 << SECU3IO::COPT_DIAGNOSTICS)) > 0);
    mp_view->EnableSECU3TFeatures((mp_idccntr->GetFWOptions() & (1 << SECU3IO::COPT_SECU3T)) > 0);
    m_comm_state = 2;
   }
   break; 
  case 2:
   //Wait before user enter to diagnostic mode
   if (true==m_diagnost_mode_active)
   {
    if (i_descriptor != DIAGINP_DAT)
     mp_comm->m_pControlApp->ChangeContext(DIAGINP_DAT);
    else
    {//first packet from diagnostic mode!
     mp_view->SetInputValues((SECU3IO::DiagInpDat*)(ip_packet));
     mp_view->EnableDiagControls(true);
     m_comm_state = 3;
     //Set correct states of outputs
     mp_comm->m_pControlApp->SendPacket(DIAGOUT_DAT, &m_outputs);
    }
   }
   break;
  case 3:
   //Display data from inputs (we are in diagnostic mode)
   if (i_descriptor != DIAGINP_DAT)
    mp_comm->m_pControlApp->ChangeContext(DIAGINP_DAT);
   else
    mp_view->SetInputValues((SECU3IO::DiagInpDat*)(ip_packet));
   break;
 };
}

//состояние подключения изменилось!
void CDevDiagnostTabController::OnConnection(const bool i_online)
{
 int state;
 ASSERT(mp_sbar);

 if (i_online) //перешли в онлайн
  state = CStatusBarManager::STATE_ONLINE;
 else
 {
  state = CStatusBarManager::STATE_OFFLINE;
  mp_view->EnableEnterButton(false);
  mp_view->EnableDiagControls(false);
  m_comm_state = 0;
  m_diagnost_mode_active = false;
 }

 mp_sbar->SetConnectionState(state);
}

bool CDevDiagnostTabController::OnClose(void)
{
 if (true==m_diagnost_mode_active)
 {
  SECU3IO::OPCompNc packet_data;
  packet_data.opcode = SECU3IO::OPCODE_DIAGNOST_LEAVE;
  packet_data.opdata = 0;
  mp_comm->m_pControlApp->SendPacket(OP_COMP_NC, &packet_data);
  Sleep(100); //wait until data transfered
 }
 return true;
}

bool CDevDiagnostTabController::OnAskFullScreen(void)
{
 //not available in this mode
 return false;
}

void CDevDiagnostTabController::OnFullScreen(bool i_what, const CRect& i_rect)
{
 //empty. Not applicable in this tab
}

void CDevDiagnostTabController::OnOutputToggle(int output_id, bool state)
{
 if (m_outputs_map.find(output_id) != m_outputs_map.end())
  *m_outputs_map[output_id] = state;
 
 mp_comm->m_pControlApp->SendPacket(DIAGOUT_DAT, &m_outputs);
}

void CDevDiagnostTabController::OnEnterButton(bool state)
{
 if (true==state)
 {
  //This command will make SECU-3 to enter diagnostic mode
  SECU3IO::OPCompNc packet_data;
  packet_data.opcode = SECU3IO::OPCODE_DIAGNOST_ENTER;
  packet_data.opdata = 0;
  mp_comm->m_pControlApp->SendPacket(OP_COMP_NC, &packet_data);
 }
 else
 {
  //This command will make SECU-3 to leave diagnostic mode
  SECU3IO::OPCompNc packet_data;
  packet_data.opcode = SECU3IO::OPCODE_DIAGNOST_LEAVE;
  packet_data.opdata = 0;
  mp_comm->m_pControlApp->SendPacket(OP_COMP_NC, &packet_data);
  mp_view->EnableDiagControls(false);
 }
}
