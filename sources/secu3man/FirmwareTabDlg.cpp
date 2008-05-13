 /****************************************************************
 *       SECU-3  - An open source, free engine control unit
 *    Designed by Alexey A. Shabelnikov. Ukraine, Gorlovka 2007.
 *       Microprocessors systems - design & programming.
 *    contacts:
 *              http://secu-3.narod.ru
 *              ICQ: 405-791-931
 ****************************************************************/

#include "stdafx.h"
#include "secu3man.h"
#include "FirmwareTabDlg.h"
#include "FirmwareContextMenuManager.h"
#include "DLLLinkedFunctions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define TIMER_ID 0

/////////////////////////////////////////////////////////////////////////////
// CFirmwareTabDlg dialog


CFirmwareTabDlg::CFirmwareTabDlg(CWnd* pParent /*=NULL*/)
: CTabDialog(CFirmwareTabDlg::IDD, pParent)	
, m_is_bl_started_emergency_available(false)
, m_is_bl_items_available(false)
{
  //{{AFX_DATA_INIT(CFirmwareTabDlg)
	// NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT
  m_ContextMenuManager.CreateContent();

  m_work_map_chart_state  = 0;
  m_temp_map_chart_state  = 0;
  m_start_map_chart_state = 0;
  m_idle_map_chart_state  = 0;

}


void CFirmwareTabDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CFirmwareTabDlg)
  DDX_Control(pDX, IDC_FIRMWARE_SUPPORT_BL_STARTED_EMERGENCY, m_bl_started_emergency);
  DDX_Control(pDX, IDC_FIRMWARE_SUPPORT_VIEW_WORK_MAP, m_view_work_map_btn);
  DDX_Control(pDX, IDC_FIRMWARE_SUPPORT_VIEW_TEMP_MAP, m_view_temp_map_btn);
  DDX_Control(pDX, IDC_FIRMWARE_SUPPORT_VIEW_START_MAP, m_view_start_map_btn);
  DDX_Control(pDX, IDC_FIRMWARE_SUPPORT_VIEW_IDLE_MAP, m_view_idle_map_btn);
  //}}AFX_DATA_MAP
}

LPCTSTR CFirmwareTabDlg::GetDialogID(void) const
{
  return (LPCTSTR)IDD; 
}

BEGIN_MESSAGE_MAP(CFirmwareTabDlg, CDialog)
  //{{AFX_MSG_MAP(CFirmwareTabDlg)
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
  ON_WM_TIMER()
  ON_WM_DESTROY()
  ON_COMMAND(IDM_READ_BOOTLOADER_SIGNATURE, OnBootLoaderInfo)
  ON_COMMAND(IDM_READ_EEPROM_TO_FILE, OnReadEepromToFile)
  ON_COMMAND(IDM_WRITE_EEPROM_FROM_FILE, OnWriteEepromFromFile)
  ON_COMMAND(IDM_READ_FLASH_TO_FILE, OnReadFlashToFile)
  ON_COMMAND(IDM_WRITE_FLASH_FROM_FILE, OnWriteFlashFromFile)
  ON_UPDATE_COMMAND_UI(IDC_FIRMWARE_SUPPORT_BL_STARTED_EMERGENCY, OnUpdateBLStartedEmergency)
  ON_UPDATE_COMMAND_UI(IDM_WRITE_FLASH, OnUpdatePopupMenu_bl)
  ON_UPDATE_COMMAND_UI(IDM_SAVE_FLASH, OnUpdatePopupMenu_file)
  ON_UPDATE_COMMAND_UI(IDM_READ_FLASH_TO_FILE, OnUpdatePopupMenu_bl)  
  ON_UPDATE_COMMAND_UI(IDM_WRITE_FLASH_FROM_FILE, OnUpdatePopupMenu_bl)  
  ON_UPDATE_COMMAND_UI(IDM_READ_EEPROM_TO_FILE, OnUpdatePopupMenu_bl)
  ON_UPDATE_COMMAND_UI(IDM_WRITE_EEPROM_FROM_FILE, OnUpdatePopupMenu_bl)
  ON_UPDATE_COMMAND_UI(IDM_READ_BOOTLOADER_SIGNATURE, OnUpdatePopupMenu_bl)
	ON_BN_CLICKED(IDC_FIRMWARE_SUPPORT_BL_STARTED_EMERGENCY, OnFirmwareSupportBlStartedEmergency)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFirmwareTabDlg message handlers

