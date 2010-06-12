 /****************************************************************
 *       SECU-3  - An open source, free engine control unit
 *    Designed by Alexey A. Shabelnikov. Ukraine, Gorlovka 2007.
 *       Microprocessors systems - design & programming.
 *    contacts:
 *              http://secu-3.narod.ru
 *              ICQ: 405-791-931
 ****************************************************************/

#include "stdafx.h"
#include "Resources\resource.h"
#include "FirmwareTabDlg.h"

#include "Application\DLLLinkedFunctions.h"
#include "common\MathHelpers.h"
#include "FirmwareContextMenuManager.h"
#include "ParamDesk\ParamDeskDlg.h"
#include "ui-core\HotKeysToCmdRouter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define TIMER_ID 0

void __cdecl CFirmwareTabDlg::OnChangeStartMap(void* i_param)
{
 CFirmwareTabDlg* _this = static_cast<CFirmwareTabDlg*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what is the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
   _this->m_OnMapChanged(TYPE_MAP_DA_START);
}

void __cdecl CFirmwareTabDlg::OnCloseStartMap(void* i_param)
{
 CFirmwareTabDlg* _this = static_cast<CFirmwareTabDlg*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what is the fuck?
  return;
 }
 _this->m_start_map_chart_state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_start_map_wnd_handle, TYPE_MAP_DA_START);
}

//------------------------------------------------------------------------
void __cdecl CFirmwareTabDlg::OnChangeIdleMap(void* i_param)
{
 CFirmwareTabDlg* _this = static_cast<CFirmwareTabDlg*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what is the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
   _this->m_OnMapChanged(TYPE_MAP_DA_IDLE);
}

void __cdecl CFirmwareTabDlg::OnCloseIdleMap(void* i_param)
{
 CFirmwareTabDlg* _this = static_cast<CFirmwareTabDlg*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what is the fuck?
  return;
 }
 _this->m_idle_map_chart_state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_idle_map_wnd_handle, TYPE_MAP_DA_IDLE);
}

//------------------------------------------------------------------------
void __cdecl CFirmwareTabDlg::OnChangeWorkMap(void* i_param)
{
 CFirmwareTabDlg* _this = static_cast<CFirmwareTabDlg*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what is the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
   _this->m_OnMapChanged(TYPE_MAP_DA_WORK);
}

//------------------------------------------------------------------------
void __cdecl CFirmwareTabDlg::OnCloseWorkMap(void* i_param)
{
 CFirmwareTabDlg* _this = static_cast<CFirmwareTabDlg*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what is the fuck?
  return;
 }
 _this->m_work_map_chart_state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_work_map_wnd_handle, TYPE_MAP_DA_WORK);
}

//------------------------------------------------------------------------
void __cdecl CFirmwareTabDlg::OnChangeTempMap(void* i_param)
{
 CFirmwareTabDlg* _this = static_cast<CFirmwareTabDlg*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what is the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
   _this->m_OnMapChanged(TYPE_MAP_DA_TEMP_CORR);
}

//------------------------------------------------------------------------
void __cdecl CFirmwareTabDlg::OnCloseTempMap(void* i_param)
{
CFirmwareTabDlg* _this = static_cast<CFirmwareTabDlg*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what is the fuck?
  return;
 }
 _this->m_temp_map_chart_state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_temp_map_wnd_handle, TYPE_MAP_DA_TEMP_CORR);
}

//------------------------------------------------------------------------
void __cdecl CFirmwareTabDlg::OnChangeAttenuatorTable(void* i_param)
{
 CFirmwareTabDlg* _this = static_cast<CFirmwareTabDlg*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what is the fuck?
  return;
 }

 if (_this->m_OnMapChanged)
   _this->m_OnMapChanged(TYPE_MAP_ATTENUATOR); 
}

//------------------------------------------------------------------------
void __cdecl CFirmwareTabDlg::OnCloseAttenuatorTable(void* i_param)
{
 CFirmwareTabDlg* _this = static_cast<CFirmwareTabDlg*>(i_param);
 if (!_this)
 {
  ASSERT(0); //what is the fuck?
  return;
 }
 _this->m_attenuator_map_chart_state = 0;

 //allow controller to detect closing of this window
 if (_this->m_OnCloseMapWnd)
  _this->m_OnCloseMapWnd(_this->m_attenuator_map_wnd_handle, TYPE_MAP_ATTENUATOR);
}

