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

/** \file TablesDeskDlg.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "Resources/resource.h"
#include "TablesDeskDlg.h"

#include <map>
#include "common/Dll.h"
#include "common/FastDelegate.h"
#include "TablDesk/ButtonsPanel.h"
#include "io-core/MapIds.h"
#include "TDContextMenuManager.h"
#include "ui-core/ToolTipCtrlEx.h"
#include "ui-core/WndScroller.h"

using namespace std;
using namespace fastdelegate;

/////////////////////////////////////////////////////////////////////////////
// CTablesDeskDlg dialog

UINT CTablesDeskDlg::IDD = IDD_TABLES_DESK;

CTablesDeskDlg::CTablesDeskDlg(CWnd* pParent /*=NULL*/)
: Super(CTablesDeskDlg::IDD, pParent)
, m_enabled(false)
, mp_ContextMenuManager(new CTDContextMenuManager())
, m_tsneb_readonly(false)
, m_lock_enchange(false)
, m_lock_killfocus(true)
, mp_ButtonsPanel(new CButtonsPanel(true, true)) //with autotune, online mode
{
 //их надо создать только один раз
 mp_ButtonsPanel->setOnMapChanged(MakeDelegate(this, &CTablesDeskDlg::OnMapChanged));
 mp_ButtonsPanel->setOnCloseMapWnd(MakeDelegate(this, &CTablesDeskDlg::OnCloseMapWnd));
 mp_ButtonsPanel->setOnOpenMapWnd(MakeDelegate(this, &CTablesDeskDlg::OnOpenMapWnd));
 mp_ButtonsPanel->setOnWndActivation(MakeDelegate(this, &CTablesDeskDlg::OnWndActivation));
 mp_ButtonsPanel->setIsAllowed(MakeDelegate(this, &CTablesDeskDlg::IsAllowed));

 mp_ContextMenuManager->CreateContent();
}

CTablesDeskDlg::~CTablesDeskDlg()
{
  //empty
}

void CTablesDeskDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);
 DDX_Control(pDX, IDC_TABLES_DESK_TITLE, m_td_title);
 DDX_Control(pDX, IDC_TD_SAVE_BUTTON, m_save_button);
 DDX_Control(pDX, IDC_TD_TABLESSET_NAME_EDIT, m_names_edit);
 DDX_Control(pDX, IDC_TD_MODIFICATION_FLAG, m_midification_flag);
 DDX_Text(pDX, IDC_TD_TABLESSET_NAME_EDIT, m_names_text);
}

BEGIN_MESSAGE_MAP(CTablesDeskDlg, Super)
 ON_WM_DESTROY()
/* ON_WM_SYSCOMMAND()*/
 ON_WM_CONTEXTMENU()
 ON_WM_SIZE()
 ON_UPDATE_COMMAND_UI(IDC_TABLES_DESK_TITLE, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_TD_SAVE_BUTTON, OnUpdateControls)
 ON_UPDATE_COMMAND_UI_RANGE(IDM_TD_LOAD_NAMES_RESERVED0, IDM_TD_LOAD_NAMES_RESERVED15, OnUpdateControls)
 ON_UPDATE_COMMAND_UI_RANGE(IDM_TD_SAVE_NAMES_RESERVED0, IDM_TD_SAVE_NAMES_RESERVED5, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDM_TD_IMPORT_FROM_S3F, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDM_TD_EXPORT_TO_S3F, OnUpdateControls)

 ON_BN_CLICKED(IDC_TD_SAVE_BUTTON, OnSaveButton)
 ON_COMMAND_RANGE(IDM_TD_LOAD_NAMES_RESERVED0, IDM_TD_LOAD_NAMES_RESERVED15, OnRangeCmdsLoad)
 ON_COMMAND_RANGE(IDM_TD_SAVE_NAMES_RESERVED0, IDM_TD_SAVE_NAMES_RESERVED5, OnRangeCmdsSave)
 ON_COMMAND(IDM_TD_IMPORT_FROM_S3F, OnImportFromS3F)
 ON_COMMAND(IDM_TD_EXPORT_TO_S3F, OnExportToS3F)

 ON_EN_CHANGE(IDC_TD_TABLESSET_NAME_EDIT, OnChangeTablesSetName)
 ON_UPDATE_COMMAND_UI(IDC_TD_TABLESSET_NAME_EDIT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_TD_TABLESSET_NAME_TITLE, OnUpdateControls)
 ON_EN_KILLFOCUS(IDC_TD_TABLESSET_NAME_EDIT, OnEditKillFocus)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CTablesDeskDlg message handlers

