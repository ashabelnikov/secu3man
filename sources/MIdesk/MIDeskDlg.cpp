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
#include "resource.h"
#include "MIDeskDlg.h"

#include "common/MathHelpers.h"
#include "MIHelpers.h"
#include "ui-core/AnalogMeterCtrl.h"

BEGIN_MESSAGE_MAP(CMIDeskDlg, Super)
 ON_WM_DESTROY()
END_MESSAGE_MAP()

const UINT CMIDeskDlg::IDD = IDD_MEAS_INSTRUMENT_DESK;

/////////////////////////////////////////////////////////////////////////////
// CMIDeskDlg dialog

CMIDeskDlg::CMIDeskDlg(CWnd* pParent /*=NULL*/)
: Super(CMIDeskDlg::IDD, pParent)
, m_update_period(100)
, m_was_initialized(false)
, m_enabled(-1)
, m_add_i1(MLL::GetString(IDS_MI_ADD_I1_TT))
, m_add_i2(MLL::GetString(IDS_MI_ADD_I2_TT))
, m_show_exf(false)
, m_show(false)
{
 memset(&m_values, 0, sizeof(SECU3IO::SensorDat));
}

void CMIDeskDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);

 //Аналоговые приборы
 m_tachometer.DDX_Controls(pDX, IDC_MI_TACHOMETER);
 m_pressure.DDX_Controls(pDX, IDC_MI_MAP);
 m_voltmeter.DDX_Controls(pDX, IDC_MI_VOLTMETER);
 m_dwell_angle.DDX_Controls(pDX, IDC_MI_DWELL_ANGLE);
 m_temperature.DDX_Controls(pDX, IDC_MI_TEMPERATURE);
 m_add_i1.DDX_Controls(pDX, IDC_MI_ADD_I1);
 m_add_i2.DDX_Controls(pDX, IDC_MI_ADD_I2);
 m_iat.DDX_Controls(pDX, IDC_MI_IAT);
 m_inj_pw.DDX_Controls(pDX, IDC_MI_INJ_PW);
 m_ego_corr.DDX_Controls(pDX, IDC_MI_EGO_CORR);

 //Булевские приборы (лампочка)
 m_gas_valve.DDX_Controls(pDX,IDC_MI_GAS_VALVE,IDC_MI_GAS_VALVE_CAPTION);
 m_throttle_gate.DDX_Controls(pDX, IDC_MI_THROTTLE_GATE, IDC_MI_THROTTLE_GATE_CAPTION);
 m_shutoff_valve.DDX_Controls(pDX, IDC_MI_SHUTOFF_VALVE, IDC_MI_SHUTOFF_VALVE_CAPTION);
 m_power_valve.DDX_Controls(pDX, IDC_MI_POWER_VALVE, IDC_MI_POWER_VALVE_CAPTION);

 //расход воздуха
 m_air_flow.DDX_Controls(pDX, IDC_MI_AIR_FLOW, IDC_MI_AIR_FLOW_NUM, IDC_MI_AIR_FLOW_CAPTION);
}

/////////////////////////////////////////////////////////////////////////////
// CMIDeskDlg message handlers

BOOL CMIDeskDlg::OnInitDialog()
{
 Super::OnInitDialog();

 //создаем приборы (оконные образы)
 m_tachometer.Create();
 m_pressure.Create();
 m_voltmeter.Create();
 m_dwell_angle.Create();
 m_gas_valve.Create();
 m_shutoff_valve.Create();
 m_power_valve.Create();
 m_throttle_gate.Create();
 m_throttle_gate.SetLimits(0,100);
 m_air_flow.Create();
 m_temperature.Create();
 m_add_i1.Create();
 m_add_i2.Create();
 m_iat.Create();
 m_add_i1.SetTitle(MLL::GetString(IDS_MI_ADD_I1_TITLE));
 m_add_i2.SetTitle(MLL::GetString(IDS_MI_ADD_I2_TITLE));
 m_inj_pw.Create();
 m_ego_corr.Create();

 Enable(false);

 m_update_timer.SetTimer(this, &CMIDeskDlg::OnUpdateTimer, m_update_period);

 m_was_initialized = true;
 return TRUE;  // return TRUE unless you set the focus to a control
}

void CMIDeskDlg::OnDestroy()
{
 m_was_initialized = false;
 Super::OnDestroy();
 m_update_timer.KillTimer();
}

//разрешение/запрещение приборов
void CMIDeskDlg::Enable(bool enable)
{
 if (((int)enable) == m_enabled)
  return; //already has needed state
 m_enabled = enable;
 m_tachometer.Enable(enable);
 m_pressure.Enable(enable);
 m_voltmeter.Enable(enable);
 m_dwell_angle.Enable(enable);
 m_gas_valve.Enable(enable);
 m_shutoff_valve.Enable(enable);
 m_power_valve.Enable(enable);
 m_throttle_gate.Enable(enable);
 m_air_flow.Enable(enable);
 m_temperature.Enable(enable);
 m_add_i1.Enable(enable);
 m_add_i2.Enable(enable);
 m_iat.Enable(enable);
 m_inj_pw.Enable(enable);
 m_ego_corr.Enable(enable);
}

