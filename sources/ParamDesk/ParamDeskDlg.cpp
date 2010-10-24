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
              http://secu-3.narod.ru
              email: secu-3@yandex.ru
*/

#pragma warning (disable:C4786)

#include "stdafx.h"
#include "resource.h"
#include "ParamDeskDlg.h"

#include <map>

//вкладки
#include "ADCCompenPageDlg.h"
#include "AnglesPageDlg.h" 
#include "CarburPageDlg.h" 
#include "CKPSPageDlg.h"
#include "FunSetPageDlg.h" 
#include "IdlRegPageDlg.h" 
#include "KnockPageDlg.h"
#include "MiscPageDlg.h"
#include "StarterPageDlg.h" 
#include "TemperPageDlg.h"

#include "common\FastDelegate.h"
#include "io-core\SECU3IO.h"
#include "io-core\ufcodes.h" 
#include "ui-core\HotKeysToCmdRouter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define TAB_CTRL_BITMAPS_COLOR_MASK RGB(192,192,192)

using namespace std;
using namespace fastdelegate;

/////////////////////////////////////////////////////////////////////////////
// CParamDeskDlg dialog

UINT CParamDeskDlg::IDD = IDD_PARAMETERS_DESK;
UINT CParamDeskDlg::IDD_F = IDD_PARAMETERS_DESK_FLOATING;

CParamDeskDlg::CParamDeskDlg(CWnd* pParent /*=NULL*/, bool i_show_knock_page /* = false*/)
: Super(CParamDeskDlg::IDD, pParent)
, m_pImgList(NULL)
, m_enabled(FALSE)
, m_show_knock_page(i_show_knock_page)
, m_hot_keys_supplier(new CHotKeysToCmdRouter())
{   
 //создаем image list для TabCtrl
 m_pImgList = new CImageList(); 
 m_pImgList->Create(16, 16, ILC_COLOR24|ILC_MASK, 0, 0);
 CBitmap bitmap;
 bitmap.LoadBitmap((LPCTSTR)IDB_TAB_CTRL_BITMAPS);
 m_pImgList->Add(&bitmap, TAB_CTRL_BITMAPS_COLOR_MASK);

 //их надо создать только один раз
 m_pStarterPageDlg = new CStarterPageDlg();
 m_pStarterPageDlg->setFunctionOnChange(MakeDelegate(this,&CParamDeskDlg::OnChangeInTab));

 m_pAnglesPageDlg = new CAnglesPageDlg();		 
 m_pAnglesPageDlg->setFunctionOnChange(MakeDelegate(this,&CParamDeskDlg::OnChangeInTab));

 m_pIdlRegPageDlg = new CIdlRegPageDlg();	  
 m_pIdlRegPageDlg->setFunctionOnChange(MakeDelegate(this,&CParamDeskDlg::OnChangeInTab));

 m_pFunSetPageDlg = new CFunSetPageDlg();		 
 m_pFunSetPageDlg->setFunctionOnChange(MakeDelegate(this,&CParamDeskDlg::OnChangeInTab));

 m_pTemperPageDlg = new CTemperPageDlg();		  
 m_pTemperPageDlg->setFunctionOnChange(MakeDelegate(this,&CParamDeskDlg::OnChangeInTab));

 m_pCarburPageDlg = new CCarburPageDlg();
 m_pCarburPageDlg->setFunctionOnChange(MakeDelegate(this,&CParamDeskDlg::OnChangeInTab));

 m_pADCCompenPageDlg = new CADCCompenPageDlg();
 m_pADCCompenPageDlg->setFunctionOnChange(MakeDelegate(this,&CParamDeskDlg::OnChangeInTab));

 m_pCKPSPageDlg = new CCKPSPageDlg();
 m_pCKPSPageDlg->setFunctionOnChange(MakeDelegate(this,&CParamDeskDlg::OnChangeInTab));

 if (m_show_knock_page)
 {
  m_pKnockPageDlg = new CKnockPageDlg();
  m_pKnockPageDlg->setFunctionOnChange(MakeDelegate(this,&CParamDeskDlg::OnChangeInTab));
 }

 m_pMiscPageDlg = new CMiscPageDlg();
 m_pMiscPageDlg->setFunctionOnChange(MakeDelegate(this,&CParamDeskDlg::OnChangeInTab));
}

