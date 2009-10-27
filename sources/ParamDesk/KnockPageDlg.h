
#pragma once

#include "io-core/SECU3IO.h"
#include "ParamPageEvents.h"
#include "propgrid/custom/CustValSelector.h"
#include "propgrid/propertygrid.h"
#include "ui-core/TabDialog.h"

class AFX_EXT_CLASS CKnockPageDlg : public CTabDialog, public ParamPageEvents
{
    typedef CTabDialog Super;
// Construction
public:
	CKnockPageDlg(CWnd* pParent = NULL);   // standard constructor

	virtual LPCTSTR GetDialogID(void) const;
    void Enable(bool enable);
	bool IsEnabled(void);

	void GetValues(SECU3IO::KnockPar* o_values);
    void SetValues(const SECU3IO::KnockPar* i_values);

	static const UINT IDD;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CKnockPageDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	SECU3IO::KnockPar m_params;
	BOOL m_enabled;

	CPropertyGrid m_ctrlGrid;
	CCustValSelector m_freq_selector;

	HITEM m_knock_use_knock_channel_item;
	HITEM m_knock_k_wnd_begin_angle_item;
	HITEM m_knock_k_wnd_end_angle_item;
	HITEM m_knock_bpf_frequency_item;

	// Generated message map functions
	//{{AFX_MSG(CKnockPageDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeData();
	afx_msg void OnUpdateControls(CCmdUI* pCmdUI);
	afx_msg LRESULT OnItemChanged(WPARAM, LPARAM);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
