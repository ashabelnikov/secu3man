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
#include "CommunicationManager.h"
#include "io-core/CComPort.h"
#include "io-core/Bootloader.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

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
{
  m_pSettings = (static_cast<CSecu3manApp*>(AfxGetApp()))->m_pAppSettingsManager;

  m_pComPort = new CComPort(_T("COM1"),m_recv_buff_size,m_send_buff_size);

  m_pBootLoader = new CBootLoader();
  m_pControlApp = new CControlApp();
}

CCommunicationManager::~CCommunicationManager()
{
  delete m_pComPort;
  delete m_pBootLoader;
  delete m_pControlApp;
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

  m_pControlApp->SetSettings(m_pSettings->m_pModel->m_optMAPCurveOffset,m_pSettings->m_pModel->m_optMAPCurveSlope);

  //оповещаем объекта слушателя об изменении настроек
  if (m_OnSettingsChanged) 
	m_OnSettingsChanged();

  return status;
}