//------------------------------------------------------------------------
void __cdecl CFirmwareTabDlg::OnGetYAxisLabel(LPTSTR io_label_string, void* i_param)
{
 CFirmwareTabDlg* _this = static_cast<CFirmwareTabDlg*>(i_param);
 if (!_this)
 {
  ASSERT(0); //WTF?
  return;
 }

 float value = (float)_tcstod(io_label_string, NULL);
 int i = MathHelpers::Round(value);
 if (i >= 0 && i < SECU3IO::ATTENUATOR_LEVELS_SIZE)
  _stprintf(io_label_string, _T("%0.3f"), SECU3IO::hip9011_attenuator_gains[i]);
}

//------------------------------------------------------------------------

const UINT CFirmwareTabDlg::IDD = IDD_FIRMWARE_SUPPORT;

/////////////////////////////////////////////////////////////////////////////
// CFirmwareTabDlg dialog

CFirmwareTabDlg::CFirmwareTabDlg(CWnd* pParent /*=NULL*/)
: Super(CFirmwareTabDlg::IDD, pParent)	
, m_is_bl_started_emergency_available(false)
, m_is_bl_items_available(false)
, m_is_app_items_available(false)
, mp_ParamDeskDlg(new CParamDeskDlg(NULL, true)) //<-- используем вкладку параметров детонации
, m_hot_keys_supplier(new CHotKeysToCmdRouter())
, mp_ContextMenuManager(new CFirmwareModeContextMenuManager())
{
 mp_ContextMenuManager->CreateContent();

 m_work_map_chart_state   = 0;
 m_temp_map_chart_state   = 0;
 m_start_map_chart_state  = 0;
 m_idle_map_chart_state   = 0;
 m_attenuator_map_chart_state = 0;


 m_start_map_wnd_handle = NULL;
 m_idle_map_wnd_handle  = NULL;
 m_work_map_wnd_handle  = NULL;
 m_temp_map_wnd_handle  = NULL;
 m_attenuator_map_wnd_handle = NULL;
 
 int rpm = 200;
 for(size_t i = 0; i < 128; i++)
 {
  m_attenuator_table_slots[i] = rpm;
  rpm+=60;
 }
}

void CFirmwareTabDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);
 DDX_Control(pDX, IDC_FIRMWARE_SUPPORT_FUNSET_LIST, m_funset_listbox);
 DDX_Control(pDX, IDC_FIRMWARE_SUPPORT_BL_STARTED_EMERGENCY, m_bl_started_emergency);
 DDX_Control(pDX, IDC_FIRMWARE_SUPPORT_VIEW_WORK_MAP, m_view_work_map_btn);
 DDX_Control(pDX, IDC_FIRMWARE_SUPPORT_VIEW_TEMP_MAP, m_view_temp_map_btn);
 DDX_Control(pDX, IDC_FIRMWARE_SUPPORT_VIEW_START_MAP, m_view_start_map_btn);
 DDX_Control(pDX, IDC_FIRMWARE_SUPPORT_VIEW_IDLE_MAP, m_view_idle_map_btn);
 DDX_Control(pDX, IDC_FIRMWARE_SUPPORT_FW_INFORMATION,m_fw_information_edit);
 DDX_Control(pDX, IDC_FIRMWARE_SUPPORT_FW_NAME,m_fw_name);
 DDX_Control(pDX, IDC_FIRMWARE_SUPPORT_CRC, m_fw_crc);
 DDX_Control(pDX, IDC_FIRMWARE_SUPPORT_MODIFICATION_FLAG, m_modification_flag);
 DDX_Control(pDX, IDC_FIRMWARE_SUPPORT_PROG_ONLY_CODE, m_prog_only_code_checkbox);
 DDX_Control(pDX, IDC_FIRMWARE_SUPPORT_VIEW_ATTENUATOR_MAP,m_view_attenuator_map_btn);
}

LPCTSTR CFirmwareTabDlg::GetDialogID(void) const
{
 return (LPCTSTR)IDD; 
}

