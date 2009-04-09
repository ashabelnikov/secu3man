 /****************************************************************
 *       SECU-3  - An open source, free engine control unit
 *    Designed by Alexey A. Shabelnikov. Ukraine, Gorlovka 2007.
 *       Microprocessors systems - design & programming.
 *    contacts:
 *              http://secu-3.narod.ru
 *              ICQ: 405-791-931
 ****************************************************************/

#include "stdafx.h"
#include "resource.h"
#include "FirmwareTabController.h"

#include "common/FastDelegate.h"
#include "FirmwareTabDlg.h"
#include "io-core/FirmwareDataMediator.h"
#include "FWImpExp/MPSZImpExpController.h"
#include "HexUtils/readhex.h"
#include "CommunicationManager.h"
#include "StatusBarManager.h"
#include "io-core\SECU3IO.h"
#include "io-core\ufcodes.h"

using namespace fastdelegate;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


#define EHKEY _T("FirmwareCntr")

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFirmwareTabController::CFirmwareTabController(CFirmwareTabDlg* i_view, CCommunicationManager* i_comm, CStatusBarManager* i_sbar)
: m_view(NULL)
, m_comm(NULL)
, m_sbar(NULL)
, m_current_funset_index(-1)
, m_bl_read_flash_mode(MODE_RD_FLASH_TO_FILE)
, m_lastSel(0)
, m_bl_started_emergency(false)
{
  m_view = i_view;
  m_comm = i_comm;
  m_sbar = i_sbar;

  m_fwdm = new CFirmwareDataMediator(); 

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
  m_view->setOnMapChanged(MakeDelegate(this,&CFirmwareTabController::OnMapChanged));
  m_view->setOnFunSetSelectionChanged(MakeDelegate(this,&CFirmwareTabController::OnFunSetSelectionChanged));
  m_view->setOnFunSetNamechanged(MakeDelegate(this,&CFirmwareTabController::OnFunSetNamechanged));  
  m_view->setOnImportDataFromAnotherFW(MakeDelegate(this,&CFirmwareTabController::OnImportDataFromAnotherFW));
  m_view->setOnImportDataFromSECU3(MakeDelegate(this,&CFirmwareTabController::OnImportDataFromSECU3));
  m_view->setOnReadFlashFromSECU(MakeDelegate(this,&CFirmwareTabController::OnReadFlashFromSECU));
  m_view->setOnWriteFlashToSECU(MakeDelegate(this,&CFirmwareTabController::OnWriteFlashToSECU));
  m_view->setOnImportMapsFromMPSZ(MakeDelegate(this,&CFirmwareTabController::OnImportMapsFromMPSZ));
  m_view->setOnExportMapsToMPSZ(MakeDelegate(this,&CFirmwareTabController::OnExportMapsToMPSZ));
  m_view->setOnFirmwareInfo(MakeDelegate(this,&CFirmwareTabController::OnWirmwareInfo));

  m_view->m_ParamDeskDlg.SetOnTabActivate(MakeDelegate(this,&CFirmwareTabController::OnParamDeskTabActivate));
  m_view->m_ParamDeskDlg.SetOnChangeInTab(MakeDelegate(this,&CFirmwareTabController::OnParamDeskChangeInTab));
}

CFirmwareTabController::~CFirmwareTabController()
{  
  delete m_fwdm;
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
  bool result = m_view->m_ParamDeskDlg.SetCurSel(m_lastSel);

 //////////////////////////////////////////////////////////////////
 //подключаем контроллер к потоку данных идущих от SECU-3
 m_comm->m_pAppAdapter->AddEventHandler(this,EHKEY); 
 m_comm->m_pBldAdapter->SetEventHandler(this); 
 m_comm->setOnSettingsChanged(MakeDelegate(this,&CFirmwareTabController::OnSettingsChanged)); 
 //////////////////////////////////////////////////////////////////

 //включаем необходимый для данного контекста коммуникационный контроллер
 m_comm->SwitchOn(CCommunicationManager::OP_ACTIVATE_APPLICATION);

 m_modification_check_timer.SetTimer(this,&CFirmwareTabController::OnModificationCheckTimer,250);

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
  m_lastSel = m_view->m_ParamDeskDlg.GetCurSel();
}


