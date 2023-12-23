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

/** \file SepTablesDeskDlg.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "Resources/resource.h"
#include "SepTablesDeskDlg.h"

#include <map>
#include "common/Dll.h"
#include "common/FastDelegate.h"
#include "TablDesk/SeptabsPanel.h"
#include "io-core/MapIds.h"
#include "ui-core/ToolTipCtrlEx.h"
#include "ui-core/WndScroller.h"

using namespace fastdelegate;

/////////////////////////////////////////////////////////////////////////////
// CSepTablesDeskDlg dialog

UINT CSepTablesDeskDlg::IDD = IDD_SEPTABLES_DESK;

CSepTablesDeskDlg::CSepTablesDeskDlg(CWnd* pParent /*=NULL*/)
: Super(CSepTablesDeskDlg::IDD, pParent)
, m_enabled(false)
, m_enable_save_button(false)
, mp_SeptabsPanel(new CSeptabsPanel())
{
 //их надо создать только один раз
 mp_SeptabsPanel->setOnMapChanged(MakeDelegate(this, &CSepTablesDeskDlg::OnMapChanged));
 mp_SeptabsPanel->setOnCloseMapWnd(MakeDelegate(this, &CSepTablesDeskDlg::OnCloseMapWnd));
 mp_SeptabsPanel->setOnOpenMapWnd(MakeDelegate(this, &CSepTablesDeskDlg::OnOpenMapWnd));
 mp_SeptabsPanel->setOnWndActivation(MakeDelegate(this, &CSepTablesDeskDlg::OnWndActivation));
 mp_SeptabsPanel->setIsAllowed(MakeDelegate(this, &CSepTablesDeskDlg::IsAllowed));
}

CSepTablesDeskDlg::~CSepTablesDeskDlg()
{
  //empty
}

void CSepTablesDeskDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);
 DDX_Control(pDX, IDC_TD_SAVE_BUTTON, m_save_button);
 DDX_Control(pDX, IDC_TD_MODIFICATION_FLAG, m_midification_flag);
}

BEGIN_MESSAGE_MAP(CSepTablesDeskDlg, Super)
 ON_WM_DESTROY()
/* ON_WM_SYSCOMMAND()*/
 ON_WM_SIZE()
 ON_UPDATE_COMMAND_UI(IDC_TABLES_DESK_TITLE, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_TD_SAVE_BUTTON, OnUpdateControlsSaveButton)
 ON_BN_CLICKED(IDC_TD_SAVE_BUTTON, OnSaveButton)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSepTablesDeskDlg message handlers

BOOL CSepTablesDeskDlg::OnInitDialog()
{
 Super::OnInitDialog();

 CRect rect;
 GetDlgItem(IDC_TD_SEPTABLES_PANEL_HOLDER)->GetWindowRect(rect);
 ScreenToClient(rect);

 mp_SeptabsPanel->Create(this);
 mp_SeptabsPanel->SetPosition(rect.left, rect.top, &m_save_button);
 mp_SeptabsPanel->ShowWindow(SW_SHOW);
 mp_SeptabsPanel->MoveWindow(rect); //Set window size to size of holder

 UpdateData(FALSE);
 UpdateDialogControls(this, TRUE);

 ///////////////////////////////////////////////
 //TODO: Replace by MIHelpers (see MIHelpers.h) or even more all releted functions to common
 //Calculate and remember right and bottom margins of buttons panel control
 CRect wndRectDlg, wndRectBP;
 GetClientRect(&wndRectDlg);
 mp_SeptabsPanel->GetWindowRect(&wndRectBP);
 ScreenToClient(&wndRectBP);
 m_bpLRMargin.cx = wndRectDlg.right - wndRectBP.right;
 m_bpLRMargin.cy = wndRectDlg.bottom - wndRectBP.bottom;
 ///////////////////////////////////////////////

 //Create tooltip control and set tooltips for widgets
 mp_ttc.reset(new CToolTipCtrlEx());
 VERIFY(mp_ttc->Create(this, WS_POPUP | TTS_ALWAYSTIP | TTS_BALLOON));
 VERIFY(mp_ttc->AddWindow(&m_save_button, MLL::GetString(IDS_SEP_SAVE_BUTTON_TT)));
 mp_ttc->SetMaxTipWidth(250); //Set width for text wrapping
 mp_ttc->ActivateToolTips(true);

 return TRUE;  // return TRUE unless you set the focus to a control
}

void CSepTablesDeskDlg::SetPosition(int x_pos, int y_pos, CWnd* wnd_insert_after /*=NULL*/)
{
 SetWindowPos(wnd_insert_after,x_pos,y_pos,0,0, (wnd_insert_after ? 0 : SWP_NOZORDER) | SWP_NOSIZE);
}

bool CSepTablesDeskDlg::IsEnabled(void)
{
 return m_enabled;
}

//enable/disable
void CSepTablesDeskDlg::Enable(bool enable)
{
 if (m_enabled == enable)
  return; //already has needed state
 m_enabled = enable;

 if (::IsWindow(m_hWnd))
  UpdateDialogControls(this, TRUE);
} 

//hide/show all
void CSepTablesDeskDlg::Show(bool show)
{
 int nCmdShow = (show) ? SW_SHOW : SW_HIDE;
 this->ShowWindow(nCmdShow);
}

