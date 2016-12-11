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

/** \file MainFrameController.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "Resources/resource.h"
#include "MainFrameController.h"

#include "about/secu-3about.h"
#include "Application/CommunicationManager.h"
#include "common/fastdelegate.h"
#include "io-core/logwriter.h"
#include "io-core/ufcodes.h"
#include "MainFrame.h"
#include "MIDesk/DVDeskDlg.h"
#include "Settings/AppSettingsManager.h"
#include "Settings/ISettingsData.h"
#include "StatusBarManager.h"
#include "TablDesk/DLLLinkedFunctions.h"  //for ShowHints()
#include "ui-core/HotKeysManager.h"
#include "ui-core/ToolTipCtrlEx.h"

using namespace fastdelegate;

#define EHKEY _T("LogWriter")
#define MFKEY _T("MFCntr")

void MainFrameController::OnWelcomeTimer(void)
{
 m_welcome_timer.KillTimer();
 DisplayWelcome();
}

MainFrameController::MainFrameController(CCommunicationManager* i_pCommunicationManager,
                   CAppSettingsManager* i_pAppSettingsManager, CStatusBarManager* i_pStatusBarManager,
                   LogWriter* i_pLogWriter, CMainFrame* ip_view /* = NULL*/)
: m_pCommunicationManager(i_pCommunicationManager)
, m_pAppSettingsManager(i_pAppSettingsManager)
, m_pStatusBarManager(i_pStatusBarManager)
, m_pLogWriter(i_pLogWriter)
, mp_view(ip_view)
, m_full_screen_mode(false)
{
 _ASSERTE(i_pCommunicationManager);
 _ASSERTE(i_pAppSettingsManager);
 _ASSERTE(i_pStatusBarManager);
 _ASSERTE(i_pLogWriter);
 _ASSERTE(ip_view);
 _SetDelegates();
 m_pCommunicationManager->m_pAppAdapter->AddEventHandler(this, MFKEY);

 //Show splash screen
 DisplaySplash(1000);
}

void MainFrameController::_SetDelegates(void)
{
 if (NULL==mp_view)
  return;
 mp_view->setOnAppAbout(MakeDelegate(this, &MainFrameController::OnAppAbout));
 mp_view->setOnAppDevSite(MakeDelegate(this, &MainFrameController::OnAppDevSite));
 mp_view->setOnAppDevForum(MakeDelegate(this, &MainFrameController::OnAppDevForum));
 mp_view->setOnAppVK(MakeDelegate(this, &MainFrameController::OnAppVK));
 mp_view->setOnAppFB(MakeDelegate(this, &MainFrameController::OnAppFB));
 mp_view->setOnAppSettings(MakeDelegate(this, &MainFrameController::OnAppSettings));
 mp_view->setOnAppBeginLog(MakeDelegate(this, &MainFrameController::OnAppBeginLog));
 mp_view->setOnAppEndLog(MakeDelegate(this, &MainFrameController::OnAppEndLog));
 mp_view->setIsBeginLoggingAllowed(MakeDelegate(this, &MainFrameController::IsBeginLoggingAllowed));
 mp_view->setIsEndLoggingAllowed(MakeDelegate(this, &MainFrameController::IsEndLoggingAllowed));
 mp_view->setOnActivate(MakeDelegate(this, &MainFrameController::OnActivate));
 mp_view->setOnFullScreen(MakeDelegate(this, &MainFrameController::OnFullScreen));
 mp_view->setOnCreate(MakeDelegate(this, &MainFrameController::OnCreate));
 mp_view->addOnClose(MakeDelegate(this, &MainFrameController::OnClose));
 mp_view->setOnGetInitialPos(MakeDelegate(this, &MainFrameController::OnGetInitialPos));
 mp_view->setOnPortDevArrived(MakeDelegate(this, &MainFrameController::OnPortDevArrived));
 mp_view->setOnAppLogMark(MakeDelegate(this, &MainFrameController::OnAppLogMark));
 mp_view->setOnAppLogFormat(MakeDelegate(this, &MainFrameController::OnAppLogFormat));
}

MainFrameController::~MainFrameController()
{
 //empty
}

void MainFrameController::OnAppAbout()
{
 DisplayAbout(mp_view);
}

void MainFrameController::OnAppDevSite()
{
 ShellExecute(NULL, _T("open"), _T("http://secu-3.org"), NULL, NULL, SW_SHOWNORMAL);
}

void MainFrameController::OnAppDevForum()
{
 ShellExecute(NULL, _T("open"), _T("http://secu-3.org/forum"), NULL, NULL, SW_SHOWNORMAL);
}

