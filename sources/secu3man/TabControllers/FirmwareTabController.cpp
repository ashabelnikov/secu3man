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
#include <algorithm>
#include <limits>

#include "Resources/resource.h"
#include "FirmwareTabController.h"

#include "Application/CommunicationManager.h"
#include "common/FastDelegate.h"
#include "FWImpExp/MPSZImpExpController.h"
#include "FWImpExp/SECUImpExpController.h"
#include "FWImpExp/EEPROMImpExpController.h"
#include "FWIORemappingController.h"
#include "HexUtils/readhex.h"
#include "io-core/EEPROMDataMediator.h"
#include "io-core/FirmwareDataMediator.h"
#include "io-core/SECU3IO.h"
#include "io-core/ufcodes.h"
#include "MainFrame/StatusBarManager.h"
#include "ParamDesk/Params/ParamDeskDlg.h"
#include "TabControllersCommunicator.h"
#include "TabDialogs/FirmwareTabDlg.h"
#include "TablDesk/MapIds.h"
#include "TablDesk/TablesSetPanel.h"
#include "Settings/ISettingsData.h"

using namespace fastdelegate;

#undef max   //avoid conflicts with C++

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define EHKEY _T("FirmwareCntr")

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFirmwareTabController::CFirmwareTabController(CFirmwareTabDlg* i_view, CCommunicationManager* i_comm, CStatusBarManager* i_sbar, ISettingsData* ip_settings)
: m_view(i_view)
, m_comm(i_comm)
, m_sbar(i_sbar)
, mp_settings(ip_settings)
, m_current_funset_index(-1)
, m_bl_read_flash_mode(MODE_RD_FLASH_TO_FILE)
, m_lastSel(0)
, m_bl_started_emergency(false)
, mp_iorCntr(new CFWIORemappingController(i_view->mp_IORemappingDlg.get()))
, m_moreSize(0)
{
 PlatformParamHolder holder(ip_settings->GetECUPlatformType());
 m_fpp = holder.GetFlashParameters();
 m_epp = holder.GetEepromParameters();
 m_fwdm = new CFirmwareDataMediator(holder.GetFlashParameters());
 ASSERT(m_fwdm);
 m_edm = new EEPROMDataMediator(holder.GetEepromParameters());
 ASSERT(m_edm);

 m_bl_data = new BYTE[m_fpp.m_total_size + 1];
 ASSERT(m_bl_data);
 m_code_for_merge_with_overhead = new BYTE[m_fpp.m_total_size + 1];
 ASSERT(m_code_for_merge_with_overhead);

 //устанавливаем делегаты (обработчики событий от представления)
 m_view->setOnBootLoaderInfo(MakeDelegate(this,&CFirmwareTabController::OnBootLoaderInfo));
 m_view->setOnReadEepromToFile(MakeDelegate(this,&CFirmwareTabController::OnReadEepromToFile));
 m_view->setOnWriteEepromFromFile(MakeDelegate(this,&CFirmwareTabController::OnWriteEepromFromFile));
 m_view->setOnReadFlashToFile(MakeDelegate(this,&CFirmwareTabController::OnReadFlashToFile));
 m_view->setOnWriteFlashFromFile(MakeDelegate(this,&CFirmwareTabController::OnWriteFlashFromFile));
 m_view->setOnBLStartedEmergency(MakeDelegate(this,&CFirmwareTabController::OnBLStartedEmergency));
 m_view->setOnOpenFlashFromFile(MakeDelegate(this,&CFirmwareTabController::OnOpenFlashFromFile));
 m_view->setOnFWInformationTextChanged(MakeDelegate(this,&CFirmwareTabController::OnFWInformationTextChanged));
 m_view->setOnSaveFlashToFile(MakeDelegate(this,&CFirmwareTabController::OnSaveFlashToFile));
 m_view->setIsFirmwareOpened(MakeDelegate(this,&CFirmwareTabController::IsFirmwareOpened));
 m_view->setOnImportDataFromAnotherFW(MakeDelegate(this,&CFirmwareTabController::OnImportDataFromAnotherFW));
 m_view->setOnImportDataFromSECU3(MakeDelegate(this,&CFirmwareTabController::OnImportDataFromSECU3));
 m_view->setOnReadFlashFromSECU(MakeDelegate(this,&CFirmwareTabController::OnReadFlashFromSECU));
 m_view->setOnWriteFlashToSECU(MakeDelegate(this,&CFirmwareTabController::OnWriteFlashToSECU));
 m_view->setOnImportMapsFromMPSZ(MakeDelegate(this,&CFirmwareTabController::OnImportMapsFromMPSZ));
 m_view->setOnImportMapsFromSECU3(MakeDelegate(this,&CFirmwareTabController::OnImportMapsFromSECU3));
 m_view->setOnImportDefParamsFromEEPROMFile(MakeDelegate(this, &CFirmwareTabController::OnImportDefParamsFromEEPROMFile));
 m_view->setOnImportTablesFromEEPROMFile(MakeDelegate(this, &CFirmwareTabController::OnImportTablesFromEEPROMFile));
 m_view->setOnExportMapsToMPSZ(MakeDelegate(this,&CFirmwareTabController::OnExportMapsToMPSZ));
 m_view->setOnExportMapsToSECU3(MakeDelegate(this,&CFirmwareTabController::OnExportMapsToSECU3));
 m_view->setOnFirmwareInfo(MakeDelegate(this,&CFirmwareTabController::OnFirmwareInfo));
 m_view->setOnViewFWOptions(MakeDelegate(this,&CFirmwareTabController::OnViewFWOptions));
 m_view->setIsViewFWOptionsAvailable(MakeDelegate(this, &CFirmwareTabController::IsViewFWOptionsAvailable)); 
 m_view->setIsIORemappingAvailable(MakeDelegate(this, &CFirmwareTabController::IsIORemappingAvailable)); 

 m_view->mp_TablesPanel->setOnMapChanged(MakeDelegate(this,&CFirmwareTabController::OnMapChanged));
 m_view->mp_TablesPanel->setOnFunSetSelectionChanged(MakeDelegate(this,&CFirmwareTabController::OnFunSetSelectionChanged));
 m_view->mp_TablesPanel->setOnFunSetNamechanged(MakeDelegate(this,&CFirmwareTabController::OnFunSetNamechanged));
 m_view->mp_TablesPanel->setOnCloseMapWnd(MakeDelegate(this, &CFirmwareTabController::OnCloseMapWnd));
 m_view->mp_TablesPanel->setOnOpenMapWnd(MakeDelegate(this, &CFirmwareTabController::OnOpenMapWnd));
 m_view->mp_TablesPanel->setIsAllowed(MakeDelegate(this,&CFirmwareTabController::IsFirmwareOpened));

 m_view->mp_ParamDeskDlg->SetOnTabActivate(MakeDelegate(this,&CFirmwareTabController::OnParamDeskTabActivate));
 m_view->mp_ParamDeskDlg->SetOnChangeInTab(MakeDelegate(this,&CFirmwareTabController::OnParamDeskChangeInTab));

 //register controller!
 TabControllersCommunicator::GetInstance()->AddReference(this, TCC_FIRMWARE_TAB_CONTROLLER);
}

