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

/** \file LambdaPageDlg.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "Resources/resource.h"
#include "LambdaPageDlg.h"
#include "ui-core/ToolTipCtrlEx.h"
#include "ui-core/WndScroller.h"
#include "ui-core/ddx_helpers.h"
#include "ui-core/fnt_helpers.h"
#include "common/dpiaware.h"

const UINT CLambdaPageDlg::IDD = IDD_PD_LAMBDA_PAGE;

BEGIN_MESSAGE_MAP(CLambdaPageDlg, Super)
 ON_WM_DESTROY()
 ON_WM_SIZE()
 ON_CBN_SELCHANGE(IDC_PD_LAMBDA_SENSTYPE_COMBO, OnChangeData)
 ON_EN_CHANGE(IDC_PD_LAMBDA_STRPERSTP_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_LAMBDA_MSPERSTP_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_LAMBDA_STEPSIZE_P_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_LAMBDA_STEPSIZE_M_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_LAMBDA_CORRLIMIT_P_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_LAMBDA_CORRLIMIT_M_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_LAMBDA_SWTPOINT_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_LAMBDA_TEMPTHRD_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_LAMBDA_RPMTHRD_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_LAMBDA_ACTIVDELAY_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_LAMBDA_DEADBAND_EDIT, OnChangeData)
 ON_BN_CLICKED(IDC_PD_LAMBDA_HTGDET_CHECK, OnChangeData)
 ON_BN_CLICKED(IDC_PD_LAMBDA_IDLCORR_CHECK, OnChangeData)
 ON_EN_CHANGE(IDC_PD_LAMBDA_2STOICHAFR_EDIT, OnChangeData)
 //heating:
 ON_EN_CHANGE(IDC_PD_LAMBDA_EH_TIME_COLD_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_LAMBDA_EH_TIME_HOT_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_LAMBDA_EH_TEMPER_THRD_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_LAMBDA_EH_HEATING_ACT_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_LAMBDA_EH_AFLOW_THRD_EDIT, OnChangeData)

 ON_UPDATE_COMMAND_UI(IDC_PD_LAMBDA_SENSTYPE_COMBO,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LAMBDA_SENSTYPE_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LAMBDA_STRPERSTP_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LAMBDA_STRPERSTP_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LAMBDA_STRPERSTP_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LAMBDA_STRPERSTP_UNIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LAMBDA_MSPERSTP_EDIT,OnUpdateMsPerStp)
 ON_UPDATE_COMMAND_UI(IDC_PD_LAMBDA_MSPERSTP_SPIN,OnUpdateMsPerStp)
 ON_UPDATE_COMMAND_UI(IDC_PD_LAMBDA_MSPERSTP_CAPTION,OnUpdateMsPerStp)
 ON_UPDATE_COMMAND_UI(IDC_PD_LAMBDA_MSPERSTP_UNIT,OnUpdateMsPerStp)
 ON_UPDATE_COMMAND_UI(IDC_PD_LAMBDA_STEPSIZE_P_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LAMBDA_STEPSIZE_P_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LAMBDA_STEPSIZE_P_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LAMBDA_STEPSIZE_P_UNIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LAMBDA_STEPSIZE_M_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LAMBDA_STEPSIZE_M_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LAMBDA_STEPSIZE_M_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LAMBDA_STEPSIZE_M_UNIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LAMBDA_CORRLIMIT_P_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LAMBDA_CORRLIMIT_P_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LAMBDA_CORRLIMIT_P_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LAMBDA_CORRLIMIT_P_UNIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LAMBDA_CORRLIMIT_M_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LAMBDA_CORRLIMIT_M_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LAMBDA_CORRLIMIT_M_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LAMBDA_CORRLIMIT_M_UNIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LAMBDA_SWTPOINT_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LAMBDA_SWTPOINT_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LAMBDA_SWTPOINT_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LAMBDA_SWTPOINT_UNIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LAMBDA_TEMPTHRD_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LAMBDA_TEMPTHRD_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LAMBDA_TEMPTHRD_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LAMBDA_TEMPTHRD_UNIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LAMBDA_RPMTHRD_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LAMBDA_RPMTHRD_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LAMBDA_RPMTHRD_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LAMBDA_RPMTHRD_UNIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LAMBDA_ACTIVDELAY_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LAMBDA_ACTIVDELAY_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LAMBDA_ACTIVDELAY_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LAMBDA_ACTIVDELAY_UNIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LAMBDA_DEADBAND_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LAMBDA_DEADBAND_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LAMBDA_DEADBAND_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LAMBDA_DEADBAND_UNIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LAMBDA_HTGDET_CHECK,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LAMBDA_IDLCORR_CHECK,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LAMBDA_2STOICHAFR_EDIT,OnUpdateFuelInjectionControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LAMBDA_2STOICHAFR_SPIN,OnUpdateFuelInjectionControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LAMBDA_2STOICHAFR_CAPTION,OnUpdateFuelInjectionControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LAMBDA_2STOICHAFR_UNIT,OnUpdateFuelInjectionControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LAMBDA_EH_TIME_COLD_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LAMBDA_EH_TIME_HOT_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LAMBDA_EH_TEMPER_THRD_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LAMBDA_EH_HEATING_ACT_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LAMBDA_EH_AFLOW_THRD_EDIT,OnUpdateControls)
END_MESSAGE_MAP()

CLambdaPageDlg::CLambdaPageDlg(CWnd* pParent /*=NULL*/)
: Super(CLambdaPageDlg::IDD, pParent)
, m_enabled(false)
, m_fuel_injection(false)
, m_strperstp_edit(CEditEx::MODE_INT, true)
, m_msperstp_edit(CEditEx::MODE_INT, true)
, m_stepsize_p_edit(CEditEx::MODE_FLOAT, true)
, m_stepsize_m_edit(CEditEx::MODE_FLOAT, true)
, m_corrlimit_p_edit(CEditEx::MODE_FLOAT, true)
, m_corrlimit_m_edit(CEditEx::MODE_FLOAT, true)
, m_swtpoint_edit(CEditEx::MODE_FLOAT, true)
, m_tempthrd_edit(CEditEx::MODE_FLOAT, true)
, m_rpmthrd_edit(CEditEx::MODE_INT, true)
, m_activdelay_edit(CEditEx::MODE_INT, true)
, m_deadband_edit(CEditEx::MODE_FLOAT, true)
, m_2stoichval_edit(CEditEx::MODE_FLOAT, true)
//heating:
, m_eh_ht_cold_edit(CEditEx::MODE_FLOAT, true)
, m_eh_ht_hot_edit(CEditEx::MODE_FLOAT, true)
, m_eh_temper_thrd_edit(CEditEx::MODE_FLOAT, true)
, m_eh_heating_act_edit(CEditEx::MODE_FLOAT, true)
, m_eh_aflow_thrd_edit(CEditEx::MODE_INT, true)
, mp_scr(new CWndScroller)
{
 m_params.lam_str_per_stp = 10;
 m_params.lam_ms_per_stp = 0;
 m_params.lam_step_size_p = 2.5f;
 m_params.lam_step_size_m = 2.5f;
 m_params.lam_corr_limit_p = 30.0f;
 m_params.lam_corr_limit_m = 30.0f;
 m_params.lam_swt_point = 0.50f;
 m_params.lam_temp_thrd = 70.0f;
 m_params.lam_rpm_thrd = 800;
 m_params.lam_activ_delay = 45;
 m_params.lam_dead_band = 0.0f;
 m_params.lam_senstype = 0; //NBO
 m_params.lam_htgdet = false;
 m_params.lam_2stoichval = 15.6f;
 //heating
 m_params.eh_heating_time[0] = 15.0f;
 m_params.eh_heating_time[1] = 30.0f;
 m_params.eh_temper_thrd = 70.0f;
 m_params.eh_heating_act = 0.06f;
 m_params.eh_aflow_thrd = 320000.0f; 
}

