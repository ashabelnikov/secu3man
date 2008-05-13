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

#include "resource.h"       // main symbols
#include "MainFrameManager.h"

#include "io-core/BootLoader.h"
#include "io-core/ControlApp.h"

#include "AppSettingsManager.h"

#include "MainFrameManager.h"
#include "CommunicationManager.h"


/////////////////////////////////////////////////////////////////////////////

class CSecu3manApp : public CWinApp
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

public:
  //{{AFX_MSG(CSecu3manApp)
  afx_msg void OnAppAbout();
  afx_msg void OnAppSettings();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()

public:
  CMainFrameManager* m_pMainFrameManager;
  CAppSettingsManager* m_pAppSettingsManager;
  CCommunicationManager* m_pCommunicationManager;
};

/////////////////////////////////////////////////////////////////////////////
