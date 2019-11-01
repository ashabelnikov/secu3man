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

/** \file InjDriverTabController.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include "common/ObjectTimer.h"
#include "common/unicodesupport.h"
#include "io-core/ControlAppAdapter.h"
#include "io-core/BootLoaderAdapter.h"
#include "io-core/ufcodes.h"
#include "TabsManagement/ITabController.h"

class CCommunicationManager;
class CInjDriverTabDlg;
class CStatusBarManager;
class ISettingsData;

class CInjDriverTabController : public ITabController, private IAPPEventHandler, private IBLDEventHandler
{
 public:
  CInjDriverTabController(CInjDriverTabDlg* ip_view, CCommunicationManager* ip_comm, CStatusBarManager* ip_sbar, ISettingsData* ip_settings);
  virtual ~CInjDriverTabController();

 private:
  //from IAPPEventHandler:
  virtual void OnPacketReceived(const BYTE i_descriptor, SECU3IO::SECU3Packet* ip_packet);
  virtual void OnConnection(const bool i_online);

  //program settings has changed
  void OnSettingsChanged(int action);

  //Appearing/disappearing of the Inj. Driver tab
  virtual void OnActivate(void);
  virtual void OnDeactivate(void);

  virtual bool OnClose(void);
  virtual void OnCloseNotify(void);
  virtual void OnFullScreen(bool i_what);
  virtual bool OnAskChangeTab(void);
  void OnLzidBlHsTimer(void);
  void OnAddressTimer(void);

  //from IBLDEventHandler
  virtual void OnUpdateUI(IBLDEventHandler::poolUpdateUI* ip_data);
  virtual void OnBegin(const int opcode,const int status);
  virtual void OnEnd(const int opcode,const int status);

  //events
  void OnChange(void);
  void OnSaveParameters(void);
  void OnExportToAFile(void);
  void OnImportFromAFile(void);
  void OnSaveToFirmware(void);
  void OnLoadFromFirmware(void);
  void OnShowFirmwareInfo(void);
  void OnExitOfflineMode(void);
  void OnWriteFirmwareFromFile(void);  
  void OnReadFirmwareToFile(void);  
  void OnReadLzblInfo(void);
  void OnWriteEEPROMFromFile(void);
  void OnReadEEPROMToFile(void);
  void OnResetEEPROM(void);
  void OnSelInjDrv(void);

  bool StartBootLoader(int op);
  bool ExitBootLoader(void);

  CInjDriverTabDlg*  mp_view;
  CCommunicationManager* mp_comm;
  CStatusBarManager*  mp_sbar;
  CControlAppAdapter* mp_pAdapter;
  ISettingsData* mp_sett;

 private:
  CObjectTimer<CInjDriverTabController> m_lzidblhs_tmr;
  CObjectTimer<CInjDriverTabController> m_address_tmr;
  bool m_initFlags[2];
  bool m_initialized;
  bool m_fw_loaded;
  bool m_active;
  bool m_recv_hs;
  SECU3IO::InjDrvPar m_fwdata[2];
  int m_saving_proc_state;
  std::vector<BYTE> m_bl_data;
  int   m_bl_op;
};
