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

#include <memory>
#include <vector>
#include "common\FastDelegate.h"
#include "ui-core\TabDialog.h"

enum EMapTypes
{
 TYPE_MAP_DA_START = 0,
 TYPE_MAP_DA_IDLE,
 TYPE_MAP_DA_WORK,
 TYPE_MAP_DA_TEMP_CORR,
 TYPE_MAP_ATTENUATOR,
 TYPE_MAP_COILREGUL
};

class CHotKeysToCmdRouter;
class CFirmwareModeContextMenuManager;
class CParamDeskDlg;

/////////////////////////////////////////////////////////////////////////////
// CFirmwareTabDlg dialog

class CFirmwareTabDlg : public CTabDialog
{
  typedef CTabDialog Super;
  typedef fastdelegate::FastDelegate0<> EventHandler;
  typedef fastdelegate::FastDelegate0<bool> EventResult;
  typedef fastdelegate::FastDelegate1<int> EventWithCode;
  typedef fastdelegate::FastDelegate2<int,CString> EventWithCodeAndString;
  typedef fastdelegate::FastDelegate2<HWND, int> EventWithHWND;

 public:
  CFirmwareTabDlg(CWnd* pParent = NULL);   // standard constructor

  static const UINT IDD;

  virtual LPCTSTR GetDialogID(void) const;
	
  bool IsBLStartedEmergency(void);
  void SetBLStartedEmergency(bool state);

  void EnableBLStartedEmergency(bool enable);
  bool IsBLStartedEmergencyEnabled(void);

  void EnableBLItems(bool enable);
  bool IsBLItemsEnabled(void);

  void EnableAppItems(bool enable);
  bool IsAppItemsEnabled(void);

  void SetFWInformationText(CString i_text);
  CString GetFWInformationText(void);

  float* GetStartMap(bool i_original);
  float* GetIdleMap(bool i_original);
  float* GetWorkMap(bool i_original);
  float* GetTempMap(bool i_original);
  float* GetAttenuatorMap(bool i_original);
  float* GetCoilRegulMap(bool i_original);

  //returns NULL if corresponding window wasn't opened
  HWND GetMapWindow(int wndType);

  void SetFunSetListBox(std::vector<_TSTRING> i_list_of_names);
  void SetFunSetListBoxSelection(int i_selected_index);
  void SetFirmwareName(_TSTRING i_name);
  void SetModified(bool i_modified);
  void SetFirmwareCRCs(unsigned int crc_stored_in_fw, unsigned int actual_crc_of_fw);

  void UpdateOpenedCharts(void);
  bool IsProgrammeOnlyCode(void);

  std::auto_ptr<CParamDeskDlg> mp_ParamDeskDlg;
  std::auto_ptr<CFirmwareModeContextMenuManager> mp_ContextMenuManager;

 public: //установка обработчиков событий от меню
  void setOnBootLoaderInfo(EventHandler OnFunction);
  void setOnReadEepromToFile(EventHandler OnFunction);
  void setOnWriteEepromFromFile(EventHandler OnFunction);
  void setOnReadFlashToFile(EventHandler OnFunction);
  void setOnWriteFlashFromFile(EventHandler OnFunction);
  void setOnOpenFlashFromFile(EventHandler OnFunction);
  void setOnFWInformationTextChanged(EventHandler OnFunction);
  void setOnSaveFlashToFile(EventHandler OnFunction);
  void setIsFirmwareOpened(EventResult IsFunction);
  void setOnMapChanged(EventWithCode OnFunction);
  void setOnFunSetSelectionChanged(EventWithCode OnFunction);
  void setOnFunSetNamechanged(EventWithCodeAndString OnFunction);
  void setOnImportDataFromAnotherFW(EventHandler OnFunction);
  void setOnReadFlashFromSECU(EventHandler OnFunction);
  void setOnWriteFlashToSECU(EventHandler OnFunction);
  void setOnImportDataFromSECU3(EventHandler OnFunction);
  void setOnImportMapsFromMPSZ(EventHandler OnFunction);
  void setOnImportDefParamsFromEEPROMFile(EventHandler OnFunction);
  void setOnExportMapsToMPSZ(EventHandler OnFunction);
  void setOnFirmwareInfo(EventHandler OnFunction);
  //... от кнопок и чек боксов
  void setOnBLStartedEmergency(EventHandler OnFunction);