CParamDeskDlg::~CParamDeskDlg()
{  
 delete m_pImgList;	

 delete m_pStarterPageDlg; 		 
 delete m_pAnglesPageDlg; 		 
 delete m_pIdlRegPageDlg;	  
 delete m_pFunSetPageDlg;		 
 delete m_pTemperPageDlg;		  
 delete m_pCarburPageDlg;
 delete m_pADCCompenPageDlg;
 delete m_pCKPSPageDlg;
 if (m_show_knock_page)  
  delete m_pKnockPageDlg;
 delete m_pMiscPageDlg;
}

void CParamDeskDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);
 DDX_Control(pDX, IDC_PARAMETERS_DESK_TITLE, m_pd_title);
 DDX_Control(pDX, IDC_PD_TAB_CTRL, m_tab_control);
 DDX_Control(pDX, IDC_PD_SAVE_BUTTON, m_save_button);
}

BEGIN_MESSAGE_MAP(CParamDeskDlg, Super)
 ON_WM_DESTROY()
 ON_UPDATE_COMMAND_UI(IDC_PARAMETERS_DESK_TITLE,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_TAB_CTRL,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_SAVE_BUTTON,OnUpdateControls)
 ON_BN_CLICKED(IDC_PD_SAVE_BUTTON,OnSaveButton)

#define ON_COMMAND_HK_XXX(x)\
 ON_COMMAND(ID_PD_ACTIVATE_##x, OnHK_##x)

 ON_COMMAND_HK_XXX(STARTR_PAR)
 ON_COMMAND_HK_XXX(ANGLES_PAR)
 ON_COMMAND_HK_XXX(IDLREG_PAR)
 ON_COMMAND_HK_XXX(FUNSET_PAR)
 ON_COMMAND_HK_XXX(TEMPER_PAR)
 ON_COMMAND_HK_XXX(CARBUR_PAR)
 ON_COMMAND_HK_XXX(ADCCOR_PAR)
 ON_COMMAND_HK_XXX(CKPS_PAR)
 ON_COMMAND_HK_XXX(KNOCK_PAR)
 ON_COMMAND_HK_XXX(MISCEL_PAR)

END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CParamDeskDlg message handlers

//если надо апдейтить отдельные контроллы, то надо будет плодить функции
void CParamDeskDlg::OnUpdateControls(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled);
}