BEGIN_MESSAGE_MAP(CFirmwareTabDlg, Super)
 ON_WM_CONTEXTMENU()
 ON_UPDATE_COMMAND_UI(IDM_READ_FLASH, OnUpdatePopupMenu_bl)
 ON_UPDATE_COMMAND_UI(IDM_OPEN_FLASH, OnUpdatePopupMenu_file)
 ON_WM_INITMENUPOPUP()
 ON_BN_CLICKED(IDC_FIRMWARE_SUPPORT_VIEW_START_MAP, OnFirmwareSupportViewStartMap)
 ON_BN_CLICKED(IDC_FIRMWARE_SUPPORT_VIEW_IDLE_MAP, OnFirmwareSupportViewIdleMap)
 ON_BN_CLICKED(IDC_FIRMWARE_SUPPORT_VIEW_WORK_MAP, OnFirmwareSupportViewWorkMap)
 ON_BN_CLICKED(IDC_FIRMWARE_SUPPORT_VIEW_TEMP_MAP, OnFirmwareSupportViewTempMap)
 ON_UPDATE_COMMAND_UI(IDC_FIRMWARE_SUPPORT_VIEW_START_MAP, OnUpdateFirmwareSupportViewStartMap)
 ON_UPDATE_COMMAND_UI(IDC_FIRMWARE_SUPPORT_VIEW_IDLE_MAP, OnUpdateFirmwareSupportViewIdleMap)
 ON_UPDATE_COMMAND_UI(IDC_FIRMWARE_SUPPORT_VIEW_WORK_MAP, OnUpdateFirmwareSupportViewWorkMap)
 ON_UPDATE_COMMAND_UI(IDC_FIRMWARE_SUPPORT_VIEW_TEMP_MAP, OnUpdateFirmwareSupportViewTempMap)
 ON_UPDATE_COMMAND_UI(IDC_FIRMWARE_SUPPORT_VIEW_ATTENUATOR_MAP, OnUpdateFirmwareSupportViewAttenuatorMap)
 ON_WM_TIMER()
 ON_WM_DESTROY()
 ON_COMMAND(IDM_READ_BOOTLOADER_SIGNATURE, OnBootLoaderInfo)
 ON_COMMAND(IDM_READ_EEPROM_TO_FILE, OnReadEepromToFile)
 ON_COMMAND(IDM_WRITE_EEPROM_FROM_FILE, OnWriteEepromFromFile)
 ON_COMMAND(IDM_READ_FLASH_TO_FILE, OnReadFlashToFile)
 ON_COMMAND(IDM_WRITE_FLASH_FROM_FILE, OnWriteFlashFromFile)
 ON_UPDATE_COMMAND_UI(IDC_FIRMWARE_SUPPORT_BL_STARTED_EMERGENCY, OnUpdateBLStartedEmergency)
 ON_BN_CLICKED(IDC_FIRMWARE_SUPPORT_BL_STARTED_EMERGENCY, OnFirmwareSupportBlStartedEmergency)
 ON_COMMAND(IDM_OPEN_FLASH, OnOpenFlashFromFile)	
 ON_COMMAND(IDM_SAVE_FLASH, OnSaveFlashToFile)	
 ON_EN_CHANGE(IDC_FIRMWARE_SUPPORT_FW_INFORMATION, OnChangeFirmwareSupportFwInformation)
 ON_UPDATE_COMMAND_UI(IDC_FIRMWARE_SUPPORT_FW_INFORMATION, OnUpdateFirmwareControls)
 ON_UPDATE_COMMAND_UI(IDC_FIRMWARE_SUPPORT_FUNSET_LIST, OnUpdateFirmwareControls)
 
 ON_UPDATE_COMMAND_UI(IDC_FIRMWARE_SUPPORT_FW_INFORMATION_LABEL, OnUpdateFirmwareControls) 
 ON_UPDATE_COMMAND_UI(IDC_FIRMWARE_SUPPORT_MAP_GROUPBOX, OnUpdateFirmwareControls)
 ON_UPDATE_COMMAND_UI(IDC_FIRMWARE_SUPPORT_CRC, OnUpdateFirmwareControls)
 ON_UPDATE_COMMAND_UI(IDC_FIRMWARE_SUPPORT_FW_NAME, OnUpdateFirmwareControls)

 ON_UPDATE_COMMAND_UI(IDM_WRITE_FLASH, OnUpdatePopupMenu_bl)
 ON_UPDATE_COMMAND_UI(IDM_SAVE_FLASH, OnUpdatePopupMenu_file1)
 ON_UPDATE_COMMAND_UI(IDM_READ_FLASH_TO_FILE, OnUpdatePopupMenu_bl)  
 ON_UPDATE_COMMAND_UI(IDM_WRITE_FLASH_FROM_FILE, OnUpdatePopupMenu_bl)  
 ON_UPDATE_COMMAND_UI(IDM_READ_EEPROM_TO_FILE, OnUpdatePopupMenu_bl)
 ON_UPDATE_COMMAND_UI(IDM_WRITE_EEPROM_FROM_FILE, OnUpdatePopupMenu_bl)
 ON_UPDATE_COMMAND_UI(IDM_READ_BOOTLOADER_SIGNATURE, OnUpdatePopupMenu_bl)
 ON_NOTIFY(LVN_ITEMCHANGED, IDC_FIRMWARE_SUPPORT_FUNSET_LIST, OnChangeFirmwareSupportFunsetList)
 ON_NOTIFY(LVN_ENDLABELEDIT, IDC_FIRMWARE_SUPPORT_FUNSET_LIST, OnEndLabelEditFirmwareSupportFunsetList)

 ON_UPDATE_COMMAND_UI(IDM_IMPORT_FW_DATA_FROM_ANOTHER_FW, OnUpdatePopupMenu_file1)  
 ON_COMMAND(IDM_IMPORT_FW_DATA_FROM_ANOTHER_FW,OnImportDataFromAnotherFW)
 ON_COMMAND(IDM_IMPORT_FW_DATA_FROM_SECU3,OnImportDataFromSECU3)

 ON_COMMAND(IDM_READ_FLASH,OnReadFlashFromSECU)
 ON_COMMAND(IDM_WRITE_FLASH,OnWriteFlashToSECU)
  
 ON_UPDATE_COMMAND_UI(IDM_IMPORT_FW_DATA_FROM_SECU3, OnUpdatePopupMenu_bl)
 ON_UPDATE_COMMAND_UI(IDC_FIRMWARE_SUPPORT_PROG_ONLY_CODE, OnUpdateProgOnlyCode)
 ON_UPDATE_COMMAND_UI(IDM_IMPORT_IMPORT_FROM_MPSZ, OnUpdatePopupMenu_file1)
 ON_UPDATE_COMMAND_UI(IDM_EXPORT_EXPORT_TO_MPSZ, OnUpdatePopupMenu_file1)

 ON_COMMAND(IDM_IMPORT_IMPORT_FROM_MPSZ, OnImportMapsFromMPSZ)
 ON_COMMAND(IDM_EXPORT_EXPORT_TO_MPSZ, OnExportMapsToMPSZ)
    	
 ON_UPDATE_COMMAND_UI(IDC_FIRMWARE_SUPPORT_VIEW_ATTENUATOR_MAP, OnUpdateFirmwareControls)
 ON_BN_CLICKED(IDC_FIRMWARE_SUPPORT_VIEW_ATTENUATOR_MAP, OnFirmwareSupportViewAttenuatorMap)  

 ON_COMMAND(IDM_READ_FW_SIGNATURE_INFO, OnWirmwareInfo)
 ON_UPDATE_COMMAND_UI(IDM_READ_FW_SIGNATURE_INFO, OnUpdatePopupMenu_app)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFirmwareTabDlg message handlers

