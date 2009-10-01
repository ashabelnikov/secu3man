
#pragma once

#include "resource.h"
#include "common/ObjectTimer.h"

#include "MITachometer.h"
#include "MIPressure.h"
#include "MIVoltmeter.h"
#include "MIDwellAngle.h"
#include "MIAirFlow.h"
#include "MIGasValve.h"
#include "MIShutoffValve.h"
#include "MIThrottleGate.h"
#include "MITemperature.h"

#include "io-core\SECU3IO.h"
#include "IMIView.h"

/////////////////////////////////////////////////////////////////////////////
// CMIDeskDlg dialog

class AFX_EXT_CLASS CMIDeskDlg : public CDialog, public IMIView
{
public:
	CMIDeskDlg(CWnd* pParent = NULL);   // standard constructor

	enum { IDD = IDD_MEAS_INSTRUMENT_DESK };

   //--------interface implementation---------------
	virtual void Show(bool show);
	virtual void Enable(bool enable);
    virtual void SetValues(const SECU3IO::SensorDat* i_values);
    virtual void GetValues(SECU3IO::SensorDat* o_values);
   //-----------------------------------------------

    //установка периода обновления в миллисекундах
    void SetUpdatePeriod(unsigned int i_period);

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
