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
#include "resource.h"

#include "MainFrame.h"
#include "about/secu-3about.h"
#include "AppSettingsDlg.h"
#include "DLLLinkedFunctions.h"
#include "MainFrameManager.h"
#include "io-core/ccomport.h"
#include "CommunicationManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSecu3manApp

BEGIN_MESSAGE_MAP(CSecu3manApp, CWinApp)
  //{{AFX_MSG_MAP(CSecu3manApp)
    ON_COMMAND(ID_APP_ABOUT, OnAppAbout)		
	ON_COMMAND(ID_APP_SETTINGS, OnAppSettings)
	ON_COMMAND(ID_APP_BEGIN_LOG, OnAppBeginLog)
	ON_COMMAND(ID_APP_END_LOG, OnAppEndLog)
	ON_UPDATE_COMMAND_UI(ID_APP_BEGIN_LOG,OnUpdateOnAppBeginLog)
	ON_UPDATE_COMMAND_UI(ID_APP_END_LOG,OnUpdateOnAppEndLog)
	//}}AFX_MSG_MAP
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
}

CSecu3manApp::~CSecu3manApp()
{
  delete m_pMainFrameManager;
  delete m_pAppSettingsManager;
  delete m_pCommunicationManager;

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

  #ifdef _AFXDLL
    Enable3dControls();		    // Call this when using MFC in a shared DLL
  #else
    Enable3dControlsStatic();	// Call this when linking to MFC statically
  #endif

  SetRegistryKey(_T("SECU-3 Manager"));

  //подгружаем функции из динамически связываемых DLL
  DLL::LoadDLLsAndLinkToFunctions(); 
  
  //читаем настройки
  m_pAppSettingsManager->ReadSettings();
 
  //инициализируем коммуникационный менеджер
  m_pCommunicationManager->Init();

  //инициализируем менеджер главного окна
  m_pMainFrameManager = new CMainFrameManager();
  m_pMainFrameManager->Init(m_pMainWnd);

  //дурацкий метод надо вызвать после создания главного окна...
  m_pCommunicationManager->OnAfterCreate();
  return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CSecu3manApp message handlers

void CSecu3manApp::OnAppAbout()
{
  DisplayAbout(this->m_pMainWnd);
}

BOOL CSecu3manApp::OnIdle(LONG lCount) 
{	
  return CWinApp::OnIdle(lCount);
}

void CSecu3manApp::OnAppSettings() 
{
  int result = m_pAppSettingsManager->ShowDialog();

  if (result==IDOK)
  {
    BeginWaitCursor();
    m_pCommunicationManager->Init();
	EndWaitCursor();
  }
}

int CSecu3manApp::ExitInstance() 
{	
  
  //сохраняем настройки
  m_pAppSettingsManager->WriteSettings();

  //завершение работы менеджера коммуникаций
  m_pCommunicationManager->Terminate();

  return CWinApp::ExitInstance();
}


void CSecu3manApp::OnAppBeginLog()
{
 m_pCommunicationManager->OnStartLogWriting();
 if (m_pCommunicationManager->IsLoggingInProcess())
  m_pMainFrameManager->m_StatusBarManager.SetLoggerState(CStatusBarManager::LOG_STATE_WRITING);
}

void CSecu3manApp::OnAppEndLog()
{
 m_pCommunicationManager->OnStopLogWriting();
 m_pMainFrameManager->m_StatusBarManager.SetLoggerState(CStatusBarManager::LOG_STATE_STOPPED);
}


void CSecu3manApp::OnUpdateOnAppBeginLog(CCmdUI* pCmdUI)
{
  pCmdUI->Enable(m_pCommunicationManager->IsLoggingInProcess() ? FALSE : TRUE);
}

void CSecu3manApp::OnUpdateOnAppEndLog(CCmdUI* pCmdUI)
{
  pCmdUI->Enable(m_pCommunicationManager->IsLoggingInProcess() ? TRUE : FALSE);
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
