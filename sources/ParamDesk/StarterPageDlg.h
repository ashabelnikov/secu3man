
#pragma once

#include <string>

#include "ui-core/TabDialog.h"
#include "resource.h"
#include "ui-core/SpinButtonCtrlEx.h"
#include "ui-core/EditEx.h"
#include "io-core/SECU3IO.h"

#include "ParamPageEvents.h"

class CStarterPageDlg : public CTabDialog, public ParamPageEvents
{
// Construction
public:
	CStarterPageDlg(CWnd* pParent = NULL);   // standard constructor

	virtual LPCTSTR GetDialogID(void) const;
    void Enable(bool enable);
	bool IsEnabled(void);

	void GetValues(SECU3IO::StartrPar* o_values);
    void SetValues(const SECU3IO::StartrPar* i_values);


// Dialog Data
	//{{AFX_DATA(CStarterPageDlg)
	enum { IDD = IDD_PD_STARTER_PAGE };
	CSpinButtonCtrlEx	m_smap_abandon_rpm_spin;
	CSpinButtonCtrlEx	m_starter_off_rpm_spin;
	CEditEx	m_smap_abandon_rpm_edit;
	CEditEx	m_starter_off_rpm_edit;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStarterPageDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	SECU3IO::StartrPar m_params;
	BOOL m_enabled;


	// Generated message map functions
	//{{AFX_MSG(CStarterPageDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangePdStarterOffRpmEdit();
	afx_msg void OnChangePdStarterSmapAbandonRpmEdit();

	afx_msg void OnUpdateControls(CCmdUI* pCmdUI);

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