CLambdaPageDlg::~CLambdaPageDlg()
{
 //empty
}

LPCTSTR CLambdaPageDlg::GetDialogID(void) const
{
 return (LPCTSTR)IDD;
}

void CLambdaPageDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);
 DDX_Control(pDX,IDC_PD_LAMBDA_SENSTYPE_COMBO, m_senstype_combo);
 DDX_Control(pDX,IDC_PD_LAMBDA_STRPERSTP_EDIT, m_strperstp_edit);
 DDX_Control(pDX,IDC_PD_LAMBDA_STRPERSTP_SPIN, m_strperstp_spin);
 DDX_Control(pDX,IDC_PD_LAMBDA_MSPERSTP_EDIT, m_msperstp_edit);
 DDX_Control(pDX,IDC_PD_LAMBDA_MSPERSTP_SPIN, m_msperstp_spin);
 DDX_Control(pDX,IDC_PD_LAMBDA_STEPSIZE_P_EDIT, m_stepsize_p_edit);
 DDX_Control(pDX,IDC_PD_LAMBDA_STEPSIZE_P_SPIN, m_stepsize_p_spin);
 DDX_Control(pDX,IDC_PD_LAMBDA_STEPSIZE_M_EDIT, m_stepsize_m_edit);
 DDX_Control(pDX,IDC_PD_LAMBDA_STEPSIZE_M_SPIN, m_stepsize_m_spin);
 DDX_Control(pDX,IDC_PD_LAMBDA_CORRLIMIT_P_EDIT, m_corrlimit_p_edit);
 DDX_Control(pDX,IDC_PD_LAMBDA_CORRLIMIT_P_SPIN, m_corrlimit_p_spin);
 DDX_Control(pDX,IDC_PD_LAMBDA_CORRLIMIT_M_EDIT, m_corrlimit_m_edit);
 DDX_Control(pDX,IDC_PD_LAMBDA_CORRLIMIT_M_SPIN, m_corrlimit_m_spin);
 DDX_Control(pDX,IDC_PD_LAMBDA_SWTPOINT_EDIT, m_swtpoint_edit);
 DDX_Control(pDX,IDC_PD_LAMBDA_SWTPOINT_SPIN, m_swtpoint_spin);
 DDX_Control(pDX,IDC_PD_LAMBDA_TEMPTHRD_EDIT, m_tempthrd_edit);
 DDX_Control(pDX,IDC_PD_LAMBDA_TEMPTHRD_SPIN, m_tempthrd_spin);
 DDX_Control(pDX,IDC_PD_LAMBDA_RPMTHRD_EDIT, m_rpmthrd_edit);
 DDX_Control(pDX,IDC_PD_LAMBDA_RPMTHRD_SPIN, m_rpmthrd_spin);
 DDX_Control(pDX,IDC_PD_LAMBDA_ACTIVDELAY_EDIT, m_activdelay_edit);
 DDX_Control(pDX,IDC_PD_LAMBDA_ACTIVDELAY_SPIN, m_activdelay_spin);
 DDX_Control(pDX,IDC_PD_LAMBDA_DEADBAND_EDIT, m_deadband_edit);
 DDX_Control(pDX,IDC_PD_LAMBDA_DEADBAND_SPIN, m_deadband_spin);
 DDX_Control(pDX,IDC_PD_LAMBDA_HTGDET_CHECK, m_htgdet_check);
 DDX_Control(pDX,IDC_PD_LAMBDA_IDLCORR_CHECK, m_idlcorr_check);
 DDX_Control(pDX,IDC_PD_LAMBDA_2STOICHAFR_EDIT, m_2stoichval_edit);
 DDX_Control(pDX,IDC_PD_LAMBDA_2STOICHAFR_SPIN, m_2stoichval_spin);
 DDX_Control(pDX, IDC_PD_LAMBDA_EH_TIME_COLD_EDIT, m_eh_ht_cold_edit);
 DDX_Control(pDX, IDC_PD_LAMBDA_EH_TIME_COLD_SPIN, m_eh_ht_cold_spin);
 DDX_Control(pDX, IDC_PD_LAMBDA_EH_TIME_HOT_EDIT, m_eh_ht_hot_edit);
 DDX_Control(pDX, IDC_PD_LAMBDA_EH_TIME_HOT_SPIN, m_eh_ht_hot_spin);
 DDX_Control(pDX, IDC_PD_LAMBDA_EH_TEMPER_THRD_EDIT, m_eh_temper_thrd_edit);
 DDX_Control(pDX, IDC_PD_LAMBDA_EH_TEMPER_THRD_SPIN, m_eh_temper_thrd_spin);
 DDX_Control(pDX, IDC_PD_LAMBDA_EH_HEATING_ACT_EDIT, m_eh_heating_act_edit);
 DDX_Control(pDX, IDC_PD_LAMBDA_EH_HEATING_ACT_SPIN, m_eh_heating_act_spin);
 DDX_Control(pDX, IDC_PD_LAMBDA_EH_AFLOW_THRD_EDIT, m_eh_aflow_thrd_edit);
 DDX_Control(pDX, IDC_PD_LAMBDA_EH_AFLOW_THRD_SPIN, m_eh_aflow_thrd_spin);

 DDX_CBIndex_UCHAR(pDX, IDC_PD_LAMBDA_SENSTYPE_COMBO, m_params.lam_senstype);
 m_strperstp_edit.DDX_Value(pDX, IDC_PD_LAMBDA_STRPERSTP_EDIT, m_params.lam_str_per_stp);
 m_msperstp_edit.DDX_Value(pDX, IDC_PD_LAMBDA_MSPERSTP_EDIT, m_params.lam_ms_per_stp);
 m_stepsize_p_edit.DDX_Value(pDX, IDC_PD_LAMBDA_STEPSIZE_P_EDIT, m_params.lam_step_size_p);
 m_stepsize_m_edit.DDX_Value(pDX, IDC_PD_LAMBDA_STEPSIZE_M_EDIT, m_params.lam_step_size_m);
 m_corrlimit_p_edit.DDX_Value(pDX, IDC_PD_LAMBDA_CORRLIMIT_P_EDIT, m_params.lam_corr_limit_p);
 m_corrlimit_m_edit.DDX_Value(pDX, IDC_PD_LAMBDA_CORRLIMIT_M_EDIT, m_params.lam_corr_limit_m);
 m_swtpoint_edit.DDX_Value(pDX, IDC_PD_LAMBDA_SWTPOINT_EDIT, m_params.lam_swt_point);
 m_tempthrd_edit.DDX_Value(pDX, IDC_PD_LAMBDA_TEMPTHRD_EDIT, m_params.lam_temp_thrd);
 m_rpmthrd_edit.DDX_Value(pDX, IDC_PD_LAMBDA_RPMTHRD_EDIT, m_params.lam_rpm_thrd);
 m_activdelay_edit.DDX_Value(pDX, IDC_PD_LAMBDA_ACTIVDELAY_EDIT, m_params.lam_activ_delay);
 m_deadband_edit.DDX_Value(pDX, IDC_PD_LAMBDA_DEADBAND_EDIT, m_params.lam_dead_band);
 m_2stoichval_edit.DDX_Value(pDX, IDC_PD_LAMBDA_2STOICHAFR_EDIT, m_params.lam_2stoichval);
 DDX_Check_bool(pDX, IDC_PD_LAMBDA_HTGDET_CHECK, m_params.lam_htgdet);
 DDX_Check_bool(pDX, IDC_PD_LAMBDA_IDLCORR_CHECK, m_params.lam_idlcorr);
 //heating:
 m_eh_ht_cold_edit.DDX_Value(pDX, IDC_PD_LAMBDA_EH_TIME_COLD_EDIT, m_params.eh_heating_time[0]);
 m_eh_ht_hot_edit.DDX_Value(pDX, IDC_PD_LAMBDA_EH_TIME_HOT_EDIT, m_params.eh_heating_time[1]);
 m_eh_temper_thrd_edit.DDX_Value(pDX, IDC_PD_LAMBDA_EH_TEMPER_THRD_EDIT, m_params.eh_temper_thrd);
 m_eh_heating_act_edit.DDX_Value(pDX, IDC_PD_LAMBDA_EH_HEATING_ACT_EDIT, m_params.eh_heating_act);
 m_eh_aflow_thrd_edit.DDX_Value(pDX, IDC_PD_LAMBDA_EH_AFLOW_THRD_EDIT, m_params.eh_aflow_thrd);
}

