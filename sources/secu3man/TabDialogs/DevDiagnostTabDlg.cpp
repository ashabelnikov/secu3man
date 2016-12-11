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

#include "common/FastDelegate.h"
#include "DiagnostContextMenuManager.h"
#include "ui-core/ddx_helpers.h"
#include "ui-core/fnt_helpers.h"
#include "ui-core/OScopeCtrl.h"

#define TIMER_ID 0

using namespace fastdelegate;

const UINT CDevDiagnostTabDlg::IDD = IDD_DEV_DIAGNOSTICS;

const UINT OutputsCheckStart = IDC_DEV_DIAG_IGN_OUT1_CHECK;
const UINT OutputsCheckEnd = IDC_DEV_DIAG_DE_CHECK;
const UINT InputsTextStart = IDC_DEV_DIAG_VOLTAGE;
const UINT InputsTextEnd = IDC_DEV_DIAG_DE;
const UINT InputsCaptionStart = IDC_DEV_DIAG_VOLTAGE_CAPTION;
const UINT InputsCaptionEnd = IDC_DEV_DIAG_DE_CAPTION;

BEGIN_MESSAGE_MAP(CDevDiagnostTabDlg, Super)
 ON_WM_DESTROY()
 ON_WM_TIMER()
 ON_WM_CONTEXTMENU()
 ON_WM_INITMENUPOPUP()
 ON_COMMAND_RANGE(OutputsCheckStart, OutputsCheckEnd, OnOutputCheckToggle)
 ON_COMMAND(IDM_DEV_DIAG_START_OUTAUTO_TST, OnStartOutputsAutoTesting)
 ON_COMMAND(IDM_DEV_DIAG_STOP_OUTAUTO_TST, OnStopOutputsAutoTesting)
 ON_COMMAND(IDM_DEV_DIAG_ENABLE_BLDE_TST, OnEnableBLDETesting)
 ON_BN_CLICKED(IDC_DEV_DIAG_ENTER_CHECK, OnEnterButton)

 ON_UPDATE_COMMAND_UI_RANGE(OutputsCheckStart, OutputsCheckEnd, OnUpdateDiagControls)
 ON_UPDATE_COMMAND_UI_RANGE(InputsTextStart, InputsTextEnd, OnUpdateDiagControls)
 ON_UPDATE_COMMAND_UI_RANGE(InputsCaptionStart, InputsCaptionEnd, OnUpdateDiagControls)
 ON_UPDATE_COMMAND_UI(IDC_DEV_DIAG_OUTPUTS_GROUP, OnUpdateDiagControls)
 ON_UPDATE_COMMAND_UI(IDC_DEV_DIAG_INPUTS_GROUP, OnUpdateDiagControls)
 ON_UPDATE_COMMAND_UI(IDM_DEV_DIAG_START_OUTAUTO_TST, OnUpdateDiagControls)
 ON_UPDATE_COMMAND_UI(IDM_DEV_DIAG_STOP_OUTAUTO_TST, OnUpdateDiagControls)
 ON_UPDATE_COMMAND_UI(IDM_DEV_DIAG_ENABLE_BLDE_TST, OnUpdateDiagControls)
 ON_UPDATE_COMMAND_UI(IDC_DEV_DIAG_ENTER_CHECK, OnUpdateEnterButton)
 ON_UPDATE_COMMAND_UI(IDC_DEV_DIAG_WARNING_TEXT, OnUpdateEnterButton)
END_MESSAGE_MAP()

CDevDiagnostTabDlg::CDevDiagnostTabDlg(CWnd* pParent /*=NULL*/)
: Super(CDevDiagnostTabDlg::IDD, pParent)
, mp_OScopeCtrl1(new COScopeCtrl())
, mp_OScopeCtrl2(new COScopeCtrl())
, mp_ContextMenuManager(new CDiagnostContextMenuManager())
, m_enable_diag_controls(false)
, m_enable_enter_button(false)
, m_enable_secu3t_features(false)
, m_enable_blde_testing(false)
{
 mp_ContextMenuManager->CreateContent();

 memset(&m_inputValues, 0, sizeof(SECU3IO::DiagInpDat));
}

void CDevDiagnostTabDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);
 DDX_Control(pDX, IDC_DEV_DIAG_ENTER_CHECK, m_enter_button);
 DDX_Text_Fmt(pDX, IDC_DEV_DIAG_VOLTAGE, m_inputValues.voltage, _T("%.3f"));
 DDX_Text_Fmt(pDX, IDC_DEV_DIAG_MAP_S, m_inputValues.map, _T("%.3f"));
 DDX_Text_Fmt(pDX, IDC_DEV_DIAG_TEMP, m_inputValues.temp, _T("%.3f"));
 DDX_Text_Fmt(pDX, IDC_DEV_DIAG_ADD_I1, m_inputValues.add_io1, _T("%.3f"));
 DDX_Text_Fmt(pDX, IDC_DEV_DIAG_ADD_I2, m_inputValues.add_io2, _T("%.3f"));
 if (m_enable_secu3t_features)
  DDX_Text_Fmt(pDX, IDC_DEV_DIAG_CARB, m_inputValues.carb,_T("%.3f"));
 else
 {
  int value = m_inputValues.carb > 0;
  DDX_Text_Fmt(pDX, IDC_DEV_DIAG_CARB, value, _T("%d"));
 }
 DDX_Text_Fmt(pDX, IDC_DEV_DIAG_GAS_V, m_inputValues.gas, _T("%d"));
 DDX_Text_Fmt(pDX, IDC_DEV_DIAG_CKPS, m_inputValues.ckps, _T("%d"));
 DDX_Text_Fmt(pDX, IDC_DEV_DIAG_REF_S, m_inputValues.ref_s, _T("%d"));
 DDX_Text_Fmt(pDX, IDC_DEV_DIAG_PS, m_inputValues.ps, _T("%d"));
 DDX_Text_Fmt(pDX, IDC_DEV_DIAG_BL, m_inputValues.bl, _T("%d"));
 DDX_Text_Fmt(pDX, IDC_DEV_DIAG_DE, m_inputValues.de, _T("%d"));
}

