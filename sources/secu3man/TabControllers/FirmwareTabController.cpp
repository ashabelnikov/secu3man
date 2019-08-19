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

/** \file FirmwareTabController.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include <algorithm>
#include <limits>

#include "Resources/resource.h"
#include "FirmwareTabController.h"

#include "about/secu-3about.h"
#include "Application/CommunicationManager.h"
#include "common/FastDelegate.h"
#include "FirmwareFileUtils.h"
#include "FWImpExp/MPSZImpExpController.h"
#include "FWImpExp/SECUImpExpController.h"
#include "FWImpExp/S3FImpExpController.h"
#include "FWImpExp/EEPROMImpExpController.h"
#include "FWIORemappingController.h"
#include "FWRPMGridEditController.h"
#include "io-core/EEPROMDataMediator.h"
#include "io-core/FirmwareDataMediator.h"
#include "io-core/SECU3IO.h"
#include "io-core/ufcodes.h"
#include "io-core/bitmask.h"
#include "MainFrame/StatusBarManager.h"
#include "ParamDesk/Params/ParamDeskDlg.h"
#include "Settings/ISettingsData.h"
#include "TabControllersCommunicator.h"
#include "TabDialogs/FirmwareTabDlg.h"
#include "TablDesk/MapIds.h"
#include "TablDesk/TablesSetPanel.h"
#include "TablDesk/CESettingsDlg.h"
#include "ErrorMsg.h"
#include "ui-core/DynFieldsDialog.h"

using namespace fastdelegate;

#define EHKEY _T("FirmwareCntr")
#define DELAY_AFTER_BL_START 100
#define WAITING_BL_START_TIMEOUT 8000

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFirmwareTabController::CFirmwareTabController(CFirmwareTabDlg* i_view, CCommunicationManager* i_comm, CStatusBarManager* i_sbar, ISettingsData* ip_settings)
: mp_view(i_view)
, m_comm(i_comm)
, m_sbar(i_sbar)
, mp_settings(ip_settings)
, m_current_funset_index(-1)
, m_bl_read_flash_mode(MODE_RD_FLASH_TO_FILE)
, m_lastSel(0)
, m_bl_started_emergency(false)
, mp_iorCntr(new CFWIORemappingController(i_view->mp_IORemappingDlg.get()))
, m_moreSize(0)
, m_clear_sbar_txt_on_conn(false)
, m_read_fw_sig_info_flag(false)
, m_blFinishOpCB(NULL)
, MapWndScrPos(ip_settings)
, m_active(false)
{
 PlatformParamHolder holder(ip_settings->GetECUPlatformType());
 m_fpp = holder.GetFlashParameters();
 m_epp = holder.GetEepromParameters();
 m_fwdm = new CFirmwareDataMediator(holder.GetFlashParameters());
 ASSERT(m_fwdm);
 //Set parameters for speed sensor calculations and set clock frequency (16 or 20 mHz)
 m_fwdm->SetNumPulsesPer1Km(mp_settings->GetNumPulsesPer1Km());
 m_fwdm->SetQuartzFrq(PlatformParamHolder::GetQuartzFreq(mp_settings->GetECUPlatformType()));
 m_edm = new EEPROMDataMediator(holder.GetEepromParameters());
 m_edm->SetNumPulsesPer1Km(mp_settings->GetNumPulsesPer1Km());
 m_edm->SetQuartzFrq(PlatformParamHolder::GetQuartzFreq(mp_settings->GetECUPlatformType()));
 ASSERT(m_edm);

 m_bl_data = new BYTE[m_fpp.m_total_size + 1];
 ASSERT(m_bl_data);
 m_code_for_merge_with_overhead = new BYTE[m_fpp.m_total_size + 1];
 ASSERT(m_code_for_merge_with_overhead);

 //========================================================
 if (!CheckVersion() || !CheckAbout() || !CheckAppLogo())
  return;
 //========================================================

 //устанавливаем делегаты (обработчики событий от представления)
 mp_view->setOnBootLoaderInfo(MakeDelegate(this, &CFirmwareTabController::OnBootLoaderInfo));
 mp_view->setOnReadEepromToFile(MakeDelegate(this, &CFirmwareTabController::OnReadEepromToFile));
 mp_view->setOnWriteEepromFromFile(MakeDelegate(this, &CFirmwareTabController::OnWriteEepromFromFile));
 mp_view->setOnResetEeprom(MakeDelegate(this, &CFirmwareTabController::OnResetEeprom));
 mp_view->setOnReadFlashToFile(MakeDelegate(this, &CFirmwareTabController::OnReadFlashToFile));
 mp_view->setOnWriteFlashFromFile(MakeDelegate(this, &CFirmwareTabController::OnWriteFlashFromFile));
 mp_view->setOnBLStartedEmergency(MakeDelegate(this, &CFirmwareTabController::OnBLStartedEmergency));
 mp_view->setOnOpenFlashFromFile(MakeDelegate(this, &CFirmwareTabController::OnOpenFlashFromFile));
 mp_view->setOnFWInformationTextChanged(MakeDelegate(this, &CFirmwareTabController::OnFWInformationTextChanged));
 mp_view->setOnSaveFlashToFile(MakeDelegate(this, &CFirmwareTabController::OnSaveFlashToFile));
 mp_view->setIsFirmwareOpened(MakeDelegate(this, &CFirmwareTabController::IsFirmwareOpened));
 mp_view->setOnImportDataFromAnotherFW(MakeDelegate(this, &CFirmwareTabController::OnImportDataFromAnotherFW));
 mp_view->setOnImportDataFromSECU3(MakeDelegate(this, &CFirmwareTabController::OnImportDataFromSECU3));
 mp_view->setOnReadFlashFromSECU(MakeDelegate(this, &CFirmwareTabController::OnReadFlashFromSECU));
 mp_view->setOnWriteFlashToSECU(MakeDelegate(this, &CFirmwareTabController::OnWriteFlashToSECU));
 mp_view->setOnImportMapsFromMPSZ(MakeDelegate(this, &CFirmwareTabController::OnImportMapsFromMPSZ));
 mp_view->setOnImportMapsFromSECU3(MakeDelegate(this, &CFirmwareTabController::OnImportMapsFromSECU3));
 mp_view->setOnImportMapsFromS3F(MakeDelegate(this, &CFirmwareTabController::OnImportMapsFromS3F));
 mp_view->setOnImportDefParamsFromEEPROMFile(MakeDelegate(this, &CFirmwareTabController::OnImportDefParamsFromEEPROMFile));
 mp_view->setOnImportTablesFromEEPROMFile(MakeDelegate(this, &CFirmwareTabController::OnImportTablesFromEEPROMFile));
 mp_view->setOnExportMapsToMPSZ(MakeDelegate(this, &CFirmwareTabController::OnExportMapsToMPSZ));
 mp_view->setOnExportMapsToSECU3(MakeDelegate(this, &CFirmwareTabController::OnExportMapsToSECU3));
 mp_view->setOnExportMapsToS3F(MakeDelegate(this, &CFirmwareTabController::OnExportMapsToS3F));
 mp_view->setOnFirmwareInfo(MakeDelegate(this, &CFirmwareTabController::OnFirmwareInfo));
 mp_view->setOnViewFWOptions(MakeDelegate(this, &CFirmwareTabController::OnViewFWOptions));
 mp_view->setIsViewFWOptionsAvailable(MakeDelegate(this, &CFirmwareTabController::IsViewFWOptionsAvailable)); 
 mp_view->setIsIORemappingAvailable(MakeDelegate(this, &CFirmwareTabController::IsIORemappingAvailable)); 
 mp_view->setOnDropFile(MakeDelegate(this, &CFirmwareTabController::OnDropFile));

 mp_view->mp_TablesPanel->setOnMapChanged(MakeDelegate(this, &CFirmwareTabController::OnMapChanged));
 mp_view->mp_TablesPanel->setOnFunSetSelectionChanged(MakeDelegate(this, &CFirmwareTabController::OnFunSetSelectionChanged));
 mp_view->mp_TablesPanel->setOnFunSetNamechanged(MakeDelegate(this, &CFirmwareTabController::OnFunSetNamechanged));
 mp_view->mp_TablesPanel->setOnCloseMapWnd(MakeDelegate(this, &MapWndScrPos::OnCloseMapWnd));
 mp_view->mp_TablesPanel->setOnOpenMapWnd(MakeDelegate(this, &MapWndScrPos::OnOpenMapWnd));
 mp_view->mp_TablesPanel->setIsAllowed(MakeDelegate(this, &CFirmwareTabController::IsFirmwareOpened));
 mp_view->mp_TablesPanel->setOnCTSXAxisEditChanged(MakeDelegate(this, &CFirmwareTabController::OnCTSXAxisEditChanged));
 mp_view->mp_TablesPanel->setOnATSXAxisEditChanged(MakeDelegate(this, &CFirmwareTabController::OnATSXAxisEditChanged));
 mp_view->mp_TablesPanel->setOnRPMGridEditButton(MakeDelegate(this, &CFirmwareTabController::OnEditRPMGrid));
 mp_view->mp_TablesPanel->setOnCESettingsButton(MakeDelegate(this, &CFirmwareTabController::OnCESettingsButton));
 mp_view->mp_TablesPanel->EnableAdvanceAngleIndication(false);
 mp_view->mp_TablesPanel->setOnChangeSettings(MakeDelegate(this, &CFirmwareTabController::OnChangeSettingsMapEd));
 mp_view->mp_TablesPanel->setOnFwConstsButton(MakeDelegate(this, &CFirmwareTabController::OnEditFwConsts));

 mp_view->mp_ParamDeskDlg->SetOnTabActivate(MakeDelegate(this, &CFirmwareTabController::OnParamDeskTabActivate));
 mp_view->mp_ParamDeskDlg->SetOnChangeInTab(MakeDelegate(this, &CFirmwareTabController::OnParamDeskChangeInTab));

 //register controller!
 TabControllersCommunicator::GetInstance()->AddReference(this, TCC_FIRMWARE_TAB_CONTROLLER);
}

CFirmwareTabController::~CFirmwareTabController()
{
 delete m_fwdm;
 delete m_edm;
 delete[] m_bl_data;
 delete m_code_for_merge_with_overhead;
}

//изменились настройки
void CFirmwareTabController::OnSettingsChanged(int action)
{
 if (action == 2)
 {
  mp_view->EnableMakingChartsChildren(mp_settings->GetChildCharts());
  return;
 }

 //включаем необходимый для данного контекста коммуникационный контроллер
 m_comm->SwitchOn(CCommunicationManager::OP_ACTIVATE_APPLICATION, true);

 //Set parameters for speed sensor calculations
 m_fwdm->SetNumPulsesPer1Km(mp_settings->GetNumPulsesPer1Km());
 //Set clock frequency (16 or 20 mHz)
 m_fwdm->SetQuartzFrq(PlatformParamHolder::GetQuartzFreq(mp_settings->GetECUPlatformType()));

 m_edm->SetNumPulsesPer1Km(mp_settings->GetNumPulsesPer1Km());
 m_edm->SetQuartzFrq(PlatformParamHolder::GetQuartzFreq(mp_settings->GetECUPlatformType()));

 mp_view->mp_TablesPanel->SetITEdMode(mp_settings->GetITEdMode());
}

void CFirmwareTabController::OnActivate(void)
{
 m_active = true;
 mp_view->mp_TablesPanel->ShowOpenedCharts(true);
 //выбираем ранее выбранную вкладку на панели параметров
 bool result = mp_view->mp_ParamDeskDlg->SetCurSel(m_lastSel);

 //////////////////////////////////////////////////////////////////
 //подключаем контроллер к потоку данных идущих от SECU-3
 m_comm->m_pAppAdapter->AddEventHandler(this,EHKEY);
 m_comm->m_pBldAdapter->SetEventHandler(this);
 m_comm->setOnSettingsChanged(MakeDelegate(this,&CFirmwareTabController::OnSettingsChanged));
 //////////////////////////////////////////////////////////////////

 //включаем необходимый для данного контекста коммуникационный контроллер
 m_comm->SwitchOn(CCommunicationManager::OP_ACTIVATE_APPLICATION);

 m_modification_check_timer.SetTimer(this,&CFirmwareTabController::OnModificationCheckTimer,250);

 //Activate children controllers
 mp_iorCntr->OnActivate();

 SetViewFirmwareValues();

 mp_view->EnableMakingChartsChildren(mp_settings->GetChildCharts());
 mp_view->EnableToggleMapWnd(mp_settings->GetToggleMapWnd());

 mp_view->mp_TablesPanel->SetITEdMode(mp_settings->GetITEdMode());

 MapPtMovStep mptms;
 mp_settings->GetMapPtMovStep(mptms);
 mp_view->mp_TablesPanel->SetPtMovStep(TYPE_MAP_DA_START, mptms.m_start_map);
 mp_view->mp_TablesPanel->SetPtMovStep(TYPE_MAP_DA_IDLE, mptms.m_idle_map);
 mp_view->mp_TablesPanel->SetPtMovStep(TYPE_MAP_DA_WORK, mptms.m_work_map);
 mp_view->mp_TablesPanel->SetPtMovStep(TYPE_MAP_DA_TEMP_CORR, mptms.m_temp_map);
 mp_view->mp_TablesPanel->SetPtMovStep(TYPE_MAP_INJ_VE, mptms.m_ve_map);
 mp_view->mp_TablesPanel->SetPtMovStep(TYPE_MAP_INJ_AFR, mptms.m_afr_map);
 mp_view->mp_TablesPanel->SetPtMovStep(TYPE_MAP_INJ_CRNK, mptms.m_crnk_map);
 mp_view->mp_TablesPanel->SetPtMovStep(TYPE_MAP_INJ_WRMP, mptms.m_wrmp_map);
 mp_view->mp_TablesPanel->SetPtMovStep(TYPE_MAP_INJ_DEAD, mptms.m_dead_map);
 mp_view->mp_TablesPanel->SetPtMovStep(TYPE_MAP_INJ_IDLR, mptms.m_idlr_map);
 mp_view->mp_TablesPanel->SetPtMovStep(TYPE_MAP_INJ_IDLC, mptms.m_idlc_map);
 mp_view->mp_TablesPanel->SetPtMovStep(TYPE_MAP_INJ_AETPS, mptms.m_aetps_map);
 mp_view->mp_TablesPanel->SetPtMovStep(TYPE_MAP_INJ_AERPM, mptms.m_aerpm_map);
 mp_view->mp_TablesPanel->SetPtMovStep(TYPE_MAP_INJ_AFTSTR, mptms.m_aftstr_map);
 mp_view->mp_TablesPanel->SetPtMovStep(TYPE_MAP_INJ_IT, mptms.m_it_map);
 mp_view->mp_TablesPanel->SetPtMovStep(TYPE_MAP_INJ_ITRPM, mptms.m_itrpm_map);
 mp_view->mp_TablesPanel->SetPtMovStep(TYPE_MAP_INJ_RIGID, mptms.m_rigid_map);
 mp_view->mp_TablesPanel->SetPtMovStep(TYPE_MAP_INJ_EGOCRV, mptms.m_egocrv_map);
 mp_view->mp_TablesPanel->SetPtMovStep(TYPE_MAP_INJ_IACC, mptms.m_iacc_map);
 mp_view->mp_TablesPanel->SetPtMovStep(TYPE_MAP_INJ_IACCW, mptms.m_iaccw_map);
 mp_view->mp_TablesPanel->SetPtMovStep(TYPE_MAP_INJ_IATCLT, mptms.m_iatclt_map);
 mp_view->mp_TablesPanel->SetPtMovStep(TYPE_MAP_INJ_TPSSWT, mptms.m_tpsswt_map);
 mp_view->mp_TablesPanel->SetPtMovStep(TYPE_MAP_INJ_GTSC, mptms.m_gtsc_map);
 mp_view->mp_TablesPanel->SetPtMovStep(TYPE_MAP_INJ_GPSC, mptms.m_gpsc_map);
 mp_view->mp_TablesPanel->SetPtMovStep(TYPE_MAP_INJ_ATSC, mptms.m_atsc_map);
 //separate
 mp_view->mp_TablesPanel->SetPtMovStep(TYPE_MAP_ATTENUATOR, mptms.m_attenuator_map);
 mp_view->mp_TablesPanel->SetPtMovStep(TYPE_MAP_DWELLCNTRL, mptms.m_dwellcntrl_map);
 mp_view->mp_TablesPanel->SetPtMovStep(TYPE_MAP_CTS_CURVE, mptms.m_cts_curve_map);
 mp_view->mp_TablesPanel->SetPtMovStep(TYPE_MAP_ATS_CURVE, mptms.m_ats_curve_map);
 mp_view->mp_TablesPanel->SetPtMovStep(TYPE_MAP_ATS_CORR, mptms.m_ats_aac_map);
 mp_view->mp_TablesPanel->SetPtMovStep(TYPE_MAP_GASDOSE, mptms.m_gasdose_map);
 mp_view->mp_TablesPanel->SetPtMovStep(TYPE_MAP_BAROCORR, mptms.m_barocorr_map);
 mp_view->mp_TablesPanel->SetPtMovStep(TYPE_MAP_MANIGNTIM, mptms.m_manigntim_map);
 mp_view->mp_TablesPanel->SetPtMovStep(TYPE_MAP_TMP2_CURVE, mptms.m_tmp2_curve_map);
 mp_view->mp_TablesPanel->SetPtMovStep(TYPE_MAP_CRKCLT_CORR, mptms.m_crktemp_map);
 mp_view->mp_TablesPanel->SetPtMovStep(TYPE_MAP_EH_PAUSE, mptms.m_eh_pause_map);

 //симулируем изменение состояния для обновления контроллов, так как OnConnection вызывается только если
 //сбрывается или разрывается принудительно (путем деактивации коммуникационного контроллера)
 bool online_status = m_comm->m_pControlApp->GetOnlineStatus();
 OnConnection(online_status); 
}

void CFirmwareTabController::OnDeactivate(void)
{
 m_active = false;
 mp_view->mp_TablesPanel->ShowOpenedCharts(false);
 //отключаемся от потока данных
 m_comm->m_pAppAdapter->RemoveEventHandler(EHKEY);
 m_sbar->SetInformationText(_T(""));
 m_sbar->ShowProgressBar(false);
 m_modification_check_timer.KillTimer();
 m_waiting_bl_timer.KillTimer();
 //запоминаем номер последней выбранной вкладки на панели параметров
 m_lastSel = mp_view->mp_ParamDeskDlg->GetCurSel();

 //Deactivate children controllers
 mp_iorCntr->OnDeactivate();
}

/////////////////////////////////////////////////////////////////////////////////
void CFirmwareTabController::OnPacketReceived(const BYTE i_descriptor, SECU3IO::SECU3Packet* ip_packet)
{
 if (i_descriptor == FWINFO_DAT)
 { //приняли пакет с сигнатурной информацией о прошивке
  if (m_read_fw_sig_info_flag)
  {
   SECU3IO::FWInfoDat* p_packet = (SECU3IO::FWInfoDat*)ip_packet;
   TCHAR string[256];
   OemToChar(p_packet->info, string);
   m_sbar->SetInformationText(string);

   //display firmware options if present
   _ShowFWOptions(string, p_packet->options, p_packet->fw_version);
   m_read_fw_sig_info_flag = false;
  }
 }
 else if (i_descriptor == OP_COMP_NC)
 {
  const SECU3IO::OPCompNc* p_ndata = (SECU3IO::OPCompNc*)ip_packet;
  switch(p_ndata->opcode)
  {
   case SECU3IO::OPCODE_EEPROM_PARAM_SAVE: //параметры были сохранены
    m_sbar->SetInformationText(MLL::LoadString(IDS_PM_PARAMS_HAS_BEEN_SAVED));
    return;
   case SECU3IO::OPCODE_SAVE_TABLSET:     //таблицы были сохранены
    m_sbar->SetInformationText(MLL::LoadString(IDS_PM_TABLSET_HAS_BEEN_SAVED));    
    return;
   case SECU3IO::OPCODE_RESET_EEPROM:     //начался процесс сброса EEPROM
    if (p_ndata->opdata == 0x55)
    {
     m_sbar->SetInformationText(MLL::LoadString(IDS_FW_RESET_EEPROM_STARTED));
     m_clear_sbar_txt_on_conn = true;
    }
    return;
   case SECU3IO::OPCODE_BL_CONFIRM: //confirmation that firmware is going to start boot loader
    if (p_ndata->opdata == 0xBC)
    {
     if (m_blFinishOpCB)
     {
      m_waiting_bl_timer.KillTimer();
      Sleep(DELAY_AFTER_BL_START);
      (this->*(m_blFinishOpCB))();
      m_blFinishOpCB = NULL;
     }
    }
    return;
  } 
 }
 else if (i_descriptor == INJDRV_PAR)
  m_comm->m_pControlApp->ChangeContext(SENSOR_DAT);
}

void CFirmwareTabController::OnConnection(const bool i_online)
{
 int state;
 ASSERT(m_sbar);

 if (i_online) //перешли в онлайн
 {
  state = CStatusBarManager::STATE_ONLINE;
  mp_view->EnableBLStartedEmergency(false);

  //в онлайне элементы меню связанные с бутлоадером всегда разрешены
  mp_view->EnableBLItems(true);

  //если мы перешли в онлайн, то убираем чекбокс - пользователь нас обманывает:
  //он указал что бутлоадер запущен аварийно и в тоже время запущен Application
  mp_view->SetBLStartedEmergency(false);

  mp_view->EnableAppItems(true);
  if (m_clear_sbar_txt_on_conn)
  {
   m_sbar->SetInformationText(_T(""));
   m_clear_sbar_txt_on_conn = false;
  }
 }
 else
 { //перешли в оффлайн
  if (!m_comm->m_pBootLoader->GetWorkState()) //разрешаем чекбокс только если мы в оффлайне сразу после онлайна
   mp_view->EnableBLStartedEmergency(true);

  //в оффлайне состояние элементов меню связанных с бутлоадером зависит от состояния чекбокса
  bool enable = mp_view->IsBLStartedEmergency();
  mp_view->EnableBLItems(enable);

  if (enable)
   state = CStatusBarManager::STATE_BOOTLOADER; //чтобы иконка бутлоадера не пропадала после завершения операции
  else
   state = CStatusBarManager::STATE_OFFLINE;

  mp_view->EnableAppItems(false);
 }

 //если бутлоадер активен (выполняется выбранная из меню операция), то будем отображать именно
 //иконку бутлоадера
 if (m_comm->m_pBootLoader->GetWorkState())
  state = CStatusBarManager::STATE_BOOTLOADER;

 m_sbar->SetConnectionState(state);
}

/////////////////////////////////////////////////////////////////////////////////
void CFirmwareTabController::OnUpdateUI(IBLDEventHandler::poolUpdateUI* ip_data)
{
 ASSERT(ip_data);
 IBLDEventHandler::poolUpdateUI data;

 /////////////////////////////////////////////////////////////
 //эксклюзивный доступ
 m_comm->m_pBootLoader->EnterCriticalSection();
 data = *ip_data;
 m_comm->m_pBootLoader->LeaveCriticalSection();
 /////////////////////////////////////////////////////////////

 if (data.opcode!=CBootLoader::BL_OP_EXIT) //для операции выхода из бутлоадера не показываем никакого прогресс бара
 {
  m_sbar->SetProgressRange(0, data.total);
  m_sbar->SetProgressPos(data.current);
 }
}

void CFirmwareTabController::OnBegin(const int opcode,const int status)
{
 if (opcode == CBootLoader::BL_OP_READ_SIGNATURE)
  m_sbar->SetInformationText(MLL::LoadString(IDS_FW_READING_SIGNATURE));
 if (opcode == CBootLoader::BL_OP_READ_EEPROM)
  m_sbar->SetInformationText(MLL::LoadString(IDS_FW_READING_EEPROM));
 if (opcode == CBootLoader::BL_OP_WRITE_EEPROM)
  m_sbar->SetInformationText(MLL::LoadString(IDS_FW_WRITING_EEPROM));
 if (opcode == CBootLoader::BL_OP_READ_FLASH)
  m_sbar->SetInformationText(MLL::LoadString(IDS_FW_READING_FW));
 if (opcode == CBootLoader::BL_OP_WRITE_FLASH)
  m_sbar->SetInformationText(MLL::LoadString(IDS_FW_WRITING_FW));
 if (opcode == CBootLoader::BL_OP_EXIT)
 {
  //Exiting from boot loader...
 }

 mp_view->EnableBLItems(false);
 mp_view->EnableBLStartedEmergency(false);
}

void CFirmwareTabController::OnEnd(const int opcode,const int status)
{
 switch(opcode)
 {
  //////////////////////////////////////////////////////////////////////
  case CBootLoader::BL_OP_EXIT: //не используется когда бутлоадер запущен аварийно
  {
   //вновь активируем коммуникационный контроллер приложения
   m_comm->SwitchOn(CCommunicationManager::OP_ACTIVATE_APPLICATION);
   break;
  }

  //////////////////////////////////////////////////////////////////////
  case CBootLoader::BL_OP_READ_SIGNATURE:
  {
   if (status==1)
   {
    m_bl_data[CBootLoader::BL_SIGNATURE_STR_LEN] = 0;
    m_sbar->SetInformationText(m_bl_data);
   }
   else
   {
    m_sbar->SetInformationText(ErrorMsg::GenerateErrorStr(m_comm));
   }

   //ждем пока не выполнится предыдущая операция
   while(!m_comm->m_pBootLoader->IsIdle());

   //Achtung! Почти рекурсия
   ExitBootLoader();

   Sleep(250);
   m_sbar->ShowProgressBar(false);
   break;
  }

  //////////////////////////////////////////////////////////////////////
  case CBootLoader::BL_OP_READ_EEPROM:
  {
   if (status==1)
   { //OK
    m_sbar->SetInformationText(MLL::LoadString(IDS_FW_EEPROM_READ_SUCCESSFULLY));
    FirmwareFileUtils::SaveEEPROMToFile(m_bl_data, m_epp.m_size);
   }
   else
   {
    m_sbar->SetInformationText(ErrorMsg::GenerateErrorStr(m_comm));
   }

   //ждем пока не выполнится предыдущая операция
   while(!m_comm->m_pBootLoader->IsIdle());

   //Achtung! Почти рекурсия
   ExitBootLoader();

   Sleep(250);
   m_sbar->ShowProgressBar(false);
   break;
  }

  //////////////////////////////////////////////////////////////////////
  case CBootLoader::BL_OP_WRITE_EEPROM:
  {
   if (status==1)
    m_sbar->SetInformationText(MLL::LoadString(IDS_FW_EEPROM_WRITTEN_SUCCESSFULLY));
   else
   {
    m_sbar->SetInformationText(ErrorMsg::GenerateErrorStr(m_comm));
   }

   //ждем пока не выполнится предыдущая операция
   while(!m_comm->m_pBootLoader->IsIdle());

   //Achtung! Почти рекурсия
   ExitBootLoader();

   Sleep(250);
   m_sbar->ShowProgressBar(false);
   break;
  }

  //////////////////////////////////////////////////////////////////////
  case CBootLoader::BL_OP_READ_FLASH:
  {
   if (status==1)
   {
    m_sbar->SetInformationText(MLL::LoadString(IDS_FW_FW_READ_SUCCESSFULLY));
    if (m_bl_read_flash_mode == MODE_RD_FLASH_TO_FILE)
    {
     FirmwareFileUtils::SaveFLASHToFile(m_bl_data, m_fpp.m_total_size, m_fwdm);
    }
    else if (m_bl_read_flash_mode == MODE_RD_FLASH_TO_BUFF_FOR_LOAD)
    {
     if (_CheckFirmwareCompatibilityAndAskUser(m_bl_data) && _CheckQuartzCompatibilityAndAskUser(m_bl_data))
     {
      if (FirmwareFileUtils::CheckFirmwareIntegrity(m_bl_data, m_fpp.m_total_size))
       PrepareOnLoadFLASH(m_bl_data, _T(""));
     }
    }
    else if (m_bl_read_flash_mode == MODE_RD_FLASH_FOR_IMPORT_DATA)
    {
     if (_CheckFirmwareCompatibilityAndAskUser(m_bl_data) && _CheckQuartzCompatibilityAndAskUser(m_bl_data))
     {
      m_fwdm->LoadDataBytesFromAnotherFirmware(m_bl_data);      
      PrepareOnLoadFLASH(NULL, m_fwdm->GetFWFileName());
     }
    }
    else if (m_bl_read_flash_mode == MODE_RD_FLASH_TO_BUFF_MERGE_DATA)
    {
     //ждем пока не выполнится предыдущая операция
     while(!m_comm->m_pBootLoader->IsIdle());

     //закончилось чтение данных. Теперь необходимо объединить прочитанные данные с данными для записи,
     //обновить контрольную сумму и запустить процесс программирования FLASH.
     size_t dataSize = m_fpp.m_app_section_size - m_code_for_merge_size;
     BYTE* dataPtr = m_code_for_merge_with_overhead + m_code_for_merge_size;
     //Так как мы программируем только код, а он содержит некоторые данные, то мы должны
     //"подтянуть" эти данные из фрагмента старого кода. m_code_for_merge_with_overhead хранит еще данные пришедшие
     //с новым кодом.
     size_t srcSize = (m_fpp.m_app_section_size - m_code_for_merge_size) + m_moreSize;
     m_fwdm->LoadCodeData(m_bl_data, srcSize, m_code_for_merge_with_overhead);
     //переносим старые данные
     memset(dataPtr, 0, dataSize);
     memcpy(dataPtr, m_bl_data + m_moreSize, dataSize);
     //Высчитываем и записываем контрольную сумму всей прошивки
     m_fwdm->CalculateAndPlaceFirmwareCRC(m_code_for_merge_with_overhead);

     Sleep(250);
     m_sbar->SetProgressPos(0);
     m_comm->m_pBootLoader->StartOperation(CBootLoader::BL_OP_WRITE_FLASH, m_code_for_merge_with_overhead, m_fpp.m_app_section_size);

     //НЕ ВЫХОДИМ ИЗ БУТЛОАДЕРА И НЕ ДЕАКТИВИРУЕМ КОММУНИКАЦИОННЫЙ КОНТРОЛЛЕР, так как должна
     //выполниться запущенная операция.
     return;
    }
    else
    {
     ASSERT(0); //what is it?
    }
   }
   else
   {
    m_sbar->SetInformationText(ErrorMsg::GenerateErrorStr(m_comm));
   }

   //ждем пока не выполнится предыдущая операция
   while(!m_comm->m_pBootLoader->IsIdle());

   //Achtung! Почти рекурсия
   ExitBootLoader();

   Sleep(250);
   m_sbar->ShowProgressBar(false);
   break;
  }

  //////////////////////////////////////////////////////////////////////
  case CBootLoader::BL_OP_WRITE_FLASH:
  {
   if (status==1)
    m_sbar->SetInformationText(MLL::LoadString(IDS_FW_FW_WRITTEN_SUCCESSFULLY));
   else
   {
    m_sbar->SetInformationText(ErrorMsg::GenerateErrorStr(m_comm));
   }

   //ждем пока не выполнится предыдущая операция
   while(!m_comm->m_pBootLoader->IsIdle());

   //Achtung! Почти рекурсия
   ExitBootLoader();

   Sleep(250);
   m_sbar->ShowProgressBar(false);
   break;
  }
  //////////////////////////////////////////////////////////////////////
 }//switch

 //если бутлоадер был запущен аварийно, то активируем коммуникационный
 //контроллер приложения.  А зачем???  TODO.
 if (m_bl_started_emergency)
 {
  mp_view->EnableBLItems(true);
  mp_view->EnableBLStartedEmergency(true);

  m_comm->SwitchOn(CCommunicationManager::OP_ACTIVATE_APPLICATION);
 }
}

/////////////////////////////////////////////////////////////////////////////////
//получение информации о бутлоадере
void CFirmwareTabController::OnBootLoaderInfo(void)
{
 if (!m_comm->m_pBootLoader->IsIdle())
  return;

 //запускаем бутлоадер (если нужно)
 StartBootLoader();

 if (!m_bl_started_emergency)
 {
  m_sbar->SetInformationText(MLL::LoadString(IDS_FW_WAITING_BOOTLOADER)); 
  m_comm->m_pControlApp->SetPacketsTimer(WAITING_BL_START_TIMEOUT);
  m_waiting_bl_timer.SetTimer(WAITING_BL_START_TIMEOUT, true); //one shot
  m_blFinishOpCB = &CFirmwareTabController::finishOnBootLoaderInfo;
 }
 else
  finishOnBootLoaderInfo();
}

void CFirmwareTabController::OnReadEepromToFile(void)
{
 if (!m_comm->m_pBootLoader->IsIdle())
  return;

 //запускаем бутлоадер по команде из приложения
 StartBootLoader();

 if (!m_bl_started_emergency)
 {
  m_sbar->SetInformationText(MLL::LoadString(IDS_FW_WAITING_BOOTLOADER)); 
  m_comm->m_pControlApp->SetPacketsTimer(WAITING_BL_START_TIMEOUT);
  m_waiting_bl_timer.SetTimer(WAITING_BL_START_TIMEOUT, true); //one shot
  m_blFinishOpCB = &CFirmwareTabController::finishOnReadEepromToFile;
 }
 else
  finishOnReadEepromToFile();
}

void CFirmwareTabController::OnWriteEepromFromFile(void)
{
 std::vector<int> sizes;
 sizes.push_back(m_epp.m_size);
 bool result = FirmwareFileUtils::LoadEEPROMFromFile(m_bl_data, sizes);

 if (!result)
  return; //cancel

 ASSERT(m_comm);

 //запускаем бутлоадер по команде из приложения
 StartBootLoader();

 if (!m_bl_started_emergency)
 {  
  m_sbar->SetInformationText(MLL::LoadString(IDS_FW_WAITING_BOOTLOADER)); 
  m_comm->m_pControlApp->SetPacketsTimer(WAITING_BL_START_TIMEOUT);
  m_waiting_bl_timer.SetTimer(WAITING_BL_START_TIMEOUT, true); //one shot
  m_blFinishOpCB = &CFirmwareTabController::finishOnWriteEepromFromFile;
 }
 else
  finishOnWriteEepromFromFile();
}

void CFirmwareTabController::OnResetEeprom(void)
{
 if (IDYES==AfxMessageBox(MLL::GetString(IDS_RESET_EEPROM_COMFIRMATION).c_str(), MB_YESNO|MB_DEFBUTTON2|MB_ICONEXCLAMATION))
 {
  SECU3IO::OPCompNc packet_data;
  packet_data.opcode = SECU3IO::OPCODE_RESET_EEPROM;
  packet_data.opdata = 0xAA;
  m_comm->m_pControlApp->SendPacket(OP_COMP_NC, &packet_data);
 }
}

void CFirmwareTabController::OnReadFlashToFile(void)
{
 m_bl_read_flash_mode = MODE_RD_FLASH_TO_FILE;
 _OnReadFlashToFile();
}

void CFirmwareTabController::_OnReadFlashToFile(void)
{
 //запускаем бутлоадер по команде из приложения
 StartBootLoader();

 if (!m_bl_started_emergency)
 {
  m_sbar->SetInformationText(MLL::LoadString(IDS_FW_WAITING_BOOTLOADER)); 
  m_comm->m_pControlApp->SetPacketsTimer(WAITING_BL_START_TIMEOUT);
  m_waiting_bl_timer.SetTimer(WAITING_BL_START_TIMEOUT, true); //one shot
  m_blFinishOpCB = &CFirmwareTabController::finish_OnReadFlashToFile;
 }
 else
  finish_OnReadFlashToFile();
}

bool CFirmwareTabController::_CheckFirmwareCompatibilityAndAskUser(BYTE* i_buff, const PlatformParamHolder* p_pph /*=NULL*/)
{
 if (!i_buff)
  return false;

 if (!m_fwdm->CheckCompatibility(i_buff, p_pph ? &p_pph->GetFlashParameters() : NULL))
 {
  if (IDNO==AfxMessageBox(MLL::LoadString(IDS_INCOMPATIBLE_FIRMWARE), MB_YESNO | MB_ICONEXCLAMATION))
   return false; //aborted by user
 }

 return true; //compatible or/and user argee
}

