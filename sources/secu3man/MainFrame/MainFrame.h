
#pragma once

#include <vector>
#include "common/FastDelegate.h"

class CChildView;

class CMainFrame : public CFrameWnd
{
 typedef fastdelegate::FastDelegate0<> EventHandler;
 typedef fastdelegate::FastDelegate0<bool> EventResult;
 typedef fastdelegate::FastDelegate1<bool> EventHandler1; 
 typedef fastdelegate::FastDelegate1<CPoint&> EventHandler2; 

 public:
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
  void setOnGetInitialPos(EventHandler2 i_OnGetInitialPos);
  void addOnClose(EventResult i_OnClose);
  void setOnAppAbout(EventHandler i_OnFunction);
  void setOnAppSettings(EventHandler i_OnFunction);
  void setOnAppBeginLog(EventHandler i_OnFunction);
  void setOnAppEndLog(EventHandler i_OnFunction);
  void setIsBeginLoggingAllowed(EventResult i_OnFunction);
  void setIsEndLoggingAllowed(EventResult i_OnFunction);
  void setOnAskFullScreen(EventResult i_OnAskFullScreen);
  void setOnFullScreen(EventResult i_OnFullScreen);
  void setOnFullScreenNotify(EventHandler1 i_OnFullScreenNotify);
  void setOnActivate(EventHandler1 i_OnActivate);

protected:
  DECLARE_DYNAMIC(CMainFrame)

  virtual BOOL PreTranslateMessage(MSG* pMsg); 
  afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnSetFocus(CWnd *pOldWnd);
  afx_msg void OnClose();
  afx_msg void OnAppAbout();
  afx_msg void OnAppSettings();
  afx_msg void OnAppBeginLog();
  afx_msg void OnAppEndLog();
  afx_msg void OnUpdateOnAppBeginLog(CCmdUI* pCmdUI);
  afx_msg void OnUpdateOnAppEndLog(CCmdUI* pCmdUI);
  afx_msg void OnFullScreen();
  afx_msg void OnUpdateOnFullScreen(CCmdUI* pCmdUI);
  afx_msg void OnActivateApp(BOOL bActive, DWORD dwThreadID);
  DECLARE_MESSAGE_MAP()

private:
  bool _UpdateTopLevelMainMenu(void);

  EventHandler2 m_OnGetInitialPos;
  std::vector<EventResult> m_OnClose;
  EventHandler m_OnAppAbout;
  EventHandler m_OnAppSettings;
  EventHandler m_OnAppBeginLog;
  EventHandler m_OnAppEndLog;
  EventResult  m_IsBeginLoggingAllowed;
  EventResult  m_IsEndLoggingAllowed;
  EventResult  m_OnAskFullScreen;
  EventResult  m_OnFullScreen;
  EventHandler1 m_OnFullScreenNotify;
  EventHandler1 m_OnActivate; 
 
  CChildView*  m_pwndView;
  const CSize m_wnd_initial_size;
  BOOL m_bDoIdle;
};

/////////////////////////////////////////////////////////////////////////////
