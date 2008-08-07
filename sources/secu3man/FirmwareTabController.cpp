 /****************************************************************
 *       SECU-3  - An open source, free engine control unit
 *    Designed by Alexey A. Shabelnikov. Ukraine, Gorlovka 2007.
 *       Microprocessors systems - design & programming.
 *    contacts:
 *              http://secu-3.narod.ru
 *              ICQ: 405-791-931
 ****************************************************************/

#include "stdafx.h"
#include "secu3man.h"
#include "FirmwareTabController.h"

#include "common/FastDelegate.h"
#include "FirmwareTabDlg.h"
#include "io-core/FirmwareDataMediator.h"

using namespace fastdelegate;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFirmwareTabController::CFirmwareTabController(CFirmwareTabDlg* i_view, CCommunicationManager* i_comm, CStatusBarManager* i_sbar)
: m_view(NULL)
, m_comm(NULL)
, m_sbar(NULL)
, m_pAppAdapter(NULL)
, m_pBldAdapter(NULL)
{
  m_view = i_view;
  m_comm = i_comm;
  m_sbar = i_sbar;

  CWnd* pParent = AfxGetApp()->m_pMainWnd;

  m_pAppAdapter = new CControlAppAdapter(this);
  m_pAppAdapter->Create(pParent);


  m_pBldAdapter = new CBootLoaderAdapter(this);
  m_pBldAdapter->Create(pParent);

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


}

CFirmwareTabController::~CFirmwareTabController()
{
  delete m_pAppAdapter;
  delete m_pBldAdapter;
  delete m_fwdm;
}

  
//изменились настройки 
void CFirmwareTabController::OnSettingsChanged(void)
{
 //включаем необходимый для данного контекста коммуникационный контроллер
 m_comm->m_pControlApp->SwitchOn(true);
 m_comm->m_pBootLoader->SwitchOn(false);  
}


void CFirmwareTabController::OnActivate(void)
{
  //AfxMessageBox(_T("CFirmwareTabController::OnActivate"));

 //////////////////////////////////////////////////////////////////
 //устанавливаем обработчики событий специфичные для контекста программы в котором работает контроллер
 m_comm->m_pControlApp->SetEventHandler(m_pAppAdapter); 
 m_comm->m_pBootLoader->SetEventHandler(m_pBldAdapter); 
 m_comm->SetOnSettingsChanged(MakeDelegate(this,&CFirmwareTabController::OnSettingsChanged)); 
 //////////////////////////////////////////////////////////////////

 m_pAppAdapter->SwitchOn(true); 

 //включаем необходимый для данного контекста коммуникационный контроллер
 m_comm->m_pControlApp->SwitchOn(true); //true
 m_comm->m_pBootLoader->SwitchOn(false);

 SetViewFirmwareValues();
}

void CFirmwareTabController::OnDeactivate(void)
{
  m_pAppAdapter->SwitchOn(false); 
  m_sbar->SetInformationText(_T(""));
}


/////////////////////////////////////////////////////////////////////////////////
void CFirmwareTabController::OnPacketReceived(const BYTE i_descriptor, const void* i_packet_data)
{
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
  }
  else
  { //перешли в оффлайн
    if (m_comm->m_pBootLoader->IsStoped()) //разрешаем чекбокс только если мы в оффлайне сразу после онлайна 
	  m_view->EnableBLStartedEmergency(true);

	//в оффлайне состояние элементов меню связанных с бутлоадером зависит от состояния чекбокса
	bool enable = m_view->IsBLStartedEmergency();
  	m_view->EnableBLItems(enable);

	if (enable)
	  state = CStatusBarManager::STATE_BOOTLOADER; //чтобы иконка бутлоадера не пропадала после завершения операции
	else
	  state = CStatusBarManager::STATE_OFFLINE;  
  }

  //если бутлоадер активен (выполняется выбранная из меню операция), то будем отображать именно 
  //иконку бутлоадера 
  if (!m_comm->m_pBootLoader->IsStoped()) 
    state = CStatusBarManager::STATE_BOOTLOADER;

  m_sbar->SetConnectionState(state);
}



