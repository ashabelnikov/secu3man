
#pragma once

#include "resource.h"
#include "common/FastDelegate.h"
#include <vector>
#include "common/UnicodeSupport.h"


enum EFWDFlags
{
  FLAG_START_MAP = 0,
  FLAG_IDLE_MAP,
  FLAG_WORK_MAP,
  FLAG_TEMP_MAP,
  FLAG_NR_OF_FLAGS
};


class CMapImpExpDlg : public CDialog
{

public:
    typedef fastdelegate::FastDelegate0<> EventHandler;
	typedef fastdelegate::FastDelegate2<int,CString> EventWithCodeAndString;
	typedef fastdelegate::FastDelegate1<int> EventWithCode;

	CMapImpExpDlg(CWnd* pParent = NULL);   // standard constructor

	void setOnExchangeButton(EventHandler OnFunction) {m_OnExchangeButton = OnFunction;};
	void setOnFWDCurrentListNamechanged(EventWithCodeAndString OnFunction) {m_OnFWDCurrentListNamechanged = OnFunction;};
	void setOnFWDCurrentSelectionChanged(EventWithCode OnFunction) {m_OnFWDCurrentSelectionChanged = OnFunction;};

	void FillFWDCurrentList(std::vector<_TSTRING> i_list_of_names);
	void SetFWDCurrentListSelection(int i_selected_index);
	void SetFWDCurrentListTitle(const _TSTRING& i_title);

	void FillFWDOtherList(std::vector<_TSTRING> i_list_of_names);
	void SetFWDOtherListSelection(int i_selected_index);
	void SetFWDOtherListTitle(const _TSTRING& i_title);

	void SetFWDFlag(EFWDFlags i_flag_type, bool i_state);
	bool GetFWDFlag(EFWDFlags i_flag_type);
	void EnableFWDFlag(EFWDFlags i_flag_type, bool i_enable);

    // Dialog Data
	//{{AFX_DATA(CMapImpExpDlg)
	enum { IDD = IDD_MAP_IMPEXP };
	CListCtrl	m_other_fwd_list;
	CListCtrl	m_current_fwd_list;
	CString	m_current_fwd_title_string;
	CString	m_other_fwd_title_string;
	BOOL    m_fwd_flags[FLAG_NR_OF_FLAGS];
	CButton m_fwd_flags_buttons[FLAG_NR_OF_FLAGS];
	//}}AFX_DATA


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
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	EventHandler m_OnExchangeButton;
	EventWithCodeAndString m_OnFWDCurrentListNamechanged;
	EventWithCode m_OnFWDCurrentSelectionChanged;
};

