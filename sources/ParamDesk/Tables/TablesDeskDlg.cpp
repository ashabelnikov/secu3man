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
#include "TablesDeskDlg.h"

#include <map>
#include "common/Dll.h"
#include "common/FastDelegate.h"
#include "TablDesk/ButtonsPanel.h"
#include "TablDesk/MapIds.h"
#include "TDContextMenuManager.h"

using namespace std;
using namespace fastdelegate;

/////////////////////////////////////////////////////////////////////////////
// CTablesDeskDlg dialog

UINT CTablesDeskDlg::IDD = IDD_TABLES_DESK;
UINT CTablesDeskDlg::IDD_F = IDD_TABLES_DESK_FLOATING;

CTablesDeskDlg::CTablesDeskDlg(CWnd* pParent /*=NULL*/)
: Super(CTablesDeskDlg::IDD, pParent)
, m_enabled(false)
, mp_ContextMenuManager(new CTDContextMenuManager())
, m_children_charts(false)
, m_tsneb_readonly(false)
, m_lock_enchange(false)
, m_lock_killfocus(true)
, mp_ButtonsPanel(new CButtonsPanel(0, NULL))
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
}

BEGIN_MESSAGE_MAP(CTablesDeskDlg, Super)
 ON_WM_DESTROY()
 ON_WM_SYSCOMMAND()
 ON_WM_CONTEXTMENU()
 ON_WM_INITMENUPOPUP()
 ON_UPDATE_COMMAND_UI(IDC_TABLES_DESK_TITLE, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_TD_SAVE_BUTTON, OnUpdateControls)
 ON_UPDATE_COMMAND_UI_RANGE(IDM_TD_LOAD_NAMES_RESERVED0, IDM_TD_LOAD_NAMES_RESERVED15, OnUpdateControls)
 ON_UPDATE_COMMAND_UI_RANGE(IDM_TD_SAVE_NAMES_RESERVED0, IDM_TD_SAVE_NAMES_RESERVED5, OnUpdateControls)
 ON_BN_CLICKED(IDC_TD_SAVE_BUTTON, OnSaveButton)
 ON_COMMAND_RANGE(IDM_TD_LOAD_NAMES_RESERVED0, IDM_TD_LOAD_NAMES_RESERVED15, OnRangeCmdsLoad)
 ON_COMMAND_RANGE(IDM_TD_SAVE_NAMES_RESERVED0, IDM_TD_SAVE_NAMES_RESERVED5, OnRangeCmdsSave)

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

 mp_ButtonsPanel->Create(mp_ButtonsPanel->IDD, this);
 mp_ButtonsPanel->SetPosition(rect.left, rect.top, &m_names_edit);
 mp_ButtonsPanel->ShowWindow(SW_SHOW);

 m_names_edit.SetReadOnly(m_tsneb_readonly);
 m_names_edit.SetLimitText(16);

 mp_ContextMenuManager->Attach(this);

 UpdateData(FALSE);
 UpdateDialogControls(this, TRUE);
 return TRUE;  // return TRUE unless you set the focus to a control
}

