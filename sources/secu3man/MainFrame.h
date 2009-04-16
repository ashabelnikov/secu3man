
#pragma once

#include "ChildView.h"
#include "StatusBarManager.h"
#include "common/FastDelegate.h"

class CMainFrame : public CFrameWnd
{
	
public:
	typedef fastdelegate::FastDelegate0<> EventHandler;
	typedef fastdelegate::FastDelegate0<bool> EventResult;
	CMainFrame();
	virtual ~CMainFrame();

	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	
    void SetView(CChildView* i_pChildView);

    //устанавливают обработчики событий
    virtual void setOnClose(EventResult i_OnClose);
	void setOnAppAbout(EventHandler i_OnFunction);
	void setOnAppSettings(EventHandler i_OnFunction);
	void setOnAppBeginLog(EventHandler i_OnFunction);
	void setOnAppEndLog(EventHandler i_OnFunction);
	void setIsBeginLoggingAllowed(EventResult i_OnFunction);
	void setIsEndLoggingAllowed(EventResult i_OnFunction);

	CChildView*  m_pwndView;

protected:
	DECLARE_DYNAMIC(CMainFrame)
	CSize m_wnd_initial_size;

	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd *pOldWnd);
	afx_msg void OnClose();
        afx_msg void OnAppAbout();
        afx_msg void OnAppSettings();
        afx_msg void OnAppBeginLog();
        afx_msg void OnAppEndLog();
        afx_msg void OnUpdateOnAppBeginLog(CCmdUI* pCmdUI);
        afx_msg void OnUpdateOnAppEndLog(CCmdUI* pCmdUI);
        afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized );
	DECLARE_MESSAGE_MAP()

private:
	EventResult  m_OnClose;
	EventHandler m_OnAppAbout;
	EventHandler m_OnAppSettings;
	EventHandler m_OnAppBeginLog;
	EventHandler m_OnAppEndLog;
	EventResult  m_IsBeginLoggingAllowed;
	EventResult  m_IsEndLoggingAllowed;

        //?????? ????????? ???????? ????: ??????? ??? ?????????
	bool m_active;
};

/////////////////////////////////////////////////////////////////////////////
