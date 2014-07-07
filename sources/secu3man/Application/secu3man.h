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
