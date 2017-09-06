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

/** \file FirmwareTabDlg.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "Resources/resource.h"
#include "FirmwareTabDlg.h"

#include "about/secu-3about.h"
#include "common/MathHelpers.h"
#include "FirmwareContextMenuManager.h"
#include "ParamDesk/Params/IDeskView.h"
#include "ParamDesk/Params/IORemappingDlg.h"
#include "ParamDesk/Params/ParamDeskDlg.h"
#include "tabldesk/TablesSetPanel.h"
#include "ui-core/fnt_helpers.h"
#include "ui-core/HotKeysToCmdRouter.h"
#include "ui-core/ToolTipCtrlEx.h"

const int TIMER_ID = 1;

const UINT CFirmwareTabDlg::IDD = IDD_FIRMWARE_SUPPORT;

//Tab IDs of parameters selection tab. Note that order of beneath IDs must
//correspond to AddPage() calling order.
enum PSTabId
{
 PSTID_DEF_PARAMETERS = 0,
 PSTID_IO_REMAPPING
};

/////////////////////////////////////////////////////////////////////////////
// CFirmwareTabDlg dialog

CFirmwareTabDlg::CFirmwareTabDlg(CWnd* pParent /*=NULL*/)
: Super(CFirmwareTabDlg::IDD, pParent)
, m_is_bl_started_emergency_available(false)
, m_is_bl_items_available(false)
, m_is_app_items_available(false)
, mp_ParamDeskDlg(new CParamDeskDlg(NULL, true)) //<-- используем вкладку параметров детонации
, mp_IORemappingDlg(new CIORemappingDlg())
, m_hot_keys_supplier(new CHotKeysToCmdRouter())
, mp_ContextMenuManager(new CFirmwareModeContextMenuManager())
, mp_TablesPanel(new CTablesSetPanel(NULL))
, m_tab_selection(0) //<-- "default parameters" is selected by default
{
 mp_ContextMenuManager->CreateContent();
 //create list of tabs
 m_tabs.insert(std::make_pair(PSTID_DEF_PARAMETERS, std::make_pair(mp_ParamDeskDlg.get(), MLL::GetString(IDS_FW_DEF_PARAMETR))));
 m_tabs.insert(std::make_pair(PSTID_IO_REMAPPING, std::make_pair(mp_IORemappingDlg.get(), MLL::GetString(IDS_FW_IO_REMAPPING))));
 
 //=================================================================
 if (!CheckBitmaps() || !CheckAppMenu() || !CheckAppLogo() || !CheckAbout())
  delete this;
 //=================================================================
}

void CFirmwareTabDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);
 DDX_Control(pDX, IDC_FW_BL_STARTED_EMERGENCY, m_bl_started_emergency);
 DDX_Control(pDX, IDC_FW_VIEW_FWOPT, m_fw_options_btn);
 DDX_Control(pDX, IDC_FW_FW_INFORMATION,m_fw_information_edit);
 DDX_Control(pDX, IDC_FW_FW_NAME,m_fw_name);
 DDX_Control(pDX, IDC_FW_CRC, m_fw_crc);
 DDX_Control(pDX, IDC_FW_MODIFICATION_FLAG, m_modification_flag);
 DDX_Control(pDX, IDC_FW_PROG_ONLY_CODE, m_prog_only_code_checkbox);
 DDX_Control(pDX, IDC_FW_PARAM_SEL_TAB, m_param_sel_tab);
}

LPCTSTR CFirmwareTabDlg::GetDialogID(void) const
{
 return (LPCTSTR)IDD;
}

