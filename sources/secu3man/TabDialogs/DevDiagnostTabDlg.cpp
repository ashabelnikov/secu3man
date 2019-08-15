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

/** \file DevDiagnostTabDlg.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "Resources/resource.h"
#include "DevDiagnostTabDlg.h"

#include "about/secu-3about.h"
#include "common/FastDelegate.h"
#include "common/GDIHelpers.h"
#include "common/DPIAware.h"
#include "DiagnostContextMenuManager.h"
#include "ui-core/ddx_helpers.h"
#include "ui-core/OScopeCtrl.h"
#include "DevDiagInpsDlg.h"
#include "DevDiagOutsDlg.h"

#define TIMER_ID 0

using namespace fastdelegate;

const UINT CDevDiagnostTabDlg::IDD = IDD_DEV_DIAGNOSTICS;

BEGIN_MESSAGE_MAP(CDevDiagnostTabDlg, Super)
 ON_WM_DESTROY()
 ON_WM_TIMER()
 ON_WM_CONTEXTMENU()
 ON_WM_INITMENUPOPUP()
 ON_WM_SIZE()

 ON_COMMAND(IDM_DEV_DIAG_START_OUTAUTO_TST, OnStartOutputsAutoTesting)
 ON_COMMAND(IDM_DEV_DIAG_STOP_OUTAUTO_TST, OnStopOutputsAutoTesting)
 ON_COMMAND(IDM_DEV_DIAG_ENABLE_BLDE_TST, OnEnableBLDETesting)
 ON_COMMAND(IDM_DEV_DIAG_ENABLE_TACH_O_TST, OnEnableTACHOTesting)
 ON_BN_CLICKED(IDC_DEV_DIAG_ENTER_CHECK, OnEnterButton)

 ON_UPDATE_COMMAND_UI(IDC_DEV_DIAG_OUTPUTS_GROUP, OnUpdateDiagControls)
 ON_UPDATE_COMMAND_UI(IDC_DEV_DIAG_INPUTS_GROUP, OnUpdateDiagControls)
 ON_UPDATE_COMMAND_UI(IDM_DEV_DIAG_START_OUTAUTO_TST, OnUpdateDiagControls)
 ON_UPDATE_COMMAND_UI(IDM_DEV_DIAG_STOP_OUTAUTO_TST, OnUpdateDiagControls)
 ON_UPDATE_COMMAND_UI(IDM_DEV_DIAG_ENABLE_BLDE_TST, OnUpdateDiagControls)
 ON_UPDATE_COMMAND_UI(IDM_DEV_DIAG_ENABLE_TACH_O_TST, OnUpdateDiagControls)
 ON_UPDATE_COMMAND_UI(IDC_DEV_DIAG_ENTER_CHECK, OnUpdateEnterButton)
 ON_UPDATE_COMMAND_UI(IDC_DEV_DIAG_WARNING_TEXT, OnUpdateEnterButton)

 ON_UPDATE_COMMAND_UI(IDC_DEV_DIAG_KS_1_CAPTION, OnUpdateDiagControls)
 ON_UPDATE_COMMAND_UI(IDC_DEV_DIAG_KS_2_CAPTION, OnUpdateDiagControls) 
END_MESSAGE_MAP()

CDevDiagnostTabDlg::CDevDiagnostTabDlg(CWnd* pParent /*=NULL*/)
: Super(CDevDiagnostTabDlg::IDD, pParent)
, mp_OScopeCtrl1(new COScopeCtrl())
, mp_OScopeCtrl2(new COScopeCtrl())
, mp_ContextMenuManager(new CDiagnostContextMenuManager())
, m_enable_diag_controls(false)
, m_enable_enter_button(false)
, m_enable_blde_testing(false)
, m_enable_tacho_testing(false)
, mp_inpsDlg(new CDevDiagInpsDlg(NULL))
, mp_outsDlg(new CDevDiagOutsDlg(NULL))
, m_start_autotst_enabled(false)
, m_stop_autotst_enabled(false)
, m_initialized(false)
{
 mp_ContextMenuManager->CreateContent();

 mp_outsDlg->setOnOutputToggle(MakeDelegate(this, &CDevDiagnostTabDlg::OnOutputCheckToggle));

 //=================================================================
 if (!CheckBitmaps() || !CheckAbout())
  delete this;
 //=================================================================
}

void CDevDiagnostTabDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);
 DDX_Control(pDX, IDC_DEV_DIAG_ENTER_CHECK, m_enter_button);
 DDX_Control(pDX, IDC_DEV_DIAG_OUTPUTS_GROUP, m_outputs_group);
 DDX_Control(pDX, IDC_DEV_DIAG_WARNING_TEXT, m_warning_text);
 DDX_Control(pDX, IDC_DEV_DIAG_INPUTS_GROUP, m_inputs_group);
}

