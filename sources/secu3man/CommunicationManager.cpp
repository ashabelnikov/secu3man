 /****************************************************************
 *       SECU-3  - An open source, free engine control unit
 *    Designed by Alexey A. Shabelnikov. Ukraine, Gorlovka 2007.
 *       Microprocessors systems - design & programming.
 *    contacts:
 *              http://secu-3.narod.ru
 *              ICQ: 405-791-931
 ****************************************************************/

#include "stdafx.h"
#include "AppSettingsManager.h"
#include "CommunicationManager.h"
#include "io-core/CComPort.h"
#include "io-core/Bootloader.h"
#include "io-core/LogWriter.h"
#include "ISECU3Man.h"
#include "ISettingsData.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

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
 m_pSettings = ISECU3Man::GetSECU3Man()->GetAppSettingsManager()->GetSettings();

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
  m_pComPort->Initialize(_TSTRING(m_pSettings->GetPortName()),9600,NOPARITY,ONESTOPBIT,0,1);
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
 {
  m_pControlApp->Initialize(m_pComPort,m_pSettings->GetBaudRateApplication(),500);
 }
 catch(CControlApp::xThread)
 {
  ASSERT(0); //because, strange situation!
  status = false;
 }

 try
 {
  m_pBootLoader->Initialize(m_pComPort,m_pSettings->GetBaudRateBootloader());
 }
 catch (CBootLoader::xThread)
 {
  ASSERT(0);
  status = false;
 }

 //Инициализруем адаптеры коммуникационных контроллеров
 CWnd* pParent = AfxGetApp()->m_pMainWnd;
 VERIFY(m_pAppAdapter->Create(pParent));
 VERIFY(m_pBldAdapter->Create(pParent));

 //Связываем адаптеры с коммуникационными контроллерами
 m_pControlApp->SetEventHandler(m_pAppAdapter);
 m_pBootLoader->SetEventHandler(m_pBldAdapter);

 //оповещаем объекта слушателя об изменении настроек. Получатель этого оповещения должен активировать
 //необходимый ему коммуникационный контроллер.
 if (m_OnSettingsChanged) 
  m_OnSettingsChanged();

 return status;
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

void CCommunicationManager::setOnSettingsChanged(EventHandler i_OnSettingsChanged) 
{
 m_OnSettingsChanged = i_OnSettingsChanged;
}