LPCTSTR CDevDiagnostTabDlg::GetDialogID(void) const
{
 return (LPCTSTR)IDD;
}

BOOL CDevDiagnostTabDlg::OnInitDialog()
{
 Super::OnInitDialog();

 //Change size of fonts (this will improve user experience)
 CloneWndFont(GetDlgItem(InputsTextStart), &m_textFont, 11, true);
 for(size_t i = InputsTextStart; i <= InputsTextEnd; ++i)
  GetDlgItem(i)->SetFont(&m_textFont);
  
 SetTimer(TIMER_ID, 250, NULL);

 //инициализируем осциллографы
 _InitializeOscilloscopeControls();

 mp_ContextMenuManager->Attach(this);

 UpdateDialogControls(this,TRUE);
 return TRUE;
}

void CDevDiagnostTabDlg::OnUpdateDiagControls(CCmdUI* pCmdUI)
{
 switch(pCmdUI->m_nID)
 {
  case IDC_DEV_DIAG_ADD_O1_CHECK:
  case IDC_DEV_DIAG_ADD_O2_CHECK:
  case IDC_DEV_DIAG_ADD_I1:
  case IDC_DEV_DIAG_ADD_I2:
  case IDC_DEV_DIAG_ADD_I1_CAPTION:
  case IDC_DEV_DIAG_ADD_I2_CAPTION:
  case IDC_DEV_DIAG_REF_S:
  case IDC_DEV_DIAG_REF_S_CAPTION:
  case IDC_DEV_DIAG_KS_2_CAPTION:
   pCmdUI->Enable(m_enable_diag_controls && m_enable_secu3t_features);
   break;
  case IDM_DEV_DIAG_ENABLE_BLDE_TST:
   pCmdUI->SetCheck(m_enable_blde_testing);  
   pCmdUI->Enable(m_enable_diag_controls);  
   break;
  case IDC_DEV_DIAG_BL_CHECK:
  case IDC_DEV_DIAG_DE_CHECK:
   pCmdUI->Enable(m_enable_diag_controls && m_enable_blde_testing);  
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
}

void CDevDiagnostTabDlg::OnContextMenu(CWnd* pWnd, CPoint point)
{
 mp_ContextMenuManager->TrackPopupMenu(point.x, point.y);
}

void CDevDiagnostTabDlg::OnInitMenuPopup(CMenu* pMenu, UINT nIndex, BOOL bSysMenu)
{
 mp_ContextMenuManager->OnInitMenuPopup(pMenu, nIndex, bSysMenu);
}

void CDevDiagnostTabDlg::OnOutputCheckToggle(UINT nID)
{
 CButton* p_check = static_cast<CButton*>(GetDlgItem(nID));
 if (!p_check)
  return;

 bool state = p_check->GetCheck() == BST_CHECKED;
 if (m_on_output_check)
  m_on_output_check(nID-OutputsCheckStart, state);
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
}

void CDevDiagnostTabDlg::EnableDiagControls(bool i_enable)
{
 m_enable_diag_controls = i_enable;
 mp_OScopeCtrl1->EnableWindow(i_enable);
 mp_OScopeCtrl2->EnableWindow(i_enable && m_enable_secu3t_features); //SECU-3T
 UpdateDialogControls(this, TRUE);
}

void CDevDiagnostTabDlg::EnableEnterButton(bool i_enable)
{
 m_enable_enter_button = i_enable;
 UpdateDialogControls(this,TRUE);
}

void CDevDiagnostTabDlg::EnableSECU3TFeatures(bool i_enable)
{
 m_enable_secu3t_features = i_enable;
 mp_OScopeCtrl2->EnableWindow(i_enable && m_enable_diag_controls); //SECU-3T
 UpdateDialogControls(this,TRUE);
}

void CDevDiagnostTabDlg::EnableBLDETesting(bool i_enable)
{
 m_enable_blde_testing = i_enable;
 UpdateDialogControls(this,TRUE);
}

bool CDevDiagnostTabDlg::IsBLDETestingEnabled(void)
{
 return m_enable_blde_testing;
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

void CDevDiagnostTabDlg::SetInputValues(const SECU3IO::DiagInpDat* i_values)
{
 ASSERT(i_values);
 memcpy(&m_inputValues, i_values, sizeof(SECU3IO::DiagInpDat));
 UpdateData(FALSE);
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
 CButton *pButton = (CButton*)GetDlgItem(OutputsCheckStart + id);
 pButton->SetCheck(state ? BST_CHECKED : BST_UNCHECKED);
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
