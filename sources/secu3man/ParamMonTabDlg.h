
#include "ui-core/TabDialog.h"
#include "MIDesk/MIDeskDlg.h"
#include "MIDesk/RSDeskDlg.h"
#include "ParamDesk/ParamDeskDlg.h"

#pragma once

/////////////////////////////////////////////////////////////////////////////
// CParamMonModePageDlg dialog

class CParamMonTabDlg : public CTabDialog
{
// Construction
public:
    typedef fastdelegate::FastDelegate0<> EventHandler;

	CParamMonTabDlg(CWnd* pParent = NULL);   // standard constructor   

public:

	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	CMIDeskDlg m_MIDeskDlg;
	CRSDeskDlg m_RSDeskDlg;
	CParamDeskDlg m_ParamDeskDlg;
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	bool GetRawSensorsCheckState(void);
	void setOnRawSensorsCheck(EventHandler i_Function)
	{
	  m_OnRawSensorsCheck = i_Function;
	}

// Dialog Data
	//{{AFX_DATA(CParamMonTabDlg)
	enum { IDD = IDD_PARAMETERS_AND_MONITOR };
	CButton m_raw_sensors_check;
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
	afx_msg void OnPmShowRawSensors();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

    EventHandler m_OnRawSensorsCheck;
};

