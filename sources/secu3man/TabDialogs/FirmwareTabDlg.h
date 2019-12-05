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

/** \file FirmwareTabDlg.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include <map>
#include <memory>
#include "common/FastDelegate.h"
#include "ui-core/ITabControllerEvent.h"
#include "ui-core/TabController.h"
#include "ui-core/TabDialog.h"

class CHotKeysToCmdRouter;
class CFirmwareModeContextMenuManager;
class CIORemappingDlg;
class CParamDeskDlg;
class CTablesSetPanel;
class CToolTipCtrlEx;
class IDeskView;

/////////////////////////////////////////////////////////////////////////////
// CFirmwareTabDlg dialog

class CFirmwareTabDlg : public CTabDialog, private ITabControllerEvent
{
  typedef CTabDialog Super;
  typedef fastdelegate::FastDelegate0<> EventHandler;
  typedef fastdelegate::FastDelegate0<bool> EventResult;
  typedef fastdelegate::FastDelegate1<_TSTRING> EventString;

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

  void SetFirmwareName(_TSTRING i_name);
  void SetModified(bool i_modified);
  void SetFirmwareCRCs(unsigned int crc_stored_in_fw, unsigned int actual_crc_of_fw);

  bool IsProgrammeOnlyCode(void);

  std::auto_ptr<CParamDeskDlg> mp_ParamDeskDlg;
  std::auto_ptr<CIORemappingDlg> mp_IORemappingDlg;
  std::auto_ptr<CFirmwareModeContextMenuManager> mp_ContextMenuManager;
  std::auto_ptr<CTablesSetPanel> mp_TablesPanel;

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
  void setOnImportDataFromAnotherFW(EventHandler OnFunction);
  void setOnReadFlashFromSECU(EventHandler OnFunction);
  void setOnWriteFlashToSECU(EventHandler OnFunction);
  void setOnImportDataFromSECU3(EventHandler OnFunction);
  void setOnImportMapsFromMPSZ(EventHandler OnFunction);
  void setOnImportMapsFromSECU3(EventHandler OnFunction);
  void setOnImportMapsFromS3F(EventHandler OnFunction);
  void setOnImportDefParamsFromEEPROMFile(EventHandler OnFunction);
  void setOnImportTablesFromEEPROMFile(EventHandler OnFunction);
  void setOnExportMapsToMPSZ(EventHandler OnFunction);
  void setOnExportMapsToSECU3(EventHandler OnFunction);
  void setOnExportMapsToS3F(EventHandler OnFunction);
  void setOnFirmwareInfo(EventHandler OnFunction);
  void setOnViewFWOptions(EventHandler OnFunction);
  void setIsViewFWOptionsAvailable(EventResult OnFunction);
  void setIsIORemappingAvailable(EventResult IsFunction);
  void setOnResetEeprom(EventHandler OnFunction);
  void setOnFirmwareMaster(EventHandler OnFunction);

  //... от кнопок и чек боксов
  void setOnBLStartedEmergency(EventHandler OnFunction);
  //Drag and Drop
  void setOnDropFile(EventString OnFunction);

  void EnableMakingChartsChildren(bool enable);
  void EnableToggleMapWnd(bool toggle);

// Implementation
 protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  virtual BOOL OnInitDialog();
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
  afx_msg void OnPopupMenuButton();
  afx_msg void OnUpdatePopupMenu_bl(CCmdUI* pCmdUI);
  afx_msg void OnUpdatePopupMenu_file(CCmdUI* pCmdUI);
  afx_msg void OnUpdatePopupMenu_file1(CCmdUI* pCmdUI);
  afx_msg void OnUpdatePopupMenu_app(CCmdUI* pCmdUI);
  afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
  afx_msg void OnUpdateFirmwareSupportViewFWOptions(CCmdUI* pCmdUI);
  afx_msg void OnTimer(UINT nIDEvent);
  afx_msg void OnDestroy();
  afx_msg void OnBootLoaderInfo();
  afx_msg void OnReadEepromToFile();
  afx_msg void OnWriteEepromFromFile();
  afx_msg void OnResetEeprom();
  afx_msg void OnFirmwareMaster();
  afx_msg void OnReadFlashToFile();
  afx_msg void OnWriteFlashFromFile();
  afx_msg void OnUpdateBLStartedEmergency(CCmdUI* pCmdUI);
  afx_msg void OnFirmwareSupportBlStartedEmergency();
  afx_msg void OnOpenFlashFromFile();
  afx_msg void OnSaveFlashToFile();
  afx_msg void OnChangeFirmwareSupportFwInformation();
  afx_msg void OnUpdateFirmwareControls(CCmdUI* pCmdUI);
  afx_msg void OnImportDataFromAnotherFW();
  afx_msg void OnReadFlashFromSECU();
  afx_msg void OnWriteFlashToSECU();
  afx_msg void OnImportDataFromSECU3();
  afx_msg void OnUpdateProgOnlyCode(CCmdUI* pCmdUI);
  afx_msg void OnImportMapsFromMPSZ();
  afx_msg void OnImportMapsFromSECU3();
  afx_msg void OnImportMapsFromS3F();
  afx_msg void OnImportDefParamsFromEEPROMFile();
  afx_msg void OnImportTablesFromEEPROMFile();
  afx_msg void OnExportMapsToMPSZ();
  afx_msg void OnExportMapsToSECU3();
  afx_msg void OnExportMapsToS3F();
  afx_msg void OnFirmwareInfo();
  afx_msg void OnViewFWOptions();
  afx_msg void OnDropFiles(HDROP hDropInfo);
  DECLARE_MESSAGE_MAP()

  //ITabControllerEvent interface (from tab control)
  virtual void OnSelchangeTabctl(void);
  virtual bool OnSelchangingTabctl(void);

 private:
  CButton   m_bl_started_emergency;
  CButton   m_prog_only_code_checkbox;
  CButton   m_fw_options_btn;
  CButton   m_fw_popup_menu_button;
  CEdit     m_fw_information_edit;
  CEdit     m_fw_name;
  CStatic   m_fw_crc;
  CStatic   m_modification_flag;
  CTabController m_param_sel_tab;

 private:
  EventHandler  m_OnBootLoaderInfo;
  EventHandler  m_OnReadEepromToFile;
  EventHandler  m_OnWriteEepromFromFile;
  EventHandler  m_OnResetEeprom;
  EventHandler  m_OnReadFlashToFile;
  EventHandler  m_OnWriteFlashFromFile;
  EventHandler  m_OnBLStartedEmergency;
  EventHandler  m_OnOpenFlashFromFile;
  EventHandler  m_OnSaveFlashToFile;
  EventHandler  m_OnFWInformationTextChanged;
  EventHandler  m_OnImportDataFromAnotherFW;
  EventHandler  m_OnReadFlashFromSECU;
  EventHandler  m_OnWriteFlashToSECU;
  EventHandler  m_OnImportDataFromSECU3;
  EventHandler  m_OnImportMapsFromMPSZ;
  EventHandler  m_OnImportMapsFromSECU3;
  EventHandler  m_OnImportMapsFromS3F;
  EventHandler  m_OnImportDefParamsFromEEPROMFile;
  EventHandler  m_OnImportTablesFromEEPROMFile;
  EventHandler  m_OnExportMapsToMPSZ;
  EventHandler  m_OnExportMapsToSECU3;
  EventHandler  m_OnExportMapsToS3F;
  EventHandler  m_OnFirmwareInfo;
  EventHandler  m_OnViewFWOptions;
  EventResult   m_IsFirmwareOpened;
  EventResult   m_IsViewFWOptionsAvailable;
  EventResult   m_IsIORemappingAvailable;
  EventString   m_OnDropFile;
  EventHandler  m_OnFirmwareMaster;

  bool IsFirmwareOpened(void);
  bool IsIORemappingAvailable(void);

  bool m_is_bl_started_emergency_available;
  bool m_is_bl_items_available;
  bool m_is_app_items_available;
  bool m_initialized;

  void _RegisterHotKeys(void);
  std::auto_ptr<CHotKeysToCmdRouter> m_hot_keys_supplier;
  std::map<int, std::pair<IDeskView*, _TSTRING> > m_tabs;
  size_t m_tab_selection;

  std::auto_ptr<CToolTipCtrlEx> mp_ttc;
  CFont m_tab_selection_fnt;
};