BOOL CFirmwareTabDlg::OnInitDialog() 
{
 Super::OnInitDialog();
	
 mp_ParamDeskDlg->Create(CParamDeskDlg::IDD,this);
 mp_ParamDeskDlg->SetPosition(390,10);	
 mp_ParamDeskDlg->SetTitle(MLL::LoadString(IDS_FW_RESERVE_PARAMETERS));
 mp_ParamDeskDlg->ShowSaveButton(false);
 mp_ParamDeskDlg->ShowWindow(SW_SHOWNORMAL);

 mp_ContextMenuManager->Attach(this);

 SetTimer(TIMER_ID,250,NULL);

 //в коде прошивки выдолено ограниченное количество байтов для строки иныормации
 m_fw_information_edit.SetLimitText(48);  

 m_hot_keys_supplier->Init(this);
 _RegisterHotKeys();

 UpdateDialogControls(this,TRUE);
 return TRUE;  // return TRUE unless you set the focus to a control
               // EXCEPTION: OCX Property Pages should return FALSE
}

void CFirmwareTabDlg::OnContextMenu(CWnd* pWnd, CPoint point) 
{
 mp_ContextMenuManager->TrackPopupMenu(point.x, point.y);	
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

//-----------------------------------------------------------------------------------------------
void CFirmwareTabDlg::OnFirmwareSupportViewStartMap() 
{      
 //если кнопку "выключили" то закрываем окно редактора
 if (m_view_start_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(m_start_map_wnd_handle,WM_CLOSE,0,0); 
  return;
 }

 if ((!m_start_map_chart_state)&&(DLL::UOZ1_Chart2DCreate))	 
 {
  m_start_map_chart_state = 1;
  m_start_map_wnd_handle = DLL::UOZ1_Chart2DCreate(GetStartMap(true),GetStartMap(false),-15.0,55.0,SECU3IO::start_map_rpm_slots,16,
    MLL::GetString(IDS_FW_MAPS_RPM_UNIT).c_str(),
    MLL::GetString(IDS_FW_MAPS_ADVANGLE_UNIT).c_str(),
    MLL::GetString(IDS_FW_START_MAP).c_str());	  
  DLL::UOZ1_Chart2DSetOnChange(m_start_map_wnd_handle,OnChangeStartMap,this);
  DLL::UOZ1_Chart2DSetOnClose(m_start_map_wnd_handle,OnCloseStartMap,this);

  //let controller to know about opening of this window
  if (m_OnOpenMapWnd)
      m_OnOpenMapWnd(m_start_map_wnd_handle, TYPE_MAP_DA_START);

  DLL::UOZ1_Chart2DShow(m_start_map_wnd_handle, true);
 }
 else
 {
  ::SetFocus(m_start_map_wnd_handle);
 }		
}

//-----------------------------------------------------------------------------------------------
void CFirmwareTabDlg::OnFirmwareSupportViewIdleMap() 
{
 //если кнопку "выключили" то закрываем окно редактора
 if (m_view_idle_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(m_idle_map_wnd_handle,WM_CLOSE,0,0); 
  return;
 }

 if ((!m_idle_map_chart_state)&&(DLL::UOZ1_Chart2DCreate))	 
 {
  m_idle_map_chart_state = 1;
  m_idle_map_wnd_handle = DLL::UOZ1_Chart2DCreate(GetIdleMap(true),GetIdleMap(false),-15.0,55.0,SECU3IO::idle_map_rpm_slots,16,
    MLL::GetString(IDS_FW_MAPS_RPM_UNIT).c_str(),
    MLL::GetString(IDS_FW_MAPS_ADVANGLE_UNIT).c_str(),
    MLL::GetString(IDS_FW_IDLE_MAP).c_str());
  DLL::UOZ1_Chart2DSetOnChange(m_idle_map_wnd_handle,OnChangeIdleMap,this);
  DLL::UOZ1_Chart2DSetOnClose(m_idle_map_wnd_handle,OnCloseIdleMap,this);

  //let controller to know about opening of this window
  if (m_OnOpenMapWnd)
      m_OnOpenMapWnd(m_idle_map_wnd_handle, TYPE_MAP_DA_IDLE);

  DLL::UOZ1_Chart2DShow(m_idle_map_wnd_handle, true);
 }
 else
 {
  ::SetFocus(m_idle_map_wnd_handle);
 }			
}

void CFirmwareTabDlg::OnFirmwareSupportViewWorkMap() 
{
 //если кнопку "выключили" то закрываем окно редактора
 if (m_view_work_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(m_work_map_wnd_handle,WM_CLOSE,0,0); 
  return;
 }

 if ((!m_work_map_chart_state)&&(DLL::UOZ2_Chart3DCreate))	 
 {
  m_work_map_chart_state = 1;
  m_work_map_wnd_handle = DLL::UOZ2_Chart3DCreate(GetWorkMap(true),GetWorkMap(false),SECU3IO::work_map_rpm_slots,16,16,-15.0,55.0,
    MLL::GetString(IDS_FW_MAPS_RPM_UNIT).c_str(),
	MLL::GetString(IDS_FW_WORK_MAP).c_str());
  DLL::UOZ2_Chart3DSetOnChange(m_work_map_wnd_handle,OnChangeWorkMap,this);
  DLL::UOZ2_Chart3DSetOnClose(m_work_map_wnd_handle,OnCloseWorkMap,this);

  //let controller to know about opening of this window
  if (m_OnOpenMapWnd)
      m_OnOpenMapWnd(m_work_map_wnd_handle, TYPE_MAP_DA_WORK);

  DLL::UOZ2_Chart3DShow(m_work_map_wnd_handle, true);
 }
 else
 {
  ::SetFocus(m_work_map_wnd_handle);
 }
}

void CFirmwareTabDlg::OnFirmwareSupportViewTempMap() 
{	
 //если кнопку "выключили" то закрываем окно редактора
 if (m_view_temp_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(m_temp_map_wnd_handle,WM_CLOSE,0,0); 
  return;
 }

 if ((!m_temp_map_chart_state)&&(DLL::UOZ1_Chart2DCreate))	 
 {
  m_temp_map_chart_state = 1;
  m_temp_map_wnd_handle = DLL::UOZ1_Chart2DCreate(GetTempMap(true),GetTempMap(false),-15.0,25.0,SECU3IO::temp_map_rpm_slots,16,
    MLL::GetString(IDS_FW_MAPS_TEMPERATURE_UNIT).c_str(),
	MLL::GetString(IDS_FW_MAPS_ADVANGLE_UNIT).c_str(),
	MLL::GetString(IDS_FW_TEMPCORR_MAP).c_str());
  DLL::UOZ1_Chart2DSetOnChange(m_temp_map_wnd_handle,OnChangeTempMap,this);
  DLL::UOZ1_Chart2DSetOnClose(m_temp_map_wnd_handle,OnCloseTempMap,this);
 
  //let controller to know about opening of this window
  if (m_OnOpenMapWnd)
      m_OnOpenMapWnd(m_temp_map_wnd_handle, TYPE_MAP_DA_TEMP_CORR);

  DLL::UOZ1_Chart2DShow(m_temp_map_wnd_handle, true);
 }
 else
 {
  ::SetFocus(m_temp_map_wnd_handle);
 }		
}

void CFirmwareTabDlg::OnUpdateFirmwareSupportViewStartMap(CCmdUI* pCmdUI) 
{
 bool opened = IsFirmwareOpened(); 
 BOOL enable = (DLL::UOZ1_Chart2DCreate!=NULL) && opened && m_funset_listbox.GetSelectedCount();
 pCmdUI->Enable(enable);
 pCmdUI->SetCheck( (m_start_map_chart_state) ? TRUE : FALSE );	
}

void CFirmwareTabDlg::OnUpdateFirmwareSupportViewIdleMap(CCmdUI* pCmdUI) 
{
 bool opened = IsFirmwareOpened(); 
 BOOL enable = (DLL::UOZ1_Chart2DCreate!=NULL) && opened && m_funset_listbox.GetSelectedCount();
 pCmdUI->Enable(enable);
 pCmdUI->SetCheck( (m_idle_map_chart_state) ? TRUE : FALSE );	
}

void CFirmwareTabDlg::OnUpdateFirmwareSupportViewWorkMap(CCmdUI* pCmdUI) 
{
 bool opened = IsFirmwareOpened(); 
 BOOL enable = (DLL::UOZ2_Chart3DCreate!=NULL) && opened && m_funset_listbox.GetSelectedCount();
 pCmdUI->Enable(enable);
 pCmdUI->SetCheck( (m_work_map_chart_state) ? TRUE : FALSE );		
}

void CFirmwareTabDlg::OnUpdateFirmwareSupportViewTempMap(CCmdUI* pCmdUI) 
{
 bool opened = IsFirmwareOpened(); 
 BOOL enable = (DLL::UOZ1_Chart2DCreate!=NULL) && opened && m_funset_listbox.GetSelectedCount();
 pCmdUI->Enable(enable);
 pCmdUI->SetCheck( (m_temp_map_chart_state) ? TRUE : FALSE );	
}

void CFirmwareTabDlg::OnUpdateFirmwareSupportViewAttenuatorMap(CCmdUI* pCmdUI)
{
 bool opened = IsFirmwareOpened(); 
 BOOL enable = (DLL::UOZ1_Chart2DCreate!=NULL) && opened;
 pCmdUI->Enable(enable);
 pCmdUI->SetCheck( (m_attenuator_map_chart_state) ? TRUE : FALSE );	
}

void CFirmwareTabDlg::OnTimer(UINT nIDEvent) 
{
 //I know it is dirty hack, but... :-) 	
 UpdateDialogControls(this,TRUE);
 Super::OnTimer(nIDEvent);
  
 //обновляем состояние (если нужно)
 bool pd_enable = IsFirmwareOpened();
 if (mp_ParamDeskDlg->IsEnabled()!=pd_enable)
  mp_ParamDeskDlg->Enable(pd_enable);  
}

void CFirmwareTabDlg::OnDestroy() 
{
 Super::OnDestroy();
 KillTimer(TIMER_ID);	
 m_hot_keys_supplier->Close();
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

void CFirmwareTabDlg::OnReadFlashToFile()
{
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
 if (m_OnOpenFlashFromFile)
  m_OnOpenFlashFromFile();
}

void CFirmwareTabDlg::OnSaveFlashToFile()
{
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

void CFirmwareTabDlg::UpdateOpenedCharts(void)
{
 if (m_start_map_chart_state)
  DLL::UOZ1_Chart2DUpdate(m_start_map_wnd_handle,GetStartMap(true),GetStartMap(false));	 
 if (m_idle_map_chart_state)
  DLL::UOZ1_Chart2DUpdate(m_idle_map_wnd_handle,GetIdleMap(true),GetIdleMap(false));	 
 if (m_work_map_chart_state)
  DLL::UOZ2_Chart3DUpdate(m_work_map_wnd_handle,GetWorkMap(true),GetWorkMap(false));	 
 if (m_temp_map_chart_state)
  DLL::UOZ1_Chart2DUpdate(m_temp_map_wnd_handle,GetTempMap(true),GetTempMap(false));	 
 if (m_attenuator_map_chart_state)
  DLL::UOZ1_Chart2DUpdate(m_attenuator_map_wnd_handle,GetAttenuatorMap(true),GetAttenuatorMap(false));	 
}

//изменилось выделение в спимке семейств характеристик
void CFirmwareTabDlg::OnChangeFirmwareSupportFunsetList(NMHDR* pNMHDR, LRESULT* pResult) 
{
 NMLISTVIEW *pnmv = (NM_LISTVIEW FAR *) pNMHDR;
  
 if (pnmv->uChanged == LVIF_STATE && (pnmv->uNewState & LVIS_SELECTED)) 
 {
  //состояние выделения изменилось
  int selected_index = 0;

  //will return -1 if there are no selected items, at all
  selected_index = m_funset_listbox.GetNextItem(-1, LVNI_SELECTED);

  if (m_OnFunSetSelectionChanged)
   m_OnFunSetSelectionChanged(selected_index);     
 }
 
 *pResult = 0;
}

void CFirmwareTabDlg::OnEndLabelEditFirmwareSupportFunsetList(NMHDR* pNMHDR, LRESULT* pResult) 
{
 NMLVDISPINFO *pdi = (LV_DISPINFO FAR *) pNMHDR;

 if (pdi->item.mask == LVIF_TEXT) 
 {
  m_funset_listbox.SetItem(&pdi->item);
  if (m_OnFunSetNamechanged)
   m_OnFunSetNamechanged(pdi->item.iItem,pdi->item.pszText); 
 }
}

void CFirmwareTabDlg::SetFunSetListBox(std::vector<_TSTRING> i_list_of_names)
{
 m_funset_listbox.DeleteAllItems();
 for (size_t i = 0; i < i_list_of_names.size(); i++)
 {
  m_funset_listbox.InsertItem(i,i_list_of_names[i].c_str());	
 }
}

void CFirmwareTabDlg::SetFunSetListBoxSelection(int i_selected_index)
{
 if (i_selected_index!=-1)
  m_funset_listbox.SetItemState(i_selected_index, LVIS_SELECTED, LVIS_SELECTED);

 int i = m_funset_listbox.GetNextItem(-1, LVNI_SELECTED);
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

void CFirmwareTabDlg::OnExportMapsToMPSZ()
{
 if (m_OnExportMapsToMPSZ)
  m_OnExportMapsToMPSZ();
}

void CFirmwareTabDlg::OnFirmwareSupportViewAttenuatorMap()
{
 //если кнопку "выключили" то закрываем окно редактора
 if (m_view_attenuator_map_btn.GetCheck()==BST_UNCHECKED)
 {
  ::SendMessage(m_attenuator_map_wnd_handle,WM_CLOSE,0,0); 
  return;
 }

 if ((!m_attenuator_map_chart_state)&&(DLL::UOZ1_Chart2DCreate))	 
 {
  m_attenuator_map_chart_state = 1;	
  m_attenuator_map_wnd_handle = DLL::UOZ1_Chart2DCreate(GetAttenuatorMap(true),GetAttenuatorMap(false),0.0f,63,m_attenuator_table_slots,128,
    MLL::GetString(IDS_FW_MAPS_RPM_UNIT).c_str(),
    MLL::GetString(IDS_FW_MAPS_ATTENUATOR_GAIN_UNIT).c_str(),
    MLL::GetString(IDS_FW_ATTENUATOR_MAP).c_str());
  DLL::UOZ1_Chart2DSetMarksVisible(m_attenuator_map_wnd_handle,1, false); //прячем надписи над узловыми точками функции
  DLL::UOZ1_Chart2DSetOnChange(m_attenuator_map_wnd_handle,OnChangeAttenuatorTable, this);
  DLL::UOZ1_Chart2DSetOnClose(m_attenuator_map_wnd_handle,OnCloseAttenuatorTable, this);
  DLL::UOZ1_Chart2DSetOnGetAxisLabel(m_attenuator_map_wnd_handle, 0, OnGetYAxisLabel, this);
  DLL::UOZ1_Chart2DSetAxisValuesFormat(m_attenuator_map_wnd_handle, 0, _T("#0.00"));
  DLL::UOZ1_Chart2DInverseAxis(m_attenuator_map_wnd_handle, 0, true);

   //allow controller to detect closing of this window
  if (m_OnOpenMapWnd)
   m_OnOpenMapWnd(m_attenuator_map_wnd_handle, TYPE_MAP_ATTENUATOR);

  DLL::UOZ1_Chart2DShow(m_attenuator_map_wnd_handle, true);
 }
 else
 {
  ::SetFocus(m_attenuator_map_wnd_handle);
 }	
}

void CFirmwareTabDlg::OnWirmwareInfo()
{
 if (m_OnFirmwareInfo)
  m_OnFirmwareInfo();
}

void CFirmwareTabDlg::_RegisterHotKeys(void)
{
 m_hot_keys_supplier->RegisterCommand(IDM_OPEN_FLASH, 'O', MOD_CONTROL);
 m_hot_keys_supplier->RegisterCommand(IDM_SAVE_FLASH, 'S', MOD_CONTROL);
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

float* CFirmwareTabDlg::GetStartMap(bool i_original) 
{
 if (i_original)
  return m_start_map_original;
 else
  return m_start_map_active;
}

float* CFirmwareTabDlg::GetIdleMap(bool i_original) 
{
 if (i_original)
  return m_idle_map_original;
 else
  return m_idle_map_active;
}

float* CFirmwareTabDlg::GetWorkMap(bool i_original) 
{
 if (i_original)
  return &m_work_map_original[0][0];
 else
  return &m_work_map_active[0][0];
}

float* CFirmwareTabDlg::GetTempMap(bool i_original) 
{
 if (i_original)
  return m_temp_map_original;
 else
  return m_temp_map_active;
}

float* CFirmwareTabDlg::GetAttenuatorMap(bool i_original) 
{
 if (i_original)
  return m_attenuator_map_original;
 else
  return m_attenuator_map_active;
}

HWND CFirmwareTabDlg::GetMapWindow(int wndType)
{
 switch(wndType)
 {
 case TYPE_MAP_DA_START:
  return m_start_map_chart_state ? m_start_map_wnd_handle : NULL;
 case TYPE_MAP_DA_IDLE:
  return m_idle_map_chart_state ? m_idle_map_wnd_handle : NULL;
 case TYPE_MAP_DA_WORK:
  return m_work_map_chart_state ? m_work_map_wnd_handle : NULL;
 case TYPE_MAP_DA_TEMP_CORR:
  return m_temp_map_chart_state ? m_temp_map_wnd_handle : NULL;
 case TYPE_MAP_ATTENUATOR: 
  return m_attenuator_map_wnd_handle ? m_attenuator_map_wnd_handle : NULL;
 default:
  return NULL;
 }
}

bool CFirmwareTabDlg::IsFirmwareOpened(void) 
{
 if (m_IsFirmwareOpened)
  return m_IsFirmwareOpened();
 return false; 
}

void CFirmwareTabDlg::setOnBootLoaderInfo(EventHandler OnFunction)
{m_OnBootLoaderInfo = OnFunction;}

void CFirmwareTabDlg::setOnReadEepromToFile(EventHandler OnFunction)
{m_OnReadEepromToFile = OnFunction;}

void CFirmwareTabDlg::setOnWriteEepromFromFile(EventHandler OnFunction)
{m_OnWriteEepromFromFile = OnFunction;}

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

void CFirmwareTabDlg::setOnMapChanged(EventWithCode OnFunction)
{m_OnMapChanged = OnFunction;}

void CFirmwareTabDlg::setOnFunSetSelectionChanged(EventWithCode OnFunction)
{m_OnFunSetSelectionChanged = OnFunction;}

void CFirmwareTabDlg::setOnFunSetNamechanged(EventWithCodeAndString OnFunction)
{m_OnFunSetNamechanged = OnFunction;}

void CFirmwareTabDlg::setOnImportDataFromAnotherFW(EventHandler OnFunction)
{m_OnImportDataFromAnotherFW = OnFunction;}	

void CFirmwareTabDlg::setOnReadFlashFromSECU(EventHandler OnFunction)
{m_OnReadFlashFromSECU = OnFunction;}

void CFirmwareTabDlg::setOnWriteFlashToSECU(EventHandler OnFunction)
{m_OnWriteFlashToSECU = OnFunction;}

void CFirmwareTabDlg::setOnImportDataFromSECU3(EventHandler OnFunction)
{m_OnImportDataFromSECU3 = OnFunction;}

void CFirmwareTabDlg::setOnImportMapsFromMPSZ(EventHandler OnFunction)
{m_OnImportMapsFromMPSZ = OnFunction;}

void CFirmwareTabDlg::setOnExportMapsToMPSZ(EventHandler OnFunction)
{m_OnExportMapsToMPSZ = OnFunction;}

void CFirmwareTabDlg::setOnFirmwareInfo(EventHandler OnFunction)
{m_OnFirmwareInfo = OnFunction;}

void CFirmwareTabDlg::setOnBLStartedEmergency(EventHandler OnFunction)
{m_OnBLStartedEmergency = OnFunction;}

void CFirmwareTabDlg::setOnCloseMapWnd(EventWithHWND OnFunction)
{ m_OnCloseMapWnd = OnFunction; }

void CFirmwareTabDlg::setOnOpenMapWnd(EventWithHWND OnFunction)
{ m_OnOpenMapWnd = OnFunction; }
