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

#include "common\ObjectTimer.h"
#include "common\unicodesupport.h"
#include "io-core\ControlApp.h"
#include "io-core\ControlAppAdapter.h"
#include "io-core\ufcodes.h"
#include "TabsManagement\ITabController.h"

class CCheckEngineTabDlg;
class CCommunicationManager;
class CStatusBarManager;

class CCheckEngineTabController : public ITabController, private IAPPEventHandler
{
 public:
  CCheckEngineTabController(CCheckEngineTabDlg* i_view, CCommunicationManager* i_comm, CStatusBarManager* i_sbar);
  virtual ~CCheckEngineTabController();

 private:
  typedef std::map<size_t, std::pair<_TSTRING, DWORD> > ErrorsIDContainer;

  void _SetErrorsToList(const SECU3IO::CEErrors* ip_errors);
  void _GetErrorsFromList(SECU3IO::CEErrors* op_errors);

  //from IAPPEventHandler:
  virtual void OnPacketReceived(const BYTE i_descriptor, SECU3IO::SECU3Packet* ip_packet);
  virtual void OnConnection(const bool i_online);

  //появление/закрытие вкладки Check Engine
  virtual void OnActivate(void);
  virtual void OnDeactivate(void);

  virtual bool OnClose(void);
  virtual bool OnAskFullScreen(void);
  virtual void OnFullScreen(bool i_what, const CRect& i_rect);

  void OnRealTimeErrors(void);
  void OnReadSavedErrors(void);
  void OnWriteSavedErrors(void);
  void OnListSetAllErrors(void);
  void OnListClearAllErrors(void);

  void OnSettingsChanged(void);

  //data
  CCheckEngineTabDlg*  m_view;
  CCommunicationManager* m_comm;
  CStatusBarManager*  m_sbar;
  CControlAppAdapter* m_pAdapter;

  bool m_real_time_errors_mode;
  ErrorsIDContainer m_errors_ids;
};
