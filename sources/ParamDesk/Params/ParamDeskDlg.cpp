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

/** \file ParamDeskDlg.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "Resources/resource.h"
#include "ParamDeskDlg.h"

#include <map>

//tabs
#include "AccelEnrPageDlg.h"
#include "ADCCompenPageDlg.h"
#include "AnglesPageDlg.h"
#include "CarburPageDlg.h"
#include "ChokePageDlg.h"
#include "CKPSPageDlg.h"
#include "FunSetPageDlg.h"
#include "GasdosePageDlg.h"
#include "IdlRegPageDlg.h"
#include "KnockPageDlg.h"
#include "MiscPageDlg.h"
#include "SecurPageDlg.h"
#include "StarterPageDlg.h"
#include "TemperPageDlg.h"
#include "UniOutPageDlg.h"
#include "InjectorPageDlg.h"
#include "LambdaPageDlg.h"

#include "common/Dll.h"
#include "common/FastDelegate.h"
#include "io-core/SECU3IO.h"
#include "io-core/ufcodes.h"
#include "ui-core/HotKeysToCmdRouter.h"
#include "common/MathHelpers.h"

#define TAB_CTRL_BITMAPS_COLOR_MASK RGB(192,192,192)

using namespace std;
using namespace fastdelegate;

/////////////////////////////////////////////////////////////////////////////
// CParamDeskDlg dialog

const UINT CParamDeskDlg::IDD = IDD_PARAMETERS_DESK;

CParamDeskDlg::CParamDeskDlg(CWnd* pParent /*=NULL*/, bool i_show_knock_page /* = false*/)
: Super(CParamDeskDlg::IDD, pParent)
, m_pImgList(NULL)
, m_enabled(false)
, m_fuel_injection(false)
, m_lambda(false)
, m_gasdose(false)
, m_choke(false)
, m_choke_ctrls(false)
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

 m_pChokePageDlg = new CChokePageDlg();
 m_pChokePageDlg->setFunctionOnChange(MakeDelegate(this,&CParamDeskDlg::OnChangeInTab));

 m_pSecurPageDlg = new CSecurPageDlg();
 m_pSecurPageDlg->setFunctionOnChange(MakeDelegate(this,&CParamDeskDlg::OnChangeInTab));

 m_pUniOutPageDlg = new CUniOutPageDlg();
 m_pUniOutPageDlg->setFunctionOnChange(MakeDelegate(this,&CParamDeskDlg::OnChangeInTab)); 

 m_pInjectorPageDlg = new CInjectorPageDlg();
 m_pInjectorPageDlg->setFunctionOnChange(MakeDelegate(this,&CParamDeskDlg::OnChangeInTab)); 

 m_pLambdaPageDlg = new CLambdaPageDlg();
 m_pLambdaPageDlg->setFunctionOnChange(MakeDelegate(this,&CParamDeskDlg::OnChangeInTab)); 

 m_pAccelEnrPageDlg = new CAccelEnrPageDlg();
 m_pAccelEnrPageDlg->setFunctionOnChange(MakeDelegate(this,&CParamDeskDlg::OnChangeInTab)); 

 m_pGasdosePageDlg = new CGasdosePageDlg();
 m_pGasdosePageDlg->setFunctionOnChange(MakeDelegate(this,&CParamDeskDlg::OnChangeInTab));
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
 delete m_pChokePageDlg;
 delete m_pSecurPageDlg;
 delete m_pUniOutPageDlg;
 delete m_pInjectorPageDlg;
 delete m_pLambdaPageDlg;
 delete m_pAccelEnrPageDlg;
 delete m_pGasdosePageDlg;
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
 ON_WM_SYSCOMMAND()
 ON_WM_SIZE()
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
 ON_COMMAND_HK_XXX(CHOKE_PAR)

END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CParamDeskDlg message handlers

//This function is called by framework to update controls
void CParamDeskDlg::OnUpdateControls(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled);
}

