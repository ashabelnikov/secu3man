
#pragma once

#include "ui-core/TabDialog.h"
#include "resource.h"
#include "ui-core/SpinButtonCtrlEx.h"
#include "ui-core/EditEx.h"
#include "io-core/SECU3IO.h"

#include <vector>
#include "ParamPageEvents.h"
#include "common/unicodesupport.h"

class CCKPSPageDlg : public CTabDialog, public ParamPageEvents
{
// Construction
public:
	CCKPSPageDlg(CWnd* pParent = NULL);   // standard constructor

	virtual LPCTSTR GetDialogID(void) const;
    void Enable(bool enable);
	bool IsEnabled(void);

	void FillCBCogsBTDCByNumbers(void);
	
	void GetValues(SECU3IO::CKPSPar* o_values);
    void SetValues(const SECU3IO::CKPSPar* i_values);

// Dialog Data
	//{{AFX_DATA(CCKPSPageDlg)
	enum { IDD = IDD_PD_CKPS_PAGE };
	CStatic	    m_ckps_front_groupbox;
	CButton     m_ckps_posfront_radio;
	CButton     m_ckps_negfront_radio;
	
	CStatic     m_teeth_before_tdc_label;
	CComboBox	m_teeth_before_tdc_combo;

	CSpinButtonCtrlEx	m_ignition_cogs_spin;
	CEditEx	m_ignition_cogs_edit;
	CStatic m_ignition_cogs_label;

	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCKPSPageDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	SECU3IO::CKPSPar m_params;	
	BOOL m_enabled;

	const unsigned char m_teeth_before_tdc_start;

	// Generated message map functions
	//{{AFX_MSG(CCKPSPageDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangePdCogsBTDCCombo();
	afx_msg void OnChangePdIgnitionCogsEdit();
	afx_msg void OnClickedPdFrontRadio();

	afx_msg void OnUpdateControls(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


