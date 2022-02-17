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
#include "ErrorMsg.h"
#include "FWImpExp/InjDrvFWMCntr.h"
#include "FWImpExp/InjDrvStockCntr.h"
#include "ui-core/MsgBox.h"

#pragma warning(disable : 4355)  

using namespace fastdelegate;
using namespace SECU3IO;

#define EHKEY _T("InjDriverCntr")
#define MCU_FLASH_SIZE 32768
#define MCU_EEPROM_SIZE 1024
#define BOOTLDR_SIZE   1024

CInjDriverTabController::CInjDriverTabController(CInjDriverTabDlg* ip_view, CCommunicationManager* ip_comm, CStatusBarManager* ip_sbar, ISettingsData* ip_settings)
: mp_view(ip_view)
, mp_comm(ip_comm)
, mp_sbar(ip_sbar)
, mp_sett(ip_settings)
, m_initialized(false)
, m_saving_proc_state(0)
, m_fw_loaded(false)
, m_lzidblhs_tmr(this, &CInjDriverTabController::OnLzidBlHsTimer)
, m_address_tmr(this, &CInjDriverTabController::OnAddressTimer)
, m_bl_op(0)
, m_active(false)
, m_recv_hs(false)
{
 std::fill(m_initFlags, m_initFlags + 2, false);
 m_bl_data.resize(MCU_FLASH_SIZE);

 //set delegates:
 mp_view->setOnChange(MakeDelegate(this, &CInjDriverTabController::OnChange));
 mp_view->setOnSaveButton(MakeDelegate(this, &CInjDriverTabController::OnSaveParameters));
 mp_view->setOnExportToAFile(MakeDelegate(this, &CInjDriverTabController::OnExportToAFile));
 mp_view->setOnImportFromAFile(MakeDelegate(this, &CInjDriverTabController::OnImportFromAFile));
 mp_view->setOnImportFromStock(MakeDelegate(this, &CInjDriverTabController::OnImportFromStock));
 mp_view->setOnSaveToFirmware(MakeDelegate(this, &CInjDriverTabController::OnSaveToFirmware));
 mp_view->setOnLoadFromFirmware(MakeDelegate(this, &CInjDriverTabController::OnLoadFromFirmware));
 mp_view->setOnShowFirmwareInfo(MakeDelegate(this, &CInjDriverTabController::OnShowFirmwareInfo));
 mp_view->setOnExitOfflineMode(MakeDelegate(this, &CInjDriverTabController::OnExitOfflineMode));
 mp_view->setOnWriteFirmwareFromFile(MakeDelegate(this, &CInjDriverTabController::OnWriteFirmwareFromFile));
 mp_view->setOnReadFirmwareToFile(MakeDelegate(this, &CInjDriverTabController::OnReadFirmwareToFile));
 mp_view->setOnReadLzblInfo(MakeDelegate(this, &CInjDriverTabController::OnReadLzblInfo));
 mp_view->setOnWriteEEPROMFromFile(MakeDelegate(this, &CInjDriverTabController::OnWriteEEPROMFromFile));
 mp_view->setOnReadEEPROMToFile(MakeDelegate(this, &CInjDriverTabController::OnReadEEPROMToFile));
 mp_view->setOnResetEEPROM(MakeDelegate(this, &CInjDriverTabController::OnResetEEPROM));
 mp_view->setOnSelInjDrv(MakeDelegate(this, &CInjDriverTabController::OnSelInjDrv));
 mp_view->setOnFirmwareMaster(MakeDelegate(this, &CInjDriverTabController::OnFirmwareMaster));
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
 mp_view->SetPtMovStep(mp_sett->GetPeakOnPtMovStep(), mp_sett->GetPeakDutyPtMovStep(), mp_sett->GetHoldDutyPtMovStep(), mp_sett->GetPeakFullPtMovStep(), mp_sett->GetPthPausePtMovStep(), mp_sett->GetPWMulPtMovStep(), mp_sett->GetPWAddPtMovStep());

 //включаем необходимый для данного контекста коммуникационный контроллер
 VERIFY(mp_comm->SwitchOn(CCommunicationManager::OP_ACTIVATE_APPLICATION, true));
}

