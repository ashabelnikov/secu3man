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

#include <limits>
#include <shlwapi.h>
#include "about/secu-3about.h"
#include "Application/CommunicationManager.h"
#include "common/fastdelegate.h"
#include "io-core/logwriter.h"
#include "io-core/ufcodes.h"
#include "MainFrame.h"
#include "MIDesk/DVDeskDlg.h"
#include "Settings/AppSettingsManager.h"
#include "Settings/ISettingsData.h"
#include "Settings/EditSettingsDlg.h"
#include "StatusBarManager.h"
#include "TablDesk/DLLLinkedFunctions.h"  //for ShowHints()
#include "ui-core/HotKeysManager.h"
#include "ui-core/ToolTipCtrlEx.h"
#include "ui-core/ScrlMessageBox.h"
#include "ui-core/ImageUtils.h"
#include "Application/RestartAPI.h"
#include "ui-core/MsgBox.h"

using namespace fastdelegate;

#undef max
#define EHKEY _T("LogWriter")
#define MFKEY _T("MFCntr")

void MainFrameController::OnWelcomeTimer(void)
{
 m_welcome_timer.KillTimer();
 DisplayWelcome();
}

void MainFrameController::OnScr5SecTimer(void)
{
 m_scr5sec_timer.KillTimer();
 mp_view->RedrawWindow();
 SaveScreenshot(AfxGetMainWnd(), true);
}

MainFrameController::MainFrameController(CCommunicationManager* i_pCommunicationManager,
                   CAppSettingsManager* i_pAppSettingsManager, CStatusBarManager* i_pStatusBarManager,
                   LogWriter* i_pLogWriter, CMainFrame* ip_view /* = NULL*/)
: m_pCommunicationManager(i_pCommunicationManager)
, m_pAppSettingsManager(i_pAppSettingsManager)
, m_pStatusBarManager(i_pStatusBarManager)
, m_pLogWriter(i_pLogWriter)
, mp_view(ip_view)
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
 mp_view->setOnAppSwitchDashboards(MakeDelegate(this, &MainFrameController::OnAppSwitchDashboards));
 mp_view->setOnAppSaveScreenshot(MakeDelegate(this, &MainFrameController::OnAppSaveScreenshot));
 mp_view->setOnAppSaveScreenshot5sec(MakeDelegate(this, &MainFrameController::OnAppSaveScreenshot5sec));
 mp_view->addOnAppSaveSettings(MakeDelegate(this, &MainFrameController::OnAppSaveSettings));
 mp_view->setOnChildCharts(MakeDelegate(this, &MainFrameController::OnChildCharts));
 mp_view->setOnToggleMapWnd(MakeDelegate(this, &MainFrameController::OnToggleMapWnd));
 mp_view->setOnEditSettings(MakeDelegate(this, &MainFrameController::OnEditSettings));
 mp_view->setOnHelp(MakeDelegate(this, &MainFrameController::OnHelp));
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
  DLL::ShowHints(settings->GetShowToolTips(), settings->GetToolTipTime());  
  CToolTipCtrlEx::SetVisibleTime(settings->GetToolTipTime());

  //Start logging if user selected always to write log  
  if (settings->GetAlwaysWriteLog() && IsBeginLoggingAllowed())
   OnAppBeginLog();

  mp_view->CheckOnAppSwitchDashboards(settings->GetShowExFixtures());

  if (m_pAppSettingsManager->IsRestartRequired())
  {
   if (IDYES == SECUMessageBox(MLL::LoadString(IDS_APP_SETTINGS_APPRESTART_CONFIRM), MB_YESNO | MB_DEFBUTTON1))
   {
    if (RA_ActivateRestartProcess())
     AfxGetMainWnd()->DestroyWindow();
   }
  }

  //Control state of the "Inj.driver" tab
  if (IsWindow(mp_view->GetSafeHwnd()))
   mp_view->EnableTab(7, settings->GetInjDrvTabActive()); 
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
  SECUMessageBox(string,MB_OK | MB_ICONSTOP);
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
 CScrlMessageBox msgbox(NULL, AfxGetAppName(), MLL::GetString(IDS_LOG_FORMAT_DESC), IDI_INFORMATION);
 msgbox.DoModal();
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
 if (DBGVAR_DAT==i_descriptor)
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

void MainFrameController::OnFullScreen(bool i_what)
{
 //not used
}

