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

/** \file AppSettingsDlg.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "Resources/resource.h"
#include "AppSettingsDlg.h"
#include "ui-core/ToolTipCtrlEx.h"
#include "ui-core/XBrowseForFolder.h"
#include "io-core/EnumPorts.h"
#include <dbt.h>

const UINT CAppSettingsDlg::IDD = IDD_APP_SETTINGS;

namespace {
_TSTRING _ComposePortDescr(const CEnumPorts::PortDescItem_t& item)
{
 return _TSTRING(item.first + _T(", ") + item.second);
}
}

_TSTRING CAppSettingsDlg::_GetForExistingCBItem(const _TSTRING& portName)
{
 //try to select portName from existing list
 for(size_t i = 0; i < m_existingList.size(); ++i)
 {  
  if (m_existingList[i].first.c_str() == portName)
   return _ComposePortDescr(m_existingList[i]);
 }
 //try to select CP210x
 for(size_t i = 0; i < m_existingList.size(); ++i)
 {
  if (_tcsstr(m_existingList[i].second.c_str(), _T("Silicon Labs CP210x")))
   return _ComposePortDescr(m_existingList[i]);
 }
 return _T("");
}


_TSTRING CAppSettingsDlg::_GetForAllCBItem(const _TSTRING& portName)
{
 for(size_t i = 0; i < m_existingList.size(); ++i)
 {
  _TSTRING str = _ComposePortDescr(m_existingList[i]);
  if (str == portName)
   return m_existingList[i].first;
 }
 return (LPCTSTR)m_port_number;
}

void CAppSettingsDlg::DDX_CBStringPort(CDataExchange* pDX, CComboBox* p_port_selection_combo, CString& value)
{
 if (GetExistingPorts())
 {
  if (pDX->m_bSaveAndValidate)
  { //get
   CString cbStr;
   DDX_CBString(pDX, p_port_selection_combo->GetDlgCtrlID(), cbStr);
   value = _GetForAllCBItem((LPCTSTR)cbStr).c_str();
  }
  else //set
  {
   CString str = _GetForExistingCBItem((LPCTSTR)value).c_str();
   DDX_CBString(pDX, p_port_selection_combo->GetDlgCtrlID(), str);
  }
 }
 else
  DDX_CBString(pDX, p_port_selection_combo->GetDlgCtrlID(), value); //just use standard
}

CAppSettingsDlg::CAppSettingsDlg(CWnd* pParent /*=NULL*/)
: CDialog(CAppSettingsDlg::IDD, pParent)
, m_OnOk(NULL)
, m_OnCancel(NULL)
, m_OnActivate(NULL)
, m_midesk_update_period_edit(CEditEx::MODE_INT)
, m_dv_update_period_edit(CEditEx::MODE_INT)
, m_tachometer_max_edit(CEditEx::MODE_INT)
, m_pressure_max_edit(CEditEx::MODE_INT)
, m_wheel_pulses_edit(CEditEx::MODE_INT)
, mp_port_selection_combo(&m_port_selection1_combo)
{
 m_app_baudrate = -1;
 m_bl_baudrate = -1;
 m_port_number = _T("COM1");
 m_log_csv_sepsymbol_index = -1;
 m_log_files_folder = _T("");
 m_use_app_folder = BST_UNCHECKED;
 m_midesk_update_period = 40;
 m_tachometer_max = 0;
 m_pressure_max = 0;
 m_use_dv_features = BST_UNCHECKED;
 m_dv_update_period = 40;
 m_show_tooltips = BST_CHECKED;
 m_exfixtures = BST_CHECKED;
 m_hexdatamode = BST_CHECKED;
 m_wheel_pulses = 6000;   //Number of pulses per 1km
}

void CAppSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
 CDialog::DoDataExchange(pDX);
 DDX_Control(pDX, IDC_APP_SETTINGS_PORT_SELECTION1_COMBO, m_port_selection1_combo); //all
 DDX_Control(pDX, IDC_APP_SETTINGS_PORT_SELECTION2_COMBO, m_port_selection2_combo); //existing

 DDX_Control(pDX, IDC_APP_SETTINGS_BL_BAUDRATE_SELECTION_COMBO, m_bl_baudrate_selection_combo);
 DDX_Control(pDX, IDC_APP_SETTINGS_APP_BAUDRATE_SELECTION_COMBO, m_app_baudrate_selection_combo);
 DDX_Control(pDX, IDC_APP_SETTINGS_LOG_CSV_SEPSYMBOL_COMBO, m_log_csv_sepsymbol_combo);
 DDX_Control(pDX, IDC_APP_SETTINGS_PLATFORM_SEL_COMBO, m_ecu_platform_selection_combo);
 DDX_Control(pDX, IDC_APP_SETTINGS_LANG_SEL_COMBO, m_iface_lang_selection_combo);

 DDX_Control(pDX, IDC_APP_SETTINGS_LOGFOLDER_EDITBOX, m_log_files_folder_edit);
 DDX_Control(pDX, IDC_APP_SETTINGS_LOGFOLDER_BUTTON, m_log_files_folder_button);
 DDX_Control(pDX, IDC_APP_SETTINGS_LOGFOLDER_USEAPPFOLDER, m_use_app_folder_button);
 DDX_Control(pDX, IDC_APP_SETTINGS_ALWAYS_WRITE_LOG, m_always_write_log_button);
 DDX_Control(pDX, IDC_APP_SETTINGS_USEDEBUG_FEATURES, m_use_dv_features_button);
 DDX_Control(pDX, IDC_APP_SETTINGS_SHOW_TOOLTIPS, m_show_tooltips_button);
 DDX_Control(pDX, IDC_APP_SETTINGS_EXFIXTURES, m_exfixtures_button);
 DDX_Control(pDX, IDC_APP_SETTINGS_HEXMODE, m_hexdatamode_button);
 DDX_Control(pDX, IDC_APP_SETTINGS_PRESPORTS, m_presports_button);

 DDX_CBIndex(pDX, IDC_APP_SETTINGS_APP_BAUDRATE_SELECTION_COMBO, m_app_baudrate);
 DDX_CBIndex(pDX, IDC_APP_SETTINGS_BL_BAUDRATE_SELECTION_COMBO, m_bl_baudrate);
 DDX_CBStringPort(pDX, mp_port_selection_combo, m_port_number);
 DDX_CBIndex(pDX, IDC_APP_SETTINGS_LOG_CSV_SEPSYMBOL_COMBO, m_log_csv_sepsymbol_index);

 DDX_CBIndex(pDX, IDC_APP_SETTINGS_PLATFORM_SEL_COMBO, m_ecu_platform_selection);
 DDX_CBIndex(pDX, IDC_APP_SETTINGS_LANG_SEL_COMBO, m_iface_lang_selection);

 DDX_Text(pDX, IDC_APP_SETTINGS_LOGFOLDER_EDITBOX, m_log_files_folder);
 DDX_Check(pDX, IDC_APP_SETTINGS_LOGFOLDER_USEAPPFOLDER, m_use_app_folder);
 DDX_Check(pDX, IDC_APP_SETTINGS_ALWAYS_WRITE_LOG, m_always_write_log);
 DDX_Check(pDX, IDC_APP_SETTINGS_USEDEBUG_FEATURES, m_use_dv_features);
 DDX_Check(pDX, IDC_APP_SETTINGS_SHOW_TOOLTIPS, m_show_tooltips);
 DDX_Check(pDX, IDC_APP_SETTINGS_EXFIXTURES, m_exfixtures);
 DDX_Check(pDX, IDC_APP_SETTINGS_HEXMODE, m_hexdatamode);

 DDX_Control(pDX, IDC_APP_SETTINGS_MIDESK_UPDATE_PERIOD_SPIN, m_midesk_update_period_spin);
 DDX_Control(pDX, IDC_APP_SETTINGS_MIDESK_UPDATE_PERIOD_EDIT, m_midesk_update_period_edit);
 m_midesk_update_period_edit.DDX_Value(pDX, IDC_APP_SETTINGS_MIDESK_UPDATE_PERIOD_EDIT, m_midesk_update_period);

 DDX_Control(pDX, IDC_APP_SETTINGS_DBGPANEL_UPDATE_PERIOD_SPIN, m_dv_update_period_spin);
 DDX_Control(pDX, IDC_APP_SETTINGS_DBGPANEL_UPDATE_PERIOD_EDIT, m_dv_update_period_edit);
 m_dv_update_period_edit.DDX_Value(pDX, IDC_APP_SETTINGS_DBGPANEL_UPDATE_PERIOD_EDIT, m_dv_update_period);

 DDX_Control(pDX, IDC_APP_SETTINGS_TACHOMETER_MAX_SPIN, m_tachometer_max_spin);
 DDX_Control(pDX, IDC_APP_SETTINGS_TACHOMETER_MAX_EDIT, m_tachometer_max_edit);
 m_tachometer_max_edit.DDX_Value(pDX, IDC_APP_SETTINGS_TACHOMETER_MAX_EDIT, m_tachometer_max);

 DDX_Control(pDX, IDC_APP_SETTINGS_PRESSURE_MAX_SPIN, m_pressure_max_spin);
 DDX_Control(pDX, IDC_APP_SETTINGS_PRESSURE_MAX_EDIT, m_pressure_max_edit);
 m_pressure_max_edit.DDX_Value(pDX, IDC_APP_SETTINGS_PRESSURE_MAX_EDIT, m_pressure_max);

 DDX_Control(pDX, IDC_APP_SETTINGS_WHEEL_PULSES_SPIN, m_wheel_pulses_spin);
 DDX_Control(pDX, IDC_APP_SETTINGS_WHEEL_PULSES_EDIT, m_wheel_pulses_edit);
 m_wheel_pulses_edit.DDX_Value(pDX, IDC_APP_SETTINGS_WHEEL_PULSES_EDIT, m_wheel_pulses);

 DDX_Control(pDX, IDC_APP_SETTINGS_DBGPANEL_UPDATE_PERIOD_CAPTION, m_dv_update_period_caption);
 DDX_Control(pDX, IDC_APP_SETTINGS_INFO_TEXT, m_info_text);
}