BOOL CTablesDeskDlg::OnInitDialog()
{
 Super::OnInitDialog();
 m_lock_killfocus = true;

 CRect rect;
 GetDlgItem(IDC_TD_BUTTONS_PANEL_HOLDER)->GetWindowRect(rect);
 ScreenToClient(rect);

 mp_ButtonsPanel->Create(this);
 mp_ButtonsPanel->SetPosition(rect.left, rect.top, &m_names_edit);
 mp_ButtonsPanel->ShowWindow(SW_SHOW);
 mp_ButtonsPanel->MoveWindow(rect); //Set window size to size of holder

 m_names_edit.SetReadOnly(m_tsneb_readonly);
 m_names_edit.SetLimitText(16);

 mp_ContextMenuManager->Attach(this);

 UpdateData(FALSE);
 UpdateDialogControls(this, TRUE);

 ///////////////////////////////////////////////
 //TODO: Replace by MIHelpers (see MIHelpers.h) or even more all releted functions to common
 //Calculate and remember right and bottom margins of buttons panel control
 CRect wndRectDlg, wndRectBP;
 GetClientRect(&wndRectDlg);
 mp_ButtonsPanel->GetWindowRect(&wndRectBP);
 ScreenToClient(&wndRectBP);
 m_bpLRMargin.cx = wndRectDlg.right - wndRectBP.right;
 m_bpLRMargin.cy = wndRectDlg.bottom - wndRectBP.bottom;
 ///////////////////////////////////////////////

 //Create tooltip control and set tooltips for widgets
 mp_ttc.reset(new CToolTipCtrlEx());
 VERIFY(mp_ttc->Create(this, WS_POPUP | TTS_ALWAYSTIP | TTS_BALLOON));
 VERIFY(mp_ttc->AddWindow(&m_save_button, MLL::GetString(IDS_TD_SAVE_BUTTON_TT)));
 VERIFY(mp_ttc->AddWindow(&m_names_edit, MLL::GetString(IDS_TD_TABLESSET_NAME_EDIT_TT)));
 mp_ttc->SetMaxTipWidth(250); //Set width for text wrapping
 mp_ttc->ActivateToolTips(true);

 return TRUE;  // return TRUE unless you set the focus to a control
}

BOOL CTablesDeskDlg::PreTranslateMessage(MSG* pMsg)
{
 //MS says:
 //The default WM_LBUTTONDOWN handler for edit controls will not set focus to the edit control if its parent
 //is an inactive captioned child window. This code is implemented as part of the Windows API function, DefWindowProc().
 //This behavior is by design.
/*
 //This code is needed only when parent window of control has WS_CHILD style and has caption bar
 if (pMsg->message==WM_LBUTTONDOWN)
 {
  TCHAR name[32];
  GetClassName(pMsg->hwnd, name, 31);
  //Set focus for edit controls which belong only to this dialog
  if (!_tcscmp(name, _T("Edit")) && ::GetParent(pMsg->hwnd) == this->m_hWnd)
   ::SetFocus(pMsg->hwnd);
 }*/

 return Super::PreTranslateMessage(pMsg);
}