/////////////////////////////////////////////////////////////////////////////////
void CFirmwareTabController::OnPacketReceived(const BYTE i_descriptor, SECU3IO::SECU3Packet* ip_packet)
{
 if (i_descriptor == FWINFO_DAT)
 { //приняли пакет с сигнатурной информацией о прошивке
  SECU3IO::FWInfoDat* p_packet = (SECU3IO::FWInfoDat*)ip_packet; 
  char raw_string[256];
  memset(raw_string, 0, 256);  
  memcpy(raw_string, p_packet->info, SECU3IO::FW_SIGNATURE_INFO_SIZE);  
  TCHAR string[256];
  OemToChar(raw_string, string);
  m_sbar->SetInformationText(string);
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
  m_sbar->SetProgressRange(0,data.total);
  m_sbar->SetProgressPos(data.current);
 }
}

void CFirmwareTabController::OnBegin(const int opcode,const int status)
{
  if (opcode == CBootLoader::BL_OP_READ_SIGNATURE)
    m_sbar->SetInformationText("Чтение сигнатуры...");
  if (opcode == CBootLoader::BL_OP_READ_EEPROM)
    m_sbar->SetInformationText("Чтение EEPROM...");
  if (opcode == CBootLoader::BL_OP_WRITE_EEPROM)
    m_sbar->SetInformationText("Запись EEPROM...");
  if (opcode == CBootLoader::BL_OP_READ_FLASH)
    m_sbar->SetInformationText("Чтение прошивки...");
  if (opcode == CBootLoader::BL_OP_WRITE_FLASH)
    m_sbar->SetInformationText("Запись прошивки...");
  if (opcode == CBootLoader::BL_OP_EXIT)
  {
    //m_sbar->SetInformationText("Выход из BL...");  
  }

  m_view->EnableBLItems(false);
  m_view->EnableBLStartedEmergency(false);
}


CString CFirmwareTabController::GenerateErrorStr(void)
{
  switch(m_comm->m_pBootLoader->GetLastError())
  {
  case CBootLoader::BL_ERROR_NOANSWER:
	return CString("Ошибка: Нет ответа!");
  case CBootLoader::BL_ERROR_CHKSUM:
	return CString("Ошибка: Контрольная сумма не совпадает!");
  case CBootLoader::BL_ERROR_WRONG_DATA:
    return CString("Ошибка: Некорректные данные!");
  }
 ASSERT(0); //что за ошибка?
 return CString(_T(""));
}