BEGIN_MESSAGE_MAP(CAppSettingsDlg, CDialog)
 ON_WM_CTLCOLOR()
 ON_WM_DEVICECHANGE()
 ON_BN_CLICKED(IDC_APP_SETTINGS_LOGFOLDER_BUTTON, OnAppSettingsLogfolderButton)
 ON_BN_CLICKED(IDC_APP_SETTINGS_LOGFOLDER_USEAPPFOLDER, OnAppSettingsLogfolderUseappfolder)
 ON_BN_CLICKED(IDC_APP_SETTINGS_USEDEBUG_FEATURES, OnAppSettingsLogfolderUseDVFeatures)
 ON_BN_CLICKED(IDC_APP_SETTINGS_PRESPORTS, OnAppSettingsPresentPorts)
 ON_CBN_SELENDOK(IDC_APP_SETTINGS_LANG_SEL_COMBO, OnSelendokRestartPerameters)
 ON_CBN_SELENDOK(IDC_APP_SETTINGS_PLATFORM_SEL_COMBO, OnSelendokRestartPerameters)
 ON_CBN_DROPDOWN(IDC_APP_SETTINGS_PORT_SELECTION2_COMBO, OnDropDownPortSel)
END_MESSAGE_MAP()

HBRUSH CAppSettingsDlg::OnCtlColor(CDC* pDC, CWnd *pWnd, UINT nCtlColor)
{
 if (pWnd->m_hWnd == m_info_text.m_hWnd && nCtlColor == CTLCOLOR_STATIC)
 {
  pDC->SetTextColor(RGB(255, 0, 0));
  pDC->SetBkMode(TRANSPARENT);
  return (HBRUSH)GetStockObject(NULL_BRUSH);
 }

 return CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
}

