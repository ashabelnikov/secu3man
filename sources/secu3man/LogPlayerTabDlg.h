
#include "ui-core/TabDialog.h"
#include "MIDesk/MIDeskDlg.h"
#include "common/FastDelegate.h"

#pragma once

class CLogPlayerTabDlg : public CTabDialog
{
public:
    typedef fastdelegate::FastDelegate0<> EventHandler;

	CLogPlayerTabDlg(CWnd* pParent = NULL);  

public:

	CMIDeskDlg m_MIDeskDlg;

	enum { IDD = IDD_LOG_PLAYER };

   virtual LPCTSTR GetDialogID(void) const;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	DECLARE_MESSAGE_MAP()
};