void CTablesDeskDlg::OnEditKillFocus()
{
 if (m_lock_killfocus)
  return; //we need to prevent wrong behaviour when window is recreated
 if (0==m_names_edit.GetWindowTextLength())
 {
  m_names_edit.SetWindowText(_T("<no name>"));
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

void CTablesDeskDlg::ShowSaveButton(bool i_show)
{
 m_save_button.ShowWindow(i_show ? SW_SHOW : SW_HIDE);
}

void CTablesDeskDlg::ShowOpenedCharts(bool i_show)
{
 HWND hwnd;
 hwnd = mp_ButtonsPanel->GetMapWindow(TYPE_MAP_DA_START);
 if (hwnd)
  ::ShowWindow(hwnd, i_show ? SW_SHOW : SW_HIDE);
 hwnd = mp_ButtonsPanel->GetMapWindow(TYPE_MAP_DA_IDLE);
 if (hwnd)
  ::ShowWindow(hwnd, i_show ? SW_SHOW : SW_HIDE);
 hwnd = mp_ButtonsPanel->GetMapWindow(TYPE_MAP_DA_WORK);
 if (hwnd)
  ::ShowWindow(hwnd, i_show ? SW_SHOW : SW_HIDE);
 hwnd = mp_ButtonsPanel->GetMapWindow(TYPE_MAP_DA_TEMP_CORR);
 if (hwnd)
  ::ShowWindow(hwnd, i_show ? SW_SHOW : SW_HIDE);

 hwnd = mp_ButtonsPanel->GetMapWindow(TYPE_MAP_INJ_VE);
 if (hwnd)
  ::ShowWindow(hwnd, i_show ? SW_SHOW : SW_HIDE);
 hwnd = mp_ButtonsPanel->GetMapWindow(TYPE_MAP_INJ_AFR);
 if (hwnd)
  ::ShowWindow(hwnd, i_show ? SW_SHOW : SW_HIDE);
 hwnd = mp_ButtonsPanel->GetMapWindow(TYPE_MAP_INJ_CRNK);
 if (hwnd)
  ::ShowWindow(hwnd, i_show ? SW_SHOW : SW_HIDE);
 hwnd = mp_ButtonsPanel->GetMapWindow(TYPE_MAP_INJ_WRMP);
 if (hwnd)
  ::ShowWindow(hwnd, i_show ? SW_SHOW : SW_HIDE);
 hwnd = mp_ButtonsPanel->GetMapWindow(TYPE_MAP_INJ_DEAD);
 if (hwnd)
  ::ShowWindow(hwnd, i_show ? SW_SHOW : SW_HIDE);
 hwnd = mp_ButtonsPanel->GetMapWindow(TYPE_MAP_INJ_IDLR);
 if (hwnd)
  ::ShowWindow(hwnd, i_show ? SW_SHOW : SW_HIDE);
 hwnd = mp_ButtonsPanel->GetMapWindow(TYPE_MAP_INJ_IDLC);
 if (hwnd)
  ::ShowWindow(hwnd, i_show ? SW_SHOW : SW_HIDE);
 hwnd = mp_ButtonsPanel->GetMapWindow(TYPE_MAP_INJ_AETPS);
 if (hwnd)
  ::ShowWindow(hwnd, i_show ? SW_SHOW : SW_HIDE);
 hwnd = mp_ButtonsPanel->GetMapWindow(TYPE_MAP_INJ_AERPM);
 if (hwnd)
  ::ShowWindow(hwnd, i_show ? SW_SHOW : SW_HIDE);

 hwnd = mp_ButtonsPanel->GetMapWindow(TYPE_MAP_GME_WND);
 if (hwnd)
  ::ShowWindow(hwnd, i_show ? SW_SHOW : SW_HIDE);
}

void CTablesDeskDlg::UpdateOpenedCharts(void)
{
 mp_ButtonsPanel->UpdateOpenedCharts();
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
 m_children_charts = children;
 HWND hwnd;
 hwnd = mp_ButtonsPanel->GetMapWindow(TYPE_MAP_DA_START);
 _MakeWindowChild(hwnd, children);
 hwnd = mp_ButtonsPanel->GetMapWindow(TYPE_MAP_DA_IDLE);
 _MakeWindowChild(hwnd, children);
 hwnd = mp_ButtonsPanel->GetMapWindow(TYPE_MAP_DA_WORK);
 _MakeWindowChild(hwnd, children);
 hwnd = mp_ButtonsPanel->GetMapWindow(TYPE_MAP_DA_TEMP_CORR);
 _MakeWindowChild(hwnd, children);
 hwnd = mp_ButtonsPanel->GetMapWindow(TYPE_MAP_INJ_VE);
 _MakeWindowChild(hwnd, children);
 hwnd = mp_ButtonsPanel->GetMapWindow(TYPE_MAP_INJ_AFR);
 _MakeWindowChild(hwnd, children);
 hwnd = mp_ButtonsPanel->GetMapWindow(TYPE_MAP_INJ_CRNK);
 _MakeWindowChild(hwnd, children);
 hwnd = mp_ButtonsPanel->GetMapWindow(TYPE_MAP_INJ_WRMP);
 _MakeWindowChild(hwnd, children);
 hwnd = mp_ButtonsPanel->GetMapWindow(TYPE_MAP_INJ_DEAD);
 _MakeWindowChild(hwnd, children);
 hwnd = mp_ButtonsPanel->GetMapWindow(TYPE_MAP_INJ_IDLR);
 _MakeWindowChild(hwnd, children);
 hwnd = mp_ButtonsPanel->GetMapWindow(TYPE_MAP_INJ_IDLC);
 _MakeWindowChild(hwnd, children);
 hwnd = mp_ButtonsPanel->GetMapWindow(TYPE_MAP_INJ_AETPS);
 _MakeWindowChild(hwnd, children);
 hwnd = mp_ButtonsPanel->GetMapWindow(TYPE_MAP_INJ_AERPM);
 _MakeWindowChild(hwnd, children);

 hwnd = mp_ButtonsPanel->GetMapWindow(TYPE_MAP_GME_WND); //pseudo map
 _MakeWindowChild(hwnd, children);
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

//------------------------------------------------------------------------
void CTablesDeskDlg::setOnMapChanged(EventWithCode OnFunction)
{ m_OnMapChanged = OnFunction; }

void CTablesDeskDlg::setOnCloseMapWnd(EventWithHWND OnFunction)
{ m_OnCloseMapWnd = OnFunction; }

void CTablesDeskDlg::setOnOpenMapWnd(EventWithHWND OnFunction)
{ m_OnOpenMapWnd = OnFunction; }

void CTablesDeskDlg::setOnTabActivate(EventHandler OnFunction)
{ m_OnTabActivate = OnFunction; }

void CTablesDeskDlg::setOnSaveButton(EventHandler OnFunction)
{ m_OnSaveButton  = OnFunction; }

void CTablesDeskDlg::setOnChangeTablesSetName(EventHandler OnFunction)
{ m_OnChangeTablesSetName  = OnFunction; }

void CTablesDeskDlg::setOnLoadTablesFrom(EventWithCode OnFunction)
{ m_OnLoadTablesFrom = OnFunction; }

void CTablesDeskDlg::setOnSaveTablesTo(EventWithCode OnFunction)
{ m_OnSaveTablesTo = OnFunction; }

//------------------------------------------------------------------------
void CTablesDeskDlg::SetTablesSetName(const _TSTRING& name)
{
 m_lock_enchange = true;
 m_names_edit.SetWindowText(name.c_str());
 m_lock_enchange = false;
}

_TSTRING CTablesDeskDlg::GetTablesSetName(void) const
{
 CString str;
 m_names_edit.GetWindowText(str);
 return str.GetBuffer(128);
}

float* CTablesDeskDlg::GetMap(int i_mapType, bool i_original)
{
 switch(i_mapType)
 {
  case TYPE_MAP_DA_START:
   return mp_ButtonsPanel->GetStartMap(i_original);

  case TYPE_MAP_DA_IDLE:
   return mp_ButtonsPanel->GetIdleMap(i_original);

  case TYPE_MAP_DA_WORK:
   return mp_ButtonsPanel->GetWorkMap(i_original);

  case TYPE_MAP_DA_TEMP_CORR:
   return mp_ButtonsPanel->GetTempMap(i_original);

  case TYPE_MAP_INJ_VE:
   return mp_ButtonsPanel->GetVEMap(i_original);

  case TYPE_MAP_INJ_AFR:
   return mp_ButtonsPanel->GetAFRMap(i_original);

  case TYPE_MAP_INJ_CRNK:
   return mp_ButtonsPanel->GetCrnkMap(i_original);

  case TYPE_MAP_INJ_WRMP:
   return mp_ButtonsPanel->GetWrmpMap(i_original);

  case TYPE_MAP_INJ_DEAD:
   return mp_ButtonsPanel->GetDeadMap(i_original);

  case TYPE_MAP_INJ_IDLR:
   return mp_ButtonsPanel->GetIdlrMap(i_original);

  case TYPE_MAP_INJ_IDLC:
   return mp_ButtonsPanel->GetIdlcMap(i_original);

  case TYPE_MAP_INJ_AETPS:
   return mp_ButtonsPanel->GetAETPSMap(i_original);

  case TYPE_MAP_INJ_AERPM:
   return mp_ButtonsPanel->GetAERPMMap(i_original);

  default:
  return NULL;
 }
}

void CTablesDeskDlg::SetDynamicValues(int rpm, float temp, int air_flow, float adv_ang, float knock_retard, bool knkret_use,
 float strt_aalt, bool strt_use, float idle_aalt, bool idle_use, float work_aalt, bool work_use, float temp_aalt, bool temp_use,
 float airt_aalt, bool airt_use, float idlreg_aac, bool idlreg_use, float octan_aac, bool octan_use)
{
 if (!mp_ButtonsPanel.get()) return;
 CGridModeEditorDlg::DynVal dv;
 dv.rpm = rpm;
 dv.temp = temp;
 dv.air_flow = air_flow;
 dv.adv_ang = adv_ang;
 dv.knock_retard = knock_retard;
 dv.knkret_use = knkret_use;
 dv.strt_aalt = strt_aalt; 
 dv.strt_use = strt_use;
 dv.idle_aalt = idle_aalt;
 dv.idle_use = idle_use;
 dv.work_aalt = work_aalt;
 dv.work_use = work_use;
 dv.temp_aalt = temp_aalt;
 dv.temp_use = temp_use;
 dv.airt_aalt = airt_aalt;
 dv.airt_use = airt_use;
 dv.idlreg_aac = idlreg_aac;
 dv.idlreg_use = idlreg_use;
 dv.octan_aac = octan_aac;
 dv.octan_use = octan_use;
 mp_ButtonsPanel->SetDynamicValues(dv);
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
 ShowOpenedCharts(false);
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

 //open popup menu beneath of button!
 CRect rc;
 m_save_button.GetWindowRect(rc);
 mp_ContextMenuManager->TrackPopupMenuSave(rc.left, rc.bottom);
}

void CTablesDeskDlg::OnSysCommand(UINT nID, LONG lParam)
{
 Super::OnSysCommand(nID, lParam);
 if (nID == SC_MINIMIZE)
  SetWindowPos(&wndTop, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
}

void CTablesDeskDlg::OnContextMenu(CWnd* pWnd, CPoint point)
{
 mp_ContextMenuManager->TrackPopupMenu(point.x, point.y);
}

void CTablesDeskDlg::OnInitMenuPopup(CMenu* pMenu, UINT nIndex, BOOL bSysMenu)
{
 mp_ContextMenuManager->OnInitMenuPopup(pMenu, nIndex, bSysMenu);
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
 _MakeWindowChild(i_hwnd, m_children_charts);
 
 if (m_OnOpenMapWnd)
  m_OnOpenMapWnd(i_hwnd, i_mapType);
}

void CTablesDeskDlg::OnWndActivation(HWND i_hwnd, long cmd)
{
 if (m_children_charts && cmd == SC_MINIMIZE && NULL != i_hwnd)
  CWnd::FromHandle(i_hwnd)->SetWindowPos(&wndTop, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
}

//------------------------------------------------------------------------
void CTablesDeskDlg::_MakeWindowChild(HWND hwnd, bool child)
{
 if (hwnd)
 {
  if (child)
   CWnd::FromHandle(hwnd)->ModifyStyle(0, WS_CHILD | WS_CLIPSIBLINGS);
  else
   CWnd::FromHandle(hwnd)->ModifyStyle(WS_CHILD | WS_CLIPSIBLINGS, 0);

  CWnd::FromHandle(hwnd)->SetParent(child ? this->GetParent() : NULL);
 }
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
  m_OnChangeTablesSetName();
 }
}