bool CFirmwareTabController::_CheckQuartzCompatibilityAndAskUser(BYTE* ip_buff, size_t fwSize /*=0*/)
{
 if (0==fwSize)
  fwSize = m_fpp.m_total_size;

 EECUPlatform platform_id;
 if (PlatformParamHolder::GetPlatformIdByFirmwareMagic(ip_buff, fwSize, platform_id))
 {
  PlatformParamHolder pph(platform_id);
  if (m_fpp.m_fcpu_hz != pph.GetFlashParameters().m_fcpu_hz)
  {
   if (IDNO==AfxMessageBox(MLL::LoadString(IDS_INCOMPATIBLE_QUARTZ), MB_YESNO | MB_ICONEXCLAMATION))
    return false; //aborted by user
  }
 }

 return true; //compatible or compatibility can't be checked
}

void CFirmwareTabController::OnWriteFlashFromFile(void)
{
 std::vector<int> sizes;
 sizes.push_back(m_fpp.m_total_size);
 bool result = FirmwareFileUtils::LoadFLASHFromFile(m_bl_data, sizes);

 if (!result || !_CheckQuartzCompatibilityAndAskUser(m_bl_data))
  return; //cancel

 StartWritingOfFLASHFromBuff();
}

void CFirmwareTabController::StartWritingOfFLASHFromBuff(void)
{
 //вычисляем контрольную сумму и сохраняем ее в массив с прошивкой. Это необходимо например когда
 //мы записываем свеже скомпилированную прошивку, которая может не содержать контрольной суммы
 m_fwdm->CalculateAndPlaceFirmwareCRC(m_bl_data);

 ASSERT(m_comm);

 //запускаем бутлоадер по команде из приложения (если нет флага что он запущен вручную)
 StartBootLoader();

 if (!m_bl_started_emergency)
 {
  m_sbar->SetInformationText(MLL::LoadString(IDS_FW_WAITING_BOOTLOADER)); 
  m_comm->m_pControlApp->SetPacketsTimer(WAITING_BL_START_TIMEOUT);
  m_waiting_bl_timer.SetTimer(WAITING_BL_START_TIMEOUT, true); //one shot
  m_blFinishOpCB = &CFirmwareTabController::finishStartWritingOfFLASHFromBuff;
 }
 else
  finishStartWritingOfFLASHFromBuff();
}