CFirmwareTabController::~CFirmwareTabController()
{
 delete m_fwdm;
 delete m_edm;
 delete m_bl_data;
 delete m_code_for_merge_with_overhead;
}

//изменились настройки
void CFirmwareTabController::OnSettingsChanged(void)
{
 //включаем необходимый для данного контекста коммуникационный контроллер
 m_comm->SwitchOn(CCommunicationManager::OP_ACTIVATE_APPLICATION, true);
}

void CFirmwareTabController::OnActivate(void)
{
 //выбираем ранее выбранную вкладку на панели параметров
 bool result = m_view->mp_ParamDeskDlg->SetCurSel(m_lastSel);

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

 //симулируем изменение состояния для обновления контроллов, так как OnConnection вызывается только если
 //сбрывается или разрывается принудительно (путем деактивации коммуникационного контроллера)
 bool online_status = m_comm->m_pControlApp->GetOnlineStatus();
 OnConnection(online_status);
}

void CFirmwareTabController::OnDeactivate(void)
{
 //отключаемся от потока данных
 m_comm->m_pAppAdapter->RemoveEventHandler(EHKEY);
 m_sbar->SetInformationText(_T(""));
 m_modification_check_timer.KillTimer();
 //запоминаем номер последней выбранной вкладки на панели параметров
 m_lastSel = m_view->mp_ParamDeskDlg->GetCurSel();

 //Deactivate children controllers
 mp_iorCntr->OnDeactivate();
}

/////////////////////////////////////////////////////////////////////////////////
void CFirmwareTabController::OnPacketReceived(const BYTE i_descriptor, SECU3IO::SECU3Packet* ip_packet)
{
 if (i_descriptor == FWINFO_DAT)
 { //приняли пакет с сигнатурной информацией о прошивке
  SECU3IO::FWInfoDat* p_packet = (SECU3IO::FWInfoDat*)ip_packet;
  TCHAR string[256];
  OemToChar(p_packet->info, string);
  m_sbar->SetInformationText(string);

  //display firmware options if present
  _ShowFWOptions(string, p_packet->options);
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
  } 
 }
}

