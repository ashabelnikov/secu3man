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

/** \file TemperPageDlg.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "Resources/resource.h"
#include "common/dpiaware.h"
#include "TemperPageDlg.h"
#include "ui-core/ToolTipCtrlEx.h"
#include "ui-core/ddx_helpers.h"
#include "ui-core/WndScroller.h"

const UINT CTemperPageDlg::IDD = IDD_PD_TEMPER_PAGE;
const float TEMP_HYSTERESIS = 0.25f;

BEGIN_MESSAGE_MAP(CTemperPageDlg, Super)
 ON_WM_DESTROY()
 ON_WM_SIZE()
 ON_EN_CHANGE(IDC_PD_TEMPER_VENT_ON_THRESHOLD_EDIT, OnChangePdTemperVentOnThresholdEdit)
 ON_EN_CHANGE(IDC_PD_TEMPER_VENT_OFF_THRESHOLD_EDIT, OnChangePdTemperVentOffThresholdEdit)
 ON_EN_CHANGE(IDC_PD_TEMPER_PWM_FRQ_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_TEMPER_CONDPVTON_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_TEMPER_CONDPVTOFF_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_TEMPER_CONDMINRPM_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_TEMPER_VENTTIMER_EDIT, OnChangeData)
 ON_BN_CLICKED(IDC_PD_TEMPER_USE_TEMP_SENSOR, OnPdTemperUseTempSensor)
 ON_BN_CLICKED(IDC_PD_TEMPER_USE_VENT_PWM, OnPdTemperUseVentPwm)
 ON_BN_CLICKED(IDC_PD_TEMPER_USE_CURVE_MAP, OnPdTemperUseCurveMap)

 ON_UPDATE_COMMAND_UI(IDC_PD_TEMPER_VENT_ON_THRESHOLD_EDIT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_TEMPER_VENT_ON_THRESHOLD_SPIN, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_TEMPER_VENT_ON_THRESHOLD_CAPTION, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_TEMPER_VENT_ON_THRESHOLD_UNIT, OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_TEMPER_VENT_OFF_THRESHOLD_EDIT, OnUpdateVentOff)
 ON_UPDATE_COMMAND_UI(IDC_PD_TEMPER_VENT_OFF_THRESHOLD_SPIN, OnUpdateVentOff)
 ON_UPDATE_COMMAND_UI(IDC_PD_TEMPER_VENT_OFF_THRESHOLD_CAPTION, OnUpdateVentOff)
 ON_UPDATE_COMMAND_UI(IDC_PD_TEMPER_VENT_OFF_THRESHOLD_UNIT, OnUpdateVentOff)

 ON_UPDATE_COMMAND_UI(IDC_PD_TEMPER_USE_TEMP_SENSOR, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_TEMPER_USE_VENT_PWM, OnUpdateUseVentPwm)
 ON_UPDATE_COMMAND_UI(IDC_PD_TEMPER_USE_CURVE_MAP, OnUpdateUseCurveMap)

 ON_UPDATE_COMMAND_UI(IDC_PD_TEMPER_PWM_FRQ_EDIT, OnUpdateUseVentPwm)
 ON_UPDATE_COMMAND_UI(IDC_PD_TEMPER_PWM_FRQ_SPIN, OnUpdateUseVentPwm)
 ON_UPDATE_COMMAND_UI(IDC_PD_TEMPER_PWM_FRQ_CAPTION, OnUpdateUseVentPwm)
 ON_UPDATE_COMMAND_UI(IDC_PD_TEMPER_PWM_FRQ_UNIT, OnUpdateUseVentPwm)

 ON_UPDATE_COMMAND_UI(IDC_PD_TEMPER_CONDPVTON_EDIT, OnUpdateControlsSECU3iFI)
 ON_UPDATE_COMMAND_UI(IDC_PD_TEMPER_CONDPVTON_SPIN, OnUpdateControlsSECU3iFI)
 ON_UPDATE_COMMAND_UI(IDC_PD_TEMPER_CONDPVTON_CAPTION, OnUpdateControlsSECU3iFI)
 ON_UPDATE_COMMAND_UI(IDC_PD_TEMPER_CONDPVTON_UNIT, OnUpdateControlsSECU3iFI)

 ON_UPDATE_COMMAND_UI(IDC_PD_TEMPER_CONDPVTOFF_EDIT, OnUpdateControlsSECU3iFI)
 ON_UPDATE_COMMAND_UI(IDC_PD_TEMPER_CONDPVTOFF_SPIN, OnUpdateControlsSECU3iFI)
 ON_UPDATE_COMMAND_UI(IDC_PD_TEMPER_CONDPVTOFF_CAPTION, OnUpdateControlsSECU3iFI)
 ON_UPDATE_COMMAND_UI(IDC_PD_TEMPER_CONDPVTOFF_UNIT, OnUpdateControlsSECU3iFI)

 ON_UPDATE_COMMAND_UI(IDC_PD_TEMPER_CONDMINRPM_EDIT, OnUpdateFuelInjectionControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_TEMPER_CONDMINRPM_SPIN, OnUpdateFuelInjectionControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_TEMPER_CONDMINRPM_CAPTION, OnUpdateFuelInjectionControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_TEMPER_CONDMINRPM_UNIT, OnUpdateFuelInjectionControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_TEMPER_VENTTIMER_EDIT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_TEMPER_VENTTIMER_SPIN, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_TEMPER_VENTTIMER_CAPTION, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_TEMPER_VENTTIMER_UNIT, OnUpdateControls)
END_MESSAGE_MAP()

CTemperPageDlg::CTemperPageDlg(CWnd* pParent /*=NULL*/)
: Super(CTemperPageDlg::IDD, pParent)
, m_enabled(false)
, m_enable_secu3t_features(false)
, m_use_vent_pwm_enabled(false)
, m_use_curve_map_enabled(false)
, m_fuel_injection(false)
, m_vent_on_threshold_edit(CEditEx::MODE_FLOAT | CEditEx::MODE_SIGNED, true)
, m_vent_off_threshold_edit(CEditEx::MODE_FLOAT | CEditEx::MODE_SIGNED, true)
, m_vent_pwmfrq_edit(CEditEx::MODE_INT, true)
, m_cond_pvt_on_edit(CEditEx::MODE_FLOAT, true)
, m_cond_pvt_off_edit(CEditEx::MODE_FLOAT, true)
, m_cond_min_rpm_edit(CEditEx::MODE_INT, true)
, m_vent_timer_edit(CEditEx::MODE_FLOAT, true)
, mp_scr(new CWndScroller)
{
 m_params.vent_on = 95.0f;
 m_params.vent_off = 98.0f;
 m_params.tmp_use = 1;
 m_params.vent_pwm = 0;
 m_params.cts_use_map = 0;
 m_params.vent_pwmfrq = 5000;
 m_params.cond_pvt_on = 1.6f;
 m_params.cond_pvt_off = 2.5f;
 m_params.cond_min_rpm = 1200;
 m_params.vent_tmr = 60.0f;
}