//from MainTabController
void CInjDriverTabController::OnActivate(void)
{
 mp_comm->m_pBootLoader->EnableBlockedEEPROMOps(false); //driver's BL doesn't not support blocked data transfer for EEPROM R/W
 m_bl_op = 0;
 m_active = true;
 mp_comm->m_pAppAdapter->AddEventHandler(this,EHKEY);
 mp_comm->m_pBldAdapter->SetEventHandler(this);
 mp_comm->setOnSettingsChanged(MakeDelegate(this, &CInjDriverTabController::OnSettingsChanged));

 //включаем необходимый для данного контекста коммуникационный контроллер
 VERIFY(mp_comm->SwitchOn(CCommunicationManager::OP_ACTIVATE_APPLICATION));

 //симулируем изменение состояния для обновления контроллов, так как OnConnection вызывается только если
 //сбрывается или разрывается принудительно (путем деактивации коммуникационного контроллера)
 bool online_status = mp_comm->m_pControlApp->GetOnlineStatus();
 OnConnection(online_status);

 mp_view->SetVoltLineColor(mp_sett->GetVoltLineColor());
 mp_view->SetPtMovStep(mp_sett->GetPeakOnPtMovStep(), mp_sett->GetPeakDutyPtMovStep(), mp_sett->GetHoldDutyPtMovStep(), mp_sett->GetPeakFullPtMovStep(), mp_sett->GetPthPausePtMovStep(), mp_sett->GetPWMulPtMovStep(), mp_sett->GetPWAddPtMovStep());

 mp_view->SetShowGraphLabels(mp_sett->GetShowGraphLabels());

 if (m_fw_loaded)
 { //offline mode
  mp_view->SetOfflineCheck();
  mp_view->SetValues(&m_fwdata[0]);
  mp_view->SetValues(&m_fwdata[1]);
  mp_view->EnableAll(true);
 }

 mp_comm->m_pControlApp->ChangeContext(SILENT);
 mp_view->EnableBLItems(true);

 //select device depending on the drop down list
 SECU3IO::InjDrvAdr adr;
 adr.dev_address = mp_view->GetInjDrvSel();
 mp_comm->m_pControlApp->SendPacket(INJDRV_ADR, &adr);
}

//from MainTabController
void CInjDriverTabController::OnDeactivate(void)
{
 m_active = false;
 mp_comm->m_pAppAdapter->RemoveEventHandler(EHKEY);
 mp_comm->m_pBootLoader->AbortOperation();
 mp_comm->m_pBldAdapter->DetachEventHandler();
 mp_sbar->SetInformationText(_T(""));
 mp_sbar->ShowProgressBar(false);
 m_lzidblhs_tmr.KillTimer();
 m_address_tmr.KillTimer();
 mp_comm->m_pBootLoader->SetPlatformParameters(PlatformParamHolder(mp_sett->GetECUPlatformType()));
 m_bl_op = 0;
 mp_comm->m_pControlApp->ChangeContext(SENSOR_DAT); //set default context before exiting
}

