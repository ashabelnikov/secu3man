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
#include "common/FastDelegate.h"
#include "ParamMonTabDlg.h"
#include "MIDesk/MIDeskDlg.h"
#include "MIDesk/RSDeskDlg.h"
#include "ParamDesk/ParamDeskDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace fastdelegate;

BEGIN_MESSAGE_MAP(CParamMonTabDlg, CDialog)
 ON_BN_CLICKED(IDC_PM_SHOW_RAW_SENSORS, OnPmShowRawSensors)
 ON_WM_CLOSE()
END_MESSAGE_MAP()

CParamMonTabDlg::CParamMonTabDlg(CWnd* pParent /*=NULL*/)
: CTabDialog(CParamMonTabDlg::IDD, pParent)
, mp_MIDeskDlg(new CMIDeskDlg())
, mp_RSDeskDlg(new CRSDeskDlg())
, mp_ParamDeskDlg(new CParamDeskDlg())
{
 //na
}

void CParamMonTabDlg::DoDataExchange(CDataExchange* pDX)
{
 CDialog::DoDataExchange(pDX);
 DDX_Control(pDX,IDC_PM_SHOW_RAW_SENSORS,m_raw_sensors_check);
 DDX_Control(pDX,IDC_PM_SAVE_NOTE_TEXT, m_save_note_text);
}

LPCTSTR CParamMonTabDlg::GetDialogID(void) const
{
 return (LPCTSTR)IDD; 
}

/////////////////////////////////////////////////////////////////////////////
// CParamMonTabDlg message handlers

BOOL CParamMonTabDlg::OnInitDialog() 
{
 CDialog::OnInitDialog();
	
 mp_MIDeskDlg->Create(CMIDeskDlg::IDD, this);
 mp_MIDeskDlg->MoveWindow(280,0,427,312);
 mp_MIDeskDlg->ShowWindow(SW_SHOWNORMAL);
 mp_MIDeskDlg->Show(true);

 mp_RSDeskDlg->Create(CRSDeskDlg::IDD,this);
 mp_RSDeskDlg->MoveWindow(280,0,427,312);
 mp_RSDeskDlg->ShowWindow(SW_HIDE);
 mp_RSDeskDlg->Show(true);

 mp_ParamDeskDlg->Create(CParamDeskDlg::IDD,this);
 mp_ParamDeskDlg->SetTitle(MLL::LoadString(IDS_PM_EEPROM_PARAMETERS));
 mp_ParamDeskDlg->ShowWindow(SW_SHOWNORMAL);
 mp_ParamDeskDlg->Show(true);
	
 return TRUE;  // return TRUE unless you set the focus to a control
               // EXCEPTION: OCX Property Pages should return FALSE
}

void CParamMonTabDlg::OnPmShowRawSensors() 
{//делегируем обработку события чекбокса	
 if (m_OnRawSensorsCheck)
  m_OnRawSensorsCheck();  
}

bool CParamMonTabDlg::GetRawSensorsCheckState(void)
{
 int check = m_raw_sensors_check.GetCheck();
 return (check==BST_CHECKED) ? true : false;
}

void CParamMonTabDlg::OnClose() 
{
 //CDialog::OnClose();
}

void CParamMonTabDlg::setOnRawSensorsCheck(EventHandler i_Function)
{
 m_OnRawSensorsCheck = i_Function;
}

void CParamMonTabDlg::MakePDFloating(bool i_floating)
{
 ///////////////////////////////////////////////////////////////
 //запоминаем номер последней выбранной вкладки 
 int lastSel = mp_ParamDeskDlg->GetCurSel();
 ///////////////////////////////////////////////////////////////
  
 mp_ParamDeskDlg->DestroyWindow();
 mp_ParamDeskDlg->Create(i_floating ? CParamDeskDlg::IDD_F : CParamDeskDlg::IDD,this);
 ::SetClassLong(mp_ParamDeskDlg->m_hWnd ,GCL_STYLE, CS_NOCLOSE);
 mp_ParamDeskDlg->SetTitle(MLL::LoadString(IDS_PM_EEPROM_PARAMETERS));
 mp_ParamDeskDlg->ShowWindow(SW_SHOWNORMAL);
 mp_ParamDeskDlg->Show(true);

 ///////////////////////////////////////////////////////////////
 VERIFY(mp_ParamDeskDlg->SetCurSel(lastSel));
 ///////////////////////////////////////////////////////////////
}

void CParamMonTabDlg::EnlargeMonitor(bool i_enlarge)
{
 CRect rect;
 GetClientRect(rect);
 
 if (i_enlarge)
 {
  mp_MIDeskDlg->GetWindowRect(m_original_mi_rect);
  ScreenToClient(m_original_mi_rect);
  mp_RSDeskDlg->GetWindowRect(m_original_rs_rect); 
  ScreenToClient(m_original_rs_rect);
  CRect check_rect;
  m_raw_sensors_check.GetWindowRect(check_rect);
  ScreenToClient(check_rect);
  rect.bottom-= check_rect.Height();
  m_raw_sensors_check.SetWindowPos(0,check_rect.left,rect.bottom,0,0,SWP_NOSIZE|SWP_NOZORDER);
  m_original_check_pos = CPoint(check_rect.left, check_rect.top); //save it!  
  CRect mi_rect = m_original_mi_rect;
  _ResizeRect(rect, mi_rect);
  mp_MIDeskDlg->Resize(mi_rect);
  CRect rs_rect = m_original_rs_rect;
  _ResizeRect(rect, rs_rect);
  mp_RSDeskDlg->Resize(rs_rect);
  m_save_note_text.ShowWindow(SW_HIDE);
 }
 else
 {
  m_raw_sensors_check.SetWindowPos(0,m_original_check_pos.x,m_original_check_pos.y,0,0,SWP_NOSIZE|SWP_NOZORDER);
  mp_MIDeskDlg->Resize(m_original_mi_rect);
  mp_RSDeskDlg->Resize(m_original_rs_rect); 
  m_save_note_text.ShowWindow(SW_SHOW);
 }
}

void CParamMonTabDlg::_ResizeRect(const CRect& i_external, CRect& io_victim)
{
 float Xf = ((float)i_external.Width()) / io_victim.Width();
 float Yf = ((float)i_external.Height()) / io_victim.Height(); 
 float factor = min(Xf, Yf);
 //масштабируем
 io_victim.right = (io_victim.Width() * factor);
 io_victim.bottom = (io_victim.Height() * factor);
 io_victim.left = 0;
 io_victim.top = 0;
 //центрируем
 CPoint center_external = i_external.CenterPoint();
 CPoint center_victim = io_victim.CenterPoint();
 io_victim.OffsetRect(center_external - center_victim);
}
