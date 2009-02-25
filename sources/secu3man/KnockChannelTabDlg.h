
#include "ui-core/TabDialog.h"
#include "resource.h"
#include "common\fastdelegate.h"
#include "HiSCCtrl/sources/ChartCtrl.h"
#include "ParamDesk/KnockPageDlg.h"

#pragma once

#define NUMBER 100


class CKnockChannelTabDlg : public CTabDialog
{
	typedef CTabDialog Super;
public:
	typedef fastdelegate::FastDelegate0<> EventHandler;


// Construction
public:
	CKnockChannelTabDlg(CWnd* pParent = NULL);   // standard constructor  
	
	void setOnSaveParameters(EventHandler OnFunction) {m_OnSaveParameters = OnFunction;}

	void EnableAll(bool i_enable);

	void AppendPoint(float value); 

public:

// Dialog Data
	//{{AFX_DATA(CKnockChannelTabDlg)
	enum { IDD = IDD_KNOCK_CHANNEL };
	CButton m_param_save_button;
	CChartCtrl* mp_RTChart;
	CKnockPageDlg m_knock_parameters_dlg;
	//}}AFX_DATA

   virtual LPCTSTR GetDialogID(void) const;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CKnockChannelTabDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:	
	// Generated message map functions
	//{{AFX_MSG(CKnockChannelTabDlg)
	virtual BOOL OnInitDialog();		
	afx_msg void OnDestroy();
	afx_msg void OnSaveParameters();
	afx_msg void OnUpdateControls(CCmdUI* pCmdUI);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
    EventHandler  m_OnSaveParameters;  
	bool m_all_enabled;

	double YValues[NUMBER];
	double XValues[NUMBER];
};