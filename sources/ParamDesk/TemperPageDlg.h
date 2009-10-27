
#pragma once

#include "io-core/SECU3IO.h"
#include "ParamPageEvents.h"
#include "ui-core/EditEx.h"
#include "ui-core/SpinButtonCtrlEx.h"
#include "ui-core/TabDialog.h"

class CTemperPageDlg : public CTabDialog, public ParamPageEvents
{
// Construction
public:
	CTemperPageDlg(CWnd* pParent = NULL);   // standard constructor

	virtual LPCTSTR GetDialogID(void) const;
    void Enable(bool enable);
	bool IsEnabled(void);

	void GetValues(SECU3IO::TemperPar* o_values);
    void SetValues(const SECU3IO::TemperPar* i_values);

	static const UINT IDD;

	CButton	m_use_temp_sensor;
	CSpinButtonCtrlEx	m_vent_on_threshold_spin;
	CSpinButtonCtrlEx	m_vent_off_threshold_spin;
	CEditEx	m_vent_off_threshold_edit;
	CEditEx	m_vent_on_threshold_edit;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTemperPageDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	SECU3IO::TemperPar m_params;
	BOOL m_enabled;

	// Generated message map functions
	//{{AFX_MSG(CTemperPageDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangePdTemperVentOnThresholdEdit();
	afx_msg void OnChangePdTemperVentOffThresholdEdit();
	afx_msg void OnPdTemperUseTempSensor();

	afx_msg void OnUpdateControls(CCmdUI* pCmdUI);

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};