BOOL CFirmwareTabDlg::OnInitDialog() 
{
  CDialog::OnInitDialog();
	
  m_ParamDeskDlg.Create(CParamDeskDlg::IDD,this);
  m_ParamDeskDlg.SetPosition(390,10);	
  m_ParamDeskDlg.SetTitle("Резервные параметры:");
  m_ParamDeskDlg.ShowWindow(SW_SHOWNORMAL);

  m_ContextMenuManager.Attach(this);

  SetTimer(TIMER_ID,250,NULL);

  UpdateDialogControls(this,TRUE);
  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}

void CFirmwareTabDlg::OnContextMenu(CWnd* pWnd, CPoint point) 
{
  m_ContextMenuManager.TrackPopupMenu(point.x, point.y);	
}

void CFirmwareTabDlg::OnInitMenuPopup(CMenu* pMenu, UINT nIndex, BOOL bSysMenu) 
{
  m_ContextMenuManager.OnInitMenuPopup(pMenu, nIndex, bSysMenu);
}


void CFirmwareTabDlg::OnUpdatePopupMenu_bl(CCmdUI* pCmdUI) 
{
  //пока не используем...
  if ((pCmdUI->m_nID==IDM_WRITE_FLASH)||((pCmdUI->m_nID==IDM_READ_FLASH)))
  {
  pCmdUI->Enable(FALSE);
  return;
  }

  BOOL enable = m_is_bl_items_available ? TRUE : FALSE;
  pCmdUI->Enable(enable);
  pCmdUI->SetCheck(FALSE);	
}

void CFirmwareTabDlg::OnUpdatePopupMenu_file(CCmdUI* pCmdUI) 
{
  pCmdUI->Enable(FALSE);
  pCmdUI->SetCheck(FALSE);	
}


void CFirmwareTabDlg::OnUpdateBLStartedEmergency(CCmdUI* pCmdUI)
{
  BOOL enable = m_is_bl_started_emergency_available ? TRUE : FALSE;
  pCmdUI->Enable(enable);
}


void CFirmwareTabDlg::EnableBLStartedEmergency(bool enable)
{
  m_is_bl_started_emergency_available = enable;
}

void CFirmwareTabDlg::OnFirmwareSupportViewStartMap() 
{
 int s[16] = {200,240,280,320,360,400,440,480,520,560,600,640,680,720,760,800}; 
 static float  original[16] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
 static float  modified[16] = {0.5,3,2,4,5,7,6,9,9,10,14,12,14,30,35,50};

 static HWND h_uoz = NULL;
    
 //если кнопку "выключили" то закрываем окно редактора
 if (m_view_start_map_btn.GetCheck()==BST_UNCHECKED)
 {
   ::SendMessage(h_uoz,WM_CLOSE,0,0); 
   return;
 }

 if ((!m_start_map_chart_state)&&(DLL::DisplayChart_UOZ1))	 
  {
    m_start_map_chart_state = 1;
	h_uoz = DLL::DisplayChart_UOZ1(original,modified,-15.0,55.0,s,16,(LPCTSTR)"Пусковая карта УОЗ",&m_start_map_chart_state);	  
  }
  else
  {
	::SetFocus(h_uoz);
  }	
	
}

void CFirmwareTabDlg::OnFirmwareSupportViewIdleMap() 
{
 int s[16] = {600,720,840,990,1170,1380,1650,1950,2310,2730,3210,3840,4530,5370,6360,7500}; 
 static float  original[16] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
 static float  modified[16] = {0.5,3,2,4,5,7,6,9,9,10,14,12,14,30,35,50};

 static HWND h_uoz = NULL;

 //если кнопку "выключили" то закрываем окно редактора
 if (m_view_idle_map_btn.GetCheck()==BST_UNCHECKED)
 {
   ::SendMessage(h_uoz,WM_CLOSE,0,0); 
   return;
 }

 if ((!m_idle_map_chart_state)&&(DLL::DisplayChart_UOZ1))	 
  {
    m_idle_map_chart_state = 1;
	h_uoz = DLL::DisplayChart_UOZ1(original,modified,-15.0,55.0,s,16,(LPCTSTR)"Карата УОЗ на холостом ходу",&m_idle_map_chart_state);	  
  }
  else
  {
	::SetFocus(h_uoz);
  }			
}

