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
#include "MainFrame/StatusBarManager.h"
#include "TabDialogs/EEPROMTabDlg.h"
#include "Settings/ISettingsData.h"
#include "io-core/EEPROMDataMediator.h"
#include "FirmwareFileUtils.h"
#include "TablDesk/ButtonsPanel.h"
#include "TablDesk/MapIds.h"
#include "ParamDesk/Params/ParamDeskDlg.h"
#include "io-core/ce_errors.h"
#include "CEErrorIdStr.h"
#include "ErrorMsg.h"

using namespace fastdelegate;
using namespace SECU3IO;

#define EHKEY _T("EEPROMDataCntr")
#define DELAY_AFTER_BL_START 100

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEEPROMTabController::CEEPROMTabController(CEEPROMTabDlg* i_view, CCommunicationManager* i_comm, CStatusBarManager* i_sbar, ISettingsData* ip_settings)
: m_view(i_view)
, m_comm(i_comm)
, m_sbar(i_sbar)
, mp_settings(ip_settings)
, m_lastSel(0)
, mp_errors_ids(new CEErrorIdStr())
, m_blFinishOpCB(NULL)
, MapWndScrPos(ip_settings)
{
 PlatformParamHolder holder(ip_settings->GetECUPlatformType());
 m_epp = holder.GetEepromParameters();
 m_eedm = new EEPROMDataMediator(holder.GetEepromParameters());
 m_eedm->SetNumPulsesPer1Km(mp_settings->GetNumPulsesPer1Km());
 m_eedm->SetQuartzFrq(PlatformParamHolder::GetQuartzFreq(mp_settings->GetECUPlatformType()));

 ASSERT(m_eedm);

 m_bl_data = new BYTE[m_epp.m_size + 1];
 ASSERT(m_bl_data);

 m_view->setOnReadEEPROMFromSECU(MakeDelegate(this, &CEEPROMTabController::OnReadEEPROMFromSECU));
 m_view->setOnWriteEEPROMToSECU(MakeDelegate(this, &CEEPROMTabController::OnWriteEEPROMToSECU));
 m_view->setOnOpenEEPROMFromFile(MakeDelegate(this, &CEEPROMTabController::OnOpenEEPROMFromFile));
 m_view->setOnSaveEEPROMToFile(MakeDelegate(this, &CEEPROMTabController::OnSaveEEPROMToFile));
 m_view->setOnDropFile(MakeDelegate(this, &CEEPROMTabController::OnDropFile));
 m_view->setOnMapsetNameChanged(MakeDelegate(this, &CEEPROMTabController::OnMapselNameChanged));
 m_view->setIsEEPROMOpened(MakeDelegate(this, &CEEPROMTabController::IsEEPROMOpened));
 m_view->setOnShowCEErrors(MakeDelegate(this, &CEEPROMTabController::OnShowCEErrors));

 m_view->mp_ParamDeskDlg->SetOnTabActivate(MakeDelegate(this, &CEEPROMTabController::OnParamDeskTabActivate));
 m_view->mp_ParamDeskDlg->SetOnChangeInTab(MakeDelegate(this, &CEEPROMTabController::OnParamDeskChangeInTab));

 m_view->mp_TablesPanel->setOnMapChanged(MakeDelegate(this, &CEEPROMTabController::OnMapChanged));
 m_view->mp_TablesPanel->setOnCloseMapWnd(MakeDelegate(this, &CEEPROMTabController::OnCloseMapWnd));
 m_view->mp_TablesPanel->setOnOpenMapWnd(MakeDelegate(this, &CEEPROMTabController::OnOpenMapWnd));
 m_view->mp_TablesPanel->setIsAllowed(MakeDelegate(this, &CEEPROMTabController::IsEEPROMOpened));
}

CEEPROMTabController::~CEEPROMTabController()
{
 delete m_eedm;
 delete m_bl_data;
}