/////////////////////////////////////////////////////////////////////////////////
void CFirmwareTabController::OnUpdateUI(const int opcode,const int total,const int current)
{
	if (opcode!=CBootLoader::BL_OP_EXIT) //для операции выхода из бутлоадера не показываем никакого прогресс бара
	{
	  m_sbar->SetProgressRange(0,total);
	  m_sbar->SetProgressPos(current);
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
  //////////////////////////////////////////////////////////////////////
  if (opcode == CBootLoader::BL_OP_EXIT) //неиспользуется когда бутлоадер запущен аварийно
  {
    // m_sbar->SetInformationText("Программа запущена.");

    //вновь активируем коммуникационный контроллер приложения
    m_comm->m_pControlApp->SwitchOn(true);
    m_comm->m_pBootLoader->SwitchOn(false);  
   
    //m_view->EnableBLItems(true);
    //m_view->EnableBLStartedEmergency(true);
  }
  //////////////////////////////////////////////////////////////////////
  if (opcode == CBootLoader::BL_OP_READ_SIGNATURE) 
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
  }

  //////////////////////////////////////////////////////////////////////
  else if (opcode == CBootLoader::BL_OP_READ_EEPROM)
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
  }
  //////////////////////////////////////////////////////////////////////
  else if (opcode == CBootLoader::BL_OP_WRITE_EEPROM)
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
  }
  //////////////////////////////////////////////////////////////////////
  else if (opcode == CBootLoader::BL_OP_READ_FLASH)
  {
    if (status==1)
    { 	
 	  m_sbar->SetInformationText("Прошивка успешно прочитана!");
      SaveFLASHToFile(m_bl_data,CBootLoader::FLASH_TOTAL_SIZE);
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
  }
  //////////////////////////////////////////////////////////////////////
  else if (opcode == CBootLoader::BL_OP_WRITE_FLASH)
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
  }
  //////////////////////////////////////////////////////////////////////

  if (m_bl_started_emergency)
  {
    m_view->EnableBLItems(true);
    m_view->EnableBLStartedEmergency(true);

    m_comm->m_pControlApp->SwitchOn(true);
    m_comm->m_pBootLoader->SwitchOn(false);  
  }

}   

/////////////////////////////////////////////////////////////////////////////////

//получение информации о бутлоадере
void CFirmwareTabController::OnBootLoaderInfo(void)
{
  //запускаем бутлоадер (если нужно) 	
  StartBootLoader(); 

  //активируем коммуникационный контроллер бутлоадера
  m_comm->m_pControlApp->SwitchOn(false);
  m_comm->m_pBootLoader->SwitchOn(true);  

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
  m_comm->m_pControlApp->SwitchOn(false);
  m_comm->m_pBootLoader->SwitchOn(true);  

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
  m_comm->m_pControlApp->SwitchOn(false);
  m_comm->m_pBootLoader->SwitchOn(true);  

  //операция не блокирует поток - стековые переменные ей передавать нельзя!
  m_comm->m_pBootLoader->StartOperation(CBootLoader::BL_OP_WRITE_EEPROM,m_bl_data,0);

  m_sbar->ShowProgressBar(true);
  m_sbar->SetProgressPos(0);
}