LPCTSTR CDevDiagnostTabDlg::GetDialogID(void) const
{
 return (LPCTSTR)IDD;
}

BOOL CDevDiagnostTabDlg::OnInitDialog()
{
 Super::OnInitDialog();

 //create outputs child dialog
 CRect rect;
 GetDlgItem(IDC_DEV_DIAG_OUTPUTS_FRAME)->GetWindowRect(rect);
 ScreenToClient(rect);
 mp_outsDlg->Create(CDevDiagOutsDlg::IDD,this);
 mp_outsDlg->SetPosition(rect.TopLeft().x,rect.TopLeft().y);
 mp_outsDlg->ShowWindow(SW_SHOW);
 //create inputs child dialog
 GetDlgItem(IDC_DEV_DIAG_INPUTS_FRAME)->GetWindowRect(rect);
 ScreenToClient(rect);
 mp_inpsDlg->Create(CDevDiagInpsDlg::IDD,this);
 mp_inpsDlg->SetPosition(rect.TopLeft().x,rect.TopLeft().y);
 mp_inpsDlg->ShowWindow(SW_SHOW);

 SetTimer(TIMER_ID, 250, NULL);

 //initialize oscilloscopes
 _InitializeOscilloscopeControls();

 mp_ContextMenuManager->Attach(this);

 UpdateDialogControls(this,TRUE);

 //=================================================================
 if (!CheckAppTitle(AfxGetMainWnd()) || !CheckAppLogo())
 { GetParent()->DestroyWindow(); }
 //=================================================================

 m_initialized = true;
 return TRUE;
}

void CDevDiagnostTabDlg::OnUpdateDiagControls(CCmdUI* pCmdUI)
{
 switch(pCmdUI->m_nID)
 {
  case IDM_DEV_DIAG_ENABLE_BLDE_TST:
   pCmdUI->SetCheck(m_enable_blde_testing);  
   pCmdUI->Enable(m_enable_diag_controls);  
   break;

  case IDM_DEV_DIAG_ENABLE_TACH_O_TST:
   pCmdUI->SetCheck(m_enable_tacho_testing);  
   pCmdUI->Enable(m_enable_diag_controls);  
   break;

  case IDM_DEV_DIAG_START_OUTAUTO_TST:
   pCmdUI->Enable(m_enable_diag_controls && m_start_autotst_enabled);  
   break;

  case IDM_DEV_DIAG_STOP_OUTAUTO_TST:
   pCmdUI->Enable(m_enable_diag_controls && m_stop_autotst_enabled);  
   break;

  default:
   pCmdUI->Enable(m_enable_diag_controls);
 };
}

void CDevDiagnostTabDlg::OnUpdateEnterButton(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enable_enter_button);
}

void CDevDiagnostTabDlg::OnTimer(UINT nIDEvent)
{
 //dirty hack
 UpdateDialogControls(this,TRUE);
 Super::OnTimer(nIDEvent);
}

void CDevDiagnostTabDlg::OnDestroy()
{
 Super::OnDestroy();
 KillTimer(TIMER_ID);
 m_initialized = false;
}

void CDevDiagnostTabDlg::OnContextMenu(CWnd* pWnd, CPoint point)
{
 mp_ContextMenuManager->TrackPopupMenu(point.x, point.y);
}

void CDevDiagnostTabDlg::OnInitMenuPopup(CMenu* pMenu, UINT nIndex, BOOL bSysMenu)
{
 mp_ContextMenuManager->OnInitMenuPopup(pMenu, nIndex, bSysMenu);
}

void CDevDiagnostTabDlg::OnOutputCheckToggle(int output_id, bool state)
{
 if (m_on_output_check)
  m_on_output_check(output_id, state);
}

void CDevDiagnostTabDlg::OnEnterButton()
{
 if (m_on_enter_button)
  m_on_enter_button();
}

void CDevDiagnostTabDlg::OnStartOutputsAutoTesting()
{
 if (m_on_start_outauto_tst)
  m_on_start_outauto_tst();
}

void CDevDiagnostTabDlg::OnStopOutputsAutoTesting()
{
 if (m_on_stop_outauto_tst)
  m_on_stop_outauto_tst();
}

void CDevDiagnostTabDlg::OnEnableBLDETesting()
{
 UINT state = mp_ContextMenuManager->GetParentMenu().GetMenuState(IDM_DEV_DIAG_ENABLE_BLDE_TST, MF_BYCOMMAND);
 ASSERT(state != 0xFFFFFFFF);
 m_enable_blde_testing = (state & MF_CHECKED) ? false : true; //toggle
 if (m_on_enable_blde_tst)
  m_on_enable_blde_tst(m_enable_blde_testing);
 mp_outsDlg->EnableBLDETesting(m_enable_blde_testing);
}