//от чекбокса...
void CFirmwareTabController::OnBLStartedEmergency(void)
{
 if (m_comm->m_pControlApp->GetOnlineStatus()==true)
  return;

 //если оффлайн, то состояние элементов меню зависит от состояния чекбокса
 bool emergency = mp_view->IsBLStartedEmergency();
 mp_view->EnableBLItems(emergency);

 //если он уже запущен, то при установленом чекбоксе в статусе будет всегда иконка бутлоадера
 if (emergency)
  m_sbar->SetConnectionState(CStatusBarManager::STATE_BOOTLOADER);
 else
  m_sbar->SetConnectionState(CStatusBarManager::STATE_OFFLINE);
}

bool CFirmwareTabController::IsBLStartedEmergency(void)
{
 //если чекбокс запрещен то всегда возвращаем отрицательный результат, а запрещен чекбокс
 //если мы в онлайне или запущен бутлоадер
 return mp_view->IsBLStartedEmergency() && mp_view->IsBLStartedEmergencyEnabled();
}

bool CFirmwareTabController::StartBootLoader(void)
{
 ASSERT(m_comm);

 //до завершения операции надо пользоваться только этой переменной
 m_bl_started_emergency = IsBLStartedEmergency();

 //запускаем бутлоадер по команде из приложения (если нет флага что он запущен вручную)
 if (!m_bl_started_emergency)
 {  
  bool result = m_comm->m_pControlApp->StartBootLoader();
  Sleep(DELAY_AFTER_BL_START); //обязательно нужно подождать не менее 50 мс, иначе будут вылазить посторонние символы при приеме данных от загрузчика
  return result;
 }

 return true; //бутлоадер запускать не надо было - он запущен аварийно
}

