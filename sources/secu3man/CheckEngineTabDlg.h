
#include "ui-core/TabDialog.h"
#include "resource.h"

#pragma once

class CCheckEngineTabDlg : public CTabDialog
{
// Construction
public:
	CCheckEngineTabDlg(CWnd* pParent = NULL);   // standard constructor   

public:

// Dialog Data
	//{{AFX_DATA(CCheckEngineTabDlg)
	enum { IDD = IDD_CHECK_ENGINE };
	CStatic m_quick_help_text;
	CListCtrl m_errors_list;
	CImageList m_image_list;
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
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
