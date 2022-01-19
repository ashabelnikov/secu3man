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

/** \file MiscPageDlg.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "Resources/resource.h"
#include "common/dpiaware.h"
#include "MiscPageDlg.h"
#include "ui-core/ToolTipCtrlEx.h"
#include "ui-core/ddx_helpers.h"
#include "ui-core/WndScroller.h"

const float AFBEGINEND_MINDIFF = 16500.0f; //this limit prevets overflow, can't be less

const UINT CMiscPageDlg::IDD = IDD_PD_MISC_PAGE;

BEGIN_MESSAGE_MAP(CMiscPageDlg, Super)
 ON_WM_DESTROY()
 ON_WM_SIZE()
 ON_EN_CHANGE(IDC_PD_MISC_PACKET_PERIOD_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_MISC_IGNCUTOFF_RPM_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_MISC_HALL_OUTPUT_START_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_MISC_HALL_OUTPUT_DURAT_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_MISC_EVAP_AFBEGIN_EDIT, OnChangeDataAfBegin)
 ON_EN_CHANGE(IDC_PD_MISC_EVAP_AFEND_EDIT, OnChangeDataAfEnd)
 ON_EN_CHANGE(IDC_PD_MISC_FLPMP_TIMEOUT_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_MISC_PWM1_FRQ_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_MISC_PWM2_FRQ_EDIT, OnChangeData)

 ON_CBN_SELCHANGE(IDC_PD_MISC_UART_SPEED_COMBO, OnChangeData)
 ON_BN_CLICKED(IDC_PD_MISC_IGNCUTOFF_CHECK, OnIgncutoffCheck)
 ON_BN_CLICKED(IDC_PD_MISC_FLPMP_OFFONGAS_CHECK, OnChangeData)
 ON_BN_CLICKED(IDC_PD_MISC_INJ_OFFONGAS_CHECK, OnChangeData)
 ON_BN_CLICKED(IDC_PD_MISC_INJ_OFFONPET_CHECK, OnChangeData)

 ON_UPDATE_COMMAND_UI(IDC_PD_MISC_UART_SPEED_COMBO_CAPTION, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_MISC_UART_SPEED_COMBO, OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_MISC_PACKET_PERIOD_EDIT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_MISC_PACKET_PERIOD_CAPTION, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_MISC_PACKET_PERIOD_UNIT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_MISC_PACKET_PERIOD_SPIN, OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_MISC_IGNCUTOFF_CHECK, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_MISC_IGNCUTOFF_RPM_EDIT, OnUpdateControlsIgncutoff)
 ON_UPDATE_COMMAND_UI(IDC_PD_MISC_IGNCUTOFF_RPM_UNIT, OnUpdateControlsIgncutoff)
 ON_UPDATE_COMMAND_UI(IDC_PD_MISC_IGNCUTOFF_RPM_SPIN, OnUpdateControlsIgncutoff)

 ON_UPDATE_COMMAND_UI(IDC_PD_MISC_HALL_OUTPUT_GROUP, OnUpdateControlsHOP)
 ON_UPDATE_COMMAND_UI(IDC_PD_MISC_HALL_OUTPUT_START_EDIT, OnUpdateControlsHOP)
 ON_UPDATE_COMMAND_UI(IDC_PD_MISC_HALL_OUTPUT_START_SPIN, OnUpdateControlsHOP)
 ON_UPDATE_COMMAND_UI(IDC_PD_MISC_HALL_OUTPUT_START_UNIT, OnUpdateControlsHOP)
 ON_UPDATE_COMMAND_UI(IDC_PD_MISC_HALL_OUTPUT_DURAT_EDIT, OnUpdateControlsHOP)
 ON_UPDATE_COMMAND_UI(IDC_PD_MISC_HALL_OUTPUT_DURAT_SPIN, OnUpdateControlsHOP)
 ON_UPDATE_COMMAND_UI(IDC_PD_MISC_HALL_OUTPUT_DURAT_UNIT, OnUpdateControlsHOP)

 ON_UPDATE_COMMAND_UI(IDC_PD_MISC_FLPMP_OFFONGAS_CHECK, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_MISC_INJ_OFFONGAS_CHECK, OnUpdateControlsInjTurnoffOnGas)
 ON_UPDATE_COMMAND_UI(IDC_PD_MISC_INJ_OFFONPET_CHECK, OnUpdateControlsInjTurnoffOnPet)

 ON_UPDATE_COMMAND_UI(IDC_PD_MISC_EVAP_AFBEGIN_EDIT, OnUpdateControlsSECU3iInj)
 ON_UPDATE_COMMAND_UI(IDC_PD_MISC_EVAP_AFBEGIN_CAPTION, OnUpdateControlsSECU3iInj)
 ON_UPDATE_COMMAND_UI(IDC_PD_MISC_EVAP_AFBEGIN_UNIT, OnUpdateControlsSECU3iInj)
 ON_UPDATE_COMMAND_UI(IDC_PD_MISC_EVAP_AFBEGIN_SPIN, OnUpdateControlsSECU3iInj)

 ON_UPDATE_COMMAND_UI(IDC_PD_MISC_EVAP_AFEND_EDIT, OnUpdateControlsSECU3iInj)
 ON_UPDATE_COMMAND_UI(IDC_PD_MISC_EVAP_AFEND_CAPTION, OnUpdateControlsSECU3iInj)
 ON_UPDATE_COMMAND_UI(IDC_PD_MISC_EVAP_AFEND_UNIT, OnUpdateControlsSECU3iInj)
 ON_UPDATE_COMMAND_UI(IDC_PD_MISC_EVAP_AFEND_SPIN, OnUpdateControlsSECU3iInj)

 ON_UPDATE_COMMAND_UI(IDC_PD_MISC_FLPMP_TIMEOUT_EDIT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_MISC_FLPMP_TIMEOUT_CAPTION, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_MISC_FLPMP_TIMEOUT_UNIT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_MISC_FLPMP_TIMEOUT_SPIN, OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_MISC_PWM1_FRQ_EDIT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_MISC_PWM1_FRQ_SPIN, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_MISC_PWM1_FRQ_CAPTION, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_MISC_PWM1_FRQ_UNIT, OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_MISC_PWM2_FRQ_EDIT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_MISC_PWM2_FRQ_SPIN, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_MISC_PWM2_FRQ_CAPTION, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_MISC_PWM2_FRQ_UNIT, OnUpdateControls)
END_MESSAGE_MAP()

CMiscPageDlg::CMiscPageDlg(CWnd* pParent /*=NULL*/)
: Super(CMiscPageDlg::IDD, pParent)
, m_enabled(false)
, m_hall_output_enabled(false)
, m_inj_turnoff_enabled(false)
, m_enable_secu3t_features(false)
, m_uart_speed_cb_index(0)
, m_packet_period_edit(CEditEx::MODE_INT, true)
, m_igncutoff_rpm_edit(CEditEx::MODE_INT, true)
, m_hop_start_edit(CEditEx::MODE_FLOAT | CEditEx::MODE_SIGNED, true)
, m_hop_durat_edit(CEditEx::MODE_FLOAT, true)
, m_evap_afbegin_edit(CEditEx::MODE_INT, true)
, m_evap_afend_edit(CEditEx::MODE_INT, true)
, m_fp_timeout_strt_edit(CEditEx::MODE_FLOAT, true)
, m_pwm1frq_edit(CEditEx::MODE_INT, true)
, m_pwm2frq_edit(CEditEx::MODE_INT, true)
, mp_scr(new CWndScroller)
{
 m_params.baud_rate = CBR_57600;
 m_params.period_ms = 0;
 m_params.ign_cutoff = 0;
 m_params.ign_cutoff_thrd = 0;
 m_params.hop_start_ang = 0;
 m_params.hop_durat_ang = 60.0f;
 m_params.flpmp_offongas = true;
 m_params.inj_offongas = false;
 m_params.evap_afbegin = 5000;
 m_params.evap_afslope = 0.000188351f;
 m_params.fp_timeout_strt = 5.0f;
 m_params.pwm2_pwmfrq[0] = 5000;
 m_params.pwm2_pwmfrq[1] = 5000;
}