  void setOnCloseMapWnd(EventWithHWND OnFunction);
  void setOnOpenMapWnd(EventWithHWND OnFunction);

// Implementation
 protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  virtual BOOL OnInitDialog();
  afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
  afx_msg void OnUpdatePopupMenu_bl(CCmdUI* pCmdUI);
  afx_msg void OnUpdatePopupMenu_file(CCmdUI* pCmdUI);
  afx_msg void OnUpdatePopupMenu_file1(CCmdUI* pCmdUI);
  afx_msg void OnUpdatePopupMenu_app(CCmdUI* pCmdUI);
  afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
  afx_msg void OnFirmwareSupportViewStartMap();
  afx_msg void OnFirmwareSupportViewIdleMap();
  afx_msg void OnFirmwareSupportViewWorkMap();
  afx_msg void OnFirmwareSupportViewTempMap();
  afx_msg void OnFirmwareSupportViewAttenuatorMap();		
  afx_msg void OnFirmwareSupportViewCoilRegulMap();		
  afx_msg void OnUpdateFirmwareSupportViewStartMap(CCmdUI* pCmdUI);
  afx_msg void OnUpdateFirmwareSupportViewIdleMap(CCmdUI* pCmdUI);
  afx_msg void OnUpdateFirmwareSupportViewWorkMap(CCmdUI* pCmdUI);
  afx_msg void OnUpdateFirmwareSupportViewTempMap(CCmdUI* pCmdUI);
  afx_msg void OnUpdateFirmwareSupportViewAttenuatorMap(CCmdUI* pCmdUI);
  afx_msg void OnUpdateFirmwareSupportViewCoilRegulMap(CCmdUI* pCmdUI);
  afx_msg void OnTimer(UINT nIDEvent);
  afx_msg void OnDestroy();
  afx_msg void OnBootLoaderInfo();
  afx_msg void OnReadEepromToFile();
  afx_msg void OnWriteEepromFromFile();
  afx_msg void OnReadFlashToFile();
  afx_msg void OnWriteFlashFromFile();
  afx_msg void OnUpdateBLStartedEmergency(CCmdUI* pCmdUI);
  afx_msg void OnFirmwareSupportBlStartedEmergency();	
  afx_msg void OnOpenFlashFromFile();
  afx_msg void OnSaveFlashToFile();
  afx_msg void OnChangeFirmwareSupportFwInformation();
  afx_msg void OnUpdateFirmwareControls(CCmdUI* pCmdUI);
  afx_msg void OnChangeFirmwareSupportFunsetList(NMHDR* pNMHDR, LRESULT* pResult);
  afx_msg void OnEndLabelEditFirmwareSupportFunsetList(NMHDR* pNMHDR, LRESULT* pResult);
  afx_msg void OnImportDataFromAnotherFW();
  afx_msg void OnReadFlashFromSECU();
  afx_msg void OnWriteFlashToSECU();
  afx_msg void OnImportDataFromSECU3();
  afx_msg void OnUpdateProgOnlyCode(CCmdUI* pCmdUI);
  afx_msg void OnImportMapsFromMPSZ();
  afx_msg void OnImportDefParamsFromEEPROMFile();
  afx_msg void OnExportMapsToMPSZ();
  afx_msg void OnFirmwareInfo();
  DECLARE_MESSAGE_MAP()

