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
#include "TablesDeskDlg.h"

#include <map>
#include "common/Dll.h"
#include "common/FastDelegate.h"
#include "TablDesk/ButtonsPanel.h"
#include "TablDesk/MapIds.h"
#include "TablesPageDlg.h"
#include "TDContextMenuManager.h"
#include "ui-core/HotKeysToCmdRouter.h"

#define TABLES_TAB_CTRL_BITMAPS_COLOR_MASK RGB(192,192,192)

using namespace std;
using namespace fastdelegate;

/////////////////////////////////////////////////////////////////////////////
// CTablesDeskDlg dialog

UINT CTablesDeskDlg::IDD = IDD_TABLES_DESK;
UINT CTablesDeskDlg::IDD_F = IDD_TABLES_DESK_FLOATING;

CTablesDeskDlg::CTablesDeskDlg(CWnd* pParent /*=NULL*/)
: Super(CTablesDeskDlg::IDD, pParent)
, m_pImgList(NULL)
, m_enabled(false)
, m_hot_keys_supplier(new CHotKeysToCmdRouter())
, mp_ContextMenuManager(new CTDContextMenuManager())
, m_children_charts(false)
{
 //создаем image list для TabCtrl
 m_pImgList = new CImageList();
 m_pImgList->Create(16, 16, ILC_COLOR24|ILC_MASK, 0, 0);
 CBitmap bitmap;
 bitmap.LoadBitmap((LPCTSTR)IDB_TABLES_TAB_CTRL_BITMAPS);
 m_pImgList->Add(&bitmap, TABLES_TAB_CTRL_BITMAPS_COLOR_MASK);

 //их надо создать только один раз
 m_pPageDlg = new CTablesPageDlg();
 m_pPageDlg->mp_ButtonsPanel->setOnMapChanged(MakeDelegate(this, &CTablesDeskDlg::OnMapChanged));
 m_pPageDlg->mp_ButtonsPanel->setOnCloseMapWnd(MakeDelegate(this, &CTablesDeskDlg::OnCloseMapWnd));
 m_pPageDlg->mp_ButtonsPanel->setOnOpenMapWnd(MakeDelegate(this, &CTablesDeskDlg::OnOpenMapWnd));
 m_pPageDlg->setOnChangeTablesSetName(MakeDelegate(this, &CTablesDeskDlg::OnChangeTablesSetName));
 m_pPageDlg->mp_ButtonsPanel->setOnWndActivation(MakeDelegate(this, &CTablesDeskDlg::OnWndActivation));

 mp_ContextMenuManager->CreateContent();
}

CTablesDeskDlg::~CTablesDeskDlg()
{
 delete m_pImgList;
 delete m_pPageDlg;
}

void CTablesDeskDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);
 DDX_Control(pDX, IDC_TABLES_DESK_TITLE, m_td_title);
 DDX_Control(pDX, IDC_TD_TAB_CTRL, m_tab_control);
 DDX_Control(pDX, IDC_TD_SAVE_BUTTON, m_save_button);
}

BEGIN_MESSAGE_MAP(CTablesDeskDlg, Super)
 ON_WM_DESTROY()
 ON_WM_SYSCOMMAND()
 ON_WM_CONTEXTMENU()
 ON_WM_INITMENUPOPUP()
 ON_UPDATE_COMMAND_UI(IDC_TABLES_DESK_TITLE, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_TD_TAB_CTRL, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_TD_SAVE_BUTTON, OnUpdateControls)
 ON_UPDATE_COMMAND_UI_RANGE(IDM_TD_LOAD_NAMES_RESERVED0, IDM_TD_LOAD_NAMES_RESERVED15, OnUpdateControls)
 ON_UPDATE_COMMAND_UI_RANGE(IDM_TD_SAVE_NAMES_RESERVED0, IDM_TD_SAVE_NAMES_RESERVED5, OnUpdateControls)
 ON_BN_CLICKED(IDC_TD_SAVE_BUTTON, OnSaveButton)
 ON_COMMAND_RANGE(IDM_TD_LOAD_NAMES_RESERVED0, IDM_TD_LOAD_NAMES_RESERVED15, OnRangeCmdsLoad)
 ON_COMMAND_RANGE(IDM_TD_SAVE_NAMES_RESERVED0, IDM_TD_SAVE_NAMES_RESERVED5, OnRangeCmdsSave)