void CFirmwareTabDlg::OnFirmwareSupportViewWorkMap() 
{
 int s[16] = {600,720,840,990,1170,1380,1650,1950,2310,2730,3210,3840,4530,5370,6360,7500}; 
 static float original[16][16];
 static float modified[16][16];
	
 static HWND h_uoz = NULL;

 //если кнопку "выключили" то закрываем окно редактора
 if (m_view_work_map_btn.GetCheck()==BST_UNCHECKED)
 {
   ::SendMessage(h_uoz,WM_CLOSE,0,0); 
   return;
 }

 if ((!m_work_map_chart_state)&&(DLL::DisplayChart_UOZ2))	 
  {
    m_work_map_chart_state = 1;
    h_uoz = DLL::DisplayChart_UOZ2(&original[0][0],&modified[0][0],s,16,16,-15.0,55.0,(LPCTSTR)"Обороты (min-1)",(LPCTSTR)"Рабочая карта УОЗ",&m_work_map_chart_state);		
  }
  else
  {
	::SetFocus(h_uoz);
  }		
}

void CFirmwareTabDlg::OnFirmwareSupportViewTempMap() 
{	
 int s[12] = {-16,-4,8,20,32,44,56,68,80,92,104,116}; 
 static float original[8][12];
 static float modified[8][12];
	
 static HWND h_uoz = NULL;

 //если кнопку "выключили" то закрываем окно редактора
 if (m_view_temp_map_btn.GetCheck()==BST_UNCHECKED)
 {
   ::SendMessage(h_uoz,WM_CLOSE,0,0); 
   return;
 }

 if ((!m_temp_map_chart_state)&&(DLL::DisplayChart_UOZ2))	 
  {
    m_temp_map_chart_state = 1;
    h_uoz = DLL::DisplayChart_UOZ2(&original[0][0],&modified[0][0],s,12,8,-15.0,25.0,(LPCTSTR)"Температура °C",(LPCTSTR)"Карта коррекции УОЗ по температуре охлаждающей жидкости",&m_temp_map_chart_state);		
  }
  else
  {
	::SetFocus(h_uoz);
  }		
}


void CFirmwareTabDlg::OnUpdateFirmwareSupportViewStartMap(CCmdUI* pCmdUI) 
{
  BOOL enable = (DLL::DisplayChart_UOZ1!=NULL);
  pCmdUI->Enable(enable);
  pCmdUI->SetCheck( (m_start_map_chart_state) ? TRUE : FALSE );	
}

void CFirmwareTabDlg::OnUpdateFirmwareSupportViewIdleMap(CCmdUI* pCmdUI) 
{
  BOOL enable = (DLL::DisplayChart_UOZ1!=NULL);
  pCmdUI->Enable(enable);
  pCmdUI->SetCheck( (m_idle_map_chart_state) ? TRUE : FALSE );	
}

void CFirmwareTabDlg::OnUpdateFirmwareSupportViewWorkMap(CCmdUI* pCmdUI) 
{
  BOOL enable = (DLL::DisplayChart_UOZ2!=NULL);
  pCmdUI->Enable(enable);
  pCmdUI->SetCheck( (m_work_map_chart_state) ? TRUE : FALSE );		
}

void CFirmwareTabDlg::OnUpdateFirmwareSupportViewTempMap(CCmdUI* pCmdUI) 
{
  BOOL enable = (DLL::DisplayChart_UOZ2!=NULL);
  pCmdUI->Enable(enable);
  pCmdUI->SetCheck( (m_temp_map_chart_state) ? TRUE : FALSE );	
}

void CFirmwareTabDlg::OnTimer(UINT nIDEvent) 
{
  //I know it is dirty hack, but... :-) 	
  UpdateDialogControls(this,TRUE);
  CDialog::OnTimer(nIDEvent);
}

void CFirmwareTabDlg::OnDestroy() 
{
  CDialog::OnDestroy();
  KillTimer(TIMER_ID);		
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
  for(size_t i = 0; i < m_ContextMenuManager.m_bl_menu_items_IDs.size(); i++)
  {
    UINT state = enable ? MF_ENABLED : MF_GRAYED;
    m_ContextMenuManager.m_ParentMenu.EnableMenuItem(m_ContextMenuManager.m_bl_menu_items_IDs[i],state);
  }
}

//от чекбокса...
void CFirmwareTabDlg::OnFirmwareSupportBlStartedEmergency() 
{
  if (m_OnBLStartedEmergency)
    m_OnBLStartedEmergency(); 	
}
