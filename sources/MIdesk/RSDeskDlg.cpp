 /****************************************************************
 *       SECU-3  - An open source, free engine control unit
 *    Designed by Alexey A. Shabelnikov. Ukraine, Gorlovka 2007.
 *       Microprocessors systems - design & programming.
 *    contacts:
 *              http://secu-3.narod.ru
 *              ICQ: 405-791-931
 ****************************************************************/

#include "stdafx.h"
#include "RSDeskDlg.h"
#include "resource.h"
#include "io-core/NumericConv.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRSDeskDlg dialog


CRSDeskDlg::CRSDeskDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRSDeskDlg::IDD, pParent)
{

	m_map_value  = 0.0f;
	m_ubat_value = 0.0f;
	m_temp_value = 0.0f;

	//{{AFX_DATA_INIT(CRSDeskDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CRSDeskDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRSDeskDlg)
	
	DDX_Control(pDX, IDC_RS_MAP_VALUE, m_map_field);
	DDX_Control(pDX, IDC_RS_UBAT_VALUE, m_ubat_field);
	DDX_Control(pDX, IDC_RS_TEMP_VALUE, m_temp_field);

	DDX_Control(pDX, IDC_RS_MAP_CAPTION, m_map_caption);
	DDX_Control(pDX, IDC_RS_UBAT_CAPTION, m_ubat_caption);
	DDX_Control(pDX, IDC_RS_TEMP_CAPTION, m_temp_caption);
	
	DDX_Control(pDX, IDC_RS_MAP_UNIT, m_map_unit);
	DDX_Control(pDX, IDC_RS_UBAT_UNIT, m_ubat_unit);
	DDX_Control(pDX, IDC_RS_TEMP_UNIT, m_temp_unit);

    DDX_Text(pDX,IDC_RS_MAP_VALUE,m_map_value);
    DDX_Text(pDX,IDC_RS_UBAT_VALUE,m_ubat_value);
    DDX_Text(pDX,IDC_RS_TEMP_VALUE,m_temp_value);	
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRSDeskDlg, CDialog)
	//{{AFX_MSG_MAP(CRSDeskDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRSDeskDlg message handlers

BOOL CRSDeskDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
		

    Enable(false);

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//разрешение/запрещение приборов
void CRSDeskDlg::Enable(bool enable)
{
  BOOL b_enable = ((enable) ? TRUE : FALSE);
  m_map_field.EnableWindow(b_enable);
  m_ubat_field.EnableWindow(b_enable);
  m_temp_field.EnableWindow(b_enable);

  m_map_caption.EnableWindow(b_enable);
  m_ubat_caption.EnableWindow(b_enable);
  m_temp_caption.EnableWindow(b_enable);

  m_map_unit.EnableWindow(b_enable);
  m_ubat_unit.EnableWindow(b_enable);
  m_temp_unit.EnableWindow(b_enable);
}

void CRSDeskDlg::Show(bool show)
{
 int sw = ((show) ? SW_SHOW : SW_HIDE);
 m_map_field.ShowWindow(sw);
 m_ubat_field.ShowWindow(sw);
 m_temp_field.ShowWindow(sw);

 m_map_caption.ShowWindow(sw);
 m_ubat_caption.ShowWindow(sw);
 m_temp_caption.ShowWindow(sw);

 m_map_unit.ShowWindow(sw);
 m_ubat_unit.ShowWindow(sw);
 m_temp_unit.ShowWindow(sw);
}

using namespace SECU3IO;

void CRSDeskDlg::SetValues(const RawSensDat* i_values)
{
  m_map_value = i_values->map_value;
  m_ubat_value = i_values->ubat_value;
  m_temp_value = i_values->temp_value;
  UpdateData(FALSE);
}

void CRSDeskDlg::GetValues(RawSensDat* o_values)
{
  UpdateData();
  o_values->map_value = m_map_value;
  o_values->ubat_value = m_ubat_value;
  o_values->temp_value = m_temp_value;
}