#define ON_COMMAND_HK_XXX(x)\
 ON_COMMAND(ID_TD_ACTIVATE_##x, OnHK_##x)

 ON_COMMAND_HK_XXX(GASOLINE_TAB)
 ON_COMMAND_HK_XXX(GAS_TAB)

END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CTablesDeskDlg message handlers

BOOL CTablesDeskDlg::OnInitDialog()
{
 Super::OnInitDialog();

 //контрол создан через ресурсы и стили описаны в ресурсах.
 m_tab_control.SetImageList(m_pImgList);
 m_tab_control.SetResourceModule(DLL::GetModuleHandle());
 m_tab_control.Init();

 //наполняем Tab control вкладками
 m_tab_control.AddPage(MLL::LoadString(IDS_TD_TABNAME_GASOLINE), m_pPageDlg, GASOLINE_TAB);
 m_tab_control.AddPage(MLL::LoadString(IDS_TD_TABNAME_GAS), m_pPageDlg, GAS_TAB);

 //ВНИМАНИЕ! SetEventListener должен быть вызван раньше чем SetCurSel, т.к. SetCurSel
 //уже использует обработчики сообщений!

 //this будет получать события от Tab control-а и делегировать их указанным обработчикам
 m_tab_control.SetEventListener(this);

 //устанавливаем предыдущее значение (разрешены вкладки или нет)
 m_tab_control.EnableItem(-1, m_enabled);

 m_hot_keys_supplier->Init(this);
 _RegisterHotKeys();

 mp_ContextMenuManager->Attach(this);

 UpdateDialogControls(this, TRUE);
 return TRUE;  // return TRUE unless you set the focus to a control
	           // EXCEPTION: OCX Property Pages should return FALSE
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
 m_pPageDlg->Enable(enable);

 if (::IsWindow(m_hWnd))
  UpdateDialogControls(this, TRUE);

 m_tab_control.EnableItem(-1, enable); //all items
 mp_ContextMenuManager->EnableMenuItems(enable);
}

//спрятать/показать все
void CTablesDeskDlg::Show(bool show)
{
 int nCmdShow = (show) ? SW_SHOW : SW_HIDE;
 m_tab_control.ShowWindow(nCmdShow);
 this->ShowWindow(nCmdShow);
}

void CTablesDeskDlg::ShowSaveButton(bool i_show)
{
 m_save_button.ShowWindow(i_show ? SW_SHOW : SW_HIDE);
}

void CTablesDeskDlg::ShowOpenedCharts(bool i_show)
{
 HWND hwnd;
 hwnd = m_pPageDlg->mp_ButtonsPanel->GetMapWindow(TYPE_MAP_DA_START);
 if (hwnd)
  ::ShowWindow(hwnd, i_show ? SW_SHOW : SW_HIDE);
 hwnd = m_pPageDlg->mp_ButtonsPanel->GetMapWindow(TYPE_MAP_DA_IDLE);
 if (hwnd)
  ::ShowWindow(hwnd, i_show ? SW_SHOW : SW_HIDE);
 hwnd = m_pPageDlg->mp_ButtonsPanel->GetMapWindow(TYPE_MAP_DA_WORK);
 if (hwnd)
  ::ShowWindow(hwnd, i_show ? SW_SHOW : SW_HIDE);
 hwnd = m_pPageDlg->mp_ButtonsPanel->GetMapWindow(TYPE_MAP_DA_TEMP_CORR);
 if (hwnd)
  ::ShowWindow(hwnd, i_show ? SW_SHOW : SW_HIDE);
 hwnd = m_pPageDlg->mp_ButtonsPanel->GetMapWindow(TYPE_MAP_GME_WND);
 if (hwnd)
  ::ShowWindow(hwnd, i_show ? SW_SHOW : SW_HIDE);
}

void CTablesDeskDlg::UpdateOpenedCharts(void)
{
 m_pPageDlg->mp_ButtonsPanel->UpdateOpenedCharts();
}

void CTablesDeskDlg::SetReadOnlyTablesSetName(bool readonly)
{
 m_pPageDlg->SetReadOnlyTablesSetName(readonly);
}

void CTablesDeskDlg::SetModificationFlag(bool value)
{
 m_pPageDlg->SetModificationFlag(value);
}

void CTablesDeskDlg::MakeChartsChildren(bool children)
{
 m_children_charts = children;
 HWND hwnd;
 hwnd = m_pPageDlg->mp_ButtonsPanel->GetMapWindow(TYPE_MAP_DA_START);
 _MakeWindowChild(hwnd, children);
 hwnd = m_pPageDlg->mp_ButtonsPanel->GetMapWindow(TYPE_MAP_DA_IDLE);
 _MakeWindowChild(hwnd, children);
 hwnd = m_pPageDlg->mp_ButtonsPanel->GetMapWindow(TYPE_MAP_DA_WORK);
 _MakeWindowChild(hwnd, children);
 hwnd = m_pPageDlg->mp_ButtonsPanel->GetMapWindow(TYPE_MAP_DA_TEMP_CORR);
 _MakeWindowChild(hwnd, children);
 hwnd = m_pPageDlg->mp_ButtonsPanel->GetMapWindow(TYPE_MAP_GME_WND); //pseudo map
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
 if (m_pPageDlg)
  std::copy(values, values + 16, m_pPageDlg->mp_ButtonsPanel->GetRPMGrid());
}

//------------------------------------------------------------------------
void CTablesDeskDlg::setOnMapChanged(EventWith2Codes OnFunction)
{ m_OnMapChanged = OnFunction; }

void CTablesDeskDlg::setOnCloseMapWnd(EventWithHWND OnFunction)
{ m_OnCloseMapWnd = OnFunction; }

void CTablesDeskDlg::setOnOpenMapWnd(EventWithHWND OnFunction)
{ m_OnOpenMapWnd = OnFunction; }

void CTablesDeskDlg::setOnTabActivate(EventHandler OnFunction)
{ m_OnTabActivate = OnFunction; }

void CTablesDeskDlg::setOnSaveButton(EventHandler OnFunction)
{ m_OnSaveButton  = OnFunction; }

void CTablesDeskDlg::setOnChangeTablesSetName(EventWithCode OnFunction)
{ m_OnChangeTablesSetName  = OnFunction; }

void CTablesDeskDlg::setOnLoadTablesFrom(EventWithCode OnFunction)
{ m_OnLoadTablesFrom = OnFunction; }

void CTablesDeskDlg::setOnSaveTablesTo(EventWithCode OnFunction)
{ m_OnSaveTablesTo = OnFunction; }

//------------------------------------------------------------------------
bool CTablesDeskDlg::SetCurSel(int sel)
{
 return m_tab_control.SetCurSel(sel);
}

int CTablesDeskDlg::GetCurSel(void)
{
 return m_tab_control.GetCurSel();
}

void CTablesDeskDlg::SetTablesSetName(const _TSTRING& name)
{
 m_pPageDlg->SetTablesSetName(name);
}

_TSTRING CTablesDeskDlg::GetTablesSetName(void) const
{
 return m_pPageDlg->GetTablesSetName();
}

float* CTablesDeskDlg::GetMap(int i_mapType, bool i_original)
{
 switch(i_mapType)
 {
  case TYPE_MAP_DA_START:
   return m_pPageDlg->mp_ButtonsPanel->GetStartMap(i_original);

  case TYPE_MAP_DA_IDLE:
   return m_pPageDlg->mp_ButtonsPanel->GetIdleMap(i_original);

  case TYPE_MAP_DA_WORK:
   return m_pPageDlg->mp_ButtonsPanel->GetWorkMap(i_original);

  case TYPE_MAP_DA_TEMP_CORR:
   return m_pPageDlg->mp_ButtonsPanel->GetTempMap(i_original);

  default:
  return NULL;
 }
}

void CTablesDeskDlg::SetDynamicValues(int rpm, float temp, float adv_ang, float knock_retard, bool knkret_use,
 float strt_aalt, bool strt_use, float idle_aalt, bool idle_use, float work_aalt, bool work_use, float temp_aalt, bool temp_use,
 float airt_aalt, bool airt_use, float idlreg_aac, bool idlreg_use, float octan_aac, bool octan_use)
{
 if (!m_pPageDlg) return;
 CGridModeEditorDlg::DynVal dv;
 dv.rpm = rpm;
 dv.temp = temp;
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
 m_pPageDlg->mp_ButtonsPanel->SetDynamicValues(dv);
}

void CTablesDeskDlg::EnableAdvanceAngleIndication(bool i_enable)
{
 if (!m_pPageDlg) return;
 m_pPageDlg->mp_ButtonsPanel->EnableAdvanceAngleIndication(i_enable);
}

//------------------------------------------------------------------------
void CTablesDeskDlg::OnDestroy()
{
 Super::OnDestroy();
 m_hot_keys_supplier->Close();
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
  m_OnMapChanged(GetCurSel(), i_mapType);
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

void CTablesDeskDlg::OnChangeTablesSetName(void)
{
 if (m_OnChangeTablesSetName)
  m_OnChangeTablesSetName(GetCurSel());
}

//------------------------------------------------------------------------
void CTablesDeskDlg::OnSelchangeTabctl(void)
{
 if (m_OnTabActivate)
  m_OnTabActivate();
}

bool CTablesDeskDlg::OnSelchangingTabctl(void)
{
 return true; //allow change of tab
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

void CTablesDeskDlg::_RegisterHotKeys(void)
{
#define RegisterHK(d,k) m_hot_keys_supplier->RegisterCommand(ID_TD_ACTIVATE_##d, k, MOD_CONTROL);

 RegisterHK(GASOLINE_TAB, VK_F11);
 RegisterHK(GAS_TAB, VK_F12);
}

#define OnHK_XXX(x)\
void CTablesDeskDlg::OnHK_##x()\
{\
 if (!m_enabled)\
  return;\
 m_tab_control.SetCurSel(##x);\
}

OnHK_XXX(GASOLINE_TAB)
OnHK_XXX(GAS_TAB)
