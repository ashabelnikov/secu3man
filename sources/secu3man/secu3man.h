 /****************************************************************
 *       SECU-3  - An open source, free engine control unit
 *    Designed by Alexey A. Shabelnikov. Ukraine, Gorlovka 2007.
 *       Microprocessors systems - design & programming.
 *    contacts:
 *              http://secu-3.narod.ru
 *              ICQ: 405-791-931
 ****************************************************************/

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif


#include "ISECU3man.h"

class CAppSettingsManager;
class CCommunicationManager;
class CMainFrameManager;

/////////////////////////////////////////////////////////////////////////////

class CSecu3manApp : public CWinApp, public ISECU3Man 
{
public:
  CSecu3manApp();
  ~CSecu3manApp();

  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CSecu3manApp)
public:
  virtual BOOL InitInstance();
  virtual BOOL OnIdle(LONG lCount);
  virtual int ExitInstance();
  //}}AFX_VIRTUAL

  //from ISECU3Man
  virtual CAppSettingsManager*    GetAppSettingsManager(void) const;
  virtual CCommunicationManager*  GetCommunicationManager(void) const;
  virtual CMainFrameManager*      GetMainFrameManager(void) const; 

public:
  //{{AFX_MSG(CSecu3manApp)
  afx_msg void OnAppAbout();
  afx_msg void OnAppSettings();
  afx_msg void OnAppBeginLog();
  afx_msg void OnAppEndLog();
  afx_msg void OnUpdateOnAppBeginLog(CCmdUI* pCmdUI);
  afx_msg void OnUpdateOnAppEndLog(CCmdUI* pCmdUI);
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()

private:
  CMainFrameManager* m_pMainFrameManager;
  CAppSettingsManager* m_pAppSettingsManager;
  CCommunicationManager* m_pCommunicationManager;
};

/////////////////////////////////////////////////////////////////////////////
