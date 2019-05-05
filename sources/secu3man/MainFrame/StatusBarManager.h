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

/** \file StatusBarManager.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include <memory>

class CStatusBarEx;

//Менеджер строки статуса
class CStatusBarManager
{
 public:
  CStatusBarManager();
  virtual ~CStatusBarManager();

  bool Create(CWnd* pParentWnd);
  void AddContent(void);

  void SetConnectionState(int i_State);
  void SetInformationText(const CString& i_text);

  void ShowProgressBar(bool show);
  void SetProgressRange(int nLower, int nUpper);
  void SetProgressPos(int nPos);
  void SetLoggerState(int i_state);

  enum { STATE_ONLINE = 1, STATE_OFFLINE = 2, STATE_BOOTLOADER = 3, STATE_HANDSHAKE = 4 };

  enum { LOG_STATE_WRITING, LOG_STATE_STOPPED};

 protected:
  CWnd* m_pParentWnd;
  std::auto_ptr<CStatusBarEx> mp_wndStatusBar;
  HICON   m_ConnIcons[4];
  HICON   m_LogWrIcon;
  CString m_ConnStrings[4];
  int m_CurrentConnectionState;
};
