
#pragma once

#include "ui-core/TabDialog.h"
#include "resource.h"
#include "ui-core/SpinButtonCtrlEx.h"
#include "ui-core/EditEx.h"
#include "io-core/SECU3IO.h"

#include "ParamPageEvents.h"

class CIdlRegPageDlg : public CTabDialog, public ParamPageEvents
{
// Construction
public:
	CIdlRegPageDlg(CWnd* pParent = NULL);   // standard constructor

	virtual LPCTSTR GetDialogID(void) const;
    void Enable(bool enable);
	bool IsEnabled(void);

	void GetValues(SECU3IO::IdlRegPar* o_values);
    void SetValues(const SECU3IO::IdlRegPar* i_values);

// Dialog Data
	//{{AFX_DATA(CIdlRegPageDlg)
	enum { IDD = IDD_PD_IDLREG_PAGE };
	CButton	m_use_regulator;
	CSpinButtonCtrlEx	m_goal_rpm_spin;
	CEditEx	m_goal_rpm_edit;
	CSpinButtonCtrlEx	m_factor_pos_spin;
	CEditEx	m_factor_pos_edit;
	CSpinButtonCtrlEx	m_factor_neg_spin;
	CEditEx	m_factor_neg_edit;
	CSpinButtonCtrlEx	m_dead_band_rpm_spin;
	CEditEx	m_dead_band_rpm_edit;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIdlRegPageDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	SECU3IO::IdlRegPar m_params;
	BOOL m_enabled;

	// Generated message map functions
	//{{AFX_MSG(CIdlRegPageDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangePdIdlregDeadBandRpmEdit();
	afx_msg void OnChangePdIdlregFactorNegEdit();
	afx_msg void OnChangePdIdlregFactorPosEdit();
	afx_msg void OnChangePdIdlregGoalRpmEdit();
	afx_msg void OnPdIdlregUseRegulator();

	afx_msg void OnUpdateControls(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};



