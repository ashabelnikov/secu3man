
#include "ui-core/TabDialog.h"
#include "resource.h"
#include "propgrid/propertygrid.h"
#include "propgrid/custom/btncheckbox.h"

#pragma once

class CKnockChannelTabDlg : public CTabDialog
{
	typedef CTabDialog Super;
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
	CBtnCheckBox m_item_attenuator_table_showhide;
	HITEM m_item_attenuator_table_showhide_item;

	// Generated message map functions
	//{{AFX_MSG(CKnockChannelTabDlg)
	virtual BOOL OnInitDialog();		
	afx_msg LRESULT OnItemChanged(WPARAM, LPARAM);
	afx_msg void OnUpdatePropertyGrid(CCmdUI* pCmdUI);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};