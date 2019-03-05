/* SECU-3  - An open source, free engine control unit
   Copyright (C) 2007 Alexey A. Shabelnikov. Ukraine, Kiev

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

/** \file DevDiagnostTabController.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "Resources/resource.h"
#include "DevDiagnostTabController.h"

#include "about/secu-3about.h"
#include "Application/CommunicationManager.h"
#include "common/FastDelegate.h"
#include "io-core/bitmask.h"
#include "io-core/ufcodes.h"
#include "MainFrame/StatusBarManager.h"
#include "PMInitDataCollector.h"
#include "Settings/ISettingsData.h"
#include "TabDialogs/DevDiagnostTabDlg.h"

using namespace fastdelegate;

#define EHKEY _T("DevDiagCntr")

namespace {

class ITstMode
{
 public:
  virtual ~ITstMode() {};
  virtual void Reset(void) = 0;
  virtual bool Next(void) = 0;  
  virtual void SetMode(bool mode) = 0;
};

//Test mode 1
class TstMode1 : public ITstMode
{
  typedef fastdelegate::FastDelegate2<int, bool> CBSetOutput;
  typedef fastdelegate::FastDelegate0<> CBUpdateOutputs;
  CBSetOutput     m_cbSO;
  CBUpdateOutputs m_cbUO;
  size_t m_state;
  std::vector<int> m_items;
 public:
  TstMode1(CBSetOutput i_cbSO, CBUpdateOutputs i_cbUO) : m_cbSO(i_cbSO), m_cbUO(i_cbUO), m_state(0)
  {
   m_items.push_back(CDevDiagnostTabDlg::OID_FE);
   m_items.push_back(CDevDiagnostTabDlg::OID_IE);
   m_items.push_back(CDevDiagnostTabDlg::OID_ST_BLOCK);
   m_items.push_back(CDevDiagnostTabDlg::OID_IGN_OUT1);
   m_items.push_back(CDevDiagnostTabDlg::OID_IGN_OUT2);
   m_items.push_back(CDevDiagnostTabDlg::OID_IGN_OUT3);
   m_items.push_back(CDevDiagnostTabDlg::OID_IGN_OUT4);
   m_items.push_back(CDevDiagnostTabDlg::OID_CE);
   m_items.push_back(CDevDiagnostTabDlg::OID_ECF);
   m_items.push_back(CDevDiagnostTabDlg::OID_ADD_IO1);
   m_items.push_back(CDevDiagnostTabDlg::OID_ADD_IO2);
   m_items.push_back(CDevDiagnostTabDlg::OID_BL);
   m_items.push_back(CDevDiagnostTabDlg::OID_DE);
  }

  virtual void Reset(void)
  {
   m_cbSO(-1, false);
   m_cbUO();
   m_state = 0;
  }

  virtual bool Next(void)
  {
   if (m_state >= m_items.size())
    return false;

   m_cbSO(m_items[(m_state == 0)  ? m_items.size()-1 : m_state - 1 ], false);
   m_cbSO(m_items[m_state++], true);
   m_cbUO();
   return true;
  }

  virtual void SetMode(bool mode)
  {
   m_items.clear();
   if (mode)
   {
    m_items.push_back(CDevDiagnostTabDlg::OID_IGN_O1);
    m_items.push_back(CDevDiagnostTabDlg::OID_IGN_O2);
    m_items.push_back(CDevDiagnostTabDlg::OID_IGN_O3);
    m_items.push_back(CDevDiagnostTabDlg::OID_IGN_O4);
    m_items.push_back(CDevDiagnostTabDlg::OID_IGN_O5);
    m_items.push_back(CDevDiagnostTabDlg::OID_INJ_O1);
    m_items.push_back(CDevDiagnostTabDlg::OID_INJ_O2);
    m_items.push_back(CDevDiagnostTabDlg::OID_INJ_O3);
    m_items.push_back(CDevDiagnostTabDlg::OID_INJ_O4);
    m_items.push_back(CDevDiagnostTabDlg::OID_INJ_O5);
    m_items.push_back(CDevDiagnostTabDlg::OID_ECFO);
    m_items.push_back(CDevDiagnostTabDlg::OID_ADD_O2);
    m_items.push_back(CDevDiagnostTabDlg::OID_EVAP_O);
    m_items.push_back(CDevDiagnostTabDlg::OID_O2SH_O);
    m_items.push_back(CDevDiagnostTabDlg::OID_COND_O);
    m_items.push_back(CDevDiagnostTabDlg::OID_STBL_O);
    m_items.push_back(CDevDiagnostTabDlg::OID_CEL_O);
    m_items.push_back(CDevDiagnostTabDlg::OID_FPMP_O);
    m_items.push_back(CDevDiagnostTabDlg::OID_PWRR_O);
    m_items.push_back(CDevDiagnostTabDlg::OID_BL);
    m_items.push_back(CDevDiagnostTabDlg::OID_DE);
    ASSERT(m_items.size()==CDevDiagnostTabDlg::OID_SECU3i_NUM);
   }
   else
   { //SECU-3T
    m_items.push_back(CDevDiagnostTabDlg::OID_FE);
    m_items.push_back(CDevDiagnostTabDlg::OID_IE);
    m_items.push_back(CDevDiagnostTabDlg::OID_ST_BLOCK);
    m_items.push_back(CDevDiagnostTabDlg::OID_IGN_OUT1);
    m_items.push_back(CDevDiagnostTabDlg::OID_IGN_OUT2);
    m_items.push_back(CDevDiagnostTabDlg::OID_IGN_OUT3);
    m_items.push_back(CDevDiagnostTabDlg::OID_IGN_OUT4);
    m_items.push_back(CDevDiagnostTabDlg::OID_CE);
    m_items.push_back(CDevDiagnostTabDlg::OID_ECF);
    m_items.push_back(CDevDiagnostTabDlg::OID_ADD_IO1);
    m_items.push_back(CDevDiagnostTabDlg::OID_ADD_IO2);
    m_items.push_back(CDevDiagnostTabDlg::OID_BL);
    m_items.push_back(CDevDiagnostTabDlg::OID_DE);
    ASSERT(m_items.size()==CDevDiagnostTabDlg::OID_SECU3T_NUM);
   }
  }
};

//Test mode 2
class TstMode2 : public ITstMode
{
  typedef fastdelegate::FastDelegate2<int, bool> CBSetOutput;
  typedef fastdelegate::FastDelegate0<> CBUpdateOutputs;
  CBSetOutput     m_cbSO;
  CBUpdateOutputs m_cbUO;
  size_t m_state;
 public:
  TstMode2(CBSetOutput i_cbSO, CBUpdateOutputs i_cbUO) : m_cbSO(i_cbSO), m_cbUO(i_cbUO), m_state(0)
  {
  }

  virtual void Reset(void)
  {
   m_cbSO(-1, false);
   m_cbUO();
   m_state = 0;
  }

  virtual bool Next(void)
  {
   if (m_state > 4)
    return false;
   if (!(m_state++ & 1))
    m_cbSO(-1, true);
   else
    m_cbSO(-1, false);
   m_cbUO();
   return true;
  }

  virtual void SetMode(bool mode)
  {
  }
 };
}

//We only save value of this pointer (we do not access members), so we can ignore warning.
#pragma warning( disable : 4355 ) // : 'this' : used in base member initializer list

CDevDiagnostTabController::CDevDiagnostTabController(CDevDiagnostTabDlg* ip_view, CCommunicationManager* ip_comm, CStatusBarManager* ip_sbar, ISettingsData* ip_settings)
: mp_view(ip_view)
, mp_comm(ip_comm)
, mp_sbar(ip_sbar)
, mp_settings(ip_settings)
, mp_idccntr(new CPMInitDataCollector(ip_comm, ip_sbar))
, m_comm_state(0) //for state machine
, m_diagnost_mode_active(false) //indicates that we are currently in diagnostic mode
, m_autoDiagTmr(this, &CDevDiagnostTabController::OnEnterButton)
{
 //========================================================
 if (!CheckVersion())
  return;
 //========================================================

 //устанавливаем делегаты (обработчики событий от представления)
 mp_view->setOnOutputToggle(MakeDelegate(this,&CDevDiagnostTabController::OnOutputToggle));
 mp_view->setOnEnterButton(MakeDelegate(this,&CDevDiagnostTabController::OnEnterButton));
 mp_view->setOnStartOutAutoTesting(MakeDelegate(this,&CDevDiagnostTabController::OnStartOutputsAutoTesting));
 mp_view->setOnStopOutAutoTesting(MakeDelegate(this,&CDevDiagnostTabController::OnStopOutputsAutoTesting));
 mp_view->setOnEnableBLDETesting(MakeDelegate(this,&CDevDiagnostTabController::OnEnableBLDETesting));

 m_tst_timer.SetMsgHandler(this, &CDevDiagnostTabController::OnTstTimer);

 memset(&m_outputs, 0, sizeof(SECU3IO::DiagOutDat));
 _FillOutputsMap();

 //Init test framework
 m_tstModes.push_back(new TstMode1(MakeDelegate(this, &CDevDiagnostTabController::SetOutputValue), MakeDelegate(this, &CDevDiagnostTabController::UpdateOutputs)));
 m_tstModes.push_back(new TstMode2(MakeDelegate(this, &CDevDiagnostTabController::SetOutputValue), MakeDelegate(this, &CDevDiagnostTabController::UpdateOutputs)));
 m_current_tst_mode = m_tstModes.end();
}

CDevDiagnostTabController::~CDevDiagnostTabController()
{
 for(size_t i = 0; i < m_tstModes.size(); ++i)
  delete m_tstModes[i];
}

//изменились настройки программы!
void CDevDiagnostTabController::OnSettingsChanged(int action)
{
 //включаем необходимый для данного контекста коммуникационный контроллер
 mp_comm->SwitchOn(CCommunicationManager::OP_ACTIVATE_APPLICATION, true);
}

//from MainTabController
void CDevDiagnostTabController::OnActivate(void)
{
 memset(&m_outputs, 0, sizeof(SECU3IO::DiagOutDat)); //Outputs = 0, 3-state outputs = HiZ
 m_comm_state = 0;
 m_diagnost_mode_active = false;
 mp_view->EnableEnterButton(false);
 mp_view->EnableDiagControls(false);
 mp_view->EnableBLDETesting(false);
 mp_view->SetEnterButtonCaption(MLL::GetString(IDS_DEV_DIAG_ENTRCHK_CAPTION_ENTER));
 mp_view->EnableStartAutoTstMenuItem(true);
 mp_view->EnableStopAutoTstMenuItem(false);

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
 OnStopOutputsAutoTesting(); //stop auto testing if it is run

 //This command will make SECU-3 to leave diagnostic mode
 if (true==m_diagnost_mode_active)
 {
  Sleep(100);
  SECU3IO::OPCompNc packet_data;
  packet_data.opcode = SECU3IO::OPCODE_DIAGNOST_LEAVE;
  packet_data.opdata = 0;
  mp_comm->m_pControlApp->SendPacket(OP_COMP_NC, &packet_data);
  Sleep(10);
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
    mp_view->SetEnterButton(true);
    mp_view->SetEnterButtonCaption(MLL::GetString(IDS_DEV_DIAG_ENTRCHK_CAPTION_LEAVE));
    m_diagnost_mode_active = true;
    return;
   case SECU3IO::OPCODE_DIAGNOST_LEAVE:    //Confirmation: Left diagnostic mode
    mp_view->SetEnterButton(false);
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
   if (i_descriptor == DIAGINP_DAT && CHECKBIT32(mp_idccntr->GetFWOptions(), SECU3IO::COPT_DIAGNOSTICS))
   {//skip initial procedures if we are already in diagnostic mode
    m_comm_state = 2;
    m_diagnost_mode_active = true;
    mp_view->SetEnterButton(true);   //checked
    mp_view->SetEnterButtonCaption(MLL::GetString(IDS_DEV_DIAG_ENTRCHK_CAPTION_LEAVE));
    mp_view->EnableEnterButton(true);//enabled
    mp_view->EnableSECU3TFeatures(((const SECU3IO::DiagInpDat*)ip_packet)->f_secu3t);
    //Set correct states of outputs
    UpdateOutputs();
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
    m_outputs.mode = !CHECKBIT32(mp_idccntr->GetFWOptions(), SECU3IO::COPT_SECU3T); //set mode (SECU-3T or SECU-3i)
    //update outputs' map
    _FillOutputsMap();
    //update test modes
    for(size_t i = 0; i < m_tstModes.size(); ++i)
     m_tstModes[i]->SetMode(m_outputs.mode);

    bool enableEnterBtn = CHECKBIT32(mp_idccntr->GetFWOptions(), SECU3IO::COPT_DIAGNOSTICS);
    mp_view->EnableEnterButton(enableEnterBtn);
    mp_view->EnableSECU3TFeatures(CHECKBIT32(mp_idccntr->GetFWOptions(), SECU3IO::COPT_SECU3T));
    mp_view->EnableExtraIO(!CHECKBIT32(mp_idccntr->GetFWOptions(), SECU3IO::COPT_SECU3T) && CHECKBIT32(mp_idccntr->GetFWOptions(), SECU3IO::COPT_TPIC8101));
    m_comm_state = 2;
    if (enableEnterBtn && mp_settings->GetAutoDiagEnter())
    { //automatic entering into a diagnostic mode
     mp_view->SetEnterButton(true);
     m_autoDiagTmr.SetTimer(100, true); //one shot timer
    }
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
     UpdateOutputs();
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
  OnStopOutputsAutoTesting(); //stop auto testing if it is run
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

void CDevDiagnostTabController::OnCloseNotify(void)
{
  //empty
}

void CDevDiagnostTabController::OnFullScreen(bool i_what)
{
 //empty. Not applicable in this tab
}

bool CDevDiagnostTabController::OnAskChangeTab(void)
{
 return true; //always allows to change tab
}

void CDevDiagnostTabController::OnOutputToggle(int output_id, bool state)
{
 _SetOuptutMapItem(output_id, state);
 UpdateOutputs(); //send outputs states to device
}

void CDevDiagnostTabController::OnEnterButton(void)
{
 //========================================================
 if (!CheckAppTitle(AfxGetMainWnd()) || !CheckAppLogo())
  return;
 //========================================================

 if (m_diagnost_mode_active)
 {
  //This command will make SECU-3 to leave diagnostic mode
  SECU3IO::OPCompNc packet_data;
  packet_data.opcode = SECU3IO::OPCODE_DIAGNOST_LEAVE;
  packet_data.opdata = 0;
  mp_comm->m_pControlApp->SendPacket(OP_COMP_NC, &packet_data);
  mp_view->EnableDiagControls(false);
 }
 else
 {
  //This command will make SECU-3 to enter diagnostic mode
  SECU3IO::OPCompNc packet_data;
  packet_data.opcode = SECU3IO::OPCODE_DIAGNOST_ENTER;
  packet_data.opdata = 0;
  mp_comm->m_pControlApp->SendPacket(OP_COMP_NC, &packet_data);
 }
}

void CDevDiagnostTabController::OnStartOutputsAutoTesting(void)
{
 if (m_current_tst_mode!=m_tstModes.end())
  return; //already started
 mp_view->EnableStartAutoTstMenuItem(false);
 mp_view->EnableStopAutoTstMenuItem(true);
 m_current_tst_mode = m_tstModes.begin();
 (*m_current_tst_mode)->Reset();
 m_tst_timer.SetTimer(400);
}

void CDevDiagnostTabController::OnStopOutputsAutoTesting(void)
{
 m_tst_timer.KillTimer();
 SetOutputValue(-1, false);
 UpdateOutputs();
 m_current_tst_mode = m_tstModes.end();
 mp_view->EnableStartAutoTstMenuItem(true);
 mp_view->EnableStopAutoTstMenuItem(false);
}

void CDevDiagnostTabController::OnEnableBLDETesting(bool enable)
{
 SetOutputValue(CDevDiagnostTabDlg::OID_BL, false);
 SetOutputValue(CDevDiagnostTabDlg::OID_DE, false);
 UpdateOutputs();
}

void CDevDiagnostTabController::OnTstTimer(void)
{
 if (!(*m_current_tst_mode)->Next())
 {
  if (++m_current_tst_mode == m_tstModes.end())
   m_current_tst_mode = m_tstModes.begin();
  (*m_current_tst_mode)->Reset();
 }
}

void CDevDiagnostTabController::UpdateOutputs(void)
{
 mp_comm->m_pControlApp->SendPacket(DIAGOUT_DAT, &m_outputs);
}

void CDevDiagnostTabController::SetOutputValue(int id, bool state)
{
 if (id == -1) //all
 {
  int outNum = m_outputs.mode ? CDevDiagnostTabDlg::OID_SECU3i_NUM : CDevDiagnostTabDlg::OID_SECU3T_NUM;
  for(int i = CDevDiagnostTabDlg::OID_IGN_OUT1; i < outNum; ++i)
  {
   mp_view->SetOutputValue(i, state);
   _SetOuptutMapItem(i, state);
  }
 }
 else //single
 {
  mp_view->SetOutputValue(id, state);
  _SetOuptutMapItem(id, state);
 }
}

void CDevDiagnostTabController::_SetOuptutMapItem(int id, bool state)
{
 if (m_outputs_map.find(id) == m_outputs_map.end())
  return;
  
 if (CDevDiagnostTabDlg::OID_BL==id || CDevDiagnostTabDlg::OID_DE==id)
  *m_outputs_map[id] = mp_view->IsBLDETestingEnabled() ? ((int)state)+1 : 0; //logic level or HiZ
 else
  *m_outputs_map[id] = state;
}

void CDevDiagnostTabController::_FillOutputsMap(void)
{
 int outNum = m_outputs.mode ? CDevDiagnostTabDlg::OID_SECU3i_NUM : CDevDiagnostTabDlg::OID_SECU3T_NUM;
 //Assosiate output state variables with IDs
 m_outputs_map.clear();
 for(int i = 0; i < outNum; ++i)
  m_outputs_map.insert(std::make_pair(CDevDiagnostTabDlg::OID_IGN_OUT1 + i, &m_outputs.out[i]));
}