//изменились настройки программы!
void CEEPROMTabController::OnSettingsChanged(int action)
{
 m_eedm->SetNumPulsesPer1Km(mp_settings->GetNumPulsesPer1Km());
 m_eedm->SetQuartzFrq(PlatformParamHolder::GetQuartzFreq(mp_settings->GetECUPlatformType()));

 //включаем необходимый для данного контекста коммуникационный контроллер
 m_comm->SwitchOn(CCommunicationManager::OP_ACTIVATE_APPLICATION, true);
}

//from MainTabController
void CEEPROMTabController::OnActivate(void)
{
 //выбираем ранее выбранную вкладку на панели параметров
 bool result = m_view->mp_ParamDeskDlg->SetCurSel(m_lastSel);

 m_comm->m_pAppAdapter->AddEventHandler(this,EHKEY);
 m_comm->m_pBldAdapter->SetEventHandler(this);
 m_comm->setOnSettingsChanged(MakeDelegate(this,&CEEPROMTabController::OnSettingsChanged));

 //включаем необходимый для данного контекста коммуникационный контроллер
 m_comm->SwitchOn(CCommunicationManager::OP_ACTIVATE_APPLICATION);

 m_modification_check_timer.SetTimer(this, &CEEPROMTabController::OnModificationCheckTimer, 250);

 SetViewValues();

 m_view->EnableMakingChartsChildren(mp_settings->GetChildCharts());

 //симулируем изменение состояния для обновления контроллов, так как OnConnection вызывается только если
 //сбрывается или разрывается принудительно (путем деактивации коммуникационного контроллера)
 bool online_status = m_comm->m_pControlApp->GetOnlineStatus();
 OnConnection(online_status);
}

//from MainTabController
void CEEPROMTabController::OnDeactivate(void)
{
 m_comm->m_pBootLoader->AbortOperation();
 m_comm->m_pBldAdapter->DetachEventHandler();
 m_comm->m_pAppAdapter->RemoveEventHandler(EHKEY);
 m_sbar->SetInformationText(_T(""));
 m_sbar->ShowProgressBar(false);
 m_modification_check_timer.KillTimer();
 //запоминаем номер последней выбранной вкладки на панели параметров
 m_lastSel = m_view->mp_ParamDeskDlg->GetCurSel();
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
    m_sbar->SetInformationText(MLL::LoadString(IDS_ERROR_CODES_SAVED_SUCCESSFULLY));
    return;
   case OPCODE_EEPROM_PARAM_SAVE: //параметры были сохранены
    m_sbar->SetInformationText(MLL::LoadString(IDS_PM_PARAMS_HAS_BEEN_SAVED));
    return;
   case OPCODE_SAVE_TABLSET: //таблицы были сохранены
    m_sbar->SetInformationText(MLL::LoadString(IDS_PM_TABLSET_HAS_BEEN_SAVED));    
    return;
   case SECU3IO::OPCODE_BL_CONFIRM: //confirmation that firmware is going to start boot loader
    if (p_ndata->opdata == 0xBC)
    {
     if (m_blFinishOpCB)
     {
      Sleep(DELAY_AFTER_BL_START);
      (this->*(m_blFinishOpCB))();
      m_blFinishOpCB = NULL;
     }
    }
    return;
  }
 }
}

void CEEPROMTabController::OnConnection(const bool i_online)
{
 int state;
 ASSERT(m_sbar);

 if (i_online) //перешли в онлайн
 {
  state = CStatusBarManager::STATE_ONLINE;

  //в онлайне элементы меню связанные с бутлоадером всегда разрешены
  m_view->EnableBLItems(true);
 }
 else
 {
  state = CStatusBarManager::STATE_OFFLINE;
 }

 //здесь мы можем только запрещать панели, а разрешать их будем только тогда, когда прочитана конфигурация
 if (i_online==false)
 {
  m_view->EnableBLItems(false);
 }

 //если бутлоадер активен (выполняется выбранная из меню операция), то будем отображать именно
 //иконку бутлоадера
 if (m_comm->m_pBootLoader->GetWorkState())
  state = CStatusBarManager::STATE_BOOTLOADER;

 m_sbar->SetConnectionState(state);
}

