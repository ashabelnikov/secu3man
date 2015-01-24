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

#pragma once

#include "common/ObjectTimer.h"
#include "IMIView.h"
#include "io-core/SECU3IO.h"

#include "MIAirFlow.h"
#include "MIDwellAngle.h"
#include "MIEGOCorr.h"
#include "MIGasValve.h"
#include "MIIAT.h"
#include "MIInjPW.h"
#include "MIPressure.h"
#include "MIShutoffValve.h"
#include "MITachometer.h"
#include "MITemperature.h"
#include "MIThrottleGate.h"
#include "MIVoltage.h"
#include "MIVoltmeter.h"

#include "ui-core/DialogWithAccelerators.h"

/////////////////////////////////////////////////////////////////////////////
// CMIDeskDlg dialog

class AFX_EXT_CLASS CMIDeskDlg : public CModelessDialog, public IMIView
{
  typedef CModelessDialog Super;

 public:
  CMIDeskDlg(CWnd* pParent = NULL);   // standard constructor
  static const UINT IDD;

  //--------interface implementation---------------
  virtual void Show(bool show, bool show_exf = false);
  virtual void Enable(bool enable);
  virtual void SetValues(const SECU3IO::SensorDat* i_values);
  //-----------------------------------------------

  //установка периода обновления в миллисекундах
  void SetUpdatePeriod(unsigned int i_period);

  //Set maximum value of tachometer displayed on the fixture
  void SetTachometerMax(int i_max);

  //Set maximum value of pressure meter displayed on the fixture
  void SetPressureMax(int i_max);

  //изменение размеров окна
  void Resize(const CRect& i_rect, const CRect& i_src = CRect(0,0,0,0));

  //Show/hide extended fixtures
  void ShowExFixtures(bool i_show);

  //Show/hide choke position indicator
  void ShowChokePos(bool i_show);

  //Show/hide speed and distance indicators
  void ShowSpeedAndDistance(bool i_show);

 protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  virtual BOOL OnInitDialog(); //activate
  afx_msg void OnDestroy();    //deactivate
  DECLARE_MESSAGE_MAP()

  void OnUpdateTimer(void);

  // Implementation
 private:
  CMITachometer   m_tachometer;
  CMIPressure     m_pressure;
  CMIVoltmeter    m_voltmeter;
  CMIDwellAngle   m_dwell_angle;
  CMITemperature  m_temperature;
  CMIVoltage      m_add_i1;
  CMIVoltage      m_add_i2;
  CMIInjPW        m_inj_pw;
  CMIIAT          m_iat;
  CMIEGOCorr      m_ego_corr;

  CMIAirFlow      m_air_flow;
  CMIGasValve     m_gas_valve;
  CMIThrottleGate m_throttle_gate;
  CMIShutoffValve m_shutoff_valve;
  CMIShutoffValve m_power_valve;

  SECU3IO::SensorDat m_values;
  CObjectTimer<CMIDeskDlg> m_update_timer;
  unsigned int m_update_period;
  bool m_was_initialized;
  int m_enabled;
};

/////////////////////////////////////////////////////////////////////////////
