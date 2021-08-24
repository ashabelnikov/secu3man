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
#include "FWImpExp/FirmwareMasterCntr.h"
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
#include "ui-core/MsgBox.h"

using namespace fastdelegate;

#define EHKEY _T("FirmwareCntr")
#define DELAY_AFTER_BL_START 100
#define WAITING_BL_START_TIMEOUT 8000

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFirmwareTabController::CFirmwareTabController(CFirmwareTabDlg* i_view, CCommunicationManager* i_comm, CStatusBarManager* i_sbar, ISettingsData* ip_settings)
: mp_view(i_view)
, mp_comm(i_comm)
, mp_sbar(i_sbar)
, mp_settings(ip_settings)
, m_current_funset_index(-1)
, m_bl_read_flash_mode(MODE_RD_FLASH_TO_FILE)
, m_lastSel(0)
, m_bl_started_emergency(false)
, mp_iorCntr(new CFWIORemappingController(i_view->mp_IORemappingDlg.get()))
, m_moreSize(0x400) //1024
, m_clear_sbar_txt_on_conn(false)
, m_read_fw_sig_info_flag(false)
, m_blFinishOpCB(NULL)
, MapWndScrPos(ip_settings)
, m_active(false)
{
 PlatformParamHolder holder(ip_settings->GetECUPlatformType());
 m_fpp = holder.GetFlashParameters();
 m_epp = holder.GetEepromParameters();
 mp_fwdm = new CFirmwareDataMediator(holder.GetFlashParameters());
 ASSERT(mp_fwdm);
 //Set parameters for speed sensor calculations and set clock frequency (16 or 20 mHz)
 mp_fwdm->SetNumPulsesPer1Km(mp_settings->GetNumPulsesPer1Km());
 mp_fwdm->SetQuartzFrq(PlatformParamHolder::GetQuartzFreq(mp_settings->GetECUPlatformType()));
 mp_edm = new EEPROMDataMediator(holder.GetEepromParameters());
 mp_edm->SetNumPulsesPer1Km(mp_settings->GetNumPulsesPer1Km());
 mp_edm->SetQuartzFrq(PlatformParamHolder::GetQuartzFreq(mp_settings->GetECUPlatformType()));
 ASSERT(mp_edm);

 mp_bl_data = new BYTE[m_fpp.m_total_size + 1];
 ASSERT(mp_bl_data);
 mp_code_for_merge_with_overhead = new BYTE[m_fpp.m_total_size + 1];
 ASSERT(mp_code_for_merge_with_overhead);

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
 mp_view->setOnFirmwareMaster(MakeDelegate(this, &CFirmwareTabController::OnFirmwareMaster));

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
 delete mp_fwdm;
 delete mp_edm;
 delete[] mp_bl_data;
 delete mp_code_for_merge_with_overhead;
}

//изменились настройки
void CFirmwareTabController::OnSettingsChanged(int action)
{
 if (action == 2)
 {
  mp_view->EnableMakingChartsChildren(mp_settings->GetChildCharts());
  return;
 }

 if (action == 3)
 {
  mp_view->EnableToggleMapWnd(mp_settings->GetToggleMapWnd());
  return;
 }

 //включаем необходимый для данного контекста коммуникационный контроллер
 VERIFY(mp_comm->SwitchOn(CCommunicationManager::OP_ACTIVATE_APPLICATION, true));

 //Set parameters for speed sensor calculations
 mp_fwdm->SetNumPulsesPer1Km(mp_settings->GetNumPulsesPer1Km());
 //Set clock frequency (16 or 20 mHz)
 mp_fwdm->SetQuartzFrq(PlatformParamHolder::GetQuartzFreq(mp_settings->GetECUPlatformType()));

 mp_edm->SetNumPulsesPer1Km(mp_settings->GetNumPulsesPer1Km());
 mp_edm->SetQuartzFrq(PlatformParamHolder::GetQuartzFreq(mp_settings->GetECUPlatformType()));

 mp_view->mp_TablesPanel->SetITEdMode(mp_settings->GetITEdMode());
 mp_view->mp_TablesPanel->SetActiveVEMap(mp_settings->GetActiveVEMap());
}

void CFirmwareTabController::OnActivate(void)
{
 mp_comm->m_pBootLoader->EnableBlockedEEPROMOps(mp_settings->GetBldrEEPROMBlocks());
 m_active = true;
 mp_view->mp_ParamDeskDlg->SetITEdMode(mp_settings->GetITEdMode());
 mp_view->mp_TablesPanel->ShowOpenedCharts(true);
 //выбираем ранее выбранную вкладку на панели параметров
 bool result = mp_view->mp_ParamDeskDlg->SetCurSel(m_lastSel);

 if (IsFirmwareOpened())
 {
  DWORD opt = mp_fwdm->GetFWOptions();
  mp_view->mp_TablesPanel->SetSplitAngMode(CHECKBIT32(opt, SECU3IO::COPT_SPLIT_ANGLE));
 }

 //////////////////////////////////////////////////////////////////
 //подключаем контроллер к потоку данных идущих от SECU-3
 mp_comm->m_pAppAdapter->AddEventHandler(this,EHKEY);
 mp_comm->m_pBldAdapter->SetEventHandler(this);
 mp_comm->setOnSettingsChanged(MakeDelegate(this,&CFirmwareTabController::OnSettingsChanged));
 //////////////////////////////////////////////////////////////////

 //включаем необходимый для данного контекста коммуникационный контроллер
 VERIFY(mp_comm->SwitchOn(CCommunicationManager::OP_ACTIVATE_APPLICATION));

 m_modification_check_timer.SetTimer(this,&CFirmwareTabController::OnModificationCheckTimer,250);

 //Activate children controllers
 mp_iorCntr->OnActivate();

 SetViewFirmwareValues();

 mp_view->EnableMakingChartsChildren(mp_settings->GetChildCharts());
 mp_view->EnableToggleMapWnd(mp_settings->GetToggleMapWnd());

 mp_view->mp_TablesPanel->SetITEdMode(mp_settings->GetITEdMode());
 mp_view->mp_TablesPanel->SetActiveVEMap(mp_settings->GetActiveVEMap());

 MapPtMovStep mptms;
 mp_settings->GetMapPtMovStep(mptms);
 mp_view->mp_TablesPanel->SetPtMovStep(TYPE_MAP_DA_START, mptms.m_start_map);
 mp_view->mp_TablesPanel->SetPtMovStep(TYPE_MAP_DA_IDLE, mptms.m_idle_map);
 mp_view->mp_TablesPanel->SetPtMovStep(TYPE_MAP_DA_WORK, mptms.m_work_map);
 mp_view->mp_TablesPanel->SetPtMovStep(TYPE_MAP_DA_TEMP_CORR, mptms.m_temp_map);
 mp_view->mp_TablesPanel->SetPtMovStep(TYPE_MAP_DA_TEMPI_CORR, mptms.m_tempi_map);
 mp_view->mp_TablesPanel->SetPtMovStep(TYPE_MAP_INJ_VE, mptms.m_ve_map);
 mp_view->mp_TablesPanel->SetPtMovStep(TYPE_MAP_INJ_VE2, mptms.m_ve2_map);
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
 mp_view->mp_TablesPanel->SetPtMovStep(TYPE_MAP_PWM1, mptms.m_pwm1_map);
 mp_view->mp_TablesPanel->SetPtMovStep(TYPE_MAP_PWM2, mptms.m_pwm2_map);
 mp_view->mp_TablesPanel->SetPtMovStep(TYPE_MAP_INJ_IACMAT, mptms.m_iacmat_map);
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
 mp_view->mp_TablesPanel->SetPtMovStep(TYPE_MAP_CRANKING_THRD, mptms.m_cranking_thrd_map);
 mp_view->mp_TablesPanel->SetPtMovStep(TYPE_MAP_CRANKING_TIME, mptms.m_cranking_time_map);
 mp_view->mp_TablesPanel->SetPtMovStep(TYPE_MAP_SMAPABAN_THRD, mptms.m_smapaban_thrd_map);
 mp_view->mp_TablesPanel->SetPtMovStep(TYPE_MAP_KNOCK_ZONE, mptms.m_knock_zone_map);
 mp_view->mp_TablesPanel->SetPtMovStep(TYPE_MAP_GRTS_CURVE, mptms.m_grts_curve_map);
 mp_view->mp_TablesPanel->SetPtMovStep(TYPE_MAP_GRHEAT_DUTY, mptms.m_grheat_duty_map);
 mp_view->mp_TablesPanel->SetPtMovStep(TYPE_MAP_PWMIAC_UCOEF, mptms.m_pwmiac_ucoef_map);
 mp_view->mp_TablesPanel->SetPtMovStep(TYPE_MAP_AFTSTR_STRK0, mptms.m_aftstr_strk0_map);
 mp_view->mp_TablesPanel->SetPtMovStep(TYPE_MAP_AFTSTR_STRK1, mptms.m_aftstr_strk1_map);
 mp_view->mp_TablesPanel->SetPtMovStep(TYPE_MAP_GRVDELAY, mptms.m_grvaldel_map);
 mp_view->mp_TablesPanel->SetPtMovStep(TYPE_MAP_FTLS_CURVE, mptms.m_ftls_curve_map);
 mp_view->mp_TablesPanel->SetPtMovStep(TYPE_MAP_EGTS_CURVE, mptms.m_egts_curve_map);
 mp_view->mp_TablesPanel->SetPtMovStep(TYPE_MAP_OPS_CURVE, mptms.m_ops_curve_map);
 mp_view->mp_TablesPanel->SetPtMovStep(TYPE_MAP_MANINJPWC, mptms.m_maninjpwc_map);
 mp_view->mp_TablesPanel->SetPtMovStep(TYPE_MAP_MAF_CURVE, mptms.m_mafcurve_map);

 //симулируем изменение состояния для обновления контроллов, так как OnConnection вызывается только если
 //сбрывается или разрывается принудительно (путем деактивации коммуникационного контроллера)
 bool online_status = mp_comm->m_pControlApp->GetOnlineStatus();
 OnConnection(online_status); 
}

void CFirmwareTabController::OnDeactivate(void)
{
 m_active = false;
 mp_view->mp_TablesPanel->ShowOpenedCharts(false);
 //отключаемся от потока данных
 mp_comm->m_pAppAdapter->RemoveEventHandler(EHKEY);
 mp_sbar->SetInformationText(_T(""));
 mp_sbar->ShowProgressBar(false);
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
   mp_sbar->SetInformationText(string);

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
    mp_sbar->SetInformationText(MLL::LoadString(IDS_PM_PARAMS_HAS_BEEN_SAVED));
    return;
   case SECU3IO::OPCODE_SAVE_TABLSET:     //таблицы были сохранены
    mp_sbar->SetInformationText(MLL::LoadString(IDS_PM_TABLSET_HAS_BEEN_SAVED));    
    return;
   case SECU3IO::OPCODE_RESET_EEPROM:     //начался процесс сброса EEPROM
    if (p_ndata->opdata == 0x55)
    {
     mp_sbar->SetInformationText(MLL::LoadString(IDS_FW_RESET_EEPROM_STARTED));
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
   case SECU3IO::OPCODE_BL_MANSTART: //confirmation that user has started boot loader manually
    if (p_ndata->opdata == SECU3IO::OPDATA_BL_MANSTART)
     mp_view->SetBLStartedEmergency(true);
    return;
  } 
 }
 else if (i_descriptor == INJDRV_PAR)
  mp_comm->m_pControlApp->ChangeContext(SENSOR_DAT);
}

void CFirmwareTabController::OnConnection(const bool i_online)
{
 int state;
 ASSERT(mp_sbar);

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
   mp_sbar->SetInformationText(_T(""));
   m_clear_sbar_txt_on_conn = false;
  }
 }
 else
 { //перешли в оффлайн
  if (!mp_comm->m_pBootLoader->GetWorkState()) //разрешаем чекбокс только если мы в оффлайне сразу после онлайна
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
 if (mp_comm->m_pBootLoader->GetWorkState())
  state = CStatusBarManager::STATE_BOOTLOADER;

 mp_sbar->SetConnectionState(state);
}

/////////////////////////////////////////////////////////////////////////////////
void CFirmwareTabController::OnUpdateUI(IBLDEventHandler::poolUpdateUI* ip_data)
{
 ASSERT(ip_data);
 IBLDEventHandler::poolUpdateUI data;

 /////////////////////////////////////////////////////////////
 //эксклюзивный доступ
 mp_comm->m_pBootLoader->EnterCriticalSection();
 data = *ip_data;
 mp_comm->m_pBootLoader->LeaveCriticalSection();
 /////////////////////////////////////////////////////////////

 if (data.opcode!=CBootLoader::BL_OP_EXIT) //для операции выхода из бутлоадера не показываем никакого прогресс бара
 {
  mp_sbar->SetProgressRange(0, data.total);
  mp_sbar->SetProgressPos(data.current);
 }
}