BOOL CParamDeskDlg::OnInitDialog() 
{
 Super::OnInitDialog();
	
 //контрол создан через ресурсы и стили описаны в ресурсах.
 m_tab_control.SetImageList(m_pImgList);
 m_tab_control.SetResourceModule(::GetModuleHandle(_T("paramdesk.dll")));
 m_tab_control.Init();
	
 m_tab_descriptors.clear();
 //наполняем Tab control вкладками	
 m_tab_descriptors.insert(TabDescriptor::value_type(m_tab_control.AddPage(MLL::LoadString(IDS_PD_TABNAME_STARTR_PAR),m_pStarterPageDlg,0), STARTR_PAR));
 m_tab_descriptors.insert(TabDescriptor::value_type(m_tab_control.AddPage(MLL::LoadString(IDS_PD_TABNAME_ANGLES_PAR),m_pAnglesPageDlg,1), ANGLES_PAR));	
 m_tab_descriptors.insert(TabDescriptor::value_type(m_tab_control.AddPage(MLL::LoadString(IDS_PD_TABNAME_IDLREG_PAR),m_pIdlRegPageDlg,2), IDLREG_PAR));
 m_tab_descriptors.insert(TabDescriptor::value_type(m_tab_control.AddPage(MLL::LoadString(IDS_PD_TABNAME_FUNSET_PAR),m_pFunSetPageDlg,3), FUNSET_PAR));
 m_tab_descriptors.insert(TabDescriptor::value_type(m_tab_control.AddPage(MLL::LoadString(IDS_PD_TABNAME_TEMPER_PAR),m_pTemperPageDlg,4), TEMPER_PAR));
 m_tab_descriptors.insert(TabDescriptor::value_type(m_tab_control.AddPage(MLL::LoadString(IDS_PD_TABNAME_CARBUR_PAR),m_pCarburPageDlg,5), CARBUR_PAR));
 m_tab_descriptors.insert(TabDescriptor::value_type(m_tab_control.AddPage(MLL::LoadString(IDS_PD_TABNAME_ADCCOR_PAR),m_pADCCompenPageDlg,6), ADCCOR_PAR));
 m_tab_descriptors.insert(TabDescriptor::value_type(m_tab_control.AddPage(MLL::LoadString(IDS_PD_TABNAME_CKPS_PAR),m_pCKPSPageDlg,7), CKPS_PAR));
 if (m_show_knock_page)
  m_tab_descriptors.insert(TabDescriptor::value_type(m_tab_control.AddPage(MLL::LoadString(IDS_PD_TABNAME_KNOCK_PAR),m_pKnockPageDlg,8), KNOCK_PAR));
 m_tab_descriptors.insert(TabDescriptor::value_type(m_tab_control.AddPage(MLL::LoadString(IDS_PD_TABNAME_MISCEL_PAR),m_pMiscPageDlg,9), MISCEL_PAR));
	
 //ВНИМАНИЕ! SetEventListener должен быть вызван раньше чем SetCurSel, т.к. SetCurSel 
 //уже использует обработчики сообщений!

 //this будет получать события от Tab control-а и делегировать их указанным обработчикам
 m_tab_control.SetEventListener(this);   

 //устанавливаем предыдущее значение (разрешены вкладки или нет)
 m_tab_control.EnableItem(-1,m_enabled ? true : false);

 m_hot_keys_supplier->Init(this);
 _RegisterHotKeys();

 UpdateDialogControls(this,TRUE);
 return TRUE;  // return TRUE unless you set the focus to a control
	           // EXCEPTION: OCX Property Pages should return FALSE
}

BYTE CParamDeskDlg::GetCurrentDescriptor(void)
{
 int selection = m_tab_control.GetCurSel();
 if (m_tab_descriptors.find(selection)==m_tab_descriptors.end())
 {
  ASSERT(0); //unexpected selection index!
  return 0;
 }
 
 return m_tab_descriptors[selection];
}

void CParamDeskDlg::OnDestroy() 
{  
 Super::OnDestroy();	  
 m_hot_keys_supplier->Close();
}

void CParamDeskDlg::SetPosition(int x_pos, int y_pos)
{
 SetWindowPos(NULL,x_pos,y_pos,0,0,SWP_NOZORDER|SWP_NOSIZE);
}

void CParamDeskDlg::SetTitle(const CString& i_str)
{
 m_pd_title.SetWindowText(i_str);
}

void CParamDeskDlg::GetTitle(CString& o_str)
{
 m_pd_title.GetWindowText(o_str);
}

//разрешение/запрещение
void CParamDeskDlg::Enable(bool enable)
{
 m_enabled = (enable) ? TRUE : FALSE;
 m_pStarterPageDlg->Enable(enable);		 
 m_pAnglesPageDlg->Enable(enable);
 m_pIdlRegPageDlg->Enable(enable);
 m_pFunSetPageDlg->Enable(enable);
 m_pTemperPageDlg->Enable(enable);
 m_pCarburPageDlg->Enable(enable);
 m_pADCCompenPageDlg->Enable(enable);
 m_pCKPSPageDlg->Enable(enable);
 if (m_show_knock_page)
  m_pKnockPageDlg->Enable(enable);
 m_pMiscPageDlg->Enable(enable);

 if (::IsWindow(m_hWnd))
  UpdateDialogControls(this,TRUE);

 m_tab_control.EnableItem(-1,enable); //all items  
}

