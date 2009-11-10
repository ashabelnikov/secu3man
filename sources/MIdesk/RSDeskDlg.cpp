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
#include "RSDeskDlg.h"

#include "io-core\SECU3IO.h"
#include "MIHelpers.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CRSDeskDlg, CDialog)
END_MESSAGE_MAP()

const UINT CRSDeskDlg::IDD = IDD_RAW_SENSORS_DESK;

/////////////////////////////////////////////////////////////////////////////
// CRSDeskDlg dialog

CRSDeskDlg::CRSDeskDlg(CWnd* pParent /*=NULL*/)
: Super(CRSDeskDlg::IDD, pParent)
{
 m_map_value  = 0.0f;
 m_ubat_value = 0.0f;
 m_temp_value = 0.0f;
 m_knock_value = 0.0f;
}

void CRSDeskDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);
	
 DDX_Control(pDX, IDC_RS_MAP_VALUE, m_map_field);
 DDX_Control(pDX, IDC_RS_UBAT_VALUE, m_ubat_field);
 DDX_Control(pDX, IDC_RS_TEMP_VALUE, m_temp_field);
 DDX_Control(pDX, IDC_RS_KNOCK_VALUE, m_knock_field);

 DDX_Control(pDX, IDC_RS_MAP_CAPTION, m_map_caption);
 DDX_Control(pDX, IDC_RS_UBAT_CAPTION, m_ubat_caption);
 DDX_Control(pDX, IDC_RS_TEMP_CAPTION, m_temp_caption);
 DDX_Control(pDX, IDC_RS_KNOCK_CAPTION, m_knock_caption);
	
 DDX_Control(pDX, IDC_RS_MAP_UNIT, m_map_unit);
 DDX_Control(pDX, IDC_RS_UBAT_UNIT, m_ubat_unit);
 DDX_Control(pDX, IDC_RS_TEMP_UNIT, m_temp_unit);
 DDX_Control(pDX, IDC_RS_KNOCK_UNIT, m_knock_unit);

 DDX_Text(pDX,IDC_RS_MAP_VALUE,m_map_value);
 DDX_Text(pDX,IDC_RS_UBAT_VALUE,m_ubat_value);
 DDX_Text(pDX,IDC_RS_TEMP_VALUE,m_temp_value);	
 DDX_Text(pDX,IDC_RS_KNOCK_VALUE,m_knock_value);	
}

/////////////////////////////////////////////////////////////////////////////
// CRSDeskDlg message handlers

BOOL CRSDeskDlg::OnInitDialog() 
{
 Super::OnInitDialog();
		
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
 m_knock_field.EnableWindow(b_enable);

 m_map_caption.EnableWindow(b_enable);
 m_ubat_caption.EnableWindow(b_enable);
 m_temp_caption.EnableWindow(b_enable);
 m_knock_caption.EnableWindow(b_enable);

 m_map_unit.EnableWindow(b_enable);
 m_ubat_unit.EnableWindow(b_enable);
 m_temp_unit.EnableWindow(b_enable);
 m_knock_unit.EnableWindow(b_enable);
}

void CRSDeskDlg::Show(bool show)
{
 int sw = ((show) ? SW_SHOW : SW_HIDE);
 m_map_field.ShowWindow(sw);
 m_ubat_field.ShowWindow(sw);
 m_temp_field.ShowWindow(sw);
 m_knock_field.ShowWindow(sw);

 m_map_caption.ShowWindow(sw);
 m_ubat_caption.ShowWindow(sw);
 m_temp_caption.ShowWindow(sw);
 m_knock_caption.ShowWindow(sw);

 m_map_unit.ShowWindow(sw);
 m_ubat_unit.ShowWindow(sw);
 m_temp_unit.ShowWindow(sw);
 m_knock_unit.ShowWindow(sw);
}

using namespace SECU3IO;

void CRSDeskDlg::SetValues(const RawSensDat* i_values)
{
 m_map_value = i_values->map_value;
 m_ubat_value = i_values->ubat_value;
 m_temp_value = i_values->temp_value;
 m_knock_value = i_values->knock_value;
 UpdateData(FALSE);
}

void CRSDeskDlg::GetValues(RawSensDat* o_values)
{
 UpdateData();
 o_values->map_value = m_map_value;
 o_values->ubat_value = m_ubat_value;
 o_values->temp_value = m_temp_value;
 o_values->knock_value = m_knock_value;
}

void CRSDeskDlg::Resize(const CRect& i_rect)
{
 //на основе предыдущего размера окна высчитываем коэффициенты масштабирования
 CRect old_rect; 
 float Xf, Yf;
 GetWindowRect(old_rect);
 MIHelpers::CalcRectToRectRatio(i_rect, old_rect, Xf, Yf);

 MoveWindow(i_rect.left, i_rect.top, i_rect.Width(), i_rect.Height());

 //ресайзим контроллы
 CRect rect;
#define _RESIZE(wnd)\
 rect = MIHelpers::GetChildWndRect(&wnd);\
 MIHelpers::ScaleRect(rect, Xf, Yf);\
 wnd.MoveWindow(rect); 

 _RESIZE(m_map_field);
 _RESIZE(m_ubat_field);
 _RESIZE(m_temp_field);
 _RESIZE(m_knock_field);

 _RESIZE(m_map_caption);
 _RESIZE(m_ubat_caption);
 _RESIZE(m_temp_caption);
 _RESIZE(m_knock_caption);

 _RESIZE(m_map_unit);
 _RESIZE(m_ubat_unit);
 _RESIZE(m_temp_unit);
 _RESIZE(m_knock_unit);

#undef _RESIZE

 RedrawWindow();
}