void CTablesDeskDlg::OnEditKillFocus()
{
 if (m_lock_killfocus)
  return; //we need to prevent wrong behaviour when window is recreated
 if (0==m_names_edit.GetWindowTextLength())
 {
  m_names_text = _T("<no name>");
  UpdateData(FALSE);
  //allow controller process latest changes
  if (m_OnChangeTablesSetName && false==m_lock_enchange)
   m_OnChangeTablesSetName();
 }

 m_lock_killfocus = true;
}

void CTablesDeskDlg::SetPosition(int x_pos, int y_pos, CWnd* wnd_insert_after /*=NULL*/)
{
 SetWindowPos(wnd_insert_after,x_pos,y_pos,0,0, (wnd_insert_after ? 0 : SWP_NOZORDER) | SWP_NOSIZE);
}

void CTablesDeskDlg::SetTitle(const CString& i_str)
{
 m_td_title.SetWindowText(i_str);
}

void CTablesDeskDlg::GetTitle(CString& o_str)
{
 m_td_title.GetWindowText(o_str);
}

bool CTablesDeskDlg::IsEnabled(void)
{
 return m_enabled;
}

//разрешение/запрещение
void CTablesDeskDlg::Enable(bool enable)
{
 if (m_enabled == enable)
  return; //already has needed state
 m_enabled = enable;

 if (::IsWindow(m_hWnd))
  UpdateDialogControls(this, TRUE);

 mp_ContextMenuManager->EnableMenuItems(enable);
}

//спрятать/показать все
void CTablesDeskDlg::Show(bool show)
{
 int nCmdShow = (show) ? SW_SHOW : SW_HIDE;
 this->ShowWindow(nCmdShow);
}

void CTablesDeskDlg::ShowOpenedCharts(bool i_show)
{
 mp_ButtonsPanel->ShowOpenedCharts(i_show);
}

void CTablesDeskDlg::UpdateOpenedCharts(void)
{
 mp_ButtonsPanel->UpdateOpenedCharts();
}

void CTablesDeskDlg::TransformValues(void)
{
 mp_ButtonsPanel->TransformValues();
}

void CTablesDeskDlg::SetReadOnlyTablesSetName(bool readonly)
{
 m_tsneb_readonly = readonly;
 m_names_edit.SetReadOnly(readonly);
}

void CTablesDeskDlg::SetModificationFlag(bool value)
{
 m_midification_flag.SetWindowText(value ? _T("*") : _T(" "));
}

void CTablesDeskDlg::MakeChartsChildren(bool children)
{
 mp_ButtonsPanel->MakeChartsChildren(children);
}

void CTablesDeskDlg::SetFunctionsNames(const std::vector<_TSTRING>& i_fwnames, const std::vector<_TSTRING>& i_eenames, int sep_index)
{
 std::vector<_TSTRING> loadnames = i_fwnames;
 std::vector<_TSTRING> savenames = i_eenames;
 //if string consists from all spaces or is empty, then use "<no name>"
 for(size_t index = 0; index < savenames.size(); ++index)
 {
  bool has_name = false;
  for(size_t i = 0; i < savenames[index].size(); ++i)
   if (savenames[index][i] != _T(' '))
    has_name = true;
  if (0==savenames[index].length() || false==has_name)
   savenames[index] = _T("<no name>");
 }
 std::copy(savenames.begin(), savenames.end(), std::back_inserter(loadnames));
 mp_ContextMenuManager->SetLoadPopupMenuNames(loadnames, sep_index);
 mp_ContextMenuManager->SetSavePopupMenuNames(savenames);
}

void CTablesDeskDlg::SetRPMGrid(const float* values)
{
 if (mp_ButtonsPanel.get())
  std::copy(values, values + 16, mp_ButtonsPanel->GetRPMGrid());
}

void CTablesDeskDlg::SetCLTGrid(const float* values)
{
 if (mp_ButtonsPanel.get())
  std::copy(values, values + 16, mp_ButtonsPanel->GetCLTGrid());
}