bool CFirmwareTabController::ExitBootLoader(void)
{
 ASSERT(m_comm);

 if (!m_bl_started_emergency)
 {
  bool result = m_comm->m_pBootLoader->StartOperation(CBootLoader::BL_OP_EXIT,NULL,0);
  return result;
 }

 //Выходить из бутлоадера не надо было - он запущен аварийно.
 //На будущее: Если бутлоадер запущен аварийно и закончилась операция записи прошивки (работающей прошивки :-)),
 //то из него можно выйти... Но это по желанию пользователя. А вообще удобства в этом - практичеки нет.
 return true;
}

bool CFirmwareTabController::CheckChangesAskAndSaveFirmware(void)
{
 bool modified = m_fwdm->IsModified();
 if (modified && mp_settings->GetSaveWarning())
 {
  int result = AfxMessageBox(MLL::LoadString(IDS_FW_MODIFICATION_WARNING), MB_YESNOCANCEL);
  if (result==IDCANCEL)
  {
   return false; //пользователь отменил действие
  }
  if (result==IDNO)
  {
   return true; //пользователь решил продолжить без сохраненя
  }
  if (result==IDYES)
  { //необходимо сохранить данные! (и продолжить)
   OnSaveFlashToFile();
   return true;
  }
 }
 //данные не были изменены - продолжение без вопросов
 return true;
}

//Эта функция вызывается при выходе из приложения. Эта функция может запретить выход, если вернет false
bool CFirmwareTabController::OnClose(void)
{
 //сохраняем позиции открытых окон!
 //ignition
 OnCloseMapWnd(mp_view->mp_TablesPanel->GetMapWindow(TYPE_MAP_DA_START), TYPE_MAP_DA_START);
 OnCloseMapWnd(mp_view->mp_TablesPanel->GetMapWindow(TYPE_MAP_DA_IDLE),  TYPE_MAP_DA_IDLE);
 OnCloseMapWnd(mp_view->mp_TablesPanel->GetMapWindow(TYPE_MAP_DA_WORK),  TYPE_MAP_DA_WORK);
 OnCloseMapWnd(mp_view->mp_TablesPanel->GetMapWindow(TYPE_MAP_DA_TEMP_CORR), TYPE_MAP_DA_TEMP_CORR);
 //fuel injection
 OnCloseMapWnd(mp_view->mp_TablesPanel->GetMapWindow(TYPE_MAP_INJ_VE), TYPE_MAP_INJ_VE);
 OnCloseMapWnd(mp_view->mp_TablesPanel->GetMapWindow(TYPE_MAP_INJ_AFR), TYPE_MAP_INJ_AFR);
 OnCloseMapWnd(mp_view->mp_TablesPanel->GetMapWindow(TYPE_MAP_INJ_CRNK), TYPE_MAP_INJ_CRNK);
 OnCloseMapWnd(mp_view->mp_TablesPanel->GetMapWindow(TYPE_MAP_INJ_WRMP), TYPE_MAP_INJ_WRMP);
 OnCloseMapWnd(mp_view->mp_TablesPanel->GetMapWindow(TYPE_MAP_INJ_DEAD), TYPE_MAP_INJ_DEAD);
 OnCloseMapWnd(mp_view->mp_TablesPanel->GetMapWindow(TYPE_MAP_INJ_IDLR), TYPE_MAP_INJ_IDLR);
 OnCloseMapWnd(mp_view->mp_TablesPanel->GetMapWindow(TYPE_MAP_INJ_IDLC), TYPE_MAP_INJ_IDLC);
 OnCloseMapWnd(mp_view->mp_TablesPanel->GetMapWindow(TYPE_MAP_INJ_AETPS), TYPE_MAP_INJ_AETPS);
 OnCloseMapWnd(mp_view->mp_TablesPanel->GetMapWindow(TYPE_MAP_INJ_AERPM), TYPE_MAP_INJ_AERPM);
 OnCloseMapWnd(mp_view->mp_TablesPanel->GetMapWindow(TYPE_MAP_INJ_AFTSTR), TYPE_MAP_INJ_AFTSTR);
 OnCloseMapWnd(mp_view->mp_TablesPanel->GetMapWindow(TYPE_MAP_INJ_IT), TYPE_MAP_INJ_IT);
 OnCloseMapWnd(mp_view->mp_TablesPanel->GetMapWindow(TYPE_MAP_INJ_ITRPM), TYPE_MAP_INJ_ITRPM);
 OnCloseMapWnd(mp_view->mp_TablesPanel->GetMapWindow(TYPE_MAP_INJ_RIGID), TYPE_MAP_INJ_RIGID);
 OnCloseMapWnd(mp_view->mp_TablesPanel->GetMapWindow(TYPE_MAP_INJ_EGOCRV), TYPE_MAP_INJ_EGOCRV);
 OnCloseMapWnd(mp_view->mp_TablesPanel->GetMapWindow(TYPE_MAP_INJ_IACC), TYPE_MAP_INJ_IACC);
 OnCloseMapWnd(mp_view->mp_TablesPanel->GetMapWindow(TYPE_MAP_INJ_IACCW), TYPE_MAP_INJ_IACCW);
 OnCloseMapWnd(mp_view->mp_TablesPanel->GetMapWindow(TYPE_MAP_INJ_IATCLT), TYPE_MAP_INJ_IATCLT);
 OnCloseMapWnd(mp_view->mp_TablesPanel->GetMapWindow(TYPE_MAP_INJ_TPSSWT), TYPE_MAP_INJ_TPSSWT);
 OnCloseMapWnd(mp_view->mp_TablesPanel->GetMapWindow(TYPE_MAP_INJ_GTSC), TYPE_MAP_INJ_GTSC);
 OnCloseMapWnd(mp_view->mp_TablesPanel->GetMapWindow(TYPE_MAP_INJ_GPSC), TYPE_MAP_INJ_GPSC);
 OnCloseMapWnd(mp_view->mp_TablesPanel->GetMapWindow(TYPE_MAP_INJ_ATSC), TYPE_MAP_INJ_ATSC);

 //separate
 OnCloseMapWnd(mp_view->mp_TablesPanel->GetMapWindow(TYPE_MAP_ATTENUATOR), TYPE_MAP_ATTENUATOR);
 OnCloseMapWnd(mp_view->mp_TablesPanel->GetMapWindow(TYPE_MAP_DWELLCNTRL), TYPE_MAP_DWELLCNTRL);
 OnCloseMapWnd(mp_view->mp_TablesPanel->GetMapWindow(TYPE_MAP_CTS_CURVE), TYPE_MAP_CTS_CURVE);
 OnCloseMapWnd(mp_view->mp_TablesPanel->GetMapWindow(TYPE_MAP_ATS_CURVE), TYPE_MAP_ATS_CURVE);
 OnCloseMapWnd(mp_view->mp_TablesPanel->GetMapWindow(TYPE_MAP_ATS_CORR), TYPE_MAP_ATS_CORR);
 OnCloseMapWnd(mp_view->mp_TablesPanel->GetMapWindow(TYPE_MAP_GASDOSE), TYPE_MAP_GASDOSE);
 OnCloseMapWnd(mp_view->mp_TablesPanel->GetMapWindow(TYPE_MAP_BAROCORR), TYPE_MAP_BAROCORR);
 OnCloseMapWnd(mp_view->mp_TablesPanel->GetMapWindow(TYPE_MAP_MANIGNTIM), TYPE_MAP_MANIGNTIM);
 OnCloseMapWnd(mp_view->mp_TablesPanel->GetMapWindow(TYPE_MAP_TMP2_CURVE), TYPE_MAP_TMP2_CURVE);
 OnCloseMapWnd(mp_view->mp_TablesPanel->GetMapWindow(TYPE_MAP_CRKCLT_CORR), TYPE_MAP_CRKCLT_CORR);
 OnCloseMapWnd(mp_view->mp_TablesPanel->GetMapWindow(TYPE_MAP_EH_PAUSE), TYPE_MAP_EH_PAUSE);
 OnCloseMapWnd(mp_view->mp_TablesPanel->GetMapWindow(TYPE_MAP_GME_IGN_WND), TYPE_MAP_GME_IGN_WND);
 OnCloseMapWnd(mp_view->mp_TablesPanel->GetMapWindow(TYPE_MAP_GME_INJ_WND), TYPE_MAP_GME_INJ_WND);

 if (m_active && (!m_comm->m_pBootLoader->IsIdle() || m_waiting_bl_timer.isActive()))
  if (!ErrorMsg::AskUserAboutTabLeaving())
   return false;

 return CheckChangesAskAndSaveFirmware();
}

void CFirmwareTabController::OnCloseNotify(void)
{
  //empty
}

void CFirmwareTabController::OnFullScreen(bool i_what)
{
 //empty
}

bool CFirmwareTabController::OnAskChangeTab(void)
{
 if (m_comm->m_pBootLoader->IsIdle() && !m_waiting_bl_timer.isActive())
  return true; //allows
 return ErrorMsg::AskUserAboutTabLeaving();
}