void CFirmwareTabController::OnEnd(const int opcode,const int status)
{
 switch(opcode)
 { 
  //////////////////////////////////////////////////////////////////////
  case CBootLoader::BL_OP_EXIT: //неиспользуется когда бутлоадер запущен аварийно
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
    m_sbar->SetInformationText("EEPROM успешно прочитано!");
    SaveEEPROMToFile(m_bl_data,CBootLoader::EEPROM_SIZE);
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
    m_sbar->SetInformationText("EEPROM успешно записано!");
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
 	m_sbar->SetInformationText("Прошивка успешно прочитана!");
	if (m_bl_read_flash_mode == MODE_RD_FLASH_TO_FILE)
	{
	 SaveFLASHToFile(m_bl_data,CBootLoader::FLASH_TOTAL_SIZE);
	}
	else if (m_bl_read_flash_mode == MODE_RD_FLASH_TO_BUFF_FOR_LOAD)
	{
	 PrepareOnLoadFLASH(m_bl_data,_T(""));
	}
	else if (m_bl_read_flash_mode == MODE_RD_FLASH_FOR_IMPORT_DATA)
	{
	 m_fwdm->LoadDataBytesFromAnotherFirmware(m_bl_data);
	 m_fwdm->StoreBytes(m_bl_data);
	 PrepareOnLoadFLASH(m_bl_data,m_fwdm->GetFWFileName());
	}
	else if (m_bl_read_flash_mode == MODE_RD_FLASH_TO_BUFF_MERGE_DATA)
	{	     
     //ждем пока не выполнится предыдущая операция
     while(!m_comm->m_pBootLoader->IsIdle());

     //закончилось чтение данных. Теперь необходимо объединить прочитанные данные с данными для записи,
     //обновить контрольную сумму и запустить процесс программирования FLASH. 
	 memcpy(m_code_for_merge_with_overhead + CBootLoader::FLASH_ONLY_CODE_SIZE,m_bl_data,CBootLoader::FLASH_ONLY_OVERHEAD_SIZE);	   
     CFirmwareDataMediator::CalculateAndPlaceFirmwareCRC(m_code_for_merge_with_overhead);

     Sleep(250);
	 m_sbar->SetProgressPos(0);
	 m_comm->m_pBootLoader->StartOperation(CBootLoader::BL_OP_WRITE_FLASH,m_code_for_merge_with_overhead,CBootLoader::FLASH_APP_SECTION_SIZE);		  	 	 

	 //НЕ ВЫХОДИМ ИЗ БУТЛОАДЕРА И НЕ ДЕАКТИВИРУЕМ КОММУНИКАЦИОННЫЙ КОНТРОЛЛЕР, так как должна 
	 //выполнится запущенная операция.
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
    m_sbar->SetInformationText("Прошивка успешно записана!");
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
  bool result = LoadEEPROMFromFile(m_bl_data,CBootLoader::EEPROM_SIZE);

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
  m_comm->m_pBootLoader->StartOperation(CBootLoader::BL_OP_READ_FLASH,m_bl_data,CBootLoader::FLASH_TOTAL_SIZE);

  m_sbar->ShowProgressBar(true);
  m_sbar->SetProgressPos(0);
}


void CFirmwareTabController::OnWriteFlashFromFile(void)
{
  bool result = LoadFLASHFromFile(m_bl_data,CBootLoader::FLASH_TOTAL_SIZE);

  if (!result)
	  return; //cancel

  StartWritingOfFLASHFromBuff(m_bl_data);
}



void CFirmwareTabController::StartWritingOfFLASHFromBuff(BYTE* io_buff)
{
  //вычисляем контрольную сумму и сохраняем ее в массив с прошивкой. Это необходимо например когда
  //мы записываем свеже скомпилированную прошивку, которая может не содержать контрольной суммы
  CFirmwareDataMediator::CalculateAndPlaceFirmwareCRC(io_buff);

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
 
   //сохраняем данные для того чтобы позже объединить их с прочитанными "верхними" данными
   memset(m_code_for_merge_with_overhead,0,CBootLoader::FLASH_APP_SECTION_SIZE);
   memcpy(m_code_for_merge_with_overhead,io_buff,CBootLoader::FLASH_ONLY_CODE_SIZE);
	  
   //операция не блокирует поток - стековые переменные ей передавать нельзя!
   m_comm->m_pBootLoader->StartOperation(CBootLoader::BL_OP_READ_FLASH,m_bl_data,
	   CBootLoader::FLASH_ONLY_OVERHEAD_SIZE, //размер данных сверху над кодом программы
	   CBootLoader::FLASH_ONLY_CODE_SIZE);    //адрес начала "верхних" данных
  }
  else
  {//все очень просто
    m_comm->m_pBootLoader->StartOperation(CBootLoader::BL_OP_WRITE_FLASH,io_buff,CBootLoader::FLASH_APP_SECTION_SIZE);
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
	memcpy(save_buff,p_data,size);
	
	//вычисляем контрольную сумму и сохраняем ее в массив с прошивкой	
	if (calculate_and_place_crc16)
      CFirmwareDataMediator::CalculateAndPlaceFirmwareCRC(save_buff);	

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


//мы заранее знаем размер файла с EEPROM
bool CFirmwareTabController::LoadEEPROMFromFile(BYTE* p_data, const int size)
{
  HANDLE   hFile=0;    
  static TCHAR BASED_CODE szFilter[] = _T("BIN Files (*.bin)|*.bin|All Files (*.*)|*.*||");
  CFileDialog open(TRUE,NULL,NULL,NULL,szFilter,NULL);
  CString cs;

  if (open.DoModal()==IDOK)
  {     
    CFile f;   
    CFileException ex;
    TCHAR szError[1024];
    if(!f.Open(open.GetFileName(),CFile::modeRead,&ex))
    {
      ex.GetErrorMessage(szError, 1024);
	  AfxMessageBox(szError);
      return false;
    }

    //TODO: Добавть проверку на размер файла (его размер должен соответствовать размеру EEPROM)

    f.Read(p_data,size);
    f.Close();	   
	return true; //подтверждение пользователя
  }
  else
	return false; //отказ пользователя
}


//мы заранее знаем размер файла с FLASH
//p_data - буфер для чтения данных. Должен быть не меньше чем 64кБ 
//size  - размер данных для чтения
//o_file_name - указатель на строку в которую будет сохранено имя файла
bool CFirmwareTabController::LoadFLASHFromFile(BYTE* p_data, const int size, CString* o_file_name /* = NULL*/)
{
  HANDLE   hFile=0;    
  static TCHAR BASED_CODE szFilter[] = _T("BIN Files (*.bin)|*.bin|HEX Files (*.hex;*.a90)|*.hex;*.a90|All Files (*.*)|*.*||");
  CFileDialog open(TRUE,NULL,NULL,NULL,szFilter,NULL);
  CString cs;

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
	 if (ulonglong_size > 262144)
	 {
	  AfxMessageBox(_T("Файл слишком большого размера! Извините."));
	  f.Close();
	  return false; //ошибка
	 }

	 int hex_file_length = static_cast<int>(ulonglong_size);
     BYTE* p_hex_buff = new BYTE[hex_file_length]; 
 	 f.Read(p_hex_buff, hex_file_length);
	 size_t bin_size = 0;
     EReadHexStatus status = HexUtils_ConvertHexToBin(p_hex_buff,hex_file_length,p_data,bin_size);
	 delete p_hex_buff;

	 switch(status)
	 {
	  case RH_INCORRECT_CHKSUM:
       AfxMessageBox(_T("Ошибка в контрольной сумме HEX-файла. Операция прервана."));
	   f.Close();
       return false; //ошибка

      default: 
	  case RH_UNEXPECTED_SYMBOL:	   
       AfxMessageBox(_T("Неожиданный символ или структура HEX-файла нарушена. Операция прервана."));
	   f.Close();
       return false; //ошибка

      case RH_SUCCESS:
       break;
	 }

	}
	else //если у файла расширение bin или нет расширения или оно другое, то по умолчанию bin 
    {
     if (f.GetLength() != size)
	 {
      CString string;
	  string.Format(_T("Неправильный размер файла прошивки. Размер должен быть %d байт."),size);
	  AfxMessageBox(string);
	  f.Close();
	  return false; //ошибка
	 }
	 f.Read(p_data,size);
	}
	//----------------------------------------------------------------------------

    f.Close();
	if (o_file_name!=NULL)
	  *o_file_name = open.GetFileName();

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
	Sleep(10);
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
   int result = AfxMessageBox(_T("Прошивка была изменена. Хотите сохранить?"),MB_YESNOCANCEL);
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
  return CheckChangesAskAndSaveFirmware();
}


void CFirmwareTabController::PrepareOnLoadFLASH(const BYTE* i_buff,const _TSTRING& i_file_name)
{   
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

   SetViewFirmwareValues();    
}


void CFirmwareTabController::OnOpenFlashFromFile(void)
{
  bool result;
  BYTE buff[65536];
  CString opened_file_name = _T("");
  
  bool is_continue = CheckChangesAskAndSaveFirmware();
 
  if (!is_continue)
    return;  //пользователь передумал
 
  //!!! без вычисления и записи контрольной суммы в буфер
  result  = LoadFLASHFromFile(buff,CBootLoader::FLASH_TOTAL_SIZE,&opened_file_name);
  if (result) //user OK?
  {
   PrepareOnLoadFLASH(buff,_TSTRING(opened_file_name));
  }
}

void CFirmwareTabController::OnSaveFlashToFile(void)
{
  BYTE buff[65536];
  CString opened_file_name = _T("");
 
  m_fwdm->StoreBytes(buff);

  //в случае подтверждения пользователя, также будует 
  //вычислена контрольная сумма и сохранена в массив с прошивкой	
  bool result = SaveFLASHToFile(buff,CBootLoader::FLASH_TOTAL_SIZE,&opened_file_name,true);
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
    m_view->UpdateOpenedCharts(); 

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
  m_fwdm->GetAttenuatorMap(m_view->GetAttenuatorMap(false),false);
  m_fwdm->GetAttenuatorMap(m_view->GetAttenuatorMap(true),true);

  if (m_current_funset_index==-1)
    return;
  m_fwdm->GetStartMap(m_current_funset_index,m_view->GetStartMap(false),false);
  m_fwdm->GetStartMap(m_current_funset_index,m_view->GetStartMap(true),true);

  m_fwdm->GetIdleMap(m_current_funset_index,m_view->GetIdleMap(false),false);
  m_fwdm->GetIdleMap(m_current_funset_index,m_view->GetIdleMap(true),true);

  m_fwdm->GetWorkMap(m_current_funset_index,m_view->GetWorkMap(false),false);
  m_fwdm->GetWorkMap(m_current_funset_index,m_view->GetWorkMap(true),true);

  m_fwdm->GetTempMap(m_current_funset_index,m_view->GetTempMap(false),false);
  m_fwdm->GetTempMap(m_current_funset_index,m_view->GetTempMap(true),true);
}


void CFirmwareTabController::SetViewFirmwareValues(void)
{
  if (m_fwdm->IsLoaded()==false)
	return;

  CString string = m_fwdm->GetSignatureInfo().c_str();
  m_view->SetFWInformationText(string);

  SetViewChartsValues();  

  std::vector<_TSTRING> funset_names = m_fwdm->GetFunctionsSetNames();  
  m_view->SetFunSetListBox(funset_names);

  m_view->UpdateOpenedCharts();

  //если было выделение в списке, то восстанавлваем его
  m_view->SetFunSetListBoxSelection(m_current_funset_index);

  m_view->SetFirmwareName(m_fwdm->GetFWFileName());

  m_view->SetFirmwareCRCs(m_fwdm->GetCRC16StoredInActiveFirmware(),m_fwdm->CalculateCRC16OfActiveFirmware());

  m_view->m_ParamDeskDlg.SetFunctionsNames(funset_names);
  BYTE descriptor = m_view->m_ParamDeskDlg.GetCurrentDescriptor();
  BYTE paramdata[256];
  m_fwdm->GetDefParamValues(descriptor,paramdata);
  m_view->m_ParamDeskDlg.SetValues(descriptor,paramdata);
}


//вкладка может быть закрыта, а график может быть по прежнему в открытом состоянии и изменен. 
//В этом случае данная функция может выхываться и при закрытой вкладке.
void CFirmwareTabController::OnMapChanged(int i_type)
{
	switch(i_type)
	{
	case TYPE_MAP_DA_START:
		ASSERT(m_current_funset_index!=-1);
        m_fwdm->SetStartMap(m_current_funset_index,m_view->GetStartMap(false)); 
		break;
	case TYPE_MAP_DA_IDLE:
		ASSERT(m_current_funset_index!=-1);
        m_fwdm->SetIdleMap(m_current_funset_index,m_view->GetIdleMap(false)); 
		break;
    case TYPE_MAP_DA_WORK:
		ASSERT(m_current_funset_index!=-1);
        m_fwdm->SetWorkMap(m_current_funset_index,m_view->GetWorkMap(false)); 
		break;
    case TYPE_MAP_DA_TEMP_CORR:
		ASSERT(m_current_funset_index!=-1);
        m_fwdm->SetTempMap(m_current_funset_index,m_view->GetTempMap(false)); 
		break;
    case TYPE_MAP_ATTENUATOR:
		m_fwdm->SetAttenuatorMap(m_view->GetAttenuatorMap(false));
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
	m_view->UpdateOpenedCharts();
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
 //I don't like "copy/paste" paradigm of programming... 
 m_bl_read_flash_mode = MODE_RD_FLASH_TO_BUFF_FOR_LOAD;
 _OnReadFlashToFile();  
}

void CFirmwareTabController::OnWriteFlashToSECU(void)
{ 
  m_fwdm->StoreBytes(m_bl_data);  
  StartWritingOfFLASHFromBuff(m_bl_data);  
}

void CFirmwareTabController::OnImportDataFromAnotherFW()
{
  bool result;
  BYTE buff[65536];
  CString opened_file_name = _T("");
  
  bool is_continue = CheckChangesAskAndSaveFirmware();
 
  if (!is_continue)
    return;  //пользователь передумал
 
  //!!! без вычисления и записи контрольной суммы в буфер
  result  = LoadFLASHFromFile(buff,CBootLoader::FLASH_TOTAL_SIZE,&opened_file_name);
  if (result) //user OK?
  {
    m_fwdm->LoadDataBytesFromAnotherFirmware(buff);
	m_fwdm->StoreBytes(m_bl_data); 
	PrepareOnLoadFLASH(m_bl_data,m_fwdm->GetFWFileName());
  }
}

void CFirmwareTabController::OnImportDataFromSECU3(void)
{
  m_bl_read_flash_mode = MODE_RD_FLASH_FOR_IMPORT_DATA;
  _OnReadFlashToFile();
}

void CFirmwareTabController::OnParamDeskTabActivate(void)
{
  BYTE descriptor = m_view->m_ParamDeskDlg.GetCurrentDescriptor();
  BYTE paramdata[256];
  m_fwdm->GetDefParamValues(descriptor,paramdata);
  m_view->m_ParamDeskDlg.SetValues(descriptor,paramdata);
}

//from ParamDesk
void CFirmwareTabController::OnParamDeskChangeInTab(void)
{
  BYTE descriptor = m_view->m_ParamDeskDlg.GetCurrentDescriptor();
  BYTE paramdata[256];
  m_view->m_ParamDeskDlg.GetValues(descriptor,paramdata);  
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

void CFirmwareTabController::OnExportMapsToMPSZ(void)
{
  FWMapsDataHolder data;
  MPSZExportController export(&data);
  m_fwdm->GetMapsData(&data);
  export.DoExport();
}

//Пользователь захотел получить информацию о пршивке из SECU-3
void CFirmwareTabController::OnWirmwareInfo(void)
{
 m_sbar->SetInformationText("Чтение информации о прошивке...");
 SECU3IO::OPCompNc packet_data;
 packet_data.opcode = SECU3IO::OPCODE_READ_FW_SIG_INFO;
 m_comm->m_pControlApp->SendPacket(OP_COMP_NC,&packet_data);
}