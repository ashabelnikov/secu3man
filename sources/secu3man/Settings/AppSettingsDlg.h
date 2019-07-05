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

/** \file AppSettingsDlg.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include "common/unicodesupport.h"
#include "IAppSettingsDlg.h"
#include "ui-core/editex.h"
#include "ui-core/EditEx.h"
#include "ui-core/SpinButtonCtrlEx.h"
#include "io-core/EnumPorts.h"

class CToolTipCtrlEx;

class CAppSettingsDlg : public CDialog, public IAppSettingsDlg
{
 public:
  CAppSettingsDlg(CWnd* pParent = NULL);   // standard constructor
  static const UINT IDD;

 //////////////////Interface//////////////////////////
  virtual void FillCtrlsWithAllowableBaudRates(std::vector<DWORD> i_AllowableBaudRates);
  virtual void FillCtrlsWithAllowablePorts(std::vector<_TSTRING> i_AllowablePorts);
  virtual void FillCtrlsWithAllowableCSVSepSymbols(std::vector<std::pair<_TSTRING, char> >  i_AllowableCSVSepSymbols);
  virtual void FillCtrlsWithAllowableInterfaceLanguages(std::vector<std::pair<std::pair<_TSTRING, _TSTRING>, int> > i_AllowableInterfaceLanguages);
  virtual void FillCtrlsWithAllowableECUPlatformTypes(std::vector<std::pair<std::pair<_TSTRING, _TSTRING>, int> > i_AllowableECUPlatformTypes);

  //"Set" methods (model => view data transfer)
  virtual void SetPortName(_TSTRING i_PortName);
  virtual void SetBaudRateApplication(DWORD i_app_baud);
  virtual void SetBaudRateBootloader(DWORD i_bl_baud);
  virtual void SetCSVSepSymbol(size_t i_index);
  virtual void SetMIDeskUpdatePeriod(int i_period);
  virtual void SetInterfaceLanguage(int i_iface_lang);
  virtual void SetECUPlatformType(int i_platform_type);
  virtual void SetTachometerMax(int i_max);
  virtual void SetPressureMax(int i_max);
  virtual void SetUseDVFeatures(bool i_use);
  virtual void SetDVDeskUpdatePeriod(int i_period);
  virtual void SetShowToolTips(bool i_show);
  virtual void SetShowExFixtures(bool i_show);
  virtual void SetHexDataMode(bool i_hex);
  virtual void SetNumPulsesPer1Km(int i_pp1km);
  virtual void SetInjDrvTabActive(bool i_active);

  //"Get" methods (view => model data transfer)
  virtual _TSTRING GetPortName(void);
  virtual DWORD GetBaudRateApplication(void);
  virtual DWORD GetBaudRateBootloader(void);
  virtual int GetMIDeskUpdatePeriod(void);
  virtual int GetInterfaceLanguage(void) const;
  virtual int GetECUPlatformType(void) const;
  virtual int GetTachometerMax(void) const;
  virtual int GetPressureMax(void) const;
  virtual bool GetUseDVFeatures(void);
  virtual int GetDVDeskUpdatePeriod(void);
  virtual bool GetShowToolTips(void) const;
  virtual bool GetShowExFixtures(void) const;
  virtual bool GetHexDataMode(void) const;
  virtual int GetNumPulsesPer1Km(void) const;
  virtual bool GetInjDrvTabActive(void) const;

  //logs
  virtual void SetLogFilesFolder(const _TSTRING& i_folder);
  virtual CString GetLogFilesFolder(void) const;
  virtual void SetUseAppFolder(bool i_use);
  virtual bool GetUseAppFolder(void) const;
  virtual void SetAlwaysWriteLog(bool i_always);
  virtual bool GetAlwaysWriteLog(void) const;
  virtual size_t GetCSVSepSymbol(void);

  virtual void SetExistingPorts(bool i_exp);
  virtual bool GetExistingPorts(void) const;

  void SetWriteLogFields(bool i_always);
  bool GetWriteLogFields(void) const;

  virtual void setFunctionOnOk(EventHandler OnOk);
  virtual void setFunctionOnCancel(EventHandler OnCancel);
  virtual void setFunctionOnActivate(EventHandler OnActivate);

  virtual int ShowDialog(void);
 /////////////////////////////////////////////////////

  bool IsRestartRequired(void);

 // Implementation
 protected:

  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  virtual BOOL OnInitDialog();
  virtual void OnCancel();
  virtual void OnOK();
  afx_msg void OnAppSettingsLogfolderButton();
  afx_msg void OnAppSettingsLogfolderUseappfolder();
  afx_msg void OnAppSettingsLogfolderUseDVFeatures();
  afx_msg void OnAppSettingsPresentPorts();
  afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd *pWnd, UINT nCtlColor);
  afx_msg void OnSelendokRestartPerameters();
  afx_msg void OnSelendokPlatform();
  afx_msg void OnDropDownPortSel();
  afx_msg BOOL OnDeviceChange(UINT nEventType, DWORD_PTR dwData);
  DECLARE_MESSAGE_MAP()

 private:
  void DDX_CBStringPort(CDataExchange* pDX, CComboBox* p_port_selection_combo, CString& value);
  _TSTRING _GetForExistingCBItem(const _TSTRING& portName);
  _TSTRING _GetForAllCBItem(const _TSTRING& portName);
  void _ShowCB(void);

  EventHandler m_OnOk;
  EventHandler m_OnCancel;
  EventHandler m_OnActivate;

  CComboBox m_port_selection1_combo;
  CComboBox m_port_selection2_combo;
  CComboBox* mp_port_selection_combo;
  CComboBox m_bl_baudrate_selection_combo;
  CComboBox m_app_baudrate_selection_combo;
  CComboBox m_log_csv_sepsymbol_combo;
  CComboBox m_iface_lang_selection_combo;
  CComboBox m_ecu_platform_selection_combo;
  CButton   m_log_files_folder_button;
  CButton   m_use_app_folder_button;
  CButton   m_always_write_log_button;
  CButton   m_write_log_fields_button;
  CButton   m_use_dv_features_button;
  CButton   m_show_tooltips_button;
  CButton   m_exfixtures_button;
  CButton   m_hexdatamode_button;
  CButton   m_presports_button;
  CButton   m_injdrvtab_button;
  CEdit     m_log_files_folder_edit;
  CEditEx   m_midesk_update_period_edit;
  CEditEx   m_dv_update_period_edit;
  CEditEx   m_tachometer_max_edit;
  CEditEx   m_pressure_max_edit;
  CEditEx   m_wheel_pulses_edit;
  CSpinButtonCtrlEx m_midesk_update_period_spin;
  CSpinButtonCtrlEx m_dv_update_period_spin;
  CSpinButtonCtrlEx m_tachometer_max_spin;
  CSpinButtonCtrlEx m_pressure_max_spin;
  CSpinButtonCtrlEx m_wheel_pulses_spin;
  CStatic m_dv_update_period_caption;
  CStatic m_info_text;
  int m_app_baudrate;
  int m_bl_baudrate;
  CString m_port_number;
  CString m_log_files_folder;
  int m_use_app_folder;
  int m_always_write_log;
  int m_write_log_fields;
  int m_use_dv_features;
  int m_show_tooltips;
  int m_exfixtures;
  int m_hexdatamode;
  int m_log_csv_sepsymbol_index;
  int m_midesk_update_period;
  int m_dv_update_period;
  int m_iface_lang_selection;
  int m_iface_lang_selection_orig;
  int m_ecu_platform_selection;
  int m_ecu_platform_selection_orig;
  int m_tachometer_max;
  int m_pressure_max;
  int m_wheel_pulses;
  int m_injdrvtab_active;

  std::auto_ptr<CToolTipCtrlEx> mp_ttc;
  CEnumPorts::PortDescList_t m_existingList;
};