BOOL CParamDeskDlg::OnInitDialog()
{
 Super::OnInitDialog();
 int idx;

 //Control created in resouces and where all its styles are defined too
 m_tab_control.SetImageList(m_pImgList);
 m_tab_control.SetResourceModule(DLL::GetModuleHandle());
 m_tab_control.Init();

 m_tab_descriptors.clear();
 //Fill Tab control with tabs
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
 m_tab_descriptors.insert(TabDescriptor::value_type(idx = m_tab_control.AddPage(MLL::LoadString(IDS_PD_TABNAME_CHOKE_PAR),m_pChokePageDlg,10), CHOKE_PAR));
 m_choke_tab_idx = idx;
 m_tab_descriptors.insert(TabDescriptor::value_type(m_tab_control.AddPage(MLL::LoadString(IDS_PD_TABNAME_SECUR_PAR),m_pSecurPageDlg,11), SECUR_PAR));
 m_tab_descriptors.insert(TabDescriptor::value_type(m_tab_control.AddPage(MLL::LoadString(IDS_PD_TABNAME_UNIOUT_PAR),m_pUniOutPageDlg,12), UNIOUT_PAR));
 m_tab_descriptors.insert(TabDescriptor::value_type(idx = m_tab_control.AddPage(MLL::LoadString(IDS_PD_TABNAME_INJCTR_PAR),m_pInjectorPageDlg,13), INJCTR_PAR));
 m_fuel_inject_idx = idx; //remember index of tab
 m_tab_descriptors.insert(TabDescriptor::value_type(idx = m_tab_control.AddPage(MLL::LoadString(IDS_PD_TABNAME_LAMBDA_PAR),m_pLambdaPageDlg,14), LAMBDA_PAR));
 m_lambda_tab_idx = idx;
 m_tab_descriptors.insert(TabDescriptor::value_type(idx = m_tab_control.AddPage(MLL::LoadString(IDS_PD_TABNAME_ACCEL_PAR),m_pAccelEnrPageDlg,15), ACCEL_PAR));
 m_accenr_tab_idx = idx;

 //GD tab
 m_tab_descriptors.insert(TabDescriptor::value_type(idx = m_tab_control.AddPage(MLL::LoadString(IDS_PD_TABNAME_GASDOSE_PAR),m_pGasdosePageDlg,16), GASDOSE_PAR));
 m_gasdose_tab_idx = idx;

 //Warning! SetEventListener must be called before SetCurSel, because SetCurSel
 //already uses event handlers

 //this will receive events from Tab control and delegate them to specified handlers
 m_tab_control.SetEventListener(this);

 //set previous value (tabs are enabled or not)
 m_tab_control.EnableItem(-1, m_enabled);

 //disable fuel injection related tab if fuel injection is not supported
 if (false==m_fuel_injection)
 {
  m_tab_control.EnableItem(m_fuel_inject_idx, false);
 }

 //disable acceleration enrichment tab if fuel injection and gas doser are not supported
 if (false==m_fuel_injection && false==m_gasdose)
 {
  m_tab_control.EnableItem(m_accenr_tab_idx, false);
 }

 //disable lambda settings tab if lambda support is not included
 if (false==m_lambda)
 {
  m_tab_control.EnableItem(m_lambda_tab_idx, false);
 }

 //disable gas dose settings tab if gas dose support is not included
 if (false==m_gasdose)
 {
  m_tab_control.EnableItem(m_gasdose_tab_idx, false);
 }

 //disable choke settings tab if choke support is not included
 if (false==m_choke)
 {
  m_tab_control.EnableItem(m_choke_tab_idx, false);
 }

 m_hot_keys_supplier->Init(this);
 _RegisterHotKeys();

 UpdateDialogControls(this, TRUE);

 ///////////////////////////////////////////////
 //TODO: Replace by MIHelpers (see MIHelpers.h) or even more all releted functions to common
 //Calculate and remember right and bottom margins of tab control
 CRect wndRectDlg, wndRectTab;
 GetClientRect(&wndRectDlg);
 m_tab_control.GetWindowRect(&wndRectTab);
 ScreenToClient(&wndRectTab);
 m_tabLRMargin.cx = wndRectDlg.right - wndRectTab.right;
 m_tabLRMargin.cy = wndRectDlg.bottom - wndRectTab.bottom;
 ///////////////////////////////////////////////

 return TRUE;  // return TRUE unless you set the focus to a control
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

void CParamDeskDlg::SetPosition(int x_pos, int y_pos, CWnd* wnd_insert_after /*=NULL*/)
{
 SetWindowPos(wnd_insert_after,x_pos,y_pos,0,0, (wnd_insert_after ? 0 : SWP_NOZORDER) | SWP_NOSIZE);
}

void CParamDeskDlg::SetTitle(const CString& i_str)
{
 m_pd_title.SetWindowText(i_str);
}

void CParamDeskDlg::GetTitle(CString& o_str)
{
 m_pd_title.GetWindowText(o_str);
}

//Enable/disable
void CParamDeskDlg::Enable(bool enable)
{
 if (m_enabled == enable)
  return; //already has needed state
 m_enabled = enable;
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
 m_pChokePageDlg->Enable(enable & m_choke);
 m_pGasdosePageDlg->Enable(enable & m_gasdose);
 m_pSecurPageDlg->Enable(enable);
 m_pUniOutPageDlg->Enable(enable);
 m_pInjectorPageDlg->Enable(enable && m_fuel_injection);
 m_pLambdaPageDlg->Enable(enable && m_lambda);
 m_pAccelEnrPageDlg->Enable(enable && (m_fuel_injection || m_gasdose));

 if (::IsWindow(m_hWnd))
  UpdateDialogControls(this,TRUE);

 m_tab_control.EnableItem(-1, enable); //all items

 //disable fuel injection related tab if fuel injection is not supported
 if (false==m_fuel_injection)
 {
  m_tab_control.EnableItem(m_fuel_inject_idx, false);
 }

 if (false==m_fuel_injection && false==m_gasdose)
 {
  m_tab_control.EnableItem(m_accenr_tab_idx, false);
 }

 if (false==m_lambda)
 {
  m_tab_control.EnableItem(m_lambda_tab_idx, false);
 }

 if (false==m_gasdose)
 {
  m_tab_control.EnableItem(m_gasdose_tab_idx, false);
 }

 if (false==m_choke)
 {
  m_tab_control.EnableItem(m_choke_tab_idx, false);
 }
}

bool CParamDeskDlg::IsEnabled(void)
{
 return m_enabled;
}

//Show/hide all tabs
void CParamDeskDlg::Show(bool show)
{
 int nCmdShow = (show) ? SW_SHOW : SW_HIDE;
 m_tab_control.ShowWindow(nCmdShow);
 this->ShowWindow(nCmdShow);
}

// Sets data of specified tab, tab is defined by descriptor
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
  case CHOKE_PAR:
   m_pChokePageDlg->SetValues((ChokePar*)i_values);
   break;
  case GASDOSE_PAR:
   m_pGasdosePageDlg->SetValues((GasdosePar*)i_values);
   break;
  case SECUR_PAR:
   m_pSecurPageDlg->SetValues((SecurPar*)i_values);
   break;
  case UNIOUT_PAR:
   m_pUniOutPageDlg->SetValues((UniOutPar*)i_values);
   break;
  case INJCTR_PAR:
   m_pInjectorPageDlg->SetValues((InjctrPar*)i_values);
   break;
  case LAMBDA_PAR:
   m_pLambdaPageDlg->SetValues((LambdaPar*)i_values);
   break;
  case ACCEL_PAR:
   m_pAccelEnrPageDlg->SetValues((AccelPar*)i_values);
   break;
  case FNNAME_DAT:
  case SENSOR_DAT:
  default:
   return false; //unknown or unsupported descriptor
 }//switch

 return true;
}