void MainFrameController::OnAppVK()
{
 ShellExecute(NULL, _T("open"), _T("http://vk.com/secu3club"), NULL, NULL, SW_SHOWNORMAL);
}

void MainFrameController::OnAppFB()
{
 ShellExecute(NULL, _T("open"), _T("https://www.facebook.com/groups/secu3club/"), NULL, NULL, SW_SHOWNORMAL);
}

void MainFrameController::OnAppSettings()
{
 int result = m_pAppSettingsManager->ShowDialog();

 if (result==IDOK)
 {
  //уведомл€ем логгер об изменени€х в настройках.
  const ISettingsData* settings = m_pAppSettingsManager->GetSettings();
  m_pLogWriter->SetSeparatingSymbol(settings->GetCSVSepSymbol());

  mp_view->BeginWaitCursor();
  m_pCommunicationManager->Init();
  mp_view->EndWaitCursor();

  //”правление отладочной панелью
  //—оздаем или уничножаем отладочную панель и обновл€ем ее период обновлени€ если она создана
  if ((NULL != mp_view->GetDVDesk()) != settings->GetUseDVFeatures())
   VERIFY(mp_view->CreateDVDesk(settings->GetUseDVFeatures()));
  if (mp_view->GetDVDesk())
  {
   mp_view->GetDVDesk()->Show(settings->GetUseDVFeatures());
   mp_view->GetDVDesk()->SetUpdatePeriod(settings->GetDVDeskUpdatePeriod());
  }

  //–азрешаем или запрещаем показавание подсказок
  CToolTipCtrlEx::ActivateAllTooltips(settings->GetShowToolTips());
  DLL::ShowHints(settings->GetShowToolTips());  

  //Start logging if user selected always to write log  
  if (settings->GetAlwaysWriteLog() && IsBeginLoggingAllowed())
   OnAppBeginLog();
 }
}

void MainFrameController::OnAppBeginLog()
{
 //јктивируем записывающий механизм и подключаемс€ к потоку данных
 _TSTRING full_path_to_folder;

 const ISettingsData* settings = m_pAppSettingsManager->GetSettings();

 //устнанавливаем разделительный символ дл€ CSV-файла указанный в настройках
 m_pLogWriter->SetSeparatingSymbol(settings->GetCSVSepSymbol());

 if (!settings->GetUseAppFolder())
  full_path_to_folder = settings->GetLogFilesFolder();
 else
  full_path_to_folder = settings->GetAppDirectory();

 _TSTRING full_file_name;

 bool result = m_pLogWriter->BeginLogging(full_path_to_folder, &full_file_name);
 if (false==result)
 {
  CString  string;
  string.Format(MLL::LoadString(IDS_CANT_START_LOG_WRITING), full_file_name.c_str());
  AfxMessageBox(string,MB_OK | MB_ICONSTOP);
  return;
 }

 m_pCommunicationManager->m_pAppAdapter->AddEventHandler(m_pLogWriter, EHKEY);
 m_pStatusBarManager->SetLoggerState(CStatusBarManager::LOG_STATE_WRITING);
}

void MainFrameController::OnAppEndLog()
{
 //ќтключаемс€ от потока данных и деактивируем записывающий механизм
 m_pCommunicationManager->m_pAppAdapter->RemoveEventHandler(EHKEY);
 m_pLogWriter->EndLogging();
 m_pStatusBarManager->SetLoggerState(CStatusBarManager::LOG_STATE_STOPPED);
}

void MainFrameController::OnAppLogMark(int mark)
{
 m_pLogWriter->InjectMarks(mark);
}

void MainFrameController::OnAppLogFormat()
{
 AfxMessageBox(MLL::GetString(IDS_LOG_FORMAT_DESC).c_str(), MB_OK|MB_ICONINFORMATION);
}

bool MainFrameController::IsBeginLoggingAllowed(void)
{
 return !m_pLogWriter->IsLoggingInProcess() && m_pCommunicationManager->m_pControlApp->GetOnlineStatus();
}

bool MainFrameController::IsEndLoggingAllowed(void)
{
 return m_pLogWriter->IsLoggingInProcess() && m_pCommunicationManager->m_pControlApp->GetOnlineStatus();
}

void MainFrameController::SetView(CMainFrame* ip_view)
{
 _ASSERTE(ip_view);
 mp_view = ip_view;
 _SetDelegates();
}