//This method called when framework ask to close application
bool CEEPROMTabController::OnClose(void)
{
 return true;
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
 if (m_comm->m_pBootLoader->IsIdle())
  return true; //allows
 return ErrorMsg::AskUserAboutTabLeaving();
}

void CEEPROMTabController::OnOpenEEPROMFromFile(void)
{
 bool result;
 std::vector<BYTE> buff_container(m_epp.m_size, 0);
 BYTE *buff = &buff_container[0];
 _TSTRING opened_file_name = _T("");
 
 if (!m_comm->m_pBootLoader->IsIdle())
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
 if (!m_eedm->VerifyParamsCheckSum(buff))
 {
  if (IDCANCEL==AfxMessageBox(IDS_FW_EEPROM_DEF_PARAMS_CRC_INVALID, MB_OKCANCEL))
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
 
 if (!m_comm->m_pBootLoader->IsIdle())
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
 if (!m_eedm->VerifyParamsCheckSum(buff))
 {
  if (IDCANCEL==AfxMessageBox(IDS_FW_EEPROM_DEF_PARAMS_CRC_INVALID, MB_OKCANCEL))
   return; //user canceled
 }

 PrepareOnLoadEEPROM(buff, _TSTRING(opened_file_name));
}

void CEEPROMTabController::OnSaveEEPROMToFile(void)
{
 std::vector<BYTE> buff_container(m_epp.m_size, 0);
 BYTE *buff = &buff_container[0];
 CString opened_file_name = _T("");

 m_eedm->StoreBytes(buff);

 //в случае подтверждения пользователя, также будует
 //вычислена контрольная сумма и сохранена в массив с EEPROM
 bool result = FirmwareFileUtils::SaveEEPROMToFile(buff, m_epp.m_size, m_eedm, &opened_file_name, true);
 if (result)
 {
  //контрольная сумма была сохранена только в массив с EEPROM, которое сохранялось,
  //так как сохранение было подтверждено, то теперь можно обновить и массив с активным EEPROM
  m_eedm->CalculateAndPlaceParamsCRC();

  //данные были успешно сохранены - можно сбрасывать признак модификации
  m_eedm->ResetModified();

  //после сохранения "Save As" обновляем имя открытого файла
  m_eedm->SetEEFileName(_TSTRING(opened_file_name));
  m_view->SetEEFileName(m_eedm->GetEEFileName());

  //устанавливаем значения только в графики
  SetViewChartsValues();
  m_view->mp_TablesPanel->UpdateOpenedCharts();
 }
}

void CEEPROMTabController::OnReadEEPROMFromSECU(void)
{
 bool is_continue = CheckChangesAskAndSaveEEPROM();
 if (!is_continue)
  return;  //пользователь передумал

 if (!m_comm->m_pBootLoader->IsIdle())
  return;

 //запускаем бутлоадер по команде из приложения
 StartBootLoader();

 m_blFinishOpCB = &CEEPROMTabController::finishOnReadEepromFromSECU;
}

void CEEPROMTabController::OnWriteEEPROMToSECU(void)
{
 m_eedm->StoreBytes(m_bl_data); //fill m_bl_date with bytes of EEPROM 

 m_eedm->CalculateAndPlaceParamsCRC(m_bl_data); //update CRC before sending data to SECU-3

 ASSERT(m_comm);

 //запускаем бутлоадер по команде из приложения
 StartBootLoader();

 m_blFinishOpCB = &CEEPROMTabController::finishOnWriteEepromToSECU;
}

void CEEPROMTabController::PrepareOnLoadEEPROM(const BYTE* i_buff, const _TSTRING& i_file_name)
{
 if (i_buff) //Do we need to load?
  m_eedm->LoadBytes(i_buff);
 if (i_file_name!=_T(""))
  m_eedm->SetEEFileName(_TSTRING(i_file_name));
 else
 {
  CString string;
  SYSTEMTIME time;
  GetLocalTime(&time);
  string.Format(_T("EE%02u%02u%02u-%02u%02u%04u.bin"),
    time.wHour,time.wMinute,time.wSecond,time.wDay,time.wMonth,time.wYear);
  m_eedm->SetEEFileName(_TSTRING(string));
 }

 //enable all tables
 m_view->mp_TablesPanel->EnableGasdose(true);
 m_view->mp_TablesPanel->EnableFuelInjection(true);
 m_view->mp_TablesPanel->EnableCarbAfr(true);
 m_view->mp_TablesPanel->EnableGasCorr(true);
 m_view->mp_TablesPanel->EnableChokeOp(true);
 m_view->mp_ParamDeskDlg->EnableIgnitionCogs(true);
 m_view->mp_ParamDeskDlg->EnableCKPSItems(true);
 m_view->mp_ParamDeskDlg->EnableHallWndWidth(true);
 m_view->mp_ParamDeskDlg->SetMaxCylinders(8);
 m_view->mp_ParamDeskDlg->EnableOddCylinders(true);
 m_view->mp_ParamDeskDlg->EnableUseVentPwm(true);
 m_view->mp_ParamDeskDlg->EnableUseCTSCurveMap(true);
 m_view->mp_ParamDeskDlg->EnableHallOutputParams(true);
 m_view->mp_ParamDeskDlg->EnableSECU3TItems(false); //SECU-3i: all functionality
 m_view->mp_ParamDeskDlg->EnableExtraIO(true);
 m_view->mp_ParamDeskDlg->EnableInputsMerging(true);
 m_view->mp_ParamDeskDlg->EnableRisingSpark(true);
 m_view->mp_ParamDeskDlg->EnableUseCamRef(true);
 m_view->mp_ParamDeskDlg->EnableFuelInjection(true);
 m_view->mp_ParamDeskDlg->EnableLambda(true);
 m_view->mp_ParamDeskDlg->EnableGasdose(true);
 m_view->mp_ParamDeskDlg->EnableChoke(true);
 m_view->mp_ParamDeskDlg->EnableChokeCtrls(true);

 SetViewValues();
}

bool CEEPROMTabController::CheckChangesAskAndSaveEEPROM(void)
{
 bool modified = m_eedm->IsModified();
 if (modified && mp_settings->GetSaveWarning())
 {
  int result = AfxMessageBox(MLL::LoadString(IDS_EE_MODIFICATION_WARNING), MB_YESNOCANCEL);
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
 //Can't set RPM grid, because it is stored in the firmware
 //m_fwdm->GetRPMGridMap(m_view->mp_TablesPanel->GetRPMGrid());

 int funset_index = 0;

 m_eedm->GetStartMap(funset_index,m_view->mp_TablesPanel->GetStartMap(false),false);
 m_eedm->GetStartMap(funset_index,m_view->mp_TablesPanel->GetStartMap(true),true);

 m_eedm->GetIdleMap(funset_index,m_view->mp_TablesPanel->GetIdleMap(false),false);
 m_eedm->GetIdleMap(funset_index,m_view->mp_TablesPanel->GetIdleMap(true),true);

 m_eedm->GetWorkMap(funset_index,m_view->mp_TablesPanel->GetWorkMap(false),false);
 m_eedm->GetWorkMap(funset_index,m_view->mp_TablesPanel->GetWorkMap(true),true);

 m_eedm->GetTempMap(funset_index,m_view->mp_TablesPanel->GetTempMap(false),false);
 m_eedm->GetTempMap(funset_index,m_view->mp_TablesPanel->GetTempMap(true),true);

 //fuel injection
 m_eedm->GetVEMap(funset_index,m_view->mp_TablesPanel->GetVEMap(false),false);
 m_eedm->GetVEMap(funset_index,m_view->mp_TablesPanel->GetVEMap(true),true);

 m_eedm->GetAFRMap(funset_index,m_view->mp_TablesPanel->GetAFRMap(false),false);
 m_eedm->GetAFRMap(funset_index,m_view->mp_TablesPanel->GetAFRMap(true),true);

 m_eedm->GetCrnkMap(funset_index,m_view->mp_TablesPanel->GetCrnkMap(false),false);
 m_eedm->GetCrnkMap(funset_index,m_view->mp_TablesPanel->GetCrnkMap(true),true);

 m_eedm->GetWrmpMap(funset_index,m_view->mp_TablesPanel->GetWrmpMap(false),false);
 m_eedm->GetWrmpMap(funset_index,m_view->mp_TablesPanel->GetWrmpMap(true),true);

 m_eedm->GetDeadMap(funset_index,m_view->mp_TablesPanel->GetDeadMap(false),false);
 m_eedm->GetDeadMap(funset_index,m_view->mp_TablesPanel->GetDeadMap(true),true);

 m_eedm->GetIdlrMap(funset_index,m_view->mp_TablesPanel->GetIdlrMap(false),false);
 m_eedm->GetIdlrMap(funset_index,m_view->mp_TablesPanel->GetIdlrMap(true),true);

 m_eedm->GetIdlcMap(funset_index,m_view->mp_TablesPanel->GetIdlcMap(false),false);
 m_eedm->GetIdlcMap(funset_index,m_view->mp_TablesPanel->GetIdlcMap(true),true);

 m_eedm->GetAETPSMap(funset_index,m_view->mp_TablesPanel->GetAETPSMap(false),false);
 m_eedm->GetAETPSMap(funset_index,m_view->mp_TablesPanel->GetAETPSMap(true),true);

 m_eedm->GetAERPMMap(funset_index,m_view->mp_TablesPanel->GetAERPMMap(false),false);
 m_eedm->GetAERPMMap(funset_index,m_view->mp_TablesPanel->GetAERPMMap(true),true);

 m_eedm->GetAftstrMap(funset_index,m_view->mp_TablesPanel->GetAftstrMap(false),false);
 m_eedm->GetAftstrMap(funset_index,m_view->mp_TablesPanel->GetAftstrMap(true),true);

 m_eedm->GetITMap(funset_index,m_view->mp_TablesPanel->GetITMap(false),false);
 m_eedm->GetITMap(funset_index,m_view->mp_TablesPanel->GetITMap(true),true);

 m_eedm->GetITRPMMap(funset_index,m_view->mp_TablesPanel->GetITRPMMap(false),false);
 m_eedm->GetITRPMMap(funset_index,m_view->mp_TablesPanel->GetITRPMMap(true),true);

 m_eedm->GetRigidMap(funset_index,m_view->mp_TablesPanel->GetRigidMap(false),false);
 m_eedm->GetRigidMap(funset_index,m_view->mp_TablesPanel->GetRigidMap(true),true);

 m_eedm->GetEGOCurveMap(funset_index,m_view->mp_TablesPanel->GetEGOCurveMap(false),false);
 m_eedm->GetEGOCurveMap(funset_index,m_view->mp_TablesPanel->GetEGOCurveMap(true),true);

 m_eedm->GetIACCorrMap(funset_index,m_view->mp_TablesPanel->GetIACCMap(false),false);
 m_eedm->GetIACCorrMap(funset_index,m_view->mp_TablesPanel->GetIACCMap(true),true);

 m_eedm->GetIACCorrWMap(funset_index,m_view->mp_TablesPanel->GetIACCWMap(false),false);
 m_eedm->GetIACCorrWMap(funset_index,m_view->mp_TablesPanel->GetIACCWMap(true),true);

 m_eedm->GetIATCLTMap(funset_index,m_view->mp_TablesPanel->GetIATCLTMap(false),false);
 m_eedm->GetIATCLTMap(funset_index,m_view->mp_TablesPanel->GetIATCLTMap(true),true);

 m_eedm->GetTpsswtMap(funset_index,m_view->mp_TablesPanel->GetTpsswtMap(false),false);
 m_eedm->GetTpsswtMap(funset_index,m_view->mp_TablesPanel->GetTpsswtMap(true),true);

 m_eedm->GetGtscMap(funset_index,m_view->mp_TablesPanel->GetGtscMap(false),false);
 m_eedm->GetGtscMap(funset_index,m_view->mp_TablesPanel->GetGtscMap(true),true);

 m_eedm->GetGpscMap(funset_index,m_view->mp_TablesPanel->GetGpscMap(false),false);
 m_eedm->GetGpscMap(funset_index,m_view->mp_TablesPanel->GetGpscMap(true),true);

 m_eedm->GetAtscMap(funset_index,m_view->mp_TablesPanel->GetAtscMap(false),false);
 m_eedm->GetAtscMap(funset_index,m_view->mp_TablesPanel->GetAtscMap(true),true);
}


void CEEPROMTabController::SetViewValues(void)
{
 if (m_eedm->IsLoaded()==false)
  return;

 std::vector<_TSTRING> str = m_eedm->GetFunctionsSetNames();
 m_view->SetMapsetName(str[0].c_str());
 SetViewChartsValues();
 m_view->mp_TablesPanel->UpdateOpenedCharts();
 m_view->SetEEFileName(m_eedm->GetEEFileName());

 std::vector<_TSTRING> funset_names;
 funset_names.push_back(_T("firmware_set1")); //stub, because stored in the firmware
 funset_names.push_back(_T("firmware_set2")); //stub
 funset_names.push_back(_T("firmware_set3")); //stub
 funset_names.push_back(_T("firmware_set4")); //stub
 funset_names.push_back(str[0]);

 m_view->mp_ParamDeskDlg->SetFunctionsNames(funset_names);
 BYTE descriptor = m_view->mp_ParamDeskDlg->GetCurrentDescriptor();
 BYTE paramdata[256];
 m_eedm->GetDefParamValues(descriptor, paramdata);
 m_view->mp_ParamDeskDlg->SetValues(descriptor, paramdata);
}

bool CEEPROMTabController::IsEEPROMOpened()
{
 return m_eedm->IsLoaded();
}

void CEEPROMTabController::OnMapChanged(int i_type)
{
 int funset_index = 0;
 switch(i_type)
 {
   //ignition maps
  case TYPE_MAP_DA_START:
   m_eedm->SetStartMap(funset_index, m_view->mp_TablesPanel->GetStartMap(false));
   break;
  case TYPE_MAP_DA_IDLE:
   m_eedm->SetIdleMap(funset_index, m_view->mp_TablesPanel->GetIdleMap(false));
   break;
  case TYPE_MAP_DA_WORK:
   m_eedm->SetWorkMap(funset_index, m_view->mp_TablesPanel->GetWorkMap(false));
   break;
  case TYPE_MAP_DA_TEMP_CORR:
   m_eedm->SetTempMap(funset_index, m_view->mp_TablesPanel->GetTempMap(false));
   break;
   //fuel injection maps
  case TYPE_MAP_INJ_VE:
   m_eedm->SetVEMap(funset_index, m_view->mp_TablesPanel->GetVEMap(false));
   break;
  case TYPE_MAP_INJ_AFR:
   m_eedm->SetAFRMap(funset_index, m_view->mp_TablesPanel->GetAFRMap(false));
   break;
  case TYPE_MAP_INJ_CRNK:
   m_eedm->SetCrnkMap(funset_index, m_view->mp_TablesPanel->GetCrnkMap(false));
   break;
  case TYPE_MAP_INJ_WRMP:
   m_eedm->SetWrmpMap(funset_index, m_view->mp_TablesPanel->GetWrmpMap(false));
   break;
  case TYPE_MAP_INJ_DEAD:
   m_eedm->SetDeadMap(funset_index, m_view->mp_TablesPanel->GetDeadMap(false));
   break;
  case TYPE_MAP_INJ_IDLR:
   m_eedm->SetIdlrMap(funset_index, m_view->mp_TablesPanel->GetIdlrMap(false));
   break;
  case TYPE_MAP_INJ_IDLC:
   m_eedm->SetIdlcMap(funset_index, m_view->mp_TablesPanel->GetIdlcMap(false));
   break;
  case TYPE_MAP_INJ_AETPS:
   m_eedm->SetAETPSMap(funset_index, m_view->mp_TablesPanel->GetAETPSMap(false));
   break;
  case TYPE_MAP_INJ_AERPM:
   m_eedm->SetAERPMMap(funset_index, m_view->mp_TablesPanel->GetAERPMMap(false));
   break;
  case TYPE_MAP_INJ_AFTSTR:
   m_eedm->SetAftstrMap(funset_index, m_view->mp_TablesPanel->GetAftstrMap(false));
   break;
  case TYPE_MAP_INJ_IT:
   m_eedm->SetITMap(funset_index, m_view->mp_TablesPanel->GetITMap(false));
   break;
  case TYPE_MAP_INJ_ITRPM:
   m_eedm->SetITRPMMap(funset_index, m_view->mp_TablesPanel->GetITRPMMap(false));
   break;
  case TYPE_MAP_INJ_RIGID:
   m_eedm->SetRigidMap(funset_index, m_view->mp_TablesPanel->GetRigidMap(false));
   break;
  case TYPE_MAP_INJ_EGOCRV:
   m_eedm->SetEGOCurveMap(funset_index, m_view->mp_TablesPanel->GetEGOCurveMap(false));
   break;
  case TYPE_MAP_INJ_IACC:
   m_eedm->SetIACCorrMap(funset_index, m_view->mp_TablesPanel->GetIACCMap(false));
   break;
  case TYPE_MAP_INJ_IACCW:
   m_eedm->SetIACCorrWMap(funset_index, m_view->mp_TablesPanel->GetIACCWMap(false));
   break;
  case TYPE_MAP_INJ_IATCLT:
   m_eedm->SetIATCLTMap(funset_index, m_view->mp_TablesPanel->GetIATCLTMap(false));
   break;
  case TYPE_MAP_INJ_TPSSWT:
   m_eedm->SetTpsswtMap(funset_index, m_view->mp_TablesPanel->GetTpsswtMap(false));
   break;
  case TYPE_MAP_INJ_GTSC:
   m_eedm->SetGtscMap(funset_index, m_view->mp_TablesPanel->GetGtscMap(false));
   break;
  case TYPE_MAP_INJ_GPSC:
   m_eedm->SetGpscMap(funset_index, m_view->mp_TablesPanel->GetGpscMap(false));
   break;
  case TYPE_MAP_INJ_ATSC:
   m_eedm->SetAtscMap(funset_index, m_view->mp_TablesPanel->GetAtscMap(false));
   break;
 }
}

void CEEPROMTabController::OnModificationCheckTimer(void)
{
 bool modified = m_eedm->IsModified();
 m_view->SetModified(modified);
}

void CEEPROMTabController::OnMapselNameChanged(void)
{
 CString string = m_view->GetMapsetName();
 m_eedm->SetFunctionsSetName(0, _TSTRING(string));
}

//from ParamDesk
void CEEPROMTabController::OnParamDeskTabActivate(void)
{
 BYTE descriptor = m_view->mp_ParamDeskDlg->GetCurrentDescriptor();
 BYTE paramdata[256];
 m_eedm->GetDefParamValues(descriptor,paramdata);
 m_view->mp_ParamDeskDlg->SetValues(descriptor,paramdata);
}

//from ParamDesk
void CEEPROMTabController::OnParamDeskChangeInTab(void)
{
 BYTE descriptor = m_view->mp_ParamDeskDlg->GetCurrentDescriptor();
 BYTE paramdata[256];
 m_view->mp_ParamDeskDlg->GetValues(descriptor,paramdata);
 m_eedm->SetDefParamValues(descriptor,paramdata);
}

void CEEPROMTabController::OnShowCEErrors()
{
 std::set<int> errors = m_eedm->GetCEErrorsList();
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
  AfxMessageBox(str, MB_OK | MB_ICONINFORMATION);
 }
 else
 { //at least one error is present
  str+= _T("\n\n");
  str+= MLL::LoadString(IDS_EE_MB_RESET_CE_ERRORS);
  int result = AfxMessageBox(str, MB_YESNO | MB_DEFBUTTON2 | MB_ICONINFORMATION);
  if (result == IDYES) 
  {
   m_eedm->ResetCEErrorsList();
  }
 }
}

bool CEEPROMTabController::StartBootLoader(void)
{
 ASSERT(m_comm);
 bool result = m_comm->m_pControlApp->StartBootLoader();
 Sleep(DELAY_AFTER_BL_START); //обязательно нужно подождать не менее 50 мс, иначе будут вылазить посторонние символы при приеме данных от загрузчика
 return result;
}

bool CEEPROMTabController::ExitBootLoader(void)
{
 ASSERT(m_comm);
 return m_comm->m_pBootLoader->StartOperation(CBootLoader::BL_OP_EXIT,NULL,0);
}

/////////////////////////////////////////////////////////////////////////////////
void CEEPROMTabController::OnUpdateUI(IBLDEventHandler::poolUpdateUI* ip_data)
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

void CEEPROMTabController::OnBegin(const int opcode,const int status)
{
 if (opcode == CBootLoader::BL_OP_READ_EEPROM)
  m_sbar->SetInformationText(MLL::LoadString(IDS_FW_READING_EEPROM));
 if (opcode == CBootLoader::BL_OP_WRITE_EEPROM)
  m_sbar->SetInformationText(MLL::LoadString(IDS_FW_WRITING_EEPROM));
 if (opcode == CBootLoader::BL_OP_EXIT)
 {
  //Exiting from boot loader...
 }

 m_view->EnableBLItems(false);
}

void CEEPROMTabController::OnEnd(const int opcode,const int status)
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
  case CBootLoader::BL_OP_READ_EEPROM:
  {
   if (status==1)
   { //OK
    m_sbar->SetInformationText(MLL::LoadString(IDS_FW_EEPROM_READ_SUCCESSFULLY));   

    //-----------------------------------------------------------------------------------
    //проверка контрольной суммы загружаемых параметров и вывод предупреждения
    bool chk_sum_status = true;
    if (!m_eedm->VerifyParamsCheckSum(m_bl_data))
    {
     if (IDCANCEL==AfxMessageBox(IDS_FW_EEPROM_DEF_PARAMS_CRC_INVALID, MB_OKCANCEL))
      chk_sum_status = false; //user canceled
    }

    if (chk_sum_status)
     PrepareOnLoadEEPROM(m_bl_data, _T(""));
    //-----------------------------------------------------------------------------------

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
 }//switch
}

void CEEPROMTabController::finishOnReadEepromFromSECU(void)
{
 //активируем коммуникационный контроллер бутлоадера
 m_comm->SwitchOn(CCommunicationManager::OP_ACTIVATE_BOOTLOADER);

 //операция не блокирует поток - стековые переменные ей передавать нельзя!
 m_comm->m_pBootLoader->StartOperation(CBootLoader::BL_OP_READ_EEPROM,m_bl_data,0);

 m_sbar->ShowProgressBar(true);
 m_sbar->SetProgressPos(0);
}

void CEEPROMTabController::finishOnWriteEepromToSECU(void)
{
 //активируем коммуникационный контроллер бутлоадера
 m_comm->SwitchOn(CCommunicationManager::OP_ACTIVATE_BOOTLOADER);

 //операция не блокирует поток - стековые переменные ей передавать нельзя!
 m_comm->m_pBootLoader->StartOperation(CBootLoader::BL_OP_WRITE_EEPROM,m_bl_data,0);

 m_sbar->ShowProgressBar(true);
 m_sbar->SetProgressPos(0);
}
