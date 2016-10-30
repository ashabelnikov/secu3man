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

/** \file secu3man.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "secu3man.h"
#include "Resources/resource.h"

#include "About/Version.h"
#include "CommunicationManager.h"
#include "io-core/ccomport.h"
#include "io-core/logwriter.h"
#include "MainFrame/MainFrame.h"
#include "MainFrame/MainFrameManager.h"
#include "Settings/AppSettingsDlg.h"
#include "Settings/AppSettingsManager.h"
#include "Settings/ISettingsData.h"
#include "TablDesk/DLLLinkedFunctions.h"
#include "ui-core/HotKeysManager.h"

namespace {

typedef void (WINAPI *SETTHEMEAPPPROPERTIES)(DWORD);
void AllowVisualTheme(bool i_allow)
{
 SETTHEMEAPPPROPERTIES pSetThemeAppProperties = NULL;
 HINSTANCE handle = ::LoadLibrary(_T("UxTheme.dll"));
 if (handle)
  pSetThemeAppProperties = (SETTHEMEAPPPROPERTIES) ::GetProcAddress(handle, "SetThemeAppProperties");
 if (pSetThemeAppProperties && false==i_allow)
  pSetThemeAppProperties(0);
 if (handle)
 ::FreeLibrary(handle);
}

//Functionality of the SetThreadLocale() function is broken beginning from Windows Vista
typedef LANGID (WINAPI *SetThreadUILanguage_Addr)(LANGID LangId);
SetThreadUILanguage_Addr pLangProc = NULL;
void SetThreadLocalSettings(LANGID language, LANGID subLanguage)
{
 OSVERSIONINFOEX osver;
 ZeroMemory(&osver, sizeof(osver));
 osver.dwOSVersionInfoSize = sizeof(osver);
 GetVersionEx((OSVERSIONINFO*)&osver);
 if((osver.dwMajorVersion > 5))
 {
  if(pLangProc==NULL)
  {
   HMODULE hLibrary = LoadLibrary(_T("kernel32"));
   if (hLibrary)
    pLangProc = (SetThreadUILanguage_Addr)GetProcAddress(hLibrary, "SetThreadUILanguage");
  }
  if(pLangProc)
  { // Vista or later
   pLangProc(MAKELANGID(language, subLanguage));
  }
  else // fallback for XP and early
   ::SetThreadLocale(MAKELCID(MAKELANGID(language, subLanguage), SORT_DEFAULT));
 }
 else //XP and early
  ::SetThreadLocale(MAKELCID(MAKELANGID(language, subLanguage), SORT_DEFAULT));
}

//Check DLL compatibility
bool CheckDLLCompatibility(void)
{
 std::vector<_TSTRING> dlls;
 dlls.push_back(_T("about.dll"));
 dlls.push_back(_T("chart2d.dll"));
 dlls.push_back(_T("chart3d.dll"));
 dlls.push_back(_T("fwimpexp.dll"));
 dlls.push_back(_T("hexutils.dll"));
 dlls.push_back(_T("io-core.dll"));
 dlls.push_back(_T("midesk.dll"));
 dlls.push_back(_T("paramdesk.dll"));
 dlls.push_back(_T("tabldesk.dll"));
 dlls.push_back(_T("ui-core.dll"));

 int exe_major = 0, exe_minor = 0;
 if (!GetVersionInfo(_T("secu3man.exe"), exe_major, exe_minor))
 {
  ::MessageBox(NULL, MLL::GetString(IDS_CANT_CHECK_DLL_COMPAT).c_str(), AfxGetAppName(), MB_OK | MB_ICONWARNING);
  return false;
 }

 std::vector<_TSTRING> incompat;
 for(std::vector<_TSTRING>::const_iterator it = dlls.begin(); it != dlls.end(); ++it)
 {
  int major = 0, minor = 0;
  bool result = GetVersionInfo((*it).c_str(), major, minor);
  if (!result || 0==major || exe_major != major || exe_minor!=minor)
   incompat.push_back(*it);
 }
 if (incompat.size())
 {
  _TSTRING str(MLL::GetString(IDS_DLL_MAY_BE_INCOMPAT));
  str.append(_T("\n"));
  for(std::vector<_TSTRING>::const_iterator it = incompat.begin(); it != incompat.end(); ++it)
  {
   str.append((*it).c_str());
   str.append(_T(" "));
  }
  ::MessageBox(NULL, str.c_str(), AfxGetAppName(), MB_OK | MB_ICONWARNING);
  return false;
 }
 return true; //all DLLs are compatible
}

void SetMainWindowTitle(void)
{
 int major = 0, minor = 0;
 if (GetProductVersion(major, minor) && major > 0)
 {
  CString strTitle(MLL::LoadString(IDS_APP_TITLE));
  CString strVersion;
  strVersion.Format(_T(" v%d.%d"), major, minor);
  strTitle.Append(strVersion);
  AfxGetMainWnd()->SetWindowText(strTitle);
 }
}

//App. info
static volatile BYTE info[100] = {0x43,0x6f,0x70,0x79,0x72,0x69,0x67,0x68,0x74,0x20,0x28,0x43,0x29,0x20,0x32,0x30,
                                  0x30,0x37,0x20,0x41,0x6c,0x65,0x78,0x65,0x79,0x20,0x41,0x2e,0x20,0x53,0x68,0x61,
                                  0x62,0x65,0x6c,0x6e,0x69,0x6b,0x6f,0x76,0x2c,0x20,0x55,0x41,0x2c,0x20,0x4b,0x69,
                                  0x65,0x76,0x2c,0x20,0x68,0x74,0x74,0x70,0x3a,0x2f,0x2f,0x73,0x65,0x63,0x75,0x2d,
                                  0x33,0x2e,0x6f,0x72,0x67,0x2c,0x20,0x65,0x6d,0x61,0x69,0x6c,0x3a,0x20,0x73,0x68,
                                  0x61,0x62,0x65,0x6c,0x6e,0x69,0x6b,0x6f,0x76,0x40,0x73,0x65,0x63,0x75,0x2d,0x33,
                                  0x2e,0x6f,0x72,0x67};
}

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
 InitCommonControls();

#if (_MSC_VER <1300)
 #ifdef _AFXDLL
  Enable3dControls();           // Call this when using MFC in a shared DLL
 #else
  Enable3dControlsStatic();     // Call this when linking to MFC statically
 #endif
#endif

 SetRegistryKey(MLL::LoadString(IDS_APP_TITLE));

 //sanity check
 CheckDLLCompatibility();

 //read settings
 m_pAppSettingsManager->ReadSettings();

 //enable/disable hot keys depending on settings
 HotKeysManager::GetInstance()->Enable(m_pAppSettingsManager->GetSettings()->GetUseHotKeys());

 //load functions from dynamically linked DLL (TablDesk)
 DLL::LoadDLLsAndLinkToFunctions();

 //Localization
 DLL::SetLanguage(m_pAppSettingsManager->GetSettings()->GetInterfaceLanguage());
 switch(m_pAppSettingsManager->GetSettings()->GetInterfaceLanguage())
 {
  case IL_ENGLISH:
   SetThreadLocalSettings(LANG_ENGLISH, SUBLANG_ENGLISH_US);
   break;
  case IL_RUSSIAN:
   SetThreadLocalSettings(LANG_RUSSIAN, SUBLANG_ENGLISH_US);
   break;
 }

 //Visual theme on/off
 AllowVisualTheme(m_pAppSettingsManager->GetSettings()->GetAllowVisualTheme());

 //create main window. It must be created first of all (before any other initialization)
 m_pMainWnd = m_pMainFrameManager->GreateMainWindow();

 //initialize communication manager
 m_pCommunicationManager->Init();

 //Initialize content of main window (child controls)
 m_pMainFrameManager->Init(m_pMainWnd);

 //Set title of main window
 SetMainWindowTitle();

 return TRUE; //OK
}

BOOL CSecu3manApp::OnIdle(LONG lCount)
{
 return CWinApp::OnIdle(lCount);
}

int CSecu3manApp::ExitInstance()
{
 //save settings
 m_pAppSettingsManager->WriteSettings();

 //finish working of communication manager
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
