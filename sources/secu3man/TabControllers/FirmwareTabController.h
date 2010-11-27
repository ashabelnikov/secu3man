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
              http://secu-3.narod.ru
              email: secu-3@yandex.ru
*/

#pragma once

#include "common/ObjectTimer.h"
#include "io-core/BootLoader.h"
#include "io-core/BootLoaderAdapter.h"
#include "io-core/ControlApp.h"
#include "io-core/ControlAppAdapter.h"
#include "io-core/PlatformParamHolder.h"
#include "TabsManagement/ITabController.h"

class CCommunicationManager;
class CFirmwareDataMediator;
class CFirmwareTabDlg;
class CStatusBarManager;
class EEPROMDataMediator;
class ISettingsData;

class CFirmwareTabController : public ITabController, private IAPPEventHandler, private IBLDEventHandler 
{
 public:
  CFirmwareTabController(CFirmwareTabDlg* i_view, CCommunicationManager* i_comm, CStatusBarManager* i_sbar, ISettingsData* ip_settings);
  virtual ~CFirmwareTabController();

  bool IsFirmwareOpened(void);

  void SetAttenuatorMap(const float* i_values);
  void GetAttenuatorMap(float* o_values);

 private:
  //from ITabController
  virtual void OnActivate(void);
  virtual void OnDeactivate(void);

  virtual bool OnClose(void);
  virtual bool OnAskFullScreen(void);
  virtual void OnFullScreen(bool i_what, const CRect& i_rect);

  //from IAPPEventHandler:
  virtual void OnPacketReceived(const BYTE i_descriptor, SECU3IO::SECU3Packet* ip_packet);
  virtual void OnConnection(const bool i_online);

  //from IBLDEventHandler
  virtual void OnUpdateUI(IBLDEventHandler::poolUpdateUI* ip_data);  
  virtual void OnBegin(const int opcode,const int status);
  virtual void OnEnd(const int opcode,const int status);    

  void OnSettingsChanged(void);

  CString GenerateErrorStr(void);
  void SaveEEPROMToFile(const BYTE* p_data, const int size);
  bool SaveFLASHToFile(const BYTE* p_data, const int size, CString* o_file_name = NULL, bool calculate_and_place_crc16 = false);
  bool LoadEEPROMFromFile(BYTE* p_data, const int size);
  bool LoadFLASHFromFile(BYTE* p_data, const int size, CString* o_file_name = NULL);

  bool CheckChangesAskAndSaveFirmware(void);
  void SetViewFirmwareValues(void);
  void SetViewChartsValues(void);

  void PrepareOnLoadFLASH(const BYTE* i_buff,const _TSTRING& i_file_name);
  void CFirmwareTabController::StartWritingOfFLASHFromBuff(BYTE* io_buff);

  void _OnReadFlashToFile(void);
  bool _CheckCompatibilityAndAskUser(BYTE* i_buff);

  ///////////context menu and other event handlers/////////////////////
  void OnBootLoaderInfo(void);
  void OnReadEepromToFile(void);
  void OnWriteEepromFromFile(void);
  void OnReadFlashToFile(void);
  void OnWriteFlashFromFile(void);
  void OnOpenFlashFromFile(void);
  void OnSaveFlashToFile(void);
  void OnImportDataFromAnotherFW(void);
  void OnReadFlashFromSECU(void);
  void OnWriteFlashToSECU(void);
  void OnImportDataFromSECU3(void);
  void OnImportMapsFromMPSZ(void);
  void OnImportDefParamsFromEEPROMFile();
  void OnExportMapsToMPSZ(void);
  void OnWirmwareInfo(void);

  void OnFWInformationTextChanged(void);
  void OnMapChanged(int i_type);
  void OnFunSetSelectionChanged(int i_selected_index);
  void OnFunSetNamechanged(int i_index_of_item, CString i_new_name);
  void OnCloseMapWnd(HWND i_hwnd, int i_mapType);
  void OnOpenMapWnd(HWND i_hwnd, int i_mapType);
  ///////////////////////////////////////////////////////////	
  void OnBLStartedEmergency(void);

  void OnModificationCheckTimer(void);
  void OnParamDeskTabActivate(void);
  void OnParamDeskChangeInTab(void);

 private: //функции - обертки
  bool IsBLStartedEmergency(void);
  bool StartBootLoader(void);
  bool ExitBootLoader(void);

 private:
  PPFlashParam  m_fpp;
  PPEepromParam m_epp;

  CFirmwareTabDlg*  m_view;
  CCommunicationManager* m_comm;
  CStatusBarManager*  m_sbar;
  CFirmwareDataMediator* m_fwdm;
  EEPROMDataMediator* m_edm;
  ISettingsData* mp_settings;

  BYTE* m_bl_data;
  BYTE* m_code_for_merge_with_overhead;
  bool m_bl_started_emergency;
  int  m_current_funset_index;
  CObjectTimer<CFirmwareTabController> m_modification_check_timer;

  enum EReadFlashMode
  {
   MODE_RD_FLASH_TO_FILE,
   MODE_RD_FLASH_TO_BUFF_FOR_LOAD,
   MODE_RD_FLASH_FOR_IMPORT_DATA,
   MODE_RD_FLASH_TO_BUFF_MERGE_DATA
  };

  int  m_bl_read_flash_mode;
  int  m_lastSel;                  //хранит номер вкладки панели параметров которая была выбрана в последний раз
};