void CAppSettingsDlg::OnSelendokRestartPerameters()
{
 UpdateData();
 bool program_restart_required = IsRestartRequired();
 
 m_info_text.SetWindowText(program_restart_required ? MLL::GetString(IDS_APP_SETTINGS_PRG_RESTART_REQ).c_str() : _T(""));
 m_info_text.ShowWindow(program_restart_required ? SW_SHOW : SW_HIDE); 
}

bool CAppSettingsDlg::IsRestartRequired(void)
{
 bool resreq = m_iface_lang_selection_orig != m_iface_lang_selection || m_ecu_platform_selection_orig != m_ecu_platform_selection;
 return resreq;
}

BOOL CAppSettingsDlg::OnInitDialog()
{
 CDialog::OnInitDialog();

 m_port_selection1_combo.LimitText(200);

 m_midesk_update_period_edit.SetLimitText(4);
 m_midesk_update_period_spin.SetBuddy(&m_midesk_update_period_edit);
 m_midesk_update_period_spin.SetRangeAndDelta(10,1000,10);

 m_dv_update_period_edit.SetLimitText(4);
 m_dv_update_period_spin.SetBuddy(&m_dv_update_period_edit);
 m_dv_update_period_spin.SetRangeAndDelta(10,1000,10);

 m_tachometer_max_edit.SetLimitText(5);
 m_tachometer_max_spin.SetBuddy(&m_tachometer_max_edit);
 m_tachometer_max_spin.SetRangeAndDelta(1000, 15000, 10);

 m_pressure_max_edit.SetLimitText(5);
 m_pressure_max_spin.SetBuddy(&m_pressure_max_edit);
 m_pressure_max_spin.SetRangeAndDelta(50, 500, 10);

 m_wheel_pulses_edit.SetLimitText(5);
 m_wheel_pulses_edit.SetDecimalPlaces(5);
 m_wheel_pulses_spin.SetBuddy(&m_wheel_pulses_edit);
 m_wheel_pulses_spin.SetRangeAndDelta(0, 30000, 1);

 if (m_OnActivate)
  m_OnActivate(); //информируем слушателя о том, что мы готовы к приему данных

 _ShowCB(); //select right combo box
 UpdateData(FALSE);

 OnAppSettingsLogfolderUseappfolder();
 OnAppSettingsLogfolderUseDVFeatures();

 mp_ttc.reset(new CToolTipCtrlEx());
 VERIFY(mp_ttc->Create(this, WS_POPUP | TTS_ALWAYSTIP | TTS_BALLOON));
 VERIFY(mp_ttc->AddWindow(&m_port_selection1_combo, MLL::GetString(IDS_APP_SETTINGS_PORT_SELECTION_COMBO_TT)));
 VERIFY(mp_ttc->AddWindow(&m_port_selection2_combo, MLL::GetString(IDS_APP_SETTINGS_PORT_SELECTION_COMBO_TT)));
 VERIFY(mp_ttc->AddWindow(&m_app_baudrate_selection_combo, MLL::GetString(IDS_APP_SETTINGS_APP_BAUDRATE_SELECTION_COMBO_TT)));
 VERIFY(mp_ttc->AddWindow(&m_bl_baudrate_selection_combo, MLL::GetString(IDS_APP_SETTINGS_BL_BAUDRATE_SELECTION_COMBO_TT)));
 mp_ttc->SetMaxTipWidth(250); //Enable text wrapping
 mp_ttc->ActivateToolTips(true);

 return TRUE;  // return TRUE unless you set the focus to a control
}

