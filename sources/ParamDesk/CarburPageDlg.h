
#pragma once

#include "io-core/SECU3IO.h"
#include "ParamPageEvents.h"
#include "ui-core/TabDialog.h"
#include "ui-core/SpinButtonCtrlEx.h"
#include "ui-core/EditEx.h"

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

	static const UINT IDD;

	CButton	m_inverse_throttle_switch;
	CSpinButtonCtrlEx	m_shutoff_lo_threshold_spin;
	CSpinButtonCtrlEx	m_shutoff_hi_threshold_spin;
	CSpinButtonCtrlEx	m_epm_on_threshold_spin;
	CSpinButtonCtrlEx	m_shutoff_lo_threshold_spin_g;
	CSpinButtonCtrlEx	m_shutoff_hi_threshold_spin_g;
	CSpinButtonCtrlEx	m_shutoff_delay_spin;
	CEditEx	m_shutoff_lo_threshold_edit;
	CEditEx	m_shutoff_hi_threshold_edit;
	CEditEx	m_epm_on_threshold_edit;
	CEditEx	m_shutoff_lo_threshold_edit_g;
	CEditEx	m_shutoff_hi_threshold_edit_g;
	CEditEx	m_shutoff_delay_edit;	

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
	afx_msg void OnChangeData();
	afx_msg void OnUpdateControls(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};