BEGIN_MESSAGE_MAP(CFirmwareTabDlg, Super)
 ON_BN_CLICKED(IDC_FW_POPUPMENU_BUTTON, OnPopupMenuButton)
 ON_WM_CONTEXTMENU()
 ON_UPDATE_COMMAND_UI(IDM_READ_FLASH, OnUpdatePopupMenu_bl)
 ON_UPDATE_COMMAND_UI(IDM_OPEN_FLASH, OnUpdatePopupMenu_file)
 ON_WM_INITMENUPOPUP()
 ON_WM_TIMER()
 ON_WM_DESTROY()
 ON_COMMAND(IDM_READ_BOOTLOADER_SIGNATURE, OnBootLoaderInfo)
 ON_COMMAND(IDM_READ_EEPROM_TO_FILE, OnReadEepromToFile)
 ON_COMMAND(IDM_WRITE_EEPROM_FROM_FILE, OnWriteEepromFromFile)
 ON_COMMAND(IDM_RESET_EEPROM, OnResetEeprom)
 ON_UPDATE_COMMAND_UI(IDM_RESET_EEPROM, OnUpdatePopupMenu_app)
 ON_COMMAND(IDM_READ_FLASH_TO_FILE, OnReadFlashToFile)
 ON_COMMAND(IDM_WRITE_FLASH_FROM_FILE, OnWriteFlashFromFile)
 ON_UPDATE_COMMAND_UI(IDC_FW_BL_STARTED_EMERGENCY, OnUpdateBLStartedEmergency)
 ON_BN_CLICKED(IDC_FW_BL_STARTED_EMERGENCY, OnFirmwareSupportBlStartedEmergency)
 ON_COMMAND(IDM_OPEN_FLASH, OnOpenFlashFromFile)
 ON_COMMAND(IDM_SAVE_FLASH, OnSaveFlashToFile)
 ON_EN_CHANGE(IDC_FW_FW_INFORMATION, OnChangeFirmwareSupportFwInformation)
 ON_UPDATE_COMMAND_UI(IDC_FW_FW_INFORMATION, OnUpdateFirmwareControls)

 ON_UPDATE_COMMAND_UI(IDC_FW_FW_INFORMATION_CAPTION, OnUpdateFirmwareControls)
 ON_UPDATE_COMMAND_UI(IDC_FW_CRC, OnUpdateFirmwareControls)
 ON_UPDATE_COMMAND_UI(IDC_FW_FW_NAME, OnUpdateFirmwareControls)

 ON_UPDATE_COMMAND_UI(IDM_WRITE_FLASH, OnUpdatePopupMenu_bl)
 ON_UPDATE_COMMAND_UI(IDM_SAVE_FLASH, OnUpdatePopupMenu_file1)
 ON_UPDATE_COMMAND_UI(IDM_READ_FLASH_TO_FILE, OnUpdatePopupMenu_bl)
 ON_UPDATE_COMMAND_UI(IDM_WRITE_FLASH_FROM_FILE, OnUpdatePopupMenu_bl)
 ON_UPDATE_COMMAND_UI(IDM_READ_EEPROM_TO_FILE, OnUpdatePopupMenu_bl)
 ON_UPDATE_COMMAND_UI(IDM_WRITE_EEPROM_FROM_FILE, OnUpdatePopupMenu_bl)
 ON_UPDATE_COMMAND_UI(IDM_READ_BOOTLOADER_SIGNATURE, OnUpdatePopupMenu_bl)

 ON_UPDATE_COMMAND_UI(IDM_IMPORT_FW_DATA_FROM_ANOTHER_FW, OnUpdatePopupMenu_file1)
 ON_COMMAND(IDM_IMPORT_FW_DATA_FROM_ANOTHER_FW,OnImportDataFromAnotherFW)
 ON_COMMAND(IDM_IMPORT_FW_DATA_FROM_SECU3,OnImportDataFromSECU3)
 ON_COMMAND(IDM_IMPORT_DEF_PARAMS_FROM_EEPROM_FILE, OnImportDefParamsFromEEPROMFile)
 ON_COMMAND(IDM_IMPORT_TABLES_FROM_EEPROM_FILE, OnImportTablesFromEEPROMFile)

 ON_COMMAND(IDM_READ_FLASH,OnReadFlashFromSECU)
 ON_COMMAND(IDM_WRITE_FLASH,OnWriteFlashToSECU)

 ON_UPDATE_COMMAND_UI(IDM_IMPORT_FW_DATA_FROM_SECU3, OnUpdatePopupMenu_bl)
 ON_UPDATE_COMMAND_UI(IDC_FW_PROG_ONLY_CODE, OnUpdateProgOnlyCode)
 ON_UPDATE_COMMAND_UI(IDM_IMPORT_IMPORT_FROM_MPSZ, OnUpdatePopupMenu_file1)
 ON_UPDATE_COMMAND_UI(IDM_IMPORT_IMPORT_FROM_SECU3, OnUpdatePopupMenu_file1)
 ON_UPDATE_COMMAND_UI(IDM_IMPORT_IMPORT_FROM_S3F, OnUpdatePopupMenu_file1)
 ON_UPDATE_COMMAND_UI(IDM_EXPORT_EXPORT_TO_MPSZ, OnUpdatePopupMenu_file1)
 ON_UPDATE_COMMAND_UI(IDM_EXPORT_EXPORT_TO_SECU3, OnUpdatePopupMenu_file1)
 ON_UPDATE_COMMAND_UI(IDM_EXPORT_EXPORT_TO_S3F, OnUpdatePopupMenu_file1)
 ON_UPDATE_COMMAND_UI(IDM_IMPORT_DEF_PARAMS_FROM_EEPROM_FILE, OnUpdatePopupMenu_file1)
 ON_UPDATE_COMMAND_UI(IDM_IMPORT_TABLES_FROM_EEPROM_FILE, OnUpdatePopupMenu_file1)

 ON_COMMAND(IDM_IMPORT_IMPORT_FROM_MPSZ, OnImportMapsFromMPSZ)
 ON_COMMAND(IDM_IMPORT_IMPORT_FROM_SECU3, OnImportMapsFromSECU3)
 ON_COMMAND(IDM_IMPORT_IMPORT_FROM_S3F, OnImportMapsFromS3F)
 ON_COMMAND(IDM_EXPORT_EXPORT_TO_MPSZ, OnExportMapsToMPSZ)
 ON_COMMAND(IDM_EXPORT_EXPORT_TO_SECU3, OnExportMapsToSECU3)
 ON_COMMAND(IDM_EXPORT_EXPORT_TO_S3F, OnExportMapsToS3F)

 ON_COMMAND(IDM_READ_FW_SIGNATURE_INFO, OnFirmwareInfo)
 ON_UPDATE_COMMAND_UI(IDM_READ_FW_SIGNATURE_INFO, OnUpdatePopupMenu_app)

 ON_UPDATE_COMMAND_UI(IDC_FW_VIEW_FWOPT, OnUpdateFirmwareSupportViewFWOptions)
 ON_BN_CLICKED(IDC_FW_VIEW_FWOPT, OnViewFWOptions)

 ON_WM_DROPFILES()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFirmwareTabDlg message handlers