void CFirmwareTabController::OnConnection(const bool i_online)
{
 int state;
 ASSERT(m_sbar);

 if (i_online) //перешли в онлайн
 {
  state = CStatusBarManager::STATE_ONLINE;
  m_view->EnableBLStartedEmergency(false);

  //в онлайне элементы меню связанные с бутлоадером всегда разрешены
  m_view->EnableBLItems(true);

  //если мы перешли в онлайн, то убираем чекбокс - пользователь нас обманывает:
  //он указал что бутлоадер запущен аварийно и в тоже время запущен Application
  m_view->SetBLStartedEmergency(false);

  m_view->EnableAppItems(true);
 }
 else
 { //перешли в оффлайн
  if (!m_comm->m_pBootLoader->GetWorkState()) //разрешаем чекбокс только если мы в оффлайне сразу после онлайна
   m_view->EnableBLStartedEmergency(true);

  //в оффлайне состояние элементов меню связанных с бутлоадером зависит от состояния чекбокса
  bool enable = m_view->IsBLStartedEmergency();
  m_view->EnableBLItems(enable);

  if (enable)
   state = CStatusBarManager::STATE_BOOTLOADER; //чтобы иконка бутлоадера не пропадала после завершения операции
  else
   state = CStatusBarManager::STATE_OFFLINE;

  m_view->EnableAppItems(false);
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

 m_view->EnableBLItems(false);
 m_view->EnableBLStartedEmergency(false);
}

CString CFirmwareTabController::GenerateErrorStr(void)
{
 switch(m_comm->m_pBootLoader->GetLastError())
 {
  case CBootLoader::BL_ERROR_NOANSWER:
   return MLL::LoadString(IDS_BL_ERROR_NOANSWER);
  case CBootLoader::BL_ERROR_CHKSUM:
   return MLL::LoadString(IDS_BL_ERROR_CRC);
  case CBootLoader::BL_ERROR_WRONG_DATA:
   return MLL::LoadString(IDS_BL_ERROR_WRONG_DATA);
 }
 ASSERT(0); //что за ошибка?
 return CString(_T(""));
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
    m_sbar->SetInformationText(GenerateErrorStr());
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
    SaveEEPROMToFile(m_bl_data, m_epp.m_size);
   }
   else
   {
    m_sbar->SetInformationText(GenerateErrorStr());
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
    m_sbar->SetInformationText(GenerateErrorStr());
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
     SaveFLASHToFile(m_bl_data, m_fpp.m_total_size);
    }
    else if (m_bl_read_flash_mode == MODE_RD_FLASH_TO_BUFF_FOR_LOAD)
    {
     if (_CheckCompatibilityAndAskUser(m_bl_data))
      PrepareOnLoadFLASH(m_bl_data, _T(""));
    }
    else if (m_bl_read_flash_mode == MODE_RD_FLASH_FOR_IMPORT_DATA)
    {
     if (_CheckCompatibilityAndAskUser(m_bl_data))
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
    m_sbar->SetInformationText(GenerateErrorStr());
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
    m_sbar->SetInformationText(GenerateErrorStr());
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
  m_view->EnableBLItems(true);
  m_view->EnableBLStartedEmergency(true);

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

 //активируем коммуникационный контроллер бутлоадера
 m_comm->SwitchOn(CCommunicationManager::OP_ACTIVATE_BOOTLOADER);

 //операция не блокирует поток - стековые переменные ей передавать нельзя!
 m_comm->m_pBootLoader->StartOperation(CBootLoader::BL_OP_READ_SIGNATURE,m_bl_data,0);

 m_sbar->ShowProgressBar(true);
 m_sbar->SetProgressPos(0);
}

void CFirmwareTabController::OnReadEepromToFile(void)
{
 if (!m_comm->m_pBootLoader->IsIdle())
  return;

 //запускаем бутлоадер по команде из приложения
 StartBootLoader();

 //активируем коммуникационный контроллер бутлоадера
 m_comm->SwitchOn(CCommunicationManager::OP_ACTIVATE_BOOTLOADER);

 //операция не блокирует поток - стековые переменные ей передавать нельзя!
 m_comm->m_pBootLoader->StartOperation(CBootLoader::BL_OP_READ_EEPROM,m_bl_data,0);

 m_sbar->ShowProgressBar(true);
 m_sbar->SetProgressPos(0);
}

void CFirmwareTabController::OnWriteEepromFromFile(void)
{
 std::vector<int> sizes;
 sizes.push_back(m_epp.m_size);
 bool result = LoadEEPROMFromFile(m_bl_data, sizes);

 if (!result)
  return; //cancel

 ASSERT(m_comm);

 //запускаем бутлоадер по команде из приложения
 StartBootLoader();

 //активируем коммуникационный контроллер бутлоадера
 m_comm->SwitchOn(CCommunicationManager::OP_ACTIVATE_BOOTLOADER);

 //операция не блокирует поток - стековые переменные ей передавать нельзя!
 m_comm->m_pBootLoader->StartOperation(CBootLoader::BL_OP_WRITE_EEPROM,m_bl_data,0);

 m_sbar->ShowProgressBar(true);
 m_sbar->SetProgressPos(0);
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

 //активируем коммуникационный контроллер бутлоадера
 m_comm->SwitchOn(CCommunicationManager::OP_ACTIVATE_BOOTLOADER);

 //операция не блокирует поток - стековые переменные ей передавать нельзя!
 m_comm->m_pBootLoader->StartOperation(CBootLoader::BL_OP_READ_FLASH,m_bl_data, m_fpp.m_total_size);

 m_sbar->ShowProgressBar(true);
 m_sbar->SetProgressPos(0);
}

bool CFirmwareTabController::_CheckCompatibilityAndAskUser(BYTE* i_buff, const PlatformParamHolder* p_pph /*=NULL*/)
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

void CFirmwareTabController::OnWriteFlashFromFile(void)
{
 std::vector<int> sizes;
 sizes.push_back(m_fpp.m_total_size);
 bool result = LoadFLASHFromFile(m_bl_data, sizes);

 if (!result)
  return; //cancel

 StartWritingOfFLASHFromBuff(m_bl_data);
}

void CFirmwareTabController::StartWritingOfFLASHFromBuff(BYTE* io_buff)
{
 //вычисляем контрольную сумму и сохраняем ее в массив с прошивкой. Это необходимо например когда
 //мы записываем свеже скомпилированную прошивку, которая может не содержать контрольной суммы
 m_fwdm->CalculateAndPlaceFirmwareCRC(io_buff);

 ASSERT(m_comm);

 //запускаем бутлоадер по команде из приложения (если нет флага что он запущен вручную)
 StartBootLoader();

 //активируем коммуникационный контроллер бутлоадера
 m_comm->SwitchOn(CCommunicationManager::OP_ACTIVATE_BOOTLOADER);

 //Если установлен режим прошивки только кода (без данных), то все несколько сложнее
 if (m_view->IsProgrammeOnlyCode())
 {
  //Мы программируем только код, одако контрольная сумма останется посчитаной для старых данных. Поэтому нам необходимо
  //прочитать данные, обединить их с новым кодом, обновить контрольную сумму и только потом программировать.
  m_bl_read_flash_mode = MODE_RD_FLASH_TO_BUFF_MERGE_DATA;

  //сохраняем данные для того, чтобы позже объединить их с прочитанными "верхними" данными
  m_code_for_merge_size = m_fwdm->GetOnlyCodeSize(io_buff);
  memcpy(m_code_for_merge_with_overhead, io_buff, m_fpp.m_app_section_size);

  m_moreSize = (m_fwdm->HasCodeData(io_buff) ? 0x400 : 0); //1024 bytes more

  //Читаем немного больше байт, для того, чтобы гарантировано прочитать данные находящиеся в коде  
  size_t reducedSize = m_code_for_merge_size - m_moreSize;
  //операция не блокирует поток - стековые переменные ей передавать нельзя!
  m_comm->m_pBootLoader->StartOperation(CBootLoader::BL_OP_READ_FLASH, m_bl_data,
  m_fpp.m_app_section_size - reducedSize, //размер данных сверху над кодом программы
  reducedSize);                           //адрес начала "верхних" данных
 }
 else
 {//все очень просто
  m_comm->m_pBootLoader->StartOperation(CBootLoader::BL_OP_WRITE_FLASH,io_buff, m_fpp.m_app_section_size);
 }

 m_sbar->ShowProgressBar(true);
 m_sbar->SetProgressPos(0);
}

void CFirmwareTabController::SaveEEPROMToFile(const BYTE* p_data, const int size)
{
 HANDLE   hFile=0;
 static TCHAR BASED_CODE szFilter[] = _T("BIN Files (*.bin)|*.bin|All Files (*.*)|*.*||");
 CFileDialog save(FALSE,NULL,NULL,NULL,szFilter,NULL);
 save.m_ofn.lpstrDefExt = _T("BIN");
 if (save.DoModal()==IDOK)
 {
  CFile f;
  CFileException ex;
  TCHAR szError[1024];
  if(!f.Open(save.GetFileName(),CFile::modeWrite|CFile::modeCreate,&ex))
  {
   ex.GetErrorMessage(szError, 1024);
   AfxMessageBox(szError);
   return;
  }
  f.Write(p_data,size);
  f.Close();
  return;
 }
 else
  return;
}

bool CFirmwareTabController::SaveFLASHToFile(const BYTE* p_data, const int size, CString* o_file_name /* = NULL*/, bool calculate_and_place_crc16/* = false*/)
{
 HANDLE   hFile=0;
 BYTE *save_buff = NULL;

 static TCHAR BASED_CODE szFilter[] = _T("BIN Files (*.bin)|*.bin|All Files (*.*)|*.*||");
 CFileDialog save(FALSE,NULL,NULL,NULL,szFilter,NULL);
 save.m_ofn.lpstrDefExt = _T("BIN");
 if (save.DoModal()==IDOK)
 {
  CFile f;
  CFileException ex;
  TCHAR szError[1024];
  if(!f.Open(save.GetFileName(),CFile::modeWrite|CFile::modeCreate,&ex))
  {
   ex.GetErrorMessage(szError, 1024);
   AfxMessageBox(szError);
   return false; //ошибка - данные не сохранены
  }

  save_buff = new BYTE[size];
  memcpy(save_buff, p_data,size);

  //вычисляем контрольную сумму и сохраняем ее в массив с прошивкой
  if (calculate_and_place_crc16)
   m_fwdm->CalculateAndPlaceFirmwareCRC(save_buff);

  f.Write(save_buff,size);
  f.Close();
  delete save_buff;

  if (o_file_name!=NULL)
   *o_file_name = save.GetFileName();
  return true; //подтверждение пользователя
 }
 else
  return false; //отказ пользователя
}


namespace {
   struct GenMessage
   {
    CString operator()(const std::vector<int>& sizes, UINT msgRegId)
    {
     CString string;
     CString size_str;
     for(size_t i = 0; i < sizes.size(); ++i)
     {
      if ((sizes.size() - 1 == i) && (sizes.size() > 1))
       size_str = size_str + _T(" ") + MLL::LoadString(IDS_FW_OR) + _T(" ");
      else
       if (0 != i)
        size_str+=(", ");
      CString value;
      value.Format(_T("%d"), sizes[i]);
      size_str+=(value);
     }
     string.Format(MLL::LoadString(msgRegId), size_str);
     return string;
    }
   }; }

//мы заранее знаем размер файла с EEPROM
bool CFirmwareTabController::LoadEEPROMFromFile(BYTE* p_data, const std::vector<int>& sizes, int* o_selected_size /*= NULL*/, _TSTRING* o_file_name /*= NULL*/)
{
 HANDLE hFile = 0;
 static TCHAR BASED_CODE szFilter[] = _T("BIN Files (*.bin)|*.bin|All Files (*.*)|*.*||");
 CFileDialog open(TRUE,NULL,NULL,NULL,szFilter,NULL);
 CString cs;

 if (sizes.empty())
  return false; //error, at least one size must be specified
 std::vector<int>::const_iterator p_size_max = std::max_element(sizes.begin(), sizes.end());

 if (open.DoModal()==IDOK)
 {
  CFile f;
  CFileException ex;
  TCHAR szError[1024];
  if(!f.Open(open.GetFileName(), CFile::modeRead, &ex))
  {
   ex.GetErrorMessage(szError, 1024);
   AfxMessageBox(szError);
   return false; //error, can't open file
  }

  //Проверка на размер файла (его размер должен соответствовать одному из разрешенных размеров EEPROM)
  std::vector<int>::const_iterator p_size = std::find(sizes.begin(), sizes.end(), f.GetLength());
  if (p_size==sizes.end())
  {
   AfxMessageBox(GenMessage()(sizes, IDS_FW_WRONG_EE_FILE_SIZE));
   f.Close();
   return false; //ошибка
  }

  f.Read(p_data, *p_size);
  f.Close();

  if (NULL != o_selected_size)
   *o_selected_size = *p_size; //save selected size

  if (NULL != o_file_name)
   *o_file_name = open.GetFileName();

  return true; //подтверждение пользователя
 }
 else
  return false; //отказ пользователя
}

//мы заранее знаем размер файла с FLASH
//p_data - буфер для чтения данных. Должен быть не меньше чем 64кБ
//size  - размер данных для чтения
//o_file_name - указатель на строку в которую будет сохранено имя файла
bool CFirmwareTabController::LoadFLASHFromFile(BYTE* p_data, const std::vector<int>& sizes, _TSTRING* i_title /*= NULL*/, int* o_selected_size /*= NULL*/, _TSTRING* o_file_name /*= NULL*/, _TSTRING* o_file_path /*= NULL*/)
{
 HANDLE   hFile=0;
 static TCHAR BASED_CODE szFilter[] = _T("BIN Files (*.bin)|*.bin|HEX Files (*.hex;*.a90)|*.hex;*.a90|All Files (*.*)|*.*||");
 CFileDialog open(TRUE,NULL,NULL,NULL,szFilter,NULL);
 if (i_title)
  open.m_ofn.lpstrTitle = i_title->c_str();
 CString cs;

 if (sizes.empty())
  return false; //error
 std::vector<int>::const_iterator p_size_max = std::max_element(sizes.begin(), sizes.end());

 if (open.DoModal()==IDOK)
 {
  CFile f;
  CFileException ex;
  TCHAR szError[1024];
  if(!f.Open(open.GetFileName(),CFile::modeRead,&ex))
  {
   ex.GetErrorMessage(szError, 1024);
   AfxMessageBox(szError);
   return false; //ошибка
  }

  //----------------------------------------------------------------------------
  if (open.GetFileExt()==_T("hex") || open.GetFileExt()==_T("a90"))
  {
   ULONGLONG ulonglong_size = f.GetLength();
   if (ulonglong_size > 524288)
   {
    AfxMessageBox(MLL::LoadString(IDS_FW_FILE_IS_TOO_BIG));
    f.Close();
    return false; //ошибка
   }

   int hex_file_length = static_cast<int>(ulonglong_size);
   BYTE* p_hex_buff = new BYTE[hex_file_length];
   f.Read(p_hex_buff, hex_file_length);
   size_t bin_size = 0;
   EReadHexStatus status = HexUtils_ConvertHexToBin(p_hex_buff, hex_file_length, p_data, bin_size, *p_size_max);
   delete p_hex_buff;

   switch(status)
   {
    case RH_INCORRECT_CHKSUM:
     AfxMessageBox(MLL::LoadString(IDS_FW_HEX_FILE_CRC_ERROR));
     f.Close();
     return false; //ошибка

    default:
     case RH_UNEXPECTED_SYMBOL:
     AfxMessageBox(MLL::LoadString(IDS_FW_HEX_FILE_STRUCTURE_ERROR));
     f.Close();
     return false; //ошибка

    case RH_ADDRESS_EXCEDED:
     break;

    case RH_SUCCESS:
     break;
   }

   //find appropriate size and check
   std::vector<int>::const_iterator p_size = std::find(sizes.begin(), sizes.end(), bin_size);
   if ((p_size==sizes.end()) || (status == RH_ADDRESS_EXCEDED))
   {
    AfxMessageBox(GenMessage()(sizes, IDS_FW_WRONG_FW_FILE_SIZE));
    f.Close();
    return false; //ошибка
   }

   if (NULL != o_selected_size)
    *o_selected_size = *p_size; //save selected size
  }
  else //если у файла расширение bin или нет расширения или оно другое, то по умолчанию bin
  {
   std::vector<int>::const_iterator p_size = std::find(sizes.begin(), sizes.end(), f.GetLength());
   if (p_size==sizes.end())
   {
    AfxMessageBox(GenMessage()(sizes, IDS_FW_WRONG_FW_FILE_SIZE));
    f.Close();
    return false; //ошибка
   }
   f.Read(p_data, *p_size);

   if (NULL != o_selected_size)
    *o_selected_size = *p_size; //save selected size
  }
  //----------------------------------------------------------------------------

  f.Close();
  if (NULL != o_file_name)
   *o_file_name = open.GetFileName();

  if (NULL != o_file_path)
   *o_file_path = open.GetPathName();

  return true; //подтверждение пользователя
 }
 else
  return false; //отказ пользователя
}

//от чекбокса...
void CFirmwareTabController::OnBLStartedEmergency(void)
{
 if (m_comm->m_pControlApp->GetOnlineStatus()==true)
  return;

 //если оффлайн, то состояние элементов меню зависит от состояния чекбокса
 bool emergency = m_view->IsBLStartedEmergency();
 m_view->EnableBLItems(emergency);

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
 return m_view->IsBLStartedEmergency() && m_view->IsBLStartedEmergencyEnabled();
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
  Sleep(55); //обязательно нужно подождать не менее 50 мс, иначе будут вылазить посторонние символы при приеме данных от загрузчика
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
 if (modified)
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
 OnCloseMapWnd(m_view->mp_TablesPanel->GetMapWindow(TYPE_MAP_DA_START), TYPE_MAP_DA_START);
 OnCloseMapWnd(m_view->mp_TablesPanel->GetMapWindow(TYPE_MAP_DA_IDLE),  TYPE_MAP_DA_IDLE);
 OnCloseMapWnd(m_view->mp_TablesPanel->GetMapWindow(TYPE_MAP_DA_WORK),  TYPE_MAP_DA_WORK);
 OnCloseMapWnd(m_view->mp_TablesPanel->GetMapWindow(TYPE_MAP_DA_TEMP_CORR), TYPE_MAP_DA_TEMP_CORR);
 OnCloseMapWnd(m_view->mp_TablesPanel->GetMapWindow(TYPE_MAP_ATTENUATOR), TYPE_MAP_ATTENUATOR);
 OnCloseMapWnd(m_view->mp_TablesPanel->GetMapWindow(TYPE_MAP_DWELLCNTRL), TYPE_MAP_DWELLCNTRL);

 return CheckChangesAskAndSaveFirmware();
}

bool CFirmwareTabController::OnAskFullScreen(void)
{
 return false;
}

void CFirmwareTabController::OnFullScreen(bool i_what, const CRect& i_rect)
{
 //na
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

 //Разрешаем или запрещаем определенные функции в зависимости от опций прошивки
 m_view->mp_TablesPanel->EnableDwellControl((m_fwdm->GetFWOptions() & (1 << SECU3IO::COPT_DWELL_CONTROL)) > 0);
 m_view->mp_ParamDeskDlg->EnableIgnitionCogs(!(m_fwdm->GetFWOptions() & (1 << SECU3IO::COPT_DWELL_CONTROL)));
 m_view->mp_ParamDeskDlg->EnableUseVentPwm((m_fwdm->GetFWOptions() & (1 << SECU3IO::COPT_COOLINGFAN_PWM)) > 0);
 m_view->mp_ParamDeskDlg->EnableHallOutputParams((m_fwdm->GetFWOptions() & (1 << SECU3IO::COPT_HALL_OUTPUT)) > 0);
 m_view->mp_ParamDeskDlg->SetCrankType(((m_fwdm->GetFWOptions() & (1 << SECU3IO::COPT_WHEEL_36_1)) > 0) ? SECU3IO::COPT_WHEEL_36_1 : -1);
 if ((m_fwdm->GetFWOptions() & (1 << SECU3IO::COPT_SECU3T)))
  //SECU-3T has two additional ignition outputs
  m_view->mp_ParamDeskDlg->SetMaxCylinders((m_fwdm->GetFWOptions() & (1 << SECU3IO::COPT_PHASED_IGNITION)) > 0 ? 6 : 8);
 else
  m_view->mp_ParamDeskDlg->SetMaxCylinders((m_fwdm->GetFWOptions() & (1 << SECU3IO::COPT_PHASED_IGNITION)) > 0 ? 4 : 8);
 this->mp_iorCntr->EnableSECU3TFeatures((m_fwdm->GetFWOptions() & (1 << SECU3IO::COPT_SECU3T)) > 0);
 this->mp_iorCntr->Enable(m_fwdm->HasCodeData());

 SetViewFirmwareValues();
}

void CFirmwareTabController::OnOpenFlashFromFile(void)
{
 bool result;
 std::vector<BYTE> buff_container(m_fpp.m_total_size, 0);
 BYTE *buff = &buff_container[0];
 _TSTRING opened_file_name = _T("");

 bool is_continue = CheckChangesAskAndSaveFirmware();
 if (!is_continue)
  return;  //пользователь передумал

 //!!! без вычисления и записи контрольной суммы в буфер
 std::vector<int> sizes;
 sizes.push_back(m_fpp.m_total_size);
 result  = LoadFLASHFromFile(buff, sizes, NULL, NULL, &opened_file_name);
 if (result && _CheckCompatibilityAndAskUser(buff)) //user OK?
 {
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
 bool result = SaveFLASHToFile(buff, m_fpp.m_total_size, &opened_file_name,true);
 if (result)
 {
  //контрольная сумма была сохранена только вмассив с прошивкий которая сохранялась,
  //так как сохранение было подтверждено, то теперь можно обновить и массив с активной прошивкой
  m_fwdm->CalculateAndPlaceFirmwareCRC();

  //данные были успешно сохранены - можно сбрасывать признак модификации
  m_fwdm->ResetModified();

  //после сохранения "Save As" обновляем имя открытого файла
  m_fwdm->SetFWFileName(_TSTRING(opened_file_name));
  m_view->SetFirmwareName(m_fwdm->GetFWFileName());

  //устанавливаем значения только в графики
  SetViewChartsValues();
  m_view->mp_TablesPanel->UpdateOpenedCharts();

  m_view->SetFirmwareCRCs(m_fwdm->GetCRC16StoredInActiveFirmware(),m_fwdm->CalculateCRC16OfActiveFirmware());
 }
}

void CFirmwareTabController::OnFWInformationTextChanged(void)
{
 CString string = m_view->GetFWInformationText();
 m_fwdm->SetSignatureInfo(_TSTRING(string));
}

bool CFirmwareTabController::IsFirmwareOpened()
{
 return m_fwdm->IsLoaded();
}

//эта функция не обновляет графики, нужно еще вызывать UpdateOpenedCharts()!
void CFirmwareTabController::SetViewChartsValues(void)
{
 m_fwdm->GetAttenuatorMap(m_view->mp_TablesPanel->GetAttenuatorMap(false),false);
 m_fwdm->GetAttenuatorMap(m_view->mp_TablesPanel->GetAttenuatorMap(true),true);

 m_fwdm->GetDwellCntrlMap(m_view->mp_TablesPanel->GetDwellCntrlMap(false),false);
 m_fwdm->GetDwellCntrlMap(m_view->mp_TablesPanel->GetDwellCntrlMap(true),true);

 if (m_current_funset_index==-1)
  return;
 m_fwdm->GetStartMap(m_current_funset_index,m_view->mp_TablesPanel->GetStartMap(false),false);
 m_fwdm->GetStartMap(m_current_funset_index,m_view->mp_TablesPanel->GetStartMap(true),true);

 m_fwdm->GetIdleMap(m_current_funset_index,m_view->mp_TablesPanel->GetIdleMap(false),false);
 m_fwdm->GetIdleMap(m_current_funset_index,m_view->mp_TablesPanel->GetIdleMap(true),true);

 m_fwdm->GetWorkMap(m_current_funset_index,m_view->mp_TablesPanel->GetWorkMap(false),false);
 m_fwdm->GetWorkMap(m_current_funset_index,m_view->mp_TablesPanel->GetWorkMap(true),true);

 m_fwdm->GetTempMap(m_current_funset_index,m_view->mp_TablesPanel->GetTempMap(false),false);
 m_fwdm->GetTempMap(m_current_funset_index,m_view->mp_TablesPanel->GetTempMap(true),true);
}

void CFirmwareTabController::SetViewFirmwareValues(void)
{
 if (m_fwdm->IsLoaded()==false)
  return;

 CString string = m_fwdm->GetSignatureInfo().c_str();
 m_view->SetFWInformationText(string);

 SetViewChartsValues();

 std::vector<_TSTRING> funset_names = m_fwdm->GetFunctionsSetNames();
 m_view->mp_TablesPanel->SetFunSetListBox(funset_names);

 m_view->mp_TablesPanel->UpdateOpenedCharts();

 //если было выделение в списке, то восстанавлваем его
 m_view->mp_TablesPanel->SetFunSetListBoxSelection(m_current_funset_index);

 m_view->SetFirmwareName(m_fwdm->GetFWFileName());

 m_view->SetFirmwareCRCs(m_fwdm->GetCRC16StoredInActiveFirmware(),m_fwdm->CalculateCRC16OfActiveFirmware());

 m_view->mp_ParamDeskDlg->SetFunctionsNames(funset_names);
 BYTE descriptor = m_view->mp_ParamDeskDlg->GetCurrentDescriptor();
 BYTE paramdata[256];
 m_fwdm->GetDefParamValues(descriptor,paramdata);
 m_view->mp_ParamDeskDlg->SetValues(descriptor,paramdata);

 //Attach fwdm to children controllers
 if (m_fwdm->HasCodeData())
  this->mp_iorCntr->AttachFWDM(m_fwdm);
}

//вкладка может быть закрыта, а график может быть по прежнему в открытом состоянии и изменен.
//В этом случае данная функция может выхываться и при закрытой вкладке.
void CFirmwareTabController::OnMapChanged(int i_type)
{
 switch(i_type)
 {
  case TYPE_MAP_DA_START:
   ASSERT(m_current_funset_index!=-1);
   m_fwdm->SetStartMap(m_current_funset_index,m_view->mp_TablesPanel->GetStartMap(false));
   break;
  case TYPE_MAP_DA_IDLE:
   ASSERT(m_current_funset_index!=-1);
   m_fwdm->SetIdleMap(m_current_funset_index,m_view->mp_TablesPanel->GetIdleMap(false));
   break;
  case TYPE_MAP_DA_WORK:
   ASSERT(m_current_funset_index!=-1);
   m_fwdm->SetWorkMap(m_current_funset_index,m_view->mp_TablesPanel->GetWorkMap(false));
   break;
  case TYPE_MAP_DA_TEMP_CORR:
   ASSERT(m_current_funset_index!=-1);
   m_fwdm->SetTempMap(m_current_funset_index,m_view->mp_TablesPanel->GetTempMap(false));
   break;
  case TYPE_MAP_ATTENUATOR:
   m_fwdm->SetAttenuatorMap(m_view->mp_TablesPanel->GetAttenuatorMap(false));
   break;
  case TYPE_MAP_DWELLCNTRL:
   m_fwdm->SetDwellCntrlMap(m_view->mp_TablesPanel->GetDwellCntrlMap(false));
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
  m_view->mp_TablesPanel->UpdateOpenedCharts();
 }
}

void CFirmwareTabController::OnFunSetNamechanged(int i_index_of_item, CString i_new_name)
{
 m_fwdm->SetFunctionsSetName(i_index_of_item,_TSTRING(i_new_name));
}

void CFirmwareTabController::OnModificationCheckTimer(void)
{
 bool modified = m_fwdm->IsModified();
 m_view->SetModified(modified);
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
 StartWritingOfFLASHFromBuff(m_bl_data);
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
 result  = LoadFLASHFromFile(buff, sizes, NULL, &selected_size, &opened_file_name);
 //Get platform information
 EECUPlatform platform_id;
 if (!PlatformParamHolder::GetPlatformIdByFirmwareSize(selected_size, platform_id))
  return; //error
 PlatformParamHolder params(platform_id);
 if (result && _CheckCompatibilityAndAskUser(buff, &params)) //user OK?
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
 BYTE descriptor = m_view->mp_ParamDeskDlg->GetCurrentDescriptor();
 BYTE paramdata[256];
 m_fwdm->GetDefParamValues(descriptor,paramdata);
 m_view->mp_ParamDeskDlg->SetValues(descriptor,paramdata);
}

//from ParamDesk
void CFirmwareTabController::OnParamDeskChangeInTab(void)
{
 BYTE descriptor = m_view->mp_ParamDeskDlg->GetCurrentDescriptor();
 BYTE paramdata[256];
 m_view->mp_ParamDeskDlg->GetValues(descriptor,paramdata);
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
 import.setFileReader(MakeDelegate(this, &CFirmwareTabController::LoadFLASHFromFile));
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
 bool result = LoadEEPROMFromFile(eeprom, sizes);

 if (!result)
  return; //cancel

 //проверка контрольной суммы загружаемых параметров и вывод предупреждения
 if (!m_edm->VerifyParamsCheckSum(eeprom))
 {
  if (IDCANCEL==AfxMessageBox(IDS_FW_EEPROM_DEF_PARAMS_CRC_INVALID, MB_OKCANCEL))
   return; //user canceled
 }

 m_fwdm->LoadDefParametersFromBuffer(eeprom + m_edm->GetParamsStartAddr());
 SetViewFirmwareValues(); //Update!
}

void CFirmwareTabController::OnImportTablesFromEEPROMFile()
{
 FWMapsDataHolder data;
 EEPROMImportController import(&data);
 import.setFileReader(MakeDelegate(this, &CFirmwareTabController::LoadEEPROMFromFile));
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
 export_cntr.setFileReader(MakeDelegate(this, &CFirmwareTabController::LoadFLASHFromFile));
 m_fwdm->GetMapsData(&data);
 export_cntr.DoExport();
}

//Пользователь захотел получить информацию о пршивке из SECU-3
void CFirmwareTabController::OnFirmwareInfo(void)
{
 m_sbar->SetInformationText(MLL::LoadString(IDS_FW_READING_FW_SIGNATURE));
 SECU3IO::OPCompNc packet_data;
 packet_data.opcode = SECU3IO::OPCODE_READ_FW_SIG_INFO;
 m_comm->m_pControlApp->SendPacket(OP_COMP_NC,&packet_data);
}

void CFirmwareTabController::OnViewFWOptions(void)
{
 _ShowFWOptions(m_fwdm->GetSignatureInfo(), m_fwdm->GetFWOptions());
}

bool CFirmwareTabController::IsViewFWOptionsAvailable(void)
{
 return m_fwdm->GetFWOptions() > 0;
}

bool CFirmwareTabController::IsIORemappingAvailable(void)
{
 return m_fwdm->HasCodeData();
}

void CFirmwareTabController::SetAttenuatorMap(const float* i_values)
{
 m_fwdm->SetAttenuatorMap(i_values);
 SetViewChartsValues();
 m_view->mp_TablesPanel->UpdateOpenedCharts();
}

void CFirmwareTabController::GetAttenuatorMap(float* o_values)
{
 ASSERT(o_values);
 m_fwdm->GetAttenuatorMap(o_values, false); //<--NOTE: modified
}

void CFirmwareTabController::OnCloseMapWnd(HWND i_hwnd, int i_mapType)
{
 if (!i_hwnd)
  return;

 RECT rc;
 GetWindowRect(i_hwnd, &rc);

 WndSettings ws;
 mp_settings->GetWndSettings(ws);

 switch(i_mapType)
 {
  case TYPE_MAP_DA_START:
   ws.m_StrtMapWnd_X = rc.left;
   ws.m_StrtMapWnd_Y = rc.top;
   break;
  case TYPE_MAP_DA_IDLE:
   ws.m_IdleMapWnd_X = rc.left;
   ws.m_IdleMapWnd_Y = rc.top;
   break;
  case TYPE_MAP_DA_WORK:
   ws.m_WorkMapWnd_X = rc.left;
   ws.m_WorkMapWnd_Y = rc.top;
   break;
  case TYPE_MAP_DA_TEMP_CORR:
   ws.m_TempMapWnd_X = rc.left;
   ws.m_TempMapWnd_Y = rc.top;
   break;
  case TYPE_MAP_ATTENUATOR:
   ws.m_AttenuatorMapWnd_X = rc.left;
   ws.m_AttenuatorMapWnd_Y = rc.top;
   break;
  case TYPE_MAP_DWELLCNTRL:
   ws.m_DwellCntrlMapWnd_X = rc.left;
   ws.m_DwellCntrlMapWnd_Y = rc.top;
   break;
 };

 mp_settings->SetWndSettings(ws);
}

void CFirmwareTabController::OnOpenMapWnd(HWND i_hwnd, int i_mapType)
{
 if (!i_hwnd)
  return;

 WndSettings ws;
 mp_settings->GetWndSettings(ws);

 int X = 0, Y = 0;

 switch(i_mapType)
 {
  case TYPE_MAP_DA_START:
   X = ws.m_StrtMapWnd_X, Y = ws.m_StrtMapWnd_Y;
   break;
  case TYPE_MAP_DA_IDLE:
   X = ws.m_IdleMapWnd_X, Y = ws.m_IdleMapWnd_Y;
   break;
  case TYPE_MAP_DA_WORK:
   X = ws.m_WorkMapWnd_X, Y = ws.m_WorkMapWnd_Y;
   break;
  case TYPE_MAP_DA_TEMP_CORR:
   X = ws.m_TempMapWnd_X, Y = ws.m_TempMapWnd_Y;
   break;
  case TYPE_MAP_ATTENUATOR:
   X = ws.m_AttenuatorMapWnd_X, Y = ws.m_AttenuatorMapWnd_Y;
   break;
  case TYPE_MAP_DWELLCNTRL:
   X = ws.m_DwellCntrlMapWnd_X, Y = ws.m_DwellCntrlMapWnd_Y;
   break;
  default:
   return; //undefined case...
 };

 if (X != std::numeric_limits<int>::max() && Y != std::numeric_limits<int>::max())
  SetWindowPos(i_hwnd, NULL, X, Y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}

void CFirmwareTabController::_ShowFWOptions(const _TSTRING& info, DWORD options)
{
 if (options!=0)
 {
  _TSTRING str_options;
  str_options+=info;
  if (info.size() > 0)
   str_options+=_T("\n\n");
  for(size_t i = 0; i < SECU3IO::SECU3_COMPILE_OPTIONS_BITS_COUNT; ++i)
  {
   if(options & 1 << SECU3IO::secu3_compile_options_bits[i].first)
   {
    str_options+= SECU3IO::secu3_compile_options_bits[i].second;
    str_options+=_T("\n");
   }
  }
  AfxMessageBox(str_options.c_str(), MB_OK|MB_ICONINFORMATION);
 }
}