void CFirmwareTabController::OnBegin(const int opcode,const int status)
{
 if (opcode == CBootLoader::BL_OP_READ_SIGNATURE)
  mp_sbar->SetInformationText(MLL::LoadString(IDS_FW_READING_SIGNATURE));
 if (opcode == CBootLoader::BL_OP_READ_EEPROM)
  mp_sbar->SetInformationText(MLL::LoadString(IDS_FW_READING_EEPROM));
 if (opcode == CBootLoader::BL_OP_WRITE_EEPROM)
  mp_sbar->SetInformationText(MLL::LoadString(IDS_FW_WRITING_EEPROM));
 if (opcode == CBootLoader::BL_OP_READ_FLASH)
  mp_sbar->SetInformationText(MLL::LoadString(IDS_FW_READING_FW));
 if (opcode == CBootLoader::BL_OP_WRITE_FLASH)
  mp_sbar->SetInformationText(MLL::LoadString(IDS_FW_WRITING_FW));
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
   VERIFY(mp_comm->SwitchOn(CCommunicationManager::OP_ACTIVATE_APPLICATION));
   break;
  }

  //////////////////////////////////////////////////////////////////////
  case CBootLoader::BL_OP_READ_SIGNATURE:
  {
   if (status==1)
   {
    mp_bl_data[CBootLoader::BL_SIGNATURE_STR_LEN] = 0;
    mp_sbar->SetInformationText(mp_bl_data);
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
  case CBootLoader::BL_OP_READ_EEPROM:
  {
   if (status==1)
   { //OK
    mp_sbar->SetInformationText(MLL::LoadString(IDS_FW_EEPROM_READ_SUCCESSFULLY));
    FirmwareFileUtils::SaveEEPROMToFile(mp_bl_data, m_epp.m_size);
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
    if (m_bl_read_flash_mode == MODE_RD_FLASH_TO_FILE)
    {
     FirmwareFileUtils::SaveFLASHToFile(mp_bl_data, m_fpp.m_total_size, mp_fwdm);
    }
    else if (m_bl_read_flash_mode == MODE_RD_FLASH_TO_BUFF_FOR_LOAD)
    {
     if (_CheckFirmwareCompatibilityAndAskUser(mp_bl_data) && _CheckQuartzCompatibilityAndAskUser(mp_bl_data))
     {
      if (FirmwareFileUtils::CheckFirmwareIntegrity(mp_bl_data, m_fpp.m_total_size))
       PrepareOnLoadFLASH(mp_bl_data, _T(""));
     }
    }
    else if (m_bl_read_flash_mode == MODE_RD_FLASH_FOR_IMPORT_DATA)
    {
     if (_CheckFirmwareCompatibilityAndAskUser(mp_bl_data) && _CheckQuartzCompatibilityAndAskUser(mp_bl_data))
     {
      mp_fwdm->LoadDataBytesFromAnotherFirmware(mp_bl_data);
      PrepareOnLoadFLASH(NULL, mp_fwdm->GetFWFileName());
     }
    }
    else if (m_bl_read_flash_mode == MODE_RD_FLASH_TO_BUFF_MERGE_DATA)
    {
     //wait for completion of previous operation
     while(!mp_comm->m_pBootLoader->IsIdle());

     //transfer old code data (logically, do not copy bits directly)
     size_t srcSize = (m_fpp.m_app_section_size - (m_code_for_merge_size - m_moreSize));
     mp_fwdm->LoadCodeData(mp_bl_data, srcSize, mp_code_for_merge_with_overhead);

     //transfer old data
     size_t dataSize = m_fpp.m_app_section_size - m_code_for_merge_size;
     BYTE* dataPtr = mp_code_for_merge_with_overhead + m_code_for_merge_size;
     memcpy(dataPtr, mp_bl_data + m_moreSize, dataSize);
     //Высчитываем и записываем контрольную сумму всей прошивки
     mp_fwdm->CalculateAndPlaceFirmwareCRC(mp_code_for_merge_with_overhead);

     Sleep(250);
     mp_sbar->SetProgressPos(0);
     mp_comm->m_pBootLoader->StartOperation(CBootLoader::BL_OP_WRITE_FLASH, mp_code_for_merge_with_overhead, m_fpp.m_app_section_size);

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
 }//switch

 //если бутлоадер был запущен аварийно, то активируем коммуникационный
 //контроллер приложения.  А зачем???  TODO.
 if (m_bl_started_emergency)
 {
  mp_view->EnableBLItems(true);
  mp_view->EnableBLStartedEmergency(true);

  VERIFY(mp_comm->SwitchOn(CCommunicationManager::OP_ACTIVATE_APPLICATION));
 }
}

/////////////////////////////////////////////////////////////////////////////////
//получение информации о бутлоадере
void CFirmwareTabController::OnBootLoaderInfo(void)
{
 if (!mp_comm->m_pBootLoader->IsIdle())
  return;

 //запускаем бутлоадер (если нужно)
 StartBootLoader();

 if (!m_bl_started_emergency)
 {
  mp_sbar->SetInformationText(MLL::LoadString(IDS_FW_WAITING_BOOTLOADER)); 
  mp_comm->m_pControlApp->SetPacketsTimer(WAITING_BL_START_TIMEOUT);
  m_waiting_bl_timer.SetTimer(WAITING_BL_START_TIMEOUT, true); //one shot
  m_blFinishOpCB = &CFirmwareTabController::finishOnBootLoaderInfo;
 }
 else
  finishOnBootLoaderInfo();
}

void CFirmwareTabController::OnReadEepromToFile(void)
{
 if (!mp_comm->m_pBootLoader->IsIdle())
  return;

 //запускаем бутлоадер по команде из приложения
 StartBootLoader();

 if (!m_bl_started_emergency)
 {
  mp_sbar->SetInformationText(MLL::LoadString(IDS_FW_WAITING_BOOTLOADER)); 
  mp_comm->m_pControlApp->SetPacketsTimer(WAITING_BL_START_TIMEOUT);
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
 bool result = FirmwareFileUtils::LoadEEPROMFromFile(mp_bl_data, sizes);

 if (!result)
  return; //cancel

 ASSERT(mp_comm);

 //запускаем бутлоадер по команде из приложения
 StartBootLoader();

 if (!m_bl_started_emergency)
 {  
  mp_sbar->SetInformationText(MLL::LoadString(IDS_FW_WAITING_BOOTLOADER)); 
  mp_comm->m_pControlApp->SetPacketsTimer(WAITING_BL_START_TIMEOUT);
  m_waiting_bl_timer.SetTimer(WAITING_BL_START_TIMEOUT, true); //one shot
  m_blFinishOpCB = &CFirmwareTabController::finishOnWriteEepromFromFile;
 }
 else
  finishOnWriteEepromFromFile();
}

void CFirmwareTabController::OnResetEeprom(void)
{
 if (IDYES==SECUMessageBox(MLL::GetString(IDS_RESET_EEPROM_COMFIRMATION).c_str(), MB_YESNO|MB_DEFBUTTON2|MB_ICONEXCLAMATION))
 {
  SECU3IO::OPCompNc packet_data;
  packet_data.opcode = SECU3IO::OPCODE_RESET_EEPROM;
  packet_data.opdata = 0xAA;
  mp_comm->m_pControlApp->SendPacket(OP_COMP_NC, &packet_data);
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
  mp_sbar->SetInformationText(MLL::LoadString(IDS_FW_WAITING_BOOTLOADER)); 
  mp_comm->m_pControlApp->SetPacketsTimer(WAITING_BL_START_TIMEOUT);
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

 if (!mp_fwdm->CheckCompatibility(i_buff, p_pph ? &p_pph->GetFlashParameters() : NULL))
 {
  if (IDNO==SECUMessageBox(MLL::LoadString(IDS_INCOMPATIBLE_FIRMWARE), MB_YESNO | MB_ICONEXCLAMATION))
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
   if (IDNO==SECUMessageBox(MLL::LoadString(IDS_INCOMPATIBLE_QUARTZ), MB_YESNO | MB_ICONEXCLAMATION))
    return false; //aborted by user
  }
 }

 return true; //compatible or compatibility can't be checked
}

void CFirmwareTabController::OnWriteFlashFromFile(void)
{
 std::vector<int> sizes;
 sizes.push_back(m_fpp.m_total_size);
 bool result = FirmwareFileUtils::LoadFLASHFromFile(mp_bl_data, sizes);

 if (!result || !_CheckQuartzCompatibilityAndAskUser(mp_bl_data))
  return; //cancel

 StartWritingOfFLASHFromBuff();
}

void CFirmwareTabController::StartWritingOfFLASHFromBuff(void)
{
 //вычисляем контрольную сумму и сохраняем ее в массив с прошивкой. Это необходимо например когда
 //мы записываем свеже скомпилированную прошивку, которая может не содержать контрольной суммы
 mp_fwdm->CalculateAndPlaceFirmwareCRC(mp_bl_data);

 ASSERT(mp_comm);

 //запускаем бутлоадер по команде из приложения (если нет флага что он запущен вручную)
 StartBootLoader();

 if (!m_bl_started_emergency)
 {
  mp_sbar->SetInformationText(MLL::LoadString(IDS_FW_WAITING_BOOTLOADER)); 
  mp_comm->m_pControlApp->SetPacketsTimer(WAITING_BL_START_TIMEOUT);
  m_waiting_bl_timer.SetTimer(WAITING_BL_START_TIMEOUT, true); //one shot
  m_blFinishOpCB = &CFirmwareTabController::finishStartWritingOfFLASHFromBuff;
 }
 else
  finishStartWritingOfFLASHFromBuff();
}

//от чекбокса...
void CFirmwareTabController::OnBLStartedEmergency(void)
{
 if (mp_comm->m_pControlApp->GetOnlineStatus()==true)
  return;

 //если оффлайн, то состояние элементов меню зависит от состояния чекбокса
 bool emergency = mp_view->IsBLStartedEmergency();
 mp_view->EnableBLItems(emergency);

 //если он уже запущен, то при установленом чекбоксе в статусе будет всегда иконка бутлоадера
 if (emergency)
  mp_sbar->SetConnectionState(CStatusBarManager::STATE_BOOTLOADER);
 else
  mp_sbar->SetConnectionState(CStatusBarManager::STATE_OFFLINE);
}

bool CFirmwareTabController::IsBLStartedEmergency(void)
{
 //если чекбокс запрещен то всегда возвращаем отрицательный результат, а запрещен чекбокс
 //если мы в онлайне или запущен бутлоадер
 return mp_view->IsBLStartedEmergency() && mp_view->IsBLStartedEmergencyEnabled();
}

bool CFirmwareTabController::StartBootLoader(void)
{
 ASSERT(mp_comm);

 //до завершения операции надо пользоваться только этой переменной
 m_bl_started_emergency = IsBLStartedEmergency();

 //запускаем бутлоадер по команде из приложения (если нет флага что он запущен вручную)
 if (!m_bl_started_emergency)
 {  
  bool result = mp_comm->m_pControlApp->StartBootLoader();
  Sleep(DELAY_AFTER_BL_START); //обязательно нужно подождать не менее 50 мс, иначе будут вылазить посторонние символы при приеме данных от загрузчика
  return result;
 }

 return true; //бутлоадер запускать не надо было - он запущен аварийно
}

bool CFirmwareTabController::ExitBootLoader(void)
{
 ASSERT(mp_comm);

 if (!m_bl_started_emergency)
 {
  bool result = mp_comm->m_pBootLoader->StartOperation(CBootLoader::BL_OP_EXIT,NULL,0);
  return result;
 }

 //Выходить из бутлоадера не надо было - он запущен аварийно.
 //На будущее: Если бутлоадер запущен аварийно и закончилась операция записи прошивки (работающей прошивки :-)),
 //то из него можно выйти... Но это по желанию пользователя. А вообще удобства в этом - практичеки нет.
 return true;
}

bool CFirmwareTabController::CheckChangesAskAndSaveFirmware(void)
{
 bool modified = mp_fwdm->IsModified();
 if (modified && mp_settings->GetSaveWarning())
 {
  int result = SECUMessageBox(MLL::LoadString(IDS_FW_MODIFICATION_WARNING), MB_YESNOCANCEL);
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
 //save positions of windows of opened charts
 for(int i = TYPE_MAP_ALL_START; i <= TYPE_MAP_ALL_END; ++i)
  OnCloseMapWnd(mp_view->mp_TablesPanel->GetMapWindow(i), i);

 //warnings user about leaving of this tab
 if (m_active && (!mp_comm->m_pBootLoader->IsIdle() || m_waiting_bl_timer.isActive()))
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
 if (mp_comm->m_pBootLoader->IsIdle() && !m_waiting_bl_timer.isActive())
  return true; //allows
 return ErrorMsg::AskUserAboutTabLeaving();
}

void CFirmwareTabController::OnSaveSettings(void)
{
 //save positions of windows of opened charts
 for(int i = TYPE_MAP_ALL_START; i <= TYPE_MAP_ALL_END; ++i)
  OnCloseMapWnd(mp_view->mp_TablesPanel->GetMapWindow(i), i);
}

void CFirmwareTabController::PrepareOnLoadFLASH(const BYTE* i_buff, const _TSTRING& i_file_name)
{
 if (i_buff) //Do we need to load?
  mp_fwdm->LoadBytes(i_buff);
 if (i_file_name!=_T(""))
  mp_fwdm->SetFWFileName(_TSTRING(i_file_name));
 else
 {
  CString string;
  SYSTEMTIME time;
  GetLocalTime(&time);
  string.Format(_T("FW%02u%02u%02u-%02u%02u%04u.bin"),
    time.wHour,time.wMinute,time.wSecond,time.wDay,time.wMonth,time.wYear);
  mp_fwdm->SetFWFileName(_TSTRING(string));
 }

 DWORD opt = mp_fwdm->GetFWOptions();
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
 mp_view->mp_TablesPanel->EnableGrtsCurve(!CHECKBIT32(opt, SECU3IO::COPT_SECU3T)); 
 mp_view->mp_TablesPanel->EnableFtlsCurve(!CHECKBIT32(opt, SECU3IO::COPT_SECU3T)); 
 mp_view->mp_TablesPanel->EnableEgtsCurve(!CHECKBIT32(opt, SECU3IO::COPT_SECU3T)); 
 mp_view->mp_TablesPanel->EnableOpsCurve(!CHECKBIT32(opt, SECU3IO::COPT_SECU3T)); 
 mp_view->mp_TablesPanel->EnableManInjPwc(!CHECKBIT32(opt, SECU3IO::COPT_SECU3T) && CHECKBIT32(opt, SECU3IO::COPT_FUEL_INJECT)); 
 mp_view->mp_TablesPanel->EnableGrHeatDutyMap(!CHECKBIT32(opt, SECU3IO::COPT_SECU3T)); 
 mp_view->mp_TablesPanel->EnablePwmIacUCoefMap(CHECKBIT32(opt, SECU3IO::COPT_FUEL_INJECT) || (fnc.GD_CONTROL && CHECKBIT32(opt, SECU3IO::COPT_GD_CONTROL))); 
 mp_view->mp_TablesPanel->EnableAftstrStrkMap(CHECKBIT32(opt, SECU3IO::COPT_FUEL_INJECT) || (fnc.GD_CONTROL && CHECKBIT32(opt, SECU3IO::COPT_GD_CONTROL))); 
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
 this->mp_iorCntr->EnableSpiAdc(!CHECKBIT32(opt, SECU3IO::COPT_SECU3T) && CHECKBIT32(opt, SECU3IO::COPT_ATMEGA1284)); 
 this->mp_iorCntr->Enable(true);

 mp_view->mp_ParamDeskDlg->EnableCKPSItems(!CHECKBIT32(opt, SECU3IO::COPT_HALL_SYNC) && !CHECKBIT32(opt, SECU3IO::COPT_CKPS_NPLUS1));
 mp_view->mp_ParamDeskDlg->EnableHallWndWidth(CHECKBIT32(opt, SECU3IO::COPT_HALL_SYNC) || CHECKBIT32(opt, SECU3IO::COPT_CKPS_NPLUS1));
 mp_view->mp_ParamDeskDlg->EnableInputsMerging(!CHECKBIT32(opt, SECU3IO::COPT_CKPS_2CHIGN));
 mp_view->mp_ParamDeskDlg->EnableRisingSpark(CHECKBIT32(opt, SECU3IO::COPT_DWELL_CONTROL) && !CHECKBIT32(opt, SECU3IO::COPT_CKPS_2CHIGN));
 mp_view->mp_ParamDeskDlg->EnableUseCamRef(CHECKBIT32(opt, SECU3IO::COPT_PHASE_SENSOR));
 mp_view->mp_ParamDeskDlg->EnableCogsBTDC(!CHECKBIT32(opt, SECU3IO::COPT_ODDFIRE_ALGO));
 mp_view->mp_ParamDeskDlg->EnableFuelInjection(CHECKBIT32(opt, SECU3IO::COPT_FUEL_INJECT));
 mp_view->mp_ParamDeskDlg->EnableLambda(CHECKBIT32(opt, SECU3IO::COPT_FUEL_INJECT) || CHECKBIT32(opt, SECU3IO::COPT_CARB_AFR) || (fnc.GD_CONTROL && CHECKBIT32(opt, SECU3IO::COPT_GD_CONTROL)));
 mp_view->mp_ParamDeskDlg->EnableGasdose(fnc.GD_CONTROL && CHECKBIT32(opt, SECU3IO::COPT_GD_CONTROL)); //GD
 mp_view->mp_ParamDeskDlg->EnableChoke(fnc.SM_CONTROL && CHECKBIT32(opt, SECU3IO::COPT_SM_CONTROL));

 mp_view->mp_ParamDeskDlg->EnableChokeCtrls(!CHECKBIT32(opt, SECU3IO::COPT_FUEL_INJECT));

 mp_view->mp_TablesPanel->SetSplitAngMode(CHECKBIT32(opt, SECU3IO::COPT_SPLIT_ANGLE)); 

 SetViewFirmwareValues();
}

void CFirmwareTabController::OnOpenFlashFromFile(void)
{
 bool result;
 std::vector<BYTE> buff_container(m_fpp.m_total_size, 0);
 BYTE *buff = &buff_container[0];
 _TSTRING opened_file_name = _T("");
 
 if (!mp_comm->m_pBootLoader->IsIdle())
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

 mp_fwdm->StoreBytes(buff);

 //в случае подтверждения пользователя, также будует
 //вычислена контрольная сумма и сохранена в массив с прошивкой
 bool result = FirmwareFileUtils::SaveFLASHToFile(buff, m_fpp.m_total_size, mp_fwdm, &opened_file_name,true);
 if (result)
 {
  //контрольная сумма была сохранена только вмассив с прошивкий которая сохранялась,
  //так как сохранение было подтверждено, то теперь можно обновить и массив с активной прошивкой
  mp_fwdm->CalculateAndPlaceFirmwareCRC();

  //данные были успешно сохранены - можно сбрасывать признак модификации
  mp_fwdm->ResetModified();

  //после сохранения "Save As" обновляем имя открытого файла
  mp_fwdm->SetFWFileName(_TSTRING(opened_file_name));
  mp_view->SetFirmwareName(mp_fwdm->GetFWFileName());

  //устанавливаем значения только в графики
  SetViewChartsValues();
  mp_view->mp_TablesPanel->TransformValues(); //transform values in some maps before they will be rendered for user
  mp_view->mp_TablesPanel->UpdateOpenedCharts();

  mp_view->SetFirmwareCRCs(mp_fwdm->GetCRC16StoredInActiveFirmware(),mp_fwdm->CalculateCRC16OfActiveFirmware());
 }
}

void CFirmwareTabController::OnFWInformationTextChanged(void)
{
 CString string = mp_view->GetFWInformationText();
 mp_fwdm->SetSignatureInfo(_TSTRING(string));
}

bool CFirmwareTabController::IsFirmwareOpened()
{
 return mp_fwdm->IsLoaded();
}

//эта функция не обновляет графики, нужно еще вызывать UpdateOpenedCharts()!
void CFirmwareTabController::SetViewChartsValues(void)
{
 mp_fwdm->GetAttenuatorMap(mp_view->mp_TablesPanel->GetAttenuatorMap(false),false);
 mp_fwdm->GetAttenuatorMap(mp_view->mp_TablesPanel->GetAttenuatorMap(true),true);

 mp_fwdm->GetDwellCntrlMap(mp_view->mp_TablesPanel->GetDwellCntrlMap(false),false);
 mp_fwdm->GetDwellCntrlMap(mp_view->mp_TablesPanel->GetDwellCntrlMap(true),true);

 mp_fwdm->GetCTSCurveMap(mp_view->mp_TablesPanel->GetCTSCurveMap(false),false);
 mp_fwdm->GetCTSCurveMap(mp_view->mp_TablesPanel->GetCTSCurveMap(true),true);
 mp_view->mp_TablesPanel->SetCTSXAxisEdits(mp_fwdm->GetCTSMapVoltageLimit(0), mp_fwdm->GetCTSMapVoltageLimit(1));

 mp_fwdm->GetATSCurveMap(mp_view->mp_TablesPanel->GetATSCurveMap(false),false);
 mp_fwdm->GetATSCurveMap(mp_view->mp_TablesPanel->GetATSCurveMap(true),true);
 mp_view->mp_TablesPanel->SetATSXAxisEdits(mp_fwdm->GetATSMapVoltageLimit(0), mp_fwdm->GetATSMapVoltageLimit(1));

 mp_fwdm->GetATSAACMap(mp_view->mp_TablesPanel->GetATSAACMap(false),false);
 mp_fwdm->GetATSAACMap(mp_view->mp_TablesPanel->GetATSAACMap(true),true);

 mp_fwdm->GetGasdosePosMap(mp_view->mp_TablesPanel->GetGasdosePosMap(false),false);
 mp_fwdm->GetGasdosePosMap(mp_view->mp_TablesPanel->GetGasdosePosMap(true),true);

 mp_fwdm->GetBarocorrMap(mp_view->mp_TablesPanel->GetBarocorrMap(false),false);
 mp_fwdm->GetBarocorrMap(mp_view->mp_TablesPanel->GetBarocorrMap(true),true);

 mp_fwdm->GetManIgntimMap(mp_view->mp_TablesPanel->GetManIgntimMap(false),false);
 mp_fwdm->GetManIgntimMap(mp_view->mp_TablesPanel->GetManIgntimMap(true),true);

 mp_fwdm->GetTmp2CurveMap(mp_view->mp_TablesPanel->GetTmp2CurveMap(false),false);
 mp_fwdm->GetTmp2CurveMap(mp_view->mp_TablesPanel->GetTmp2CurveMap(true),true);

 mp_fwdm->GetCrkTempMap(mp_view->mp_TablesPanel->GetCrkTempMap(false),false);
 mp_fwdm->GetCrkTempMap(mp_view->mp_TablesPanel->GetCrkTempMap(true),true);

 mp_fwdm->GetEHPauseMap(mp_view->mp_TablesPanel->GetEHPauseMap(false),false);
 mp_fwdm->GetEHPauseMap(mp_view->mp_TablesPanel->GetEHPauseMap(true),true);

 mp_fwdm->GetCrankingThrdMap(mp_view->mp_TablesPanel->GetCrankingThrdMap(false),false);
 mp_fwdm->GetCrankingThrdMap(mp_view->mp_TablesPanel->GetCrankingThrdMap(true),true);

 mp_fwdm->GetCrankingTimeMap(mp_view->mp_TablesPanel->GetCrankingTimeMap(false),false);
 mp_fwdm->GetCrankingTimeMap(mp_view->mp_TablesPanel->GetCrankingTimeMap(true),true);

 mp_fwdm->GetSmapabanThrdMap(mp_view->mp_TablesPanel->GetSmapabanThrdMap(false),false);
 mp_fwdm->GetSmapabanThrdMap(mp_view->mp_TablesPanel->GetSmapabanThrdMap(true),true);

 mp_fwdm->GetKnockZoneMap(mp_view->mp_TablesPanel->GetKnockZoneMap(false),false);
 mp_fwdm->GetKnockZoneMap(mp_view->mp_TablesPanel->GetKnockZoneMap(true),true);

 mp_fwdm->GetGrtsCurveMap(mp_view->mp_TablesPanel->GetGrtsCurveMap(false),false);
 mp_fwdm->GetGrtsCurveMap(mp_view->mp_TablesPanel->GetGrtsCurveMap(true),true);

 mp_fwdm->GetGrHeatDutyMap(mp_view->mp_TablesPanel->GetGrHeatDutyMap(false),false);
 mp_fwdm->GetGrHeatDutyMap(mp_view->mp_TablesPanel->GetGrHeatDutyMap(true),true);

 mp_fwdm->GetPwmIacUCoefMap(mp_view->mp_TablesPanel->GetPwmIacUCoefMap(false),false);
 mp_fwdm->GetPwmIacUCoefMap(mp_view->mp_TablesPanel->GetPwmIacUCoefMap(true),true);
 
 mp_fwdm->GetAftstrStrk0Map(mp_view->mp_TablesPanel->GetAftstrStrk0Map(false),false);
 mp_fwdm->GetAftstrStrk0Map(mp_view->mp_TablesPanel->GetAftstrStrk0Map(true),true);

 mp_fwdm->GetAftstrStrk1Map(mp_view->mp_TablesPanel->GetAftstrStrk1Map(false),false);
 mp_fwdm->GetAftstrStrk1Map(mp_view->mp_TablesPanel->GetAftstrStrk1Map(true),true);

 mp_fwdm->GetGrValDelMap(mp_view->mp_TablesPanel->GetGrValDelMap(false),false);
 mp_fwdm->GetGrValDelMap(mp_view->mp_TablesPanel->GetGrValDelMap(true),true);

 mp_fwdm->GetFtlsCurveMap(mp_view->mp_TablesPanel->GetFtlsCurveMap(false),false);
 mp_fwdm->GetFtlsCurveMap(mp_view->mp_TablesPanel->GetFtlsCurveMap(true),true);

 mp_fwdm->GetEgtsCurveMap(mp_view->mp_TablesPanel->GetEgtsCurveMap(false),false);
 mp_fwdm->GetEgtsCurveMap(mp_view->mp_TablesPanel->GetEgtsCurveMap(true),true);

 mp_fwdm->GetOpsCurveMap(mp_view->mp_TablesPanel->GetOpsCurveMap(false),false);
 mp_fwdm->GetOpsCurveMap(mp_view->mp_TablesPanel->GetOpsCurveMap(true),true);

 mp_fwdm->GetManInjPwcMap(mp_view->mp_TablesPanel->GetManInjPwcMap(false),false);
 mp_fwdm->GetManInjPwcMap(mp_view->mp_TablesPanel->GetManInjPwcMap(true),true);

 mp_fwdm->GetMAFCurveMap(mp_view->mp_TablesPanel->GetMAFCurveMap(false),false);
 mp_fwdm->GetMAFCurveMap(mp_view->mp_TablesPanel->GetMAFCurveMap(true),true);

 mp_fwdm->GetRPMGridMap(mp_view->mp_TablesPanel->GetRPMGrid());
 mp_fwdm->GetCLTGridMap(mp_view->mp_TablesPanel->GetCLTGrid());
 mp_fwdm->GetLoadGridMap(mp_view->mp_TablesPanel->GetLoadGrid());

 //apply load axis's grid settings for all related maps
 SECU3IO::FunSetPar params;
 mp_fwdm->GetDefParamValues(FUNSET_PAR, &params);
 mp_view->mp_TablesPanel->SetLoadAxisCfg(params.map_lower_pressure, params.map_upper_pressure, 0, params.use_load_grid, true);

 if (m_current_funset_index==-1)
  return;
 mp_fwdm->GetStartMap(m_current_funset_index,mp_view->mp_TablesPanel->GetStartMap(false),false);
 mp_fwdm->GetStartMap(m_current_funset_index,mp_view->mp_TablesPanel->GetStartMap(true),true);

 mp_fwdm->GetIdleMap(m_current_funset_index,mp_view->mp_TablesPanel->GetIdleMap(false),false);
 mp_fwdm->GetIdleMap(m_current_funset_index,mp_view->mp_TablesPanel->GetIdleMap(true),true);

 mp_fwdm->GetWorkMap(m_current_funset_index,mp_view->mp_TablesPanel->GetWorkMap(false),false);
 mp_fwdm->GetWorkMap(m_current_funset_index,mp_view->mp_TablesPanel->GetWorkMap(true),true);

 mp_fwdm->GetTempMap(m_current_funset_index,mp_view->mp_TablesPanel->GetTempMap(false),false);
 mp_fwdm->GetTempMap(m_current_funset_index,mp_view->mp_TablesPanel->GetTempMap(true),true);

 mp_fwdm->GetTempIdlMap(m_current_funset_index,mp_view->mp_TablesPanel->GetTempIdlMap(false),false);
 mp_fwdm->GetTempIdlMap(m_current_funset_index,mp_view->mp_TablesPanel->GetTempIdlMap(true),true);

 //fuel injection
 mp_fwdm->GetVEMap(m_current_funset_index,mp_view->mp_TablesPanel->GetVEMap(false),false);
 mp_fwdm->GetVEMap(m_current_funset_index,mp_view->mp_TablesPanel->GetVEMap(true),true);

 mp_fwdm->GetVE2Map(m_current_funset_index,mp_view->mp_TablesPanel->GetVE2Map(false),false);
 mp_fwdm->GetVE2Map(m_current_funset_index,mp_view->mp_TablesPanel->GetVE2Map(true),true);

 mp_fwdm->GetAFRMap(m_current_funset_index,mp_view->mp_TablesPanel->GetAFRMap(false),false);
 mp_fwdm->GetAFRMap(m_current_funset_index,mp_view->mp_TablesPanel->GetAFRMap(true),true);

 mp_fwdm->GetCrnkMap(m_current_funset_index,mp_view->mp_TablesPanel->GetCrnkMap(false),false);
 mp_fwdm->GetCrnkMap(m_current_funset_index,mp_view->mp_TablesPanel->GetCrnkMap(true),true);

 mp_fwdm->GetWrmpMap(m_current_funset_index,mp_view->mp_TablesPanel->GetWrmpMap(false),false);
 mp_fwdm->GetWrmpMap(m_current_funset_index,mp_view->mp_TablesPanel->GetWrmpMap(true),true);

 mp_fwdm->GetDeadMap(m_current_funset_index,mp_view->mp_TablesPanel->GetDeadMap(false),false);
 mp_fwdm->GetDeadMap(m_current_funset_index,mp_view->mp_TablesPanel->GetDeadMap(true),true);

 mp_fwdm->GetIdlrMap(m_current_funset_index,mp_view->mp_TablesPanel->GetIdlrMap(false),false);
 mp_fwdm->GetIdlrMap(m_current_funset_index,mp_view->mp_TablesPanel->GetIdlrMap(true),true);

 mp_fwdm->GetIdlcMap(m_current_funset_index,mp_view->mp_TablesPanel->GetIdlcMap(false),false);
 mp_fwdm->GetIdlcMap(m_current_funset_index,mp_view->mp_TablesPanel->GetIdlcMap(true),true);

 mp_fwdm->GetAETPSMap(m_current_funset_index,mp_view->mp_TablesPanel->GetAETPSMap(false),false);
 mp_fwdm->GetAETPSMap(m_current_funset_index,mp_view->mp_TablesPanel->GetAETPSMap(true),true);

 mp_fwdm->GetAERPMMap(m_current_funset_index,mp_view->mp_TablesPanel->GetAERPMMap(false),false);
 mp_fwdm->GetAERPMMap(m_current_funset_index,mp_view->mp_TablesPanel->GetAERPMMap(true),true);

 mp_fwdm->GetAftstrMap(m_current_funset_index,mp_view->mp_TablesPanel->GetAftstrMap(false),false);
 mp_fwdm->GetAftstrMap(m_current_funset_index,mp_view->mp_TablesPanel->GetAftstrMap(true),true);

 mp_fwdm->GetITMap(m_current_funset_index,mp_view->mp_TablesPanel->GetITMap(false),false);
 mp_fwdm->GetITMap(m_current_funset_index,mp_view->mp_TablesPanel->GetITMap(true),true);

 mp_fwdm->GetITRPMMap(m_current_funset_index,mp_view->mp_TablesPanel->GetITRPMMap(false),false);
 mp_fwdm->GetITRPMMap(m_current_funset_index,mp_view->mp_TablesPanel->GetITRPMMap(true),true);

 mp_fwdm->GetRigidMap(m_current_funset_index,mp_view->mp_TablesPanel->GetRigidMap(false),false);
 mp_fwdm->GetRigidMap(m_current_funset_index,mp_view->mp_TablesPanel->GetRigidMap(true),true);

 mp_fwdm->GetEGOCurveMap(m_current_funset_index,mp_view->mp_TablesPanel->GetEGOCurveMap(false),false);
 mp_fwdm->GetEGOCurveMap(m_current_funset_index,mp_view->mp_TablesPanel->GetEGOCurveMap(true),true);

 mp_fwdm->GetIACCorrMap(m_current_funset_index,mp_view->mp_TablesPanel->GetIACCMap(false),false);
 mp_fwdm->GetIACCorrMap(m_current_funset_index,mp_view->mp_TablesPanel->GetIACCMap(true),true);

 mp_fwdm->GetIACCorrWMap(m_current_funset_index,mp_view->mp_TablesPanel->GetIACCWMap(false),false);
 mp_fwdm->GetIACCorrWMap(m_current_funset_index,mp_view->mp_TablesPanel->GetIACCWMap(true),true);

 mp_fwdm->GetIATCLTMap(m_current_funset_index,mp_view->mp_TablesPanel->GetIATCLTMap(false),false);
 mp_fwdm->GetIATCLTMap(m_current_funset_index,mp_view->mp_TablesPanel->GetIATCLTMap(true),true);

 mp_fwdm->GetTpsswtMap(m_current_funset_index,mp_view->mp_TablesPanel->GetTpsswtMap(false),false);
 mp_fwdm->GetTpsswtMap(m_current_funset_index,mp_view->mp_TablesPanel->GetTpsswtMap(true),true);

 mp_fwdm->GetGtscMap(m_current_funset_index,mp_view->mp_TablesPanel->GetGtscMap(false),false);
 mp_fwdm->GetGtscMap(m_current_funset_index,mp_view->mp_TablesPanel->GetGtscMap(true),true);

 mp_fwdm->GetGpscMap(m_current_funset_index,mp_view->mp_TablesPanel->GetGpscMap(false),false);
 mp_fwdm->GetGpscMap(m_current_funset_index,mp_view->mp_TablesPanel->GetGpscMap(true),true);

 mp_fwdm->GetAtscMap(m_current_funset_index,mp_view->mp_TablesPanel->GetAtscMap(false),false);
 mp_fwdm->GetAtscMap(m_current_funset_index,mp_view->mp_TablesPanel->GetAtscMap(true),true);

 mp_fwdm->GetPwm1Map(m_current_funset_index,mp_view->mp_TablesPanel->GetPwm1Map(false),false);
 mp_fwdm->GetPwm1Map(m_current_funset_index,mp_view->mp_TablesPanel->GetPwm1Map(true),true);

 mp_fwdm->GetPwm2Map(m_current_funset_index,mp_view->mp_TablesPanel->GetPwm2Map(false),false);
 mp_fwdm->GetPwm2Map(m_current_funset_index,mp_view->mp_TablesPanel->GetPwm2Map(true),true);

 mp_fwdm->GetIACMATMap(m_current_funset_index,mp_view->mp_TablesPanel->GetIACMATMap(false),false);
 mp_fwdm->GetIACMATMap(m_current_funset_index,mp_view->mp_TablesPanel->GetIACMATMap(true),true);
}

void CFirmwareTabController::SetViewFirmwareValues(void)
{
 if (mp_fwdm->IsLoaded()==false)
  return;

 CString string = mp_fwdm->GetSignatureInfo().c_str();
 mp_view->SetFWInformationText(string);

 SetViewChartsValues();

 std::vector<_TSTRING> funset_names = mp_fwdm->GetFunctionsSetNames();
 mp_view->mp_TablesPanel->SetFunSetListBox(funset_names);

 mp_view->mp_TablesPanel->TransformValues(); //transform values in some maps before they will be rendered for user
 mp_view->mp_TablesPanel->UpdateOpenedCharts();

 //если было выделение в списке, то восстанавлваем его
 mp_view->mp_TablesPanel->SetFunSetListBoxSelection(m_current_funset_index);

 mp_view->SetFirmwareName(mp_fwdm->GetFWFileName());

 mp_view->SetFirmwareCRCs(mp_fwdm->GetCRC16StoredInActiveFirmware(),mp_fwdm->CalculateCRC16OfActiveFirmware());

 funset_names.push_back(_T("eeprom_set1")); //add stub name so that list contain 5 sets
 mp_view->mp_ParamDeskDlg->SetFunctionsNames(funset_names);
 BYTE descriptor = mp_view->mp_ParamDeskDlg->GetCurrentDescriptor();
 BYTE paramdata[256];
 mp_fwdm->GetDefParamValues(descriptor,paramdata);
 mp_view->mp_ParamDeskDlg->SetValues(descriptor,paramdata);

 //Attach fwdm to children controllers
 this->mp_iorCntr->AttachFWDM(mp_fwdm);
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
   mp_fwdm->SetStartMap(m_current_funset_index, mp_view->mp_TablesPanel->GetStartMap(false));
   break;
  case TYPE_MAP_DA_IDLE:
   ASSERT(m_current_funset_index!=-1);
   mp_fwdm->SetIdleMap(m_current_funset_index, mp_view->mp_TablesPanel->GetIdleMap(false));
   break;
  case TYPE_MAP_DA_WORK:
   ASSERT(m_current_funset_index!=-1);
   mp_fwdm->SetWorkMap(m_current_funset_index, mp_view->mp_TablesPanel->GetWorkMap(false));
   break;
  case TYPE_MAP_DA_TEMP_CORR:
   ASSERT(m_current_funset_index!=-1);
   mp_fwdm->SetTempMap(m_current_funset_index, mp_view->mp_TablesPanel->GetTempMap(false));
   break;
  case TYPE_MAP_DA_TEMPI_CORR:
   ASSERT(m_current_funset_index!=-1);
   mp_fwdm->SetTempIdlMap(m_current_funset_index, mp_view->mp_TablesPanel->GetTempIdlMap(false));
   break;
   //fuel injection maps
  case TYPE_MAP_INJ_VE:
   ASSERT(m_current_funset_index!=-1);
   mp_fwdm->SetVEMap(m_current_funset_index, mp_view->mp_TablesPanel->GetVEMap(false));
   break;
  case TYPE_MAP_INJ_VE2:
   ASSERT(m_current_funset_index!=-1);
   mp_fwdm->SetVE2Map(m_current_funset_index, mp_view->mp_TablesPanel->GetVE2Map(false));
   break;
  case TYPE_MAP_INJ_AFR:
   ASSERT(m_current_funset_index!=-1);
   mp_fwdm->SetAFRMap(m_current_funset_index, mp_view->mp_TablesPanel->GetAFRMap(false));
   break;
  case TYPE_MAP_INJ_CRNK:
   ASSERT(m_current_funset_index!=-1);
   mp_fwdm->SetCrnkMap(m_current_funset_index, mp_view->mp_TablesPanel->GetCrnkMap(false));
   break;
  case TYPE_MAP_INJ_WRMP:
   ASSERT(m_current_funset_index!=-1);
   mp_fwdm->SetWrmpMap(m_current_funset_index, mp_view->mp_TablesPanel->GetWrmpMap(false));
   break;
  case TYPE_MAP_INJ_DEAD:
   ASSERT(m_current_funset_index!=-1);
   mp_fwdm->SetDeadMap(m_current_funset_index, mp_view->mp_TablesPanel->GetDeadMap(false));
   break;
  case TYPE_MAP_INJ_IDLR:
   ASSERT(m_current_funset_index!=-1);
   mp_fwdm->SetIdlrMap(m_current_funset_index, mp_view->mp_TablesPanel->GetIdlrMap(false));
   break;
  case TYPE_MAP_INJ_IDLC:
   ASSERT(m_current_funset_index!=-1);
   mp_fwdm->SetIdlcMap(m_current_funset_index, mp_view->mp_TablesPanel->GetIdlcMap(false));
   break;
  case TYPE_MAP_INJ_AETPS:
   ASSERT(m_current_funset_index!=-1);
   mp_fwdm->SetAETPSMap(m_current_funset_index, mp_view->mp_TablesPanel->GetAETPSMap(false));
   break;
  case TYPE_MAP_INJ_AERPM:
   ASSERT(m_current_funset_index!=-1);
   mp_fwdm->SetAERPMMap(m_current_funset_index, mp_view->mp_TablesPanel->GetAERPMMap(false));
   break;
  case TYPE_MAP_INJ_AFTSTR:
   ASSERT(m_current_funset_index!=-1);
   mp_fwdm->SetAftstrMap(m_current_funset_index, mp_view->mp_TablesPanel->GetAftstrMap(false));
   break;
  case TYPE_MAP_INJ_IT:
   ASSERT(m_current_funset_index!=-1);
   mp_fwdm->SetITMap(m_current_funset_index, mp_view->mp_TablesPanel->GetITMap(false));
   break;
  case TYPE_MAP_INJ_ITRPM:
   ASSERT(m_current_funset_index!=-1);
   mp_fwdm->SetITRPMMap(m_current_funset_index, mp_view->mp_TablesPanel->GetITRPMMap(false));
   break;
  case TYPE_MAP_INJ_RIGID:
   ASSERT(m_current_funset_index!=-1);
   mp_fwdm->SetRigidMap(m_current_funset_index, mp_view->mp_TablesPanel->GetRigidMap(false));
   break;
  case TYPE_MAP_INJ_EGOCRV:
   ASSERT(m_current_funset_index!=-1);
   mp_fwdm->SetEGOCurveMap(m_current_funset_index, mp_view->mp_TablesPanel->GetEGOCurveMap(false));
   break;
  case TYPE_MAP_INJ_IACC:
   ASSERT(m_current_funset_index!=-1);
   mp_fwdm->SetIACCorrMap(m_current_funset_index, mp_view->mp_TablesPanel->GetIACCMap(false));
   break;
  case TYPE_MAP_INJ_IACCW:
   ASSERT(m_current_funset_index!=-1);
   mp_fwdm->SetIACCorrWMap(m_current_funset_index, mp_view->mp_TablesPanel->GetIACCWMap(false));
   break;
  case TYPE_MAP_INJ_IATCLT:
   ASSERT(m_current_funset_index!=-1);
   mp_fwdm->SetIATCLTMap(m_current_funset_index, mp_view->mp_TablesPanel->GetIATCLTMap(false));
   break;
  case TYPE_MAP_INJ_TPSSWT:
   ASSERT(m_current_funset_index!=-1);
   mp_fwdm->SetTpsswtMap(m_current_funset_index, mp_view->mp_TablesPanel->GetTpsswtMap(false));
   break;
  case TYPE_MAP_INJ_GTSC:
   ASSERT(m_current_funset_index!=-1);
   mp_fwdm->SetGtscMap(m_current_funset_index, mp_view->mp_TablesPanel->GetGtscMap(false));
   break;
  case TYPE_MAP_INJ_GPSC:
   ASSERT(m_current_funset_index!=-1);
   mp_fwdm->SetGpscMap(m_current_funset_index, mp_view->mp_TablesPanel->GetGpscMap(false));
   break;
  case TYPE_MAP_INJ_ATSC:
   ASSERT(m_current_funset_index!=-1);
   mp_fwdm->SetAtscMap(m_current_funset_index, mp_view->mp_TablesPanel->GetAtscMap(false));
   break;
  case TYPE_MAP_PWM1:
   ASSERT(m_current_funset_index!=-1);
   mp_fwdm->SetPwm1Map(m_current_funset_index, mp_view->mp_TablesPanel->GetPwm1Map(false));
   break;
  case TYPE_MAP_PWM2:
   ASSERT(m_current_funset_index!=-1);
   mp_fwdm->SetPwm2Map(m_current_funset_index, mp_view->mp_TablesPanel->GetPwm2Map(false));
   break;
  case TYPE_MAP_INJ_IACMAT:
   ASSERT(m_current_funset_index!=-1);
   mp_fwdm->SetIACMATMap(m_current_funset_index, mp_view->mp_TablesPanel->GetIACMATMap(false));
   break;

   //separate maps
  case TYPE_MAP_ATTENUATOR:
   mp_fwdm->SetAttenuatorMap(mp_view->mp_TablesPanel->GetAttenuatorMap(false));
   break;
  case TYPE_MAP_DWELLCNTRL:
   mp_fwdm->SetDwellCntrlMap(mp_view->mp_TablesPanel->GetDwellCntrlMap(false));
   break;
  case TYPE_MAP_CTS_CURVE:
   mp_fwdm->SetCTSCurveMap(mp_view->mp_TablesPanel->GetCTSCurveMap(false));
   break;
  case TYPE_MAP_ATS_CURVE:
   mp_fwdm->SetATSCurveMap(mp_view->mp_TablesPanel->GetATSCurveMap(false));
   break;
  case TYPE_MAP_ATS_CORR:
   mp_fwdm->SetATSAACMap(mp_view->mp_TablesPanel->GetATSAACMap(false));
   break;
  case TYPE_MAP_GASDOSE:
   mp_fwdm->SetGasdosePosMap(mp_view->mp_TablesPanel->GetGasdosePosMap(false));
   break;
  case TYPE_MAP_BAROCORR:
   mp_fwdm->SetBarocorrMap(mp_view->mp_TablesPanel->GetBarocorrMap(false));
   break;
  case TYPE_MAP_MANIGNTIM:
   mp_fwdm->SetManIgntimMap(mp_view->mp_TablesPanel->GetManIgntimMap(false));
   break;
  case TYPE_MAP_TMP2_CURVE:
   mp_fwdm->SetTmp2CurveMap(mp_view->mp_TablesPanel->GetTmp2CurveMap(false));
   break;
  case TYPE_MAP_CRKCLT_CORR:
   mp_fwdm->SetCrkTempMap(mp_view->mp_TablesPanel->GetCrkTempMap(false));
   break;
  case TYPE_MAP_EH_PAUSE:
   mp_fwdm->SetEHPauseMap(mp_view->mp_TablesPanel->GetEHPauseMap(false));
   break;
  case TYPE_MAP_CRANKING_THRD:
   mp_fwdm->SetCrankingThrdMap(mp_view->mp_TablesPanel->GetCrankingThrdMap(false));
   break;
  case TYPE_MAP_CRANKING_TIME:
   mp_fwdm->SetCrankingTimeMap(mp_view->mp_TablesPanel->GetCrankingTimeMap(false));
   break;
  case TYPE_MAP_SMAPABAN_THRD:
   mp_fwdm->SetSmapabanThrdMap(mp_view->mp_TablesPanel->GetSmapabanThrdMap(false));
   break;
  case TYPE_MAP_KNOCK_ZONE:
   mp_fwdm->SetKnockZoneMap(mp_view->mp_TablesPanel->GetKnockZoneMap(false));
   break;
  case TYPE_MAP_GRTS_CURVE:
   mp_fwdm->SetGrtsCurveMap(mp_view->mp_TablesPanel->GetGrtsCurveMap(false));
   break;
  case TYPE_MAP_FTLS_CURVE:
   mp_fwdm->SetFtlsCurveMap(mp_view->mp_TablesPanel->GetFtlsCurveMap(false));
   break;
  case TYPE_MAP_EGTS_CURVE:
   mp_fwdm->SetEgtsCurveMap(mp_view->mp_TablesPanel->GetEgtsCurveMap(false));
   break;
  case TYPE_MAP_OPS_CURVE:
   mp_fwdm->SetOpsCurveMap(mp_view->mp_TablesPanel->GetOpsCurveMap(false));
   break;
  case TYPE_MAP_GRHEAT_DUTY:
   mp_fwdm->SetGrHeatDutyMap(mp_view->mp_TablesPanel->GetGrHeatDutyMap(false));
   break;
  case TYPE_MAP_PWMIAC_UCOEF:
   mp_fwdm->SetPwmIacUCoefMap(mp_view->mp_TablesPanel->GetPwmIacUCoefMap(false));
   break;
  case TYPE_MAP_AFTSTR_STRK0:
   mp_fwdm->SetAftstrStrk0Map(mp_view->mp_TablesPanel->GetAftstrStrk0Map(false));
   break;
  case TYPE_MAP_AFTSTR_STRK1:
   mp_fwdm->SetAftstrStrk1Map(mp_view->mp_TablesPanel->GetAftstrStrk1Map(false));
   break;
  case TYPE_MAP_GRVDELAY:
   mp_fwdm->SetGrValDelMap(mp_view->mp_TablesPanel->GetGrValDelMap(false));
   break;
  case TYPE_MAP_MANINJPWC:
   mp_fwdm->SetManInjPwcMap(mp_view->mp_TablesPanel->GetManInjPwcMap(false));
   break;
  case TYPE_MAP_MAF_CURVE:
   mp_fwdm->SetMAFCurveMap(mp_view->mp_TablesPanel->GetMAFCurveMap(false));
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
 mp_fwdm->SetFunctionsSetName(i_index_of_item,_TSTRING(i_new_name));
}

void CFirmwareTabController::OnCTSXAxisEditChanged(int i_type, float i_value)
{
 mp_fwdm->SetCTSMapVoltageLimit(i_type, i_value);
}

void CFirmwareTabController::OnATSXAxisEditChanged(int i_type, float i_value)
{
 mp_fwdm->SetATSMapVoltageLimit(i_type, i_value);
}

void CFirmwareTabController::OnModificationCheckTimer(void)
{
 bool modified = mp_fwdm->IsModified();
 mp_view->SetModified(modified);
 if (mp_fwdm->IsLoaded())
  mp_view->SetFirmwareCRCs(mp_fwdm->GetCRC16StoredInActiveFirmware(),mp_fwdm->CalculateCRC16OfActiveFirmware());
}

void CFirmwareTabController::OnReadFlashFromSECU(void)
{
 if (!mp_comm->m_pBootLoader->IsIdle())
  return;
 //I don't like "copy/paste" paradigm of programming...
 m_bl_read_flash_mode = MODE_RD_FLASH_TO_BUFF_FOR_LOAD;
 _OnReadFlashToFile();
}

void CFirmwareTabController::OnWriteFlashToSECU(void)
{
 if (!mp_comm->m_pBootLoader->IsIdle())
  return;
 mp_fwdm->StoreBytes(mp_bl_data);
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
  mp_fwdm->LoadDataBytesFromAnotherFirmware(buff, &params.GetFlashParameters());
  PrepareOnLoadFLASH(NULL, mp_fwdm->GetFWFileName());
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
 mp_fwdm->GetDefParamValues(descriptor,paramdata);
 mp_view->mp_ParamDeskDlg->SetValues(descriptor,paramdata);
}

//from ParamDesk
void CFirmwareTabController::OnParamDeskChangeInTab(void)
{
 BYTE descriptor = mp_view->mp_ParamDeskDlg->GetCurrentDescriptor();
 BYTE paramdata[256];
 mp_view->mp_ParamDeskDlg->GetValues(descriptor, paramdata);
 mp_fwdm->SetDefParamValues(descriptor, paramdata);
 //update load axis's grids in all related maps
 if (descriptor==FUNSET_PAR)
 {
  SECU3IO::FunSetPar &params = (SECU3IO::FunSetPar&)paramdata;
  mp_view->mp_TablesPanel->SetLoadAxisCfg(params.map_lower_pressure, params.map_upper_pressure, 0, params.use_load_grid, true); //force update
 }
}

void CFirmwareTabController::OnImportMapsFromMPSZ(void)
{
 FWMapsDataHolder data;
 MPSZImportController import(&data);
 mp_fwdm->GetMapsData(&data);
 int result = import.DoImport();
 if (result == IDOK)
 {
  mp_fwdm->SetMapsData(&data);
  SetViewFirmwareValues();
 }
}

void CFirmwareTabController::OnImportMapsFromSECU3(void)
{
 FWMapsDataHolder data;
 SECU3ImportController import(&data);
 import.setFileReader(&FirmwareFileUtils::LoadFLASHFromFile);
 mp_fwdm->GetMapsData(&data);
 int result = import.DoImport();
 if (result == IDOK)
 {
  mp_fwdm->SetMapsData(&data);
  SetViewFirmwareValues();
 }
}

void CFirmwareTabController::OnImportMapsFromS3F(void)
{
 FWMapsDataHolder data;
 S3FImportController import(&data);
 mp_fwdm->GetMapsData(&data);
 int result = import.DoImport();
 if (result == IDOK)
 {
  mp_fwdm->SetMapsData(&data);
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
 if (!mp_edm->VerifyParamsCheckSum(eeprom))
 {
  if (IDCANCEL==SECUMessageBox(IDS_FW_EEPROM_DEF_PARAMS_CRC_INVALID, MB_OKCANCEL))
   return; //user canceled
 }

 mp_fwdm->LoadDefParametersFromBuffer(eeprom + mp_edm->GetParamsStartAddr(), ErrorMsg::AskUserAboutVrefCompensation);
 SetViewFirmwareValues(); //Update!
}

void CFirmwareTabController::OnImportTablesFromEEPROMFile()
{
 FWMapsDataHolder data;
 EEPROMImportController import(&data);
 import.setFileReader(&FirmwareFileUtils::LoadEEPROMFromFile);
 mp_fwdm->GetMapsData(&data);
 int result = import.DoImport();
 if (result == IDOK)
 {
  mp_fwdm->SetMapsData(&data);
  SetViewFirmwareValues();
 }
}

void CFirmwareTabController::OnExportMapsToMPSZ(void)
{
 FWMapsDataHolder data;
 MPSZExportController export_cntr(&data);
 mp_fwdm->GetMapsData(&data);
 export_cntr.DoExport();
}

void CFirmwareTabController::OnExportMapsToSECU3(void)
{
 FWMapsDataHolder data;
 SECU3ExportController export_cntr(&data);
 export_cntr.setFileReader(&FirmwareFileUtils::LoadFLASHFromFile);
 mp_fwdm->GetMapsData(&data);
 export_cntr.DoExport();
}

void CFirmwareTabController::OnExportMapsToS3F(void)
{
 FWMapsDataHolder data;
 S3FExportController export_cntr(&data);
 mp_fwdm->GetMapsData(&data);
 export_cntr.DoExport();
}

void CFirmwareTabController::OnEditRPMGrid(void)
{
 CFWRPMGridEditController cntr;
 cntr.AttachFWDM(mp_fwdm);
 if (IDOK == cntr.Edit())
 {
  mp_fwdm->GetRPMGridMap(mp_view->mp_TablesPanel->GetRPMGrid());
  mp_fwdm->GetCLTGridMap(mp_view->mp_TablesPanel->GetCLTGrid());
  mp_fwdm->GetLoadGridMap(mp_view->mp_TablesPanel->GetLoadGrid());

  //apply load axis's grid settings for all related maps
  SECU3IO::FunSetPar params;
  mp_fwdm->GetDefParamValues(FUNSET_PAR, &params);
  mp_view->mp_TablesPanel->SetLoadAxisCfg(params.map_lower_pressure, params.map_upper_pressure, 0, params.use_load_grid, true);

  mp_view->mp_TablesPanel->UpdateOpenedCharts();
 }
}

void CFirmwareTabController::OnEditFwConsts(void)
{
 CDynFieldsContainer dfd(mp_view, (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN) ? _T("Редактирование констант прошивки") : _T("Editing constants of firmware"), 408, true);

 SECU3IO::FwConstsData d;
 mp_fwdm->GetFwConstsData(d);

 //Sizes of averaging circular buffers
 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Размеры буферов усреднения входов:"));
 else
  dfd.AppendItem(_T("Sizes of inputs' averaging buffers:"));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Размер буфера усреднения оборотов"), _T(""), 1, 8, 1, 0, &d.inpavnum[0], _T("Размер буфера усреднения оборотов. Чем больше значение усреднения, тем больше сглаживание, но при этом увеличивается запаздывание."));
 else
  dfd.AppendItem(_T("Size of averaging buffer for RPM"), _T(""), 1, 8, 1, 0, &d.inpavnum[0], _T("Size of averaging buffer for RPM. The more averaging value, the greater smoothing, but at the same time the delay increases."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Размер буфера усреднения ДАД"), _T(""), 1, 8, 1, 0, &d.inpavnum[1], _T("Размер буфера усреднения ДАД. Чем больше значение усреднения, тем больше сглаживание, но при этом увеличивается запаздывание."));
 else
  dfd.AppendItem(_T("Size of averaging buffer for MAP"), _T(""), 1, 8, 1, 0, &d.inpavnum[1], _T("Size of averaging buffer for MAP. The more averaging value, the greater smoothing, but at the same time the delay increases."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Размер буфера усреднения напряжения бортовой сети"), _T(""), 1, 8, 1, 0, &d.inpavnum[2], _T("Размер буфера усреднения напряжения бортовой сети. Чем больше значение усреднения, тем больше сглаживание, но при этом увеличивается запаздывание."));
 else
  dfd.AppendItem(_T("Size of averaging buffer for board voltage"), _T(""), 1, 8, 1, 0, &d.inpavnum[2], _T("Size of averaging buffer for board voltage. The more averaging value, the greater smoothing, but at the same time the delay increases."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Размер буфера усреднения ДТОЖ"), _T(""), 1, 8, 1, 0, &d.inpavnum[3], _T("Размер буфера усреднения ДТОЖ. Чем больше значение усреднения, тем больше сглаживание, но при этом увеличивается запаздывание."));
 else
  dfd.AppendItem(_T("size of averaging buffer for CLT"), _T(""), 1, 8, 1, 0, &d.inpavnum[3], _T("Size of averaging buffer for CLT. The more averaging value, the greater smoothing, but at the same time the delay increases."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Размер буфера усреднения ДПДЗ"), _T(""), 1, 8, 1, 0, &d.inpavnum[4], _T("Размер буфера усреднения ДПДЗ. Чем больше значение усреднения, тем больше сглаживание, но при этом увеличивается запаздывание."));
 else
  dfd.AppendItem(_T("size of averaging buffer for TPS"), _T(""), 1, 8, 1, 0, &d.inpavnum[4], _T("Size of averaging buffer for TPS. The more averaging value, the greater smoothing, but at the same time the delay increases."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Размер буфера усреднения ADD_I1"), _T(""), 1, 8, 1, 0, &d.inpavnum[5], _T("Размер буфера усреднения ADD_I1. Чем больше значение усреднения, тем больше сглаживание, но при этом увеличивается запаздывание."));
 else
  dfd.AppendItem(_T("size of averaging buffer for ADD_I1"), _T(""), 1, 8, 1, 0, &d.inpavnum[5], _T("Size of averaging buffer for ADD_I1. The more averaging value, the greater smoothing, but at the same time the delay increases."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Размер буфера усреднения ADD_I2"), _T(""), 1, 8, 1, 0, &d.inpavnum[6], _T("Размер буфера усреднения ADD_I2. Чем больше значение усреднения, тем больше сглаживание, но при этом увеличивается запаздывание."));
 else
  dfd.AppendItem(_T("size of averaging buffer for ADD_I2"), _T(""), 1, 8, 1, 0, &d.inpavnum[6], _T("Size of averaging buffer for ADD_I2. The more averaging value, the greater smoothing, but at the same time the delay increases."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Размер буфера усреднения ADD_I3"), _T(""), 1, 8, 1, 0, &d.inpavnum[8], _T("Размер буфера усреднения ADD_I3. Чем больше значение усреднения, тем больше сглаживание, но при этом увеличивается запаздывание."));
 else
  dfd.AppendItem(_T("size of averaging buffer for ADD_I3"), _T(""), 1, 8, 1, 0, &d.inpavnum[8], _T("Size of averaging buffer for ADD_I3. The more averaging value, the greater smoothing, but at the same time the delay increases."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Размер буфера усреднения ADD_I4"), _T(""), 1, 8, 1, 0, &d.inpavnum[9], _T("Размер буфера усреднения ADD_I4. Чем больше значение усреднения, тем больше сглаживание, но при этом увеличивается запаздывание."));
 else
  dfd.AppendItem(_T("size of averaging buffer for ADD_I4"), _T(""), 1, 8, 1, 0, &d.inpavnum[9], _T("Size of averaging buffer for ADD_I4. The more averaging value, the greater smoothing, but at the same time the delay increases."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Размер буфера усреднения ADD_I5"), _T(""), 1, 8, 1, 0, &d.inpavnum[10], _T("Размер буфера усреднения ADD_I5. Чем больше значение усреднения, тем больше сглаживание, но при этом увеличивается запаздывание."));
 else
  dfd.AppendItem(_T("size of averaging buffer for ADD_I5"), _T(""), 1, 8, 1, 0, &d.inpavnum[10], _T("Size of averaging buffer for ADD_I5. The more averaging value, the greater smoothing, but at the same time the delay increases."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Размер буфера усреднения ADD_I6"), _T(""), 1, 8, 1, 0, &d.inpavnum[11], _T("Размер буфера усреднения ADD_I6. Чем больше значение усреднения, тем больше сглаживание, но при этом увеличивается запаздывание."));
 else
  dfd.AppendItem(_T("size of averaging buffer for ADD_I6"), _T(""), 1, 8, 1, 0, &d.inpavnum[11], _T("Size of averaging buffer for ADD_I6. The more averaging value, the greater smoothing, but at the same time the delay increases."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Размер буфера усреднения ADD_I7"), _T(""), 1, 8, 1, 0, &d.inpavnum[12], _T("Размер буфера усреднения ADD_I7. Чем больше значение усреднения, тем больше сглаживание, но при этом увеличивается запаздывание."));
 else
  dfd.AppendItem(_T("size of averaging buffer for ADD_I7"), _T(""), 1, 8, 1, 0, &d.inpavnum[12], _T("Size of averaging buffer for ADD_I7. The more averaging value, the greater smoothing, but at the same time the delay increases."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Размер буфера усреднения ADD_I8"), _T(""), 1, 8, 1, 0, &d.inpavnum[13], _T("Размер буфера усреднения ADD_I8. Чем больше значение усреднения, тем больше сглаживание, но при этом увеличивается запаздывание."));
 else
  dfd.AppendItem(_T("size of averaging buffer for ADD_I8"), _T(""), 1, 8, 1, 0, &d.inpavnum[13], _T("Size of averaging buffer for ADD_I8. The more averaging value, the greater smoothing, but at the same time the delay increases."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Размер буфера усреднения скорости"), _T(""), 1, 8, 1, 0, &d.inpavnum[7], _T("Размер буфера усреднения скорости автомобиля. Чем больше значение усреднения, тем больше сглаживание, но при этом увеличивается запаздывание."));
 else
  dfd.AppendItem(_T("size of averaging buffer for VSS"), _T(""), 1, 8, 1, 0, &d.inpavnum[7], _T("Size of averaging buffer for vehicle speed. The more averaging value, the greater smoothing, but at the same time the delay increases."));

 //idling regulator
 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Регулятор холостого хода:"));
 else
  dfd.AppendItem(_T("Idling regulator:"));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Зона оборотов подхвата УОЗ РХХ"), _T("мин-1"), 0, 1000, 1, 0, &d.idlreg_captrange, _T("УОЗ РХХ будет активирован когда обороты опустятся ниже значения равного сумме целевых оборотов ХХ и зоны подхвата. Например целевые обороты ХХ = 900, зона подхвата = 200, значит регулятор начнет работать когда обороты двигателя опустятся ниже 1100."));
 else
  dfd.AppendItem(_T("Capture range of the ign. tim. idling regulator"), _T("min-1"), 0, 1000, 1, 0, &d.idlreg_captrange, _T("Ignition-based idling regulator will be activated when the RPM falls below a value equal to the sum of the target idling RPM and the pickup zone. For example, target RPM = 900, pick-up zone = 200, which means the regulator will start working when the RPM drops below 1100."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Задержка включения УОЗ РХХ"), _T("сек"), 0.01f, 2.5f, 0.01f, 2, &d.idlent_timval, _T("После наступления всех необходимых условий УОЗ РХХ включится с задержкой определяемой этим параметром"));
 else
  dfd.AppendItem(_T("Ign. tim. idling regulator's turn on delay"), _T("sec"), 0.01f, 2.5f, 0.01f, 2, &d.idlent_timval, _T("Ign. tim. idling regulator will be turned on with delay specified by this parameter"));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Смещение РХХ при включении вентилятора"), _T("%"), 0.00f, 50.0f, 0.5f, 1, &d.vent_iacoff, _T("Это значение будет прибавлено к положению РХХ при поступлении запроса на включение вентилятора. В режиме closed loop смещение один раз прибавляется к положению РХХ. В режиме open loop смещение прибавляется при включении вентилятора и вычитается при выключении вентилятора"));
 else
  dfd.AppendItem(_T("Displacement of IAC on turn on of cooling fan"), _T("%"), 0.00f, 50.0f, 0.5f, 1, &d.vent_iacoff, _T("This value will be added to the IAC position when a request to turn on the fan is received. In closed loop mode, the offset is added once to the IAC position. In open loop mode, the displacement value is added when the fan is turned on and subtracted when the fan is turned off"));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Смещение РХХ при включении усилителя руля"), _T("%"), 0.00f, 50.0f, 0.5f, 1, &d.epas_iacoff, _T("Это значение прибавляется к положению РХХ при поступлении запроса включения усилителя руля (вход EPAS_I). В режиме closed loop смещение один раз прибавляется к положению РХХ. В режиме open loop смещение прибавляется при включении услилителя руля и вычитается при его выключении"));
 else
  dfd.AppendItem(_T("Displacement of IAC on turn on of EPAS"), _T("%"), 0.00f, 50.0f, 0.5f, 1, &d.epas_iacoff, _T("This value will be added to the IAC position when a request to turn on the EPAS is received (EPAS_I input). In closed loop mode, the offset is added once to the IAC position. In open loop mode, the displacement value is added when the EPAS is turned on and subtracted when the EPAS is turned off"));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Добавка к РДВ при вкл. кондиционера"), _T("%"), 0.0f, 100.0f, 0.5, 1, &d.iac_cond_add, _T("'Мин.положение РДВ' увеличивается на это значение при включении кондиционера"));
 else
  dfd.AppendItem(_T("Add. to IAC pos on turn on of air cond. "), _T("%"), 0.0f, 100.0f, 0.5, 1, &d.iac_cond_add, _T("'Minimum IAC position' increases by this value when the air conditioner is turned on"));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Шаг уменьшения добавки после выхода РХХ"), _T("%"), 0.025f, 7.5f, 0.025f, 3, &d.idltorun_stp_en, _T("Значение добавки после выхода уменьшается на это значение каждые 100мс если дроссель закрыт и обороты двигателя находятся между 1-м и 2-м порогами переходных режимов (пороги пер. режимов задаются соответствующими коэффициентами на вкладке параметров '3:ХХ', там же задается значение добавки после выхода)"));
 else
  dfd.AppendItem(_T("Decrease step of the IAC idle to run additive"), _T("%"), 0.025f, 7.5f, 0.025f, 3, &d.idltorun_stp_en, _T("Value of the IAC add after exit decreases by this value every 100ms if the throttle is closed and the engine speed is between the 1st and 2nd transient thresholds (transient thresholds are set by the corresponding coefficients on the '3:Idling' parameters' tab, value of the 'IAC add after exit' is also set there)"));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Шаг увеличения добавки после выхода РХХ"), _T("%"), 0.025f, 7.5f, 0.025f, 3, &d.idltorun_stp_le, _T("Значение добавки после выхода увеличивается на это значение каждые 100мс если дроссель открыт и обороты двигателя находятся выше 2-го порога переходных режимов (значение добавки после выхода и пороги пер. режимов задаются соответствующими коэффициентами на вкладке параметров '3:ХХ', там же задается значение добавки после выхода)"));
 else
  dfd.AppendItem(_T("Increase step of the IAC idle to run additive"), _T("%"), 0.025f, 7.5f, 0.025f, 3, &d.idltorun_stp_le, _T("Value of the IAC add after exit increases by this value every 100ms if the throttle is open and the engine speed is above the 2nd transient threshold (transient thresholds are set by the corresponding coefficients on the '3:Idling' parameters' tab, value of the 'IAC add after exit' is also set there)"));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Коэфф. нагрузки в аргументе таблицы жесткости РХХ"), _T(""), 0.0f, 6.0f, 0.05f, 2, &d.irr_k_load, _T("Коэффициент доли нагрузки в аргументе по горизонтальной оси в таблице жесткости РХХ. Чем выше значение, тем сильнее жесткость РХХ будет зависеть от нагрузки на двигатель."));
 else
  dfd.AppendItem(_T("Coeff. of load in argument of idl. rigidity map"), _T(""), 0.0f, 6.0f, 0.05f, 2, &d.irr_k_load, _T("The coefficient of the load share in the argument along the horizontal axis in the idling rigidity map. The higher the value, the more the idlign rigidity will depend on the engine load."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Коэфф. оборотов в аргументе таблицы жесткости РХХ"), _T(""), 0.0f, 6.0f, 0.05f, 2, &d.irr_k_rpm, _T("Коэффициент доли оборотов в аргументе по горизонтальной оси в таблице жесткости РХХ. Чем выше значение, тем сильнее жесткость РХХ будет зависеть от оборотов двигателя."));
 else
  dfd.AppendItem(_T("Coeff. of rpm in argument of idl. rigidity map"), _T(""), 0.0f, 6.0f, 0.05f, 2, &d.irr_k_rpm, _T("The coefficient of the rpm share in the argument along the horizontal axis in the idling rigidity map. The higher the value, the more the idlign rigidity will depend on the engine's rpm."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Использовать специальный алгоритм для холодного двигателя"), _T(""), 0, 1, 1, 1, &d.cold_eng_int, _T("Использовать отдельный алгоритм для регулятора ХХ при холодном двигателе (1). Установите в 0, чтобы использовать одинаковый алгоритм для холодного и горячего двигателя."));
 else
  dfd.AppendItem(_T("Use special algorithm for cold engine"), _T(""), 0, 1, 1, 1, &d.cold_eng_int, _T("Use separate closed loop algorithm when engine is cold (1). Set to 0 for using same algorithm for cold and hot engine."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Период вызова кода closed loop РДВ/ВЗ"), _T("сек"), 0.00f, 1.0f, 0.01f, 2, &d.iacreg_period, _T("Период вызова кода closed loop РДВ/ВЗ. Определяет частоту вызова регулятора. Используется в коде управления РДВ и в коде управления ВЗ"));
 else
  dfd.AppendItem(_T("Period of calling IAC's/Choke closed loop code"), _T("sec"), 0.00f, 1.0f, 0.01f, 2, &d.iacreg_period, _T("Period of calling IAC's/Choke closed loop code. Used in the IAC's regulator code and in the choke regulator code."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Порог ДТОЖ вкл. РДВ closed loop"), _T("°C"), 0.0f, 120.0f, 0.25f, 2, &d.iacreg_turn_on_temp, _T("Режим closed loop для РДВ включится только если температура двигателя выше этого порога или обороты выше целевых оборотов ХХ. Данный параметр актуален только если используется специальный алгоритм для холодного двигателя."));
 else
  dfd.AppendItem(_T("CLT threshold for turn on of the IAC closed loop"), _T("°C"), 0.0f, 120.0f, 0.25f, 2, &d.iacreg_turn_on_temp, _T("IAC closed loop will turn on only if the engine temperature is above this threshold or RPM is above target value. This parameter is useful only if special algorithm for cold engine is used."));

 //ignition timing
 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Управление зажиганием:"));
 else
  dfd.AppendItem(_T("Ignition control:"));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Всегда использовать карту УОЗ рабочего режима"), _T(""), 0, 1, 1, 0, &d.igntim_wrkmap, _T("Если установить в 1, то рабочая карта УОЗ будет использоваться не только для рабочего режима, но и для режима ХХ. Установите в 0 для работы в обычном режиме (для ХХ своя карта УОЗ)"));
 else
  dfd.AppendItem(_T("Always use working mode's ignition timing map"), _T(""), 0, 1, 1, 0, &d.igntim_wrkmap, _T("If set to 1, then working ignition timing map will be used also for idling mode. Set ot 0 for working in default mode (seperate map will be used on idling)"));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Применять ручную октан коррецию в режиме ХХ"), _T(""), 0, 1, 1, 0, &d.manigntim_idl, _T("0 - не применять ручную октан коррекцию на холостом ходу (коррекция будет применяться только в рабочем режиме), 1 - разрешить применение ручной октан коррекции на холостом ходу"));
 else
  dfd.AppendItem(_T("Apply manual ignition timing correction on idling"), _T(""), 0, 1, 1, 0, &d.manigntim_idl, _T("0 - don't apply manual ignition timing correction on idling, 1 - enable applying of manual ignition timing correction on idling"));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("УОЗ при переключении передач"), _T("°"), -15.0f, 55.0f, 0.1f, 1, &d.shift_igntim, _T("Значение УОЗ используемое при переключени передач (пока на входе AUTO_I присутствует сигнал)"));
 else
  dfd.AppendItem(_T("Ignition timing when shifting gears"), _T("°"), -15.0f, 55.0f, 0.1f, 1, &d.shift_igntim, _T("Value of ignition timing used when shifting gears (while signal is present at the AUTO_I input)"));

 //injection
 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Впрыск топлива:"));
 else
  dfd.AppendItem(_T("Fuel injection:"));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Длит. плавного входа в ПХХ"), _T("такт"), 0, 255, 1, 1, &d.fi_enter_strokes, _T("Длительность плавного входа в ПХХ в тактах двигателя. Впрыск топлива отключится не сразу, а будет постепенно убывать на протяжении указанного кол-ва тактов."));
 else
  dfd.AppendItem(_T("Smoothing of forced idle entering"), _T("str"), 0, 255, 1, 1, &d.fi_enter_strokes, _T("Smoothing of forced idle entering in engine strokes. Fuel injection will not turn off immediately, but will gradually decrease over the specified number of strokes."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Длит. плавного выхода из ПХХ"), _T("такт"), 0, 255, 1, 1, &d.fi_leave_strokes, _T("Длительность плавного выхода из ПХХ в тактах двигателя. Впрыск топлива включится не сразу, а будет постепенно возрастать на протяжении указанного кол-ва тактов."));
 else
  dfd.AppendItem(_T("Smoothing of forced idle leaving"), _T("str"), 0, 255, 1, 1, &d.fi_leave_strokes, _T("Smoothing of forced idle entering in engine strokes. Fuel injection will not turn on immediately, but will gradually increase over the specified number of strokes."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Порог ДПДЗ резкого выхода из режима ПХХ"), _T("%"), 0.0f, 100.0f, 0.5f, 1, &d.sfc_tps_thrd, _T("Выход из режима ПХХ будет резким, а не плавным, если ДПДЗ выше установленного порога"));
 else
  dfd.AppendItem(_T("TPS threshold for immediate exit from fuel cut mode"), _T("%"), 0.0f, 100.0f, 0.5f, 1, &d.sfc_tps_thrd, _T("Exiting from fuel cut mode will be immediate (not smooth) if TPS greater than this threshold"));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Наполнение на ХХ для бенз. (=0 - игнор.)"), _T("коэф"), 0.0f, 1.99f, 0.01f, 2, &d.idl_ve, _T("Установите значение больше 0, если вам нужно чтобы вместо таблицы наполнения в режиме ХХ использовалась эта константа. Данный параметр используется для бензина (GAS_V = 0)."));
 else
  dfd.AppendItem(_T("VE on idling for petrol (=0 - ignore)"), _T("coef"), 0.0f, 1.99f, 0.01f, 2, &d.idl_ve, _T("Set value reater than 0 if you want this constant to be used instead of the VE table on idling. This parameter is used for petrol (GAS_V = 0)."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Наполнение на ХХ для газа (=0 - игнор.)"), _T("коэф"), 0.0f, 1.99f, 0.01f, 2, &d.idl_ve_g, _T("Установите значение больше 0, если вам нужно чтобы вместо таблицы наполнения в режиме ХХ использовалась эта константа. Данный параметр используется для газа (GAS_V = 1)."));
 else
  dfd.AppendItem(_T("VE on idling for gas (=0 - ignore)"), _T("coef"), 0.0f, 1.99f, 0.01f, 2, &d.idl_ve_g, _T("Set value reater than 0 if you want this constant to be used instead of the VE table on idling. This parameter is used for LPG (GAS_V = 1)."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Максимальная длительность впрыска"), _T("мс"), 0.0f, 100.0f, 0.1f, 1, &d.inj_max_pw, _T("Ограничение максимальной длительности впрыска. Длительность импульсов впрыска не может быть больше этого значения."));
 else
  dfd.AppendItem(_T("Max. injection pulse width"), _T("ms"), 0.0f, 100.0f, 0.1f, 1, &d.inj_max_pw, _T("Limitting the maximum injection pulse width. The width of injection pulses can not be above this value."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Alpha-N: умножать время впрыска на ДПДЗ"), _T(""), 0, 2, 1, 0, &d.an_tps_mul, _T("Выбор на что умножать базовое время впрыска при работе в режиме Alpha-N: на ДАД или ДПДЗ. Если установить 0, то прошивка производит умножение на ДАД, если установить 1, то прошивка производит умножение на ДПДЗ. Если установить 2, то прошивка производит умножение на 101.3"));
 else
  dfd.AppendItem(_T("Alpha-N: multiply inj.PW by TPS"), _T(""), 0, 2, 1, 0, &d.an_tps_mul, _T("The choice of what to multiply the base injection PW when working in Alpha-N mode: by MAP or TPS. If you set 0, then the firmware multiplies by MAP, if you set 1, then the firmware multiplies by TPS. If you set it to 2, then firmware multiplies by 101.3 constant"));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Абсол. давление в топливной рампе"), _T("кПа"), 0.0f, 1000.0f, 0.1f, 1, &d.frap, _T("Абсолютное давление в топливной рампе. Установите значение больше 0, если хотите чтобы время впрыска корректировалось по разности давлений во впускном коллекторе и топливной рампе. Предполагается, что производительность форсунок указана для давления 3 бара."));
 else
  dfd.AppendItem(_T("Absolute pressure in the fuel rail"), _T("kPa"), 0.0f, 1000.0f, 0.1f, 1, &d.frap, _T("Absolute pressure in the fuel rail. Set value greater than 0 if you want the injection PW to be corrected for the pressure difference in the intake manifold and the fuel rail. It is assumed that injectors' flow rate is indicated for a pressure of 3 bars."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Использовать ручную корр. впрыска на ХХ"), _T(""), 0, 1, 1, 1, &d.maninjpw_idl, _T("Установите в 0, если не хотите чтобы ручная коррекция длительности впрыска работала на ХХ. Установите в 1 и коррекция будет работать не только в режиме под нагрузкой, но и в режиме ХХ."));
 else
  dfd.AppendItem(_T("Use manual inj. PW correction on idling"), _T(""), 0, 1, 1, 1, &d.maninjpw_idl, _T("Set to 0 if you don't want the manual inj.PW correction to work at idling. Set to 1 and correction will work not only in load mode, but also in idling mode."));

 //air conditioner
 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Кондиционер:"));
 else
  dfd.AppendItem(_T("Air conditioner:"));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Порог ДТОЖ вкл. кондиционера"), _T("°C"), 0.0f, 120.0f, 0.25f, 2, &d.aircond_clt, _T("Кондиционер включится только если температура двигателя выше этого порога, иначе запросы на включение будут игнорироваться."));
 else
  dfd.AppendItem(_T("CLT threshold for turn on of air cond."), _T("°C"), 0.0f, 120.0f, 0.25f, 2, &d.aircond_clt, _T("Air conditioner will turn on only if the engine temperature is above this threshold, otherwise requests for switching on will be ignored."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Порог ДПДЗ выкл. кондиционера"), _T("%"), 0.0f, 100.0f, 0.5f, 1, &d.aircond_tps, _T("Компрессор кондиционера не будет включаться (или будет выключаться) если положение дроссельной заслогки превышает указанное значение."));
 else
  dfd.AppendItem(_T("TPS threshold for turning off air cond."), _T("%"), 0.0f, 100.0f, 0.5f, 1, &d.aircond_tps, _T("The compressor of air conditioner will not turn on (or it will turn off) if throttle position exceeds specified value."));

 //canister purge valve
 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Управление адсорбером:"));
 else
  dfd.AppendItem(_T("Canister purge valve:"));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Порог ДТОЖ вкл. продувки адсорбера"), _T("°C"), 0.0f, 120.0f, 0.25f, 2, &d.evap_clt, _T("Продувка адсорбера будет включаться только если температура двигателя выше этого порога."));
 else
  dfd.AppendItem(_T("CLT threshold for turning on of the canister purge valve"), _T("°C"), 0.0f, 120.0f, 0.25f, 2, &d.evap_clt, _T("The canister purge valve will work only if engine's temperature is above this threshold."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Нижн. порог ДПДЗ продувки адсорбера"), _T("%"), 0.0f, 100.0f, 0.5f, 1, &d.evap_tps_lo, _T("Продувка адсорбера не будет включаться если положение дроссельной заслонки ниже или равно этому порогу. Продувка не должна работать на ХХ."));
 else
  dfd.AppendItem(_T("Low TPS threshold for the canister purge valve"), _T("%"), 0.0f, 100.0f, 0.5f, 1, &d.evap_tps_lo, _T("The canister purge valve will not turn on if throttle position is lower or equal to this threshold. Purge should not work on idling."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Верх. порог ДПДЗ продувки адсорбера"), _T("%"), 0.0f, 100.0f, 0.5f, 1, &d.evap_tps_hi, _T("Продувка адсорбера не будет включаться если положение дроссельной заслонки выше этого порога. Продувка не должна работать в режимах полных нагрузок."));
 else
  dfd.AppendItem(_T("Hi TPS threshold for the canister purge valve"), _T("%"), 0.0f, 100.0f, 0.5f, 1, &d.evap_tps_hi, _T("The canister purge valve will not turn on if throttle position is above this threshold. Purge should not work in full load conditions."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Абсол. давление выключения продувки адсорбера"), _T("кПа"), 0.0f, 500.0f, 1.0f, 1, &d.evap_map_thrd, _T("Порог абсолютного давления на впуске, при превышении которого клапан продувки будет выключен."));
 else
  dfd.AppendItem(_T("Absolute pressure threshold for valve's turn off"), _T("kPa"), 0.0f, 500.0f, 1.0f, 1, &d.evap_map_thrd, _T("Manifold absolute pressure threshold, above which canister purge valve will be turned off."));

 //heating control
 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Управление подогревом:"));
 else
  dfd.AppendItem(_T("Control of heaters:"));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Порог выключения подогрева коллектора"), _T("°C"), -40.0f, 120.0f, 0.25f, 2, &d.heating_t_off, _T("Подогрев впускного коллектора будет выключен после того как температура двигателя достигнет указанного значения. Управление подогревом осуществляется посредством выхода переназначенного как INTK_HEAT."));
 else
  dfd.AppendItem(_T("Turn off threshold of intake manifold heating"), _T("°C"), -40.0f, 120.0f, 0.25f, 2, &d.heating_t_off, _T("Intake manifold heating will be turned off after the temperature of engine reaches the specified value. The heating control is carried out by means of the output reassigned as INTK_HEAT."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Макс. время работы подогрева коллектора"), _T("мин"), 0.1f, 10.9f, 0.1f, 1, &d.heating_time, _T("Подгрев впускного коллектора будет выключен по истечении этого времени даже если температурный порог еще не достигнут. Это должно предотвратить чрезмерный разряд аккумулятора."));
 else
  dfd.AppendItem(_T("Turn off threshold of intake manifold heating"), _T("min"), 0.1f, 10.9f, 0.1f, 1, &d.heating_time, _T("Intake manifold heating will be turned off after this time, even if the temperature threshold has not yet been reached. This should prevent over-discharge of the battery."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Макс. время работы подогрева газового редуктора"), _T("мин"), 0.01f, 10.0f, 0.01f, 2, &d.grheat_time, _T("Подгрев впускного коллектора будет выключен по истечении этого времени когда двигатель остановлен. Это должно предотвратить чрезмерный разряд аккумулятора."));
 else
  dfd.AppendItem(_T("Timeout threshold of gas reducer's heater"), _T("min"), 0.01f, 10.0f, 0.01f, 2, &d.grheat_time, _T("Gas reducer's heating will be turned off after this time when engine is stopped. This should prevent over-discharge of the battery."));

 //cooling fan
 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Управление вентилятором:"));
 else
  dfd.AppendItem(_T("Cooling fan's control:"));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Задержка включения вентилятора"), _T("сек"), 0.00f, 2.5f, 0.01f, 2, &d.vent_delay, _T("Используется для включения вентилятора с задержкой чтобы РХХ успел за это время поднять обороты двигателя (актуально только для прошивок с впрыском топлива)."));
 else
  dfd.AppendItem(_T("Cooling fan's turn on delay"), _T("sec"), 0.00f, 2.5f, 0.01f, 2, &d.vent_delay, _T("It is used to turn on the fan with delay so that the IAC has time to increase the engine speed during this time (relevant only for firmware with fuel injection)."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Число дискрет ШИМ вентилятора"), _T("N"), 4, 64, 1, 1, &d.vent_pwmsteps, _T("Число шагов ШИМ вентилятора. Чем больше число шагов, тем шире зона температуры и тем более плавно вентилятор меняет скорость и наоборот, чем меньше число шагов, тем уже диапазон температуры и тем менее плавно вентилятор меняет скорость."));
 else
  dfd.AppendItem(_T("Number of cooling fan's PWM discretes"), _T("N"), 4, 64, 1, 1, &d.vent_pwmsteps, _T("Number of cooling fan's PWM steps. The larger number of steps, the wider the temperature band and fan changes speed more smoothly and vice versa, the smaller number of steps, the narrower the temperature band and fan changes speed less smoothly."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Нижняя мертвая зона ШИМ вентилятора"), _T("N"), 0, 60, 1, 1, &d.vent_minband, _T("Если текущий номер шага меньше этого значения, то вентилятор полностью выключается (или не включается). Это нужно для того, чтобы ШИМ зря не 'мучил' щетки вентилятора когда скважности не хватает для создания достаточного крутящего момента чтобы вал вращался."));
 else
  dfd.AppendItem(_T("Low dead band for cooling fan's PWM"), _T("N"), 0, 60, 1, 1, &d.vent_minband, _T("If the current step number (duty) is less than this value, the fan turns off completely (or does not turn on). This is necessary so that the PWM in vain does not 'torment' the fan's brushes when duty cycle is not enough to create torque to rotate the shaft."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Число шагов ШИМ используемое при работе кондиционера"), _T("N"), 0, 60, 1, 1, &d.vent_maxband, _T("При работе кондиционера скважность ШИМ вентилятора будет определяться этим числом шагов."));
 else
  dfd.AppendItem(_T("Number of PWM steps used with air conditioner"), _T("N"), 0, 60, 1, 1, &d.vent_maxband, _T("When the air conditioner is on, the duty cycle of the fan's PWM will be determined by this number of steps."));

 //miscellaneous
 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Прочие константы:"));
 else
  dfd.AppendItem(_T("Other constants:"));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Разрешить запуск при продувке двигателя"), _T(""), 0, 1, 1, 0, &d.fldclr_start, _T("Если запуск в режиме продувки двигателя разрешен (1), то при достижении оборотов перехода с пусковой карты впрыск топлива возобновится и двигатель запустится."));
 else
  dfd.AppendItem(_T("Allow engine start in flood clear mode"), _T(""), 0, 1, 1, 0, &d.fldclr_start, _T("If start in flood clear mode is enabled (1), then after engine speed reach 'Switch from cranking ignition map RPM' supply of fuel will be resumed and engine will start."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Таймер блокировки стартера"), _T("такт"), 0, 255, 1, 1, &d.stbl_str_cnt, _T("Количество тактов от момента достижения порога оборотов блокировки стартера до момента его фактического выключения. Установите значение 0, если хотите чтобы использовалась таблица вместо этой простой константы."));
 else
  dfd.AppendItem(_T("Starter's blocking timer"), _T("str"), 0, 255, 1, 1, &d.stbl_str_cnt, _T("Тumber of strokes from the moment the starter's blocking threshold is reached until starter is actually turned off. Set the value to 0 if you want the table to be used instead of this simple constant."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Алгоритм прогнозирования положения коленвала для ДХ"), _T(""), 0, 1, 1, 0, &d.hall_predict, _T("Выбор алгоритма прогнозирования положения коленвала в прошивках с синхронизацией от ДХ (Nзуб=Nцил). 0 - последний интервал. 1 - Первая производная"));
 else
  dfd.AppendItem(_T("Algorithm of crankshaft position prediction for hall"), _T(""), 0, 1, 1, 0, &d.hall_predict, _T("Selection of algorithm of crankshaft position prediction for firmwares with synchronization from a Hall sensor (Nteeth=Ncyl). 0 - Last interval. 1 - First derivative."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Множитель частоты импульсов на выходе для тахометра (VTACHOM)"), _T("*"), 0.125f, 7.9f, 0.001f, 3, &d.vtachom_mult, _T("Данный множитель определяет соотношение оборотов и частоты импульсов для тахометра выдаваемых на выход VTACHOM. Например, 1.000 означает, что за один оборот коленвала выдается 1 импульс. Чем выше значение множителя, тем выше частота."));
 else
  dfd.AppendItem(_T("Frequency multiplier for tachometer output (VTACHOM)"), _T("*"), 0.125f, 7.9f, 0.001f, 3, &d.vtachom_mult, _T("This multiplier specify ratio of RPM and frequency of pulses produced at VTACHOM output. For example, 1.000 means that for 1 revolution of crankshaft 1 pulse is produced. The higher the value of multiplier, the higher the frequency."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Передавать вместо ADD_I1 значение другого входа"), _T(""), 1, 8, 1, 0, &d.add_i1_sub, _T("Выбор что прошивка передает вместо значения входа ADD_I1. 1 - вход ADD_I1 (по умолчанию), 2 - вход ADD_I2, 3 - вход ADD_I3, 4 - вход ADD_I4 и т.д."));
 else
  dfd.AppendItem(_T("Send value of another input instead of ADD_I1"), _T(""), 1, 8, 1, 0, &d.add_i1_sub, _T("Select what firmware should send instead of ADD_I1 value. 1 - ADD_I1 input (default), 2 - ADD_I2 input, 3 - ADD_I3 input, 4 - ADD_I4 input etc"));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Передавать вместо ADD_I2 значение другого входа"), _T(""), 1, 8, 1, 0, &d.add_i2_sub, _T("Выбор что прошивка передает вместо значения входа ADD_I2. 1 - вход ADD_I1, 2 - вход ADD_I2 (по умолчанию), 3 - вход ADD_I3, 4 - вход ADD_I4 и т.д."));
 else
  dfd.AppendItem(_T("Send value of another input instead of ADD_I2"), _T(""), 1, 8, 1, 0, &d.add_i2_sub, _T("Select what firmware should send instead of ADD_I2 value. 1 - ADD_I1 input, 2 - ADD_I2 input (default), 3 - ADD_I3 input, 4 - ADD_I4 input etc"));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Время открытого газового клапана на не запущенном двигателе"), _T("сек"), 1.0f, 100.0f, 0.1f, 1, &d.gasval_ontime, _T("Время на протяжении которого выход управляющий газовым клапаном (GASVAL_O) будет включен на не запущеном двигателе."));
 else
  dfd.AppendItem(_T("Time of opened gas valve on stopped engine"), _T("sec"), 1.0f, 100.0f, 0.1f, 1, &d.gasval_ontime, _T("During this time output which controls gas valve (GASVAL_O) will be turned on when engine is stopped."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Отсечка зажигания и впрыска при низком давлении масла"), _T(""), 0, 1, 1, 0, &d.oilpress_cut, _T("Защита двигателя от работы при низком или отсутствующем давлении масла. 1 - включить защиту, 0 - выключить."));
 else
  dfd.AppendItem(_T("Allow ignition and fuel injection cut off when oil pressure is low"), _T(""), 0, 1, 1, 0, &d.oilpress_cut, _T("Protection of engine against deterioration when pressure of oil is too low"));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Мин. значение dt используемое для вычисления d%/dt ДЗ"), _T("мс"), 5.0f, 200.0f, 1.0f, 1, &d.tpsdot_mindt, _T("Укажите значение дифференциала времени используемого для вычисления скорости движения дроссельной заслонки"));
 else
  dfd.AppendItem(_T("Minimum value of dt used for calculation of the d%/dt for TPS"), _T("ms"), 5.0f, 200.0f, 1.0f, 1, &d.tpsdot_mindt, _T("Specify value of the time differencial used by calculation of the speed of moving throttle"));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Длительность работы блока после выключения зажигания."), _T("сек"), 0.01f, 100.0f, 0.1f, 2, &d.pwron_time, _T("Блок продолжит работу указанное время после выключения зажигания. Подразумевается использование управления питанием блока."));
 else
  dfd.AppendItem(_T("The duration of the unit's operation after the ignition is turned off. "), _T("sec"), 0.01f, 100.0f, 0.1f, 2, &d.pwron_time, _T("Unit will continue to operate specified time after switching ignition off."));

 //Crankshaft position settings
 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Положение коленвала (нов.алгор.)"));
 else
  dfd.AppendItem(_T("Crankshaft position (new algor.):"));


 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("ВМТ цилиндра 1"), _T("°"), 0.0f, 720.0f, 0.1f, 2, &d.tdc_angle[0], _T("Положение ВМТ 1-го цилиндра относительно первого зуба диска синхронизации"));
 else
  dfd.AppendItem(_T("TDC of the 1 cylinder"), _T("°"), 0.0f, 720.0f, 0.1f, 2, &d.tdc_angle[0], _T("TDC position of 1 cylinder relatively to the first tooth of trigger wheel"));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("ВМТ цилиндра 2"), _T("°"), 0.0f, 720.0f, 0.1f, 2, &d.tdc_angle[1], _T("Положение ВМТ 2-го цилиндра относительно первого зуба диска синхронизации"));
 else
  dfd.AppendItem(_T("TDC of the 2 cylinder"), _T("°"), 0.0f, 720.0f, 0.1f, 2, &d.tdc_angle[1], _T("TDC position of 2 cylinder relatively to the first tooth of trigger wheel"));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("ВМТ цилиндра 3"), _T("°"), 0.0f, 720.0f, 0.1f, 2, &d.tdc_angle[2], _T("Положение ВМТ 3-го цилиндра относительно первого зуба диска синхронизации"));
 else
  dfd.AppendItem(_T("TDC of the 3 cylinder"), _T("°"), 0.0f, 720.0f, 0.1f, 2, &d.tdc_angle[2], _T("TDC position of 3 cylinder relatively to the first tooth of trigger wheel"));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("ВМТ цилиндра 4"), _T("°"), 0.0f, 720.0f, 0.1f, 2, &d.tdc_angle[3], _T("Положение ВМТ 4-го цилиндра относительно первого зуба диска синхронизации"));
 else
  dfd.AppendItem(_T("TDC of the 4 cylinder"), _T("°"), 0.0f, 720.0f, 0.1f, 2, &d.tdc_angle[3], _T("TDC position of 4 cylinder relatively to the first tooth of trigger wheel"));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("ВМТ цилиндра 5"), _T("°"), 0.0f, 720.0f, 0.1f, 2, &d.tdc_angle[4], _T("Положение ВМТ 5-го цилиндра относительно первого зуба диска синхронизации"));
 else
  dfd.AppendItem(_T("TDC of the 5 cylinder"), _T("°"), 0.0f, 720.0f, 0.1f, 2, &d.tdc_angle[4], _T("TDC position of 5 cylinder relatively to the first tooth of trigger wheel"));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("ВМТ цилиндра 6"), _T("°"), 0.0f, 720.0f, 0.1f, 2, &d.tdc_angle[5], _T("Положение ВМТ 6-го цилиндра относительно первого зуба диска синхронизации"));
 else
  dfd.AppendItem(_T("TDC of the 6 cylinder"), _T("°"), 0.0f, 720.0f, 0.1f, 2, &d.tdc_angle[5], _T("TDC position of 6 cylinder relatively to the first tooth of trigger wheel"));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("ВМТ цилиндра 7"), _T("°"), 0.0f, 720.0f, 0.1f, 2, &d.tdc_angle[6], _T("Положение ВМТ 7-го цилиндра относительно первого зуба диска синхронизации"));
 else
  dfd.AppendItem(_T("TDC of the 7 cylinder"), _T("°"), 0.0f, 720.0f, 0.1f, 2, &d.tdc_angle[6], _T("TDC position of 7 cylinder relatively to the first tooth of trigger wheel"));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("ВМТ цилиндра 8"), _T("°"), 0.0f, 720.0f, 0.1f, 2, &d.tdc_angle[7], _T("Положение ВМТ 8-го цилиндра относительно первого зуба диска синхронизации"));
 else
  dfd.AppendItem(_T("TDC of the 8 cylinder"), _T("°"), 0.0f, 720.0f, 0.1f, 2, &d.tdc_angle[7], _T("TDC position of 8 cylinder relatively to the first tooth of trigger wheel"));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Угол опроса датчиков (до ВМТ)"), _T("°"), 0.0f, 720.0f, 0.1f, 2, &d.smp_angle, _T("Угол начала опроса датчиков (запуск АЦП) относительно ВМТ (угол до ВМТ)"));
 else
  dfd.AppendItem(_T("Angle of sensors' polling (BTDC)"), _T("°"), 0.0f, 720.0f, 0.1f, 2, &d.smp_angle, _T("Angle of sensors' polling (start of ADC) relatively to TDC (angle before TDC)"));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Время между искрой и началом накопления"), _T("мс"), 0.10f, 5.00f, 0.01f, 2, &d.dwl_dead_time, _T("Данный параметр ограничивает длительность импульса нокопления чтобы последнее не начиналось пока горит искра."));
 else
  dfd.AppendItem(_T("Time between spark and start of dwell"), _T("ms"), 0.10f, 5.00f, 0.01f, 2, &d.dwl_dead_time, _T("This parameter limits pulse width of dwell so that the latter does not start while the spark is on"));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Число зубьев пропускаемое перед пуском"), _T("N"), 0, 500, 1, 1, &d.ckps_skip_trig, _T("Число зубьев, которое система проигнорирует перед поиском синхрометки в начале пуска двигателя."));
 else
  dfd.AppendItem(_T("Number of teeth to skip at the beginning of cranking"), _T("N"), 0, 500, 1, 1, &d.ckps_skip_trig, _T("Number of teeth, system will ignore before looking for missing tooth at the beginning of cranking"));

 if (dfd.DoModal()==IDOK)
 {
  mp_fwdm->SetFwConstsData(d);
 }
}

//Пользователь захотел получить информацию о прошивке из SECU-3
void CFirmwareTabController::OnFirmwareInfo(void)
{
 mp_sbar->SetInformationText(MLL::LoadString(IDS_FW_READING_FW_SIGNATURE));
 SECU3IO::OPCompNc packet_data;
 packet_data.opcode = SECU3IO::OPCODE_READ_FW_SIG_INFO;
 mp_comm->m_pControlApp->SendPacket(OP_COMP_NC,&packet_data);
 m_read_fw_sig_info_flag = true;
}

void CFirmwareTabController::OnViewFWOptions(void)
{
 BYTE fw_version[2];
 _ShowFWOptions(mp_fwdm->GetSignatureInfo(), mp_fwdm->GetFWOptions(), mp_fwdm->GetFWVersion(fw_version));
}

bool CFirmwareTabController::IsViewFWOptionsAvailable(void)
{
 return mp_fwdm->GetFWOptions() > 0;
}

bool CFirmwareTabController::IsIORemappingAvailable(void)
{
 return true;
}

void CFirmwareTabController::SetAttenuatorMap(const float* i_values)
{
 mp_fwdm->SetAttenuatorMap(i_values);
 SetViewChartsValues();
 mp_view->mp_TablesPanel->UpdateOpenedCharts();
}

void CFirmwareTabController::GetAttenuatorMap(float* o_values)
{
 ASSERT(o_values);
 mp_fwdm->GetAttenuatorMap(o_values, false); //<--NOTE: modified
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
  SECUMessageBox(str_options.c_str(), MB_OK|MB_ICONINFORMATION);
 }
}


void CFirmwareTabController::finishOnBootLoaderInfo(void)
{
 //активируем коммуникационный контроллер бутлоадера
 VERIFY(mp_comm->SwitchOn(CCommunicationManager::OP_ACTIVATE_BOOTLOADER));

 //операция не блокирует поток - стековые переменные ей передавать нельзя!
 mp_comm->m_pBootLoader->StartOperation(CBootLoader::BL_OP_READ_SIGNATURE,mp_bl_data,0);

 mp_sbar->ShowProgressBar(true);
 mp_sbar->SetProgressPos(0);
}

void CFirmwareTabController::finishOnReadEepromToFile(void)
{
 //активируем коммуникационный контроллер бутлоадера
 VERIFY(mp_comm->SwitchOn(CCommunicationManager::OP_ACTIVATE_BOOTLOADER));

 //операция не блокирует поток - стековые переменные ей передавать нельзя!
 mp_comm->m_pBootLoader->StartOperation(CBootLoader::BL_OP_READ_EEPROM,mp_bl_data,0);

 mp_sbar->ShowProgressBar(true);
 mp_sbar->SetProgressPos(0);
}

void CFirmwareTabController::finishOnWriteEepromFromFile(void)
{
 //активируем коммуникационный контроллер бутлоадера
 VERIFY(mp_comm->SwitchOn(CCommunicationManager::OP_ACTIVATE_BOOTLOADER));

 //операция не блокирует поток - стековые переменные ей передавать нельзя!
 mp_comm->m_pBootLoader->StartOperation(CBootLoader::BL_OP_WRITE_EEPROM,mp_bl_data,0);

 mp_sbar->ShowProgressBar(true);
 mp_sbar->SetProgressPos(0);
}

void CFirmwareTabController::finish_OnReadFlashToFile(void)
{
 //активируем коммуникационный контроллер бутлоадера
 VERIFY(mp_comm->SwitchOn(CCommunicationManager::OP_ACTIVATE_BOOTLOADER));

 //операция не блокирует поток - стековые переменные ей передавать нельзя!
 mp_comm->m_pBootLoader->StartOperation(CBootLoader::BL_OP_READ_FLASH,mp_bl_data, m_fpp.m_total_size);

 mp_sbar->ShowProgressBar(true);
 mp_sbar->SetProgressPos(0);
}

void CFirmwareTabController::finishStartWritingOfFLASHFromBuff(void)
{
 //активируем коммуникационный контроллер бутлоадера
 VERIFY(mp_comm->SwitchOn(CCommunicationManager::OP_ACTIVATE_BOOTLOADER));

 //Если установлен режим прошивки только кода (без данных), то все несколько сложнее
 if (mp_view->IsProgrammeOnlyCode())
 {
  //Мы программируем только код, однако контрольная сумма останется посчитаной для старых данных. Поэтому нам необходимо
  //прочитать данные, обединить их с новым кодом, обновить контрольную сумму и только потом программировать.
  m_bl_read_flash_mode = MODE_RD_FLASH_TO_BUFF_MERGE_DATA;

  //сохраняем данные для того, чтобы позже объединить их с прочитанными "верхними" данными
  m_code_for_merge_size = mp_fwdm->GetOnlyCodeSize(mp_bl_data); //code + code data
  memcpy(mp_code_for_merge_with_overhead, mp_bl_data, m_fpp.m_app_section_size);

  //Читаем немного больше байт, для того, чтобы гарантировано прочитать данные находящиеся в коде
  size_t reducedSize = m_code_for_merge_size - m_moreSize; // + some number of bytes more
  //операция не блокирует поток - стековые переменные ей передавать нельзя!
  mp_comm->m_pBootLoader->StartOperation(CBootLoader::BL_OP_READ_FLASH, mp_bl_data,
  m_fpp.m_app_section_size - reducedSize, //размер данных сверху над кодом программы
  reducedSize);                           //адрес начала "верхних" данных
 }
 else
 {//все очень просто
  mp_comm->m_pBootLoader->StartOperation(CBootLoader::BL_OP_WRITE_FLASH, mp_bl_data, m_fpp.m_app_section_size);
 }

 mp_sbar->ShowProgressBar(true);
 mp_sbar->SetProgressPos(0);
}

void CFirmwareTabController::OnCESettingsButton(void)
{
 CCESettingsCntr dialog;
 CESettingsData data;
 mp_fwdm->GetCESettingsData(data);
 dialog.SetValues(data);
 dialog.EnableSECU3TItems(CHECKBIT32(mp_fwdm->GetFWOptions(), SECU3IO::COPT_SECU3T));
 dialog.EnableExtraIO(!CHECKBIT32(mp_fwdm->GetFWOptions(), SECU3IO::COPT_SECU3T) && CHECKBIT32(mp_fwdm->GetFWOptions(), SECU3IO::COPT_TPIC8101));
 WndSettings ws;
 WndSize sz;
 mp_settings->GetWndSettings(ws);
 mp_settings->GetWndSize(sz);
 dialog.SetWndPosition(ws.m_CESettingsWnd_X, ws.m_CESettingsWnd_Y);
 dialog.SetWndSize(sz.m_CESettingsWnd_W, sz.m_CESettingsWnd_H);

 if (dialog.DoModal() == IDOK)
 {
  dialog.GetValues(data);
  mp_fwdm->SetCESettingsData(data);
 }

 CPoint wndPos = dialog.GetWndPosition();
 CSize wndSize = dialog.GetWndSize();
 ws.m_CESettingsWnd_X = wndPos.x; 
 ws.m_CESettingsWnd_Y = wndPos.y;
 mp_settings->SetWndSettings(ws);
 sz.m_CESettingsWnd_W = wndSize.cx; 
 sz.m_CESettingsWnd_H = wndSize.cy;
 mp_settings->SetWndSize(sz);
}

void CFirmwareTabController::OnChangeSettingsMapEd(void)
{
 mp_settings->SetITEdMode(mp_view->mp_TablesPanel->GetITEdMode());
 mp_view->mp_ParamDeskDlg->SetITEdMode(mp_view->mp_TablesPanel->GetITEdMode());
 mp_settings->SetActiveVEMap(mp_view->mp_TablesPanel->GetActiveVEMap());

 MapPtMovStep mptms;
 mp_settings->GetMapPtMovStep(mptms);
 mptms.m_start_map = mp_view->mp_TablesPanel->GetPtMovStep(TYPE_MAP_DA_START);
 mptms.m_idle_map = mp_view->mp_TablesPanel->GetPtMovStep(TYPE_MAP_DA_IDLE);
 mptms.m_work_map = mp_view->mp_TablesPanel->GetPtMovStep(TYPE_MAP_DA_WORK);
 mptms.m_temp_map = mp_view->mp_TablesPanel->GetPtMovStep(TYPE_MAP_DA_TEMP_CORR);
 mptms.m_tempi_map = mp_view->mp_TablesPanel->GetPtMovStep(TYPE_MAP_DA_TEMPI_CORR);
 mptms.m_ve_map = mp_view->mp_TablesPanel->GetPtMovStep(TYPE_MAP_INJ_VE);
 mptms.m_ve2_map = mp_view->mp_TablesPanel->GetPtMovStep(TYPE_MAP_INJ_VE2);
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
 mptms.m_pwm1_map = mp_view->mp_TablesPanel->GetPtMovStep(TYPE_MAP_PWM1);
 mptms.m_pwm2_map = mp_view->mp_TablesPanel->GetPtMovStep(TYPE_MAP_PWM2);
 mptms.m_iacmat_map = mp_view->mp_TablesPanel->GetPtMovStep(TYPE_MAP_INJ_IACMAT);

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
 mptms.m_cranking_thrd_map = mp_view->mp_TablesPanel->GetPtMovStep(TYPE_MAP_CRANKING_THRD);
 mptms.m_cranking_time_map = mp_view->mp_TablesPanel->GetPtMovStep(TYPE_MAP_CRANKING_TIME);
 mptms.m_smapaban_thrd_map = mp_view->mp_TablesPanel->GetPtMovStep(TYPE_MAP_SMAPABAN_THRD);
 mptms.m_knock_zone_map = mp_view->mp_TablesPanel->GetPtMovStep(TYPE_MAP_KNOCK_ZONE);
 mptms.m_grts_curve_map = mp_view->mp_TablesPanel->GetPtMovStep(TYPE_MAP_GRTS_CURVE);
 mptms.m_grheat_duty_map = mp_view->mp_TablesPanel->GetPtMovStep(TYPE_MAP_GRHEAT_DUTY);
 mptms.m_pwmiac_ucoef_map = mp_view->mp_TablesPanel->GetPtMovStep(TYPE_MAP_PWMIAC_UCOEF);
 mptms.m_aftstr_strk0_map = mp_view->mp_TablesPanel->GetPtMovStep(TYPE_MAP_AFTSTR_STRK0);
 mptms.m_aftstr_strk1_map = mp_view->mp_TablesPanel->GetPtMovStep(TYPE_MAP_AFTSTR_STRK1);
 mptms.m_grvaldel_map = mp_view->mp_TablesPanel->GetPtMovStep(TYPE_MAP_GRVDELAY);
 mptms.m_ftls_curve_map = mp_view->mp_TablesPanel->GetPtMovStep(TYPE_MAP_FTLS_CURVE);
 mptms.m_egts_curve_map = mp_view->mp_TablesPanel->GetPtMovStep(TYPE_MAP_EGTS_CURVE);
 mptms.m_ops_curve_map = mp_view->mp_TablesPanel->GetPtMovStep(TYPE_MAP_OPS_CURVE);
 mptms.m_maninjpwc_map = mp_view->mp_TablesPanel->GetPtMovStep(TYPE_MAP_MANINJPWC);
 mptms.m_mafcurve_map = mp_view->mp_TablesPanel->GetPtMovStep(TYPE_MAP_MAF_CURVE);
 mp_settings->SetMapPtMovStep(mptms);
}

void CFirmwareTabController::OnFirmwareMaster(void)
{
 bool is_continue = CheckChangesAskAndSaveFirmware();
 if (!is_continue)
  return;  //user has decided to cancel

 FirmwareMasterCntr cntr;
 _TSTRING name;
 std::vector<BYTE> buff(m_fpp.m_total_size, 0);

 EECUPlatform ep = mp_settings->GetECUPlatformType();
 if (ep != EP_ATMEGA644 && ep != EP_ATMEGA1284)
  return; //error: unsupported platform

 int result = cntr.DoLoad(m_fpp.m_flAnnex, &buff[0], m_fpp.m_total_size, &name);
 if (result == IDOK && cntr.GetStatus())
 {
  if (_CheckFirmwareCompatibilityAndAskUser(&buff[0]) && _CheckQuartzCompatibilityAndAskUser(&buff[0])) //user OK?
  {
   if (FirmwareFileUtils::CheckFirmwareIntegrity(&buff[0], m_fpp.m_total_size))
    PrepareOnLoadFLASH(&buff[0], _TSTRING(name));
  }
 }
}
