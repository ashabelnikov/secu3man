
#pragma once

#include "ui-core/TabDialog.h"
#include "resource.h"

#include "ui-core/SpinButtonCtrlEx.h"
#include "ui-core/EditEx.h"
#include "io-core/SECU3IO.h"
#include "ParamPageEvents.h"


class CAnglesPageDlg : public CTabDialog, public ParamPageEvents
{
// Construction
public:
	CAnglesPageDlg(CWnd* pParent = NULL);   // standard constructor

	virtual LPCTSTR GetDialogID(void) const;
    void Enable(bool enable);
	bool IsEnabled(void);

	void GetValues(SECU3IO::AnglesPar* o_values);
    void SetValues(const SECU3IO::AnglesPar* i_values);

// Dialog Data
	//{{AFX_DATA(CAnglesPageDlg)
	enum { IDD = IDD_PD_ANGLES_PAGE };
	CSpinButtonCtrlEx	m_min_angle_spin;
	CSpinButtonCtrlEx	m_max_angle_spin;
	CSpinButtonCtrlEx	m_correction_spin;
	CSpinButtonCtrlEx	m_decrease_spead_spin;
	CSpinButtonCtrlEx	m_increase_spead_spin;
	CEditEx	m_min_angle_edit;
	CEditEx	m_max_angle_edit;
	CEditEx	m_correction_edit;
	CEditEx	m_decrease_spead_edit;
	CEditEx	m_increase_spead_edit;
	
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAnglesPageDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	SECU3IO::AnglesPar m_params;
    BOOL m_enabled;
	
	// Generated message map functions
	//{{AFX_MSG(CAnglesPageDlg)
	afx_msg void OnChangeData();
	virtual BOOL OnInitDialog();
	afx_msg void OnUpdateControls(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};