// Gets data from specified tab, tab is defined by descriptor
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
  case CHOKE_PAR:
   m_pChokePageDlg->GetValues((ChokePar*)o_values);
   break;
  case GASDOSE_PAR:
   m_pGasdosePageDlg->GetValues((GasdosePar*)o_values);
   break;
  case SECUR_PAR:
   m_pSecurPageDlg->GetValues((SecurPar*)o_values);
   break;
  case UNIOUT_PAR:
   m_pUniOutPageDlg->GetValues((UniOutPar*)o_values);
   break;
  case INJCTR_PAR:
   m_pInjectorPageDlg->GetValues((InjctrPar*)o_values);
   break;
  case LAMBDA_PAR:
   m_pLambdaPageDlg->GetValues((LambdaPar*)o_values);
   break;
  case ACCEL_PAR:
   m_pAccelEnrPageDlg->GetValues((AccelPar*)o_values);
   break;
  case FNNAME_DAT:
  case SENSOR_DAT:
  default:
   return false; //unknown or unsupported descriptor
 }//switch

 return true;
}

bool CParamDeskDlg::LockUIUpdate(BYTE i_descriptor, bool lock)
{
 using namespace SECU3IO;
 switch(i_descriptor)
 {
  case CHOKE_PAR:
    m_pChokePageDlg->LockUIUpdate(lock);
   break;
  default:
   return false; //unknown or unsupported descriptor
 }//switch

 return true;
}

