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

#include <map>
#include "common/ObjectTimer.h"
#include "common/unicodesupport.h"
#include "io-core/ControlApp.h"
#include "io-core/ControlAppAdapter.h"
#include "io-core/ufcodes.h"
#include "TabsManagement/ITabController.h"

class CCommunicationManager;
class CParamMonTabDlg;
class CStatusBarManager;
class ISettingsData;

class CParamMonTabController : public ITabController, private IAPPEventHandler
{
 public:
  CParamMonTabController(CParamMonTabDlg* i_view, CCommunicationManager* i_comm, CStatusBarManager* i_sbar, ISettingsData* ip_settings);
  virtual ~CParamMonTabController();

 private:
  //события от панели параметров
  void OnParamDeskTabActivate(void);
  void OnParamDeskChangeInTab(void);

  //появление/закрытие вкладки параметров и монитора
  virtual void OnActivate(void);
  virtual void OnDeactivate(void);

  virtual bool OnClose(void);
  virtual bool OnAskFullScreen(void);
  virtual void OnFullScreen(bool i_what, const CRect& i_rect);

  //from IAPPEventHandler:
  virtual void OnPacketReceived(const BYTE i_descriptor, SECU3IO::SECU3Packet* ip_packet);
  virtual void OnConnection(const bool i_online);

  void OnSettingsChanged(void);

  //from m_pd_changes_timer
  void OnParamDeskChangesTimer(void);

  enum //Packet Processing States
  {
   PPS_COLLECT_INITIAL_DATA = 0,
   PPS_READ_NECESSARY_PARAMETERS = 1,
   PPS_BEFORE_READ_MONITOR_DATA = 2,
   PPS_READ_MONITOR_DATA = 3
  };

  ////////////////////internal//////////////////////////
  bool CollectInitialDataFromSECU(const BYTE i_descriptor, const void* i_packet_data);
  void StartCollectingInitialData(void);
  //////////////////////////////////////////////////////
  bool ReadNecessaryParametersFromSECU(const BYTE i_descriptor, const void* i_packet_data);
  void StartReadingNecessaryParameters(void);
  //////////////////////////////////////////////////////

  void OnRawSensorsCheckBox(void);
  void OnPDSaveButton(void);

 private:
  CParamMonTabDlg*  m_view;
  CCommunicationManager* m_comm;
  CStatusBarManager*  m_sbar;
  ISettingsData* mp_settings;
  CObjectTimer<CParamMonTabController> m_pd_changes_timer;

  int  m_packet_processing_state;  //хранит код текущей операции, если никаких других операций не выполняется то должна выполнятся PPS_READ_MONITOR_DATA
  int  m_operation_state;          //хранит состояние конечных автоматов конкретной операции, если -1 -значит КА остановлен
  int  m_lastSel;                  //хранит номер вкладки панели параметров которая была выбрана в последний раз
  bool m_parameters_changed;       //этот флаг устанавливается при изменении на любой из вкладок параметров и сбрасывается после посылки измененных данных в SECU
};


