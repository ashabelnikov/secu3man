
#include "ui-core/TabDialog.h"
#include "MIDesk/MIDeskDlg.h"
#include "ParamDesk/ParamDeskDlg.h"

#pragma once

/////////////////////////////////////////////////////////////////////////////
// CParamMonModePageDlg dialog

class CParamMonTabDlg : public CTabDialog
{
// Construction
public:
	CParamMonTabDlg(CWnd* pParent = NULL);   // standard constructor   

public:

	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	CMIDeskDlg m_MIDeskDlg;
	CParamDeskDlg m_ParamDeskDlg;
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

// Dialog Data
	//{{AFX_DATA(CParamMonTabDlg)
	enum { IDD = IDD_PARAMETERS_AND_MONITOR };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

   virtual LPCTSTR GetDialogID(void) const;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CParamMonTabDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CParamMonTabDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

