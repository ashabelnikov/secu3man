
#include "ui-core/TabDialog.h"
#include "resource.h"
#include "propgrid/propertygrid.h"

#pragma once

class CKnockChannelTabDlg : public CTabDialog
{
// Construction
public:
	CKnockChannelTabDlg(CWnd* pParent = NULL);   // standard constructor   

public:

// Dialog Data
	//{{AFX_DATA(CKnockChannelTabDlg)
	enum { IDD = IDD_KNOCK_CHANNEL };
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
	CPropertyGrid m_ctrlGrid;

	// Generated message map functions
	//{{AFX_MSG(CKnockChannelTabDlg)
	virtual BOOL OnInitDialog();	
	afx_msg void OnClose();
	afx_msg LRESULT OnItemChanged(WPARAM, LPARAM);
	afx_msg void OnUpdatePropertyGrid(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};