void CAppSettingsDlg::OnCancel()
{
 UpdateData();
 if (m_OnCancel) m_OnCancel();
  CDialog::OnCancel();
}

void CAppSettingsDlg::OnOK()
{
 UpdateData();
 if (m_OnOk) m_OnOk();
  CDialog::OnOK();
}

//обзор каталога
void CAppSettingsDlg::OnAppSettingsLogfolderButton()
{
 UpdateData();
 TCHAR szFolder[MAX_PATH*2] = { _T('\0') };
 LPCTSTR lpszInitial = m_log_files_folder.GetBuffer(MAX_PATH);
 BOOL bEditBox = TRUE;

 BOOL bRet = XBrowseForFolder(m_hWnd,
                              lpszInitial,
                              -1,
                              MLL::LoadString(IDS_SELECT_FOLDER_FOR_LOG_FILES),
                              szFolder,
                              sizeof(szFolder)/sizeof(TCHAR)-2,
                              bEditBox);

 if (bRet)
 {
  m_log_files_folder = szFolder;
  UpdateData(FALSE);
 }
}

void CAppSettingsDlg::OnAppSettingsLogfolderUseappfolder()
{
 UpdateData();
 m_log_files_folder_edit.EnableWindow(m_use_app_folder==BST_UNCHECKED);
 m_log_files_folder_button.EnableWindow(m_use_app_folder==BST_UNCHECKED);
}

void CAppSettingsDlg::OnAppSettingsLogfolderUseDVFeatures()
{
 UpdateData();
 m_dv_update_period_edit.EnableWindow(m_use_dv_features==BST_CHECKED);
 m_dv_update_period_spin.EnableWindow(m_use_dv_features==BST_CHECKED);
 m_dv_update_period_caption.EnableWindow(m_use_dv_features==BST_CHECKED);
}

void CAppSettingsDlg::FillCtrlsWithAllowableBaudRates(std::vector<DWORD> i_AllowableBaudRates)
{
 for(size_t i = 0; i < i_AllowableBaudRates.size(); i++)
 {
  CString str;
  str.Format(_T("%d"),i_AllowableBaudRates[i]);
  m_bl_baudrate_selection_combo.AddString(str);
  m_app_baudrate_selection_combo.AddString(str);
 }
}

void CAppSettingsDlg::FillCtrlsWithAllowablePorts(std::vector<_TSTRING> i_AllowablePorts)
{
 //fill combo2
 CEnumPorts ep;
 BeginWaitCursor();
 VERIFY(ep.Query());
 EndWaitCursor();
 ep.getPortsList(m_existingList);
 m_port_selection2_combo.ResetContent();
 for(size_t i = 0; i < m_existingList.size(); ++i)
 {
  _TSTRING str = _ComposePortDescr(m_existingList[i]);
  m_port_selection2_combo.AddString(str.c_str());
 }
 //fill combo1
 for(size_t i = 0; i < i_AllowablePorts.size(); i++)
  m_port_selection1_combo.AddString(i_AllowablePorts[i].c_str());
}

void CAppSettingsDlg::FillCtrlsWithAllowableCSVSepSymbols(std::vector<std::pair<_TSTRING, char> >  i_AllowableCSVSepSymbols)
{
 for(size_t i = 0; i < i_AllowableCSVSepSymbols.size(); i++)
 {
  int index = m_log_csv_sepsymbol_combo.AddString(i_AllowableCSVSepSymbols[i].first.c_str());
  ASSERT(index != LB_ERR);
  m_log_csv_sepsymbol_combo.SetItemData(index, i);
 }
}