 private:
  CListCtrl	m_funset_listbox;
  CButton	m_bl_started_emergency;
  CButton	m_view_work_map_btn;
  CButton	m_view_temp_map_btn;
  CButton	m_view_start_map_btn;
  CButton	m_view_idle_map_btn;
  CButton m_prog_only_code_checkbox;
  CButton m_view_attenuator_map_btn;
  CButton m_view_coilregul_map_btn;
  CEdit   m_fw_information_edit;
  CEdit m_fw_name;
  CStatic m_fw_crc;
  CStatic m_modification_flag;

 private: 
  EventWithCode m_OnMapChanged;
  EventWithCode m_OnFunSetSelectionChanged;
  EventHandler  m_OnBootLoaderInfo;
  EventHandler  m_OnReadEepromToFile;
  EventHandler  m_OnWriteEepromFromFile;
  EventHandler  m_OnReadFlashToFile;
  EventHandler  m_OnWriteFlashFromFile;
  EventHandler  m_OnBLStartedEmergency;
  EventHandler  m_OnOpenFlashFromFile;
  EventHandler  m_OnSaveFlashToFile;
  EventHandler  m_OnFWInformationTextChanged;
  EventResult   m_IsFirmwareOpened;
  EventWithCodeAndString m_OnFunSetNamechanged; 
  EventHandler  m_OnImportDataFromAnotherFW;
  EventHandler  m_OnReadFlashFromSECU;
  EventHandler  m_OnWriteFlashToSECU;
  EventHandler  m_OnImportDataFromSECU3;
  EventHandler  m_OnImportMapsFromMPSZ;
  EventHandler  m_OnImportDefParamsFromEEPROMFile;
  EventHandler  m_OnExportMapsToMPSZ;
  EventHandler  m_OnFirmwareInfo;
  EventWithHWND m_OnCloseMapWnd;
  EventWithHWND m_OnOpenMapWnd;
  
  //for C - functions
  int m_work_map_chart_state;
  int m_temp_map_chart_state;
  int m_start_map_chart_state;
  int m_idle_map_chart_state;
  int m_attenuator_map_chart_state;
  int m_coilregul_map_chart_state;
 
  HWND m_start_map_wnd_handle;
  HWND m_idle_map_wnd_handle;
  HWND m_work_map_wnd_handle;
  HWND m_temp_map_wnd_handle;
  HWND m_attenuator_map_wnd_handle;
  HWND m_coilregul_map_wnd_handle;
   
  float m_attenuator_table_slots[128];

  static void __cdecl OnChangeStartMap(void* i_param);
  static void __cdecl OnCloseStartMap(void* i_param);
  static void __cdecl OnChangeIdleMap(void* i_param);
  static void __cdecl OnCloseIdleMap(void* i_param);
  static void __cdecl OnChangeWorkMap(void* i_param);
  static void __cdecl OnCloseWorkMap(void* i_param);
  static void __cdecl OnChangeTempMap(void* i_param);
  static void __cdecl OnCloseTempMap(void* i_param);
  static void __cdecl OnChangeAttenuatorTable(void* i_param);
  static void __cdecl OnCloseAttenuatorTable(void* i_param);
  static void __cdecl OnChangeCoilRegulTable(void* i_param);
  static void __cdecl OnCloseCoilRegulTable(void* i_param);
  static void __cdecl OnGetYAxisLabel(LPTSTR io_label_string, void* i_param);

  bool IsFirmwareOpened(void); 

  bool m_is_bl_started_emergency_available;
  bool m_is_bl_items_available;
  bool m_is_app_items_available;

  ///////////////////////////////////////////////////////
  float m_start_map_active[16];
  float m_start_map_original[16];

  float m_idle_map_active[16];
  float m_idle_map_original[16];

  float m_work_map_active[16][16];
  float m_work_map_original[16][16];
	
  float m_temp_map_active[16];
  float m_temp_map_original[16];

  float m_attenuator_map_active[128];
  float m_attenuator_map_original[128];

  float m_coilregul_map_active[32];
  float m_coilregul_map_original[32];
  ///////////////////////////////////////////////////////

  void _RegisterHotKeys(void);
  std::auto_ptr<CHotKeysToCmdRouter> m_hot_keys_supplier;
};