void CFirmwareTabController::PrepareOnLoadFLASH(const BYTE* i_buff, const _TSTRING& i_file_name)
{
 if (i_buff) //Do we need to load?
  m_fwdm->LoadBytes(i_buff);
 if (i_file_name!=_T(""))
  m_fwdm->SetFWFileName(_TSTRING(i_file_name));
 else
 {
  CString string;
  SYSTEMTIME time;
  GetLocalTime(&time);
  string.Format(_T("FW%02u%02u%02u-%02u%02u%04u.bin"),
    time.wHour,time.wMinute,time.wSecond,time.wDay,time.wMonth,time.wYear);
  m_fwdm->SetFWFileName(_TSTRING(string));
 }

 DWORD opt = m_fwdm->GetFWOptions();
 Functionality fnc;
 mp_settings->GetFunctionality(fnc);

 //Разрешаем или запрещаем определенные функции в зависимости от опций прошивки
 mp_view->mp_TablesPanel->EnableDwellControl(CHECKBIT32(opt, SECU3IO::COPT_DWELL_CONTROL));
 mp_view->mp_TablesPanel->EnableCTSCurve(CHECKBIT32(opt, SECU3IO::COPT_THERMISTOR_CS));
 mp_view->mp_TablesPanel->EnableChokeOp(fnc.SM_CONTROL && CHECKBIT32(opt, SECU3IO::COPT_SM_CONTROL));
 mp_view->mp_TablesPanel->EnableGasdose(fnc.GD_CONTROL && CHECKBIT32(opt, SECU3IO::COPT_GD_CONTROL));
 mp_view->mp_TablesPanel->EnableCarbAfr(CHECKBIT32(opt, SECU3IO::COPT_CARB_AFR));
 mp_view->mp_TablesPanel->EnableFuelInjection(CHECKBIT32(opt, SECU3IO::COPT_FUEL_INJECT));
 mp_view->mp_TablesPanel->EnableTmp2Curve(!CHECKBIT32(opt, SECU3IO::COPT_SECU3T)); 
 bool en_for_gd = (CHECKBIT32(opt, SECU3IO::COPT_ATMEGA1284) || CHECKBIT32(opt, SECU3IO::COPT_FUEL_INJECT)); //TODO: remove this line after migration to M1284!
 mp_view->mp_TablesPanel->EnableGasCorr(!CHECKBIT32(opt, SECU3IO::COPT_SECU3T) && en_for_gd);
 mp_view->mp_ParamDeskDlg->EnableIgnitionCogs(!CHECKBIT32(opt, SECU3IO::COPT_DWELL_CONTROL) && !CHECKBIT32(opt, SECU3IO::COPT_CKPS_2CHIGN));
 mp_view->mp_ParamDeskDlg->EnableUseVentPwm(CHECKBIT32(opt, SECU3IO::COPT_COOLINGFAN_PWM));
 mp_view->mp_ParamDeskDlg->EnableUseCTSCurveMap(CHECKBIT32(opt, SECU3IO::COPT_THERMISTOR_CS));
 mp_view->mp_ParamDeskDlg->EnableHallOutputParams(CHECKBIT32(opt, SECU3IO::COPT_HALL_OUTPUT) && !CHECKBIT32(opt, SECU3IO::COPT_HALL_SYNC) && !CHECKBIT32(opt, SECU3IO::COPT_CKPS_NPLUS1));
 mp_view->mp_ParamDeskDlg->EnableSECU3TItems(CHECKBIT32(opt, SECU3IO::COPT_SECU3T));
 mp_view->mp_ParamDeskDlg->EnableExtraIO(!CHECKBIT32(opt, SECU3IO::COPT_SECU3T) && CHECKBIT32(opt, SECU3IO::COPT_TPIC8101));
 if (CHECKBIT32(opt, SECU3IO::COPT_SECU3T))
  mp_view->mp_ParamDeskDlg->SetMaxCylinders(CHECKBIT32(opt, SECU3IO::COPT_PHASED_IGNITION) ? 8 : 8);
 else //SECU-3i:
  mp_view->mp_ParamDeskDlg->SetMaxCylinders(CHECKBIT32(opt, SECU3IO::COPT_PHASED_IGNITION) ? 8 : 8);

 //in full-sequential ignition mode odd cylinder number engines are also supported,
 //also if hall sensor synchronization is used
 mp_view->mp_ParamDeskDlg->EnableOddCylinders(CHECKBIT32(opt, SECU3IO::COPT_PHASED_IGNITION) || CHECKBIT32(opt, SECU3IO::COPT_HALL_SYNC));

 this->mp_iorCntr->EnableSECU3TFeatures(CHECKBIT32(opt, SECU3IO::COPT_SECU3T));
 this->mp_iorCntr->EnableExtraIO(!CHECKBIT32(opt, SECU3IO::COPT_SECU3T) && CHECKBIT32(opt, SECU3IO::COPT_TPIC8101)); 
 this->mp_iorCntr->Enable(true);

 mp_view->mp_ParamDeskDlg->EnableCKPSItems(!CHECKBIT32(opt, SECU3IO::COPT_HALL_SYNC) && !CHECKBIT32(opt, SECU3IO::COPT_CKPS_NPLUS1));
 mp_view->mp_ParamDeskDlg->EnableHallWndWidth(CHECKBIT32(opt, SECU3IO::COPT_HALL_SYNC) || CHECKBIT32(opt, SECU3IO::COPT_CKPS_NPLUS1));
 mp_view->mp_ParamDeskDlg->EnableInputsMerging(!CHECKBIT32(opt, SECU3IO::COPT_CKPS_2CHIGN));
 mp_view->mp_ParamDeskDlg->EnableRisingSpark(CHECKBIT32(opt, SECU3IO::COPT_DWELL_CONTROL) && !CHECKBIT32(opt, SECU3IO::COPT_CKPS_2CHIGN));
 mp_view->mp_ParamDeskDlg->EnableUseCamRef(CHECKBIT32(opt, SECU3IO::COPT_PHASE_SENSOR));

 mp_view->mp_ParamDeskDlg->EnableFuelInjection(CHECKBIT32(opt, SECU3IO::COPT_FUEL_INJECT));
 mp_view->mp_ParamDeskDlg->EnableLambda(CHECKBIT32(opt, SECU3IO::COPT_FUEL_INJECT) || CHECKBIT32(opt, SECU3IO::COPT_CARB_AFR) || (fnc.GD_CONTROL && CHECKBIT32(opt, SECU3IO::COPT_GD_CONTROL)));
 mp_view->mp_ParamDeskDlg->EnableGasdose(fnc.GD_CONTROL && CHECKBIT32(opt, SECU3IO::COPT_GD_CONTROL)); //GD
 mp_view->mp_ParamDeskDlg->EnableChoke(fnc.SM_CONTROL && CHECKBIT32(opt, SECU3IO::COPT_SM_CONTROL));

 mp_view->mp_ParamDeskDlg->EnableChokeCtrls(!CHECKBIT32(opt, SECU3IO::COPT_FUEL_INJECT));

 SetViewFirmwareValues();
}

void CFirmwareTabController::OnOpenFlashFromFile(void)
{
 bool result;
 std::vector<BYTE> buff_container(m_fpp.m_total_size, 0);
 BYTE *buff = &buff_container[0];
 _TSTRING opened_file_name = _T("");
 
 if (!m_comm->m_pBootLoader->IsIdle())
  if (!ErrorMsg::AskUserAboutTabLeaving())
   return;
  
 bool is_continue = CheckChangesAskAndSaveFirmware();
 if (!is_continue)
  return;  //пользователь передумал

 //!!! без вычисления и записи контрольной суммы в буфер
 std::vector<int> sizes;
 sizes.push_back(m_fpp.m_total_size);
 result  = FirmwareFileUtils::LoadFLASHFromFile(buff, sizes, NULL, NULL, &opened_file_name);
 if (result && _CheckFirmwareCompatibilityAndAskUser(buff) && _CheckQuartzCompatibilityAndAskUser(buff)) //user OK?
 {
  if (FirmwareFileUtils::CheckFirmwareIntegrity(buff, m_fpp.m_total_size))
   PrepareOnLoadFLASH(buff, _TSTRING(opened_file_name));
 }
}

void CFirmwareTabController::OnDropFile(_TSTRING fileName)
{
 bool result;
 std::vector<BYTE> buff_container(m_fpp.m_total_size, 0);
 BYTE *buff = &buff_container[0];
 _TSTRING opened_file_name = _T("");

 bool is_continue = CheckChangesAskAndSaveFirmware();
 if (!is_continue)
  return;  //пользователь передумал

 std::vector<int> sizes;
 sizes.push_back(m_fpp.m_total_size);
 result  = FirmwareFileUtils::LoadFLASHFromFile(buff, sizes, NULL, NULL, &opened_file_name, &fileName);
 if (result && _CheckFirmwareCompatibilityAndAskUser(buff) && _CheckQuartzCompatibilityAndAskUser(buff)) //user OK?
 {
  if (FirmwareFileUtils::CheckFirmwareIntegrity(buff, m_fpp.m_total_size))
   PrepareOnLoadFLASH(buff, _TSTRING(opened_file_name));
 }
}

void CFirmwareTabController::OnSaveFlashToFile(void)
{
 std::vector<BYTE> buff_container(m_fpp.m_total_size, 0);
 BYTE *buff = &buff_container[0];
 CString opened_file_name = _T("");

 m_fwdm->StoreBytes(buff);

 //в случае подтверждения пользователя, также будует
 //вычислена контрольная сумма и сохранена в массив с прошивкой
 bool result = FirmwareFileUtils::SaveFLASHToFile(buff, m_fpp.m_total_size, m_fwdm, &opened_file_name,true);
 if (result)
 {
  //контрольная сумма была сохранена только вмассив с прошивкий которая сохранялась,
  //так как сохранение было подтверждено, то теперь можно обновить и массив с активной прошивкой
  m_fwdm->CalculateAndPlaceFirmwareCRC();

  //данные были успешно сохранены - можно сбрасывать признак модификации
  m_fwdm->ResetModified();

  //после сохранения "Save As" обновляем имя открытого файла
  m_fwdm->SetFWFileName(_TSTRING(opened_file_name));
  mp_view->SetFirmwareName(m_fwdm->GetFWFileName());

  //устанавливаем значения только в графики
  SetViewChartsValues();
  mp_view->mp_TablesPanel->TransformValues(); //transform values in some maps before they will be rendered for user
  mp_view->mp_TablesPanel->UpdateOpenedCharts();

  mp_view->SetFirmwareCRCs(m_fwdm->GetCRC16StoredInActiveFirmware(),m_fwdm->CalculateCRC16OfActiveFirmware());
 }
}

void CFirmwareTabController::OnFWInformationTextChanged(void)
{
 CString string = mp_view->GetFWInformationText();
 m_fwdm->SetSignatureInfo(_TSTRING(string));
}

bool CFirmwareTabController::IsFirmwareOpened()
{
 return m_fwdm->IsLoaded();
}

