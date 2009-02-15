
#include "ui-core/TabDialog.h"
#include "common/fastdelegate.h"
#include "common\unicodesupport.h"
#include <map>

#pragma once

class CCheckEngineTabDlg : public CTabDialog
{
	typedef CTabDialog Super;
public:
	typedef fastdelegate::FastDelegate0<> EventHandler;

// Construction
public:
	CCheckEngineTabDlg(CWnd* pParent = NULL);   // standard constructor   

	void setOnRealTimeErrors(EventHandler OnFunction) {m_OnRealTimeErrors = OnFunction;}
	void setOnReadSavedErrors(EventHandler OnFunction) {m_OnReadSavedErrors = OnFunction;}
	void setOnWriteSavedErrors(EventHandler OnFunction) {m_OnWriteSavedErrors = OnFunction;}
	void setOnListSetAllErrors(EventHandler OnFunction) {m_OnListSetAllErrors = OnFunction;}
	void setOnListClearAllErrors(EventHandler OnFunction) {m_OnListClearAllErrors = OnFunction;}

	bool GetRealTimeErrorsCheck(void) const;

	void AppendErrorsList(size_t i_id, const _TSTRING& i_description, bool i_state = false);
	void ResetErrorsList(void);
	void SetErrorState(size_t i_id, bool i_state);
	bool GetErrorState(size_t i_id) const;

    void EnableAll(bool i_enable);
	void EnableRWButtons(bool i_enable);

public:

// Dialog Data
	//{{AFX_DATA(CCheckEngineTabDlg)
	enum { IDD = IDD_CHECK_ENGINE };
	CStatic m_quick_help_text;
	CListCtrl m_errors_list;
	CImageList m_image_list;
	CButton m_realtime_checkbox;
	CButton m_read_saved_button;
	CButton m_write_saved_button;
	CButton m_list_set_all_button;
	CButton m_list_clear_all_button;
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
	afx_msg void OnRealTimeErrorsCheckbox();
	afx_msg void OnReadSavedErrors();
	afx_msg void OnWriteSavedErrors();
    afx_msg void OnUpdateControls(CCmdUI* pCmdUI); 
    afx_msg void OnUpdateRWButtons(CCmdUI* pCmdUI);
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnListSetAllErrors();
	afx_msg void OnListClearAllErrors();

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	EventHandler  m_OnRealTimeErrors;  
	EventHandler  m_OnReadSavedErrors;  
	EventHandler  m_OnWriteSavedErrors;  
	EventHandler  m_OnListSetAllErrors;
	EventHandler  m_OnListClearAllErrors;


	//Key = ID, Value = index of item in the list
	typedef std::map<size_t,size_t> Indexes;
	Indexes m_list_items_indexes;
    size_t m_list_next_item_index;

	bool m_all_enabled;
	bool m_rw_buttons_enabled;
};
