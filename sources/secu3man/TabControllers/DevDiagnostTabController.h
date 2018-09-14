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

/** \file DevDiagnostTabController.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include <map>
#include <memory>
#include "common/ObjectTimer.h"
#include "common/unicodesupport.h"
#include "io-core/ControlAppAdapter.h"
#include "io-core/SECU3IO.h"
#include "TabsManagement/ITabController.h"

class CCommunicationManager;
class CDevDiagnostTabDlg;
class CPMInitDataCollector;
class CStatusBarManager;
class ISettingsData;
namespace { class ITstMode; }

class CDevDiagnostTabController : public ITabController, private IAPPEventHandler
{
 public:
  CDevDiagnostTabController(CDevDiagnostTabDlg* ip_view, CCommunicationManager* ip_comm, CStatusBarManager* ip_sbar, ISettingsData* ip_settings);
  virtual ~CDevDiagnostTabController();

 private:
  //activation/deactivation of diagnistics tab
  virtual void OnActivate(void);
  virtual void OnDeactivate(void);

  virtual bool OnClose(void);
  virtual void OnCloseNotify(void);
  virtual void OnFullScreen(bool i_what);
  virtual bool OnAskChangeTab(void);

  //from IAPPEventHandler:
  virtual void OnPacketReceived(const BYTE i_descriptor, SECU3IO::SECU3Packet* ip_packet);
  virtual void OnConnection(const bool i_online);

  //от представления (диалога)
  void OnSettingsChanged(int action);
  void OnOutputToggle(int output_id, bool state);
  void OnEnterButton(void);
  void OnStartOutputsAutoTesting(void);
  void OnStopOutputsAutoTesting(void);
  void OnEnableBLDETesting(bool enable);

  void OnTstTimer(void);
  void UpdateOutputs(void);
  void SetOutputValue(int id, bool state);

  void _FillOutputsMap(void);
 private:
  void _SetOuptutMapItem(int id, bool state);

  CDevDiagnostTabDlg*  mp_view;
  CCommunicationManager* mp_comm;
  CStatusBarManager*  mp_sbar;
  ISettingsData* mp_settings;

  //sub-controllers
  std::auto_ptr<CPMInitDataCollector> mp_idccntr;
  int m_comm_state;
  bool m_diagnost_mode_active;
  SECU3IO::DiagOutDat m_outputs;
  std::map<int, int*> m_outputs_map;
  CObjectTimer<CDevDiagnostTabController> m_tst_timer;
  std::vector<ITstMode*> m_tstModes;
  std::vector<ITstMode*>::iterator m_current_tst_mode;
};
