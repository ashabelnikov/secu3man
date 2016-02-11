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

/** \file GasdosePageDlg.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "Resources/resource.h"
#include "GasdosePageDlg.h"
#include "ui-core/ddx_helpers.h"
#include "ui-core/ToolTipCtrlEx.h"

const UINT CGasdosePageDlg::IDD = IDD_PD_GASDOSE_PAGE;

BEGIN_MESSAGE_MAP(CGasdosePageDlg, Super)
 ON_EN_CHANGE(IDC_PD_GASDOSE_SM_STEPS_NUM_EDIT, OnChangePdSMStepsNumEdit)
 ON_EN_CHANGE(IDC_PD_GASDOSE_FC_CLOSING_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_GASDOSE_CORRLIMIT_P_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_GASDOSE_CORRLIMIT_M_EDIT, OnChangeData)
 ON_BN_CLICKED(IDC_PD_GASDOSE_SM_TEST_CHECK, OnSMTestButton)

 ON_UPDATE_COMMAND_UI(IDC_PD_GASDOSE_SM_STEPS_NUM_SPIN,OnUpdateGasdoseSMSteps)
 ON_UPDATE_COMMAND_UI(IDC_PD_GASDOSE_SM_STEPS_NUM_CAPTION,OnUpdateGasdoseSMSteps)
 ON_UPDATE_COMMAND_UI(IDC_PD_GASDOSE_SM_STEPS_NUM_EDIT,OnUpdateGasdoseSMSteps)
 ON_UPDATE_COMMAND_UI(IDC_PD_GASDOSE_SM_TEST_CHECK,OnUpdateGasdoseTestBtn)
 ON_UPDATE_COMMAND_UI(IDC_PD_GASDOSE_MANUAL_POS_SPIN,OnUpdateGasdoseManPosBtn)
 ON_UPDATE_COMMAND_UI(IDC_PD_GASDOSE_MANUAL_POS_CAPTION,OnUpdateGasdoseManPosBtn)
 ON_NOTIFY(UDN_DELTAPOS, IDC_PD_GASDOSE_MANUAL_POS_SPIN, OnManualDeltapos)

 ON_UPDATE_COMMAND_UI(IDC_PD_GASDOSE_FC_CLOSING_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_GASDOSE_FC_CLOSING_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_GASDOSE_FC_CLOSING_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_GASDOSE_FC_CLOSING_UNIT,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_GASDOSE_CORRLIMIT_P_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_GASDOSE_CORRLIMIT_P_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_GASDOSE_CORRLIMIT_P_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_GASDOSE_CORRLIMIT_P_UNIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_GASDOSE_CORRLIMIT_M_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_GASDOSE_CORRLIMIT_M_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_GASDOSE_CORRLIMIT_M_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_GASDOSE_CORRLIMIT_M_UNIT,OnUpdateControls)
END_MESSAGE_MAP()

CGasdosePageDlg::CGasdosePageDlg(CWnd* pParent /*=NULL*/)
: Super(CGasdosePageDlg::IDD, pParent)
, m_enabled(false)
, m_sm_steps_num_edit(CEditEx::MODE_INT, true)
, m_fc_closing_edit(CEditEx::MODE_FLOAT, true)
, m_corrlimit_p_edit(CEditEx::MODE_FLOAT, true)
, m_corrlimit_m_edit(CEditEx::MODE_FLOAT, true)
, m_gasdose_tst_enabled(false)
, m_gasdose_manpos_enabled(false)
, m_lock_ui_update(false) //UI update is not locked
{
 m_params.gd_steps = 700;
 m_params.testing = 0;
 m_params.manual_delta = 0;
 m_params.fc_closing = 30.0f;
 m_params.lam_corr_limit_p = 30.0f;
 m_params.lam_corr_limit_m = 30.0f;
}

LPCTSTR CGasdosePageDlg::GetDialogID(void) const
{
 return (LPCTSTR)IDD;
}

void CGasdosePageDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);
 DDX_Control(pDX, IDC_PD_GASDOSE_SM_STEPS_NUM_SPIN, m_sm_steps_num_spin);
 DDX_Control(pDX, IDC_PD_GASDOSE_SM_STEPS_NUM_EDIT, m_sm_steps_num_edit);
 DDX_Control(pDX, IDC_PD_GASDOSE_SM_TEST_CHECK, m_sm_test_check);
 DDX_Control(pDX, IDC_PD_GASDOSE_MANUAL_POS_SPIN, m_man_ctrl_spin);
 DDX_Control(pDX, IDC_PD_GASDOSE_FC_CLOSING_SPIN, m_fc_closing_spin);
 DDX_Control(pDX, IDC_PD_GASDOSE_FC_CLOSING_EDIT, m_fc_closing_edit);
 DDX_Control(pDX,IDC_PD_GASDOSE_CORRLIMIT_P_EDIT, m_corrlimit_p_edit);
 DDX_Control(pDX,IDC_PD_GASDOSE_CORRLIMIT_P_SPIN, m_corrlimit_p_spin);
 DDX_Control(pDX,IDC_PD_GASDOSE_CORRLIMIT_M_EDIT, m_corrlimit_m_edit);
 DDX_Control(pDX,IDC_PD_GASDOSE_CORRLIMIT_M_SPIN, m_corrlimit_m_spin);

 m_sm_steps_num_edit.DDX_Value(pDX, IDC_PD_GASDOSE_SM_STEPS_NUM_EDIT, m_params.gd_steps);
 DDX_Check_UCHAR(pDX, IDC_PD_GASDOSE_SM_TEST_CHECK, m_params.testing);
 m_fc_closing_edit.DDX_Value(pDX, IDC_PD_GASDOSE_FC_CLOSING_EDIT, m_params.fc_closing);
 m_corrlimit_p_edit.DDX_Value(pDX, IDC_PD_GASDOSE_CORRLIMIT_P_EDIT, m_params.lam_corr_limit_p);
 m_corrlimit_m_edit.DDX_Value(pDX, IDC_PD_GASDOSE_CORRLIMIT_M_EDIT, m_params.lam_corr_limit_m);
}

void CGasdosePageDlg::OnUpdateControls(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled);
}

void CGasdosePageDlg::OnUpdateGasdoseTestBtn(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled && m_gasdose_tst_enabled);
}

void CGasdosePageDlg::OnUpdateGasdoseManPosBtn(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled && m_gasdose_manpos_enabled);
}

void CGasdosePageDlg::OnUpdateGasdoseSMSteps(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled);
}

/////////////////////////////////////////////////////////////////////////////
// CGasdosePageDlg message handlers