LPCTSTR CMiscPageDlg::GetDialogID(void) const
{
 return (LPCTSTR)IDD;
}

float CMiscPageDlg::_calcAfEnd(void)
{
 float slope = m_params.evap_afslope > .0f ? m_params.evap_afslope : 0.000001f;
 return (32.0f / slope) + m_params.evap_afbegin; //convert afslope to afend
}

float CMiscPageDlg::_calcAfSlope(float afend)
{
 return 32.0f / (afend - m_params.evap_afbegin); //convert afend to afslope
}

void CMiscPageDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);

 DDX_Control(pDX,IDC_PD_MISC_UART_SPEED_COMBO_CAPTION, m_uart_speed_label);
 DDX_Control(pDX,IDC_PD_MISC_UART_SPEED_COMBO, m_uart_speed_combo);

 DDX_Control(pDX,IDC_PD_MISC_PACKET_PERIOD_EDIT, m_packet_period_edit);
 DDX_Control(pDX,IDC_PD_MISC_PACKET_PERIOD_CAPTION, m_packet_period_label);
 DDX_Control(pDX,IDC_PD_MISC_PACKET_PERIOD_UNIT, m_packet_period_unit);
 DDX_Control(pDX,IDC_PD_MISC_PACKET_PERIOD_SPIN, m_packet_period_spin);

 DDX_Control(pDX,IDC_PD_MISC_IGNCUTOFF_CHECK, m_igncutoff_check);
 DDX_Control(pDX,IDC_PD_MISC_IGNCUTOFF_RPM_EDIT, m_igncutoff_rpm_edit);
 DDX_Control(pDX,IDC_PD_MISC_IGNCUTOFF_RPM_UNIT, m_igncutoff_rpm_unit);
 DDX_Control(pDX,IDC_PD_MISC_IGNCUTOFF_RPM_SPIN, m_igncutoff_rpm_spin);

 DDX_Control(pDX,IDC_PD_MISC_HALL_OUTPUT_START_EDIT, m_hop_start_edit);
 DDX_Control(pDX,IDC_PD_MISC_HALL_OUTPUT_START_UNIT, m_hop_start_unit);
 DDX_Control(pDX,IDC_PD_MISC_HALL_OUTPUT_START_SPIN, m_hop_start_spin);

 DDX_Control(pDX,IDC_PD_MISC_HALL_OUTPUT_DURAT_EDIT, m_hop_durat_edit);
 DDX_Control(pDX,IDC_PD_MISC_HALL_OUTPUT_DURAT_UNIT, m_hop_durat_unit);
 DDX_Control(pDX,IDC_PD_MISC_HALL_OUTPUT_DURAT_SPIN, m_hop_durat_spin);

 DDX_Control(pDX,IDC_PD_MISC_FLPMP_OFFONGAS_CHECK, m_flpmp_offongas_check);
 DDX_Control(pDX,IDC_PD_MISC_INJ_OFFONGAS_CHECK, m_inj_offongas_check);
 DDX_Control(pDX,IDC_PD_MISC_INJ_OFFONPET_CHECK, m_inj_offonpet_check);

 DDX_Control(pDX,IDC_PD_MISC_EVAP_AFBEGIN_EDIT, m_evap_afbegin_edit);
 DDX_Control(pDX,IDC_PD_MISC_EVAP_AFBEGIN_SPIN, m_evap_afbegin_spin);

 DDX_Control(pDX,IDC_PD_MISC_EVAP_AFEND_EDIT, m_evap_afend_edit);
 DDX_Control(pDX,IDC_PD_MISC_EVAP_AFEND_SPIN, m_evap_afend_spin);

 DDX_Control(pDX,IDC_PD_MISC_FLPMP_TIMEOUT_EDIT, m_fp_timeout_strt_edit);
 DDX_Control(pDX,IDC_PD_MISC_FLPMP_TIMEOUT_SPIN, m_fp_timeout_strt_spin);

 DDX_Control(pDX, IDC_PD_MISC_PWM1_FRQ_SPIN, m_pwm1frq_spin);
 DDX_Control(pDX, IDC_PD_MISC_PWM1_FRQ_EDIT, m_pwm1frq_edit);

 DDX_Control(pDX, IDC_PD_MISC_PWM2_FRQ_SPIN, m_pwm2frq_spin);
 DDX_Control(pDX, IDC_PD_MISC_PWM2_FRQ_EDIT, m_pwm2frq_edit);

 DDX_CBIndex(pDX, IDC_PD_MISC_UART_SPEED_COMBO, m_uart_speed_cb_index);
 m_packet_period_edit.DDX_Value(pDX, IDC_PD_MISC_PACKET_PERIOD_EDIT, m_params.period_ms);
 m_igncutoff_rpm_edit.DDX_Value(pDX, IDC_PD_MISC_IGNCUTOFF_RPM_EDIT, m_params.ign_cutoff_thrd);
 m_hop_start_edit.DDX_Value(pDX, IDC_PD_MISC_HALL_OUTPUT_START_EDIT, m_params.hop_start_ang);
 m_hop_durat_edit.DDX_Value(pDX, IDC_PD_MISC_HALL_OUTPUT_DURAT_EDIT, m_params.hop_durat_ang);
 DDX_Check_UCHAR(pDX, IDC_PD_MISC_IGNCUTOFF_CHECK, m_params.ign_cutoff);
 DDX_Check_bool(pDX, IDC_PD_MISC_FLPMP_OFFONGAS_CHECK, m_params.flpmp_offongas);
 DDX_Check_bool(pDX, IDC_PD_MISC_INJ_OFFONGAS_CHECK, m_params.inj_offongas);
 DDX_Check_bool(pDX, IDC_PD_MISC_INJ_OFFONPET_CHECK, m_params.inj_offonpet);

 m_evap_afbegin_edit.DDX_Value(pDX, IDC_PD_MISC_EVAP_AFBEGIN_EDIT, m_params.evap_afbegin);
 float afend = _calcAfEnd(); //convert afslope to afend
 m_evap_afend_edit.DDX_Value(pDX, IDC_PD_MISC_EVAP_AFEND_EDIT, afend);
 m_params.evap_afslope = _calcAfSlope(afend); //convert afend to afslope

 m_fp_timeout_strt_edit.DDX_Value(pDX, IDC_PD_MISC_FLPMP_TIMEOUT_EDIT, m_params.fp_timeout_strt);

 m_pwm1frq_edit.DDX_Value(pDX, IDC_PD_MISC_PWM1_FRQ_EDIT, m_params.pwm2_pwmfrq[0]);
 m_pwm2frq_edit.DDX_Value(pDX, IDC_PD_MISC_PWM2_FRQ_EDIT, m_params.pwm2_pwmfrq[1]);
}