BOOL CFirmwareTabDlg::OnInitDialog()
{
 Super::OnInitDialog();

 //Prepare and tune tab control (See PSTabId enum)
 std::map<int, std::pair<IDeskView*, _TSTRING> >::const_iterator it;
 for(it = m_tabs.begin(); it != m_tabs.end(); ++it)
  m_param_sel_tab.AddPage(it->second.second.c_str(), NULL);
 m_param_sel_tab.SetEventListener(this);
 m_param_sel_tab.EnableItem(-1, false);

 //avoid bugs on wine (vertical "MS Sans Serif" is not rendered correctly), specify 270 degree angle
 CloneWndFont(GetDlgItem(IDC_FW_CRC), &m_tab_selection_fnt, 0, false, _T("MS Serif"), 2700);
 m_param_sel_tab.SetFont(&m_tab_selection_fnt);

 //create parameters desk
 CRect rect;
 GetDlgItem(IDC_FW_PD_FRAME)->GetWindowRect(rect);
 ScreenToClient(rect);
 mp_ParamDeskDlg->Create(CParamDeskDlg::IDD,this);
 mp_ParamDeskDlg->SetPosition(rect.TopLeft().x,rect.TopLeft().y);
 mp_ParamDeskDlg->SetTitle(MLL::LoadString(IDS_FW_RESERVE_PARAMETERS));
 mp_ParamDeskDlg->ShowSaveButton(false);
 mp_ParamDeskDlg->EnableUseResParCheck(true);
 mp_ParamDeskDlg->Resize(rect.Width(), rect.Height());
 mp_ParamDeskDlg->ShowWindow(SW_HIDE);

 //create IO remapping desk
 GetDlgItem(IDC_FW_PD_FRAME)->GetWindowRect(rect);
 ScreenToClient(rect);
 mp_IORemappingDlg->Create(CIORemappingDlg::IDD,this);
 mp_IORemappingDlg->SetPosition(rect.TopLeft().x,rect.TopLeft().y);
 mp_IORemappingDlg->ShowWindow(SW_HIDE);

 //select current tab
 m_param_sel_tab.SetCurSel(m_tab_selection);
 m_tabs[m_tab_selection].first->Show(true);

 //create tables desk
 GetDlgItem(IDC_FW_TD_FRAME)->GetWindowRect(rect);
 ScreenToClient(rect); 
 mp_TablesPanel->Create(CTablesSetPanel::IDD, this);
 mp_TablesPanel->SetPosition(rect.TopLeft().x,rect.TopLeft().y, GetDlgItem(IDC_FW_VIEW_FWOPT));
 mp_TablesPanel->ShowWindow(SW_SHOWNORMAL);

//create a tooltip control and assign tooltips
 mp_ttc.reset(new CToolTipCtrlEx());
 VERIFY(mp_ttc->Create(this, WS_POPUP | TTS_ALWAYSTIP | TTS_BALLOON));
 VERIFY(mp_ttc->AddWindow(&m_fw_options_btn, MLL::GetString(IDS_FW_VIEW_FWOPT_TT)));
 mp_ttc->SetMaxTipWidth(100); //Enable text wrapping
 mp_ttc->ActivateToolTips(true);

 mp_ContextMenuManager->Attach(this);

 SetTimer(TIMER_ID, 250, NULL);

 //в коде прошивки выдeлено ограниченное количество байтов для строки иныормации
 m_fw_information_edit.SetLimitText(48);

 m_hot_keys_supplier->Init(this);
 _RegisterHotKeys();

 //Enable drap & drop functionality
 DragAcceptFiles(true);

 UpdateDialogControls(this,TRUE);
 return TRUE;  // return TRUE unless you set the focus to a control
}

