
#pragma once

#include "ChildView.h"
#include "StatusBarManager.h"
#include "common/FastDelegate.h"

class CMainFrame : public CFrameWnd
{
	
public:
	typedef fastdelegate::FastDelegate0<bool> EventHandler;
	CMainFrame();
	virtual ~CMainFrame();

protected: 
	DECLARE_DYNAMIC(CMainFrame)
	CSize m_wnd_initial_size;

public:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	//}}AFX_VIRTUAL

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	
public:	
	CChildView*  m_pwndView;
    void SetView(CChildView* i_pChildView) {m_pwndView = i_pChildView;};

    //устанавливают обработчики событий
    virtual void setFunctionOnClose(EventHandler OnClose);


// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd *pOldWnd);
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	EventHandler m_OnClose;
};

/////////////////////////////////////////////////////////////////////////////