void CInjDriverTabController::OnPacketReceived(const BYTE i_descriptor, SECU3IO::SECU3Packet* ip_packet)
{
 //особый случай: пришел пакет с нотификацонным кодом
 if (i_descriptor == OP_COMP_NC)
 {
  const OPCompNc* p_ndata = (OPCompNc*)ip_packet;
  switch(p_ndata->opcode)
  {
   case OPCODE_SAVE_LTFT: //LTFT table had been saved
    mp_sbar->SetInformationText(MLL::LoadString(IDS_CE_LTFT_HAS_BEEN_SAVED));
    return;
   case OPCODE_RESET_LTFT: //LTFT table had been reset
    mp_sbar->SetInformationText(MLL::LoadString(IDS_CE_LTFT_HAS_BEEN_RESET));
    return;
   case OPCODE_EEPROM_PARAM_SAVE: //параметры были сохранены
    mp_sbar->SetInformationText(MLL::LoadString(IDS_KC_PARAMETERS_HAS_BEEN_SAVED));
    return;
   case OPCODE_SAVE_TABLSET:      //таблицы были сохранены
    mp_sbar->SetInformationText(MLL::LoadString(IDS_PM_TABLSET_HAS_BEEN_SAVED));    
    return;
  }
 }

 if (!mp_comm->m_pBootLoader->IsIdle() || m_recv_hs)
  return; //do not do any operations if boot loader is active

 if (i_descriptor == LZIDBL_HS)
 {
  const LzidBLHS* p_data = (LzidBLHS*)ip_packet;
  if (0==strncmp(p_data->data, "LZBL", 4))
  {
   m_recv_hs = true;
   m_lzidblhs_tmr.KillTimer(); //got answer!
   Sleep(300);
   //activate communication controller of the boot loader
   VERIFY(mp_comm->SwitchOn(CCommunicationManager::OP_ACTIVATE_BOOTLOADER));
   mp_sbar->SetConnectionState(CStatusBarManager::STATE_BOOTLOADER);
   if (m_bl_op == 1) //deferred reading of flash
    mp_comm->m_pBootLoader->StartOperation(CBootLoader::BL_OP_READ_FLASH, &m_bl_data[0], MCU_FLASH_SIZE); //operation does not block thread
   if (m_bl_op == 2) //deferred writing of flash
    mp_comm->m_pBootLoader->StartOperation(CBootLoader::BL_OP_WRITE_FLASH, &m_bl_data[0], MCU_FLASH_SIZE-BOOTLDR_SIZE); //operation does not block thread
   if (m_bl_op == 3) //deferred reading of boot loader information
    mp_comm->m_pBootLoader->StartOperation(CBootLoader::BL_OP_READ_SIGNATURE, &m_bl_data[0], 0);
   if (m_bl_op == 4) //deferred reading of the EEPROM
    mp_comm->m_pBootLoader->StartOperation(CBootLoader::BL_OP_READ_EEPROM, &m_bl_data[0], 0);
   if (m_bl_op == 5) //deferred writing to the EEPROM
    mp_comm->m_pBootLoader->StartOperation(CBootLoader::BL_OP_WRITE_EEPROM, &m_bl_data[0], 0);
   mp_sbar->ShowProgressBar(true);
   mp_sbar->SetProgressPos(0);
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
  if ((int)data->dev_address == mp_view->GetInjDrvSel())
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
  m_address_tmr.KillTimer();
 }
 else
 {
  state = CStatusBarManager::STATE_OFFLINE;
  std::fill(m_initFlags, m_initFlags + 2, false);
  m_initialized = false;
  m_address_tmr.SetTimer(250);
 }

 if (i_online==false)
 { //здесь только запрещаем, разрешим в другом месте после выполнения подготовительных операций
  mp_view->EnableAll(i_online || m_fw_loaded);
  mp_view->EnableVoltage(i_online);
  mp_view->EnableSaveBtn(i_online);
 }

 if (mp_comm->m_pBootLoader->IsIdle())
  mp_sbar->SetConnectionState(state);
 else
  mp_sbar->SetConnectionState(CStatusBarManager::STATE_BOOTLOADER);
}

bool CInjDriverTabController::OnClose(void)
{
 mp_comm->m_pControlApp->ChangeContext(SENSOR_DAT); //set default context before exiting
 Sleep(100);
 if (m_active && (!mp_comm->m_pBootLoader->IsIdle()))
  if (!ErrorMsg::AskUserAboutTabLeaving())
   return false;
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

 if (mp_comm->m_pBootLoader->IsIdle())
  return true; //allows
 return ErrorMsg::AskUserAboutTabLeaving();
}

void CInjDriverTabController::OnSaveSettings(void)
{
 //empty
}

void CInjDriverTabController::OnChange(void)
{
 int set_idx = mp_view->GetCurrSetIdx();
 SECU3IO::InjDrvPar params;
 params.set_idx = set_idx; //specify desired set to obtain
 mp_view->GetValues(&params);
 params.ee_status = false; //no command
 params.start_bldr = false;
 params.reset_eeprom = false;
 params.dev_address = mp_view->GetInjDrvSel();
 params.broadcast = mp_view->GetEditSimultanCheck();
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
 params.ee_status = true; //this will say firmware to save data into EEPROM
 params.start_bldr = false;
 params.reset_eeprom = false;
 params.dev_address = mp_view->GetInjDrvSel();
 params.broadcast = mp_view->GetEditSimultanCheck();
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
 int set_idx = mp_view->GetCurrSetIdx();
 SECU3IO::InjDrvPar params;
 params.set_idx = set_idx; //specify desired set to obtain
 mp_view->GetValues(&params);
 if (!CInjDrvFileDataIO::ImportSet(&params))
  return;
 params.set_idx = set_idx;
 mp_view->SetValues(&params);
 params.ee_status = false; //no command
 params.start_bldr = false;
 params.reset_eeprom = false;
 params.dev_address = mp_view->GetInjDrvSel();
 params.broadcast = mp_view->GetEditSimultanCheck();
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
 params[0].dev_address = mp_view->GetInjDrvSel();
 params[0].broadcast = mp_view->GetEditSimultanCheck();

 params[1].set_idx = 1;
 params[1].dev_address = mp_view->GetInjDrvSel();
 params[1].broadcast = mp_view->GetEditSimultanCheck();

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
 else if (params.type == 3)
  strDrvType = "3 - SECU-LZID2 (mega328)";
 else
  strDrvType = "Unknown type";

 char* inv_pwm = CHECKBIT8(params.fw_opt, 0) ? "Yes" : "No"; 
 char* inv_flb = CHECKBIT8(params.fw_opt, 1) ? "Yes" : "No"; 
 char* cur_cut = CHECKBIT8(params.fw_opt, 2) ? "Yes" : "No"; 
 char* inv_inp = CHECKBIT8(params.fw_opt, 3) ? "Yes" : "No"; 
 char* five_ch = CHECKBIT8(params.fw_opt, 4) ? "Yes" : "No"; 
 char* ld_flash = CHECKBIT8(params.fw_opt, 5) ? "Yes" : "No"; 
 char* pwcorr = CHECKBIT8(params.fw_opt, 6) ? "Yes" : "No";
 str.Format("Firmware version: v%d.%d\nDriver type: %s\nInv. PWM outputs: %s\nInv. FLB outputs: %s\nInv. inputs: %s\nCurrent cut off: %s\nFive channels: %s\nLoad from flash: %s\nPW corrections: %s\n", ver_maj, ver_min, strDrvType, inv_pwm, inv_flb, inv_inp, cur_cut, five_ch, ld_flash, pwcorr);
 SECUMessageBox(str, MB_ICONINFORMATION);
}

void CInjDriverTabController::OnExitOfflineMode(void)
{
 m_fw_loaded = false;
 if (!mp_comm->m_pControlApp->GetOnlineStatus())
  mp_view->EnableAll(false);
}

void CInjDriverTabController::OnWriteFirmwareFromFile(void)
{
 if (!mp_comm->m_pBootLoader->IsIdle())
  return;

 if (!CInjDrvFileDataIO::LoadFirmware(m_bl_data))
  return; //canceled by user or error

  mp_comm->m_pBootLoader->SetPlatformParameters(PlatformParamHolder(EP_ATMEGA328PB));


 //запускаем бутлоадер по команде из приложения
 if (StartBootLoader(2))
 {
  //activate communication controller of the boot loader
  VERIFY(mp_comm->SwitchOn(CCommunicationManager::OP_ACTIVATE_BOOTLOADER));
  mp_sbar->SetConnectionState(CStatusBarManager::STATE_BOOTLOADER);

  //operation does not block thread
  mp_comm->m_pBootLoader->StartOperation(CBootLoader::BL_OP_WRITE_FLASH, &m_bl_data[0], MCU_FLASH_SIZE-BOOTLDR_SIZE);

  mp_sbar->ShowProgressBar(true);
  mp_sbar->SetProgressPos(0);
 }
}

void CInjDriverTabController::OnReadFirmwareToFile(void)
{
 if (!mp_comm->m_pBootLoader->IsIdle())
  return;

 mp_comm->m_pBootLoader->SetPlatformParameters(PlatformParamHolder(EP_ATMEGA328PB));

 //запускаем бутлоадер по команде из приложения
 if (StartBootLoader(1))
 {
  //activate communication controller of the boot loader
  VERIFY(mp_comm->SwitchOn(CCommunicationManager::OP_ACTIVATE_BOOTLOADER));
  mp_sbar->SetConnectionState(CStatusBarManager::STATE_BOOTLOADER);

  //operation does not block thread
  mp_comm->m_pBootLoader->StartOperation(CBootLoader::BL_OP_READ_FLASH, &m_bl_data[0], MCU_FLASH_SIZE);

  mp_sbar->ShowProgressBar(true);
  mp_sbar->SetProgressPos(0);
 }
}

void CInjDriverTabController::OnReadLzblInfo(void)
{
 if (!mp_comm->m_pBootLoader->IsIdle())
  return;

 mp_comm->m_pBootLoader->SetPlatformParameters(PlatformParamHolder(EP_ATMEGA328PB));

 //запускаем бутлоадер по команде из приложения
 if (StartBootLoader(3))
 {
  //activate communication controller of the boot loader
  VERIFY(mp_comm->SwitchOn(CCommunicationManager::OP_ACTIVATE_BOOTLOADER));
  mp_sbar->SetConnectionState(CStatusBarManager::STATE_BOOTLOADER);

  //operation does not block thread
  mp_comm->m_pBootLoader->StartOperation(CBootLoader::BL_OP_READ_SIGNATURE, &m_bl_data[0], 0);

  mp_sbar->ShowProgressBar(true);
  mp_sbar->SetProgressPos(0);
 }
}

void CInjDriverTabController::OnReadEEPROMToFile(void)
{
 if (!mp_comm->m_pBootLoader->IsIdle())
  return;

 mp_comm->m_pBootLoader->SetPlatformParameters(PlatformParamHolder(EP_ATMEGA328PB));

 //запускаем бутлоадер по команде из приложения
 if (StartBootLoader(4))
 {
  //activate communication controller of the boot loader
  VERIFY(mp_comm->SwitchOn(CCommunicationManager::OP_ACTIVATE_BOOTLOADER));
  mp_sbar->SetConnectionState(CStatusBarManager::STATE_BOOTLOADER);

  //operation does not block thread
  mp_comm->m_pBootLoader->StartOperation(CBootLoader::BL_OP_READ_EEPROM, &m_bl_data[0], 0);

  mp_sbar->ShowProgressBar(true);
  mp_sbar->SetProgressPos(0);
 }
}

void CInjDriverTabController::OnWriteEEPROMFromFile(void)
{
 if (!mp_comm->m_pBootLoader->IsIdle())
  return;

 if (!CInjDrvFileDataIO::LoadEEPROM(m_bl_data, MCU_EEPROM_SIZE))
  return; //canceled by user or error

 mp_comm->m_pBootLoader->SetPlatformParameters(PlatformParamHolder(EP_ATMEGA328PB));

 //запускаем бутлоадер по команде из приложения
 if (StartBootLoader(5))
 {
  //activate communication controller of the boot loader
  VERIFY(mp_comm->SwitchOn(CCommunicationManager::OP_ACTIVATE_BOOTLOADER));
  mp_sbar->SetConnectionState(CStatusBarManager::STATE_BOOTLOADER);

  //operation does not block thread
  mp_comm->m_pBootLoader->StartOperation(CBootLoader::BL_OP_WRITE_EEPROM, &m_bl_data[0], 0);

  mp_sbar->ShowProgressBar(true);
  mp_sbar->SetProgressPos(0);
 }
}

void CInjDriverTabController::OnLzidBlHsTimer(void)
{
 if (!m_recv_hs)
 {
  SECU3IO::LzidBLHS packet;
  strncpy(packet.data, (mp_view->GetInjDrvSel() ? "3MAN1" : "3MAN0"), 5);
  mp_comm->m_pControlApp->SendPacket(LZIDBL_HS, &packet);
 }
}

bool CInjDriverTabController::StartBootLoader(int op)
{
 if (mp_comm->m_pControlApp->GetOnlineStatus())
 { //start boot loader using special command
  SECU3IO::InjDrvPar params;
  memset(&params, 0, sizeof(SECU3IO::InjDrvPar));
  params.start_bldr = true; //command to start boot loader! 
  params.dev_address = mp_view->GetInjDrvSel();
  mp_comm->m_pControlApp->SendPacket(INJDRV_PAR, &params);
  m_bl_op = op;
  Sleep(100);
  return true; //started immediately
 }
 else
 { //start boot loader using "handshake" mechanism
  mp_sbar->SetInformationText(MLL::LoadString(IDS_FW_WAITING_BOOTLOADER));
  mp_sbar->SetConnectionState(CStatusBarManager::STATE_HANDSHAKE);
  m_recv_hs = false;
  m_lzidblhs_tmr.SetTimer(10);
  m_bl_op = op;
  mp_comm->m_pControlApp->IgnoreNPackets(3);
  return false; //deferred start (using a "handshake" mechanism)
 }
}

bool CInjDriverTabController::ExitBootLoader(void)
{
 ASSERT(mp_comm);
 return mp_comm->m_pBootLoader->StartOperation(CBootLoader::BL_OP_EXIT, NULL, 0);
}

/////////////////////////////////////////////////////////////////////////////////
void CInjDriverTabController::OnUpdateUI(IBLDEventHandler::poolUpdateUI* ip_data)
{
 ASSERT(ip_data);
 IBLDEventHandler::poolUpdateUI data;

 /////////////////////////////////////////////////////////////
 //exclusive access
 mp_comm->m_pBootLoader->EnterCriticalSection();
 data = *ip_data;
 mp_comm->m_pBootLoader->LeaveCriticalSection();
 /////////////////////////////////////////////////////////////

 if (data.opcode!=CBootLoader::BL_OP_EXIT) //do not show progress bar for boot loader's exit operation
 {
  mp_sbar->SetProgressRange(0, data.total);
  mp_sbar->SetProgressPos(data.current);
 }
}

void CInjDriverTabController::OnBegin(const int opcode, const int status)
{
 if (opcode == CBootLoader::BL_OP_READ_SIGNATURE)
  mp_sbar->SetInformationText(MLL::LoadString(IDS_FW_READING_SIGNATURE));
 if (opcode == CBootLoader::BL_OP_READ_FLASH)
  mp_sbar->SetInformationText(MLL::LoadString(IDS_FW_READING_FW));
 if (opcode == CBootLoader::BL_OP_WRITE_FLASH)
  mp_sbar->SetInformationText(MLL::LoadString(IDS_FW_WRITING_FW));
 if (opcode == CBootLoader::BL_OP_READ_EEPROM)
  mp_sbar->SetInformationText(MLL::LoadString(IDS_FW_READING_EEPROM));
 if (opcode == CBootLoader::BL_OP_WRITE_EEPROM)
  mp_sbar->SetInformationText(MLL::LoadString(IDS_FW_WRITING_EEPROM));

 if (opcode == CBootLoader::BL_OP_EXIT)
 {
  //Exiting from boot loader...
 }

 mp_view->EnableBLItems(false);
}

void CInjDriverTabController::OnEnd(const int opcode,const int status)
{
 switch(opcode)
 {
  //////////////////////////////////////////////////////////////////////
  case CBootLoader::BL_OP_EXIT:
  {
   //activate application's communication controller again
   VERIFY(mp_comm->SwitchOn(CCommunicationManager::OP_ACTIVATE_APPLICATION));
   m_recv_hs = false;
   break;
  }
  //////////////////////////////////////////////////////////////////////
  case CBootLoader::BL_OP_READ_SIGNATURE:
  {
   if (status==1)
   {
    m_bl_data[CBootLoader::BL_SIGNATURE_STR_LEN] = 0;
    mp_sbar->SetInformationText(&m_bl_data[0]);
    SECUMessageBox(CString(&m_bl_data[0]), MB_OK | MB_ICONINFORMATION);
   }
   else
   {
    mp_sbar->SetInformationText(ErrorMsg::GenerateErrorStr(mp_comm));
   }

   //ждем пока не выполнится предыдущая операция
   while(!mp_comm->m_pBootLoader->IsIdle());

   //Achtung! Почти рекурсия
   ExitBootLoader();

   Sleep(250);
   mp_sbar->ShowProgressBar(false);
   break;
  }
  //////////////////////////////////////////////////////////////////////
  case CBootLoader::BL_OP_READ_FLASH:
  {
   if (status==1)
   {
    mp_sbar->SetInformationText(MLL::LoadString(IDS_FW_FW_READ_SUCCESSFULLY));
    CInjDrvFileDataIO::SaveFirmware(m_bl_data);
   }
   else
   {
    mp_sbar->SetInformationText(ErrorMsg::GenerateErrorStr(mp_comm));
   }

   //ждем пока не выполнится предыдущая операция
   while(!mp_comm->m_pBootLoader->IsIdle());

   ExitBootLoader();

   Sleep(250);
   mp_sbar->ShowProgressBar(false);
   break;
  }

  //////////////////////////////////////////////////////////////////////
  case CBootLoader::BL_OP_WRITE_FLASH:
  {
   if (status==1)
    mp_sbar->SetInformationText(MLL::LoadString(IDS_FW_FW_WRITTEN_SUCCESSFULLY));
   else
   {
    mp_sbar->SetInformationText(ErrorMsg::GenerateErrorStr(mp_comm));
   }

   //ждем пока не выполнится предыдущая операция
   while(!mp_comm->m_pBootLoader->IsIdle());

   //Achtung! Почти рекурсия
   ExitBootLoader();

   Sleep(250);
   mp_sbar->ShowProgressBar(false);
   break;
  }

  //////////////////////////////////////////////////////////////////////
  case CBootLoader::BL_OP_READ_EEPROM:
  {
   if (status==1)
   { //OK
    mp_sbar->SetInformationText(MLL::LoadString(IDS_FW_EEPROM_READ_SUCCESSFULLY));
    CInjDrvFileDataIO::SaveEEPROM(m_bl_data, MCU_EEPROM_SIZE);
   }
   else
   {
    mp_sbar->SetInformationText(ErrorMsg::GenerateErrorStr(mp_comm));
   }

   //ждем пока не выполнится предыдущая операция
   while(!mp_comm->m_pBootLoader->IsIdle());

   //Achtung! Почти рекурсия
   ExitBootLoader();

   Sleep(250);
   mp_sbar->ShowProgressBar(false);
   break;
  }

  //////////////////////////////////////////////////////////////////////
  case CBootLoader::BL_OP_WRITE_EEPROM:
  {
   if (status==1)
    mp_sbar->SetInformationText(MLL::LoadString(IDS_FW_EEPROM_WRITTEN_SUCCESSFULLY));
   else
    mp_sbar->SetInformationText(ErrorMsg::GenerateErrorStr(mp_comm));

   //ждем пока не выполнится предыдущая операция
   while(!mp_comm->m_pBootLoader->IsIdle());

   //Achtung! Почти рекурсия
   ExitBootLoader();

   Sleep(250);
   mp_sbar->ShowProgressBar(false);
   break;
  }

  //////////////////////////////////////////////////////////////////////
 }//switch
 mp_view->EnableBLItems(true);
 m_bl_op = 0;
}

void CInjDriverTabController::OnResetEEPROM(void)
{
 if (IDYES==SECUMessageBox(MLL::GetString(IDS_RESET_EEPROM_COMFIRMATION).c_str(), MB_YESNO|MB_DEFBUTTON2|MB_ICONEXCLAMATION))
 {
  m_saving_proc_state = 1;
  mp_view->EnableSaveBtn(false);
  mp_sbar->SetInformationText(MLL::LoadString(IDS_FW_RESET_EEPROM_STARTED));

  SECU3IO::InjDrvPar params;
  memset(&params, 0, sizeof(SECU3IO::InjDrvPar));
  params.ee_status = false; //no command
  params.start_bldr = false; //no command
  params.reset_eeprom = true; //reset eeprom command
  params.dev_address = mp_view->GetInjDrvSel();
  params.broadcast = mp_view->GetEditSimultanCheck();
  mp_comm->m_pControlApp->SendPacket(INJDRV_PAR, &params);
  mp_view->RedrawWindow();
  AfxGetMainWnd()->BeginWaitCursor();
  Sleep(300);
  AfxGetMainWnd()->EndWaitCursor();
 }
}

void CInjDriverTabController::OnSelInjDrv(void)
{
 //select device depending on the drop down list
 SECU3IO::InjDrvAdr adr;
 adr.dev_address = mp_view->GetInjDrvSel();
 mp_comm->m_pControlApp->SendPacket(INJDRV_ADR, &adr);
 Sleep(10);
 std::fill(m_initFlags, m_initFlags + 2, false);
 m_initialized = false;
}

void CInjDriverTabController::OnAddressTimer(void)
{
 if (!mp_comm->m_pBootLoader->IsIdle() || m_recv_hs || 0!=m_bl_op)
  return; //do not do any operations if boot loader is active
 //select device depending on the drop down list
 SECU3IO::InjDrvAdr adr;
 adr.dev_address = mp_view->GetInjDrvSel();
 mp_comm->m_pControlApp->SendPacket(INJDRV_ADR, &adr);
}

void CInjDriverTabController::OnFirmwareMaster()
{
 if (!mp_comm->m_pBootLoader->IsIdle())
  return;

 InjDrvFWMCntr cntr;

 int result = cntr.DoLoad(&m_bl_data[0], MCU_FLASH_SIZE);
 if (result != IDOK || !cntr.GetStatus())
  return; //canceled by user or error
 
 mp_comm->m_pBootLoader->SetPlatformParameters(PlatformParamHolder(EP_ATMEGA328PB));

 //запускаем бутлоадер по команде из приложения
 if (StartBootLoader(2))
 {
  //activate communication controller of the boot loader
  VERIFY(mp_comm->SwitchOn(CCommunicationManager::OP_ACTIVATE_BOOTLOADER));
  mp_sbar->SetConnectionState(CStatusBarManager::STATE_BOOTLOADER);

  //operation does not block thread
  mp_comm->m_pBootLoader->StartOperation(CBootLoader::BL_OP_WRITE_FLASH, &m_bl_data[0], MCU_FLASH_SIZE-BOOTLDR_SIZE);

  mp_sbar->ShowProgressBar(true);
  mp_sbar->SetProgressPos(0);
 }
}

void CInjDriverTabController::OnImportFromStock()
{
 int set_idx = mp_view->GetCurrSetIdx();
 SECU3IO::InjDrvPar params;
 params.set_idx = set_idx; //specify desired set to obtain
 mp_view->GetValues(&params);
 
 size_t buffSize = 2048;
 std::vector<BYTE> buff(buffSize);
 InjDrvStockCntr sc;
 if (IDCANCEL==sc.DoLoad(&buff[0], &buffSize))
  return;
 if (!CInjDrvFileDataIO::ImportSet(&params, &buff[0], buffSize))
  return;

 params.set_idx = set_idx;
 mp_view->SetValues(&params);
 params.ee_status = false; //no command
 params.start_bldr = false;
 params.reset_eeprom = false;
 params.dev_address = mp_view->GetInjDrvSel();
 params.broadcast = mp_view->GetEditSimultanCheck();
 mp_comm->m_pControlApp->SendPacket(INJDRV_PAR, &params);
}