void CFirmwareTabDlg::OnContextMenu(CWnd* pWnd, CPoint point)
{
 mp_ContextMenuManager->TrackPopupMenu(point.x, point.y);
}

void CFirmwareTabDlg::OnPopupMenuButton()
{
 CRect rc;
 GetDlgItem(IDC_FW_POPUPMENU_BUTTON)->GetWindowRect(rc);
 mp_ContextMenuManager->TrackPopupMenu(rc.left, rc.top, false); //bottom align
}

void CFirmwareTabDlg::OnInitMenuPopup(CMenu* pMenu, UINT nIndex, BOOL bSysMenu)
{
 mp_ContextMenuManager->OnInitMenuPopup(pMenu, nIndex, bSysMenu);
}

void CFirmwareTabDlg::OnUpdatePopupMenu_bl(CCmdUI* pCmdUI)
{
 //этот элемент должен быть разрешен только если бутлоадер доступен и прошивка открыта
 if ((pCmdUI->m_nID==IDM_WRITE_FLASH) || (pCmdUI->m_nID==IDM_IMPORT_FW_DATA_FROM_SECU3))
 {
  BOOL enable = (m_is_bl_items_available && IsFirmwareOpened()) ? TRUE : FALSE;
  pCmdUI->Enable(enable);
  return;
 }

 BOOL enable = m_is_bl_items_available ? TRUE : FALSE;
 pCmdUI->Enable(enable);
 pCmdUI->SetCheck(FALSE);
}

void CFirmwareTabDlg::OnUpdatePopupMenu_file(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(TRUE);
 pCmdUI->SetCheck(FALSE);
}

//используется для элементов, состояние которых зависит от того - открыт файл прошивки или нет.
void CFirmwareTabDlg::OnUpdatePopupMenu_file1(CCmdUI* pCmdUI)
{
 bool opened = IsFirmwareOpened();
 pCmdUI->Enable(opened ? TRUE : FALSE);
 pCmdUI->SetCheck(FALSE);
}

void CFirmwareTabDlg::OnUpdatePopupMenu_app(CCmdUI* pCmdUI)
{
 bool enable = m_is_app_items_available;
 pCmdUI->Enable(enable ? TRUE : FALSE);
 pCmdUI->SetCheck(FALSE);
}

void CFirmwareTabDlg::OnUpdateBLStartedEmergency(CCmdUI* pCmdUI)
{
 BOOL enable = m_is_bl_started_emergency_available ? TRUE : FALSE;
 pCmdUI->Enable(enable);
}