void CLambdaPageDlg::OnUpdateControls(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled);
}

void CLambdaPageDlg::OnUpdateMsPerStp(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled && m_params.lam_str_per_stp == 0);
}

void CLambdaPageDlg::OnUpdateFuelInjectionControls(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled && m_fuel_injection);
}

/////////////////////////////////////////////////////////////////////////////
// CLambdaPageDlg message handlers

BOOL CLambdaPageDlg::OnInitDialog()
{
 Super::OnInitDialog();

 m_strperstp_spin.SetBuddy(&m_strperstp_edit);
 m_strperstp_edit.SetLimitText(3);
 m_strperstp_edit.SetDecimalPlaces(3);
 m_strperstp_spin.SetRangeAndDelta(0, 100, 1);
 m_strperstp_edit.SetRange(0, 100);

 m_msperstp_spin.SetBuddy(&m_msperstp_edit);
 m_msperstp_edit.SetLimitText(4);
 m_msperstp_edit.SetDecimalPlaces(4);
 m_msperstp_spin.SetRangeAndDelta(0, 2500, 10);
 m_msperstp_edit.SetRange(0, 2500);

 m_stepsize_p_spin.SetBuddy(&m_stepsize_p_edit);
 m_stepsize_p_edit.SetLimitText(5);
 m_stepsize_p_edit.SetDecimalPlaces(2);
 m_stepsize_p_spin.SetRangeAndDelta(0.0f, 45.00f, 0.25f);
 m_stepsize_p_edit.SetRange(0.0f, 45.00f);

 m_stepsize_m_spin.SetBuddy(&m_stepsize_m_edit);
 m_stepsize_m_edit.SetLimitText(5);
 m_stepsize_m_edit.SetDecimalPlaces(2);
 m_stepsize_m_spin.SetRangeAndDelta(0.0f, 45.00f, 0.25f);
 m_stepsize_m_edit.SetRange(0.0f, 45.00f);

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

 m_swtpoint_spin.SetBuddy(&m_swtpoint_edit);
 m_swtpoint_edit.SetLimitText(4);
 m_swtpoint_edit.SetDecimalPlaces(2);
 m_swtpoint_spin.SetRangeAndDelta(0.10f, 5.00f, 0.01f);
 m_swtpoint_edit.SetRange(0.10f, 5.00f);

 m_tempthrd_spin.SetBuddy(&m_tempthrd_edit);
 m_tempthrd_edit.SetLimitText(5);
 m_tempthrd_edit.SetDecimalPlaces(2);
 m_tempthrd_spin.SetRangeAndDelta(0.00f, 200.00f, 0.25f);
 m_tempthrd_edit.SetRange(0.00f, 200.00f);

 m_rpmthrd_spin.SetBuddy(&m_rpmthrd_edit);
 m_rpmthrd_edit.SetLimitText(4);
 m_rpmthrd_edit.SetDecimalPlaces(5);
 m_rpmthrd_spin.SetRangeAndDelta(0, 15000, 50);
 m_rpmthrd_edit.SetRange(0, 15000);

 m_activdelay_spin.SetBuddy(&m_activdelay_edit);
 m_activdelay_edit.SetLimitText(3);
 m_activdelay_edit.SetDecimalPlaces(3);
 m_activdelay_spin.SetRangeAndDelta(0, 255, 1);
 m_activdelay_edit.SetRange(0, 255);

 m_deadband_spin.SetBuddy(&m_deadband_edit);
 m_deadband_edit.SetLimitText(4);
 m_deadband_edit.SetDecimalPlaces(2);
 m_deadband_spin.SetRangeAndDelta(0.0f, 5.00f, 0.01f);
 m_deadband_edit.SetRange(0.0f, 5.00f);

 m_2stoichval_spin.SetBuddy(&m_2stoichval_edit);
 m_2stoichval_edit.SetLimitText(4);
 m_2stoichval_edit.SetDecimalPlaces(1);
 m_2stoichval_spin.SetRangeAndDelta(8.0f, 22.00f, 0.1f);
 m_2stoichval_edit.SetRange(8.0f, 22.00f);

 m_eh_ht_cold_spin.SetBuddy(&m_eh_ht_cold_edit);
 m_eh_ht_cold_edit.SetLimitText(3);
 m_eh_ht_cold_edit.SetDecimalPlaces(0);
 m_eh_ht_cold_spin.SetRangeAndDelta(0.0f, 255.00f, 1.0f);
 m_eh_ht_cold_edit.SetRange(0.0f, 255.00f);

 m_eh_ht_hot_spin.SetBuddy(&m_eh_ht_hot_edit);
 m_eh_ht_hot_edit.SetLimitText(3);
 m_eh_ht_hot_edit.SetDecimalPlaces(0);
 m_eh_ht_hot_spin.SetRangeAndDelta(0.0f, 255.00f, 1.0f);
 m_eh_ht_hot_edit.SetRange(0.0f, 255.00f);

 m_eh_temper_thrd_spin.SetBuddy(&m_eh_temper_thrd_edit);
 m_eh_temper_thrd_edit.SetLimitText(3);
 m_eh_temper_thrd_edit.SetDecimalPlaces(0);
 m_eh_temper_thrd_spin.SetRangeAndDelta(0.0f, 200.00f, 1.0f);
 m_eh_temper_thrd_edit.SetRange(0.0f, 200.00f);

 m_eh_heating_act_spin.SetBuddy(&m_eh_heating_act_edit);
 m_eh_heating_act_edit.SetLimitText(4);
 m_eh_heating_act_edit.SetDecimalPlaces(2);
 m_eh_heating_act_spin.SetRangeAndDelta(0.0f, 2.55f, 0.01f);
 m_eh_heating_act_edit.SetRange(0.0f, 2.55f);

 m_eh_aflow_thrd_spin.SetBuddy(&m_eh_aflow_thrd_edit);
 m_eh_aflow_thrd_edit.SetLimitText(7);
 m_eh_aflow_thrd_edit.SetDecimalPlaces(7);
 m_eh_aflow_thrd_spin.SetRangeAndDelta(100.0f, 2000000.0f, 10.0f);
 m_eh_aflow_thrd_edit.SetRange(100.0f, 2000000.0f);

 m_senstype_combo.AddString(MLL::LoadString(IDS_PD_NBO));
 m_senstype_combo.AddString(MLL::LoadString(IDS_PD_WBO));

 //initialize window scroller
 mp_scr->Init(this);

 //create a tooltip control and assign tooltips
 mp_ttc.reset(new CToolTipCtrlEx());
 VERIFY(mp_ttc->Create(this, WS_POPUP | TTS_ALWAYSTIP | TTS_BALLOON));
 VERIFY(mp_ttc->AddWindow(&m_strperstp_edit, MLL::GetString(IDS_PD_LAMBDA_STRPERSTP_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_strperstp_spin, MLL::GetString(IDS_PD_LAMBDA_STRPERSTP_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_senstype_combo, MLL::GetString(IDS_PD_LAMBDA_SENSTYPE_COMBO_TT)));
 VERIFY(mp_ttc->AddWindow(&m_msperstp_edit, MLL::GetString(IDS_PD_LAMBDA_MSPERSTP_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_msperstp_spin, MLL::GetString(IDS_PD_LAMBDA_MSPERSTP_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_rpmthrd_edit, MLL::GetString(IDS_PD_LAMBDA_RPMTHRD_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_rpmthrd_spin, MLL::GetString(IDS_PD_LAMBDA_RPMTHRD_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_tempthrd_edit, MLL::GetString(IDS_PD_LAMBDA_TEMPTHRD_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_tempthrd_spin, MLL::GetString(IDS_PD_LAMBDA_TEMPTHRD_EDIT_TT)));

 mp_ttc->SetMaxTipWidth(250); //Enable text wrapping
 mp_ttc->ActivateToolTips(true);

 //Set bold font
 CloneWndFont(this, &m_boldDlgFont, -1, true);
 GetDlgItem(IDC_PD_LAMBDA_HEATING_GROUP)->SetFont(&m_boldDlgFont);

 UpdateData(FALSE);
 UpdateDialogControls(this, TRUE);
 return TRUE;  // return TRUE unless you set the focus to a control
}

void CLambdaPageDlg::OnDestroy()
{
 Super::OnDestroy();
 mp_scr->Close();
}

void CLambdaPageDlg::OnChangeData()
{
 UpdateData();
 OnChangeNotify(); //notify event receiver about change in view content(see class ParamPageEvents)
}

void CLambdaPageDlg::EnableFuelInjection(bool i_enable)
{
 if (m_fuel_injection == i_enable)
  return; //already has needed state
 m_fuel_injection = i_enable;
 if (::IsWindow(this->m_hWnd))
  UpdateDialogControls(this, TRUE);
}

//разрешение/запрещение контроллов (всех поголовно)
void CLambdaPageDlg::Enable(bool enable)
{
 if (m_enabled == enable)
  return; //already has needed state
 m_enabled = enable;
 if (::IsWindow(m_hWnd))
  UpdateDialogControls(this,TRUE);
}

//что с контроллами?
bool CLambdaPageDlg::IsEnabled(void)
{
 return m_enabled;
}

//Use this function for obtaining data from dialog
void CLambdaPageDlg::GetValues(SECU3IO::LambdaPar* o_values)
{
 ASSERT(o_values);
 UpdateData(TRUE); //copy data from dialog controls to variables
 memcpy(o_values,&m_params, sizeof(SECU3IO::LambdaPar));
}

//Use this function to set dialog data
void CLambdaPageDlg::SetValues(const SECU3IO::LambdaPar* i_values)
{
 ASSERT(i_values);
 memcpy(&m_params,i_values, sizeof(SECU3IO::LambdaPar));
 UpdateData(FALSE); //copy data from variables to dialog controls
 UpdateDialogControls(this, TRUE);
}

void CLambdaPageDlg::OnSize( UINT nType, int cx, int cy )
{
 Super::OnSize(nType, cx, cy);

 DPIAware da;
 if (mp_scr.get())
  mp_scr->SetViewSize(cx, da.ScaleY(700));
}