bool CParamDeskDlg::IsEnabled(void) 
{ 
 return m_enabled ? true : false;
}

//спрятать/показать все
void CParamDeskDlg::Show(bool show)
{
 int nCmdShow = (show) ? SW_SHOW : SW_HIDE;
 m_tab_control.ShowWindow(nCmdShow);
}

// Устанавливает данные указанной вкладки, вкладка определяется дескриптором
bool CParamDeskDlg::SetValues(BYTE i_descriptor, const void* i_values)
{
 using namespace SECU3IO;
 switch(i_descriptor)
 {
  case TEMPER_PAR:
   m_pTemperPageDlg->SetValues((TemperPar*)i_values);
   break;
  case CARBUR_PAR:
   m_pCarburPageDlg->SetValues((CarburPar*)i_values);
   break;
  case IDLREG_PAR: 
   m_pIdlRegPageDlg->SetValues((IdlRegPar*)i_values);
   break;
  case ANGLES_PAR:
   m_pAnglesPageDlg->SetValues((AnglesPar*)i_values);
   break;
  case FUNSET_PAR:
   m_pFunSetPageDlg->SetValues((FunSetPar*)i_values);
   break;
  case STARTR_PAR:
   m_pStarterPageDlg->SetValues((StartrPar*)i_values);
   break;
  case ADCCOR_PAR:
   m_pADCCompenPageDlg->SetValues((ADCCompenPar*)i_values);
   break;
  case CKPS_PAR:
   m_pCKPSPageDlg->SetValues((CKPSPar*)i_values);
   break;
  case KNOCK_PAR:
   if (!m_show_knock_page)
    return false;
   m_pKnockPageDlg->SetValues((KnockPar*)i_values);
   break;
  case MISCEL_PAR:
   m_pMiscPageDlg->SetValues((MiscelPar*)i_values);
   break;
  case FNNAME_DAT:     
  case SENSOR_DAT:					      
  default:
   return false; //неизвестный или неподдерживаемый дескриптор
 }//switch 
  
 return true;
}

// Получает данные из указанной вкладки, вкладка определяется дескриптором
bool CParamDeskDlg::GetValues(BYTE i_descriptor, void* o_values)
{
 using namespace SECU3IO;
 switch(i_descriptor)
 {
  case TEMPER_PAR:
   m_pTemperPageDlg->GetValues((TemperPar*)o_values);
   break;
  case CARBUR_PAR:
   m_pCarburPageDlg->GetValues((CarburPar*)o_values);
   break;
  case IDLREG_PAR: 
   m_pIdlRegPageDlg->GetValues((IdlRegPar*)o_values);
   break;
  case ANGLES_PAR:
   m_pAnglesPageDlg->GetValues((AnglesPar*)o_values);
   break;
  case FUNSET_PAR:
   m_pFunSetPageDlg->GetValues((FunSetPar*)o_values);
   break;
  case STARTR_PAR:
   m_pStarterPageDlg->GetValues((StartrPar*)o_values);
   break;
  case ADCCOR_PAR:
   m_pADCCompenPageDlg->GetValues((ADCCompenPar*)o_values);
   break;
  case CKPS_PAR:
   m_pCKPSPageDlg->GetValues((CKPSPar*)o_values);
   break;
  case KNOCK_PAR:
   if (!m_show_knock_page)
    return false;
   m_pKnockPageDlg->GetValues((KnockPar*)o_values);
   break;
  case MISCEL_PAR:
   m_pMiscPageDlg->GetValues((MiscelPar*)o_values);
   break;
  case FNNAME_DAT:     
  case SENSOR_DAT:					      
  default:
   return false; //неизвестный или неподдерживаемый дескриптор
 }//switch        

 return true;
}

