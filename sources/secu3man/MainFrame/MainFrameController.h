/* SECU-3  - An open source, free engine control unit
   Copyright (C) 2007 Alexey A. Shabelnikov. Ukraine, Gorlovka

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

 private:
  //from view
  void OnAppAbout();
  void OnAppSettings();
  void OnAppBeginLog();
  void OnAppEndLog();
  void OnActivate(bool i_state);
  bool OnFullScreen();
  bool IsBeginLoggingAllowed(void);
  bool IsEndLoggingAllowed(void);
  bool OnClose(void);

  //called by VIEW at the startup for get desired window position
  void OnGetInitialPos(CPoint& o_point);

  CRect _GetScreenRect(void) const;

  //You have to call this function before using of this controller!
  void _SetDelegates(void);

  CMainFrame*            mp_view;
  CCommunicationManager* m_pCommunicationManager;
  CAppSettingsManager*   m_pAppSettingsManager;
  CStatusBarManager*     m_pStatusBarManager;
  LogWriter*             m_pLogWriter;

  //true, если установлен полноэкранный режим
  bool m_full_screen_mode;
  //запоминает размеры и позицию окна перед переходом в полноэкранный режим
  CRect m_last_wnd_rect;
};