void CAppSettingsDlg::FillCtrlsWithAllowableInterfaceLanguages(std::vector<std::pair<std::pair<_TSTRING, _TSTRING>, int> > i_AllowableInterfaceLanguages)
{
 for(size_t i = 0; i < i_AllowableInterfaceLanguages.size(); ++i)
 {
  int index = m_iface_lang_selection_combo.AddString(i_AllowableInterfaceLanguages[i].first.first.c_str());
  ASSERT(index != LB_ERR);
  m_iface_lang_selection_combo.SetItemData(i_AllowableInterfaceLanguages[i].second, i);
 }
}

void CAppSettingsDlg::FillCtrlsWithAllowableECUPlatformTypes(std::vector<std::pair<std::pair<_TSTRING, _TSTRING>, int> > i_AllowableECUPlatformTypes)
{
 for(size_t i = 0; i < i_AllowableECUPlatformTypes.size(); ++i)
 {
  int index = m_ecu_platform_selection_combo.AddString(i_AllowableECUPlatformTypes[i].first.first.c_str());
  ASSERT(index != LB_ERR);
  m_ecu_platform_selection_combo.SetItemData(i, i_AllowableECUPlatformTypes[i].second);
 }
}

//"Set" methods (model => view data transfer)
void CAppSettingsDlg::SetPortName(_TSTRING i_PortName)
{
 m_port_number = i_PortName.c_str();
}

void CAppSettingsDlg::SetBaudRateApplication(DWORD i_app_baud)
{
 CString str;
 str.Format(_T("%d"),i_app_baud);
 int result = m_app_baudrate_selection_combo.FindString(-1,str);
 if (result!=LB_ERR)
  m_app_baudrate = result;
}

void CAppSettingsDlg::SetBaudRateBootloader(DWORD i_bl_baud)
{
 CString str;
 str.Format(_T("%d"),i_bl_baud);
 int result = m_bl_baudrate_selection_combo.FindString(-1,str);
 if (result!=LB_ERR)
  m_bl_baudrate = result;
}

void CAppSettingsDlg::SetCSVSepSymbol(size_t i_index)
{
 int count = m_log_csv_sepsymbol_combo.GetCount();
 for (int i = 0; i < count; i++)
 {
  size_t index = m_log_csv_sepsymbol_combo.GetItemData(i);
  if (index == i_index)
  { //found!
   m_log_csv_sepsymbol_index = i;
   return;
  }
 }
 ASSERT(0); //WTF...
}

//"Get" methods (view => model data transfer)
_TSTRING CAppSettingsDlg::GetPortName(void)
{
 return (LPCTSTR)m_port_number;
}

DWORD CAppSettingsDlg::GetBaudRateApplication(void)
{
 CString str;
 m_app_baudrate_selection_combo.GetLBText(m_app_baudrate,str);
 return _ttoi(str);
}

DWORD CAppSettingsDlg::GetBaudRateBootloader(void)
{
 CString str;
 m_bl_baudrate_selection_combo.GetLBText(m_bl_baudrate,str);
 return _ttoi(str);
}

void CAppSettingsDlg::SetLogFilesFolder(const _TSTRING& i_folder)
{
 m_log_files_folder = i_folder.c_str();
}

CString CAppSettingsDlg::GetLogFilesFolder(void) const
{
 return m_log_files_folder;
}

void CAppSettingsDlg::SetUseAppFolder(bool i_use)
{
 m_use_app_folder = i_use ? BST_CHECKED : BST_UNCHECKED;
}

bool CAppSettingsDlg::GetUseAppFolder(void) const
{
 return (m_use_app_folder == BST_CHECKED) ? true : false;
}

void CAppSettingsDlg::SetAlwaysWriteLog(bool i_always)
{
 m_always_write_log = i_always ? BST_CHECKED : BST_UNCHECKED;
}

bool CAppSettingsDlg::GetAlwaysWriteLog(void) const
{
 return (m_always_write_log == BST_CHECKED) ? true : false;
}

size_t CAppSettingsDlg::GetCSVSepSymbol(void)
{
 return m_log_csv_sepsymbol_combo.GetItemData(m_log_csv_sepsymbol_index);
}

