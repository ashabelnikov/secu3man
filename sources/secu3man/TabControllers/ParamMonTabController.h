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

/** \file ParamMonTabController.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include <vector>
#include "common/unicodesupport.h"
#include "io-core/ControlApp.h"
#include "io-core/ControlAppAdapter.h"
#include "io-core/ufcodes.h"
#include "TabsManagement/ITabController.h"
#include "common/ObjectTimer.h"

class CCommunicationManager;
class CParamMonTabDlg;      //view
class CPMInitDataCollector; //sub-controller
class CPMMonitorController; //sub-controller
class CPMParamsController;  //sub-controller
class CPMStateMachineState; //sub-controller
class CPMTablesController;  //sub-controller
class CStatusBarManager;
class ISettingsData;

class CParamMonTabController : public ITabController, private IAPPEventHandler
{
 public:
  CParamMonTabController(CParamMonTabDlg* ip_view, CCommunicationManager* ip_comm, CStatusBarManager* ip_sbar, ISettingsData* ip_settings);
  virtual ~CParamMonTabController();

 private:
  //called when settings are being changed
  void OnSettingsChanged(int action);
  
  //появление/закрытие вкладки параметров и монитора
  virtual void OnActivate(void);
  virtual void OnDeactivate(void);
    
  //from IAPPEventHandler:
  virtual void OnPacketReceived(const BYTE i_descriptor, SECU3IO::SECU3Packet* ip_packet);
  virtual void OnConnection(const bool i_online);

  //form main form
  virtual bool OnClose(void);
  virtual void OnCloseNotify(void);
  virtual void OnFullScreen(bool i_what);
  virtual bool OnAskChangeTab(void);
  
  //from view
  void OnRawSensorsCheckBox(void);  //from "show raw sensors" check box  
  void OnEditTablesCheckBox(void);  //from "edit tables" check box
  
  //from CPMParamsController sub-controller
  void OnPDRequestsDataCollection();

  void OnParametersChanged(const BYTE i_descriptor, const void* ip_packet);

  void ConfigureIndicators(void);

 private:
  void _OnOneShotTimer(void);

  CParamMonTabDlg*  mp_view;
  CCommunicationManager* mp_comm;
  CStatusBarManager*  mp_sbar;
  ISettingsData* mp_settings;

  //sub-controllers
  std::auto_ptr<CPMInitDataCollector> mp_idccntr;
  std::auto_ptr<CPMMonitorController> mp_moncntr;
  std::auto_ptr<CPMParamsController> mp_parcntr;
  std::auto_ptr<CPMTablesController> mp_tabcntr;

  void _StartScenario(const std::vector<CPMStateMachineState*>& scenario);
  //различные машины состояний (для разных сценариев)
  std::vector<CPMStateMachineState*> m_scenario1;
  std::vector<CPMStateMachineState*> m_scenario2;
  std::vector<CPMStateMachineState*> m_scenario3;
  std::vector<CPMStateMachineState*> m_scenario4;
  std::vector<CPMStateMachineState*> m_scenario5;
  //машина состояний
  std::vector<CPMStateMachineState*> m_state_machine;
  std::vector<CPMStateMachineState*>::iterator m_current_state;

  CObjectTimer<CParamMonTabController> m_one_shot_timer; 
};
