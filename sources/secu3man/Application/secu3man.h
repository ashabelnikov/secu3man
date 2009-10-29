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
class LogWriter;

/////////////////////////////////////////////////////////////////////////////

class CSecu3manApp : public CWinApp, public ISECU3Man 
{
 public:
  CSecu3manApp();
  ~CSecu3manApp();

  //MFC stuff :-)
  virtual BOOL InitInstance();
  virtual BOOL OnIdle(LONG lCount);
  virtual int ExitInstance();
 
  //from ISECU3Man
  virtual CAppSettingsManager*    GetAppSettingsManager(void) const;
  virtual CCommunicationManager*  GetCommunicationManager(void) const;
  virtual CMainFrameManager*      GetMainFrameManager(void) const;
  virtual LogWriter*              GetLogWriter(void) const;

 protected:
  DECLARE_MESSAGE_MAP()

 private:
  CMainFrameManager*     m_pMainFrameManager;
  CAppSettingsManager*   m_pAppSettingsManager;
  CCommunicationManager* m_pCommunicationManager;
  LogWriter*             m_pLogWriter; 
};

/////////////////////////////////////////////////////////////////////////////
