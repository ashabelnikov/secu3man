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

/** \file CEEPROMTabController.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "Resources/resource.h"
#include "EEPROMTabController.h"
#include <limits>
#include <set>

#include "Application/CommunicationManager.h"
#include "common/FastDelegate.h"
#include "FirmwareTabController.h"
#include "MainFrame/StatusBarManager.h"
#include "TabDialogs/EEPROMTabDlg.h"
#include "Settings/ISettingsData.h"
#include "io-core/bitmask.h"
#include "io-core/ce_errors.h"
#include "io-core/EEPROMDataMediator.h"
#include "io-core/FirmwareDataMediator.h"
#include "FirmwareFileUtils.h"
#include "TabControllersCommunicator.h"
#include "TablDesk/ButtonsPanel.h"
#include "io-core/MapIds.h"
#include "ParamDesk/Params/ParamDeskDlg.h"
#include "CEErrorIdStr.h"
#include "ErrorMsg.h"
#include "ui-core/MsgBox.h"
#include "ui-core/DynFieldsDialog.h"

using namespace fastdelegate;
using namespace SECU3IO;

#define EHKEY _T("EEPROMDataCntr")
#define DELAY_AFTER_BL_START 100
#define WAITING_BL_START_TIMEOUT 8000

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEEPROMTabController::CEEPROMTabController(CEEPROMTabDlg* i_view, CCommunicationManager* i_comm, CStatusBarManager* i_sbar, ISettingsData* ip_settings)
: mp_view(i_view)
, mp_comm(i_comm)
, mp_sbar(i_sbar)
, mp_settings(ip_settings)
, m_lastSel(0)
, mp_errors_ids(new CEErrorIdStr())
, m_blFinishOpCB(NULL)
, MapWndScrPos(ip_settings)
, m_active(false)
, m_firmware_opened(false)
, m_clear_sbar_txt_on_conn(false)
{
 PlatformParamHolder holder(ip_settings->GetECUPlatformType());
 m_epp = holder.GetEepromParameters();
 mp_eedm = new EEPROMDataMediator(holder.GetEepromParameters());
 mp_eedm->SetQuartzFrq(PlatformParamHolder::GetQuartzFreq(mp_settings->GetECUPlatformType()));

 ASSERT(mp_eedm);

 mp_bl_data = new BYTE[m_epp.m_size + 1];
 ASSERT(mp_bl_data);

 mp_view->setOnReadEEPROMFromSECU(MakeDelegate(this, &CEEPROMTabController::OnReadEEPROMFromSECU));
 mp_view->setOnWriteEEPROMToSECU(MakeDelegate(this, &CEEPROMTabController::OnWriteEEPROMToSECU));
 mp_view->setOnOpenEEPROMFromFile(MakeDelegate(this, &CEEPROMTabController::OnOpenEEPROMFromFile));
 mp_view->setOnSaveEEPROMToFile(MakeDelegate(this, &CEEPROMTabController::OnSaveEEPROMToFile));
 mp_view->setOnDropFile(MakeDelegate(this, &CEEPROMTabController::OnDropFile));
 mp_view->setOnMapsetNameChanged(MakeDelegate(this, &CEEPROMTabController::OnMapselNameChanged));
 mp_view->setIsEEPROMOpened(MakeDelegate(this, &CEEPROMTabController::IsEEPROMOpened));
 mp_view->setOnShowCEErrors(MakeDelegate(this, &CEEPROMTabController::OnShowCEErrors));
 mp_view->setOnEditOdometer(MakeDelegate(this, &CEEPROMTabController::OnEditOdometer));
 mp_view->setIsLoadGridsAvailable(MakeDelegate(this, &CEEPROMTabController::IsLoadGridsAvailable));
 mp_view->setOnLoadGrids(MakeDelegate(this, &CEEPROMTabController::OnLoadGrids));
 mp_view->setOnResetEeprom(MakeDelegate(this, &CEEPROMTabController::OnResetEeprom));
 
 mp_view->mp_ParamDeskDlg->SetOnTabActivate(MakeDelegate(this, &CEEPROMTabController::OnParamDeskTabActivate));
 mp_view->mp_ParamDeskDlg->SetOnChangeInTab(MakeDelegate(this, &CEEPROMTabController::OnParamDeskChangeInTab));

 mp_view->mp_TablesPanel->setOnMapChanged(MakeDelegate(this, &CEEPROMTabController::OnMapChanged));
 mp_view->mp_TablesPanel->setOnCloseMapWnd(MakeDelegate(this, &CEEPROMTabController::OnCloseMapWnd));
 mp_view->mp_TablesPanel->setOnOpenMapWnd(MakeDelegate(this, &CEEPROMTabController::OnOpenMapWnd));
 mp_view->mp_TablesPanel->setIsAllowed(MakeDelegate(this, &CEEPROMTabController::IsEEPROMOpened));

 mp_view->mp_TablesPanel->setOnChangeSettings(MakeDelegate(this, &CEEPROMTabController::OnChangeSettingsMapEd));

 //register controller!
 TabControllersCommunicator::GetInstance()->AddReference(this, TCC_EEPROM_TAB_CONTROLLER);
}

CEEPROMTabController::~CEEPROMTabController()
{
 delete mp_eedm;
 delete[] mp_bl_data;
}

//изменились настройки программы!
void CEEPROMTabController::OnSettingsChanged(int action)
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

 if (action == 4)
 {
  mp_view->EnableEmbedMapWnd(mp_settings->GetEmbedMapWnd());
  return;
 }

 mp_eedm->SetQuartzFrq(PlatformParamHolder::GetQuartzFreq(mp_settings->GetECUPlatformType()));

 mp_view->mp_TablesPanel->SetITEdMode(mp_settings->GetITEdMode());
 mp_view->mp_TablesPanel->SetActiveVEMap(mp_settings->GetActiveVEMap());

 mp_view->mp_TablesPanel->SetCSVSepSymbol(mp_settings->GetMapCSVSepSymbol());
 mp_view->mp_TablesPanel->SetClassic2DKeys(mp_settings->GetClassic2DKeys());

 //включаем необходимый для данного контекста коммуникационный контроллер
 VERIFY(mp_comm->SwitchOn(CCommunicationManager::OP_ACTIVATE_APPLICATION, true));
}

//from MainTabController
void CEEPROMTabController::OnActivate(void)
{
 mp_comm->m_pBootLoader->EnableBlockedEEPROMOps(mp_settings->GetBldrEEPROMBlocks());
 m_active = true;
 mp_view->mp_TablesPanel->ShowOpenedCharts(true);
 //выбираем ранее выбранную вкладку на панели параметров
 bool result = mp_view->mp_ParamDeskDlg->SetCurSel(m_lastSel);

 if (mp_eedm->IsLoaded())
 {
  bool splitAng = mp_eedm->GetSplitAngMode(0);
  mp_view->mp_TablesPanel->SetSplitAngMode(splitAng);
 }

 mp_comm->m_pAppAdapter->AddEventHandler(this,EHKEY);
 mp_comm->m_pBldAdapter->SetEventHandler(this);
 mp_comm->setOnSettingsChanged(MakeDelegate(this,&CEEPROMTabController::OnSettingsChanged));

 //включаем необходимый для данного контекста коммуникационный контроллер
 VERIFY(mp_comm->SwitchOn(CCommunicationManager::OP_ACTIVATE_APPLICATION));

 m_modification_check_timer.SetTimer(this, &CEEPROMTabController::OnModificationCheckTimer, 250);

 SetViewValues();

 mp_view->EnableMakingChartsChildren(mp_settings->GetChildCharts());
 mp_view->EnableToggleMapWnd(mp_settings->GetToggleMapWnd());
 mp_view->EnableEmbedMapWnd(mp_settings->GetEmbedMapWnd());

 mp_view->mp_TablesPanel->SetITEdMode(mp_settings->GetITEdMode());
 mp_view->mp_ParamDeskDlg->SetFuelDensity(mp_settings->GetFuelDensity1(), mp_settings->GetFuelDensity2());
 mp_view->mp_TablesPanel->SetActiveVEMap(mp_settings->GetActiveVEMap());
 mp_view->mp_TablesPanel->SetCSVSepSymbol(mp_settings->GetMapCSVSepSymbol());
 mp_view->mp_TablesPanel->SetClassic2DKeys(mp_settings->GetClassic2DKeys());

 MapPtMovStep mptms;
 mp_settings->GetMapPtMovStep(mptms);
 for(int i = ETMT_SET_START; i <= ETMT_SET_END; ++i )
  mp_view->mp_TablesPanel->SetPtMovStep(i, mptms.m_value[i]);

 //disable "Load grids" menu item if firmware is not opened
 CFirmwareTabController* p_controller = static_cast<CFirmwareTabController*>
 (TabControllersCommunicator::GetInstance()->GetReference(TCC_FIRMWARE_TAB_CONTROLLER));
 m_firmware_opened = p_controller->IsFirmwareOpened();

 //симулируем изменение состояния для обновления контроллов, так как OnConnection вызывается только если
 //сбрывается или разрывается принудительно (путем деактивации коммуникационного контроллера)
 bool online_status = mp_comm->m_pControlApp->GetOnlineStatus();
 OnConnection(online_status);
}

//from MainTabController
void CEEPROMTabController::OnDeactivate(void)
{
 m_active = false;
 mp_view->mp_TablesPanel->ShowOpenedCharts(false);
 mp_comm->m_pBootLoader->AbortOperation();
 mp_comm->m_pBldAdapter->DetachEventHandler();
 mp_comm->m_pAppAdapter->RemoveEventHandler(EHKEY);
 mp_sbar->SetInformationText(_T(""));
 mp_sbar->ShowProgressBar(false);
 m_modification_check_timer.KillTimer();
 m_waiting_bl_timer.KillTimer();
 //запоминаем номер последней выбранной вкладки на панели параметров
 m_lastSel = mp_view->mp_ParamDeskDlg->GetCurSel();
}

void CEEPROMTabController::OnPacketReceived(const BYTE i_descriptor, SECU3IO::SECU3Packet* ip_packet)
{
 //особый случай: пришел пакет с нотификацонным кодом
 if (i_descriptor == OP_COMP_NC)
 {
  const OPCompNc* p_ndata = (OPCompNc*)ip_packet;
  switch(p_ndata->opcode)
  {
   case OPCODE_CE_SAVE_ERRORS: //Посланные ранее коды ошибок были сохранены в EEPROM
    mp_sbar->SetInformationText(MLL::LoadString(IDS_ERROR_CODES_SAVED_SUCCESSFULLY));
    return;
   case OPCODE_EEPROM_PARAM_SAVE: //параметры были сохранены
    mp_sbar->SetInformationText(MLL::LoadString(IDS_PM_PARAMS_HAS_BEEN_SAVED));
    return;
   case OPCODE_SAVE_TABLSET: //таблицы были сохранены
    mp_sbar->SetInformationText(MLL::LoadString(IDS_PM_TABLSET_HAS_BEEN_SAVED));    
    return;
   case OPCODE_RESET_LTFT: //LTFT table had been reset
    mp_sbar->SetInformationText(MLL::LoadString(IDS_CE_LTFT_HAS_BEEN_RESET));
    return;
   case OPCODE_SAVE_LTFT: //LTFT table had been saved
    mp_sbar->SetInformationText(MLL::LoadString(IDS_CE_LTFT_HAS_BEEN_SAVED));
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
   case SECU3IO::OPCODE_RESET_EEPROM:     //начался процесс сброса EEPROM
    if (p_ndata->opdata == 0x55)
    {
     mp_sbar->SetInformationText(MLL::LoadString(IDS_FW_RESET_EEPROM_STARTED));
     m_clear_sbar_txt_on_conn = true;
    }
    return;
  }
 }
 else if (i_descriptor == INJDRV_PAR)
  mp_comm->m_pControlApp->ChangeContext(SENSOR_DAT);
}

void CEEPROMTabController::OnConnection(const bool i_online)
{
 int state;
 ASSERT(mp_sbar);

 if (i_online) //перешли в онлайн
 {
  state = CStatusBarManager::STATE_ONLINE;

  //в онлайне элементы меню связанные с бутлоадером всегда разрешены
  mp_view->EnableBLItems(true);
  if (m_clear_sbar_txt_on_conn)
  {
   mp_sbar->SetInformationText(_T(""));
   m_clear_sbar_txt_on_conn = false;
  }
 }
 else
 {
  state = CStatusBarManager::STATE_OFFLINE;
 }

 //здесь мы можем только запрещать панели, а разрешать их будем только тогда, когда прочитана конфигурация
 if (i_online==false)
 {
  mp_view->EnableBLItems(false);
 }

 //если бутлоадер активен (выполняется выбранная из меню операция), то будем отображать именно
 //иконку бутлоадера
 if (mp_comm->m_pBootLoader->GetWorkState())
  state = CStatusBarManager::STATE_BOOTLOADER;

 mp_sbar->SetConnectionState(state);
}

//This method called when framework ask to close application
bool CEEPROMTabController::OnClose(void)
{
 //save positions of windows of opened charts
 for(int i = ETMT_SET_START; i <= ETMT_SET_END; ++i)
  OnCloseMapWnd(mp_view->mp_TablesPanel->GetMapWindow(i), i);
 OnCloseMapWnd(mp_view->mp_TablesPanel->GetMapWindow(ETMT_GME_IGN_WND), ETMT_GME_IGN_WND);
 OnCloseMapWnd(mp_view->mp_TablesPanel->GetMapWindow(ETMT_GME_INJ_WND), ETMT_GME_INJ_WND);

 if (m_active && (!mp_comm->m_pBootLoader->IsIdle() || m_waiting_bl_timer.isActive()))
  if (!ErrorMsg::AskUserAboutTabLeaving())
   return false;

 return CheckChangesAskAndSaveEEPROM();
}

void CEEPROMTabController::OnCloseNotify(void)
{
  //empty
}

void CEEPROMTabController::OnFullScreen(bool i_what)
{
 //empty
}

bool CEEPROMTabController::OnAskChangeTab(void)
{
 if (mp_comm->m_pBootLoader->IsIdle() && !m_waiting_bl_timer.isActive())
  return true; //allows
 return ErrorMsg::AskUserAboutTabLeaving();
}

void CEEPROMTabController::OnSaveSettings(void)
{
 //save positions of windows of opened charts
 for(int i = ETMT_SET_START; i <= ETMT_SET_END; ++i)
  OnCloseMapWnd(mp_view->mp_TablesPanel->GetMapWindow(i), i);
 OnCloseMapWnd(mp_view->mp_TablesPanel->GetMapWindow(ETMT_GME_IGN_WND), ETMT_GME_IGN_WND);
 OnCloseMapWnd(mp_view->mp_TablesPanel->GetMapWindow(ETMT_GME_INJ_WND), ETMT_GME_INJ_WND);
}

void CEEPROMTabController::OnOpenEEPROMFromFile(void)
{
 bool result;
 std::vector<BYTE> buff_container(m_epp.m_size, 0);
 BYTE *buff = &buff_container[0];
 _TSTRING opened_file_name = _T("");
 
 if (!mp_comm->m_pBootLoader->IsIdle())
  if (!ErrorMsg::AskUserAboutTabLeaving())
   return;
  
 bool is_continue = CheckChangesAskAndSaveEEPROM();
 if (!is_continue)
  return;  //пользователь передумал

 std::vector<int> sizes;
 sizes.push_back(m_epp.m_size);
 result  = FirmwareFileUtils::LoadEEPROMFromFile(buff, sizes, NULL, &opened_file_name);

 if (!result)
  return;

 //проверка контрольной суммы загружаемых параметров и вывод предупреждения
 if (!mp_eedm->VerifyParamsCheckSum(buff))
 {
  if (IDCANCEL==SECUMessageBox(IDS_FW_EEPROM_DEF_PARAMS_CRC_INVALID, MB_OKCANCEL))
   return; //user canceled
 }

 if (!mp_eedm->VerifyTablesCheckSum(buff))
 {
  if (IDCANCEL==SECUMessageBox(IDS_EE_EEPROM_TABLES_CRC_INVALID, MB_OKCANCEL))
   return; //user canceled
 }

 PrepareOnLoadEEPROM(buff, _TSTRING(opened_file_name));
}

void CEEPROMTabController::OnDropFile(_TSTRING fileName)
{
 bool result;
 std::vector<BYTE> buff_container(m_epp.m_size, 0);
 BYTE *buff = &buff_container[0];
 _TSTRING opened_file_name = _T("");
 
 if (!mp_comm->m_pBootLoader->IsIdle())
  if (!ErrorMsg::AskUserAboutTabLeaving())
   return;
  
 bool is_continue = CheckChangesAskAndSaveEEPROM();
 if (!is_continue)
  return;  //пользователь передумал

 std::vector<int> sizes;
 sizes.push_back(m_epp.m_size);
 result  = FirmwareFileUtils::LoadEEPROMFromFile(buff, sizes, NULL, &opened_file_name, &fileName);

 if (!result)
  return;

 //проверка контрольной суммы загружаемых параметров и вывод предупреждения
 if (!mp_eedm->VerifyParamsCheckSum(buff))
 {
  if (IDCANCEL==SECUMessageBox(IDS_FW_EEPROM_DEF_PARAMS_CRC_INVALID, MB_OKCANCEL))
   return; //user canceled
 }

 if (!mp_eedm->VerifyTablesCheckSum(buff))
 {
  if (IDCANCEL==SECUMessageBox(IDS_EE_EEPROM_TABLES_CRC_INVALID, MB_OKCANCEL))
   return; //user canceled
 }

 PrepareOnLoadEEPROM(buff, _TSTRING(opened_file_name));
}

void CEEPROMTabController::OnSaveEEPROMToFile(void)
{
 std::vector<BYTE> buff_container(m_epp.m_size, 0);
 BYTE *buff = &buff_container[0];
 CString opened_file_name = _T("");

 mp_eedm->StoreBytes(buff);

 //в случае подтверждения пользователя, также будует
 //вычислена контрольная сумма и сохранена в массив с EEPROM
 bool result = FirmwareFileUtils::SaveEEPROMToFile(buff, m_epp.m_size, mp_eedm, &opened_file_name, true);
 if (result)
 {
  //контрольная сумма была сохранена только в массив с EEPROM, которое сохранялось,
  //так как сохранение было подтверждено, то теперь можно обновить и массив с активным EEPROM
  mp_eedm->CalculateAndPlaceParamsCRC();
  mp_eedm->CalculateAndPlaceTablesCRC();

  //данные были успешно сохранены - можно сбрасывать признак модификации
  mp_eedm->ResetModified();

  //после сохранения "Save As" обновляем имя открытого файла
  mp_eedm->SetEEFileName(_TSTRING(opened_file_name));
  mp_view->SetEEFileName(mp_eedm->GetEEFileName());

  //устанавливаем значения только в графики
  SetViewChartsValues();
  mp_view->mp_TablesPanel->TransformValues(); //transform values in some maps before they will be rendered for user
  mp_view->mp_TablesPanel->UpdateOpenedCharts();
 }
}

void CEEPROMTabController::OnReadEEPROMFromSECU(void)
{
 bool is_continue = CheckChangesAskAndSaveEEPROM();
 if (!is_continue)
  return;  //пользователь передумал

 if (!mp_comm->m_pBootLoader->IsIdle())
  return;

 //запускаем бутлоадер по команде из приложения
 StartBootLoader();

 mp_sbar->SetInformationText(MLL::LoadString(IDS_FW_WAITING_BOOTLOADER)); 
 mp_comm->m_pControlApp->SetPacketsTimer(WAITING_BL_START_TIMEOUT);
 m_waiting_bl_timer.SetTimer(WAITING_BL_START_TIMEOUT, true); //one shot
 m_blFinishOpCB = &CEEPROMTabController::finishOnReadEepromFromSECU;
}

void CEEPROMTabController::OnWriteEEPROMToSECU(void)
{
 mp_eedm->StoreBytes(mp_bl_data); //fill m_bl_date with bytes of EEPROM 

 mp_eedm->CalculateAndPlaceParamsCRC(mp_bl_data); //update CRC before sending data to SECU-3
 mp_eedm->CalculateAndPlaceTablesCRC(mp_bl_data);

 ASSERT(mp_comm);

 //запускаем бутлоадер по команде из приложения
 StartBootLoader();

 mp_sbar->SetInformationText(MLL::LoadString(IDS_FW_WAITING_BOOTLOADER)); 
 mp_comm->m_pControlApp->SetPacketsTimer(WAITING_BL_START_TIMEOUT);
 m_waiting_bl_timer.SetTimer(WAITING_BL_START_TIMEOUT, true); //one shot
 m_blFinishOpCB = &CEEPROMTabController::finishOnWriteEepromToSECU;
}

void CEEPROMTabController::PrepareOnLoadEEPROM(const BYTE* i_buff, const _TSTRING& i_file_name)
{
 if (i_buff) //Do we need to load?
  mp_eedm->LoadBytes(i_buff);
 if (i_file_name!=_T(""))
  mp_eedm->SetEEFileName(_TSTRING(i_file_name));
 else
 {
  CString string;
  SYSTEMTIME time;
  GetLocalTime(&time);
  string.Format(_T("EE%02u%02u%02u-%02u%02u%04u.bin"),
    time.wHour,time.wMinute,time.wSecond,time.wDay,time.wMonth,time.wYear);
  mp_eedm->SetEEFileName(_TSTRING(string));
 }

 //enable all tables
 mp_view->mp_TablesPanel->EnableGasdose(true);
 mp_view->mp_TablesPanel->EnableFuelInjection(true);
 mp_view->mp_TablesPanel->EnableCarbAfr(true);
 mp_view->mp_TablesPanel->EnableGasCorr(true);
 mp_view->mp_TablesPanel->EnableChokeOp(true);
 mp_view->mp_ParamDeskDlg->EnableIgnitionCogs(true);
 mp_view->mp_ParamDeskDlg->EnableCKPSItems(true);
 mp_view->mp_ParamDeskDlg->EnableHallWndWidth(true);
 mp_view->mp_ParamDeskDlg->SetMaxCylinders(8);
 mp_view->mp_ParamDeskDlg->EnableOddCylinders(true);
 mp_view->mp_ParamDeskDlg->EnableUseVentPwm(true);
 mp_view->mp_ParamDeskDlg->EnableUseCTSCurveMap(true);
 mp_view->mp_ParamDeskDlg->EnableHallOutputParams(true);
 mp_view->mp_ParamDeskDlg->EnableSECU3TItems(false); //SECU-3i: all functionality
 mp_view->mp_ParamDeskDlg->EnableExtraIO(true);
 mp_view->mp_ParamDeskDlg->EnableInputsMerging(true);
 mp_view->mp_ParamDeskDlg->EnableRisingSpark(true);
 mp_view->mp_ParamDeskDlg->EnableUseCamRef(true);
 mp_view->mp_ParamDeskDlg->EnableCogsBTDC(true);
 mp_view->mp_ParamDeskDlg->EnableFuelInjection(true);
 mp_view->mp_ParamDeskDlg->EnableLambda(true);
 mp_view->mp_ParamDeskDlg->EnableGasdose(true);
 mp_view->mp_ParamDeskDlg->EnableChoke(true);
 mp_view->mp_ParamDeskDlg->EnableChokeCtrls(true);

 bool splitAng = mp_eedm->GetSplitAngMode(0);
 mp_view->mp_TablesPanel->SetSplitAngMode(splitAng);
 
 SetViewValues();
}

bool CEEPROMTabController::CheckChangesAskAndSaveEEPROM(void)
{
 bool modified = mp_eedm->IsModified();
 if (modified && mp_settings->GetSaveWarning())
 {
  int result = SECUMessageBox(MLL::LoadString(IDS_EE_MODIFICATION_WARNING), MB_YESNOCANCEL);
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
   OnSaveEEPROMToFile();
   return true;
  }
 }
 //данные не были изменены - продолжение без вопросов
 return true;
}

//эта функция не обновляет графики, нужно еще вызывать UpdateOpenedCharts()!
void CEEPROMTabController::SetViewChartsValues(void)
{
 //apply load axis's grid settings for all related maps
 SECU3IO::FunSetPar params;
 mp_eedm->GetDefParamValues(FUNSET_PAR, &params);
 mp_view->mp_TablesPanel->SetLoadAxisCfg(params.map_lower_pressure, params.map_upper_pressure, params.load_src_cfg, params.use_load_grid, true);

 for(int i = ETMT_SET_START; i <= ETMT_SET_END; ++i)
 {
  mp_eedm->GetSetMap(0, i, mp_view->mp_TablesPanel->GetMap(i, false), false);
  mp_eedm->GetSetMap(0, i, mp_view->mp_TablesPanel->GetMap(i, true), true);
 }
}


void CEEPROMTabController::SetViewValues(void)
{
 if (mp_eedm->IsLoaded()==false)
  return;

 std::vector<_TSTRING> str = mp_eedm->GetFunctionsSetNames();
 mp_view->SetMapsetName(str[0].c_str());
 SetViewChartsValues();
 mp_view->mp_TablesPanel->TransformValues(); //transform values in some maps before they will be rendered for user
 mp_view->mp_TablesPanel->UpdateOpenedCharts();
 mp_view->SetEEFileName(mp_eedm->GetEEFileName());

 std::vector<_TSTRING> funset_names = m_funset_names;

 if (m_funset_names.empty())
 {
  funset_names.push_back(_T("firmware_set1")); //stub, because stored in the firmware
  funset_names.push_back(_T("firmware_set2")); //stub
  funset_names.push_back(_T("firmware_set3")); //stub
  funset_names.push_back(_T("firmware_set4")); //stub
 }
 funset_names.push_back(str[0]);

 mp_view->mp_ParamDeskDlg->SetFunctionsNames(funset_names);
 BYTE descriptor = mp_view->mp_ParamDeskDlg->GetCurrentDescriptor();
 BYTE paramdata[256];
 mp_eedm->GetDefParamValues(descriptor, paramdata);
 mp_view->mp_ParamDeskDlg->SetValues(descriptor, paramdata);
}

bool CEEPROMTabController::IsEEPROMOpened()
{
 return mp_eedm->IsLoaded();
}

void CEEPROMTabController::OnMapChanged(int i_type)
{
 mp_eedm->SetSetMap(0, i_type, mp_view->mp_TablesPanel->GetMap(i_type, false));
}

void CEEPROMTabController::OnModificationCheckTimer(void)
{
 bool modified = mp_eedm->IsModified();
 mp_view->SetModified(modified);
}

void CEEPROMTabController::OnMapselNameChanged(void)
{
 CString string = mp_view->GetMapsetName();
 mp_eedm->SetFunctionsSetName(0, _TSTRING(string));
}

//from ParamDesk
void CEEPROMTabController::OnParamDeskTabActivate(void)
{
 BYTE descriptor = mp_view->mp_ParamDeskDlg->GetCurrentDescriptor();
 BYTE paramdata[256];
 mp_eedm->GetDefParamValues(descriptor,paramdata);
 mp_view->mp_ParamDeskDlg->SetValues(descriptor,paramdata);
}

//from ParamDesk
void CEEPROMTabController::OnParamDeskChangeInTab(void)
{
 BYTE descriptor = mp_view->mp_ParamDeskDlg->GetCurrentDescriptor();
 BYTE paramdata[256];
 mp_view->mp_ParamDeskDlg->GetValues(descriptor,paramdata);
 mp_eedm->SetDefParamValues(descriptor,paramdata);

 //update load axis's grids in all related maps
 if (descriptor==FUNSET_PAR)
 {
  SECU3IO::FunSetPar &params = (SECU3IO::FunSetPar&)paramdata;
  mp_view->mp_TablesPanel->SetLoadAxisCfg(params.map_lower_pressure, params.map_upper_pressure, params.load_src_cfg, params.use_load_grid, true); //force update
 }
}

void CEEPROMTabController::OnShowCEErrors()
{
 std::set<int> errors = mp_eedm->GetCEErrorsList();
 std::set<int>::const_iterator it = errors.begin();
 CString str; 

 for(; it != errors.end(); ++it)
 {
  const CEErrorIdStr::ErrorsIDContainer errors_ids = mp_errors_ids->Get();
  const CEErrorIdStr::ErrorsIDContainer::const_iterator eii = errors_ids.find(*it);
  if (eii != errors_ids.end())
  {
   size_t found = 0;
   for(size_t i = 0; i < SECU3_CE_ERRCODES_COUNT; ++i)
    if (secu3_ce_error_codes[i].first==eii->first)
     found = i;
   if (found < SECU3_CE_ERRCODES_COUNT)
   {
    TCHAR str_code[10];
    _stprintf(str_code, _T("%d"), secu3_ce_error_codes[found].second);
    str+= str_code;
    str+= _T("  ");
   }
   else
    ASSERT(0);

   str+= eii->second.first.c_str();
   str+= _T("\n");
  }
 }

 if (errors.empty())
 { //no errors
  str = MLL::LoadString(IDS_EE_NO_CE_ERRORS);
  SECUMessageBox(str, MB_OK | MB_ICONINFORMATION);
 }
 else
 { //at least one error is present
  str+= _T("\n\n");
  str+= MLL::LoadString(IDS_EE_MB_RESET_CE_ERRORS);
  int result = SECUMessageBox(str, MB_YESNO | MB_DEFBUTTON2 | MB_ICONINFORMATION);
  if (result == IDYES) 
  {
   mp_eedm->ResetCEErrorsList();
  }
 }
}

bool CEEPROMTabController::StartBootLoader(void)
{
 ASSERT(mp_comm);
 bool result = mp_comm->m_pControlApp->StartBootLoader();
 Sleep(DELAY_AFTER_BL_START); //обязательно нужно подождать не менее 50 мс, иначе будут вылазить посторонние символы при приеме данных от загрузчика
 return result;
}

bool CEEPROMTabController::ExitBootLoader(void)
{
 ASSERT(mp_comm);
 return mp_comm->m_pBootLoader->StartOperation(CBootLoader::BL_OP_EXIT,NULL,0);
}

/////////////////////////////////////////////////////////////////////////////////
void CEEPROMTabController::OnUpdateUI(IBLDEventHandler::poolUpdateUI* ip_data)
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

void CEEPROMTabController::OnBegin(const int opcode,const int status)
{
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

void CEEPROMTabController::OnEnd(const int opcode,const int status)
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
  case CBootLoader::BL_OP_READ_EEPROM:
  {
   if (status==1)
   { //OK
    mp_sbar->SetInformationText(MLL::LoadString(IDS_FW_EEPROM_READ_SUCCESSFULLY));   

    //-----------------------------------------------------------------------------------
    //проверка контрольной суммы загружаемых параметров и вывод предупреждения
    bool chk_sum_status = true;
    if (!mp_eedm->VerifyParamsCheckSum(mp_bl_data))
    {
     if (IDCANCEL==SECUMessageBox(IDS_FW_EEPROM_DEF_PARAMS_CRC_INVALID, MB_OKCANCEL))
      chk_sum_status = false; //user canceled
    }

    if (!mp_eedm->VerifyTablesCheckSum(mp_bl_data))
    {
     if (IDCANCEL==SECUMessageBox(IDS_EE_EEPROM_TABLES_CRC_INVALID, MB_OKCANCEL))
      chk_sum_status = false; //user canceled
    }

    if (chk_sum_status)
     PrepareOnLoadEEPROM(mp_bl_data, _T(""));
    //-----------------------------------------------------------------------------------

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
 }//switch
}

void CEEPROMTabController::finishOnReadEepromFromSECU(void)
{
 //активируем коммуникационный контроллер бутлоадера
 VERIFY(mp_comm->SwitchOn(CCommunicationManager::OP_ACTIVATE_BOOTLOADER));

 //операция не блокирует поток - стековые переменные ей передавать нельзя!
 mp_comm->m_pBootLoader->StartOperation(CBootLoader::BL_OP_READ_EEPROM, mp_bl_data, 0);

 mp_sbar->ShowProgressBar(true);
 mp_sbar->SetProgressPos(0);
}

void CEEPROMTabController::finishOnWriteEepromToSECU(void)
{
 //активируем коммуникационный контроллер бутлоадера
 VERIFY(mp_comm->SwitchOn(CCommunicationManager::OP_ACTIVATE_BOOTLOADER));

 //операция не блокирует поток - стековые переменные ей передавать нельзя!
 mp_comm->m_pBootLoader->StartOperation(CBootLoader::BL_OP_WRITE_EEPROM, mp_bl_data, 0);

 mp_sbar->ShowProgressBar(true);
 mp_sbar->SetProgressPos(0);
}

void CEEPROMTabController::OnChangeSettingsMapEd(void)
{
 mp_settings->SetITEdMode(mp_view->mp_TablesPanel->GetITEdMode());
 mp_settings->SetActiveVEMap(mp_view->mp_TablesPanel->GetActiveVEMap());

 MapPtMovStep mptms;
 mp_settings->GetMapPtMovStep(mptms);
 for(int i = ETMT_SET_START; i <= ETMT_SET_END; ++i)
  mptms.m_value[i] = mp_view->mp_TablesPanel->GetPtMovStep(i);
 mp_settings->SetMapPtMovStep(mptms);
}

bool CEEPROMTabController::IsLoadGridsAvailable(void)
{
 return m_firmware_opened;
}

void CEEPROMTabController::OnLoadGrids(void)
{
 AfxGetMainWnd()->BeginWaitCursor();
 
 CFirmwareTabController* p_controller = static_cast<CFirmwareTabController*>
 (TabControllersCommunicator::GetInstance()->GetReference(TCC_FIRMWARE_TAB_CONTROLLER));
 CFirmwareDataMediator *p_fwdm = p_controller->GetFWDM();

 p_fwdm->GetRPMGridMap(mp_view->mp_TablesPanel->GetRPMGrid());
 p_fwdm->GetCLTGridMap(mp_view->mp_TablesPanel->GetCLTGrid());
 p_fwdm->GetLoadGridMap(mp_view->mp_TablesPanel->GetLoadGrid());

 //apply load axis's grid settings for all related maps
 SECU3IO::FunSetPar params;
 mp_eedm->GetDefParamValues(FUNSET_PAR, &params);
 mp_view->mp_TablesPanel->SetLoadAxisCfg(params.map_lower_pressure, params.map_upper_pressure, params.load_src_cfg, params.use_load_grid, true);

 mp_view->mp_TablesPanel->UpdateOpenedCharts();

 //load functions' set names from firmware
 std::vector<_TSTRING> funset_names = p_fwdm->GetFunctionsSetNames();
 std::vector<_TSTRING> str = mp_eedm->GetFunctionsSetNames();
 m_funset_names = funset_names; //save for further use
 funset_names.push_back(str[0]);
 mp_view->mp_ParamDeskDlg->SetFunctionsNames(funset_names);

 //-----------------------------------------------------------------------------------------------------
 //see also CFirmwareTabController::PrepareOnLoadFLASH()
 DWORD opt = p_fwdm->GetFWOptions();
 Functionality fnc;
 mp_settings->GetFunctionality(fnc);

 //Разрешаем или запрещаем определенные функции в зависимости от опций прошивки
 mp_view->mp_TablesPanel->EnableChokeOp(fnc.SM_CONTROL && CHECKBIT32(opt, SECU3IO::COPT_SM_CONTROL));
 mp_view->mp_TablesPanel->EnableGasdose(fnc.GD_CONTROL && CHECKBIT32(opt, SECU3IO::COPT_GD_CONTROL));
 mp_view->mp_TablesPanel->EnableCarbAfr(CHECKBIT32(opt, SECU3IO::COPT_CARB_AFR));
 mp_view->mp_TablesPanel->EnableFuelInjection(CHECKBIT32(opt, SECU3IO::COPT_FUEL_INJECT));
 mp_view->mp_TablesPanel->EnableGasCorr(!CHECKBIT32(opt, SECU3IO::COPT_SECU3T));

 mp_view->mp_ParamDeskDlg->EnableIgnitionCogs(!CHECKBIT32(opt, SECU3IO::COPT_DWELL_CONTROL) && !CHECKBIT32(opt, SECU3IO::COPT_CKPS_2CHIGN));
 mp_view->mp_ParamDeskDlg->EnableUseVentPwm(CHECKBIT32(opt, SECU3IO::COPT_COOLINGFAN_PWM));
 mp_view->mp_ParamDeskDlg->EnableUseCTSCurveMap(CHECKBIT32(opt, SECU3IO::COPT_THERMISTOR_CS));
 mp_view->mp_ParamDeskDlg->EnableHallOutputParams(CHECKBIT32(opt, SECU3IO::COPT_HALL_OUTPUT) && !CHECKBIT32(opt, SECU3IO::COPT_HALL_SYNC) && !CHECKBIT32(opt, SECU3IO::COPT_CKPS_NPLUS1));
 mp_view->mp_ParamDeskDlg->EnableSECU3TItems(CHECKBIT32(opt, SECU3IO::COPT_SECU3T));
 mp_view->mp_ParamDeskDlg->EnableExtraIO(CHECKBIT32(opt, SECU3IO::COPT_TPIC8101));
 if (CHECKBIT32(opt, SECU3IO::COPT_SECU3T))
  mp_view->mp_ParamDeskDlg->SetMaxCylinders(CHECKBIT32(opt, SECU3IO::COPT_PHASED_IGNITION) ? 8 : 8);
 else //SECU-3i:
  mp_view->mp_ParamDeskDlg->SetMaxCylinders(CHECKBIT32(opt, SECU3IO::COPT_PHASED_IGNITION) ? 8 : 8);

 //in full-sequential ignition mode odd cylinder number engines are also supported,
 //also if hall sensor synchronization is used
 mp_view->mp_ParamDeskDlg->EnableOddCylinders(CHECKBIT32(opt, SECU3IO::COPT_PHASED_IGNITION) || CHECKBIT32(opt, SECU3IO::COPT_HALL_SYNC));
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
 //-----------------------------------------------------------------------------------------------------

 AfxGetMainWnd()->EndWaitCursor();
}

void CEEPROMTabController::OnResetEeprom(void)
{
 if (IDYES==SECUMessageBox(MLL::GetString(IDS_RESET_EEPROM_COMFIRMATION).c_str(), MB_YESNO|MB_DEFBUTTON2|MB_ICONEXCLAMATION))
 {
  SECU3IO::OPCompNc packet_data;
  packet_data.opcode = SECU3IO::OPCODE_RESET_EEPROM;
  packet_data.opdata = 0xAA;
  mp_comm->m_pControlApp->SendPacket(OP_COMP_NC, &packet_data);
 }
}

void CEEPROMTabController::OnEditOdometer(void)
{
 CDynFieldsContainer dfd(mp_view, (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN) ? _T("Редактирование одометра") : _T("Editing odometer"), 200, true);

 float odometer_value = mp_eedm->GetOdometer();
 float consfuel_value = mp_eedm->GetConsFuel();

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Текущий пробег"), _T("км"), 0, 99999.999f, 1, 4, &odometer_value, _T("Текущий пробег сохраненный в памяти. Укажите требуемое значение"));
 else
  dfd.AppendItem(_T("Current mileage"), _T("km"), 0, 99999.999f, 1, 4, &odometer_value, _T("Current mileage stored in memory. Specify required value"));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Израсходовано топлива"), _T("Л"), 0, 9999.999f, 1, 4, &consfuel_value, _T("Объем израсходованного топлива сохраненный в памяти. Укажите требуемое значение"));
 else
  dfd.AppendItem(_T("Fuel consumed"), _T("L"), 0, 9999.999f, 1, 4, &consfuel_value, _T("Volume of consumed fuel stored in memory. Specify required value"));

 if (dfd.DoModal()==IDOK)
 {
  mp_eedm->SetOdometer(odometer_value);
  mp_eedm->SetConsFuel(consfuel_value);
 }
}
