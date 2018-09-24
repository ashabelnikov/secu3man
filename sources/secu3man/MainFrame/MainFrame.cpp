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

/** \file MainFrame.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include <dbt.h>
#include "Resources/resource.h"
#include "MainFrame.h"
#include "common/DPIAware.h"
#include "MIDesk/DVDeskDlg.h"
#include "About/secu-3about.h"

#include <limits>
#include <vector>
#include "ChildView.h"

#undef max                        //avoid conflicts with C++

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
 ON_WM_CREATE()
 ON_WM_SETFOCUS()
 ON_WM_CLOSE()
 ON_WM_GETMINMAXINFO()
 ON_WM_SYSCOMMAND()
 ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
 ON_COMMAND(ID_APP_DEV_SITE, OnAppDevSite)
 ON_COMMAND(ID_APP_DEV_FORUM, OnAppDevForum)
 ON_COMMAND(ID_APP_VK, OnAppVK)
 ON_COMMAND(ID_APP_FB, OnAppFB)
 ON_COMMAND(ID_APP_SETTINGS, OnAppSettings)
 ON_COMMAND(ID_APP_BEGIN_LOG, OnAppBeginLog)
 ON_COMMAND(ID_APP_END_LOG, OnAppEndLog)
 ON_COMMAND(ID_APP_LOG_MARK1, OnAppLogMark1)
 ON_COMMAND(ID_APP_LOG_MARK2, OnAppLogMark2)
 ON_COMMAND(ID_APP_LOG_MARK3, OnAppLogMark3)
 ON_COMMAND(ID_APP_LOG_FORMAT, OnAppLogFormat)
 ON_COMMAND(ID_APP_SWITCH_DASHBOARD, OnAppSwitchDashboards)
 ON_COMMAND(ID_APP_SAVESCR, OnAppSaveScreenshot)
 ON_COMMAND(ID_APP_SAVEINI, OnAppSaveSettings)
 ON_UPDATE_COMMAND_UI(ID_APP_BEGIN_LOG,OnUpdateOnAppBeginLog)
 ON_UPDATE_COMMAND_UI(ID_APP_END_LOG,OnUpdateOnAppEndLog)
 ON_UPDATE_COMMAND_UI(ID_APP_LOG_MARK1,OnUpdateOnAppEndLog)
 ON_UPDATE_COMMAND_UI(ID_APP_LOG_MARK2,OnUpdateOnAppEndLog)
 ON_UPDATE_COMMAND_UI(ID_APP_LOG_MARK3,OnUpdateOnAppEndLog)
 ON_UPDATE_COMMAND_UI(ID_APP_SWITCH_DASHBOARD,OnUpdateOnAppSwitchDashboards)
 ON_WM_ACTIVATEAPP()
 ON_WM_DEVICECHANGE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
: m_pwndView(NULL)
, m_bDoIdle(TRUE)
{
 //empty
}

CMainFrame::~CMainFrame()
{
 //empty
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
 if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
  return -1;

 m_createSize.cx = lpCreateStruct->cx;
 m_createSize.cy = lpCreateStruct->cy;

 if (m_OnCreate)
  m_OnCreate();  //notify controller about creation of this window
 return 0;
}

bool CMainFrame::CreateDVDesk(bool create)
{
 bool result = false;
 if (create)
 {
  mp_DVDeskDlg = std::auto_ptr<CDVDeskDlg>(new CDVDeskDlg());
  result = mp_DVDeskDlg->Create(CDVDeskDlg::IDD, this);
  mp_DVDeskDlg->SetWindowPos(&wndTop, 0,0,0,0, SWP_NOSIZE | SWP_NOMOVE);
  mp_DVDeskDlg->Show(false);
 }
 else
 {
  result = true;
  if (mp_DVDeskDlg.get())
   result = mp_DVDeskDlg->DestroyWindow();
  mp_DVDeskDlg = std::auto_ptr<CDVDeskDlg>();
 }
 return result;
}

//хак:
BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
 MSG msg;
 if(!::PeekMessage(&msg, NULL, NULL, NULL, PM_NOREMOVE) && m_bDoIdle)
 {
  //этот код вызывается ТОЛЬКО один раз и ТОЛЬКО когда нет сообщений в очереди
  if (_UpdateTopLevelMainMenu())
   DrawMenuBar(); //redraw menu

  m_bDoIdle = FALSE; //remember that we already called redraw function
 }
 else
 {
  if(AfxGetApp()->IsIdleMessage(pMsg) && pMsg->message != 0x3FC)
   m_bDoIdle = TRUE;
 }

 return CFrameWnd::PreTranslateMessage(pMsg);
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
 if( !CFrameWnd::PreCreateWindow(cs) )
  return FALSE;

 //устанавливаем позицию главного окна запомненную ранее, если это возможно
 if (m_OnGetInitialPos)
 {
  CPoint point;
  m_OnGetInitialPos(point);

  if (point.x != std::numeric_limits<int>::max() && point.y != std::numeric_limits<int>::max())
  {
   cs.x = point.x;
   cs.y = point.y;
  }
 }

 //set default size of window
 DPIAware dpia;
 cs.cx = dpia.ScaleX(750);
 cs.cy = dpia.ScaleY(500);

 cs.style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME;
 cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
 cs.dwExStyle |= WS_EX_CONTROLPARENT;
 cs.lpszClass = AfxRegisterWndClass(0);
 return TRUE;
}

void CMainFrame::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
 lpMMI->ptMinTrackSize.x = m_createSize.cx;
 lpMMI->ptMinTrackSize.y = m_createSize.cy;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
 CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
 CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers
void CMainFrame::OnSetFocus(CWnd* pOldWnd)
{
 // forward focus to the view window
 if (m_pwndView)
  m_pwndView->SetFocus();
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
 return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CMainFrame::SetView(CChildView* i_pChildView)
{
 m_pwndView = i_pChildView;
}

void CMainFrame::addOnClose(EventResult i_OnClose)
{
 m_OnClose.push_back(i_OnClose);
}

void CMainFrame::setOnFullScreen(EventHandler1 i_OnFullScreen)
{
 m_OnFullScreen = i_OnFullScreen;
}

void CMainFrame::setOnFullScreenNotify(EventHandler1 i_OnFullScreenNotify)
{
 m_OnFullScreenNotify = i_OnFullScreenNotify;
}

void CMainFrame::setOnActivate(EventHandler1 i_OnActivate)
{
 m_OnActivate = i_OnActivate;
}

void CMainFrame::setOnCreate(EventHandler i_OnCreate)
{
 m_OnCreate = i_OnCreate;
}

void CMainFrame::setOnAppAbout(EventHandler i_OnFunction)
{
 m_OnAppAbout = i_OnFunction;
}

void CMainFrame::setOnAppDevSite(EventHandler i_OnFunction)
{
 m_OnAppDevSite = i_OnFunction;
}

void CMainFrame::setOnAppDevForum(EventHandler i_OnFunction)
{
 m_OnAppDevForum = i_OnFunction;
}

void CMainFrame::setOnAppVK(EventHandler i_OnFunction)
{
 m_OnAppVK = i_OnFunction;
}

void CMainFrame::setOnAppFB(EventHandler i_OnFunction)
{
 m_OnAppFB = i_OnFunction;
}

void CMainFrame::setOnAppSettings(EventHandler i_OnFunction)
{
 m_OnAppSettings = i_OnFunction;
}

void CMainFrame::setOnAppBeginLog(EventHandler i_OnFunction)
{
 m_OnAppBeginLog = i_OnFunction;
}

void CMainFrame::setOnAppEndLog(EventHandler i_OnFunction)
{
 m_OnAppEndLog = i_OnFunction;
}

void CMainFrame::setIsBeginLoggingAllowed(EventResult i_OnFunction)
{
 m_IsBeginLoggingAllowed = i_OnFunction;
}

void CMainFrame::setIsEndLoggingAllowed(EventResult i_OnFunction)
{
 m_IsEndLoggingAllowed = i_OnFunction;
}

void CMainFrame::setOnGetInitialPos(EventHandler2 i_OnGetInitialPos)
{
 m_OnGetInitialPos = i_OnGetInitialPos;
}

void CMainFrame::setOnPortDevArrived(EventHandler3 i_OnPortDevArrived)
{
 m_OnPortDevArrived = i_OnPortDevArrived;
}

void CMainFrame::setOnAppLogMark(EventHandler4 i_OnFunction)
{
 m_OnAppLogMark = i_OnFunction;
}

void CMainFrame::setOnAppLogFormat(EventHandler i_OnFunction)
{
 m_OnAppLogFormat = i_OnFunction;
}

void CMainFrame::setOnCloseNotify(EventHandler i_OnFunction)
{
 m_OnCloseNotify = i_OnFunction;
}

void CMainFrame::setOnAppSwitchDashboards(EventHandler i_OnFunction)
{
 m_OnSwitchDashboards = i_OnFunction;
}

void CMainFrame::setOnAppSaveScreenshot(EventHandler i_OnFunction)
{
 m_OnSaveScreenshot = i_OnFunction;
}

void CMainFrame::setOnAppSaveSettings(EventHandler i_OnFunction)
{
 m_OnSaveSettings = i_OnFunction;
}

void CMainFrame::OnClose()
{
 bool result = true;

 //все ли согласны с закрытием программы?
 for (size_t i = 0; i < m_OnClose.size(); ++i)
  if (m_OnClose[i] && !m_OnClose[i]())
    result = false;

 if (result)
 {
  if (m_OnCloseNotify)
   m_OnCloseNotify();   //notify controller about application is being closed
  CFrameWnd::OnClose();
 }
}

void CMainFrame::OnAppAbout()
{
 if (m_OnAppAbout)
  m_OnAppAbout();
}

void CMainFrame::OnAppDevSite()
{
 if (m_OnAppDevSite)
  m_OnAppDevSite();
}

void CMainFrame::OnAppDevForum()
{
 if (m_OnAppDevForum)
  m_OnAppDevForum();
}

void CMainFrame::OnAppVK()
{
 if (m_OnAppVK)
  m_OnAppVK();
}

void CMainFrame::OnAppFB()
{
 if (m_OnAppFB)
  m_OnAppFB();
}

void CMainFrame::OnAppSettings()
{
 //=================================================================
 if (!CheckAppTitle(AfxGetMainWnd()))
  return;
 if (!CheckAppLogo())
  return;
 //=================================================================

 if (m_OnAppSettings)
  m_OnAppSettings();
}

void CMainFrame::OnAppBeginLog()
{
 //=================================================================
 if (!CheckAppTitle(AfxGetMainWnd()))
  return;
 if (!CheckAppLogo())
  return;
 //=================================================================

 if (m_OnAppBeginLog)
  m_OnAppBeginLog();
}

void CMainFrame::OnAppEndLog()
{
 //=================================================================
 if (!CheckAppTitle(AfxGetMainWnd()))
  return;
 if (!CheckAppLogo())
  return;
 //=================================================================

 if (m_OnAppEndLog)
  m_OnAppEndLog();
}

void CMainFrame::OnAppLogMark1()
{
 if (m_OnAppLogMark)
  m_OnAppLogMark(1);
}

void CMainFrame::OnAppLogMark2()
{
 if (m_OnAppLogMark)
  m_OnAppLogMark(2);
}

void CMainFrame::OnAppLogMark3()
{
 if (m_OnAppLogMark)
  m_OnAppLogMark(4);
}

void CMainFrame::OnAppLogFormat()
{
 if (m_OnAppLogFormat)
  m_OnAppLogFormat();
}

void CMainFrame::OnUpdateOnAppBeginLog(CCmdUI* pCmdUI)
{
 BOOL enable = FALSE;
 if (m_IsBeginLoggingAllowed)
  enable = m_IsBeginLoggingAllowed();
 pCmdUI->Enable(enable);
}

void CMainFrame::OnUpdateOnAppEndLog(CCmdUI* pCmdUI)
{
 BOOL enable = FALSE;
 if (m_IsEndLoggingAllowed)
  enable = m_IsEndLoggingAllowed();
 pCmdUI->Enable(enable);
}

void CMainFrame::OnUpdateOnAppSwitchDashboards(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(true);
}

void CMainFrame::OnActivateApp(BOOL bActive, DWORD dwThreadID)
{
 ASSERT(m_OnActivate);
 if (m_OnActivate)
  m_OnActivate(bActive == TRUE);
}

void CMainFrame::OnSysCommand(UINT nID, LPARAM lParam)
{
 Super::OnSysCommand(nID, lParam);

 bool what = false;
 if ((nID == SC_MAXIMIZE) || (nID == SC_RESTORE))
 {
  DWORD style = GetStyle();
  what = (style & WS_MAXIMIZE);
 }
 else
  return; //do nothing

 //=================================================================
 if (!CheckAppTitle(AfxGetMainWnd()))
  return;
 if (!CheckAppLogo())
  return;
 //=================================================================

 if (m_OnFullScreen)
  m_OnFullScreen(what);  //notify controller

 if (m_OnFullScreenNotify)
  m_OnFullScreenNotify(what);
}

//Стандартный вызов ON_UPDATE_COMMAND_UI запрещает элементы меню, но не делает их grayed.
//Чтобы все таки запрещенные элементы меню выглядели как grayed, нужно вызывать эту функцию
//Note: эта функция не перерисовывает меню.
//Возвращает true - если произошли изменения и нужно перерисовать меню, иначе false
bool CMainFrame::_UpdateTopLevelMainMenu(void)
{
 CMenu* pMenu = this->GetMenu();
 std::vector<UINT> old_menu_state;

 //remember old states of all menu items
 {
  int menu_items_count = pMenu->GetMenuItemCount();
  for(int i = 0; i < menu_items_count; ++i)
   old_menu_state.push_back(pMenu->GetMenuState(pMenu->GetMenuItemID(i),MF_BYCOMMAND));
 }

 //Perform update
 CCmdUI ui;
 ui.m_nIndexMax = pMenu->GetMenuItemCount();
 ui.m_pMenu = pMenu;
 for (ui.m_nIndex = 0; ui.m_nIndex < ui.m_nIndexMax; ui.m_nIndex++)
 {
  ui.m_nID = pMenu->GetMenuItemID(ui.m_nIndex);
  ui.DoUpdate(this, m_bAutoMenuEnable);
 }

 //Check: do we need to redraw menu?
 {
  int menu_items_count = pMenu->GetMenuItemCount();
  for(int i = 0; i < menu_items_count; ++i)
  {
   UINT old_s = old_menu_state[i];
   UINT new_s = pMenu->GetMenuState(pMenu->GetMenuItemID(i),MF_BYCOMMAND);
   if (old_s!=new_s)
    return true; //caller should redraw main menu
  }
 }
 //nothing changed: redraw is not needed
 return false;
}

CDVDeskDlg* CMainFrame::GetDVDesk(void)
{
 return mp_DVDeskDlg.get();
}

BOOL CMainFrame::OnDeviceChange(UINT nEventType, DWORD_PTR dwData)
{
 if (DBT_DEVICEARRIVAL==nEventType)
 {
  if (DBT_DEVTYP_PORT==((DEV_BROADCAST_HDR*)dwData)->dbch_devicetype)
  {
   DEV_BROADCAST_PORT* pPort = (DEV_BROADCAST_PORT*)dwData;
   if (m_OnPortDevArrived)
    m_OnPortDevArrived(_TSTRING(pPort->dbcp_name));
  } 
 }
 return TRUE;
}

void CMainFrame::OnAppSwitchDashboards()
{
 if (m_OnSwitchDashboards)
  m_OnSwitchDashboards();
}

void CMainFrame::CheckOnAppSwitchDashboards(bool checked)
{
 GetMenu()->CheckMenuItem(ID_APP_SWITCH_DASHBOARD, (checked ? MF_CHECKED : MF_UNCHECKED) | MF_BYCOMMAND);
}

void CMainFrame::OnAppSaveScreenshot()
{
 if (m_OnSaveScreenshot)
  m_OnSaveScreenshot();
}

void CMainFrame::OnAppSaveSettings()
{
 if (m_OnSaveSettings)
  m_OnSaveSettings();
}