void MainFrameController::OnPacketReceived(const BYTE i_descriptor, SECU3IO::SECU3Packet* ip_packet)
{
 //≈сли пришел пакет с отладочными данными, то обновл€ем отладочную панель этими данными
 //(если она(панель) создана)
 if(DBGVAR_DAT==i_descriptor)
 {
  const ISettingsData* settings = m_pAppSettingsManager->GetSettings();
  if (mp_view->GetDVDesk())
  {
   mp_view->GetDVDesk()->SetValues((SECU3IO::DbgvarDat*)ip_packet);
   //≈сли отладочна€ панель еще не разрешена, то разрешаем ее.
   //“аким образом он будет запрещенной если DBGVAR_DAT пакеты не приход€т от SECU-3
   if (!mp_view->GetDVDesk()->IsEnabled())
    mp_view->GetDVDesk()->Enable(true);
  }
 }
}

void MainFrameController::OnConnection(const bool i_online)
{
 const ISettingsData* settings = m_pAppSettingsManager->GetSettings();
 if (false==i_online)
 {
  if (m_pLogWriter->IsLoggingInProcess())
  {
   if (!settings->GetAlwaysWriteLog())
    OnAppEndLog(); //прекращаем запись лога при потере коннекта (если не установлена галочка "«аписывать лог всегда")
   else
    m_pLogWriter->FlushFileBuffers();
  }

  //Disabele debug panel if we lose connection
  //here we can only disable it. It may be enabled only after we get DBGVAR_DAT packet!
  if (mp_view->GetDVDesk())
   mp_view->GetDVDesk()->Enable(false);
 }
 else
 {
  //Start logging if user selected always to write log  
  if (settings->GetAlwaysWriteLog() && IsBeginLoggingAllowed())
   OnAppBeginLog();
 }
}

bool MainFrameController::OnFullScreen()
{
 if (!m_full_screen_mode)
 {
  mp_view->GetWindowRect(m_last_wnd_rect);
  CRect rect = _GetScreenRect();
  mp_view->MoveWindow(rect.left,rect.top,rect.Width(), rect.Height());
  m_full_screen_mode = true;
 }
 else
 {
  mp_view->MoveWindow(m_last_wnd_rect.left,m_last_wnd_rect.top,
      m_last_wnd_rect.Width(), m_last_wnd_rect.Height());
  m_full_screen_mode = false;
 }
 return m_full_screen_mode;
}

void MainFrameController::OnActivate(bool i_state)
{
 if (false==i_state && true==HotKeysManager::GetInstance()->IsActive())
  HotKeysManager::GetInstance()->DeactivateAllHotKeys();

 if (true==i_state && false==HotKeysManager::GetInstance()->IsActive())
  HotKeysManager::GetInstance()->ActivateAllHotKeys();
}

CRect MainFrameController::_GetScreenRect(void) const
{
 CDC* pDC = mp_view->GetDC();
 int x_resolution = pDC->GetDeviceCaps(HORZRES);
 int y_resolution = pDC->GetDeviceCaps(VERTRES);
 return CRect(0, 0, x_resolution, y_resolution);
}

void MainFrameController::OnCreate(void)
{
 const ISettingsData* settings = m_pAppSettingsManager->GetSettings();
 if (settings->GetShowWelcome()) {
  //Set welcome timer
  m_welcome_timer.SetTimer(this, &MainFrameController::OnWelcomeTimer, 2000);
 }

 VERIFY(mp_view->CreateDVDesk(settings->GetUseDVFeatures()));
 if (mp_view->GetDVDesk())
  mp_view->GetDVDesk()->Show(settings->GetUseDVFeatures());
}

bool MainFrameController::OnClose(void)
{
 ISettingsData* settings = m_pAppSettingsManager->GetSettings();
 WndSettings ws;
 settings->GetWndSettings(ws);

 CRect rc;
 mp_view->GetWindowRect(rc);
 ws.m_MainFrmWnd_X = rc.left;
 ws.m_MainFrmWnd_Y = rc.top;

 //store last position of main window
 settings->SetWndSettings(ws);

 return true;
}

void MainFrameController::OnGetInitialPos(CPoint& o_point)
{
 const ISettingsData* settings = m_pAppSettingsManager->GetSettings();
 WndSettings ws;
 settings->GetWndSettings(ws);

 //restore remembered position of main winwow
 o_point.x = ws.m_MainFrmWnd_X, o_point.y = ws.m_MainFrmWnd_Y;
}

void MainFrameController::OnPortDevArrived(const _TSTRING& devName)
{
 if (m_pAppSettingsManager->GetSettings()->GetPortName() == devName)
 {
  mp_view->BeginWaitCursor();
  m_pCommunicationManager->Init();
  mp_view->EndWaitCursor();
 }
}