//Sets names of tables' sets
void CParamDeskDlg::SetFunctionsNames(const std::vector<_TSTRING>& i_names)
{
 std::vector<_TSTRING> names = i_names;
 //if string consists from all spaces or is empty, then use "<no name>"
 for(size_t index = 0; index < names.size(); ++index)
 {
  bool has_name = false;
  for(size_t i = 0; i < names[index].size(); ++i)
   if (names[index][i] != _T(' '))
    has_name = true;
  if (0==names[index].length() || false==has_name)
   names[index] = _T("<no name>");
 }

 m_pFunSetPageDlg->AccessFunNames() = names;
 m_pFunSetPageDlg->FillCBByFunNames();
}

const std::vector<_TSTRING>& CParamDeskDlg::GetFunctionsNames(void)
{
 return m_pFunSetPageDlg->AccessFunNames();
}

void CParamDeskDlg::EnableIgnitionCogs(bool enable)
{
 m_pCKPSPageDlg->EnableIgnitionCogs(enable);
}

void CParamDeskDlg::EnableUseVentPwm(bool enable)
{
 m_pTemperPageDlg->EnableUseVentPwm(enable);
}

void CParamDeskDlg::EnableUseCTSCurveMap(bool enable)
{
 m_pTemperPageDlg->EnableUseCTSCurveMap(enable);
}

void CParamDeskDlg::EnableHallOutputParams(bool enable)
{
 m_pMiscPageDlg->EnableHallOutputParams(enable);
}

void CParamDeskDlg::SetMaxCylinders(int number)
{
 m_pCKPSPageDlg->SetMaxCylinders(number);
}

void CParamDeskDlg::EnableOddCylinders(bool enable)
{
 m_pCKPSPageDlg->EnableOddCylinders(enable);
}

void CParamDeskDlg::EnableSECU3TItems(bool i_enable)
{
 m_pADCCompenPageDlg->EnableSECU3TItems(i_enable);
 m_pMiscPageDlg->EnableSECU3TItems(i_enable);
 m_pTemperPageDlg->EnableSECU3TItems(i_enable);
 m_pFunSetPageDlg->EnableSECU3TItems(i_enable);
 m_pInjectorPageDlg->EnableSECU3TItems(i_enable);
}

void CParamDeskDlg::EnableExtraIO(bool i_enable)
{
 m_pADCCompenPageDlg->EnableExtraIO(i_enable);
}

void CParamDeskDlg::EnableChokeTesting(bool i_enable)
{
 m_pChokePageDlg->EnableChokeTesting(i_enable);
}

void CParamDeskDlg::EnableChokeManPos(bool i_enable)
{
 m_pChokePageDlg->EnableChokeManPos(i_enable);
}

void CParamDeskDlg::EnableGasdoseTesting(bool i_enable)
{
 m_pGasdosePageDlg->EnableGasdoseTesting(i_enable);
}

void CParamDeskDlg::EnableGasdoseManPos(bool i_enable)
{
 m_pGasdosePageDlg->EnableGasdoseManPos(i_enable);
}

void CParamDeskDlg::EnableCKPSItems(bool i_enable)
{
 m_pCKPSPageDlg->EnableCKPSItems(i_enable);
}

void CParamDeskDlg::EnableHallWndWidth(bool enable)
{
 m_pCKPSPageDlg->EnableHallWndWidth(enable);
}

void CParamDeskDlg::EnableBTNameAndPass(bool i_enable)
{
 m_pSecurPageDlg->EnableBTNameAndPass(i_enable);
}

void CParamDeskDlg::EnableUseResParCheck(bool i_enable)
{
 m_pSecurPageDlg->EnableUseResParCheck(i_enable);
}

void CParamDeskDlg::EnableInputsMerging(bool i_enable)
{
 m_pCKPSPageDlg->EnableInputsMerging(i_enable);
}

void CParamDeskDlg::EnableRisingSpark(bool i_enable)
{
 m_pCKPSPageDlg->EnableRisingSpark(i_enable);
}

void CParamDeskDlg::EnableUseCamRef(bool enable)
{
 m_pCKPSPageDlg->EnableUseCamRef(enable);
};

