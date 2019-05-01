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

/** \file InjDriverTabController.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "Resources/resource.h"
#include "Application/CommunicationManager.h"
#include "common/fastdelegate.h"
#include "common/MathHelpers.h"
#include "InjDriverTabController.h"
#include "MainFrame/StatusBarManager.h"
#include "TabDialogs/InjDriverTabDlg.h"
#include "io-core/BitMask.h"
#include "Settings/ISettingsData.h"
#include "InjDrvFileDataIO.h"

using namespace fastdelegate;
using namespace SECU3IO;

#define EHKEY _T("InjDriverCntr")

CInjDriverTabController::CInjDriverTabController(CInjDriverTabDlg* ip_view, CCommunicationManager* ip_comm, CStatusBarManager* ip_sbar, ISettingsData* ip_settings)
: mp_view(ip_view)
, mp_comm(ip_comm)
, mp_sbar(ip_sbar)
, mp_sett(ip_settings)
, m_initialized(false)
, m_saving_proc_state(0)
, m_fw_loaded(false)
{
 std::fill(m_initFlags, m_initFlags + 2, false);

 //set delegates:
 mp_view->setOnChange(MakeDelegate(this, &CInjDriverTabController::OnChange));
 mp_view->setOnSaveButton(MakeDelegate(this, &CInjDriverTabController::OnSaveParameters));
 mp_view->setOnExportToAFile(MakeDelegate(this, &CInjDriverTabController::OnExportToAFile));
 mp_view->setOnImportFromAFile(MakeDelegate(this, &CInjDriverTabController::OnImportFromAFile));
 mp_view->setOnSaveToFirmware(MakeDelegate(this, &CInjDriverTabController::OnSaveToFirmware));
 mp_view->setOnLoadFromFirmware(MakeDelegate(this, &CInjDriverTabController::OnLoadFromFirmware));
 mp_view->setOnShowFirmwareInfo(MakeDelegate(this, &CInjDriverTabController::OnShowFirmwareInfo));
 mp_view->setOnExitOfflineMode(MakeDelegate(this, &CInjDriverTabController::OnExitOfflineMode));
}

CInjDriverTabController::~CInjDriverTabController()
{
 //empty
}

//изменились настройки программы!
void CInjDriverTabController::OnSettingsChanged(int action)
{
 if (action == 2) //child charts check in main menu
  return;

 mp_view->SetVoltLineColor(mp_sett->GetVoltLineColor());
 mp_view->SetPtMovStep(mp_sett->GetPeakOnPtMovStep(), mp_sett->GetPeakDutyPtMovStep(), mp_sett->GetHoldDutyPtMovStep());

 //включаем необходимый для данного контекста коммуникационный контроллер
 mp_comm->SwitchOn(CCommunicationManager::OP_ACTIVATE_APPLICATION, true);
}

//from MainTabController
void CInjDriverTabController::OnActivate(void)
{
 mp_comm->m_pAppAdapter->AddEventHandler(this,EHKEY);
 mp_comm->setOnSettingsChanged(MakeDelegate(this, &CInjDriverTabController::OnSettingsChanged));

 //включаем необходимый для данного контекста коммуникационный контроллер
 mp_comm->SwitchOn(CCommunicationManager::OP_ACTIVATE_APPLICATION);

 //симулируем изменение состояния для обновления контроллов, так как OnConnection вызывается только если
 //сбрывается или разрывается принудительно (путем деактивации коммуникационного контроллера)
 bool online_status = mp_comm->m_pControlApp->GetOnlineStatus();
 OnConnection(online_status);

 mp_view->SetVoltLineColor(mp_sett->GetVoltLineColor());
 mp_view->SetPtMovStep(mp_sett->GetPeakOnPtMovStep(), mp_sett->GetPeakDutyPtMovStep(), mp_sett->GetHoldDutyPtMovStep());

 if (m_fw_loaded)
 { //offline mode
  mp_view->SetOfflineCheck();
  mp_view->SetValues(&m_fwdata[0]);
  mp_view->SetValues(&m_fwdata[1]);
  mp_view->EnableAll(true);
 }
}

//from MainTabController
void CInjDriverTabController::OnDeactivate(void)
{
 mp_comm->m_pAppAdapter->RemoveEventHandler(EHKEY);
 mp_sbar->SetInformationText(_T(""));
}

void CInjDriverTabController::OnPacketReceived(const BYTE i_descriptor, SECU3IO::SECU3Packet* ip_packet)
{
 //особый случай: пришел пакет с нотификацонным кодом
 if (i_descriptor == OP_COMP_NC)
 {
  const OPCompNc* p_ndata = (OPCompNc*)ip_packet;
  switch(p_ndata->opcode)
  {
   case OPCODE_EEPROM_PARAM_SAVE: //параметры были сохранены
    mp_sbar->SetInformationText(MLL::LoadString(IDS_KC_PARAMETERS_HAS_BEEN_SAVED));
    return;
   case OPCODE_SAVE_TABLSET:      //таблицы были сохранены
    mp_sbar->SetInformationText(MLL::LoadString(IDS_PM_TABLSET_HAS_BEEN_SAVED));    
    return;
  }
 }

 if (i_descriptor != INJDRV_PAR)
 {
  mp_comm->m_pControlApp->ChangeContext(SILENT);  
  return;
 }
 else
 {
  SECU3IO::InjDrvPar* data = (SECU3IO::InjDrvPar*)ip_packet;
  if (data->ee_status == false && m_saving_proc_state == 1)
  {
   mp_sbar->SetInformationText(MLL::LoadString(IDS_INJDRV_SAVING_EEPROM));
   m_saving_proc_state++;
  }
  else if (data->ee_status == true && m_saving_proc_state == 2)
  {
   mp_sbar->SetInformationText(MLL::LoadString(IDS_PM_READY));
   m_saving_proc_state = 0;
   mp_view->EnableSaveBtn(true);
  }
 }

 if (!m_initialized)
 {
  SECU3IO::InjDrvPar* data = (SECU3IO::InjDrvPar*)ip_packet;
  mp_view->SetValues(data);
  m_initFlags[data->set_idx] = true;
  if (m_initFlags[0] == true && m_initFlags[1] == true)
  { //has collected all necessary data!
   mp_view->EnableAll(true);
   mp_view->EnableVoltage(true);
   mp_view->EnableSaveBtn(data->ee_status);
   m_initialized = true;   
   mp_sbar->SetInformationText(MLL::LoadString(IDS_PM_READY));    
  }
 }
 else
 { //update voltage
  SECU3IO::InjDrvPar* data = (SECU3IO::InjDrvPar*)ip_packet;
  mp_view->SetValues(data, true);
 } 
}

void CInjDriverTabController::OnConnection(const bool i_online)
{
 int state;
 ASSERT(mp_sbar);

 if (i_online) //перешли в онлайн
 {
  mp_sbar->SetInformationText(MLL::LoadString(IDS_INJDRV_READING_SETTINGS));    
  state = CStatusBarManager::STATE_ONLINE;
  std::fill(m_initFlags, m_initFlags + 2, false);
  m_initialized = false;
 }
 else
 {
  state = CStatusBarManager::STATE_OFFLINE;
  std::fill(m_initFlags, m_initFlags + 2, false);
  m_initialized = false;
 }

 if (i_online==false)
 { //здесь только запрещаем, разрешим в другом месте после выполнения подготовительных операций
  mp_view->EnableAll(i_online || m_fw_loaded);
  mp_view->EnableVoltage(i_online);
  mp_view->EnableSaveBtn(i_online);
 }

 mp_sbar->SetConnectionState(state);
}

bool CInjDriverTabController::OnClose(void)
{
 mp_comm->m_pControlApp->ChangeContext(SENSOR_DAT); //set default context before exiting
 Sleep(100);
 return true;
}

void CInjDriverTabController::OnCloseNotify(void)
{
  //empty
}

void CInjDriverTabController::OnFullScreen(bool i_what)
{
 //empty
}

bool CInjDriverTabController::OnAskChangeTab(void)
{
 SECU3IO::InjDrvPar params;
 params.set_idx = 0; //specify desired set to obtain
 mp_view->GetValues(&params);
 m_fwdata[0] = params;
 params.set_idx = 1; //specify desired set to obtain
 mp_view->GetValues(&params);
 m_fwdata[1] = params;

 return true; //always allows to change tab
}

void CInjDriverTabController::OnChange(void)
{
 int set_idx = mp_view->GetCurrSetIdx();
 SECU3IO::InjDrvPar params;
 params.set_idx = set_idx; //specify desired set to obtain
 mp_view->GetValues(&params);
 params.ee_status = false; //no command
 mp_comm->m_pControlApp->SendPacket(INJDRV_PAR, &params);
}

void CInjDriverTabController::OnSaveParameters(void)
{
 m_saving_proc_state = 1;
 mp_view->EnableSaveBtn(false);
 int set_idx = mp_view->GetCurrSetIdx();
 SECU3IO::InjDrvPar params;
 params.set_idx = set_idx; //specify desired set to obtain
 mp_view->GetValues(&params);
 params.ee_status = true; //this will say firmware to save data int EEPROM
 mp_comm->m_pControlApp->SendPacket(INJDRV_PAR, &params);
}

void CInjDriverTabController::OnExportToAFile(void)
{
 int set_idx = mp_view->GetCurrSetIdx();
 SECU3IO::InjDrvPar params;
 params.set_idx = set_idx; //specify desired set to obtain
 mp_view->GetValues(&params);
 CInjDrvFileDataIO::ExportSet(&params);
}

void CInjDriverTabController::OnImportFromAFile(void)
{
 SECU3IO::InjDrvPar params;
 if (!CInjDrvFileDataIO::ImportSet(&params))
  return;
 int set_idx = mp_view->GetCurrSetIdx();
 params.set_idx = set_idx;
 mp_view->SetValues(&params);
 params.ee_status = false; //no command
 mp_comm->m_pControlApp->SendPacket(INJDRV_PAR, &params);
}

void CInjDriverTabController::OnSaveToFirmware(void)
{
 SECU3IO::InjDrvPar params[2];
 params[0].set_idx = 0;
 params[1].set_idx = 1;
 mp_view->GetValues(&params[0]);
 mp_view->GetValues(&params[1]);
 CInjDrvFileDataIO::SaveSetsToFirmware(params);
}

void CInjDriverTabController::OnLoadFromFirmware(void)
{
 SECU3IO::InjDrvPar params[2];
 if (!CInjDrvFileDataIO::LoadSetsFromFirmware(params))
  return;
 params[0].set_idx = 0;
 params[1].set_idx = 1;

 if (mp_comm->m_pControlApp->GetOnlineStatus())
 {
  //send to LZID
  AfxGetMainWnd()->BeginWaitCursor();
  mp_comm->m_pControlApp->SendPacket(INJDRV_PAR, &params[0]);
  Sleep(100);
  mp_comm->m_pControlApp->SendPacket(INJDRV_PAR, &params[1]);
  AfxGetMainWnd()->EndWaitCursor();
 }
 else
 { //offline
  params[0].voltage = .0f;
  params[1].voltage = .0f;
 }

 mp_view->SetValues(&params[0]);
 mp_view->SetValues(&params[1]);

 mp_view->EnableAll(true);

 m_fw_loaded = true;
 mp_view->SetOfflineCheck();
 
 mp_sbar->SetInformationText(MLL::LoadString(IDS_INJDRV_LOADED_FROM_FW));    
}

void CInjDriverTabController::OnShowFirmwareInfo(void)
{
 int set_idx = mp_view->GetCurrSetIdx();

 SECU3IO::InjDrvPar params;
 params.set_idx = set_idx; //specify desired set to obtain
 mp_view->GetValues(&params);

 CString str;
 int ver_maj = params.version >> 4;
 int ver_min = params.version & 0xF;
 char* strDrvType = "0 - Unknown";
 if (params.type == 1)
  strDrvType = "1 - SECU-LZID4 (mega1284)";
 else if (params.type == 2)
  strDrvType = "2 - SECU-LZID5 (mega328PB)";
 else
  strDrvType = "Unknown type";

 char* inv_pwm = CHECKBIT8(params.fw_opt, 0) ? "Yes" : "No"; 
 char* inv_flb = CHECKBIT8(params.fw_opt, 1) ? "Yes" : "No"; 
 char* cur_cut = CHECKBIT8(params.fw_opt, 2) ? "Yes" : "No"; 
 char* inv_inp = CHECKBIT8(params.fw_opt, 3) ? "Yes" : "No"; 
 char* five_ch = CHECKBIT8(params.fw_opt, 4) ? "Yes" : "No"; 
 str.Format("Firmware version: v%d.%d\nDriver type: %s\nInv. PWM outputs: %s\nInv. FLB outputs: %s\nInv. inputs: %s\nCurrent cut off: %s\nFive channels: %s\n", ver_maj, ver_min, strDrvType, inv_pwm, inv_flb, inv_inp, cur_cut, five_ch);
 AfxMessageBox(str, MB_ICONINFORMATION);
}

void CInjDriverTabController::OnExitOfflineMode(void)
{
 m_fw_loaded = false;
 if (!mp_comm->m_pControlApp->GetOnlineStatus())
  mp_view->EnableAll(false);
}
