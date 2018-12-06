/* SECU-3  - An open source, free engine control unit
   Copyright (C) 2007 Alexey A. Shabelnikov. Ukraine, Kiev

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

   contacts:
              http://secu-3.org
              email: shabelnikov@secu-3.org
*/

/** \file MainFrame.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include <vector>
#include "common/FastDelegate.h"

class CChildView;
class CDVDeskDlg;

class CMainFrame : public CFrameWnd
{
 typedef CFrameWnd Super;
 typedef fastdelegate::FastDelegate0<> EventHandler;
 typedef fastdelegate::FastDelegate0<bool> EventResult;
 typedef fastdelegate::FastDelegate1<bool> EventHandler1;
 typedef fastdelegate::FastDelegate1<CPoint&> EventHandler2;
 typedef fastdelegate::FastDelegate1<const _TSTRING&> EventHandler3;
 typedef fastdelegate::FastDelegate1<int> EventHandler4;

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
  void CheckOnAppSwitchDashboards(bool checked);

  //устанавливают обработчики событий
  void setOnGetInitialPos(EventHandler2 i_OnGetInitialPos);
  void addOnClose(EventResult i_OnClose);
  void setOnAppAbout(EventHandler i_OnFunction);
  void setOnAppDevSite(EventHandler i_OnFunction);
  void setOnAppDevForum(EventHandler i_OnFunction);
  void setOnAppVK(EventHandler i_OnFunction);
  void setOnAppFB(EventHandler i_OnFunction);
  void setOnAppSettings(EventHandler i_OnFunction);
  void setOnAppBeginLog(EventHandler i_OnFunction);
  void setOnAppEndLog(EventHandler i_OnFunction);
  void setIsBeginLoggingAllowed(EventResult i_OnFunction);
  void setIsEndLoggingAllowed(EventResult i_OnFunction);
  void setOnFullScreen(EventHandler1 i_OnFullScreen);
  void setOnFullScreenNotify(EventHandler1 i_OnFullScreenNotify);
  void setOnActivate(EventHandler1 i_OnActivate);
  void setOnCreate(EventHandler i_OnCreate);
  void setOnPortDevArrived(EventHandler3 i_OnPortDevArrived);
  void setOnAppLogMark(EventHandler4 i_OnFunction);
  void setOnAppLogFormat(EventHandler i_OnFunction);
  void setOnCloseNotify(EventHandler i_OnFunction);
  void setOnAppSwitchDashboards(EventHandler i_OnFunction);
  void setOnAppSaveScreenshot(EventHandler i_OnFunction);
  void setOnAppSaveSettings(EventHandler i_OnFunction);

  bool CreateDVDesk(bool create);
  CDVDeskDlg* GetDVDesk(void);

protected:
  DECLARE_DYNAMIC(CMainFrame)

  virtual BOOL PreTranslateMessage(MSG* pMsg);
  afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnSetFocus(CWnd *pOldWnd);
  afx_msg void OnClose();
  afx_msg void OnAppAbout();
  afx_msg void OnAppDevSite();
  afx_msg void OnAppDevForum();
  afx_msg void OnAppVK();
  afx_msg void OnAppFB();
  afx_msg void OnAppSettings();
  afx_msg void OnAppBeginLog();
  afx_msg void OnAppEndLog();
  afx_msg void OnAppLogMark1();
  afx_msg void OnAppLogMark2();
  afx_msg void OnAppLogMark3();
  afx_msg void OnAppLogFormat();
  afx_msg void OnAppSwitchDashboards();
  afx_msg void OnAppSaveScreenshot();
  afx_msg void OnAppSaveSettings();
  afx_msg void OnUpdateOnAppBeginLog(CCmdUI* pCmdUI);
  afx_msg void OnUpdateOnAppEndLog(CCmdUI* pCmdUI);
  afx_msg void OnUpdateOnAppSwitchDashboards(CCmdUI* pCmdUI);
  afx_msg void OnActivateApp(BOOL bActive, DWORD dwThreadID);
  afx_msg BOOL OnDeviceChange(UINT nEventType, DWORD_PTR dwData);
  afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
  afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
  LRESULT OnSysColorChange(WPARAM wParam, LPARAM lParam);
  DECLARE_MESSAGE_MAP()

private:
  bool _UpdateTopLevelMainMenu(void);

  EventHandler2 m_OnGetInitialPos;
  std::vector<EventResult> m_OnClose;
  EventHandler m_OnAppAbout;
  EventHandler m_OnAppDevSite;
  EventHandler m_OnAppDevForum;
  EventHandler m_OnAppVK;
  EventHandler m_OnAppFB;
  EventHandler m_OnAppSettings;
  EventHandler m_OnAppBeginLog;
  EventHandler m_OnAppEndLog;
  EventHandler m_OnAppLogFormat;
  EventResult  m_IsBeginLoggingAllowed;
  EventResult  m_IsEndLoggingAllowed;
  EventHandler1 m_OnFullScreen;
  EventHandler1 m_OnFullScreenNotify;
  EventHandler1 m_OnActivate;
  EventHandler m_OnCreate;
  EventHandler3 m_OnPortDevArrived;
  EventHandler4 m_OnAppLogMark;
  EventHandler m_OnCloseNotify;
  EventHandler m_OnSwitchDashboards;
  EventHandler m_OnSaveScreenshot;
  EventHandler m_OnSaveSettings;
  std::auto_ptr<CDVDeskDlg> mp_DVDeskDlg;
  CChildView*  m_pwndView;
  BOOL m_bDoIdle;
  CSize m_createSize;
};

/////////////////////////////////////////////////////////////////////////////