void CSepTablesDeskDlg::ShowOpenedCharts(bool i_show)
{
 mp_SeptabsPanel->ShowOpenedCharts(i_show);
}

void CSepTablesDeskDlg::UpdateOpenedCharts(void)
{
 mp_SeptabsPanel->UpdateOpenedCharts();
}

void CSepTablesDeskDlg::SetModificationFlag(bool value)
{
 m_midification_flag.SetWindowText(value ? _T("*") : _T(" "));
}

void CSepTablesDeskDlg::MakeChartsChildren(bool children)
{
 mp_SeptabsPanel->MakeChartsChildren(children);
}

void CSepTablesDeskDlg::SetRPMGrid(const float* values)
{
 if (mp_SeptabsPanel.get())
  std::copy(values, values + 16, mp_SeptabsPanel->GetRPMGrid());
}

void CSepTablesDeskDlg::SetCLTGrid(const float* values)
{
 if (mp_SeptabsPanel.get())
  std::copy(values, values + 16, mp_SeptabsPanel->GetCLTGrid());
}

void CSepTablesDeskDlg::SetLoadGrid(const float* values)
{
 if (mp_SeptabsPanel.get())
  std::copy(values, values + 16, mp_SeptabsPanel->GetLoadGrid());
}

void CSepTablesDeskDlg::CloseAllCharts(void)
{
 HWND hwnd;
 for(int i = ETMT_SEP_START; i <= ETMT_SEP_END; ++i)
 {
  hwnd = mp_SeptabsPanel->GetMapWindow(i);
  if (hwnd) ::DestroyWindow(hwnd);
 }
}

//------------------------------------------------------------------------
void CSepTablesDeskDlg::setOnMapChanged(EventWithCode OnFunction)
{ m_OnMapChanged = OnFunction; }

void CSepTablesDeskDlg::setOnCloseMapWnd(EventWithHWND OnFunction)
{ m_OnCloseMapWnd = OnFunction; }

void CSepTablesDeskDlg::setOnOpenMapWnd(EventWithHWND OnFunction)
{ m_OnOpenMapWnd = OnFunction; }

void CSepTablesDeskDlg::setOnSaveButton(EventHandler OnFunction)
{ m_OnSaveButton  = OnFunction; }

//------------------------------------------------------------------------
void CSepTablesDeskDlg::OnDestroy()
{
 Super::OnDestroy();
 ShowOpenedCharts(false);
}

void CSepTablesDeskDlg::OnUpdateControls(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled);
}

void CSepTablesDeskDlg::OnUpdateControlsSaveButton(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled && m_enable_save_button);
}

void CSepTablesDeskDlg::OnSaveButton()
{
 if (m_OnSaveButton)
  m_OnSaveButton();
}

/*
void CSepTablesDeskDlg::OnSysCommand(UINT nID, LONG lParam)
{
 Super::OnSysCommand(nID, lParam);
 if (nID == SC_MINIMIZE)
  SetWindowPos(&wndTop, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
}*/

//------------------------------------------------------------------------
void CSepTablesDeskDlg::OnMapChanged(int i_mapType)
{
 if (m_OnMapChanged)
  m_OnMapChanged(i_mapType);
}

void CSepTablesDeskDlg::OnCloseMapWnd(HWND i_hwnd, int i_mapType)
{
 if (m_OnCloseMapWnd)
  m_OnCloseMapWnd(i_hwnd, i_mapType);
}

void CSepTablesDeskDlg::OnOpenMapWnd(HWND i_hwnd, int i_mapType)
{
 if (m_OnOpenMapWnd)
  m_OnOpenMapWnd(i_hwnd, i_mapType);
}

void CSepTablesDeskDlg::OnWndActivation(HWND i_hwnd, long cmd)
{
 //empty
}

//from CSeptabsPanel
bool CSepTablesDeskDlg::IsAllowed(void)
{
 return m_enabled;
}

void CSepTablesDeskDlg::OnSize( UINT nType, int cx, int cy )
{
 Super::OnSize(nType, cx, cy);
 
 if (mp_SeptabsPanel.get() && ::IsWindow(mp_SeptabsPanel->m_hWnd))
 {
  CRect wndRectDlg, wndRectBP;
  GetClientRect(&wndRectDlg);
  mp_SeptabsPanel->GetWindowRect(&wndRectBP);
  ScreenToClient(&wndRectBP);
  wndRectBP.right = wndRectDlg.right - m_bpLRMargin.cx;
  wndRectBP.bottom = wndRectDlg.bottom - m_bpLRMargin.cy;
  mp_SeptabsPanel->MoveWindow(wndRectBP);
 }
}

void CSepTablesDeskDlg::EnableToggleMapWnd(bool toggle)
{
 mp_SeptabsPanel->EnableToggleMapWnd(toggle);
}

void CSepTablesDeskDlg::OnShow(bool show)
{
 mp_SeptabsPanel->OnShow(show);
 ShowOpenedCharts(IsWindowVisible() && show);
}

void CSepTablesDeskDlg::EnableSaveButton(bool enable)
{
 m_enable_save_button = enable;
 UpdateDialogControls(this, TRUE);
}

void CSepTablesDeskDlg::SetDynamicValues(const SECU3IO::SensorDat* sd)
{
 if (!mp_SeptabsPanel.get()) return;
 mp_SeptabsPanel->SetDynamicValues(*sd);
}
