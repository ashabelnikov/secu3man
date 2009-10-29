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
#include "Resources\resource.h"

#include "CommunicationManager.h"
#include "DLLLinkedFunctions.h"
#include "io-core\ccomport.h"
#include "io-core\logwriter.h"
#include "MainFrame\MainFrame.h"
#include "MainFrame\MainFrameManager.h"
#include "Settings\AppSettingsDlg.h"
#include "Settings\AppSettingsManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CSecu3manApp, CWinApp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSecu3manApp construction

CSecu3manApp::CSecu3manApp()
: m_pMainFrameManager(NULL)
, m_pAppSettingsManager(NULL)
, m_pCommunicationManager(NULL)
{
 m_pAppSettingsManager = new CAppSettingsManager();
 m_pCommunicationManager = new CCommunicationManager();
 m_pLogWriter = new LogWriter();
 m_pMainFrameManager = new CMainFrameManager();
}

CSecu3manApp::~CSecu3manApp()
{
 delete m_pMainFrameManager;
 delete m_pAppSettingsManager;
 delete m_pCommunicationManager;
 delete m_pLogWriter;
}
/////////////////////////////////////////////////////////////////////////////
// The one and only CSecu3manApp object

CSecu3manApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CSecu3manApp initialization

BOOL CSecu3manApp::InitInstance()
{
 AfxEnableControlContainer();

 CoInitialize(NULL);

#if (_MSC_VER <1300)
 #ifdef _AFXDLL
  Enable3dControls();		    // Call this when using MFC in a shared DLL
 #else
  Enable3dControlsStatic();	// Call this when linking to MFC statically
 #endif
#endif

 SetRegistryKey(MLL::LoadString(IDS_APP_TITLE));

 //подгружаем функции из динамически связываемых DLL
 DLL::LoadDLLsAndLinkToFunctions(); 
  
 //читаем настройки
 m_pAppSettingsManager->ReadSettings();
 
 //Создаем главное окно. Оно должно быть создано прежде чем будет произведена
 //дальнейшая инициализация.
 m_pMainWnd = m_pMainFrameManager->GreateMainWindow();

 //инициализируем коммуникационный менеджер
 m_pCommunicationManager->Init();

 //Инициализируем содержимое главного окна (дочерние контроллеры).  
 m_pMainFrameManager->Init(m_pMainWnd);

 return TRUE;
}

BOOL CSecu3manApp::OnIdle(LONG lCount) 
{	
 return CWinApp::OnIdle(lCount);
}

int CSecu3manApp::ExitInstance() 
{	  
 //сохраняем настройки
 m_pAppSettingsManager->WriteSettings();

 //завершение работы менеджера коммуникаций
 m_pCommunicationManager->Terminate();

 return CWinApp::ExitInstance();
}

CAppSettingsManager* CSecu3manApp::GetAppSettingsManager(void) const
{
 return m_pAppSettingsManager;
}

CCommunicationManager* CSecu3manApp::GetCommunicationManager(void) const
{
 return m_pCommunicationManager;
}

CMainFrameManager* CSecu3manApp::GetMainFrameManager(void) const
{
 return m_pMainFrameManager;
}

LogWriter* CSecu3manApp::GetLogWriter(void) const
{
 return m_pLogWriter;
}
