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

#include <memory>
#include "common/unicodesupport.h"
#include "io-core/ControlAppAdapter.h"
#include "TabsManagement/ITabController.h"

class CCommunicationManager;
class CDevDiagnostTabDlg;
class CPMInitDataCollector;
class CStatusBarManager;
class ISettingsData;

class CDevDiagnostTabController : public ITabController, private IAPPEventHandler
{
 public:
  CDevDiagnostTabController(CDevDiagnostTabDlg* ip_view, CCommunicationManager* ip_comm, CStatusBarManager* ip_sbar, ISettingsData* ip_settings);
  virtual ~CDevDiagnostTabController();

 private:
  //появление/закрытие вкладки диагностики
  virtual void OnActivate(void);
  virtual void OnDeactivate(void);

  virtual bool OnClose(void);
  virtual bool OnAskFullScreen(void);
  virtual void OnFullScreen(bool i_what, const CRect& i_rect);

  //from IAPPEventHandler:
  virtual void OnPacketReceived(const BYTE i_descriptor, SECU3IO::SECU3Packet* ip_packet);
  virtual void OnConnection(const bool i_online);

  //от представления (диалога)
  void OnSettingsChanged(void);
  void OnOutputToggle(int output_id, bool state);
  void OnEnterButton(bool state);

 private:
  CDevDiagnostTabDlg*  mp_view;
  CCommunicationManager* mp_comm;
  CStatusBarManager*  mp_sbar;
  ISettingsData* mp_settings;

    //sub-controllers
  std::auto_ptr<CPMInitDataCollector> mp_idccntr;
  int m_comm_state;
  bool m_diagnost_mode_active;
};