LPCTSTR CTemperPageDlg::GetDialogID(void) const
{
 return (LPCTSTR)IDD;
}

void CTemperPageDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);
 DDX_Control(pDX, IDC_PD_TEMPER_USE_TEMP_SENSOR, m_use_temp_sensor);
 DDX_Control(pDX, IDC_PD_TEMPER_USE_VENT_PWM, m_use_vent_pwm);
 DDX_Control(pDX, IDC_PD_TEMPER_USE_CURVE_MAP, m_use_curve_map);
 DDX_Control(pDX, IDC_PD_TEMPER_VENT_ON_THRESHOLD_SPIN, m_vent_on_threshold_spin);
 DDX_Control(pDX, IDC_PD_TEMPER_VENT_OFF_THRESHOLD_SPIN, m_vent_off_threshold_spin);
 DDX_Control(pDX, IDC_PD_TEMPER_VENT_OFF_THRESHOLD_EDIT, m_vent_off_threshold_edit);
 DDX_Control(pDX, IDC_PD_TEMPER_VENT_ON_THRESHOLD_EDIT, m_vent_on_threshold_edit);
 DDX_Control(pDX, IDC_PD_TEMPER_PWM_FRQ_SPIN, m_vent_pwmfrq_spin);
 DDX_Control(pDX, IDC_PD_TEMPER_PWM_FRQ_EDIT, m_vent_pwmfrq_edit);
 DDX_Control(pDX, IDC_PD_TEMPER_CONDPVTON_SPIN, m_cond_pvt_on_spin);
 DDX_Control(pDX, IDC_PD_TEMPER_CONDPVTON_EDIT, m_cond_pvt_on_edit);
 DDX_Control(pDX, IDC_PD_TEMPER_CONDPVTOFF_SPIN, m_cond_pvt_off_spin);
 DDX_Control(pDX, IDC_PD_TEMPER_CONDPVTOFF_EDIT, m_cond_pvt_off_edit);
 DDX_Control(pDX, IDC_PD_TEMPER_CONDMINRPM_SPIN, m_cond_min_rpm_spin);
 DDX_Control(pDX, IDC_PD_TEMPER_CONDMINRPM_EDIT, m_cond_min_rpm_edit);
 DDX_Control(pDX, IDC_PD_TEMPER_VENTTIMER_SPIN, m_vent_timer_spin);
 DDX_Control(pDX, IDC_PD_TEMPER_VENTTIMER_EDIT, m_vent_timer_edit);

 m_vent_on_threshold_edit.DDX_Value(pDX, IDC_PD_TEMPER_VENT_ON_THRESHOLD_EDIT, m_params.vent_on);
 m_vent_off_threshold_edit.DDX_Value(pDX, IDC_PD_TEMPER_VENT_OFF_THRESHOLD_EDIT, m_params.vent_off);
 m_vent_pwmfrq_edit.DDX_Value(pDX, IDC_PD_TEMPER_PWM_FRQ_EDIT, m_params.vent_pwmfrq);
 m_cond_pvt_on_edit.DDX_Value(pDX, IDC_PD_TEMPER_CONDPVTON_EDIT, m_params.cond_pvt_on);
 m_cond_pvt_off_edit.DDX_Value(pDX, IDC_PD_TEMPER_CONDPVTOFF_EDIT, m_params.cond_pvt_off);
 m_cond_min_rpm_edit.DDX_Value(pDX, IDC_PD_TEMPER_CONDMINRPM_EDIT, m_params.cond_min_rpm);
 m_vent_timer_edit.DDX_Value(pDX, IDC_PD_TEMPER_VENTTIMER_EDIT, m_params.vent_tmr);
 DDX_Check_bool(pDX, IDC_PD_TEMPER_USE_TEMP_SENSOR, m_params.tmp_use);
 DDX_Check_bool(pDX, IDC_PD_TEMPER_USE_VENT_PWM, m_params.vent_pwm);
 DDX_Check_bool(pDX, IDC_PD_TEMPER_USE_CURVE_MAP, m_params.cts_use_map);
}

