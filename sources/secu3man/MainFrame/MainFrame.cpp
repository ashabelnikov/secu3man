/* SECU-3  - An open source, free engine control unit
   Copyright (C) 2007 Alexey A. Shabelnikov. Ukraine, Gorlovka

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

#include "stdafx.h"
#include "Resources/resource.h"
#include "MainFrame.h"
#include "common/DPIAware.h"
#include "MIDesk/DVDeskDlg.h"

#include <limits>
#include <vector>
#include "ChildView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#undef max                        //avoid conflicts with C++

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
 ON_WM_CREATE()
 ON_WM_SETFOCUS()
 ON_WM_CLOSE()
 ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
 ON_COMMAND(ID_APP_SETTINGS, OnAppSettings)
 ON_COMMAND(ID_APP_BEGIN_LOG, OnAppBeginLog)
 ON_COMMAND(ID_APP_END_LOG, OnAppEndLog)
 ON_COMMAND(ID_FULL_SCREEN, OnFullScreen)
 ON_UPDATE_COMMAND_UI(ID_APP_BEGIN_LOG,OnUpdateOnAppBeginLog)
 ON_UPDATE_COMMAND_UI(ID_APP_END_LOG,OnUpdateOnAppEndLog)
 ON_UPDATE_COMMAND_UI(ID_FULL_SCREEN, OnUpdateOnFullScreen)
 ON_WM_ACTIVATEAPP()
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

  m_bDoIdle = FALSE; //запоминаем что уже вызвали
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

 //устанавливаем необходимый размер главного окна. Windows непонятным образо м масштабирует диалоги (непропорционально DPI), поэтому
 //для каждого DPI подгоняем размеры
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
  case 120:
   cs.cx = dpia.ScaleX(765);
   break;
  case 144:
   cs.cx = dpia.ScaleX(795); //OK
   break;
  case 192:
   cs.cx = dpia.ScaleX(730);
   break;
 }
 switch(dpia.GetDPIY())
 {
  case 72:
   cs.cy = dpia.ScaleY(580);
   break;
  default:
  case 96:
   cs.cy = dpia.ScaleY(455);
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

void CMainFrame::OnClose()
{
 bool result = true;

 //все ли согласны с закрытием программы?
 for (size_t i = 0; i < m_OnClose.size(); ++i)
  if (m_OnClose[i] && !m_OnClose[i]())
    result = false;

 if (result)
  CFrameWnd::OnClose();
}

void CMainFrame::OnAppAbout()
{
 if (m_OnAppAbout)
  m_OnAppAbout();
}

void CMainFrame::OnAppSettings()
{
 if (m_OnAppSettings)
  m_OnAppSettings();
}

void CMainFrame::OnAppBeginLog()
{
 if (m_OnAppBeginLog)
  m_OnAppBeginLog();
}

void CMainFrame::OnAppEndLog()
{
 if (m_OnAppEndLog)
  m_OnAppEndLog();
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