//Устанавливает имена семейств характеристик
void CParamDeskDlg::SetFunctionsNames(const std::vector<_TSTRING>& i_names)
{
 m_pFunSetPageDlg->AccessFunNames() = i_names;
 m_pFunSetPageDlg->FillCBByFunNames();
}

const std::vector<_TSTRING>& CParamDeskDlg::GetFunctionsNames(void) 
{ 
 return m_pFunSetPageDlg->AccessFunNames();
}

void CParamDeskDlg::OnSaveButton()
{
 if (m_OnSaveButton)
  m_OnSaveButton();
}

void CParamDeskDlg::OnSelchangeTabctl(void) 
{ 
 if (m_OnTabActivate) 
  m_OnTabActivate();
} 

void CParamDeskDlg::OnSelchangingTabctl(void) 
{
 //empty
}

void CParamDeskDlg::OnChangeInTab(void) 
{ 
 if (m_OnChangeInTab) 
  m_OnChangeInTab();
}

void CParamDeskDlg::ShowSaveButton(bool i_show)
{
 m_save_button.ShowWindow(i_show ? SW_SHOW : SW_HIDE);
}

int CParamDeskDlg::_GetTabIndex(unsigned i_descriptor)
{
 TabDescriptor::const_iterator it = m_tab_descriptors.begin();
 for (; it != m_tab_descriptors.end(); ++it)
 {
  if ((*it).second == i_descriptor)
   return (*it).first;
 }
 ASSERT(0);
 return -1;
}

void CParamDeskDlg::SetOnTabActivate(EventHandler OnTabActivate) 
{
 m_OnTabActivate = OnTabActivate;
}

void CParamDeskDlg::SetOnChangeInTab(EventHandler OnChangeInTab) 
{
 m_OnChangeInTab = OnChangeInTab;
}

void CParamDeskDlg::SetOnSaveButton(EventHandler OnSaveButton)
{
 m_OnSaveButton  = OnSaveButton;
}

bool CParamDeskDlg::SetCurSel(int sel)
{
 return m_tab_control.SetCurSel(sel);
}

int CParamDeskDlg::GetCurSel(void) 
{
 return m_tab_control.GetCurSel();
}

void CParamDeskDlg::_RegisterHotKeys(void)
{
#define RegisterHK(d,k) m_hot_keys_supplier->RegisterCommand(ID_PD_ACTIVATE_##d, k, 0);

 RegisterHK(STARTR_PAR, VK_F1);
 RegisterHK(ANGLES_PAR, VK_F2);
 RegisterHK(IDLREG_PAR, VK_F3);
 RegisterHK(FUNSET_PAR, VK_F4);
 RegisterHK(TEMPER_PAR, VK_F5);
 RegisterHK(CARBUR_PAR, VK_F6);
 RegisterHK(ADCCOR_PAR, VK_F7);
 RegisterHK(CKPS_PAR,   VK_F8);
 if (m_show_knock_page) 
  RegisterHK(KNOCK_PAR,  VK_F9);
 RegisterHK(MISCEL_PAR, VK_F10);
}

#define OnHK_XXX(x)\
void CParamDeskDlg::OnHK_##x()\
{\
 if (!m_enabled)\
  return;\
 m_tab_control.SetCurSel(_GetTabIndex(##x));\
}

OnHK_XXX(STARTR_PAR)
OnHK_XXX(ANGLES_PAR)
OnHK_XXX(IDLREG_PAR)
OnHK_XXX(FUNSET_PAR)
OnHK_XXX(TEMPER_PAR)
OnHK_XXX(CARBUR_PAR)
OnHK_XXX(ADCCOR_PAR)
OnHK_XXX(CKPS_PAR)
OnHK_XXX(KNOCK_PAR)
OnHK_XXX(MISCEL_PAR)