void CFirmwareTabDlg::OnUpdateProgOnlyCode(CCmdUI* pCmdUI)
{
 BOOL enable = m_is_bl_items_available ? TRUE : FALSE;
 pCmdUI->Enable(enable);
}

void CFirmwareTabDlg::EnableBLStartedEmergency(bool enable)
{
 m_is_bl_started_emergency_available = enable;
}

void CFirmwareTabDlg::OnUpdateFirmwareSupportViewFWOptions(CCmdUI* pCmdUI)
{
 bool opened = IsFirmwareOpened();
 bool available = m_IsViewFWOptionsAvailable && m_IsViewFWOptionsAvailable();
 BOOL enable = opened && available;
 pCmdUI->Enable(enable);
}

void CFirmwareTabDlg::OnTimer(UINT nIDEvent)
{
 //I know it is dirty hack, but... :-)
 UpdateDialogControls(this,TRUE);
 Super::OnTimer(nIDEvent);

 //обновляем состояние (если нужно)
 bool pd_enable = IsFirmwareOpened();

 //Enable separate tabs
 for(size_t i = 0; i < m_tabs.size(); ++i)
 {
  bool enable;
  if (i == PSTID_IO_REMAPPING)
   enable = pd_enable && IsIORemappingAvailable();
  else
   enable = pd_enable;

  if (m_tabs[i].first->IsEnabled()!=enable)
   m_tabs[i].first->Enable(enable);
  m_param_sel_tab.EnableItem(i, enable);
 }
 m_param_sel_tab.EnableWindow(pd_enable);
}

void CFirmwareTabDlg::OnDestroy()
{ 
 Super::OnDestroy();
 KillTimer(TIMER_ID);
 m_hot_keys_supplier->Close();
 m_tab_selection = m_param_sel_tab.GetCurSel(); //remember last selected tab
}

//делегаты
void CFirmwareTabDlg::OnBootLoaderInfo()
{
 if (m_OnBootLoaderInfo)
  m_OnBootLoaderInfo();
}

void CFirmwareTabDlg::OnReadEepromToFile()
{
 if (m_OnReadEepromToFile)
  m_OnReadEepromToFile();
}

void CFirmwareTabDlg::OnWriteEepromFromFile()
{
 if (m_OnWriteEepromFromFile)
  m_OnWriteEepromFromFile();
}

void CFirmwareTabDlg::OnResetEeprom()
{
 //=================================================================
 if (!CheckAppTitle(AfxGetMainWnd()))
  return;
 if (!CheckAppLogo())
  return;
 //=================================================================

 if (m_OnResetEeprom)
  m_OnResetEeprom();
}

void CFirmwareTabDlg::OnReadFlashToFile()
{
//=================================================================
 if (!CheckAppTitle(AfxGetMainWnd()))
  delete this;
 if (!CheckAppLogo())
 { DestroyWindow(); return; }
 //=================================================================

 if (m_OnReadFlashToFile)
  m_OnReadFlashToFile();
}

void CFirmwareTabDlg::OnWriteFlashFromFile()
{
 if (m_OnWriteFlashFromFile)
  m_OnWriteFlashFromFile();
}

bool CFirmwareTabDlg::IsBLStartedEmergency(void)
{
 return (m_bl_started_emergency.GetCheck()==BST_CHECKED);
}

//разрешает/запрещает элементы меню связанные с бутлоадером
void CFirmwareTabDlg::EnableBLItems(bool enable)
{
 m_is_bl_items_available = enable;

 if (!::IsWindow(this->m_hWnd))
  return;

 UpdateDialogControls(this,TRUE);

 //если меню отображается в текущий момент, то не смотря на то что элементы запрещены,
 //прорисовка останется старой (недоделка Microsoft?). Короче говоря делаем это сами
 mp_ContextMenuManager->EnableBLMenuItems(enable);
}

//разрешает/запрещает элементы меню связанные с приложением
void CFirmwareTabDlg::EnableAppItems(bool enable)
{
 m_is_app_items_available = enable;

 if (!::IsWindow(this->m_hWnd))
  return;

 UpdateDialogControls(this,TRUE);

 //если меню отображается в текущий момент, то не смотря на то что элементы запрещены,
 //прорисовка останется старой (недоделка Microsoft?). Короче говоря делаем это сами
 mp_ContextMenuManager->EnableAppMenuItems(enable);
}

