
#pragma once

#include "ui-core/TabDialog.h"
#include "resource.h"
#include "ui-core/SpinButtonCtrlEx.h"
#include "ui-core/EditEx.h"
#include "io-core/SECU3IO.h"
#include "ParamPageEvents.h"

class CCarburPageDlg : public CTabDialog, public ParamPageEvents
{
// Construction
public:
	CCarburPageDlg(CWnd* pParent = NULL);   // standard constructor

	virtual LPCTSTR GetDialogID(void) const;
    void Enable(bool enable);
	bool IsEnabled(void);

	void GetValues(SECU3IO::CarburPar* o_values);
    void SetValues(const SECU3IO::CarburPar* i_values);

// Dialog Data
	//{{AFX_DATA(CCarburPageDlg)
	enum { IDD = IDD_PD_CARBUR_PAGE };
	CButton	m_inverse_throttle_switch;
	CSpinButtonCtrlEx	m_shutoff_lo_threshold_spin;
	CSpinButtonCtrlEx	m_shutoff_hi_threshold_spin;
	CEditEx	m_shutoff_lo_threshold_edit;
	CEditEx	m_shutoff_hi_threshold_edit;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CarburPageDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	SECU3IO::CarburPar m_params;
	BOOL m_enabled;

	// Generated message map functions
	//{{AFX_MSG(CCarburPageDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangePdCarburShutoffHiThresholdEdit();
	afx_msg void OnChangePdCarburShutoffLoThresholdEdit();
	afx_msg void OnPdCarburInverseSwitch();
	afx_msg void OnUpdateControls(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};