void MainFrameController::OnActivate(bool i_state)
{
 if (false==i_state && true==HotKeysManager::GetInstance()->IsActive())
  HotKeysManager::GetInstance()->DeactivateAllHotKeys();

 if (true==i_state && false==HotKeysManager::GetInstance()->IsActive())
  HotKeysManager::GetInstance()->ActivateAllHotKeys();
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

 //Restore saved size of the window
 WndSize sz;
 settings->GetWndSize(sz);
 if (sz.m_MainFrmWnd_W != std::numeric_limits<int>::max() && sz.m_MainFrmWnd_H != std::numeric_limits<int>::max())
  mp_view->SetWindowPos(NULL, 0, 0, sz.m_MainFrmWnd_W, sz.m_MainFrmWnd_H, SWP_NOZORDER | SWP_NOMOVE);
 else
  mp_view->SetWindowPos(NULL, 0, 0, 750, 500, SWP_NOZORDER | SWP_NOMOVE); //fix issue with first launch

 mp_view->CheckOnAppSwitchDashboards(settings->GetShowExFixtures());
 mp_view->CheckOnChildCharts(settings->GetChildCharts());

 //Fill log file's fields
 LogFileFields lff;
 settings->GetLogFileFields(lff);
 int i = 0;
 m_pLogWriter->SetFieldName(i++, lff.m_fldTime);
 m_pLogWriter->SetFieldName(i++, lff.m_fldRPM);
 m_pLogWriter->SetFieldName(i++, lff.m_fldIgnTim);
 m_pLogWriter->SetFieldName(i++, lff.m_fldMAP);
 m_pLogWriter->SetFieldName(i++, lff.m_fldVBat);
 m_pLogWriter->SetFieldName(i++, lff.m_fldCLT);
 m_pLogWriter->SetFieldName(i++, lff.m_fldKnock);
 m_pLogWriter->SetFieldName(i++, lff.m_fldKnockCorr);
 m_pLogWriter->SetFieldName(i++, lff.m_fldLoadCurve);
 m_pLogWriter->SetFieldName(i++, lff.m_fldCarb);
 m_pLogWriter->SetFieldName(i++, lff.m_fldGas_v);
 m_pLogWriter->SetFieldName(i++, lff.m_fldIdleValve);
 m_pLogWriter->SetFieldName(i++, lff.m_fldPowerValve);
 m_pLogWriter->SetFieldName(i++, lff.m_fldCoolingFan);
 m_pLogWriter->SetFieldName(i++, lff.m_fldStBlock);
 m_pLogWriter->SetFieldName(i++, lff.m_fldAE);
 m_pLogWriter->SetFieldName(i++, lff.m_fldFCRevLim);
 m_pLogWriter->SetFieldName(i++, lff.m_fldFloodClear);
 m_pLogWriter->SetFieldName(i++, lff.m_fldSysLocked);
 m_pLogWriter->SetFieldName(i++, lff.m_fldCE);
 m_pLogWriter->SetFieldName(i++, lff.m_fldIgn_i);
 m_pLogWriter->SetFieldName(i++, lff.m_fldCond_i);
 m_pLogWriter->SetFieldName(i++, lff.m_fldEpas_i);
 m_pLogWriter->SetFieldName(i++, lff.m_fldAftStrEnr);
 m_pLogWriter->SetFieldName(i++, lff.m_fldIacClLoop);
 m_pLogWriter->SetFieldName(i++, lff.m_fldTPS);
 m_pLogWriter->SetFieldName(i++, lff.m_fldAdd_i1);
 m_pLogWriter->SetFieldName(i++, lff.m_fldAdd_i2);
 m_pLogWriter->SetFieldName(i++, lff.m_fldChokePos);
 m_pLogWriter->SetFieldName(i++, lff.m_fldGDPos);
 m_pLogWriter->SetFieldName(i++, lff.m_fldVehSpeed);
 m_pLogWriter->SetFieldName(i++, lff.m_fldPasDist);
 m_pLogWriter->SetFieldName(i++, lff.m_fldFuelConsum);
 m_pLogWriter->SetFieldName(i++, lff.m_fldFuelConsFreq);
 m_pLogWriter->SetFieldName(i++, lff.m_fldIAT);
 m_pLogWriter->SetFieldName(i++, lff.m_fldStrtIgnTim);
 m_pLogWriter->SetFieldName(i++, lff.m_fldIdleIgnTim);
 m_pLogWriter->SetFieldName(i++, lff.m_fldWorkIgnTim);
 m_pLogWriter->SetFieldName(i++, lff.m_fldTempIgnTim);
 m_pLogWriter->SetFieldName(i++, lff.m_fldIATIgnTim);
 m_pLogWriter->SetFieldName(i++, lff.m_fldIdlRegIgnTim);
 m_pLogWriter->SetFieldName(i++, lff.m_fldOctanCorr);
 m_pLogWriter->SetFieldName(i++, lff.m_fldEGOCorr);
 m_pLogWriter->SetFieldName(i++, lff.m_fldInjPW);
 m_pLogWriter->SetFieldName(i++, lff.m_fldTPSdot);
 m_pLogWriter->SetFieldName(i++, lff.m_fldMAP2);
 m_pLogWriter->SetFieldName(i++, lff.m_fldTmp2);
 m_pLogWriter->SetFieldName(i++, lff.m_fldDiffMAP);
 m_pLogWriter->SetFieldName(i++, lff.m_fldAFR);
 m_pLogWriter->SetFieldName(i++, lff.m_fldSynLoad);
 m_pLogWriter->SetFieldName(i++, lff.m_fldBaroPress);
 m_pLogWriter->SetFieldName(i++, lff.m_fldInjTimBeg);
 m_pLogWriter->SetFieldName(i++, lff.m_fldInjTimEnd);
 m_pLogWriter->SetFieldName(i++, lff.m_fldGrts);
 m_pLogWriter->SetFieldName(i++, lff.m_fldFtls);
 m_pLogWriter->SetFieldName(i++, lff.m_fldEgts);
 m_pLogWriter->SetFieldName(i++, lff.m_fldOps);
 m_pLogWriter->SetFieldName(i++, lff.m_fldInjDuty);
 m_pLogWriter->SetFieldName(i++, lff.m_fldRigidArg);
 m_pLogWriter->SetFieldName(i++, lff.m_fldRxlaf);
 m_pLogWriter->SetFieldName(i++, lff.m_fldMAF);
 m_pLogWriter->SetFieldName(i++, lff.m_fldLogMarks);
 m_pLogWriter->SetFieldName(i++, lff.m_fldServFlag);
 m_pLogWriter->SetFieldName(i++, lff.m_fldCECodes);
 m_pLogWriter->SetWriteFields(settings->GetWriteLogFields());
}

