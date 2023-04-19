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

class CFirmwareTabController;
class CEEPROMTabController;

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
  virtual void OnSaveSettings(void);

  void OnRealTimeErrors(void);
  void OnReadSavedErrors(void);
  void OnWriteSavedErrors(void);
  void OnListSetAllErrors(void);
  void OnListClearAllErrors(void);

  void OnSettingsChanged(int action);

  void OnTrimtabButton(void);
  void OnTrimtabReadButton(void);
  void OnTrimtabResetButton(void);
  void OnTrimtabSaveButton(void);
  void OnTrimtabExport(int setIdx);
  void ApplyTrimtabToVE(float *ve);

  void PPS_SetOperation(int pps, bool clear_ltft_chache = false);
  bool PPS_ReadFWOptions(const BYTE i_descriptor, const void* i_packet_data);
  bool PPS_ReadFunset(const BYTE i_descriptor, const void* i_packet_data);
  bool PPS_ReadRPMGrid(const BYTE i_descriptor, const void* i_packet_data);
  bool PPS_ReadLodGrid(const BYTE i_descriptor, const void* i_packet_data);
  bool PPS_ReadLTFTMap(const BYTE i_descriptor, const void* i_packet_data);

  //data
  CCheckEngineTabDlg*  m_view;
  CCommunicationManager* m_comm;
  CStatusBarManager*  m_sbar;
  CControlAppAdapter* m_pAdapter;
  ISettingsData* mp_settings;

  CFirmwareTabController* mp_fwdcntr;
  CEEPROMTabController* mp_eedcntr;

  bool m_real_time_errors_mode;
  bool m_pending_ce_autoreading;
  std::auto_ptr<CEErrorIdStr> mp_errors_ids;

  CObjectTimer<CCheckEngineTabController> m_autoCETmr;

  float m_rpmGrid[16];       //RPM grid for LTFT map
  float m_loadGrid[16];      //load grid for LTFT map
  float m_trimTab[256];      //contents of the LTFT 1 map
  float m_trimTab2[256];     //contents of the LTFT 2 map

  int m_packet_processing_state;
  int m_operation_state;

  enum //Packet Processing States
  {
   PPS_READ_FWOPTIONS = 0,   //read firmare options
   PPS_READ_FUNSET = 1,      //read FUNSET_PAR
   PPS_READ_RPMGRID = 2,     //read RPM grid
   PPS_READ_LODGRID = 3,     //read load grid
   PPS_READ_LTFT_MAP = 4,    //read LTFT map
   PPS_READ_MONITOR_DATA = 5 //read SENSOR_DAT
  };

  DWORD m_fw_options; //Firmware options' bits read from SECU
  std::vector<int> m_rdLTFTMapFlags; //LTFT 1 map acquisition flags
  std::vector<int> m_rdLTFTMapFlags2;//LTFT 2 map acquisition flags
  std::vector<float> m_ltft_load_slots; //load axis's grid
  float m_ldaxMinVal;  //minimum value on the load axis
  bool m_ldaxBaro;     //use dynamic upper value on the load axis
  float m_baroPrev;    //previous value of baro pressure
};