//эта функция не обновляет графики, нужно еще вызывать UpdateOpenedCharts()!
void CFirmwareTabController::SetViewChartsValues(void)
{
 m_fwdm->GetAttenuatorMap(mp_view->mp_TablesPanel->GetAttenuatorMap(false),false);
 m_fwdm->GetAttenuatorMap(mp_view->mp_TablesPanel->GetAttenuatorMap(true),true);

 m_fwdm->GetDwellCntrlMap(mp_view->mp_TablesPanel->GetDwellCntrlMap(false),false);
 m_fwdm->GetDwellCntrlMap(mp_view->mp_TablesPanel->GetDwellCntrlMap(true),true);

 m_fwdm->GetCTSCurveMap(mp_view->mp_TablesPanel->GetCTSCurveMap(false),false);
 m_fwdm->GetCTSCurveMap(mp_view->mp_TablesPanel->GetCTSCurveMap(true),true);
 mp_view->mp_TablesPanel->SetCTSXAxisEdits(m_fwdm->GetCTSMapVoltageLimit(0), m_fwdm->GetCTSMapVoltageLimit(1));

 m_fwdm->GetATSCurveMap(mp_view->mp_TablesPanel->GetATSCurveMap(false),false);
 m_fwdm->GetATSCurveMap(mp_view->mp_TablesPanel->GetATSCurveMap(true),true);
 mp_view->mp_TablesPanel->SetATSXAxisEdits(m_fwdm->GetATSMapVoltageLimit(0), m_fwdm->GetATSMapVoltageLimit(1));

 m_fwdm->GetATSAACMap(mp_view->mp_TablesPanel->GetATSAACMap(false),false);
 m_fwdm->GetATSAACMap(mp_view->mp_TablesPanel->GetATSAACMap(true),true);

 m_fwdm->GetGasdosePosMap(mp_view->mp_TablesPanel->GetGasdosePosMap(false),false);
 m_fwdm->GetGasdosePosMap(mp_view->mp_TablesPanel->GetGasdosePosMap(true),true);

 m_fwdm->GetBarocorrMap(mp_view->mp_TablesPanel->GetBarocorrMap(false),false);
 m_fwdm->GetBarocorrMap(mp_view->mp_TablesPanel->GetBarocorrMap(true),true);

 m_fwdm->GetManIgntimMap(mp_view->mp_TablesPanel->GetManIgntimMap(false),false);
 m_fwdm->GetManIgntimMap(mp_view->mp_TablesPanel->GetManIgntimMap(true),true);

 m_fwdm->GetTmp2CurveMap(mp_view->mp_TablesPanel->GetTmp2CurveMap(false),false);
 m_fwdm->GetTmp2CurveMap(mp_view->mp_TablesPanel->GetTmp2CurveMap(true),true);

 m_fwdm->GetCrkTempMap(mp_view->mp_TablesPanel->GetCrkTempMap(false),false);
 m_fwdm->GetCrkTempMap(mp_view->mp_TablesPanel->GetCrkTempMap(true),true);

 m_fwdm->GetEHPauseMap(mp_view->mp_TablesPanel->GetEHPauseMap(false),false);
 m_fwdm->GetEHPauseMap(mp_view->mp_TablesPanel->GetEHPauseMap(true),true);
 
 m_fwdm->GetRPMGridMap(mp_view->mp_TablesPanel->GetRPMGrid());

 if (m_current_funset_index==-1)
  return;
 m_fwdm->GetStartMap(m_current_funset_index,mp_view->mp_TablesPanel->GetStartMap(false),false);
 m_fwdm->GetStartMap(m_current_funset_index,mp_view->mp_TablesPanel->GetStartMap(true),true);

 m_fwdm->GetIdleMap(m_current_funset_index,mp_view->mp_TablesPanel->GetIdleMap(false),false);
 m_fwdm->GetIdleMap(m_current_funset_index,mp_view->mp_TablesPanel->GetIdleMap(true),true);

 m_fwdm->GetWorkMap(m_current_funset_index,mp_view->mp_TablesPanel->GetWorkMap(false),false);
 m_fwdm->GetWorkMap(m_current_funset_index,mp_view->mp_TablesPanel->GetWorkMap(true),true);

 m_fwdm->GetTempMap(m_current_funset_index,mp_view->mp_TablesPanel->GetTempMap(false),false);
 m_fwdm->GetTempMap(m_current_funset_index,mp_view->mp_TablesPanel->GetTempMap(true),true);

 //fuel injection
 m_fwdm->GetVEMap(m_current_funset_index,mp_view->mp_TablesPanel->GetVEMap(false),false);
 m_fwdm->GetVEMap(m_current_funset_index,mp_view->mp_TablesPanel->GetVEMap(true),true);

 m_fwdm->GetAFRMap(m_current_funset_index,mp_view->mp_TablesPanel->GetAFRMap(false),false);
 m_fwdm->GetAFRMap(m_current_funset_index,mp_view->mp_TablesPanel->GetAFRMap(true),true);

 m_fwdm->GetCrnkMap(m_current_funset_index,mp_view->mp_TablesPanel->GetCrnkMap(false),false);
 m_fwdm->GetCrnkMap(m_current_funset_index,mp_view->mp_TablesPanel->GetCrnkMap(true),true);

 m_fwdm->GetWrmpMap(m_current_funset_index,mp_view->mp_TablesPanel->GetWrmpMap(false),false);
 m_fwdm->GetWrmpMap(m_current_funset_index,mp_view->mp_TablesPanel->GetWrmpMap(true),true);

 m_fwdm->GetDeadMap(m_current_funset_index,mp_view->mp_TablesPanel->GetDeadMap(false),false);
 m_fwdm->GetDeadMap(m_current_funset_index,mp_view->mp_TablesPanel->GetDeadMap(true),true);

 m_fwdm->GetIdlrMap(m_current_funset_index,mp_view->mp_TablesPanel->GetIdlrMap(false),false);
 m_fwdm->GetIdlrMap(m_current_funset_index,mp_view->mp_TablesPanel->GetIdlrMap(true),true);

 m_fwdm->GetIdlcMap(m_current_funset_index,mp_view->mp_TablesPanel->GetIdlcMap(false),false);
 m_fwdm->GetIdlcMap(m_current_funset_index,mp_view->mp_TablesPanel->GetIdlcMap(true),true);

 m_fwdm->GetAETPSMap(m_current_funset_index,mp_view->mp_TablesPanel->GetAETPSMap(false),false);
 m_fwdm->GetAETPSMap(m_current_funset_index,mp_view->mp_TablesPanel->GetAETPSMap(true),true);

 m_fwdm->GetAERPMMap(m_current_funset_index,mp_view->mp_TablesPanel->GetAERPMMap(false),false);
 m_fwdm->GetAERPMMap(m_current_funset_index,mp_view->mp_TablesPanel->GetAERPMMap(true),true);

 m_fwdm->GetAftstrMap(m_current_funset_index,mp_view->mp_TablesPanel->GetAftstrMap(false),false);
 m_fwdm->GetAftstrMap(m_current_funset_index,mp_view->mp_TablesPanel->GetAftstrMap(true),true);

 m_fwdm->GetITMap(m_current_funset_index,mp_view->mp_TablesPanel->GetITMap(false),false);
 m_fwdm->GetITMap(m_current_funset_index,mp_view->mp_TablesPanel->GetITMap(true),true);

 m_fwdm->GetITRPMMap(m_current_funset_index,mp_view->mp_TablesPanel->GetITRPMMap(false),false);
 m_fwdm->GetITRPMMap(m_current_funset_index,mp_view->mp_TablesPanel->GetITRPMMap(true),true);

 m_fwdm->GetRigidMap(m_current_funset_index,mp_view->mp_TablesPanel->GetRigidMap(false),false);
 m_fwdm->GetRigidMap(m_current_funset_index,mp_view->mp_TablesPanel->GetRigidMap(true),true);

 m_fwdm->GetEGOCurveMap(m_current_funset_index,mp_view->mp_TablesPanel->GetEGOCurveMap(false),false);
 m_fwdm->GetEGOCurveMap(m_current_funset_index,mp_view->mp_TablesPanel->GetEGOCurveMap(true),true);

 m_fwdm->GetIACCorrMap(m_current_funset_index,mp_view->mp_TablesPanel->GetIACCMap(false),false);
 m_fwdm->GetIACCorrMap(m_current_funset_index,mp_view->mp_TablesPanel->GetIACCMap(true),true);

 m_fwdm->GetIACCorrWMap(m_current_funset_index,mp_view->mp_TablesPanel->GetIACCWMap(false),false);
 m_fwdm->GetIACCorrWMap(m_current_funset_index,mp_view->mp_TablesPanel->GetIACCWMap(true),true);

 m_fwdm->GetIATCLTMap(m_current_funset_index,mp_view->mp_TablesPanel->GetIATCLTMap(false),false);
 m_fwdm->GetIATCLTMap(m_current_funset_index,mp_view->mp_TablesPanel->GetIATCLTMap(true),true);

 m_fwdm->GetTpsswtMap(m_current_funset_index,mp_view->mp_TablesPanel->GetTpsswtMap(false),false);
 m_fwdm->GetTpsswtMap(m_current_funset_index,mp_view->mp_TablesPanel->GetTpsswtMap(true),true);

 m_fwdm->GetGtscMap(m_current_funset_index,mp_view->mp_TablesPanel->GetGtscMap(false),false);
 m_fwdm->GetGtscMap(m_current_funset_index,mp_view->mp_TablesPanel->GetGtscMap(true),true);

 m_fwdm->GetGpscMap(m_current_funset_index,mp_view->mp_TablesPanel->GetGpscMap(false),false);
 m_fwdm->GetGpscMap(m_current_funset_index,mp_view->mp_TablesPanel->GetGpscMap(true),true);

 m_fwdm->GetAtscMap(m_current_funset_index,mp_view->mp_TablesPanel->GetAtscMap(false),false);
 m_fwdm->GetAtscMap(m_current_funset_index,mp_view->mp_TablesPanel->GetAtscMap(true),true);
}

void CFirmwareTabController::SetViewFirmwareValues(void)
{
 if (m_fwdm->IsLoaded()==false)
  return;

 CString string = m_fwdm->GetSignatureInfo().c_str();
 mp_view->SetFWInformationText(string);

 SetViewChartsValues();

 std::vector<_TSTRING> funset_names = m_fwdm->GetFunctionsSetNames();
 mp_view->mp_TablesPanel->SetFunSetListBox(funset_names);

 mp_view->mp_TablesPanel->TransformValues(); //transform values in some maps before they will be rendered for user
 mp_view->mp_TablesPanel->UpdateOpenedCharts();

 //если было выделение в списке, то восстанавлваем его
 mp_view->mp_TablesPanel->SetFunSetListBoxSelection(m_current_funset_index);

 mp_view->SetFirmwareName(m_fwdm->GetFWFileName());

 mp_view->SetFirmwareCRCs(m_fwdm->GetCRC16StoredInActiveFirmware(),m_fwdm->CalculateCRC16OfActiveFirmware());

 mp_view->mp_ParamDeskDlg->SetFunctionsNames(funset_names);
 BYTE descriptor = mp_view->mp_ParamDeskDlg->GetCurrentDescriptor();
 BYTE paramdata[256];
 m_fwdm->GetDefParamValues(descriptor,paramdata);
 mp_view->mp_ParamDeskDlg->SetValues(descriptor,paramdata);

 //Attach fwdm to children controllers
 this->mp_iorCntr->AttachFWDM(m_fwdm);
}

//вкладка может быть закрыта, а график может быть по прежнему в открытом состоянии и изменен.
//В этом случае данная функция может вызываться и при закрытой вкладке.
void CFirmwareTabController::OnMapChanged(int i_type)
{
 switch(i_type)
 {
   //ignition maps
  case TYPE_MAP_DA_START:
   ASSERT(m_current_funset_index!=-1);
   m_fwdm->SetStartMap(m_current_funset_index, mp_view->mp_TablesPanel->GetStartMap(false));
   break;
  case TYPE_MAP_DA_IDLE:
   ASSERT(m_current_funset_index!=-1);
   m_fwdm->SetIdleMap(m_current_funset_index, mp_view->mp_TablesPanel->GetIdleMap(false));
   break;
  case TYPE_MAP_DA_WORK:
   ASSERT(m_current_funset_index!=-1);
   m_fwdm->SetWorkMap(m_current_funset_index, mp_view->mp_TablesPanel->GetWorkMap(false));
   break;
  case TYPE_MAP_DA_TEMP_CORR:
   ASSERT(m_current_funset_index!=-1);
   m_fwdm->SetTempMap(m_current_funset_index, mp_view->mp_TablesPanel->GetTempMap(false));
   break;
   //fuel injection maps
  case TYPE_MAP_INJ_VE:
   ASSERT(m_current_funset_index!=-1);
   m_fwdm->SetVEMap(m_current_funset_index, mp_view->mp_TablesPanel->GetVEMap(false));
   break;
  case TYPE_MAP_INJ_AFR:
   ASSERT(m_current_funset_index!=-1);
   m_fwdm->SetAFRMap(m_current_funset_index, mp_view->mp_TablesPanel->GetAFRMap(false));
   break;
  case TYPE_MAP_INJ_CRNK:
   ASSERT(m_current_funset_index!=-1);
   m_fwdm->SetCrnkMap(m_current_funset_index, mp_view->mp_TablesPanel->GetCrnkMap(false));
   break;
  case TYPE_MAP_INJ_WRMP:
   ASSERT(m_current_funset_index!=-1);
   m_fwdm->SetWrmpMap(m_current_funset_index, mp_view->mp_TablesPanel->GetWrmpMap(false));
   break;
  case TYPE_MAP_INJ_DEAD:
   ASSERT(m_current_funset_index!=-1);
   m_fwdm->SetDeadMap(m_current_funset_index, mp_view->mp_TablesPanel->GetDeadMap(false));
   break;
  case TYPE_MAP_INJ_IDLR:
   ASSERT(m_current_funset_index!=-1);
   m_fwdm->SetIdlrMap(m_current_funset_index, mp_view->mp_TablesPanel->GetIdlrMap(false));
   break;
  case TYPE_MAP_INJ_IDLC:
   ASSERT(m_current_funset_index!=-1);
   m_fwdm->SetIdlcMap(m_current_funset_index, mp_view->mp_TablesPanel->GetIdlcMap(false));
   break;
  case TYPE_MAP_INJ_AETPS:
   ASSERT(m_current_funset_index!=-1);
   m_fwdm->SetAETPSMap(m_current_funset_index, mp_view->mp_TablesPanel->GetAETPSMap(false));
   break;
  case TYPE_MAP_INJ_AERPM:
   ASSERT(m_current_funset_index!=-1);
   m_fwdm->SetAERPMMap(m_current_funset_index, mp_view->mp_TablesPanel->GetAERPMMap(false));
   break;
  case TYPE_MAP_INJ_AFTSTR:
   ASSERT(m_current_funset_index!=-1);
   m_fwdm->SetAftstrMap(m_current_funset_index, mp_view->mp_TablesPanel->GetAftstrMap(false));
   break;
  case TYPE_MAP_INJ_IT:
   ASSERT(m_current_funset_index!=-1);
   m_fwdm->SetITMap(m_current_funset_index, mp_view->mp_TablesPanel->GetITMap(false));
   break;
  case TYPE_MAP_INJ_ITRPM:
   ASSERT(m_current_funset_index!=-1);
   m_fwdm->SetITRPMMap(m_current_funset_index, mp_view->mp_TablesPanel->GetITRPMMap(false));
   break;
  case TYPE_MAP_INJ_RIGID:
   ASSERT(m_current_funset_index!=-1);
   m_fwdm->SetRigidMap(m_current_funset_index, mp_view->mp_TablesPanel->GetRigidMap(false));
   break;
  case TYPE_MAP_INJ_EGOCRV:
   ASSERT(m_current_funset_index!=-1);
   m_fwdm->SetEGOCurveMap(m_current_funset_index, mp_view->mp_TablesPanel->GetEGOCurveMap(false));
   break;
  case TYPE_MAP_INJ_IACC:
   ASSERT(m_current_funset_index!=-1);
   m_fwdm->SetIACCorrMap(m_current_funset_index, mp_view->mp_TablesPanel->GetIACCMap(false));
   break;
  case TYPE_MAP_INJ_IACCW:
   ASSERT(m_current_funset_index!=-1);
   m_fwdm->SetIACCorrWMap(m_current_funset_index, mp_view->mp_TablesPanel->GetIACCWMap(false));
   break;
  case TYPE_MAP_INJ_IATCLT:
   ASSERT(m_current_funset_index!=-1);
   m_fwdm->SetIATCLTMap(m_current_funset_index, mp_view->mp_TablesPanel->GetIATCLTMap(false));
   break;
  case TYPE_MAP_INJ_TPSSWT:
   ASSERT(m_current_funset_index!=-1);
   m_fwdm->SetTpsswtMap(m_current_funset_index, mp_view->mp_TablesPanel->GetTpsswtMap(false));
   break;
  case TYPE_MAP_INJ_GTSC:
   ASSERT(m_current_funset_index!=-1);
   m_fwdm->SetGtscMap(m_current_funset_index, mp_view->mp_TablesPanel->GetGtscMap(false));
   break;
  case TYPE_MAP_INJ_GPSC:
   ASSERT(m_current_funset_index!=-1);
   m_fwdm->SetGpscMap(m_current_funset_index, mp_view->mp_TablesPanel->GetGpscMap(false));
   break;
  case TYPE_MAP_INJ_ATSC:
   ASSERT(m_current_funset_index!=-1);
   m_fwdm->SetAtscMap(m_current_funset_index, mp_view->mp_TablesPanel->GetAtscMap(false));
   break;

   //separate maps
  case TYPE_MAP_ATTENUATOR:
   m_fwdm->SetAttenuatorMap(mp_view->mp_TablesPanel->GetAttenuatorMap(false));
   break;
  case TYPE_MAP_DWELLCNTRL:
   m_fwdm->SetDwellCntrlMap(mp_view->mp_TablesPanel->GetDwellCntrlMap(false));
   break;
  case TYPE_MAP_CTS_CURVE:
   m_fwdm->SetCTSCurveMap(mp_view->mp_TablesPanel->GetCTSCurveMap(false));
   break;
  case TYPE_MAP_ATS_CURVE:
   m_fwdm->SetATSCurveMap(mp_view->mp_TablesPanel->GetATSCurveMap(false));
   break;
  case TYPE_MAP_ATS_CORR:
   m_fwdm->SetATSAACMap(mp_view->mp_TablesPanel->GetATSAACMap(false));
   break;
  case TYPE_MAP_GASDOSE:
   m_fwdm->SetGasdosePosMap(mp_view->mp_TablesPanel->GetGasdosePosMap(false));
   break;
  case TYPE_MAP_BAROCORR:
   m_fwdm->SetBarocorrMap(mp_view->mp_TablesPanel->GetBarocorrMap(false));
   break;
  case TYPE_MAP_MANIGNTIM:
   m_fwdm->SetManIgntimMap(mp_view->mp_TablesPanel->GetManIgntimMap(false));
   break;
  case TYPE_MAP_TMP2_CURVE:
   m_fwdm->SetTmp2CurveMap(mp_view->mp_TablesPanel->GetTmp2CurveMap(false));
   break;
  case TYPE_MAP_CRKCLT_CORR:
   m_fwdm->SetCrkTempMap(mp_view->mp_TablesPanel->GetCrkTempMap(false));
   break;
  case TYPE_MAP_EH_PAUSE:
   m_fwdm->SetEHPauseMap(mp_view->mp_TablesPanel->GetEHPauseMap(false));
   break;
 }
}