bool MainFrameController::OnClose(void)
{
 _GrabWindowPos(false);
 mp_view->RedrawWindow();
 return true;
}

void MainFrameController::_GrabWindowPos(bool restZoom)
{
 ISettingsData* settings = m_pAppSettingsManager->GetSettings();

 //Save state of the main window
 WndState sw;
 settings->GetWndState(sw);
 sw.m_MainFrmWnd = 1; //normal state
 if (mp_view->IsIconic())
  sw.m_MainFrmWnd = 0;
 if (mp_view->IsZoomed())
  sw.m_MainFrmWnd = 2; 
 settings->SetWndState(sw);

 bool wasZoomed = mp_view->IsZoomed();
 mp_view->ShowWindow(SW_SHOWNORMAL); //normalize window pos, otherwise saved position will be wrong
 if (!mp_view->IsIconic() && !mp_view->IsZoomed())
 {
  WndSettings ws;
  settings->GetWndSettings(ws);

  CRect rc;
  mp_view->GetWindowRect(rc);
  ws.m_MainFrmWnd_X = rc.left;
  ws.m_MainFrmWnd_Y = rc.top;

  //store last position of main window
  settings->SetWndSettings(ws);

  //store size of the window
  WndSize sz;
  settings->GetWndSize(sz);
  sz.m_MainFrmWnd_W = rc.Width();
  sz.m_MainFrmWnd_H = rc.Height();
  settings->SetWndSize(sz);
 }

 if (wasZoomed && restZoom)
  mp_view->ShowWindow(SW_MAXIMIZE);
}

void MainFrameController::OnAfterCreate(void)
{
 mp_view->ShowWindow(SW_SHOW); //show it first to ensure correct initialization

 ISettingsData* settings = m_pAppSettingsManager->GetSettings();
 WndState sw;
 settings->GetWndState(sw);

 if (sw.m_MainFrmWnd == 0)
  mp_view->ShowWindow(SW_MINIMIZE);
 if (sw.m_MainFrmWnd == 2)
  mp_view->ShowWindow(SW_MAXIMIZE);

 mp_view->UpdateWindow();

 //–азрешаем или запрещаем показавание подсказок
 CToolTipCtrlEx::ActivateAllTooltips(settings->GetShowToolTips());
 DLL::ShowHints(settings->GetShowToolTips(), settings->GetToolTipTime());  
 CToolTipCtrlEx::SetVisibleTime(settings->GetToolTipTime());

 //set check's state
 mp_view->CheckOnToggleMapWnd(settings->GetToggleMapWnd());
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
 const ISettingsData* settings = m_pAppSettingsManager->GetSettings();
 if (settings->GetPortName() == devName && false==settings->GetPortAutoReopen())
 {
  mp_view->BeginWaitCursor();
  m_pCommunicationManager->Init();
  mp_view->EndWaitCursor();
 }
}