void CDevDiagnostTabDlg::OnEnableTACHOTesting()
{
 UINT state = mp_ContextMenuManager->GetParentMenu().GetMenuState(IDM_DEV_DIAG_ENABLE_TACH_O_TST, MF_BYCOMMAND);
 ASSERT(state != 0xFFFFFFFF);
 m_enable_tacho_testing = (state & MF_CHECKED) ? false : true; //toggle
 if (m_on_enable_tacho_tst)
  m_on_enable_tacho_tst(m_enable_tacho_testing);
 mp_outsDlg->EnableTACHOTesting(m_enable_tacho_testing);
}

void CDevDiagnostTabDlg::EnableDiagControls(bool i_enable)
{
 m_enable_diag_controls = i_enable;
 mp_OScopeCtrl1->EnableWindow(i_enable);
 mp_OScopeCtrl2->EnableWindow(i_enable);
 mp_inpsDlg->EnableDiagControls(i_enable);
 mp_outsDlg->EnableDiagControls(i_enable);
 UpdateDialogControls(this, TRUE);
}

void CDevDiagnostTabDlg::EnableEnterButton(bool i_enable)
{
 m_enable_enter_button = i_enable;
 UpdateDialogControls(this,TRUE);
}

void CDevDiagnostTabDlg::EnableSECU3TFeatures(bool i_enable)
{
 mp_inpsDlg->EnableSECU3TFeatures(i_enable);
 mp_outsDlg->EnableSECU3TFeatures(i_enable);
 UpdateDialogControls(this,TRUE);
}

void CDevDiagnostTabDlg::EnableExtraIO(bool i_enable)
{
 mp_inpsDlg->EnableExtraIO(i_enable);
 UpdateDialogControls(this,TRUE);
}

void CDevDiagnostTabDlg::EnableBLDETesting(bool i_enable)
{
 m_enable_blde_testing = i_enable;
 UpdateDialogControls(this,TRUE);
}

void CDevDiagnostTabDlg::EnableTACHOTesting(bool i_enable)
{
 m_enable_tacho_testing = i_enable;
 UpdateDialogControls(this,TRUE);
}

bool CDevDiagnostTabDlg::IsBLDETestingEnabled(void)
{
 return m_enable_blde_testing;
}

bool CDevDiagnostTabDlg::IsTACHOTestingEnabled(void)
{
 return m_enable_tacho_testing;
}

void CDevDiagnostTabDlg::setOnOutputToggle(EventOutputToggle OnFunction)
{
 m_on_output_check = OnFunction;
}

void CDevDiagnostTabDlg::setOnEnterButton(EventHandler OnFunction)
{
 m_on_enter_button = OnFunction;
}

void CDevDiagnostTabDlg::setOnStartOutAutoTesting(EventHandler OnFunction)
{
 m_on_start_outauto_tst = OnFunction;
}

void CDevDiagnostTabDlg::setOnStopOutAutoTesting(EventHandler OnFunction)
{
 m_on_stop_outauto_tst = OnFunction;
}

void CDevDiagnostTabDlg::setOnEnableBLDETesting(EventFlag OnFunction)
{
 m_on_enable_blde_tst = OnFunction;
}

void CDevDiagnostTabDlg::setOnEnableTACHOTesting(EventFlag OnFunction)
{
 m_on_enable_tacho_tst = OnFunction;
}

void CDevDiagnostTabDlg::SetInputValues(const SECU3IO::DiagInpDat* i_values)
{
 ASSERT(i_values);
 mp_inpsDlg->SetInputValues(i_values);
 mp_OScopeCtrl1->AppendPoint(i_values->ks_1);
 mp_OScopeCtrl2->AppendPoint(i_values->ks_2);
}

void CDevDiagnostTabDlg::SetEnterButtonCaption(const _TSTRING& i_text)
{
 m_enter_button.SetWindowText(i_text.c_str());
}

void CDevDiagnostTabDlg::SetEnterButton(bool i_state)
{
 m_enter_button.SetCheck(i_state ? BST_CHECKED : BST_UNCHECKED);
}

void CDevDiagnostTabDlg::SetOutputValue(int id, bool state)
{
 mp_outsDlg->SetOutputValue(id, state);
}

