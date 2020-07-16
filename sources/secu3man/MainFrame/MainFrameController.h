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

/** \file MainFrameController.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include "common/ObjectTimer.h"
#include "io-core/ControlAppAdapter.h"

class CMainFrame;
class CCommunicationManager;
class CAppSettingsManager;
class CStatusBarManager;
class LogWriter;

class MainFrameController : public IAPPEventHandler
{
 public:
  MainFrameController(CCommunicationManager* i_pCommunicationManager,
                      CAppSettingsManager* i_pAppSettingsManager,
                      CStatusBarManager* i_pStatusBarManager,
                      LogWriter* i_pLogWriter, CMainFrame* ip_view = NULL);
  ~MainFrameController();

  void SetView(CMainFrame* ip_view);

  //from IAPPEventHandler
  virtual void OnPacketReceived(const BYTE i_descriptor, SECU3IO::SECU3Packet* ip_packet);
  virtual void OnConnection(const bool i_online);

  void OnAfterCreate(void);

 private:
  //from view
  void OnAppAbout();
  void OnAppDevSite();
  void OnAppDevForum();
  void OnAppVK();
  void OnAppFB();
  void OnAppSettings();
  void OnAppBeginLog();
  void OnAppEndLog();
  void OnAppLogMark(int mark);
  void OnAppLogFormat();
  void OnAppSwitchDashboards();
  void OnAppSaveScreenshot();
  void OnAppSaveScreenshot5sec();
  void OnAppSaveSettings();
  void OnActivate(bool i_state);
  void OnFullScreen(bool i_what);
  void OnChildCharts();
  void OnToggleMapWnd();
  bool IsBeginLoggingAllowed(void);
  bool IsEndLoggingAllowed(void);
  bool OnClose(void);
  void OnCreate(void);
  void OnWelcomeTimer(void);
  void OnScr5SecTimer(void);
  void OnEditSettings();

  //called by VIEW at the startup for get desired window position
  void OnGetInitialPos(CPoint& o_point);

  //called when new port device arrived
  void OnPortDevArrived(const _TSTRING& devName);

  //You have to call this function before using of this controller!
  void _SetDelegates(void);

  CMainFrame*            mp_view;
  CCommunicationManager* m_pCommunicationManager;
  CAppSettingsManager*   m_pAppSettingsManager;
  CStatusBarManager*     m_pStatusBarManager;
  LogWriter*             m_pLogWriter;
  CObjectTimer<MainFrameController> m_welcome_timer;
  CObjectTimer<MainFrameController> m_scr5sec_timer;
};
