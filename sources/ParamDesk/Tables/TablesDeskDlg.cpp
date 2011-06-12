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
 m_pGasolinePageDlg = new CTablesPageDlg();
 m_pGasolinePageDlg->setFunctionOnChange(MakeDelegate(this, &CTablesDeskDlg::OnChangeInTab));

 m_pGasPageDlg = new CTablesPageDlg();
 m_pGasPageDlg->setFunctionOnChange(MakeDelegate(this, &CTablesDeskDlg::OnChangeInTab));
}

CTablesDeskDlg::~CTablesDeskDlg()
{
 delete m_pImgList;
 delete m_pGasolinePageDlg;
 delete m_pGasPageDlg;
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
 m_tab_control.AddPage(MLL::LoadString(IDS_TD_TABNAME_GASOLINE), m_pGasolinePageDlg, GASOLINE_TAB);
 m_tab_control.AddPage(MLL::LoadString(IDS_TD_TABNAME_GAS), m_pGasPageDlg, GAS_TAB);

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

void CTablesDeskDlg::OnDestroy()
{
 Super::OnDestroy();
 m_hot_keys_supplier->Close();
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
 m_pGasolinePageDlg->Enable(enable);
 m_pGasPageDlg->Enable(enable);

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
/*
bool CTablesDeskDlg::SetValues(BYTE i_descriptor, const void* i_values)
{
 using namespace SECU3IO;
 switch(i_descriptor)
 {
  case TEMPER_PAR:
   m_pTemperPageDlg->SetValues((TemperPar*)i_values);
   break;
  case MISCEL_PAR:
   m_pMiscPageDlg->SetValues((MiscelPar*)i_values);
   break;
  case SENSOR_DAT:
  default:
   return false; //неизвестный или неподдерживаемый дескриптор
 }//switch

 return true;
}

bool CTablesDeskDlg::GetValues(BYTE i_descriptor, void* o_values)
{
 using namespace SECU3IO;
 switch(i_descriptor)
 {
  case TEMPER_PAR:
   m_pTemperPageDlg->GetValues((TemperPar*)o_values);
   break;
  case MISCEL_PAR:
   m_pMiscPageDlg->GetValues((MiscelPar*)o_values);
   break;
  case FNNAME_DAT:
  default:
   return false; //неизвестный или неподдерживаемый дескриптор
 }//switch

 return true;
}*/

/*
//Устанавливает имена семейств характеристик
void CTablesDeskDlg::SetFunctionsNames(const std::vector<_TSTRING>& i_names)
{
 //m_pFunSetPageDlg->AccessFunNames() = i_names;
 //m_pFunSetPageDlg->FillCBByFunNames();
}

const std::vector<_TSTRING>& CTablesDeskDlg::GetFunctionsNames(void)
{
 return std::vector<_TSTRING>();//m_pFunSetPageDlg->AccessFunNames();
}*/

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

void CTablesDeskDlg::OnChangeInTab(void)
{
 if (m_OnChangeInTab)
  m_OnChangeInTab();
}

void CTablesDeskDlg::ShowSaveButton(bool i_show)
{
 m_save_button.ShowWindow(i_show ? SW_SHOW : SW_HIDE);
}

void CTablesDeskDlg::SetOnTabActivate(EventHandler OnTabActivate)
{
 m_OnTabActivate = OnTabActivate;
}

void CTablesDeskDlg::SetOnChangeInTab(EventHandler OnChangeInTab)
{
 m_OnChangeInTab = OnChangeInTab;
}

void CTablesDeskDlg::SetOnSaveButton(EventHandler OnSaveButton)
{
 m_OnSaveButton  = OnSaveButton;
}

bool CTablesDeskDlg::SetCurSel(int sel)
{
 return m_tab_control.SetCurSel(sel);
}

int CTablesDeskDlg::GetCurSel(void)
{
 return m_tab_control.GetCurSel();
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
