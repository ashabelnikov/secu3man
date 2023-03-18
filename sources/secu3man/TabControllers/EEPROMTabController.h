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

/** \file EEPROMTabController.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include <memory>
#include "common/ObjectTimer.h"
#include "common/unicodesupport.h"
#include "io-core/ControlApp.h"
#include "io-core/ControlAppAdapter.h"
#include "io-core/BootLoaderAdapter.h"
#include "io-core/PlatformParamHolder.h"
#include "io-core/ufcodes.h"
#include "TabsManagement/ITabController.h"
#include "MapWndScrPos.h"

class CEEPROMTabDlg;
class CCommunicationManager;
class CStatusBarManager;
class ISettingsData;
class EEPROMDataMediator;
class CEErrorIdStr;

class CEEPROMTabController : public ITabController, private IAPPEventHandler, private IBLDEventHandler, public MapWndScrPos
{
 public:
  CEEPROMTabController(CEEPROMTabDlg* i_view, CCommunicationManager* i_comm, CStatusBarManager* i_sbar, ISettingsData* ip_settings);
  virtual ~CEEPROMTabController();

  bool IsEEPROMOpened(void);
  EEPROMDataMediator* GetEEDM(void) {return mp_eedm;}

 private:
  typedef std::map<size_t, std::pair<_TSTRING, DWORD> > ErrorsIDContainer;

  //from IAPPEventHandler:
  virtual void OnPacketReceived(const BYTE i_descriptor, SECU3IO::SECU3Packet* ip_packet);
  virtual void OnConnection(const bool i_online);

  //from IBLDEventHandler
  virtual void OnUpdateUI(IBLDEventHandler::poolUpdateUI* ip_data);
  virtual void OnBegin(const int opcode,const int status);
  virtual void OnEnd(const int opcode,const int status);

  //activation/deactivation of Check Engine tab
  virtual void OnActivate(void);
  virtual void OnDeactivate(void);

  virtual bool OnClose(void);
  virtual void OnCloseNotify(void);
  virtual void OnFullScreen(bool i_what);
  virtual bool OnAskChangeTab(void);
  virtual void OnSaveSettings(void);

  void OnSettingsChanged(int action);

  //from view
  void OnChangeSettingsMapEd(void);
  void OnOpenEEPROMFromFile(void);
  void OnSaveEEPROMToFile(void);
  void OnReadEEPROMFromSECU(void);
  void OnWriteEEPROMToSECU(void);
  void OnDropFile(_TSTRING fileName);
  void OnResetEeprom(void);

  void OnMapChanged(int i_type);
  void OnMapselNameChanged(void);
  void OnShowCEErrors();
  bool IsLoadGridsAvailable();
  void OnLoadGrids();
  void OnEditOdometer();

  void PrepareOnLoadEEPROM(const BYTE* i_buff, const _TSTRING& i_file_name);
  bool CheckChangesAskAndSaveEEPROM(void);
  void SetViewChartsValues(void);
  void SetViewValues(void);
  void OnModificationCheckTimer(void);
  void OnParamDeskTabActivate(void);
  void OnParamDeskChangeInTab(void);
  bool StartBootLoader(void);
  bool ExitBootLoader(void);

  typedef void (CEEPROMTabController::*BLFinishOpType) (void);
  BLFinishOpType m_blFinishOpCB;
  void finishOnReadEepromFromSECU();
  void finishOnWriteEepromToSECU();

  //data
  CEEPROMTabDlg*  mp_view;
  CCommunicationManager* mp_comm;
  CStatusBarManager*  mp_sbar;
  CControlAppAdapter* m_pAdapter;
  ISettingsData* mp_settings;

  EEPROMDataMediator* mp_eedm;
  PPEepromParam m_epp;
  CObjectTimer<CEEPROMTabController> m_modification_check_timer;
  CObjectTimer<CEEPROMTabController> m_waiting_bl_timer;
  int m_lastSel;

  BYTE* mp_bl_data;
  std::auto_ptr<CEErrorIdStr> mp_errors_ids;
  bool m_active;
  bool m_firmware_opened;
  std::vector<_TSTRING> m_funset_names;
  bool m_clear_sbar_txt_on_conn;
};
