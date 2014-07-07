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

#include "stdafx.h"
#include "Resources/resource.h"
#include "common/DPIAware.h"
#include "StatusBarManager.h"
#include "ui-core/MPCLogoPane.h"
#include "ui-core/MPCStatusBar.h"

static UINT indicators[] =
{
 ID_SEPARATOR,           // status line indicator
 ID_INDICATOR_CAPS,
 ID_INDICATOR_NUM,
 ID_INDICATOR_SCRL
};

CStatusBarManager::CStatusBarManager()
: m_pParentWnd(NULL)
, m_CurrentConnectionState(STATE_OFFLINE)
, m_LogWrIcon(NULL)
, mp_wndStatusBar(new MPCStatusBar())
{
 for(int i = 0; i < 3; i++)
 {
  m_ConnIcons[i] = NULL;
  m_ConnStrings[i] = _T("");
 }
}

CStatusBarManager::~CStatusBarManager()
{
 //нет необходимости удалять контроллы добавленные в StatusBar
}

bool CStatusBarManager::Create(CWnd* pParentWnd)
{
 if (!pParentWnd)
 {
  ASSERT(pParentWnd); //надо передать родительское окно!
  return false;
 }

 m_pParentWnd = pParentWnd;

 //создаем StatusBar и добавляем в него стандартные поля (индикаторы)
 if (!mp_wndStatusBar->Create(pParentWnd,WS_CHILD|WS_VISIBLE|CBRS_BOTTOM,ID_MAIN_STATUS_BAR) ||
     !mp_wndStatusBar->SetIndicators(indicators,sizeof(indicators)/sizeof(UINT)))
 {
  TRACE0("Failed to create status bar\n");
  return false;      // fail to create
 }

 //подгружаем из ресурсов иконки и строки
 m_ConnIcons[0] = ::LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_CONN_CONNECTED));
 m_ConnIcons[1] = ::LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_CONN_DISCONNECTED));
 m_ConnIcons[2] = ::LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_CONN_BOOTLOADER));
 m_LogWrIcon = ::LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_LOGWR_WRITE));

 m_ConnStrings[0].LoadString(IDS_CONN_CONNECTED);
 m_ConnStrings[1].LoadString(IDS_CONN_DISCONNECTED);
 m_ConnStrings[2].LoadString(IDS_CONN_BOOTLOADER);

 return true;
}

//добавление в StatusBar дополнительных полей
void CStatusBarManager::AddContent(void)
{
 int idx = 0;
 DPIAware dpia;

 //логотип SECU-3
 mp_wndStatusBar->AddIndicator(0,ID_MSB_INDICATOR_LOGO);
 idx = mp_wndStatusBar->CommandToIndex(ID_MSB_INDICATOR_LOGO);
 mp_wndStatusBar->SetPaneWidth(idx,60);
 mp_wndStatusBar->SetPaneStyle(idx, mp_wndStatusBar->GetPaneStyle(idx) | SBPS_NOBORDERS );
 MPCLogoPane* pLogo = new MPCLogoPane;
 pLogo->Create(MLL::LoadString(IDS_STATUS_BAR_LOGO),WS_CHILD|WS_VISIBLE,mp_wndStatusBar.get(),ID_MSB_INDICATOR_LOGO);
 pLogo->SetLogoFont(_T("Arial"), 18);
 mp_wndStatusBar->AddControl(pLogo,ID_MSB_INDICATOR_LOGO);

 //информационное поле (используем ID_SEPARATOR)
 idx = mp_wndStatusBar->CommandToIndex(ID_SEPARATOR);
 mp_wndStatusBar->SetPaneWidth(idx, 80);
 mp_wndStatusBar->SetPaneStyle(idx, mp_wndStatusBar->GetPaneStyle(idx) | SBPS_NOBORDERS );
 mp_wndStatusBar->SetPaneText(idx,_T(" "));

 //прогресс бар
 mp_wndStatusBar->AddIndicator(2,ID_MSB_INDICATOR_PROGRESS);
 idx = mp_wndStatusBar->CommandToIndex(ID_MSB_INDICATOR_PROGRESS);
 mp_wndStatusBar->SetPaneWidth(idx,dpia.ScaleX(80));
 mp_wndStatusBar->SetPaneStyle(idx, mp_wndStatusBar->GetPaneStyle(idx) | SBPS_NOBORDERS );
 CProgressCtrl* progress = new CProgressCtrl;
 progress->Create(WS_CHILD,CRect(0,0,0,0),mp_wndStatusBar.get(),ID_MSB_INDICATOR_PROGRESS);
 mp_wndStatusBar->AddControl(progress,ID_MSB_INDICATOR_PROGRESS);

 //проценты
 mp_wndStatusBar->AddIndicator(3,ID_MSB_INDICATOR_PERCENT);
 idx = mp_wndStatusBar->CommandToIndex(ID_MSB_INDICATOR_PERCENT);
 mp_wndStatusBar->SetPaneWidth(idx,dpia.ScaleX(30));
 mp_wndStatusBar->SetPaneStyle(idx, mp_wndStatusBar->GetPaneStyle(idx) | SBPS_NOBORDERS );

 //иконка коннекта
 mp_wndStatusBar->AddIndicator(4,ID_MSB_INDICATOR_ICON);
 idx = mp_wndStatusBar->CommandToIndex(ID_MSB_INDICATOR_ICON);
 mp_wndStatusBar->SetPaneWidth(idx,16);
 mp_wndStatusBar->SetPaneStyle(idx, SBPS_NORMAL);

 //текст коннекта
 mp_wndStatusBar->AddIndicator(5,ID_MSB_INDICATOR_CONNTXT);
 idx = mp_wndStatusBar->CommandToIndex(ID_MSB_INDICATOR_CONNTXT);
 mp_wndStatusBar->SetPaneWidth(idx,dpia.ScaleX(80));
 mp_wndStatusBar->SetPaneStyle(idx, SBPS_NORMAL);

 //немного пустого пространства
 mp_wndStatusBar->AddIndicator(6,ID_MSB_INDICATOR_EMPTY);
 idx = mp_wndStatusBar->CommandToIndex(ID_MSB_INDICATOR_EMPTY);
 mp_wndStatusBar->SetPaneWidth(idx,10);
 mp_wndStatusBar->SetPaneStyle(idx, mp_wndStatusBar->GetPaneStyle(idx) | SBPS_NOBORDERS );

 //иконка индицирующая статус записи логов
 mp_wndStatusBar->AddIndicator(7,ID_MSB_INDICATOR_LOGWR);
 idx = mp_wndStatusBar->CommandToIndex(ID_MSB_INDICATOR_LOGWR);
 mp_wndStatusBar->SetPaneWidth(idx,16);
 mp_wndStatusBar->SetPaneStyle(idx, SBPS_NORMAL);

 //дальше расположены: CAP, NUM, SCRL
}