void CMIDeskDlg::Show(bool show, bool show_exf /*=false*/)
{
 m_show_exf = show_exf; //save flag
 m_show = show;
 m_tachometer.Show(show);
 m_pressure.Show(show);
 m_voltmeter.Show(show);
 m_dwell_angle.Show(show);
 m_gas_valve.Show(show);
 m_shutoff_valve.Show(show);
 m_power_valve.Show(show);
 m_throttle_gate.Show(show);
 m_air_flow.Show(show);
 m_temperature.Show(show);
 //extended fixtures
 m_add_i1.Show(show && show_exf);
 m_add_i2.Show(show && show_exf && !m_values.add_i2_mode);
 m_iat.Show(show && show_exf && m_values.add_i2_mode);
 m_inj_pw.Show(show && show_exf);
 m_ego_corr.Show(show && show_exf);
}

using namespace SECU3IO;

void CMIDeskDlg::SetValues(const SensorDat* i_values)
{
 if (m_values.add_i2_mode != i_values->add_i2_mode)
 {
  m_add_i2.Show(m_show && m_show_exf && !i_values->add_i2_mode);
  m_iat.Show(m_show && m_show_exf && i_values->add_i2_mode); 
 }
 m_values = *i_values;
}

void CMIDeskDlg::OnUpdateTimer(void)
{
 if (!m_was_initialized)
  return;
 m_tachometer.SetSpeed(m_values.speed);   //top-left pane
 m_tachometer.SetDistance(m_values.distance); //top-right pane
 m_tachometer.SetValue((float)m_values.frequen);
 m_pressure.SetValue(m_values.pressure);
 m_voltmeter.SetValue(m_values.voltage);
 m_dwell_angle.SetValue(m_values.adv_angle);
 m_gas_valve.SetValue(m_values.gas);
 m_shutoff_valve.SetValue(m_values.ephh_valve);
 m_power_valve.SetValue(m_values.epm_valve);
 m_throttle_gate.SetValue(m_values.carb);
 m_throttle_gate.SetPosition(m_values.tps);
 m_air_flow.SetValue(m_values.air_flow);
 m_temperature.SetChokePos(m_values.choke_pos); //top-right pane
 m_temperature.SetValue(m_values.temperat);
 m_add_i1.SetValue(m_values.add_i1);
 if (!m_values.add_i2_mode)
  m_add_i2.SetValue(m_values.add_i2);
 else
  m_iat.SetValue(m_values.air_temp);
 m_inj_pw.SetValue(m_values.inj_pw);
 m_ego_corr.SetValue(m_values.lambda_corr);
}

void CMIDeskDlg::SetUpdatePeriod(unsigned int i_period)
{
 m_update_period = i_period;
 if (m_was_initialized)
 {
  m_update_timer.KillTimer();
  m_update_timer.SetTimer(this,&CMIDeskDlg::OnUpdateTimer, m_update_period);
 }
}

void CMIDeskDlg::SetTachometerMax(int i_max)
{
 m_tachometer.SetLimits(0, (float)i_max);
}

void CMIDeskDlg::SetPressureMax(int i_max)
{
 m_pressure.SetLimits(10, (float)i_max);
}

void CMIDeskDlg::Resize(const CRect& i_rect, const CRect& i_src)
{
 //на основе предыдущего размера окна высчитываем коэффициенты масштабирования
 CRect old_rect;
 float Xf, Yf;
 if (i_src.IsRectNull())
  GetWindowRect(old_rect);
 else
  old_rect = i_src;
 MIHelpers::CalcRectToRectRatio(i_rect, old_rect, Xf, Yf);

 //изменяем размеры окна
 MoveWindow(i_rect.left, i_rect.top, i_rect.Width(), i_rect.Height());

 //ресайзим контроллы
 m_tachometer.Scale(Xf, Yf);
 m_pressure.Scale(Xf, Yf);
 m_voltmeter.Scale(Xf, Yf);
 m_dwell_angle.Scale(Xf, Yf);
 m_gas_valve.Scale(Xf, Yf);
 m_shutoff_valve.Scale(Xf, Yf);
 m_power_valve.Scale(Xf, Yf);
 m_throttle_gate.Scale(Xf, Yf);
 m_air_flow.Scale(Xf, Yf);
 m_temperature.Scale(Xf, Yf);
 m_add_i1.Scale(Xf, Yf);
 m_add_i2.Scale(Xf, Yf);
 m_iat.Scale(Xf, Yf);
 m_inj_pw.Scale(Xf, Yf);
 m_ego_corr.Scale(Xf, Yf);

 RedrawWindow();
}

void CMIDeskDlg::ShowExFixtures(bool i_show)
{
 m_show_exf = i_show; //save flag
 m_add_i1.Show(i_show);
 m_add_i2.Show(i_show && !m_values.add_i2_mode);
 m_iat.Show(i_show && m_values.add_i2_mode);
 m_inj_pw.Show(i_show);
 m_ego_corr.Show(i_show);
}

void CMIDeskDlg::ShowChokePos(bool i_show)
{
 m_temperature.ShowChokePos(i_show);
}

void CMIDeskDlg::ShowSpeedAndDistance(bool i_show)
{
 m_tachometer.ShowSpeed(i_show);
 m_tachometer.ShowDistance(i_show);
}
