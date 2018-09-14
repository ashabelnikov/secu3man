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

/** \file CheckEngineTabController.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include <memory>
#include "common/ObjectTimer.h"
#include "common/unicodesupport.h"
#include "io-core/ControlApp.h"
#include "io-core/ControlAppAdapter.h"
#include "io-core/ufcodes.h"
#include "TabsManagement/ITabController.h"

class CCheckEngineTabDlg;
class CCommunicationManager;
class CStatusBarManager;
class ISettingsData;
class CEErrorIdStr;

class CCheckEngineTabController : public ITabController, private IAPPEventHandler
{
 public:
  CCheckEngineTabController(CCheckEngineTabDlg* i_view, CCommunicationManager* i_comm, CStatusBarManager* i_sbar, ISettingsData* ip_settings);
  virtual ~CCheckEngineTabController();

 private:
  void _SetErrorsToList(const SECU3IO::CEErrors* ip_errors);
  void _GetErrorsFromList(SECU3IO::CEErrors* op_errors);

  //from IAPPEventHandler:
  virtual void OnPacketReceived(const BYTE i_descriptor, SECU3IO::SECU3Packet* ip_packet);
  virtual void OnConnection(const bool i_online);

  //activation/deactivation of Check Engine tab
  virtual void OnActivate(void);
  virtual void OnDeactivate(void);

  virtual bool OnClose(void);
  virtual void OnCloseNotify(void);
  virtual void OnFullScreen(bool i_what);
  virtual bool OnAskChangeTab(void);

  void OnRealTimeErrors(void);
  void OnReadSavedErrors(void);
  void OnWriteSavedErrors(void);
  void OnListSetAllErrors(void);
  void OnListClearAllErrors(void);

  void OnSettingsChanged(int action);

  //data
  CCheckEngineTabDlg*  m_view;
  CCommunicationManager* m_comm;
  CStatusBarManager*  m_sbar;
  CControlAppAdapter* m_pAdapter;
  ISettingsData* mp_settings;

  bool m_real_time_errors_mode;
  std::auto_ptr<CEErrorIdStr> mp_errors_ids;
};
