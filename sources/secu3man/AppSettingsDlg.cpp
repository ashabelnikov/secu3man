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
	DDX_Control(pDX, IDC_APP_SETTINGS_BL_BAUDRATE_SELECTION_COMBO, m_bl_baudrate_selection_combo);
	DDX_Control(pDX, IDC_APP_SETTINGS_APP_BAUDRATE_SELECTION_COMBO, m_app_baudrate_selection_combo);

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