void CTablesDeskDlg::SetLoadGrid(const float* values)
{
 if (mp_ButtonsPanel.get())
  std::copy(values, values + 16, mp_ButtonsPanel->GetLoadGrid());
}

void CTablesDeskDlg::SetIRPMGrid(const float* values)
{
 if (mp_ButtonsPanel.get())
  std::copy(values, values + 8, mp_ButtonsPanel->GetIRPMGrid());
}

void CTablesDeskDlg::SetILoadGrid(const float* values)
{
 if (mp_ButtonsPanel.get())
  std::copy(values, values + 8, mp_ButtonsPanel->GetILoadGrid());
}

void CTablesDeskDlg::CloseAllCharts(void)
{
 HWND hwnd;
 for(int i = ETMT_SET_START; i <= ETMT_SET_END; ++i)
 {
  hwnd = mp_ButtonsPanel->GetMapWindow(i);
  if (hwnd) ::DestroyWindow(hwnd);
 }

 hwnd = mp_ButtonsPanel->GetMapWindow(ETMT_GME_IGN_WND);
 if (hwnd) ::DestroyWindow(hwnd);

 hwnd = mp_ButtonsPanel->GetMapWindow(ETMT_GME_INJ_WND);
 if (hwnd) ::DestroyWindow(hwnd);
}

//------------------------------------------------------------------------
void CTablesDeskDlg::setOnMapChanged(EventWithCode OnFunction)
{ m_OnMapChanged = OnFunction; }

void CTablesDeskDlg::setOnCloseMapWnd(EventWithHWND OnFunction)
{ m_OnCloseMapWnd = OnFunction; }

void CTablesDeskDlg::setOnOpenMapWnd(EventWithHWND OnFunction)
{ m_OnOpenMapWnd = OnFunction; }

void CTablesDeskDlg::setOnSaveButton(EventHandler OnFunction)
{ m_OnSaveButton  = OnFunction; }

void CTablesDeskDlg::setOnChangeTablesSetName(EventHandler OnFunction)
{ m_OnChangeTablesSetName  = OnFunction; }

void CTablesDeskDlg::setOnLoadTablesFrom(EventWithCode OnFunction)
{ m_OnLoadTablesFrom = OnFunction; }

void CTablesDeskDlg::setOnSaveTablesTo(EventWithCode OnFunction)
{ m_OnSaveTablesTo = OnFunction; }

void CTablesDeskDlg::setOnImportFromS3F(EventHandler OnFunction)
{ m_OnImportFromS3F = OnFunction; }

void CTablesDeskDlg::setOnExportToS3F(EventHandler OnFunction)
{ m_OnExportToS3F = OnFunction; }

//------------------------------------------------------------------------
void CTablesDeskDlg::SetTablesSetName(const _TSTRING& name)
{
 m_lock_enchange = true;
 m_names_text = name.c_str();
 UpdateData(FALSE);
 m_lock_enchange = false;
}

_TSTRING CTablesDeskDlg::GetTablesSetName(void) const
{
 CString str;
 m_names_edit.GetWindowText(str);
 return (LPCTSTR)str;
}

void CTablesDeskDlg::SetDynamicValues(const SECU3IO::SensorDat* sd)
{
 if (!mp_ButtonsPanel.get()) return;
 mp_ButtonsPanel->SetDynamicValues(*sd);
}

void CTablesDeskDlg::EnableAdvanceAngleIndication(bool i_enable)
{
 if (!mp_ButtonsPanel.get()) return;
 mp_ButtonsPanel->EnableAdvanceAngleIndication(i_enable);
}

//------------------------------------------------------------------------
void CTablesDeskDlg::OnDestroy()
{
 Super::OnDestroy();
}

//если надо апдейтить отдельные контроллы, то надо будет плодить функции
void CTablesDeskDlg::OnUpdateControls(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled);
}

