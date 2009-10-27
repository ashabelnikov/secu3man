
#pragma once

#include <vector>
#include "common/FastDelegate.h"
#include "common/UnicodeSupport.h"

//типы флажков определ€ющих какие данные перемещать
enum EFWDFlags
{
  FLAG_START_MAP = 0,
  FLAG_IDLE_MAP,
  FLAG_WORK_MAP,
  FLAG_TEMP_MAP,
  FLAG_NR_OF_FLAGS
};

//"”льтра-тонкое"  представление ;-)
class CMapImpExpDlg : public CDialog
{

public:
    typedef fastdelegate::FastDelegate0<> EventHandler;
   typedef fastdelegate::FastDelegate0<bool> EventResult;
	typedef fastdelegate::FastDelegate2<int,CString> EventWithCodeAndString;
	typedef fastdelegate::FastDelegate1<int> EventWithCode;

	CMapImpExpDlg(CWnd* pParent = NULL);   // standard constructor

	void setOnExchangeButton(EventHandler OnFunction) {m_OnExchangeButton = OnFunction;};
	void setOnFWDCurrentListNamechanged(EventWithCodeAndString OnFunction) {m_OnFWDCurrentListNamechanged = OnFunction;};
	void setOnFWDCurrentSelectionChanged(EventWithCode OnFunction) {m_OnFWDCurrentSelectionChanged = OnFunction;};
	void setOnFWDOtherListNamechanged(EventWithCodeAndString OnFunction) {m_OnFWDOtherListNamechanged = OnFunction;};
	void setOnFWDOtherSelectionChanged(EventWithCode OnFunction) {m_OnFWDOtherSelectionChanged = OnFunction;};
	void setIsExchengeButtonAllowed(EventResult OnFunction) {m_IsExchengeButtonAllowed = OnFunction;};
	
	void setOnOkButton(EventHandler OnFunction) {m_OnOkButton = OnFunction;};
	void setOnCancelButton(EventHandler OnFunction) {m_OnCancelButton = OnFunction;};
    void setOnActivate(EventHandler OnActivate) {m_OnActivate = OnActivate;};

	void FillFWDCurrentList(std::vector<_TSTRING> i_list_of_names);
	void SetFWDCurrentListSelection(int i_selected_index);
	int  GetFWDCurrentListSelection(void);
	void SetFWDCurrentListTitle(const _TSTRING& i_title);

	void FillFWDOtherList(std::vector<_TSTRING> i_list_of_names);
	void SetFWDOtherListSelection(int i_selected_index);
	int  GetFWDOtherListSelection(void);
	void SetFWDOtherListTitle(const _TSTRING& i_title);

	void SetFWDFlag(EFWDFlags i_flag_type, bool i_state);
	bool GetFWDFlag(EFWDFlags i_flag_type);
	void EnableFWDFlag(EFWDFlags i_flag_type, bool i_enable);
	void SetExchangeButtonCaption(const _TSTRING& i_text);

	static const UINT IDD;

	CButton	m_exchange_button;
	CListCtrl	m_other_fwd_list;
	CListCtrl	m_current_fwd_list;
	CString	m_current_fwd_title_string;
	CString	m_other_fwd_title_string;	

    BOOL    m_fwd_flags[FLAG_NR_OF_FLAGS];
	CButton m_fwd_flags_buttons[FLAG_NR_OF_FLAGS];


    // Overrides
	//{{AFX_VIRTUAL(CMapImpExpDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

    // Implementation
protected:

	//{{AFX_MSG(CMapImpExpDlg)
	afx_msg void OnMapImpexpExchangeButton();
	afx_msg void OnChangeFWDCurrentList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndLabelEditFWDCurrentList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeFWDOtherList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndLabelEditFWDOtherList(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
    afx_msg void OnUpdateExchangeButton(CCmdUI* pCmdUI);
	afx_msg LRESULT OnKickIdle(WPARAM, LPARAM);

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	EventHandler m_OnExchangeButton;
	EventWithCodeAndString m_OnFWDCurrentListNamechanged;
	EventWithCode m_OnFWDCurrentSelectionChanged;
	EventWithCodeAndString m_OnFWDOtherListNamechanged;
	EventWithCode m_OnFWDOtherSelectionChanged;
	EventHandler m_OnOkButton;
	EventHandler m_OnCancelButton;
	EventHandler m_OnActivate;
	EventResult m_IsExchengeButtonAllowed;
};

