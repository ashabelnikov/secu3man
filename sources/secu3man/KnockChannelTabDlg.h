
#include "ui-core/TabDialog.h"
#include "resource.h"
#include "propgrid/propertygrid.h"
#include "propgrid/custom/btncheckbox.h"
#include "common\fastdelegate.h"

#pragma once

class CKnockChannelTabDlg : public CTabDialog
{
	typedef CTabDialog Super;
public:
	typedef fastdelegate::FastDelegate0<> EventHandler;


// Construction
public:
	CKnockChannelTabDlg(CWnd* pParent = NULL);   // standard constructor  
	
	void setOnSaveParameters(EventHandler OnFunction) {m_OnSaveParameters = OnFunction;}

public:

// Dialog Data
	//{{AFX_DATA(CKnockChannelTabDlg)
	enum { IDD = IDD_KNOCK_CHANNEL };
	CButton m_param_save_button;
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
	
	void _OnShowHideAttenuatorGainTable(void);
	// Generated message map functions
	//{{AFX_MSG(CKnockChannelTabDlg)
	virtual BOOL OnInitDialog();		
	afx_msg LRESULT OnItemChanged(WPARAM, LPARAM);
	afx_msg void OnUpdatePropertyGrid(CCmdUI* pCmdUI);
	afx_msg void OnDestroy();
	afx_msg void OnSaveParameters();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
    EventHandler  m_OnSaveParameters;

    static void __cdecl OnChangeAttenuatorTable(void* i_param);
    static void __cdecl OnCloseAttenuatorTable(void* i_param);

    int m_attenuator_table_state;
	HWND m_attenuator_table_wnd_handle;
	int m_attenuator_table_slots[128];


};