void CAppSettingsDlg::setFunctionOnOk(EventHandler OnOk)
{
 m_OnOk = OnOk;
}

void CAppSettingsDlg::setFunctionOnCancel(EventHandler OnCancel)
{
 m_OnCancel = OnCancel;
}

void CAppSettingsDlg::setFunctionOnActivate(EventHandler OnActivate)
{
 m_OnActivate = OnActivate;
}

int CAppSettingsDlg::ShowDialog(void)
{
 return DoModal();
}

void CAppSettingsDlg::SetMIDeskUpdatePeriod(int i_period)
{
 m_midesk_update_period = i_period;
}

int CAppSettingsDlg::GetMIDeskUpdatePeriod(void)
{
 return m_midesk_update_period;
}

void CAppSettingsDlg::SetInterfaceLanguage(int i_iface_lang)
{
 int count = m_iface_lang_selection_combo.GetCount();
 for (int i = 0; i < count; i++)
 {
  size_t id = m_iface_lang_selection_combo.GetItemData(i);
  if (id == i_iface_lang)
  { //found!
   m_iface_lang_selection = i;
   m_iface_lang_selection_orig = i;
   return;
  }
 }
 ASSERT(0); //WTF...
}

void CAppSettingsDlg::SetECUPlatformType(int i_platform_type)
{
 int count = m_ecu_platform_selection_combo.GetCount();
 for (int i = 0; i < count; i++)
 {
  size_t id = m_ecu_platform_selection_combo.GetItemData(i);
  if (id == i_platform_type)
  { //found!
   m_ecu_platform_selection = i;
   m_ecu_platform_selection_orig = i;
   return;
  }
 }
 ASSERT(0); //WTF...
}

int CAppSettingsDlg::GetInterfaceLanguage(void) const
{
 return m_iface_lang_selection_combo.GetItemData(m_iface_lang_selection);
}

int CAppSettingsDlg::GetECUPlatformType(void) const
{
 return m_ecu_platform_selection_combo.GetItemData(m_ecu_platform_selection);
}

void CAppSettingsDlg::SetTachometerMax(int i_max)
{
 m_tachometer_max = i_max;
}

void CAppSettingsDlg::SetPressureMax(int i_max)
{
 m_pressure_max = i_max;
}

int CAppSettingsDlg::GetTachometerMax(void) const
{
 return m_tachometer_max;
}

int CAppSettingsDlg::GetPressureMax(void) const
{
 return m_pressure_max;
}

void CAppSettingsDlg::SetUseDVFeatures(bool i_use)
{
 m_use_dv_features = i_use ? BST_CHECKED : BST_UNCHECKED;
}

void CAppSettingsDlg::SetDVDeskUpdatePeriod(int i_period)
{
 m_dv_update_period = i_period;
}

bool CAppSettingsDlg::GetUseDVFeatures(void)
{
 return (m_use_dv_features == BST_CHECKED) ? true : false;
}

int CAppSettingsDlg::GetDVDeskUpdatePeriod(void)
{
 return m_dv_update_period;
}

void CAppSettingsDlg::SetShowToolTips(bool i_show)
{
 m_show_tooltips = i_show ? BST_CHECKED : BST_UNCHECKED;
}

bool CAppSettingsDlg::GetShowToolTips(void) const
{
 return (m_show_tooltips == BST_CHECKED) ? true : false;
}

void CAppSettingsDlg::SetShowExFixtures(bool i_show)
{
 m_exfixtures = i_show ? BST_CHECKED : BST_UNCHECKED;
}

bool CAppSettingsDlg::GetShowExFixtures(void) const
{
 return (m_exfixtures == BST_CHECKED) ? true : false;
}

void CAppSettingsDlg::SetHexDataMode(bool i_hex)
{
 m_hexdatamode = i_hex ? BST_CHECKED : BST_UNCHECKED;
}