/////////////////////////////////////////////////////////////////////////////
// CMiscPageDlg message handlers

//Update controls
void CMiscPageDlg::OnUpdateControls(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled);
}

void CMiscPageDlg::OnUpdateControlsHOP(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled && m_hall_output_enabled);
}

void CMiscPageDlg::OnUpdateControlsInjTurnoffOnGas(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled && m_inj_turnoff_enabled);
}

void CMiscPageDlg::OnUpdateControlsInjTurnoffOnPet(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled && m_inj_turnoff_enabled);
}

void CMiscPageDlg::OnUpdateControlsIgncutoff(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled && m_igncutoff_check.GetCheck() == BST_CHECKED);
}

void CMiscPageDlg::OnUpdateControlsSECU3iInj(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled && !m_enable_secu3t_features && m_inj_turnoff_enabled);
}

void CMiscPageDlg::OnChangeData()
{
 UpdateData();
 OnChangeNotify(); //notify event receiver about change of view content(see class ParamPageEvents)
}

void CMiscPageDlg::OnChangeDataAfBegin()
{
 UpdateData();

 //Evap begin air flow can not be greater than end air flow
 if (m_params.evap_afbegin > _calcAfEnd() - AFBEGINEND_MINDIFF)
  m_evap_afend_spin.SetPos(m_params.evap_afbegin + AFBEGINEND_MINDIFF);

 OnChangeNotify(); //notify event receiver about change of view content(see class ParamPageEvents)
}