void CTablesDeskDlg::OnSaveButton()
{
 if (m_OnSaveButton)
  m_OnSaveButton();

 OnRangeCmdsSave(IDM_TD_SAVE_NAMES_RESERVED0);

 //open popup menu beneath of button!
// CRect rc;
// m_save_button.GetWindowRect(rc);
// mp_ContextMenuManager->TrackPopupMenuSave(rc.left, rc.bottom);
}
/*
void CTablesDeskDlg::OnSysCommand(UINT nID, LONG lParam)
{
 Super::OnSysCommand(nID, lParam);
 if (nID == SC_MINIMIZE)
  SetWindowPos(&wndTop, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
}*/

void CTablesDeskDlg::OnContextMenu(CWnd* pWnd, CPoint point)
{
 mp_ContextMenuManager->TrackPopupMenu(point.x, point.y);
}

void CTablesDeskDlg::OnRangeCmdsLoad(UINT nID)
{
 if (m_OnLoadTablesFrom)
  m_OnLoadTablesFrom(nID - IDM_TD_LOAD_NAMES_RESERVED0);
}

void CTablesDeskDlg::OnRangeCmdsSave(UINT nID)
{
 if (m_OnSaveTablesTo)
  m_OnSaveTablesTo(nID - IDM_TD_SAVE_NAMES_RESERVED0);
}

void CTablesDeskDlg::OnImportFromS3F(void)
{
 if (m_OnImportFromS3F)
  m_OnImportFromS3F();
}

void CTablesDeskDlg::OnExportToS3F(void)
{
 if (m_OnExportToS3F)
  m_OnExportToS3F();
}

//------------------------------------------------------------------------
void CTablesDeskDlg::OnMapChanged(int i_mapType)
{
 if (m_OnMapChanged)
  m_OnMapChanged(i_mapType);
}

void CTablesDeskDlg::OnCloseMapWnd(HWND i_hwnd, int i_mapType)
{
 if (m_OnCloseMapWnd)
  m_OnCloseMapWnd(i_hwnd, i_mapType);
}

void CTablesDeskDlg::OnOpenMapWnd(HWND i_hwnd, int i_mapType)
{
 if (m_OnOpenMapWnd)
  m_OnOpenMapWnd(i_hwnd, i_mapType);
}

void CTablesDeskDlg::OnWndActivation(HWND i_hwnd, long cmd)
{
 //empty
}

//from CButtonsPanel
bool CTablesDeskDlg::IsAllowed(void)
{
 return m_enabled;
}

void CTablesDeskDlg::OnChangeTablesSetName()
{
 if (m_OnChangeTablesSetName && false==m_lock_enchange)
 {
  m_lock_killfocus = false;
  UpdateData(TRUE);
  m_OnChangeTablesSetName();
 }
}

void CTablesDeskDlg::OnSize( UINT nType, int cx, int cy )
{
 Super::OnSize(nType, cx, cy);
 
//resize tab control
 if (mp_ButtonsPanel.get() && ::IsWindow(mp_ButtonsPanel->m_hWnd))
 {
  CRect wndRectDlg, wndRectBP;
  GetClientRect(&wndRectDlg);
  mp_ButtonsPanel->GetWindowRect(&wndRectBP);
  ScreenToClient(&wndRectBP);
  wndRectBP.right = wndRectDlg.right - m_bpLRMargin.cx;
  wndRectBP.bottom = wndRectDlg.bottom - m_bpLRMargin.cy;
  mp_ButtonsPanel->MoveWindow(wndRectBP);
 }
}

void CTablesDeskDlg::EnableToggleMapWnd(bool toggle)
{
 mp_ButtonsPanel->EnableToggleMapWnd(toggle);
}

//note: this event is about shoving tab and not obligatory about showing this dialog
void CTablesDeskDlg::OnShow(bool show)
{
 mp_ButtonsPanel->OnShow(show);
 ShowOpenedCharts(IsWindowVisible() && show);
}
