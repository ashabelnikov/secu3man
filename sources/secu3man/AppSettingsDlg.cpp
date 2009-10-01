 /****************************************************************
 *       SECU-3  - An open source, free engine control unit
 *    Designed by Alexey A. Shabelnikov. Ukraine, Gorlovka 2007.
 *       Microprocessors systems - design & programming.
 *    contacts:
 *              http://secu-3.narod.ru
 *              ICQ: 405-791-931
 ****************************************************************/

#include "stdafx.h"
#include "resource.h"
#include "AppSettingsDlg.h"
#include "ui-core/XBrowseForFolder.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CAppSettingsDlg::CAppSettingsDlg(CWnd* pParent /*=NULL*/)
: CDialog(CAppSettingsDlg::IDD, pParent)
, m_OnOk(NULL)
, m_OnCancel(NULL)
, m_OnActivate(NULL)
, m_midesk_update_period_edit(CEditEx::MODE_INT)
{
 m_app_baudrate = -1;
 m_bl_baudrate = -1;
 m_port_number = -1;
 m_log_csv_sepsymbol_index = -1;
 m_log_files_folder = _T("");
 m_use_app_folder = 0;
 m_midesk_update_period = 40;
}


void CAppSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
 CDialog::DoDataExchange(pDX);
 DDX_Control(pDX, IDC_APP_SETTINGS_PORT_SELECTION_COMBO, m_port_selection_combo);
 DDX_Control(pDX, IDC_APP_SETTINGS_BL_BAUDRATE_SELECTION_COMBO, m_bl_baudrate_selection_combo);
 DDX_Control(pDX, IDC_APP_SETTINGS_APP_BAUDRATE_SELECTION_COMBO, m_app_baudrate_selection_combo);
 DDX_Control(pDX, IDC_APP_SETTINGS_LOG_CSV_SEPSYMBOL_COMBO, m_log_csv_sepsymbol_combo);

 DDX_Control(pDX, IDC_APP_SETTINGS_LOGFOLDER_EDITBOX, m_log_files_folder_edit);
 DDX_Control(pDX, IDC_APP_SETTINGS_LOGFOLDER_BUTTON, m_log_files_folder_button);
 DDX_Control(pDX, IDC_APP_SETTINGS_LOGFOLDER_USEAPPFOLDER, m_use_app_folder_button);

 DDX_CBIndex(pDX, IDC_APP_SETTINGS_APP_BAUDRATE_SELECTION_COMBO, m_app_baudrate);
 DDX_CBIndex(pDX, IDC_APP_SETTINGS_BL_BAUDRATE_SELECTION_COMBO, m_bl_baudrate);
 DDX_CBIndex(pDX, IDC_APP_SETTINGS_PORT_SELECTION_COMBO, m_port_number);
 DDX_CBIndex(pDX, IDC_APP_SETTINGS_LOG_CSV_SEPSYMBOL_COMBO, m_log_csv_sepsymbol_index);

 DDX_Text(pDX, IDC_APP_SETTINGS_LOGFOLDER_EDITBOX, m_log_files_folder);
 DDX_Check(pDX, IDC_APP_SETTINGS_LOGFOLDER_USEAPPFOLDER, m_use_app_folder);

 DDX_Control(pDX, IDC_APP_SETTINGS_MIDESK_UPDATE_PERIOD_SPIN, m_midesk_update_period_spin);
 DDX_Control(pDX, IDC_APP_SETTINGS_MIDESK_UPDATE_PERIOD_EDIT, m_midesk_update_period_edit);
 m_midesk_update_period_edit.DDX_Value(pDX, IDC_APP_SETTINGS_MIDESK_UPDATE_PERIOD_EDIT, m_midesk_update_period);
}


BEGIN_MESSAGE_MAP(CAppSettingsDlg, CDialog)
 ON_BN_CLICKED(IDC_APP_SETTINGS_LOGFOLDER_BUTTON, OnAppSettingsLogfolderButton)
 ON_BN_CLICKED(IDC_APP_SETTINGS_LOGFOLDER_USEAPPFOLDER, OnAppSettingsLogfolderUseappfolder)
END_MESSAGE_MAP()


BOOL CAppSettingsDlg::OnInitDialog() 
{
 CDialog::OnInitDialog();

 m_midesk_update_period_edit.SetLimitText(4);
 m_midesk_update_period_spin.SetBuddy(&m_midesk_update_period_edit);
 m_midesk_update_period_spin.SetRangeAndDelta(0,1000,10);

 if (m_OnActivate)
  m_OnActivate(); //информируем слушателя о том что мы готовы к приему данных

 UpdateData(FALSE);

 OnAppSettingsLogfolderUseappfolder();

 return TRUE;  // return TRUE unless you set the focus to a control
	           // EXCEPTION: OCX Property Pages should return FALSE
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
 BOOL enable = m_use_app_folder ? FALSE : TRUE;
 m_log_files_folder_edit.EnableWindow(enable); 
 m_log_files_folder_button.EnableWindow(enable); 
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
 for(size_t i = 0; i < i_AllowablePorts.size(); i++)
 {
  m_port_selection_combo.AddString(i_AllowablePorts[i].c_str());
 }
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

//"Set" methods (model => view data transfer)
void CAppSettingsDlg::SetPortName(_TSTRING i_PortName) 
{
 int result = m_port_selection_combo.FindString(-1,i_PortName.c_str());
 if (result!=LB_ERR)
  m_port_number = result;
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
 CString str;
 m_port_selection_combo.GetLBText(m_port_number,str);
 return _TSTRING(str);
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
   
void CAppSettingsDlg::SetLogFilesFolder(const CString& i_folder)
{
 m_log_files_folder = i_folder;
}

CString CAppSettingsDlg::GetLogFilesFolder(void) const
{
 return m_log_files_folder; 
}

void CAppSettingsDlg::SetUseAppFolder(bool i_use)
{
 m_use_app_folder = i_use;
}

bool CAppSettingsDlg::GetUseAppFolder(void) const
{
 return m_use_app_folder ? true : false;
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
