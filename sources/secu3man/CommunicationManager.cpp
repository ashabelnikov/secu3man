 /****************************************************************
 *       SECU-3  - An open source, free engine control unit
 *    Designed by Alexey A. Shabelnikov. Ukraine, Gorlovka 2007.
 *       Microprocessors systems - design & programming.
 *    contacts:
 *              http://secu-3.narod.ru
 *              ICQ: 405-791-931
 ****************************************************************/

#include "stdafx.h"
#include "ISECU3Man.h"
#include "CommunicationManager.h"
#include "io-core/CComPort.h"
#include "io-core/Bootloader.h"
#include "io-core/LogWriter.h"
#include "AppSettingsModel.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


#define EHKEY _T("CommManager")

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCommunicationManager::CCommunicationManager()
: m_pComPort(NULL)
, m_pBootLoader(NULL)
, m_pControlApp(NULL)
, m_recv_buff_size(32768)
, m_send_buff_size(32768)
, m_pSettings(NULL)  //external!
, m_OnSettingsChanged(NULL)
, m_pAppAdapter(NULL)
, m_pBldAdapter(NULL)
{
  m_pSettings = ISECU3Man::GetSECU3Man()->GetAppSettingsManager();

  m_pComPort    = new CComPort(_T("COM1"),m_recv_buff_size,m_send_buff_size);
  m_pBootLoader = new CBootLoader();
  m_pControlApp = new CControlApp();  
  m_pAppAdapter = new CControlAppAdapter();  
  m_pBldAdapter = new CBootLoaderAdapter();
}

CCommunicationManager::~CCommunicationManager()
{
  delete m_pComPort;
  delete m_pBootLoader;
  delete m_pControlApp;  
  delete m_pAppAdapter;
  delete m_pBldAdapter;
}


bool CCommunicationManager::Terminate(void)
{
  //завершаем работу контроллеров интерфейсов
  m_pControlApp->Terminate();
  m_pBootLoader->Terminate();

  m_pComPort->Terminate();

  return true;
}


bool CCommunicationManager::Init(void)
{
  ASSERT(m_pSettings);  //ты забыл меня инициализировать!
  ASSERT(m_pBootLoader);
  ASSERT(m_pControlApp);
  ASSERT(m_pAppAdapter);
  ASSERT(m_pBldAdapter);

  bool status = true;

  //останавливаем работу контроллеров интерфейсов, конкретный контроллер будет активирован
  //позже - в зависимости от текущего контекста программы
  m_pControlApp->SwitchOn(false); 
  m_pBootLoader->SwitchOn(false); 

  //для того чтобы можно было применить новые настройки к порту, необходимо его заново открыть
  m_pComPort->Terminate();

  try
  {
    //на скорость переданную в эту функцию не нужно обращать внимания (она будет установлена конкретным контроллером интрерфейса)
	m_pComPort->Initialize(_TSTRING(m_pSettings->m_pModel->m_optPortName),9600,NOPARITY,ONESTOPBIT,0,1);
  }
  catch(CComPort::xInitialize e)
  {
    //говорим пользователю что нам не удалось открыть указанный порт!
    AfxMessageBox(e.GetDetailStr());
	status = false;
  }
  
  m_pComPort->Purge();

  //инициализируем контроллеры интерфейсов
  try 
  { m_pControlApp->Initialize(m_pComPort,m_pSettings->m_pModel->m_optBaudRateApplication,500);
  }
  catch(CControlApp::xThread)
  {
    ASSERT(0); //because, strange situation!
	status = false;
  }

  try
  { m_pBootLoader->Initialize(m_pComPort,m_pSettings->m_pModel->m_optBaudRateBootloader);
  }
  catch (CBootLoader::xThread)
  {
    ASSERT(0);
	status = false;
  }

  m_pControlApp->SetEventHandler(m_pAppAdapter);
  m_pBootLoader->SetEventHandler(m_pBldAdapter);

  //оповещаем объекта слушателя об изменении настроек
  if (m_OnSettingsChanged) 
	m_OnSettingsChanged();

  return status;
}

void CCommunicationManager::OnAfterCreate(void)
{
  CWnd* pParent = AfxGetApp()->m_pMainWnd;
  VERIFY(m_pAppAdapter->Create(pParent));
  VERIFY(m_pBldAdapter->Create(pParent));
}

//активирование указанного коммуникационного контроллера или деактивирование всех контроллеров
void CCommunicationManager::SwitchOn(size_t i_cntr, bool i_force_reinit  /* = false */)
{
 _ASSERTE(!(m_pControlApp->GetWorkState() && m_pBootLoader->GetWorkState()));
 
 switch(i_cntr)
 {
  case OP_ACTIVATE_APPLICATION:
   m_pBootLoader->SwitchOn(false, i_force_reinit);
   m_pControlApp->SwitchOn(true, i_force_reinit); 
   break;

  case OP_ACTIVATE_BOOTLOADER:
   m_pControlApp->SwitchOn(false, i_force_reinit); 
   m_pBootLoader->SwitchOn(true, i_force_reinit); 
   break;
   
  case OP_DEACTIVATE_ALL: //deactivate all 
   m_pControlApp->SwitchOn(false, i_force_reinit); 
   m_pBootLoader->SwitchOn(false, i_force_reinit); 
   break;

  default:
   _ASSERTE(0);
   break;
 }
}

void CCommunicationManager::SetOnSettingsChanged(EventHandler OnSettingsChanged) 
{
 m_OnSettingsChanged = OnSettingsChanged;
}

///////////////////Управление записью логов/////////////////////

void CCommunicationManager::OnStartLogWriting(void)
{
  //Активируем записывающий механизм и подключаемся к потоку данных
  _TSTRING full_path_to_folder;

  CAppSettingsModel* settings = m_pSettings->m_pModel;

  if (!settings->m_optUseAppFolder)
    full_path_to_folder = settings->m_optLogFilesFolder;
  else
    full_path_to_folder = settings->GetAppDirectory();
  
  _TSTRING full_file_name;

  bool result = m_logwriter.BeginLogging(full_path_to_folder, &full_file_name);
  if (false==result)
  {
   CString  string;
   string.Format(_T("Не могу начать запись лога в файл: %s\n\
Возможно этот файл защищен от записи или уже открыт другой программой."), full_file_name.c_str());
   AfxMessageBox(string,MB_OK | MB_ICONSTOP);
   return;
  }

  m_pAppAdapter->AddEventHandler(&m_logwriter, EHKEY);
}

void CCommunicationManager::OnStopLogWriting(void)
{
  //Отключаемся от потока данных и деактивируем записывающий механизм	 
  m_pAppAdapter->RemoveEventHandler(EHKEY);
  m_logwriter.EndLogging();
}

bool CCommunicationManager::IsLoggingInProcess(void)
{
  //записывающий механизм активен ?
  return m_logwriter.IsLoggingInProcess(); 
}

////////////////////////////////////////////////////////////////