//Устанавливает текущую иконку и текст (состояние соединения)
void CStatusBarManager::SetConnectionState(int i_State)
{
 int idx_icon = mp_wndStatusBar->CommandToIndex(ID_MSB_INDICATOR_ICON);
 int idx_str =  mp_wndStatusBar->CommandToIndex(ID_MSB_INDICATOR_CONNTXT);

 CStatusBarCtrl& status = mp_wndStatusBar->GetStatusBarCtrl();

 switch(i_State)
 {
  case STATE_ONLINE:
   status.SetIcon(idx_icon,m_ConnIcons[0]);
   mp_wndStatusBar->SetPaneText(idx_str,m_ConnStrings[0]);
   m_CurrentConnectionState = i_State;
   break;
  case STATE_OFFLINE:
   status.SetIcon(idx_icon,m_ConnIcons[1]);
   mp_wndStatusBar->SetPaneText(idx_str,m_ConnStrings[1]);
   m_CurrentConnectionState = i_State;
   break;
  case STATE_BOOTLOADER:
   status.SetIcon(idx_icon,m_ConnIcons[2]);
   mp_wndStatusBar->SetPaneText(idx_str,m_ConnStrings[2]);
   m_CurrentConnectionState = i_State;
   break;
 }
}

//показывает/прячет ProgressCtrl
void CStatusBarManager::ShowProgressBar(bool show)
{
 MPCStatusBarPaneControlInfo* info = mp_wndStatusBar->GetPanControl(ID_MSB_INDICATOR_PROGRESS);
 ASSERT(info);
 int nCmdShow = ((show) ? SW_SHOW : SW_HIDE);
 info->m_hWnd->ShowWindow(nCmdShow);
 if (nCmdShow == SW_HIDE)
 { //очищаем текстовое значение
  int idx = mp_wndStatusBar->CommandToIndex(ID_MSB_INDICATOR_PERCENT);
  mp_wndStatusBar->SetPaneText(idx,_T(""));
 }
}

//устанавливает диапазон для ProgressCtrl
void CStatusBarManager::SetProgressRange(int nLower, int nUpper)
{
 MPCStatusBarPaneControlInfo* info = mp_wndStatusBar->GetPanControl(ID_MSB_INDICATOR_PROGRESS);
 ASSERT(info);
 ((CProgressCtrl*)info->m_hWnd)->SetRange32(nLower, nUpper);
}

//устанавливает текущее значение для ProgressCtrl и текстовое значение
void CStatusBarManager::SetProgressPos(int nPos)
{
 MPCStatusBarPaneControlInfo* info = mp_wndStatusBar->GetPanControl(ID_MSB_INDICATOR_PROGRESS);
 ASSERT(info);
 CProgressCtrl* pProgress = static_cast<CProgressCtrl*>(info->m_hWnd);
 pProgress->SetPos(nPos);
 int idx = mp_wndStatusBar->CommandToIndex(ID_MSB_INDICATOR_PERCENT);

 int nLower,nUpper;
 pProgress->GetRange(nLower,nUpper);

 int percents = ((100 * nPos) / (nUpper - nLower));
 CString str;
 str.Format(_T("%d%s"),percents,_T("%"));
 mp_wndStatusBar->SetPaneText(idx,str);
}

//Устанавливает информационный текст
void CStatusBarManager::SetInformationText(const CString& i_text)
{
 int idx = mp_wndStatusBar->CommandToIndex(ID_SEPARATOR);
 mp_wndStatusBar->SetPaneText(idx,i_text);
}

//Устанавливает текущую иконку и текст (состояние соединения)
void CStatusBarManager::SetLoggerState(int i_state)
{
 int idx_icon = mp_wndStatusBar->CommandToIndex(ID_MSB_INDICATOR_LOGWR);
 CStatusBarCtrl& status = mp_wndStatusBar->GetStatusBarCtrl();

 switch(i_state)
 {
  case LOG_STATE_WRITING:
   status.SetIcon(idx_icon,m_LogWrIcon);
   break;
  case LOG_STATE_STOPPED:
   status.SetIcon(idx_icon,NULL);
   break;
 }
}