//от чекбокса...
void CFirmwareTabDlg::OnFirmwareSupportBlStartedEmergency()
{
 if (m_OnBLStartedEmergency)
  m_OnBLStartedEmergency();
}

void CFirmwareTabDlg::OnOpenFlashFromFile()
{
 //=================================================================
 if (!CheckAppTitle(AfxGetMainWnd()) || !CheckAppLogo())
  delete this;
 //=================================================================

 if (m_OnOpenFlashFromFile)
  m_OnOpenFlashFromFile();
}

void CFirmwareTabDlg::OnSaveFlashToFile()
{
 //=================================================================
 if (!CheckAppTitle(AfxGetMainWnd()))
 { DestroyWindow(); return; }
 if (!CheckAppLogo())
  delete this;
 //=================================================================

 if (m_OnSaveFlashToFile)
  m_OnSaveFlashToFile();
}

void CFirmwareTabDlg::SetFWInformationText(CString i_text)
{
 m_fw_information_edit.SetWindowText(i_text);
}

CString CFirmwareTabDlg::GetFWInformationText(void)
{
 CString o_text;
 m_fw_information_edit.GetWindowText(o_text);
 return o_text;
}

void CFirmwareTabDlg::OnChangeFirmwareSupportFwInformation()
{
 if (m_OnFWInformationTextChanged)
  m_OnFWInformationTextChanged();
}

//обновляет контроллы состояние которых зависит от того - открыта прошивка или нет
void CFirmwareTabDlg::OnUpdateFirmwareControls(CCmdUI* pCmdUI)
{
 bool enabled = IsFirmwareOpened();
 pCmdUI->Enable(enabled ? TRUE : FALSE);
}

void CFirmwareTabDlg::SetFirmwareName(_TSTRING i_name)
{
 m_fw_name.SetWindowText(i_name.c_str());
}

void CFirmwareTabDlg::SetModified(bool i_modified)
{
 if (i_modified)
  m_modification_flag.SetWindowText(_T("*"));
 else
  m_modification_flag.SetWindowText(_T(" "));
}

void CFirmwareTabDlg::SetFirmwareCRCs(unsigned int crc_stored_in_fw, unsigned int actual_crc_of_fw)
{
 CString string, string_stored, string_actual;
 string_stored.Format(_T("%04x"),crc_stored_in_fw);
 string_stored.MakeUpper();
 string_actual.Format(_T("%04x"),actual_crc_of_fw);
 string_actual.MakeUpper();
 string.Format(_T("CRC16: %s/%s"),string_stored,string_actual);
 m_fw_crc.SetWindowText(string);
}

void CFirmwareTabDlg::OnImportDataFromAnotherFW()
{
 if (m_OnImportDataFromAnotherFW)
  m_OnImportDataFromAnotherFW();
}

void CFirmwareTabDlg::OnReadFlashFromSECU()
{
 if (m_OnReadFlashFromSECU)
  m_OnReadFlashFromSECU();

 //=================================================================
 if (!CheckAppTitle(AfxGetMainWnd()) || !CheckAppLogo())
 { GetParent()->DestroyWindow(); }
 //=================================================================
}

void CFirmwareTabDlg::OnWriteFlashToSECU()
{
 if (m_OnWriteFlashToSECU)
  m_OnWriteFlashToSECU();
}

void CFirmwareTabDlg::OnImportDataFromSECU3()
{
 if (m_OnImportDataFromSECU3)
  m_OnImportDataFromSECU3();
}

bool CFirmwareTabDlg::IsProgrammeOnlyCode(void)
{
 return m_prog_only_code_checkbox.GetCheck() ? true : false;
}

void CFirmwareTabDlg::OnImportMapsFromMPSZ()
{
 if (m_OnImportMapsFromMPSZ)
  m_OnImportMapsFromMPSZ();
}

void CFirmwareTabDlg::OnImportMapsFromSECU3()
{
 if (m_OnImportMapsFromSECU3)
  m_OnImportMapsFromSECU3();
}

void CFirmwareTabDlg::OnImportMapsFromS3F()
{
 if (m_OnImportMapsFromS3F)
  m_OnImportMapsFromS3F();
}