BOOL CGasdosePageDlg::OnInitDialog()
{
 Super::OnInitDialog();

 m_sm_steps_num_edit.SetLimitText(4);
 m_sm_steps_num_spin.SetBuddy(&m_sm_steps_num_edit);
 m_sm_steps_num_spin.SetRangeAndDelta(10, 2000, 1);
 m_sm_steps_num_edit.SetRange(10, 2000);

 m_fc_closing_edit.SetLimitText(5);
 m_fc_closing_spin.SetBuddy(&m_fc_closing_edit);
 m_fc_closing_edit.SetDecimalPlaces(1);
 m_fc_closing_spin.SetRangeAndDelta(0.0f, 100.0f, 0.5f);
 m_fc_closing_edit.SetRange(0.0f, 100.0f);

 m_corrlimit_p_spin.SetBuddy(&m_corrlimit_p_edit);
 m_corrlimit_p_edit.SetLimitText(6);
 m_corrlimit_p_edit.SetDecimalPlaces(2);
 m_corrlimit_p_spin.SetRangeAndDelta(0.50f, 100.00f, 0.5f);
 m_corrlimit_p_edit.SetRange(0.50f, 100.00f);

 m_corrlimit_m_spin.SetBuddy(&m_corrlimit_m_edit);
 m_corrlimit_m_edit.SetLimitText(6);
 m_corrlimit_m_edit.SetDecimalPlaces(2);
 m_corrlimit_m_spin.SetRangeAndDelta(0.50f, 100.00f, 0.5f);
 m_corrlimit_m_edit.SetRange(0.50f, 100.00f);

 m_man_ctrl_spin.SetBuddy(&m_man_ctrl_spin); //loves himself

 //create a tooltip control and assign tooltips
 mp_ttc.reset(new CToolTipCtrlEx());
 VERIFY(mp_ttc->Create(this, WS_POPUP | TTS_ALWAYSTIP | TTS_BALLOON));
 VERIFY(mp_ttc->AddWindow(&m_sm_steps_num_edit, MLL::GetString(IDS_PD_CHOKE_SM_STEPS_NUM_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_sm_steps_num_spin, MLL::GetString(IDS_PD_CHOKE_SM_STEPS_NUM_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_sm_test_check, MLL::GetString(IDS_PD_CHOKE_SM_TEST_CHECK_TT)));
 VERIFY(mp_ttc->AddWindow(&m_man_ctrl_spin, MLL::GetString(IDS_PD_CHOKE_MANUAL_POS_SPIN_TT)));

 mp_ttc->SetMaxTipWidth(100); //Enable text wrapping
 mp_ttc->ActivateToolTips(true);

 UpdateData(FALSE);
 UpdateDialogControls(this, TRUE);
 return TRUE;  // return TRUE unless you set the focus to a control
}

void CGasdosePageDlg::OnChangePdSMStepsNumEdit()
{
 UpdateData();
 OnChangeNotify(); //notify event receiver about change of view content(see class ParamPageEvents)
}

void CGasdosePageDlg::OnChangeData()
{
 UpdateData();
 OnChangeNotify();
}

void CGasdosePageDlg::OnSMTestButton()
{
 UpdateData();
 OnChangeNotify(); //notify event receiver about change of view content(see class ParamPageEvents)
}

void CGasdosePageDlg::OnManualDeltapos(NMHDR* pNMHDR, LRESULT* pResult)
{
 NM_UPDOWN* pUD = (NM_UPDOWN*)pNMHDR;
 m_man_ctrl_spin.SetFocus();

 int accDelta = m_params.manual_delta - pUD->iDelta;
 if (accDelta > 127)  accDelta = 127; //because signed char
 if (accDelta < -127) accDelta = -127;
 m_params.manual_delta = accDelta;

 OnChangeNotify(); //notify event receiver about change of view content(see class ParamPageEvents)
 *pResult = 0;
}

//Enable/disable all controls
void CGasdosePageDlg::Enable(bool enable)
{
 if (m_enabled == enable)
  return; //already has needed state
 m_enabled = enable;
 if (::IsWindow(m_hWnd))
  UpdateDialogControls(this,TRUE);
}

//get state of all controls (enabled or disabled)
bool CGasdosePageDlg::IsEnabled(void)
{
 return m_enabled;
}

void CGasdosePageDlg::EnableGasdoseTesting(bool enable)
{
 if (m_gasdose_tst_enabled == enable)
  return; //already has needed state
 m_gasdose_tst_enabled = enable;
 if (::IsWindow(this->m_hWnd))
  UpdateDialogControls(this, TRUE);
}

void CGasdosePageDlg::EnableGasdoseManPos(bool enable)
{
 if (m_gasdose_manpos_enabled == enable)
  return; //already has needed state
 m_gasdose_manpos_enabled = enable;
 if (::IsWindow(this->m_hWnd))
  UpdateDialogControls(this, TRUE);
}

//Use this function to obtain data from dialog
void CGasdosePageDlg::GetValues(SECU3IO::GasdosePar* o_values)
{
 ASSERT(o_values);
 UpdateData(TRUE); //копируем данные из диалога в переменные
 memcpy(o_values,&m_params, sizeof(SECU3IO::GasdosePar));
}

//эту функцию необходимо использовать когда надо занести данные в диалог
void CGasdosePageDlg::SetValues(const SECU3IO::GasdosePar* i_values)
{
 ASSERT(i_values);
 memcpy(&m_params,i_values, sizeof(SECU3IO::GasdosePar));
 if (!m_lock_ui_update)
  UpdateData(FALSE); //копируем данные из переменных в диалог
}

void CGasdosePageDlg::LockUIUpdate(bool lock)
{
 m_lock_ui_update = lock;
}
