
#pragma once

#include "resource.h"

class CMapImpExpDlg : public CDialog
{
// Construction
public:
	CMapImpExpDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMapImpExpDlg)
	enum { IDD = IDD_MAP_IMPEXP };
	CListCtrl	m_other_fwd_list;
	CListCtrl	m_current_fwd_list;
	CString	m_current_fwd_title_string;
	CString	m_other_fwd_title_string;
	BOOL	m_idle_map_flag;
	BOOL	m_start_map_flag;
	BOOL	m_temp_map_flag;
	BOOL	m_work_map_flag;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMapImpExpDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMapImpExpDlg)
	afx_msg void OnMapImpexpExchangeButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

