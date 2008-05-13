
#pragma once

#include "resource.h"
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
// Construction
public:
	CMIDeskDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMIDeskDlg)
	enum { IDD = IDD_MEAS_INSTRUMENT_DESK };
	CMITachometer	m_tachometer;
	CMIPressure     m_pressure;
	CMIVoltmeter    m_voltmeter;
	CMIDwellAngle   m_dwell_angle;
    CMITemperature  m_temperature;

	CMIAirFlow      m_air_flow;
	CMIGasValve     m_gas_valve;
	CMIThrottleGate m_throttle_gate;
    CMIShutoffValve m_shutoff_valve;
	//}}AFX_DATA

   //--------interface implementation---------------
	virtual void Show(bool show);
	virtual void Enable(bool enable);
    virtual void SetValues(const SECU3IO::SensorDat* i_values);
    virtual void GetValues(SECU3IO::SensorDat* o_values);
   //-----------------------------------------------


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMIDeskDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMIDeskDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
