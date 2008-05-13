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
#include "AppSettingsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAppSettings dialog


CAppSettingsDlg::CAppSettingsDlg(CWnd* pParent /*=NULL*/)
: CDialog(CAppSettingsDlg::IDD, pParent)
, m_OnOk(NULL)
, m_OnCancel(NULL)
, m_OnActivate(NULL)
{
	//{{AFX_DATA_INIT(CAppSettingsDlg)
	m_map_offset = 0.0f;
	m_map_slopping = 0.0f;
	m_app_baudrate = -1;
	m_bl_baudrate = -1;
	m_port_number = -1;
	//}}AFX_DATA_INIT
}


void CAppSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAppSettingsDlg)
	DDX_Control(pDX, IDC_APP_SETTINGS_PORT_SELECTION_COMBO, m_port_selection_combo);
	DDX_Control(pDX, IDC_APP_SETTINGS_MAP_SLOPPING_SPIN, m_map_slopping_spin);
	DDX_Control(pDX, IDC_APP_SETTINGS_MAP_SLOPPING_EDIT, m_map_slopping_edit);
	DDX_Control(pDX, IDC_APP_SETTINGS_MAP_OFFSET_SPIN, m_map_offset_spin);
	DDX_Control(pDX, IDC_APP_SETTINGS_MAP_OFFSET_EDIT, m_map_offset_edit);
	DDX_Control(pDX, IDC_APP_SETTINGS_BL_BAUDRATE_SELECTION_COMBO, m_bl_baudrate_selection_combo);
	DDX_Control(pDX, IDC_APP_SETTINGS_APP_BAUDRATE_SELECTION_COMBO, m_app_baudrate_selection_combo);

	m_map_offset_edit.DDX_Value(pDX, IDC_APP_SETTINGS_MAP_OFFSET_EDIT, m_map_offset);
	m_map_slopping_edit.DDX_Value(pDX, IDC_APP_SETTINGS_MAP_SLOPPING_EDIT, m_map_slopping);
	DDX_CBIndex(pDX, IDC_APP_SETTINGS_APP_BAUDRATE_SELECTION_COMBO, m_app_baudrate);
	DDX_CBIndex(pDX, IDC_APP_SETTINGS_BL_BAUDRATE_SELECTION_COMBO, m_bl_baudrate);
	DDX_CBIndex(pDX, IDC_APP_SETTINGS_PORT_SELECTION_COMBO, m_port_number);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAppSettingsDlg, CDialog)
	//{{AFX_MSG_MAP(CAppSettingsDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAppSettings message handlers

BOOL CAppSettingsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_map_slopping_edit.SetLimitText(6);
    m_map_slopping_edit.SetDecimalPlaces(2);
    m_map_slopping_edit.SetMode(CEditEx::MODE_FLOAT | CEditEx::MODE_SIGNED);
	m_map_slopping_spin.SetBuddy(&m_map_slopping_edit);
    m_map_slopping_spin.SetRangeAndDelta(0.2f,100.0f,0.01f);
	
	m_map_offset_edit.SetLimitText(6);
    m_map_offset_edit.SetDecimalPlaces(2);
    m_map_offset_edit.SetMode(CEditEx::MODE_FLOAT | CEditEx::MODE_SIGNED);
	m_map_offset_spin.SetBuddy(&m_map_offset_edit);
    m_map_offset_spin.SetRangeAndDelta(0.0f,5.0f,0.01f);
	
	if (m_OnActivate)
	  m_OnActivate(); //информируем слушателя о том что мы готовы к приему данных

	UpdateData(FALSE);
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