bool CAppSettingsDlg::GetHexDataMode(void) const
{
 return (m_hexdatamode == BST_CHECKED) ? true : false;
}

void CAppSettingsDlg::SetNumPulsesPer1Km(int i_pp1km)
{
 m_wheel_pulses = i_pp1km;
}

int CAppSettingsDlg::GetNumPulsesPer1Km(void) const
{
 return m_wheel_pulses;
}

void CAppSettingsDlg::_ShowCB(void)
{
 if (m_presports_button.GetCheck() == BST_CHECKED)
 {
  mp_port_selection_combo = &m_port_selection2_combo;
  m_port_selection1_combo.ShowWindow(SW_HIDE);
  m_port_selection2_combo.ShowWindow(SW_SHOW);
  m_port_selection2_combo.EnableWindow(m_existingList.size() > 0); //disable if list of existing ports is empty
 }
 else
 {
  mp_port_selection_combo = &m_port_selection1_combo;
  m_port_selection1_combo.ShowWindow(SW_SHOW);
  m_port_selection2_combo.ShowWindow(SW_HIDE);
 }
}

void CAppSettingsDlg::OnAppSettingsPresentPorts()
{
 _ShowCB();
 CString str;
 if (m_presports_button.GetCheck() == BST_CHECKED)
 {
  m_port_selection1_combo.GetWindowText(str);
  m_port_selection2_combo.SelectString(0,_GetForExistingCBItem((LPCTSTR)str).c_str());
 }
 else
 {
  m_port_selection2_combo.GetWindowText(str);
  m_port_selection1_combo.SetWindowText(_GetForAllCBItem((LPCTSTR)str).c_str());
 }
 UpdateData(TRUE); //get
}

void CAppSettingsDlg::OnDropDownPortSel()
{
 //update list of ports on each opening of combo box, we also preserve selection
 CEnumPorts ep;
 BeginWaitCursor();
 VERIFY(ep.Query());
 EndWaitCursor();
 ep.getPortsList(m_existingList);
 CString str;
 m_port_selection2_combo.GetWindowText(str); //save selection
 m_port_selection2_combo.ResetContent();
 for(size_t i = 0; i < m_existingList.size(); ++i)
  m_port_selection2_combo.AddString(_ComposePortDescr(m_existingList[i]).c_str());

 if (str.IsEmpty())
 { //select m_port_number if there are no selection information
  m_port_selection2_combo.SelectString(0,_GetForExistingCBItem((LPCTSTR)m_port_number).c_str());
 }
 else
  m_port_selection2_combo.SelectString(0, str); //rescue selection
}

void CAppSettingsDlg::SetExistingPorts(bool i_exp)
{
 m_presports_button.SetCheck(i_exp ? BST_CHECKED : BST_UNCHECKED);
}

bool CAppSettingsDlg::GetExistingPorts(void) const
{
 return m_presports_button.GetCheck() == BST_CHECKED;
}

BOOL CAppSettingsDlg::OnDeviceChange(UINT nEventType, DWORD_PTR dwData)
{
 if (DBT_DEVICEARRIVAL==nEventType)
 {
  if (DBT_DEVTYP_PORT==((DEV_BROADCAST_HDR*)dwData)->dbch_devicetype)
  {
   UpdateData(TRUE); //get

   OnDropDownPortSel();
   m_port_selection2_combo.EnableWindow(m_existingList.size() > 0); //disable if list of existing ports is empty
  }
 }
 else if (DBT_DEVICEREMOVECOMPLETE==nEventType)
 {
  if (DBT_DEVTYP_PORT==((DEV_BROADCAST_HDR*)dwData)->dbch_devicetype)
  {
   UpdateData(TRUE); //get

   OnDropDownPortSel();
   m_port_selection2_combo.EnableWindow(m_existingList.size() > 0); //disable if list of existing ports is empty
  }
 }

 return TRUE;
}