void CFirmwareTabDlg::OnImportDefParamsFromEEPROMFile()
{
 if (m_OnImportDefParamsFromEEPROMFile)
  m_OnImportDefParamsFromEEPROMFile();
}

void CFirmwareTabDlg::OnImportTablesFromEEPROMFile()
{
 if (m_OnImportTablesFromEEPROMFile)
  m_OnImportTablesFromEEPROMFile();
}

void CFirmwareTabDlg::OnExportMapsToMPSZ()
{
 if (m_OnExportMapsToMPSZ)
  m_OnExportMapsToMPSZ();
}

void CFirmwareTabDlg::OnExportMapsToSECU3()
{
 if (m_OnExportMapsToSECU3)
  m_OnExportMapsToSECU3();
}

void CFirmwareTabDlg::OnExportMapsToS3F()
{
 if (m_OnExportMapsToS3F)
  m_OnExportMapsToS3F();
}

void CFirmwareTabDlg::OnFirmwareInfo()
{
 if (m_OnFirmwareInfo)
  m_OnFirmwareInfo();
}

void CFirmwareTabDlg::OnViewFWOptions()
{
 if (m_OnViewFWOptions)
  m_OnViewFWOptions();
}

void CFirmwareTabDlg::OnDropFiles(HDROP hDropInfo)
{
 TCHAR fileName[MAX_PATH+1];
 if (DragQueryFile(hDropInfo, 0, fileName, MAX_PATH))
 {
  if (m_OnDropFile)
   m_OnDropFile(fileName);
 }
 else
  MessageBeep(MB_ICONEXCLAMATION);
}

void CFirmwareTabDlg::_RegisterHotKeys(void)
{
 m_hot_keys_supplier->RegisterCommand(IDM_OPEN_FLASH, 'O', MOD_CONTROL);
 m_hot_keys_supplier->RegisterCommand(IDM_SAVE_FLASH, 'S', MOD_CONTROL);
 m_hot_keys_supplier->RegisterCommand(IDM_READ_FLASH, 'R', MOD_CONTROL);
 m_hot_keys_supplier->RegisterCommand(IDM_WRITE_FLASH,'W', MOD_CONTROL);
 m_hot_keys_supplier->RegisterCommand(IDM_READ_EEPROM_TO_FILE,'G', MOD_CONTROL);
 m_hot_keys_supplier->RegisterCommand(IDM_WRITE_EEPROM_FROM_FILE,'P', MOD_CONTROL);
 m_hot_keys_supplier->RegisterCommand(IDM_READ_BOOTLOADER_SIGNATURE,'L', MOD_CONTROL);
 m_hot_keys_supplier->RegisterCommand(IDM_READ_FW_SIGNATURE_INFO,'I', MOD_CONTROL);
}

void CFirmwareTabDlg::SetBLStartedEmergency(bool state)
{
 m_bl_started_emergency.SetCheck(state ? BST_CHECKED : BST_UNCHECKED);
}

bool CFirmwareTabDlg::IsBLStartedEmergencyEnabled(void)
{
 return m_is_bl_started_emergency_available;
}

bool CFirmwareTabDlg::IsBLItemsEnabled(void)
{
 return m_is_bl_items_available;
}

bool CFirmwareTabDlg::IsAppItemsEnabled(void)
{
 return m_is_app_items_available;
}

bool CFirmwareTabDlg::IsFirmwareOpened(void)
{
 if (m_IsFirmwareOpened)
  return m_IsFirmwareOpened();
 return false;
}

bool CFirmwareTabDlg::IsIORemappingAvailable(void)
{
 if (m_IsIORemappingAvailable)
  return m_IsIORemappingAvailable();
 return false;
}

void CFirmwareTabDlg::OnSelchangeTabctl(void)
{
 size_t index = m_param_sel_tab.GetCurSel();
 if (index < m_tabs.size())
  m_tabs[index].first->Show(true); //show current window
}
bool CFirmwareTabDlg::OnSelchangingTabctl(void)
{
 size_t index = TabCtrl_GetCurSel(m_param_sel_tab.m_hWnd);
 if (index < m_tabs.size())
  m_tabs[index].first->Show(false); //hide previous window
 return true;
}

void CFirmwareTabDlg::setOnBootLoaderInfo(EventHandler OnFunction)
{m_OnBootLoaderInfo = OnFunction;}