void CFirmwareTabController::OnReadFlashToFile(void)
{
  //запускаем бутлоадер по команде из приложения 	
  StartBootLoader(); 

  //активируем коммуникационный контроллер бутлоадера
  m_comm->m_pControlApp->SwitchOn(false);
  m_comm->m_pBootLoader->SwitchOn(true);  

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

  //вычисляем контрольную сумму и сохраняем ее в массив с прошивкой. Это необходимо например когда
  //мы записываем свеже скомпилированную прошивку, которая может не содержать контрольной суммы
  CFirmwareDataMediator::CalculateAndPlaceFirmwareCRC(m_bl_data);

  ASSERT(m_comm);

  //запускаем бутлоадер по команде из приложения (если нет флага что он запущен вручную) 	
  StartBootLoader(); 

  //активируем коммуникационный контроллер бутлоадера
  m_comm->m_pControlApp->SwitchOn(false);
  m_comm->m_pBootLoader->SwitchOn(true);  

  //операция не блокирует поток - стековые переменные ей передавать нельзя!
  m_comm->m_pBootLoader->StartOperation(CBootLoader::BL_OP_WRITE_FLASH,m_bl_data,CBootLoader::FLASH_APP_SECTION_SIZE);

  m_sbar->ShowProgressBar(true);
  m_sbar->SetProgressPos(0); 
}



void CFirmwareTabController::SaveEEPROMToFile(const BYTE* p_data, const int size)
{
  HANDLE   hFile=0;    
  static TCHAR BASED_CODE szFilter[] = _T("BIN Files (*.bin)|*.bin|All Files (*.*)|*.*||");
  CFileDialog save(FALSE,NULL,NULL,NULL,szFilter,NULL);
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


void CFirmwareTabController::SaveFLASHToFile(const BYTE* p_data, const int size)
{
  HANDLE   hFile=0;    
  static TCHAR BASED_CODE szFilter[] = _T("BIN Files (*.bin)|*.bin|All Files (*.*)|*.*||");
  CFileDialog save(FALSE,NULL,NULL,NULL,szFilter,NULL);
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
bool CFirmwareTabController::LoadFLASHFromFile(BYTE* p_data, const int size)
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

    //TODO: Добавть проверку на размер файла (его размер должен соответствовать переданному size!!!)

    f.Read(p_data,size);
    f.Close();	   

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


//Эта функция вызывается при выходе из прложения. Эта функция может запретить выход, если вернет false 
bool CFirmwareTabController::OnClose(void)
{
  bool modified = m_fwdm->IsModified();
  if (modified)
  {
   int result = AfxMessageBox(_T("Прошивка была изменена. Хотите сохранить?"),MB_YESNOCANCEL);
   if (result==IDCANCEL)
   {
     return false; //пользователь отменил выход из приложения
   }
   if (result==IDNO)
   {
     return true; //пользователь решил выйти без сохраненя
   }
   if (result==IDYES)
   { //необходимо сохранить данные!
     OnSaveFlashToFile();
     return true;
   }  
  }

  //данные не были изменены - разрешаем выход из приложения
  return true;
}


void CFirmwareTabController::OnOpenFlashFromFile(void)
{
  bool result;
  BYTE buff[65536];
  
  //!!! без вычисления и записи контрольной суммы в буфер
  result  = LoadFLASHFromFile(buff,CBootLoader::FLASH_TOTAL_SIZE);
  if (result) //user OK?
  {
   m_fwdm->LoadBytes(buff);
  
   SetViewFirmwareValues();
  }

}

void CFirmwareTabController::OnSaveFlashToFile(void)
{
  BYTE buff[65536];
 
  m_fwdm->StoreBytes(buff);

  //вычисляем контрольную сумму и сохраняем ее в массив с прошивкой	
  CFirmwareDataMediator::CalculateAndPlaceFirmwareCRC(buff);

  SaveFLASHToFile(buff,CBootLoader::FLASH_TOTAL_SIZE);

  //данные были успешно сохранены - можно сбрасывать признак модификации
  m_fwdm->ResetModified();
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

void CFirmwareTabController::SetViewFirmwareValues(void)
{
  if (m_fwdm->IsLoaded()==false)
	return;
  CString string = m_fwdm->GetSignatureInfo().c_str();
  m_view->SetFWInformationText(string);
}