void MainFrameController::OnAppSwitchDashboards()
{
 bool exf = m_pAppSettingsManager->GetSettings()->GetShowExFixtures();
 exf = exf ? false : true;
 mp_view->CheckOnAppSwitchDashboards(exf);
 m_pAppSettingsManager->GetSettings()->SetShowExFixtures(exf);
 m_pCommunicationManager->NotifySettingsChanged(1); //only ExFixtures check changed
}

void MainFrameController::OnAppSaveScreenshot()
{
 mp_view->RedrawWindow();
 SaveScreenshot(AfxGetMainWnd(), true);
}

void MainFrameController::OnAppSaveScreenshot5sec()
{
 m_scr5sec_timer.SetTimer(this, &MainFrameController::OnScr5SecTimer, 5000);
}

void MainFrameController::OnAppSaveSettings()
{
 m_pStatusBarManager->SetInformationText(MLL::LoadString(IDS_SAVING_APP_SETTINGS));
 mp_view->BeginWaitCursor();
 mp_view->LockWindowUpdate();
 mp_view->SetRedraw(false);
 _GrabWindowPos(true); //call it to update window position before saving of settings
 mp_view->SetRedraw(true);
 mp_view->UnlockWindowUpdate();
 mp_view->UpdateWindow();
 m_pAppSettingsManager->WriteSettings();
 mp_view->EndWaitCursor();
 m_pStatusBarManager->SetInformationText(MLL::LoadString(IDS_SAVED_APP_SETTINGS));
}

void MainFrameController::OnChildCharts()
{
 bool cch = !m_pAppSettingsManager->GetSettings()->GetChildCharts();
 mp_view->CheckOnChildCharts(cch);
 m_pAppSettingsManager->GetSettings()->SetChildCharts(cch);
 m_pCommunicationManager->NotifySettingsChanged(2); //only ChildCharts check changed
}

void MainFrameController::OnToggleMapWnd()
{
 bool tmw = !m_pAppSettingsManager->GetSettings()->GetToggleMapWnd();
 mp_view->CheckOnToggleMapWnd(tmw);
 m_pAppSettingsManager->GetSettings()->SetToggleMapWnd(tmw);
 m_pCommunicationManager->NotifySettingsChanged(3); //only Toggle map windows check changed
}

void MainFrameController::OnEditSettings()
{
 OnAppSaveSettings();
 ISettingsData* p_sett = m_pAppSettingsManager->GetSettings();
 CEditSettingsDlg esd((LPCTSTR)p_sett->GetINIFileFullName(), p_sett->GetIniEditorSyntax());
 if (IDOK==esd.DoModal())
 { //apply updated settings from a file
  mp_view->BeginWaitCursor();
  m_pAppSettingsManager->ReadSettings(); 
  mp_view->EndWaitCursor();
  if (IDYES == SECUMessageBox(MLL::LoadString(IDS_APP_SETTINGS_APPRESTART_CONFIRM), MB_YESNO | MB_DEFBUTTON1))
  {
   if (RA_ActivateRestartProcess())
    AfxGetMainWnd()->DestroyWindow();
  }
 }
}

void MainFrameController::OnHelp()
{
 TCHAR directory[MAX_PATH+1];
 HMODULE hModule = GetModuleHandle(NULL);
 ASSERT(hModule);
 _tcscpy(directory,_T(""));
 GetModuleFileName(hModule, directory, MAX_PATH);
 VERIFY(PathRemoveFileSpec(directory));
 
 CString path(directory);
 CString last_char = path.Right(1);
 if (last_char != _T("\\")) //если корневой каталог, то '\' уже есть
  path+=_T("\\");

 ISettingsData* p_sett = m_pAppSettingsManager->GetSettings();
 if (p_sett->GetInterfaceLanguage() == IL_RUSSIAN)
  path = path + _T("secu3help-ru.chm");
 else
  path = path + _T("secu3help-en.chm");

 if (!PathFileExists((LPCTSTR)path))
  SECUMessageBox(_T("File not found: ") + path);
 else
  VERIFY(::HtmlHelp(mp_view->m_hWnd, (LPCTSTR)path, HH_DISPLAY_TOPIC, NULL));
}