//Состояние выделения в списке семейств характеристик изменилось.
// i_selected_index = -1 if there are no selected item
void CFirmwareTabController::OnFunSetSelectionChanged(int i_selected_index)
{
 m_current_funset_index = i_selected_index;

 if (m_current_funset_index != -1)
 { //только если в списке выбрано
  SetViewChartsValues();
  mp_view->mp_TablesPanel->TransformValues(); //transform values in some maps before they will be rendered for user
  mp_view->mp_TablesPanel->UpdateOpenedCharts();
 }
}

void CFirmwareTabController::OnFunSetNamechanged(int i_index_of_item, CString i_new_name)
{
 m_fwdm->SetFunctionsSetName(i_index_of_item,_TSTRING(i_new_name));
}

void CFirmwareTabController::OnCTSXAxisEditChanged(int i_type, float i_value)
{
 m_fwdm->SetCTSMapVoltageLimit(i_type, i_value);
}

void CFirmwareTabController::OnATSXAxisEditChanged(int i_type, float i_value)
{
 m_fwdm->SetATSMapVoltageLimit(i_type, i_value);
}

void CFirmwareTabController::OnModificationCheckTimer(void)
{
 bool modified = m_fwdm->IsModified();
 mp_view->SetModified(modified);
 if (m_fwdm->IsLoaded())
  mp_view->SetFirmwareCRCs(m_fwdm->GetCRC16StoredInActiveFirmware(),m_fwdm->CalculateCRC16OfActiveFirmware());
}

void CFirmwareTabController::OnReadFlashFromSECU(void)
{
 if (!m_comm->m_pBootLoader->IsIdle())
  return;
 //I don't like "copy/paste" paradigm of programming...
 m_bl_read_flash_mode = MODE_RD_FLASH_TO_BUFF_FOR_LOAD;
 _OnReadFlashToFile();
}

void CFirmwareTabController::OnWriteFlashToSECU(void)
{
 if (!m_comm->m_pBootLoader->IsIdle())
  return;
 m_fwdm->StoreBytes(m_bl_data);
 StartWritingOfFLASHFromBuff();
}

void CFirmwareTabController::OnImportDataFromAnotherFW()
{
 bool is_continue = CheckChangesAskAndSaveFirmware();
 if (!is_continue)
  return;  //пользователь передумал

 bool result;
 std::vector<int> sizes = PlatformParamHolder::GetFirmwareSizes();
 std::vector<int>::const_iterator it = std::max_element(sizes.begin(), sizes.end());
 std::vector<BYTE> buff_container((*it), 0);
 BYTE *buff = &buff_container[0];
 _TSTRING opened_file_name = _T("");
 int selected_size = 0;

 //!!! без вычисления и записи контрольной суммы в буфер
 result  = FirmwareFileUtils::LoadFLASHFromFile(buff, sizes, NULL, &selected_size, &opened_file_name);
 //Get platform information
 EECUPlatform platform_id;
 if (!PlatformParamHolder::GetPlatformIdByFirmwareMagic(buff, selected_size, platform_id))
 {
  if (!PlatformParamHolder::GetPlatformIdByFirmwareSize(selected_size, platform_id))
   return; //error
 }
 PlatformParamHolder params(platform_id);
 if (result && _CheckFirmwareCompatibilityAndAskUser(buff, &params) && _CheckQuartzCompatibilityAndAskUser(buff, selected_size)) //user OK?
 {
  m_fwdm->LoadDataBytesFromAnotherFirmware(buff, &params.GetFlashParameters());
  PrepareOnLoadFLASH(NULL, m_fwdm->GetFWFileName());
 }
}

void CFirmwareTabController::OnImportDataFromSECU3(void)
{
 m_bl_read_flash_mode = MODE_RD_FLASH_FOR_IMPORT_DATA;
 _OnReadFlashToFile();
}

void CFirmwareTabController::OnParamDeskTabActivate(void)
{
 BYTE descriptor = mp_view->mp_ParamDeskDlg->GetCurrentDescriptor();
 BYTE paramdata[256];
 m_fwdm->GetDefParamValues(descriptor,paramdata);
 mp_view->mp_ParamDeskDlg->SetValues(descriptor,paramdata);
}

//from ParamDesk
void CFirmwareTabController::OnParamDeskChangeInTab(void)
{
 BYTE descriptor = mp_view->mp_ParamDeskDlg->GetCurrentDescriptor();
 BYTE paramdata[256];
 mp_view->mp_ParamDeskDlg->GetValues(descriptor,paramdata);
 m_fwdm->SetDefParamValues(descriptor,paramdata);
}

void CFirmwareTabController::OnImportMapsFromMPSZ(void)
{
 FWMapsDataHolder data;
 MPSZImportController import(&data);
 m_fwdm->GetMapsData(&data);
 int result = import.DoImport();
 if (result == IDOK)
 {
  m_fwdm->SetMapsData(&data);
  SetViewFirmwareValues();
 }
}

void CFirmwareTabController::OnImportMapsFromSECU3(void)
{
 FWMapsDataHolder data;
 SECU3ImportController import(&data);
 import.setFileReader(&FirmwareFileUtils::LoadFLASHFromFile);
 m_fwdm->GetMapsData(&data);
 int result = import.DoImport();
 if (result == IDOK)
 {
  m_fwdm->SetMapsData(&data);
  SetViewFirmwareValues();
 }
}

void CFirmwareTabController::OnImportMapsFromS3F(void)
{
 FWMapsDataHolder data;
 S3FImportController import(&data);
 m_fwdm->GetMapsData(&data);
 int result = import.DoImport();
 if (result == IDOK)
 {
  m_fwdm->SetMapsData(&data);
  SetViewFirmwareValues();
 }
}

void CFirmwareTabController::OnImportDefParamsFromEEPROMFile()
{
 std::vector<BYTE> eeprom_buffer(m_epp.m_size, 0x00);
 BYTE *eeprom = &eeprom_buffer[0];

 std::vector<int> sizes;
 sizes.push_back(m_epp.m_size);
 bool result = FirmwareFileUtils::LoadEEPROMFromFile(eeprom, sizes);

 if (!result)
  return; //cancel

 //проверка контрольной суммы загружаемых параметров и вывод предупреждения
 if (!m_edm->VerifyParamsCheckSum(eeprom))
 {
  if (IDCANCEL==AfxMessageBox(IDS_FW_EEPROM_DEF_PARAMS_CRC_INVALID, MB_OKCANCEL))
   return; //user canceled
 }

 m_fwdm->LoadDefParametersFromBuffer(eeprom + m_edm->GetParamsStartAddr(), ErrorMsg::AskUserAboutVrefCompensation);
 SetViewFirmwareValues(); //Update!
}

void CFirmwareTabController::OnImportTablesFromEEPROMFile()
{
 FWMapsDataHolder data;
 EEPROMImportController import(&data);
 import.setFileReader(&FirmwareFileUtils::LoadEEPROMFromFile);
 m_fwdm->GetMapsData(&data);
 int result = import.DoImport();
 if (result == IDOK)
 {
  m_fwdm->SetMapsData(&data);
  SetViewFirmwareValues();
 }
}

void CFirmwareTabController::OnExportMapsToMPSZ(void)
{
 FWMapsDataHolder data;
 MPSZExportController export_cntr(&data);
 m_fwdm->GetMapsData(&data);
 export_cntr.DoExport();
}

void CFirmwareTabController::OnExportMapsToSECU3(void)
{
 FWMapsDataHolder data;
 SECU3ExportController export_cntr(&data);
 export_cntr.setFileReader(&FirmwareFileUtils::LoadFLASHFromFile);
 m_fwdm->GetMapsData(&data);
 export_cntr.DoExport();
}

void CFirmwareTabController::OnExportMapsToS3F(void)
{
 FWMapsDataHolder data;
 S3FExportController export_cntr(&data);
 m_fwdm->GetMapsData(&data);
 export_cntr.DoExport();
}

void CFirmwareTabController::OnEditRPMGrid(void)
{
 CFWRPMGridEditController cntr;
 cntr.AttachFWDM(m_fwdm);
 if (IDOK == cntr.Edit())
 {
  m_fwdm->GetRPMGridMap(mp_view->mp_TablesPanel->GetRPMGrid());
  mp_view->mp_TablesPanel->UpdateOpenedCharts();
 }
}

void CFirmwareTabController::OnEditFwConsts(void)
{
 CDynFieldsDialog dfd(mp_view, (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN) ? _T("Редактирование констант прошивки") : _T("Editing constants of firmware"), 300);

 SECU3IO::FwConstsData d;
 m_fwdm->GetFwConstsData(d);

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Длит. плавного входа в ПХХ"), _T("такт"), 0, 255, 1, 1, &d.fi_enter_strokes);
 else
  dfd.AppendItem(_T("Smoothing of forced idle entering"), _T("str"), 0, 255, 1, 1, &d.fi_enter_strokes);

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Длит. плавного выхода из ПХХ"), _T("такт"), 0, 255, 1, 1, &d.fi_leave_strokes);
 else
  dfd.AppendItem(_T("Smoothing of forced idle leaving"), _T("str"), 0, 255, 1, 1, &d.fi_leave_strokes);

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Добавка к РДВ при вкл. кондиционера"), _T("%"), 0.0f, 100.0f, 0.5, 1, &d.iac_cond_add);
 else
  dfd.AppendItem(_T("Add. to IAC pos on turn on of air cond. "), _T("%"), 0.0f, 100.0f, 0.5, 1, &d.iac_cond_add);

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Максимальная длительность впрыска"), _T("мс"), 0.0f, 100.0f, 0.1f, 1, &d.inj_max_pw);
 else
  dfd.AppendItem(_T("Max. injection pulse width"), _T("ms"), 0.0f, 100.0f, 0.1f, 1, &d.inj_max_pw);

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Порог ДТОЖ вкл. кондиционера"), _T("°C"), 0.0f, 120.0f, 0.25f, 2, &d.aircond_clt);
 else
  dfd.AppendItem(_T("CLT threshold for turn on of air cond."), _T("°C"), 0.0f, 120.0f, 0.25f, 2, &d.aircond_clt);

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Порог ДПДЗ вкл. кондиционера"), _T("%"), 0.0f, 120.0f, 0.5f, 1, &d.aircond_tps);
 else
  dfd.AppendItem(_T("TPS threshold for turn on of air cond."), _T("%"), 0.0f, 120.0f, 0.5f, 1, &d.aircond_tps);

 if (dfd.DoModal()==IDOK)
 {
  m_fwdm->SetFwConstsData(d);
 }
}

