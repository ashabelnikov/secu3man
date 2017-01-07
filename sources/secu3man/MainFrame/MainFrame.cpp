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
 ON_COMMAND(ID_FULL_SCREEN, OnFullScreen)

 ON_UPDATE_COMMAND_UI(ID_APP_BEGIN_LOG,OnUpdateOnAppBeginLog)
 ON_UPDATE_COMMAND_UI(ID_APP_END_LOG,OnUpdateOnAppEndLog)
 ON_UPDATE_COMMAND_UI(ID_FULL_SCREEN, OnUpdateOnFullScreen)
 ON_UPDATE_COMMAND_UI(ID_APP_LOG_MARK1,OnUpdateOnAppEndLog)
 ON_UPDATE_COMMAND_UI(ID_APP_LOG_MARK2,OnUpdateOnAppEndLog)
 ON_UPDATE_COMMAND_UI(ID_APP_LOG_MARK3,OnUpdateOnAppEndLog)
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

 //Yes, it is stupid. If you know better solution, please let me know
 DPIAware dpia;
 switch(dpia.GetDPIX())
 {
  case 72:
   cs.cx = dpia.ScaleX(965);
   break;
  default:
  case 96:
   cs.cx = dpia.ScaleX(730); //OK
   break;
  case 97:
   cs.cx = dpia.ScaleX(724);
   break;
  case 98:
   cs.cx = dpia.ScaleX(716);
   break;
  case 99:
   cs.cx = dpia.ScaleX(708);
   break;
  case 100:
   cs.cx = dpia.ScaleX(703);
   break;
  case 101:
   cs.cx = dpia.ScaleX(695);
   break;
  case 102:
   cs.cx = dpia.ScaleX(690);
   break;
  case 103:
   cs.cx = dpia.ScaleX(683);
   break;
  case 104:
   cs.cx = dpia.ScaleX(676);
   break;
  case 105:
   cs.cx = dpia.ScaleX(671);
   break;
  case 106:
   cs.cx = dpia.ScaleX(665);
   break;
  case 107:
  case 108:
   cs.cx = dpia.ScaleX(657);
   break;
  case 109:
   cs.cx = dpia.ScaleX(850);
   break;
  case 110:
   cs.cx = dpia.ScaleX(842);
   break;
  case 111:
   cs.cx = dpia.ScaleX(835);
   break;
  case 112:
   cs.cx = dpia.ScaleX(826);
   break;
  case 113:
   cs.cx = dpia.ScaleX(819);
   break;
  case 114:
   cs.cx = dpia.ScaleX(812);
   break;
  case 115:
   cs.cx = dpia.ScaleX(805);
   break;
  case 116:
   cs.cx = dpia.ScaleX(795);
   break;
  case 117:
   cs.cx = dpia.ScaleX(792);
   break;
  case 118:
   cs.cx = dpia.ScaleX(788);
   break;
  case 119:
   cs.cx = dpia.ScaleX(780);
   break;
  case 120:
   cs.cx = dpia.ScaleX(765);
   break;
  case 121:
   cs.cx = dpia.ScaleX(768);
   break;
  case 122:
  case 123:
  case 124:
   cs.cx = dpia.ScaleX(765);
   break;
  case 125:
  case 126:
  case 127:
  case 128:
   cs.cx = dpia.ScaleX(745);
   break;
  case 129:
  case 130:
   cs.cx = dpia.ScaleX(722);
   break;
  case 131:
  case 132:
  case 133:
   cs.cx = dpia.ScaleX(711);
   break;
  case 134:
  case 135:
  case 136:
   cs.cx = dpia.ScaleX(695);
   break;
  case 137:
  case 138:
  case 139:
   cs.cx = dpia.ScaleX(681);
   break;
  case 140:
  case 141:
  case 142:
  case 143:
   cs.cx = dpia.ScaleX(823);
   break;
  case 144:
  case 145:
  case 146:
   cs.cx = dpia.ScaleX(795); //OK
   break;
  case 192:
   cs.cx = dpia.ScaleX(730);
   break;
 }

 switch(dpia.GetDPIY())
 {
  case 72:
   cs.cy = dpia.ScaleY(607);
   break;
  default:
  case 96:
   cs.cy = dpia.ScaleY(477);
   break;
  case 100:
   cs.cy = dpia.ScaleY(460);
   break;
  case 101:
  case 102:
   cs.cy = dpia.ScaleY(450);
   break;
  case 103:
   cs.cy = dpia.ScaleY(442);
   break;
  case 104:
  case 105:
  case 106:
  case 107:
  case 108:
   cs.cy = dpia.ScaleY(435);
   break;
  case 109:
  case 110:
   cs.cy = dpia.ScaleY(495);
   break;
  case 111:
   cs.cy = dpia.ScaleY(490);
   break;
  case 112:
  case 113:
   cs.cy = dpia.ScaleY(484);
   break;
  case 121:
   cs.cy = dpia.ScaleY(454);
   break;
  case 122:
  case 123:
  case 124:
   cs.cy = dpia.ScaleY(460);
   break;
  case 125:
  case 126:
  case 127:
  case 128:
   cs.cy = dpia.ScaleY(450);
   break;
  case 129:
  case 130:
   cs.cy = dpia.ScaleY(432);
   break;
  case 131:
  case 132:
  case 133:
   cs.cy = dpia.ScaleY(423);
   break;
  case 134:
  case 135:
  case 136:
   cs.cy = dpia.ScaleY(416);
   break;
  case 137:
  case 138:
  case 139:
   cs.cy = dpia.ScaleY(411);
   break;
 }
 
 cs.style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
 cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
 cs.dwExStyle |= WS_EX_CONTROLPARENT;
 cs.lpszClass = AfxRegisterWndClass(0);
 return TRUE;
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

void CMainFrame::setOnAskFullScreen(EventResult i_OnAskFullScreen)
{
 m_OnAskFullScreen = i_OnAskFullScreen;
}

void CMainFrame::setOnFullScreen(EventResult i_OnFullScreen)
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

void CMainFrame::OnActivateApp(BOOL bActive, DWORD dwThreadID)
{
 ASSERT(m_OnActivate);
 if (m_OnActivate)
  m_OnActivate(bActive == TRUE);
}

void CMainFrame::OnFullScreen()
{
 //=================================================================
 if (!CheckAppTitle(AfxGetMainWnd()))
  return;
 if (!CheckAppLogo())
  return;
 //=================================================================

 bool what = false;
 if (m_OnFullScreen)
  what = m_OnFullScreen();

 if (m_OnFullScreenNotify)
  m_OnFullScreenNotify(what);
}

void CMainFrame::OnUpdateOnFullScreen(CCmdUI* pCmdUI)
{
 bool enable = false;
 if (m_OnAskFullScreen)
  enable = m_OnAskFullScreen();

 pCmdUI->Enable(enable);
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