void CMiscPageDlg::OnChangeDataAfEnd()
{
 UpdateData();

 //Evap end air flow can not be less than begin air flow
 if (_calcAfEnd() <  m_params.evap_afbegin + AFBEGINEND_MINDIFF)
  m_evap_afbegin_spin.SetPos(_calcAfEnd() - AFBEGINEND_MINDIFF);

 OnChangeNotify(); //notify event receiver about change of view content(see class ParamPageEvents)
}

BOOL CMiscPageDlg::OnInitDialog()
{
 Super::OnInitDialog();

 m_packet_period_edit.SetLimitText(3);
 m_packet_period_edit.SetDecimalPlaces(3);
 m_packet_period_spin.SetBuddy(&m_packet_period_edit);
 m_packet_period_spin.SetRangeAndDelta(0, 500, 10);
 m_packet_period_edit.SetRange(0, 500);

 m_igncutoff_rpm_edit.SetLimitText(5);
 m_igncutoff_rpm_edit.SetDecimalPlaces(5);
 m_igncutoff_rpm_spin.SetBuddy(&m_igncutoff_rpm_edit);
 m_igncutoff_rpm_spin.SetRangeAndDelta(1000, 30000, 10);
 m_igncutoff_rpm_edit.SetRange(1000, 30000);

 m_hop_start_edit.SetLimitText(4);
 m_hop_start_edit.SetDecimalPlaces(0);
 m_hop_start_spin.SetBuddy(&m_hop_start_edit);
 m_hop_start_spin.SetRangeAndDelta(-36.0f, 72.0f, 1);
 m_hop_start_edit.SetRange(-36.0f, 72.0f);

 m_hop_durat_edit.SetLimitText(4);
 m_hop_durat_edit.SetDecimalPlaces(0);
 m_hop_durat_spin.SetBuddy(&m_hop_durat_edit);
 m_hop_durat_spin.SetRangeAndDelta(6.0f, 180.0f, 1);
 m_hop_durat_edit.SetRange(6.0f, 180.0f);

 m_evap_afbegin_edit.SetLimitText(7);
 m_evap_afbegin_edit.SetDecimalPlaces(7);
 m_evap_afbegin_spin.SetBuddy(&m_evap_afbegin_edit);
 m_evap_afbegin_spin.SetRangeAndDelta(100, 2000000, 1);
 m_evap_afbegin_edit.SetRange(100, 2000000);

 m_evap_afend_edit.SetLimitText(7);
 m_evap_afend_edit.SetDecimalPlaces(7);
 m_evap_afend_spin.SetBuddy(&m_evap_afend_edit);
 m_evap_afend_spin.SetRangeAndDelta(100, 2000000, 1);
 m_evap_afend_edit.SetRange(100, 2000000);

 m_fp_timeout_strt_edit.SetLimitText(4);
 m_fp_timeout_strt_edit.SetDecimalPlaces(1);
 m_fp_timeout_strt_spin.SetBuddy(&m_fp_timeout_strt_edit);
 m_fp_timeout_strt_spin.SetRangeAndDelta(0.1f, 25.5f, 0.1f);
 m_fp_timeout_strt_edit.SetRange(0.1f, 25.5f);

 m_pwm1frq_spin.SetBuddy(&m_pwm1frq_edit);
 m_pwm1frq_edit.SetLimitText(4);
 m_pwm1frq_edit.SetDecimalPlaces(4);
 m_pwm1frq_spin.SetRangeAndDelta(10, 5000, 1);
 m_pwm1frq_edit.SetRange(10, 5000);

 m_pwm2frq_spin.SetBuddy(&m_pwm2frq_edit);
 m_pwm2frq_edit.SetLimitText(4);
 m_pwm2frq_edit.SetDecimalPlaces(4);
 m_pwm2frq_spin.SetRangeAndDelta(10, 5000, 1);
 m_pwm2frq_edit.SetRange(10, 5000);

 BRCType br;
 for(size_t i = 0; i < SECU3IO::SECU3_ALLOWABLE_UART_DIVISORS_COUNT; ++i)
  br.push_back(SECU3IO::secu3_allowable_uart_divisors[i].first);
 FillUARTSpeedComboBox(br); //initialize combobox

 UpdateData(FALSE);  //initialize dialog controls with data

 //Create tooltip control
 mp_ttc.reset(new CToolTipCtrlEx());
 VERIFY(mp_ttc->Create(this, WS_POPUP | TTS_ALWAYSTIP | TTS_BALLOON));
 //set tooltips for widgets
 //ignition cut off threshold
 VERIFY(mp_ttc->AddWindow(&m_igncutoff_rpm_spin,MLL::GetString(IDS_PD_MISC_IGNCUTOFF_CHECK_TT)));
 VERIFY(mp_ttc->AddWindow(&m_igncutoff_rpm_edit,MLL::GetString(IDS_PD_MISC_IGNCUTOFF_CHECK_TT)));
 //ignition cut off check
 VERIFY(mp_ttc->AddWindow(&m_igncutoff_check,MLL::GetString(IDS_PD_MISC_IGNCUTOFF_CHECK_TT)));

 VERIFY(mp_ttc->AddWindow(&m_flpmp_offongas_check,MLL::GetString(IDS_PD_MISC_FLPMP_OFFONGAS_CHECK_TT)));
 VERIFY(mp_ttc->AddWindow(&m_inj_offongas_check,MLL::GetString(IDS_PD_MISC_INJ_OFFONGAS_CHECK_TT)));
    
 VERIFY(mp_ttc->AddWindow(&m_hop_start_edit, MLL::GetString(IDS_PD_MISC_HALL_OUTPUT_START_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_hop_start_spin, MLL::GetString(IDS_PD_MISC_HALL_OUTPUT_START_EDIT_TT)));

 VERIFY(mp_ttc->AddWindow(&m_hop_durat_edit, MLL::GetString(IDS_PD_MISC_HALL_OUTPUT_DURAT_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_hop_durat_spin, MLL::GetString(IDS_PD_MISC_HALL_OUTPUT_DURAT_EDIT_TT)));

 VERIFY(mp_ttc->AddWindow(&m_evap_afbegin_edit, MLL::GetString(IDS_PD_MISC_EVAP_AFBEGIN_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_evap_afbegin_spin, MLL::GetString(IDS_PD_MISC_EVAP_AFBEGIN_EDIT_TT)));

 VERIFY(mp_ttc->AddWindow(&m_evap_afend_edit, MLL::GetString(IDS_PD_MISC_EVAP_AFEND_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_evap_afend_spin, MLL::GetString(IDS_PD_MISC_EVAP_AFEND_EDIT_TT)));

 VERIFY(mp_ttc->AddWindow(&m_fp_timeout_strt_edit, MLL::GetString(IDS_PD_MISC_FLPMP_TIMEOUT_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_fp_timeout_strt_spin, MLL::GetString(IDS_PD_MISC_FLPMP_TIMEOUT_EDIT_TT)));

 VERIFY(mp_ttc->AddWindow(&m_uart_speed_combo, MLL::GetString(IDS_PD_MISC_UART_SPEED_COMBO_TT)));
 
 VERIFY(mp_ttc->AddWindow(&m_packet_period_edit, MLL::GetString(IDS_PD_MISC_PACKET_PERIOD_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_packet_period_spin, MLL::GetString(IDS_PD_MISC_PACKET_PERIOD_EDIT_TT)));

 VERIFY(mp_ttc->AddWindow(&m_inj_offonpet_check, MLL::GetString(IDS_PD_MISC_INJ_OFFONPET_CHECK_TT)));

 VERIFY(mp_ttc->AddWindow(&m_pwm1frq_edit, MLL::GetString(IDS_PD_MISC_PWM1_FRQ_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_pwm1frq_spin, MLL::GetString(IDS_PD_MISC_PWM1_FRQ_EDIT_TT)));

 VERIFY(mp_ttc->AddWindow(&m_pwm2frq_edit, MLL::GetString(IDS_PD_MISC_PWM2_FRQ_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_pwm2frq_spin, MLL::GetString(IDS_PD_MISC_PWM2_FRQ_EDIT_TT)));

 mp_ttc->SetMaxTipWidth(250); //Set width for text wrapping
 mp_ttc->ActivateToolTips(true);

 //initialize window scroller
 mp_scr->Init(this);

 UpdateDialogControls(this, TRUE);
 return TRUE;  // return TRUE unless you set the focus to a control
}

void CMiscPageDlg::OnDestroy()
{
 Super::OnDestroy();
 mp_scr->Close();
}

void CMiscPageDlg::OnIgncutoffCheck()
{
 UpdateData();
 OnChangeNotify();
 UpdateDialogControls(this, TRUE);
}

//разрешение/запрещение контроллов (всех поголовно)
void CMiscPageDlg::Enable(bool enable)
{
 if (m_enabled == enable)
  return; //already has needed state
 m_enabled = enable;
 if (::IsWindow(m_hWnd))
  UpdateDialogControls(this, TRUE);
}

//get state of controls
bool CMiscPageDlg::IsEnabled(void)
{
 return m_enabled;
}

void CMiscPageDlg::EnableHallOutputParams(bool enable)
{
 if (m_hall_output_enabled == enable)
  return; //already has needed state
 m_hall_output_enabled = enable;
 if (::IsWindow(this->m_hWnd))
  UpdateDialogControls(this, TRUE);
}

void CMiscPageDlg::EnableFuelInjection(bool i_enable)
{
 if (m_inj_turnoff_enabled == i_enable)
  return; //already
 m_inj_turnoff_enabled = i_enable;
 if (::IsWindow(this->m_hWnd))
  UpdateDialogControls(this, TRUE);
}

void CMiscPageDlg::EnableSECU3TItems(bool i_enable)
{
 if (m_enable_secu3t_features == i_enable)
  return; //already has needed state
 m_enable_secu3t_features = i_enable;
 if (::IsWindow(m_hWnd))
 {
  UpdateDialogControls(this, TRUE);
  RedrawWindow(); //strange, without this function call spin buttons don't update correctly...
 }
}

//эту функцию необходимо использовать когда надо получить данные из диалога
void CMiscPageDlg::GetValues(SECU3IO::MiscelPar* o_values)
{
 ASSERT(o_values);
 UpdateData(TRUE); //copy data from dialog to variables
 m_params.baud_rate = _GetBRFromComboBoxByIndex(m_uart_speed_cb_index);
 memcpy(o_values, &m_params, sizeof(SECU3IO::MiscelPar));
}

//эту функцию необходимо использовать когда надо занести данные в диалог
void CMiscPageDlg::SetValues(const SECU3IO::MiscelPar* i_values)
{
 ASSERT(i_values);
 memcpy(&m_params, i_values, sizeof(SECU3IO::MiscelPar));
 m_uart_speed_cb_index = _GetIndexFromComboBoxByBR(m_params.baud_rate);
 UpdateData(FALSE); //copy data from variables to dialog
}

void CMiscPageDlg::FillUARTSpeedComboBox(const BRCType& i_baudrates)
{
 m_baudrates.clear();
 m_baudrates = i_baudrates; //make a copy

 m_uart_speed_combo.ResetContent();
 for(size_t i = 0; i < m_baudrates.size(); i++)
 {
  CString string;
  string.Format(_T("%d"), m_baudrates[i]);
  int index = m_uart_speed_combo.AddString(string);
  m_uart_speed_combo.SetItemData(index, static_cast<DWORD>(i));
 }
}

int CMiscPageDlg::_GetBRFromComboBoxByIndex(int i_cb_index)
{
 DWORD item_data = m_uart_speed_combo.GetItemData(i_cb_index);
 ASSERT(item_data < m_baudrates.size()); //invalid index?
 return m_baudrates[static_cast<size_t>(item_data)];
}

int CMiscPageDlg::_GetIndexFromComboBoxByBR(int i_baudrate)
{
 int count = m_uart_speed_combo.GetCount();
 for (int i = 0; i < count; i++)
 {
  DWORD item_data = m_uart_speed_combo.GetItemData(i);
  int baud = m_baudrates[static_cast<size_t>(item_data)];
  if (i_baudrate == baud)
   return i;
 }
 ASSERT(0); //WOW! Unknown baud rate?
 return 0;
}

void CMiscPageDlg::OnSize( UINT nType, int cx, int cy )
{
 Super::OnSize(nType, cx, cy);

 DPIAware da;
 if (mp_scr.get())
  mp_scr->SetViewSize(cx, da.ScaleY(470));
}
