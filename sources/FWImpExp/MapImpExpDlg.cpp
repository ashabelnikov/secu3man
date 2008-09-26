/****************************************************************
 *       SECU-3  - An open source, free engine control unit
 *    Designed by Alexey A. Shabelnikov. Ukraine, Gorlovka 2008.
 *       Microprocessors systems - design & programming.
 *    contacts:
 *              http://secu-3.narod.ru
 *              ICQ: 405-791-931
 ****************************************************************/

#include "stdafx.h"
#include "MapImpExpDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMapImpExpDlg dialog


CMapImpExpDlg::CMapImpExpDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMapImpExpDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMapImpExpDlg)
	m_current_fwd_title_string = _T("");
	m_other_fwd_title_string = _T("");
	m_idle_map_flag = FALSE;
	m_start_map_flag = FALSE;
	m_temp_map_flag = FALSE;
	m_work_map_flag = FALSE;
	//}}AFX_DATA_INIT
}


void CMapImpExpDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMapImpExpDlg)
	DDX_Control(pDX, IDC_MAP_IMPEXP_OTHER_FWD_LIST, m_other_fwd_list);
	DDX_Control(pDX, IDC_MAP_IMPEXP_CURRENT_FWD_LIST, m_current_fwd_list);
	DDX_Text(pDX, IDC_MAP_IMPEXP_CURRENT_FWD_TITLE, m_current_fwd_title_string);
	DDX_Text(pDX, IDC_MAP_IMPEXP_OTHER_FWD_TITLE, m_other_fwd_title_string);
	DDX_Check(pDX, IDC_MAP_IMPEXP_IDLEMAP_FLAG, m_idle_map_flag);
	DDX_Check(pDX, IDC_MAP_IMPEXP_STARTMAP_FLAG, m_start_map_flag);
	DDX_Check(pDX, IDC_MAP_IMPEXP_TEMPMAP_FLAG, m_temp_map_flag);
	DDX_Check(pDX, IDC_MAP_IMPEXP_WORKMAP_FLAG, m_work_map_flag);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMapImpExpDlg, CDialog)
	//{{AFX_MSG_MAP(CMapImpExpDlg)
	ON_BN_CLICKED(IDC_MAP_IMPEXP_EXCHANGE_BUTTON, OnMapImpexpExchangeButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMapImpExpDlg message handlers

void CMapImpExpDlg::OnMapImpexpExchangeButton() 
{
	// TODO: Add your control notification handler code here
	
}