void CTemperPageDlg::OnUpdateControls(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled);
}

void CTemperPageDlg::OnUpdateVentOff(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled && (!m_use_vent_pwm_enabled || m_use_vent_pwm.GetCheck()!=BST_CHECKED));
}

void CTemperPageDlg::OnUpdateUseVentPwm(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled && m_use_vent_pwm_enabled);
}

void CTemperPageDlg::OnUpdateUseCurveMap(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled && m_use_curve_map_enabled);
}

void CTemperPageDlg::OnUpdateControlsSECU3iFI(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled && !m_enable_secu3t_features && m_fuel_injection);
}

void CTemperPageDlg::OnUpdateFuelInjectionControls(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled && m_fuel_injection);
}

/////////////////////////////////////////////////////////////////////////////
// CTemperPageDlg message handlers

BOOL CTemperPageDlg::OnInitDialog()
{
 Super::OnInitDialog();

 m_vent_on_threshold_spin.SetBuddy(&m_vent_on_threshold_edit);
 m_vent_on_threshold_edit.SetLimitText(4);
 m_vent_on_threshold_edit.SetDecimalPlaces(2);
 m_vent_on_threshold_spin.SetRangeAndDelta(-10.0f,125.0f,0.2f);
 m_vent_on_threshold_edit.SetRange(-10.0f,125.0f);

 m_vent_off_threshold_spin.SetBuddy(&m_vent_off_threshold_edit);
 m_vent_off_threshold_edit.SetLimitText(4);
 m_vent_off_threshold_edit.SetDecimalPlaces(2);
 m_vent_off_threshold_spin.SetRangeAndDelta(-10.0f,125.0f,0.2f);
 m_vent_off_threshold_edit.SetRange(-10.0f,125.0f);

 m_vent_pwmfrq_spin.SetBuddy(&m_vent_pwmfrq_edit);
 m_vent_pwmfrq_edit.SetLimitText(4);
 m_vent_pwmfrq_edit.SetDecimalPlaces(4);
 m_vent_pwmfrq_spin.SetRangeAndDelta(10, 5000, 1);
 m_vent_pwmfrq_edit.SetRange(10, 5000);

 m_cond_pvt_on_spin.SetBuddy(&m_cond_pvt_on_edit);
 m_cond_pvt_on_edit.SetLimitText(4);
 m_cond_pvt_on_edit.SetDecimalPlaces(4);
 m_cond_pvt_on_spin.SetRangeAndDelta(.0, 5.0, 0.01);
 m_cond_pvt_on_edit.SetRange(.0, 5.0);

 m_cond_pvt_off_spin.SetBuddy(&m_cond_pvt_off_edit);
 m_cond_pvt_off_edit.SetLimitText(4);
 m_cond_pvt_off_edit.SetDecimalPlaces(4);
 m_cond_pvt_off_spin.SetRangeAndDelta(.0, 5.0, 0.01);
 m_cond_pvt_off_edit.SetRange(.0, 5.0);

 m_cond_min_rpm_spin.SetBuddy(&m_cond_min_rpm_edit);
 m_cond_min_rpm_edit.SetLimitText(4);
 m_cond_min_rpm_edit.SetDecimalPlaces(0);
 m_cond_min_rpm_spin.SetRangeAndDelta(500, 3000, 10);
 m_cond_min_rpm_edit.SetRange(500, 3000);

 m_vent_timer_spin.SetBuddy(&m_vent_timer_edit);
 m_vent_timer_edit.SetLimitText(3);
 m_vent_timer_edit.SetDecimalPlaces(0);
 m_vent_timer_spin.SetRangeAndDelta(.0, 600.0, 1.0);
 m_vent_timer_edit.SetRange(.0, 600.0);

 UpdateData(FALSE);

 //create tooltip control
 mp_ttc.reset(new CToolTipCtrlEx());
 VERIFY(mp_ttc->Create(this, WS_POPUP | TTS_ALWAYSTIP | TTS_BALLOON));
 //Set tooltips for widgets
 //Turn on threshold
 VERIFY(mp_ttc->AddWindow(&m_vent_on_threshold_edit, MLL::GetString(IDS_PD_TEMPER_VENT_ON_THRESHOLD_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_vent_on_threshold_spin, MLL::GetString(IDS_PD_TEMPER_VENT_ON_THRESHOLD_EDIT_TT)));
 //turn off threshold
 VERIFY(mp_ttc->AddWindow(&m_vent_off_threshold_edit, MLL::GetString(IDS_PD_TEMPER_VENT_OFF_THRESHOLD_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_vent_off_threshold_spin, MLL::GetString(IDS_PD_TEMPER_VENT_OFF_THRESHOLD_EDIT_TT)));

 VERIFY(mp_ttc->AddWindow(&m_use_curve_map, MLL::GetString(IDS_PD_TEMPER_USE_CURVE_MAP_TT)));
 VERIFY(mp_ttc->AddWindow(&m_use_vent_pwm, MLL::GetString(IDS_PD_TEMPER_USE_VENT_PWM_TT)));
 VERIFY(mp_ttc->AddWindow(&m_use_temp_sensor, MLL::GetString(IDS_PD_TEMPER_USE_TEMP_SENSOR_TT)));

 VERIFY(mp_ttc->AddWindow(&m_vent_timer_edit, MLL::GetString(IDS_PD_TEMPER_VENTTIMER_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_vent_timer_spin, MLL::GetString(IDS_PD_TEMPER_VENTTIMER_EDIT_TT)));

 VERIFY(mp_ttc->AddWindow(&m_cond_min_rpm_edit, MLL::GetString(IDS_PD_TEMPER_CONDMINRPM_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_cond_min_rpm_spin, MLL::GetString(IDS_PD_TEMPER_CONDMINRPM_EDIT_TT)));

 VERIFY(mp_ttc->AddWindow(&m_cond_pvt_on_edit, MLL::GetString(IDS_PD_TEMPER_CONDPVTON_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_cond_pvt_on_spin, MLL::GetString(IDS_PD_TEMPER_CONDPVTON_EDIT_TT)));

 VERIFY(mp_ttc->AddWindow(&m_cond_pvt_off_edit, MLL::GetString(IDS_PD_TEMPER_CONDPVTOFF_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_cond_pvt_off_spin, MLL::GetString(IDS_PD_TEMPER_CONDPVTOFF_EDIT_TT)));

 VERIFY(mp_ttc->AddWindow(&m_vent_pwmfrq_edit, MLL::GetString(IDS_PD_TEMPER_PWM_FRQ_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_vent_pwmfrq_spin, MLL::GetString(IDS_PD_TEMPER_PWM_FRQ_EDIT_TT)));

 mp_ttc->SetMaxTipWidth(250); //Set text wrapping width
 mp_ttc->ActivateToolTips(true);

 //initialize window scroller
 mp_scr->Init(this);

 UpdateDialogControls(this, TRUE);
 return TRUE;  // return TRUE unless you set the focus to a control
}

void CTemperPageDlg::OnDestroy()
{
 Super::OnDestroy();
 mp_scr->Close();
}

void CTemperPageDlg::OnChangePdTemperVentOnThresholdEdit()
{
 UpdateData();
 if (m_use_vent_pwm.GetCheck()!=BST_CHECKED)
 {
  //Turn on temperature can not be less than turn off temperature
  if (m_params.vent_on < m_params.vent_off + TEMP_HYSTERESIS)
   m_vent_off_threshold_spin.SetPos(m_params.vent_on - TEMP_HYSTERESIS);
 }
 OnChangeNotify(); //notify event receiver about change of view content(see class ParamPageEvents)
}

void CTemperPageDlg::OnChangePdTemperVentOffThresholdEdit()
{
 UpdateData();
 //Turn off temperature can not be above than turn on temperature
 if (m_params.vent_off > m_params.vent_on - TEMP_HYSTERESIS)
  m_vent_on_threshold_spin.SetPos(m_params.vent_off + TEMP_HYSTERESIS);
 OnChangeNotify();
}

void CTemperPageDlg::OnChangeData()
{
 UpdateData();
 OnChangeNotify(); //notify event receiver about change in view content(see class ParamPageEvents)
}

void CTemperPageDlg::OnPdTemperUseTempSensor()
{
 UpdateData();
 OnChangeNotify();
}

void CTemperPageDlg::OnPdTemperUseVentPwm()
{
 UpdateData();
 OnChangeNotify();
 UpdateDialogControls(this,TRUE);
 //Turn on temperature can not be less than turn off temperature
 if (m_params.vent_on < m_params.vent_off + TEMP_HYSTERESIS)
  m_vent_off_threshold_spin.SetPos(m_params.vent_on - TEMP_HYSTERESIS);
}

void CTemperPageDlg::OnPdTemperUseCurveMap()
{
 UpdateData();
 OnChangeNotify();
 UpdateDialogControls(this,TRUE);
}

//Enable/disable all controls
void CTemperPageDlg::Enable(bool enable)
{
 if (m_enabled == enable)
  return; //already has needed state
 m_enabled = enable;
 if (::IsWindow(m_hWnd))
  UpdateDialogControls(this, TRUE);
}

//get state of controls (enabled/disabled)
bool CTemperPageDlg::IsEnabled(void)
{
 return m_enabled;
}

//Use this function to obtain data from a dialog
void CTemperPageDlg::GetValues(SECU3IO::TemperPar* o_values)
{
 ASSERT(o_values);
 UpdateData(TRUE); //copy data from dialog to variables
 memcpy(o_values,&m_params, sizeof(SECU3IO::TemperPar));
}

//Use this function for getting data from a dialog
void CTemperPageDlg::SetValues(const SECU3IO::TemperPar* i_values)
{
 ASSERT(i_values);
 memcpy(&m_params,i_values, sizeof(SECU3IO::TemperPar));
 UpdateData(FALSE); //copy data from variables to dialog
}

void CTemperPageDlg::EnableUseVentPwm(bool enable)
{
 if (m_use_vent_pwm_enabled == enable)
  return; //already has needed state
 m_use_vent_pwm_enabled = enable;
 if (::IsWindow(this->m_hWnd))
  UpdateDialogControls(this, TRUE);
}

void CTemperPageDlg::EnableUseCTSCurveMap(bool enable)
{
 if (m_use_curve_map_enabled == enable)
  return; //already has needed state
 m_use_curve_map_enabled = enable;
 if (::IsWindow(this->m_hWnd))
  UpdateDialogControls(this, TRUE);
}

void CTemperPageDlg::EnableSECU3TItems(bool i_enable)
{
 if (m_enable_secu3t_features == i_enable)
  return; //already has needed state
 m_enable_secu3t_features = i_enable;
 if (::IsWindow(m_hWnd))
  UpdateDialogControls(this, TRUE);
}

void CTemperPageDlg::EnableFuelInjection(bool i_enable)
{
 if (m_fuel_injection == i_enable)
  return; //already has needed state
 m_fuel_injection = i_enable;
 if (::IsWindow(this->m_hWnd))
  UpdateDialogControls(this, TRUE);
}

void CTemperPageDlg::OnSize( UINT nType, int cx, int cy )
{
 Super::OnSize(nType, cx, cy);

 DPIAware da;
 if (mp_scr.get())
  mp_scr->SetViewSize(cx, da.ScaleY(400));
}