//Пользователь захотел получить информацию о прошивке из SECU-3
void CFirmwareTabController::OnFirmwareInfo(void)
{
 m_sbar->SetInformationText(MLL::LoadString(IDS_FW_READING_FW_SIGNATURE));
 SECU3IO::OPCompNc packet_data;
 packet_data.opcode = SECU3IO::OPCODE_READ_FW_SIG_INFO;
 m_comm->m_pControlApp->SendPacket(OP_COMP_NC,&packet_data);
 m_read_fw_sig_info_flag = true;
}

void CFirmwareTabController::OnViewFWOptions(void)
{
 BYTE fw_version[2];
 _ShowFWOptions(m_fwdm->GetSignatureInfo(), m_fwdm->GetFWOptions(), m_fwdm->GetFWVersion(fw_version));
}

bool CFirmwareTabController::IsViewFWOptionsAvailable(void)
{
 return m_fwdm->GetFWOptions() > 0;
}

bool CFirmwareTabController::IsIORemappingAvailable(void)
{
 return true;
}

void CFirmwareTabController::SetAttenuatorMap(const float* i_values)
{
 m_fwdm->SetAttenuatorMap(i_values);
 SetViewChartsValues();
 mp_view->mp_TablesPanel->UpdateOpenedCharts();
}

void CFirmwareTabController::GetAttenuatorMap(float* o_values)
{
 ASSERT(o_values);
 m_fwdm->GetAttenuatorMap(o_values, false); //<--NOTE: modified
}

void CFirmwareTabController::_ShowFWOptions(const _TSTRING& info, DWORD options, BYTE fw_version[2])
{
 if (options!=0)
 {
  _TSTRING str_options;
  str_options+=info;
  if (info.size() > 0)
   str_options+=_T("\n\n");
  int opt_count = 0;
  for(size_t i = 0; i < SECU3IO::SECU3_COMPILE_OPTIONS_BITS_COUNT; ++i)
  {
   if(CHECKBIT32(options, SECU3IO::secu3_compile_options_bits[i].first))
   {
    str_options+= SECU3IO::secu3_compile_options_bits[i].second;
    str_options+=_T("\n");
    ++opt_count;
   }
  }
  str_options+=MLL::GetString(IDS_FW_TOTAL_NUMOF_OPT);
  _TSSTREAM str;  str << _T(" ") << opt_count;
  str << _T("  Version v") << (int)fw_version[1] << _T(".") << (int)fw_version[0];
  str_options+=str.str();
  AfxMessageBox(str_options.c_str(), MB_OK|MB_ICONINFORMATION);
 }
}


void CFirmwareTabController::finishOnBootLoaderInfo(void)
{
 //активируем коммуникационный контроллер бутлоадера
 m_comm->SwitchOn(CCommunicationManager::OP_ACTIVATE_BOOTLOADER);

 //операция не блокирует поток - стековые переменные ей передавать нельзя!
 m_comm->m_pBootLoader->StartOperation(CBootLoader::BL_OP_READ_SIGNATURE,m_bl_data,0);

 m_sbar->ShowProgressBar(true);
 m_sbar->SetProgressPos(0);
}

void CFirmwareTabController::finishOnReadEepromToFile(void)
{
 //активируем коммуникационный контроллер бутлоадера
 m_comm->SwitchOn(CCommunicationManager::OP_ACTIVATE_BOOTLOADER);

 //операция не блокирует поток - стековые переменные ей передавать нельзя!
 m_comm->m_pBootLoader->StartOperation(CBootLoader::BL_OP_READ_EEPROM,m_bl_data,0);

 m_sbar->ShowProgressBar(true);
 m_sbar->SetProgressPos(0);
}

void CFirmwareTabController::finishOnWriteEepromFromFile(void)
{
 //активируем коммуникационный контроллер бутлоадера
 m_comm->SwitchOn(CCommunicationManager::OP_ACTIVATE_BOOTLOADER);

 //операция не блокирует поток - стековые переменные ей передавать нельзя!
 m_comm->m_pBootLoader->StartOperation(CBootLoader::BL_OP_WRITE_EEPROM,m_bl_data,0);

 m_sbar->ShowProgressBar(true);
 m_sbar->SetProgressPos(0);
}

void CFirmwareTabController::finish_OnReadFlashToFile(void)
{
 //активируем коммуникационный контроллер бутлоадера
 m_comm->SwitchOn(CCommunicationManager::OP_ACTIVATE_BOOTLOADER);

 //операция не блокирует поток - стековые переменные ей передавать нельзя!
 m_comm->m_pBootLoader->StartOperation(CBootLoader::BL_OP_READ_FLASH,m_bl_data, m_fpp.m_total_size);

 m_sbar->ShowProgressBar(true);
 m_sbar->SetProgressPos(0);
}

void CFirmwareTabController::finishStartWritingOfFLASHFromBuff(void)
{
 //активируем коммуникационный контроллер бутлоадера
 m_comm->SwitchOn(CCommunicationManager::OP_ACTIVATE_BOOTLOADER);

 //Если установлен режим прошивки только кода (без данных), то все несколько сложнее
 if (mp_view->IsProgrammeOnlyCode())
 {
  //Мы программируем только код, одако контрольная сумма останется посчитаной для старых данных. Поэтому нам необходимо
  //прочитать данные, обединить их с новым кодом, обновить контрольную сумму и только потом программировать.
  m_bl_read_flash_mode = MODE_RD_FLASH_TO_BUFF_MERGE_DATA;

  //сохраняем данные для того, чтобы позже объединить их с прочитанными "верхними" данными
  m_code_for_merge_size = m_fwdm->GetOnlyCodeSize(m_bl_data);
  memcpy(m_code_for_merge_with_overhead, m_bl_data, m_fpp.m_app_section_size);

  //Читаем немного больше байт, для того, чтобы гарантировано прочитать данные находящиеся в коде  
  size_t reducedSize = m_code_for_merge_size - 0x400; //1024 bytes more
  //операция не блокирует поток - стековые переменные ей передавать нельзя!
  m_comm->m_pBootLoader->StartOperation(CBootLoader::BL_OP_READ_FLASH, m_bl_data,
  m_fpp.m_app_section_size - reducedSize, //размер данных сверху над кодом программы
  reducedSize);                           //адрес начала "верхних" данных
 }
 else
 {//все очень просто
  m_comm->m_pBootLoader->StartOperation(CBootLoader::BL_OP_WRITE_FLASH,m_bl_data, m_fpp.m_app_section_size);
 }

 m_sbar->ShowProgressBar(true);
 m_sbar->SetProgressPos(0);
}

void CFirmwareTabController::OnCESettingsButton(void)
{
 CCESettingsDlg dialog;
 SECU3IO::CESettingsData data;
 m_fwdm->GetCESettingsData(data);
 dialog.SetValues(data);
 dialog.EnableSECU3TItems(CHECKBIT32(m_fwdm->GetFWOptions(), SECU3IO::COPT_SECU3T));
 dialog.EnableExtraIO(!CHECKBIT32(m_fwdm->GetFWOptions(), SECU3IO::COPT_SECU3T) && CHECKBIT32(m_fwdm->GetFWOptions(), SECU3IO::COPT_TPIC8101));
 WndSettings ws;
 mp_settings->GetWndSettings(ws);
 dialog.SetWndPosition(ws.m_CESettingsWnd_X, ws.m_CESettingsWnd_Y);

 if (dialog.DoModal() == IDOK)
 {
  dialog.GetValues(data);
  m_fwdm->SetCESettingsData(data);
 }

 CPoint wndPos = dialog.GetWndPosition();
 ws.m_CESettingsWnd_X = wndPos.x; 
 ws.m_CESettingsWnd_Y = wndPos.y;
 mp_settings->SetWndSettings(ws); 
}

void CFirmwareTabController::OnChangeSettingsMapEd(void)
{
 mp_settings->SetITEdMode(mp_view->mp_TablesPanel->GetITEdMode());

 MapPtMovStep mptms;
 mp_settings->GetMapPtMovStep(mptms);
 mptms.m_start_map = mp_view->mp_TablesPanel->GetPtMovStep(TYPE_MAP_DA_START);
 mptms.m_idle_map = mp_view->mp_TablesPanel->GetPtMovStep(TYPE_MAP_DA_IDLE);
 mptms.m_work_map = mp_view->mp_TablesPanel->GetPtMovStep(TYPE_MAP_DA_WORK);
 mptms.m_temp_map = mp_view->mp_TablesPanel->GetPtMovStep(TYPE_MAP_DA_TEMP_CORR);
 mptms.m_ve_map = mp_view->mp_TablesPanel->GetPtMovStep(TYPE_MAP_INJ_VE);
 mptms.m_afr_map = mp_view->mp_TablesPanel->GetPtMovStep(TYPE_MAP_INJ_AFR);
 mptms.m_crnk_map = mp_view->mp_TablesPanel->GetPtMovStep(TYPE_MAP_INJ_CRNK);
 mptms.m_wrmp_map = mp_view->mp_TablesPanel->GetPtMovStep(TYPE_MAP_INJ_WRMP);
 mptms.m_dead_map = mp_view->mp_TablesPanel->GetPtMovStep(TYPE_MAP_INJ_DEAD);
 mptms.m_idlr_map = mp_view->mp_TablesPanel->GetPtMovStep(TYPE_MAP_INJ_IDLR);
 mptms.m_idlc_map = mp_view->mp_TablesPanel->GetPtMovStep(TYPE_MAP_INJ_IDLC);
 mptms.m_aetps_map = mp_view->mp_TablesPanel->GetPtMovStep(TYPE_MAP_INJ_AETPS);
 mptms.m_aerpm_map = mp_view->mp_TablesPanel->GetPtMovStep(TYPE_MAP_INJ_AERPM);
 mptms.m_aftstr_map = mp_view->mp_TablesPanel->GetPtMovStep(TYPE_MAP_INJ_AFTSTR);
 mptms.m_it_map = mp_view->mp_TablesPanel->GetPtMovStep(TYPE_MAP_INJ_IT);
 mptms.m_itrpm_map = mp_view->mp_TablesPanel->GetPtMovStep(TYPE_MAP_INJ_ITRPM);
 mptms.m_rigid_map = mp_view->mp_TablesPanel->GetPtMovStep(TYPE_MAP_INJ_RIGID);
 mptms.m_egocrv_map = mp_view->mp_TablesPanel->GetPtMovStep(TYPE_MAP_INJ_EGOCRV);
 mptms.m_iacc_map = mp_view->mp_TablesPanel->GetPtMovStep(TYPE_MAP_INJ_IACC);
 mptms.m_iaccw_map = mp_view->mp_TablesPanel->GetPtMovStep(TYPE_MAP_INJ_IACCW);
 mptms.m_iatclt_map = mp_view->mp_TablesPanel->GetPtMovStep(TYPE_MAP_INJ_IATCLT);
 mptms.m_tpsswt_map = mp_view->mp_TablesPanel->GetPtMovStep(TYPE_MAP_INJ_TPSSWT);
 mptms.m_gtsc_map = mp_view->mp_TablesPanel->GetPtMovStep(TYPE_MAP_INJ_GTSC);
 mptms.m_gpsc_map = mp_view->mp_TablesPanel->GetPtMovStep(TYPE_MAP_INJ_GPSC);
 mptms.m_atsc_map = mp_view->mp_TablesPanel->GetPtMovStep(TYPE_MAP_INJ_ATSC);
 //separate
 mptms.m_attenuator_map = mp_view->mp_TablesPanel->GetPtMovStep(TYPE_MAP_ATTENUATOR);
 mptms.m_dwellcntrl_map = mp_view->mp_TablesPanel->GetPtMovStep(TYPE_MAP_DWELLCNTRL);
 mptms.m_cts_curve_map = mp_view->mp_TablesPanel->GetPtMovStep(TYPE_MAP_CTS_CURVE);
 mptms.m_ats_curve_map = mp_view->mp_TablesPanel->GetPtMovStep(TYPE_MAP_ATS_CURVE);
 mptms.m_ats_aac_map = mp_view->mp_TablesPanel->GetPtMovStep(TYPE_MAP_ATS_CORR);
 mptms.m_gasdose_map = mp_view->mp_TablesPanel->GetPtMovStep(TYPE_MAP_GASDOSE);
 mptms.m_barocorr_map = mp_view->mp_TablesPanel->GetPtMovStep(TYPE_MAP_BAROCORR);
 mptms.m_manigntim_map = mp_view->mp_TablesPanel->GetPtMovStep(TYPE_MAP_MANIGNTIM);
 mptms.m_tmp2_curve_map = mp_view->mp_TablesPanel->GetPtMovStep(TYPE_MAP_TMP2_CURVE);
 mptms.m_crktemp_map = mp_view->mp_TablesPanel->GetPtMovStep(TYPE_MAP_CRKCLT_CORR);
 mptms.m_eh_pause_map = mp_view->mp_TablesPanel->GetPtMovStep(TYPE_MAP_EH_PAUSE);
 mp_settings->SetMapPtMovStep(mptms);
}
