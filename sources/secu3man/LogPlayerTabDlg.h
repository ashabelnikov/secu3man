
#include "ui-core/TabDialog.h"
#include "MIDesk/MIDeskDlg.h"
#include "common/FastDelegate.h"

#pragma once

class CLogPlayerTabDlg : public CTabDialog
{
public:
    typedef fastdelegate::FastDelegate0<> EventHandler;

	CLogPlayerTabDlg(CWnd* pParent = NULL);  


    //Time factor combo box
    void FillTimeFactorCombo(std::vector<_TSTRING>& i_factor_strings);
	size_t GetTimeFactor(void) const;
	void SetTimeFactor(size_t i_factor_id);


public:

	CMIDeskDlg m_MIDeskDlg;

	enum { IDD = IDD_LOG_PLAYER };

   virtual LPCTSTR GetDialogID(void) const;

   CComboBox m_time_factor_combo;

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);
	afx_msg void OnClose();
	DECLARE_MESSAGE_MAP()
};

