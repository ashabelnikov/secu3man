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

#pragma warning (disable:C4786)

#include "stdafx.h"
#include "Resources/resource.h"
#include "TablesDeskDlg.h"

#include <map>
#include "common/FastDelegate.h"
#include "TablDesk/ButtonsPanel.h"
#include "TablDesk/MapIds.h"
#include "TablesPageDlg.h"
#include "ui-core/HotKeysToCmdRouter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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
, m_enabled(FALSE)
, m_hot_keys_supplier(new CHotKeysToCmdRouter())
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
 ON_UPDATE_COMMAND_UI(IDC_TABLES_DESK_TITLE, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_TD_TAB_CTRL, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_TD_SAVE_BUTTON, OnUpdateControls)
 ON_BN_CLICKED(IDC_TD_SAVE_BUTTON, OnSaveButton)

#define ON_COMMAND_HK_XXX(x)\
 ON_COMMAND(ID_TD_ACTIVATE_##x, OnHK_##x)

 ON_COMMAND_HK_XXX(GASOLINE_TAB)
 ON_COMMAND_HK_XXX(GAS_TAB)

END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CTablesDeskDlg message handlers

//если надо апдейтить отдельные контроллы, то надо будет плодить функции
void CTablesDeskDlg::OnUpdateControls(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled);
}

BOOL CTablesDeskDlg::OnInitDialog()
{
 Super::OnInitDialog();

 //контрол создан через ресурсы и стили описаны в ресурсах.
 m_tab_control.SetImageList(m_pImgList);
 m_tab_control.SetResourceModule(::GetModuleHandle(_T("paramdesk.dll")));
 m_tab_control.Init();

 //наполняем Tab control вкладками
 m_tab_control.AddPage(MLL::LoadString(IDS_TD_TABNAME_GASOLINE), m_pPageDlg, GASOLINE_TAB);
 m_tab_control.AddPage(MLL::LoadString(IDS_TD_TABNAME_GAS), m_pPageDlg, GAS_TAB);

 //ВНИМАНИЕ! SetEventListener должен быть вызван раньше чем SetCurSel, т.к. SetCurSel
 //уже использует обработчики сообщений!

 //this будет получать события от Tab control-а и делегировать их указанным обработчикам
 m_tab_control.SetEventListener(this);

 //устанавливаем предыдущее значение (разрешены вкладки или нет)
 m_tab_control.EnableItem(-1, m_enabled ? true : false);

 m_hot_keys_supplier->Init(this);
 _RegisterHotKeys();

 UpdateDialogControls(this,TRUE);
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

//разрешение/запрещение
void CTablesDeskDlg::Enable(bool enable)
{
 m_enabled = (enable) ? TRUE : FALSE;
 m_pPageDlg->Enable(enable);

 if (::IsWindow(m_hWnd))
  UpdateDialogControls(this,TRUE);

 m_tab_control.EnableItem(-1, enable); //all items
}

bool CTablesDeskDlg::IsEnabled(void)
{
 return m_enabled ? true : false;
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
 if (m_OnOpenMapWnd)
  m_OnOpenMapWnd(i_hwnd, i_mapType);
}

void CTablesDeskDlg::OnChangeTablesSetName(void)
{
 if (m_OnChangeTablesSetName)
  m_OnChangeTablesSetName(GetCurSel());
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

//------------------------------------------------------------------------
void CTablesDeskDlg::OnDestroy()
{
 Super::OnDestroy();
 m_hot_keys_supplier->Close();
}

void CTablesDeskDlg::OnSaveButton()
{
 if (m_OnSaveButton)
  m_OnSaveButton();
}

void CTablesDeskDlg::OnSelchangeTabctl(void)
{
 if (m_OnTabActivate)
  m_OnTabActivate();
}

void CTablesDeskDlg::OnSelchangingTabctl(void)
{
 //empty
}

//------------------------------------------------------------------------
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
