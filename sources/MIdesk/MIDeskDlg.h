
#pragma once

#include "common\ObjectTimer.h"
#include "IMIView.h"
#include "io-core\SECU3IO.h"

#include "MIAirFlow.h"
#include "MIDwellAngle.h"
#include "MIGasValve.h"
#include "MIPressure.h"
#include "MIShutoffValve.h"
#include "MITachometer.h"
#include "MITemperature.h"
#include "MIThrottleGate.h"
#include "MIVoltmeter.h"

#include "ui-core\DialogWithAccelerators.h"

/////////////////////////////////////////////////////////////////////////////
// CMIDeskDlg dialog

class AFX_EXT_CLASS CMIDeskDlg : public CModelessDialog, public IMIView
{
  typedef CModelessDialog Super;

 public:
  CMIDeskDlg(CWnd* pParent = NULL);   // standard constructor
  static const UINT IDD;

  //--------interface implementation---------------
  virtual void Show(bool show);
  virtual void Enable(bool enable);
  virtual void SetValues(const SECU3IO::SensorDat* i_values);
  virtual void GetValues(SECU3IO::SensorDat* o_values);
  //-----------------------------------------------

  //установка периода обновления в миллисекундах
  void SetUpdatePeriod(unsigned int i_period);

  //изменение размеров окна
  void Resize(const CRect& i_rect);

 protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  virtual BOOL OnInitDialog(); //activate
  afx_msg void OnDestroy();    //deactivate
  DECLARE_MESSAGE_MAP()

  void OnUpdateTimer(void);

  // Implementation
 private:
  CMITachometer	m_tachometer;
  CMIPressure     m_pressure;
  CMIVoltmeter    m_voltmeter;
  CMIDwellAngle   m_dwell_angle;
  CMITemperature  m_temperature;

  CMIAirFlow      m_air_flow;
  CMIGasValve     m_gas_valve;
  CMIThrottleGate m_throttle_gate;
  CMIShutoffValve m_shutoff_valve;

  SECU3IO::SensorDat m_values;
  CObjectTimer<CMIDeskDlg> m_update_timer;
  unsigned int m_update_period;
  bool m_was_initialized;
};

/////////////////////////////////////////////////////////////////////////////
