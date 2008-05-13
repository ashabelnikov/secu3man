
#pragma once

class AFX_EXT_CLASS CProgressCtrlEx : public CProgressCtrl
{
// Construction
public:
	CProgressCtrlEx();


// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProgressCtrlEx)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CProgressCtrlEx();

	// Generated message map functions
protected:
	//{{AFX_MSG(Cpp)
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