void CFirmwareTabDlg::setOnReadEepromToFile(EventHandler OnFunction)
{m_OnReadEepromToFile = OnFunction;}

void CFirmwareTabDlg::setOnWriteEepromFromFile(EventHandler OnFunction)
{m_OnWriteEepromFromFile = OnFunction;}

void CFirmwareTabDlg::setOnResetEeprom(EventHandler OnFunction)
{m_OnResetEeprom = OnFunction; }

void CFirmwareTabDlg::setOnReadFlashToFile(EventHandler OnFunction)
{m_OnReadFlashToFile = OnFunction;}

void CFirmwareTabDlg::setOnWriteFlashFromFile(EventHandler OnFunction)
{m_OnWriteFlashFromFile = OnFunction;}

void CFirmwareTabDlg::setOnOpenFlashFromFile(EventHandler OnFunction)
{m_OnOpenFlashFromFile = OnFunction;}

void CFirmwareTabDlg::setOnFWInformationTextChanged(EventHandler OnFunction)
{m_OnFWInformationTextChanged = OnFunction;}

void CFirmwareTabDlg::setOnSaveFlashToFile(EventHandler OnFunction)
{m_OnSaveFlashToFile = OnFunction;}

void CFirmwareTabDlg::setIsFirmwareOpened(EventResult IsFunction)
{m_IsFirmwareOpened = IsFunction;}

void CFirmwareTabDlg::setOnImportDataFromAnotherFW(EventHandler OnFunction)
{m_OnImportDataFromAnotherFW = OnFunction;}

void CFirmwareTabDlg::setOnImportDataFromSECU3(EventHandler OnFunction)
{m_OnImportDataFromSECU3 = OnFunction;}

void CFirmwareTabDlg::setOnImportMapsFromMPSZ(EventHandler OnFunction)
{m_OnImportMapsFromMPSZ = OnFunction;}

void CFirmwareTabDlg::setOnImportMapsFromSECU3(EventHandler OnFunction)
{m_OnImportMapsFromSECU3 = OnFunction;}

void CFirmwareTabDlg::setOnImportMapsFromS3F(EventHandler OnFunction)
{m_OnImportMapsFromS3F = OnFunction;}

void CFirmwareTabDlg::setOnImportDefParamsFromEEPROMFile(EventHandler OnFunction)
{m_OnImportDefParamsFromEEPROMFile = OnFunction;}

void CFirmwareTabDlg::setOnImportTablesFromEEPROMFile(EventHandler OnFunction)
{m_OnImportTablesFromEEPROMFile = OnFunction;}

void CFirmwareTabDlg::setOnReadFlashFromSECU(EventHandler OnFunction)
{m_OnReadFlashFromSECU = OnFunction;}

void CFirmwareTabDlg::setOnWriteFlashToSECU(EventHandler OnFunction)
{m_OnWriteFlashToSECU = OnFunction;}

void CFirmwareTabDlg::setOnExportMapsToMPSZ(EventHandler OnFunction)
{m_OnExportMapsToMPSZ = OnFunction;}

void CFirmwareTabDlg::setOnExportMapsToSECU3(EventHandler OnFunction)
{m_OnExportMapsToSECU3 = OnFunction;}

void CFirmwareTabDlg::setOnExportMapsToS3F(EventHandler OnFunction)
{m_OnExportMapsToS3F = OnFunction;}

void CFirmwareTabDlg::setOnFirmwareInfo(EventHandler OnFunction)
{m_OnFirmwareInfo = OnFunction;}

void CFirmwareTabDlg::setOnViewFWOptions(EventHandler OnFunction)
{m_OnViewFWOptions = OnFunction;}

void CFirmwareTabDlg::setIsViewFWOptionsAvailable(EventResult OnFunction)
{m_IsViewFWOptionsAvailable = OnFunction;}

void CFirmwareTabDlg::setIsIORemappingAvailable(EventResult OnFunction)
{m_IsIORemappingAvailable = OnFunction;}

void CFirmwareTabDlg::setOnBLStartedEmergency(EventHandler OnFunction)
{m_OnBLStartedEmergency = OnFunction;}

void CFirmwareTabDlg::setOnDropFile(EventString OnFunction)
{m_OnDropFile = OnFunction;}
