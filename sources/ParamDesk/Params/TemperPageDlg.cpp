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

#include "stdafx.h"
#include "Resources/resource.h"
#include "TemperPageDlg.h"
#include "ui-core/ddx_helpers.h"
#include "ui-core/ToolTipCtrlEx.h"


const UINT CTemperPageDlg::IDD = IDD_PD_TEMPER_PAGE;
const float TEMP_HYSTERESIS = 0.25f;

BEGIN_MESSAGE_MAP(CTemperPageDlg, Super)
 ON_EN_CHANGE(IDC_PD_TEMPER_VENT_ON_THRESHOLD_EDIT, OnChangePdTemperVentOnThresholdEdit)
 ON_EN_CHANGE(IDC_PD_TEMPER_VENT_OFF_THRESHOLD_EDIT, OnChangePdTemperVentOffThresholdEdit)
 ON_EN_CHANGE(IDC_PD_TEMPER_PWM_FRQ_EDIT, OnChangeData)
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
END_MESSAGE_MAP()

CTemperPageDlg::CTemperPageDlg(CWnd* pParent /*=NULL*/)
: Super(CTemperPageDlg::IDD, pParent)
, m_enabled(false)
, m_use_vent_pwm_enabled(false)
, m_use_curve_map_enabled(false)
, m_vent_on_threshold_edit(CEditEx::MODE_FLOAT | CEditEx::MODE_SIGNED, true)
, m_vent_off_threshold_edit(CEditEx::MODE_FLOAT | CEditEx::MODE_SIGNED, true)
, m_vent_pwmfrq_edit(CEditEx::MODE_INT, true)
{
 m_params.vent_on = 95.0f;
 m_params.vent_off = 98.0f;
 m_params.tmp_use = 1;
 m_params.vent_pwm = 0;
 m_params.cts_use_map = 0;
 m_params.vent_pwmfrq = 5000;
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

 m_vent_on_threshold_edit.DDX_Value(pDX, IDC_PD_TEMPER_VENT_ON_THRESHOLD_EDIT, m_params.vent_on);
 m_vent_off_threshold_edit.DDX_Value(pDX, IDC_PD_TEMPER_VENT_OFF_THRESHOLD_EDIT, m_params.vent_off);
 m_vent_pwmfrq_edit.DDX_Value(pDX, IDC_PD_TEMPER_PWM_FRQ_EDIT, m_params.vent_pwmfrq);
 DDX_Check_UCHAR(pDX, IDC_PD_TEMPER_USE_TEMP_SENSOR, m_params.tmp_use);
 DDX_Check_UCHAR(pDX, IDC_PD_TEMPER_USE_VENT_PWM, m_params.vent_pwm);
 DDX_Check_UCHAR(pDX, IDC_PD_TEMPER_USE_CURVE_MAP, m_params.cts_use_map);
}

//если надо апдейтить отдельные контроллы, то надо будет плодить функции
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

 UpdateData(FALSE);
 //create a tooltip control and assign tooltips
 mp_ttc.reset(new CToolTipCtrlEx());
 VERIFY(mp_ttc->Create(this, WS_POPUP | TTS_ALWAYSTIP | TTS_BALLOON));
 VERIFY(mp_ttc->AddWindow(&m_use_temp_sensor, MLL::GetString(IDS_PD_TEMPER_USE_TEMP_SENSOR_TT)));
 VERIFY(mp_ttc->AddWindow(&m_use_vent_pwm, MLL::GetString(IDS_PD_TEMPER_USE_VENT_PWM_TT)));
 VERIFY(mp_ttc->AddWindow(&m_use_curve_map, MLL::GetString(IDS_PD_TEMPER_USE_CURVE_MAP_TT)));
 VERIFY(mp_ttc->AddWindow(&m_vent_on_threshold_spin, MLL::GetString(IDS_PD_TEMPER_VENT_ON_THRESHOLD_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_vent_off_threshold_spin, MLL::GetString(IDS_PD_TEMPER_VENT_OFF_THRESHOLD_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_vent_off_threshold_edit, MLL::GetString(IDS_PD_TEMPER_VENT_OFF_THRESHOLD_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_vent_on_threshold_edit, MLL::GetString(IDS_PD_TEMPER_VENT_ON_THRESHOLD_EDIT_TT)));

 mp_ttc->SetMaxTipWidth(250); //Enable text wrapping
 mp_ttc->ActivateToolTips(true);
 UpdateDialogControls(this, TRUE);
 return TRUE;  // return TRUE unless you set the focus to a control
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

//разрешение/запрещение контроллов (всех поголовно)
void CTemperPageDlg::Enable(bool enable)
{
 if (m_enabled == enable)
  return; //already has needed state
 m_enabled = enable;
 if (::IsWindow(m_hWnd))
  UpdateDialogControls(this, TRUE);
}

//что с контроллами?
bool CTemperPageDlg::IsEnabled(void)
{
 return m_enabled;
}

//эту функцию необходимо использовать когда надо получить данные из диалога
void CTemperPageDlg::GetValues(SECU3IO::TemperPar* o_values)
{
 ASSERT(o_values);
 UpdateData(TRUE); //копируем данные из диалога в переменные
 memcpy(o_values,&m_params, sizeof(SECU3IO::TemperPar));
}

//эту функцию необходимо использовать когда надо занести данные в диалог
void CTemperPageDlg::SetValues(const SECU3IO::TemperPar* i_values)
{
 ASSERT(i_values);
 memcpy(&m_params,i_values, sizeof(SECU3IO::TemperPar));
 UpdateData(FALSE); //копируем данные из переменных в диалог
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