//инициализация осциллографа для сигналов с ДД
void CDevDiagnostTabDlg::_InitializeOscilloscopeControls(void)
{
 CRect rect;

 GetDlgItem(IDC_DEV_DIAG_OSCILLOSCOPE_KS1_HOLDER)->GetWindowRect(rect);
 ScreenToClient(rect);

 // create the control
 mp_OScopeCtrl1->Create(WS_VISIBLE | WS_CHILD | WS_DISABLED, rect, this);

 // customize the control
 mp_OScopeCtrl1->SetRange(0.0, 5.0, 1);
 mp_OScopeCtrl1->SetYUnits(MLL::LoadString(IDS_KC_OSCILLOSCOPE_V_UNIT));
 mp_OScopeCtrl1->SetXUnits(MLL::LoadString(IDS_KC_OSCILLOSCOPE_H_UNIT));
 mp_OScopeCtrl1->SetBackgroundColor(RGB(0, 64, 0));
 mp_OScopeCtrl1->SetGridColor(RGB(192, 192, 255));
 mp_OScopeCtrl1->SetPlotColor(RGB(255, 255, 255));

 GetDlgItem(IDC_DEV_DIAG_OSCILLOSCOPE_KS2_HOLDER)->GetWindowRect(rect);
 ScreenToClient(rect);

 // create the control
 mp_OScopeCtrl2->Create(WS_VISIBLE | WS_CHILD | WS_DISABLED, rect, this);

 // customize the control
 mp_OScopeCtrl2->SetRange(0.0, 5.0, 1);
 mp_OScopeCtrl2->SetYUnits(MLL::LoadString(IDS_KC_OSCILLOSCOPE_V_UNIT));
 mp_OScopeCtrl2->SetXUnits(MLL::LoadString(IDS_KC_OSCILLOSCOPE_H_UNIT));
 mp_OScopeCtrl2->SetBackgroundColor(RGB(0, 64, 0));
 mp_OScopeCtrl2->SetGridColor(RGB(192, 192, 255));
 mp_OScopeCtrl2->SetPlotColor(RGB(255, 255, 255));
}

void CDevDiagnostTabDlg::EnableStartAutoTstMenuItem(bool i_enable)
{
 m_start_autotst_enabled = i_enable;
}

void CDevDiagnostTabDlg::EnableStopAutoTstMenuItem(bool i_enable)
{
 m_stop_autotst_enabled = i_enable;
}

void CDevDiagnostTabDlg::OnSize( UINT nType, int cx, int cy )
{
 if (m_initialized)
 {
  CRect rc1, rc2, wt_rc;

  DPIAware da;
  rc1 = GDIHelpers::GetChildWndRect(&m_enter_button);
  m_enter_button.MoveWindow(rc1.left, cy - rc1.Height() - da.ScaleY(10), rc1.Width(), rc1.Height());

  rc1 = GDIHelpers::GetChildWndRect(&m_warning_text);
  m_warning_text.MoveWindow(rc1.left, cy - rc1.Height()- da.ScaleY(2), rc1.Width(), rc1.Height());

  GetClientRect(&rc2);
  
  rc1 = GDIHelpers::GetChildWndRect(&m_outputs_group);
  m_outputs_group.SetWindowPos(NULL, 0, 0, rc1.Width(), rc2.bottom - rc1.top - da.ScaleY(3), SWP_NOMOVE | SWP_NOZORDER);

  wt_rc = GDIHelpers::GetChildWndRect(&m_warning_text);

  rc1 = GDIHelpers::GetChildWndRect(&m_inputs_group);
  m_inputs_group.SetWindowPos(NULL, 0, 0, cx - rc1.left - da.ScaleX(3), wt_rc.top - rc1.top, SWP_NOMOVE | SWP_NOZORDER);

  //outputs dialog
  rc1 = GDIHelpers::GetChildWndRect(mp_outsDlg.get());
  rc2 = GDIHelpers::GetChildWndRect(&m_outputs_group);
  mp_outsDlg->SetWindowPos(NULL, 0, 0, rc1.Width(), rc2.bottom - rc1.top - da.ScaleY(8), SWP_NOMOVE | SWP_NOZORDER);

  //inputs dialog
  rc1 = GDIHelpers::GetChildWndRect(mp_inpsDlg.get());
  rc2 = GDIHelpers::GetChildWndRect(&m_inputs_group);
  mp_inpsDlg->SetWindowPos(NULL, 0, 0, rc1.Width(), rc2.bottom - rc1.top - da.ScaleY(8), SWP_NOMOVE | SWP_NOZORDER);

  //oscill1
  rc1 = GDIHelpers::GetChildWndRect(mp_OScopeCtrl1.get());
  rc2 = GDIHelpers::GetChildWndRect(&m_inputs_group);
  mp_OScopeCtrl1->SetWindowPos(NULL, 0, 0, rc2.right - rc1.left - da.ScaleY(8), rc1.Height(), SWP_NOMOVE | SWP_NOZORDER);

  //oscill2
  rc1 = GDIHelpers::GetChildWndRect(mp_OScopeCtrl2.get());
  rc2 = GDIHelpers::GetChildWndRect(&m_inputs_group);
  mp_OScopeCtrl2->SetWindowPos(NULL, 0, 0, rc2.right - rc1.left - da.ScaleY(8), rc1.Height(), SWP_NOMOVE | SWP_NOZORDER);
 }

 Super::OnSize(nType, cx, cy);
}