void CParamDeskDlg::EnableFuelInjection(bool i_enable)
{
 if (m_fuel_injection == i_enable)
  return; //already has needed state
 m_fuel_injection = i_enable;

 m_tab_control.EnableItem(m_fuel_inject_idx, i_enable && m_enabled);
 m_tab_control.EnableItem(m_accenr_tab_idx, (i_enable || m_gasdose) && m_enabled);
 m_pInjectorPageDlg->Enable(i_enable && m_enabled);
 m_pAccelEnrPageDlg->Enable((i_enable || m_gasdose) && m_enabled);
 m_pAccelEnrPageDlg->EnableFuelInjection(i_enable);
 m_pStarterPageDlg->EnableFuelInjection(i_enable);
 m_pCarburPageDlg->EnableFuelInjection(i_enable);
 m_pChokePageDlg->EnableFuelInjection(i_enable);
 m_pMiscPageDlg->EnableFuelInjection(i_enable);
 m_pIdlRegPageDlg->EnableFuelInjection(i_enable);
 m_pLambdaPageDlg->EnableFuelInjection(i_enable || m_gasdose);
 m_pTemperPageDlg->EnableFuelInjection(i_enable);
}

void CParamDeskDlg::EnableGasdose(bool i_enable)
{
 if (m_gasdose == i_enable)
  return; //already has needed state
 m_gasdose = i_enable;
 m_tab_control.EnableItem(m_gasdose_tab_idx, i_enable && m_enabled);
 m_tab_control.EnableItem(m_accenr_tab_idx, (m_gasdose || m_fuel_injection) && m_enabled);
 m_pGasdosePageDlg->Enable(i_enable && m_enabled);
 m_pAccelEnrPageDlg->Enable((m_fuel_injection || i_enable) && m_enabled);
 m_pCarburPageDlg->EnableGasdose(i_enable);
 m_pStarterPageDlg->EnableGasdose(i_enable);
 m_pLambdaPageDlg->EnableFuelInjection(i_enable || m_fuel_injection);
}

void CParamDeskDlg::EnableChokeCtrls(bool i_enable)
{
 if (m_choke_ctrls == i_enable)
  return; //already has needed state
 m_choke_ctrls = i_enable;

 m_pChokePageDlg->EnableFuelInjection(!m_choke_ctrls);
}

void CParamDeskDlg::EnableChoke(bool i_enable)
{
 if (m_choke == i_enable)
  return; //already has needed state
 m_choke = i_enable;
 m_tab_control.EnableItem(m_choke_tab_idx, i_enable && m_enabled);
 m_pChokePageDlg->Enable(i_enable && m_enabled);
 m_pStarterPageDlg->EnableChoke(i_enable);
}

void CParamDeskDlg::EnableLambda(bool i_enable)
{
 if (m_lambda == i_enable)
  return; //already has needed state
 m_lambda = i_enable;
 m_tab_control.EnableItem(m_lambda_tab_idx, i_enable && m_enabled);
 m_pLambdaPageDlg->Enable(i_enable && m_enabled);
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

bool CParamDeskDlg::OnSelchangingTabctl(void)
{
 return true; //allow changing of tab
}

void CParamDeskDlg::OnChangeInTab(void)
{
 if (m_OnChangeInTab)
  m_OnChangeInTab();
}

void CParamDeskDlg::OnSysCommand(UINT nID, LONG lParam)
{
 Super::OnSysCommand(nID, lParam);
 if (nID == SC_MINIMIZE)
  SetWindowPos(&wndTop, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
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
 RegisterHK(CHOKE_PAR,  VK_F11);
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
OnHK_XXX(CHOKE_PAR)

void CParamDeskDlg::OnSize( UINT nType, int cx, int cy )
{
 Super::OnSize(nType, cx, cy);

 //resize tab control
 if (::IsWindow(m_tab_control.m_hWnd))
 {
  CRect wndRectDlg, wndRectTab;
  GetClientRect(&wndRectDlg);
  m_tab_control.GetWindowRect(&wndRectTab);
  ScreenToClient(&wndRectTab);
  wndRectTab.right = wndRectDlg.right - m_tabLRMargin.cx;
  if (wndRectTab.right <= wndRectTab.left)
   wndRectTab.right = wndRectTab.left + 1; //prevent zero width
  wndRectTab.bottom = wndRectDlg.bottom - m_tabLRMargin.cy;
  m_tab_control.MoveWindow(wndRectTab);
 }
}

