/* SECU-3  - An open source, free engine control unit
   Copyright (C) 2007 Alexey A. Shabelnikov. Ukraine, Gorlovka

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

#include "stdafx.h"
#include "Resources/resource.h"
#include "ParamMonTabDlg.h"

#include "common/FastDelegate.h"
#include "common/MathHelpers.h"
#include "MIDesk/MIDeskDlg.h"
#include "MIDesk/RSDeskDlg.h"
#include "ParamDesk/Params/ParamDeskDlg.h"
#include "ParamDesk/Tables/TablesDeskDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace fastdelegate;

BEGIN_MESSAGE_MAP(CParamMonTabDlg, Super)
 ON_BN_CLICKED(IDC_PM_SHOW_RAW_SENSORS, OnPmShowRawSensors)
 ON_BN_CLICKED(IDC_PM_EDIT_TABLES, OnPmEditTables)
END_MESSAGE_MAP()

const UINT CParamMonTabDlg::IDD = IDD_PARAMETERS_AND_MONITOR;

CParamMonTabDlg::CParamMonTabDlg(CWnd* pParent /*=NULL*/)
: Super(CParamMonTabDlg::IDD, pParent)
, mp_MIDeskDlg(new CMIDeskDlg())
, mp_RSDeskDlg(new CRSDeskDlg())
, mp_ParamDeskDlg(new CParamDeskDlg())
, mp_TablesDeskDlg(new CTablesDeskDlg())
, floating(false)
{
 //na
}

void CParamMonTabDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);
 DDX_Control(pDX,IDC_PM_SHOW_RAW_SENSORS,m_raw_sensors_check);
 DDX_Control(pDX,IDC_PM_EDIT_TABLES,m_edit_tables_check);
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
 Super::OnInitDialog();

 CRect rect;
 GetDlgItem(IDC_PM_MIDESK_FRAME)->GetWindowRect(rect);
 ScreenToClient(rect);

 mp_MIDeskDlg->Create(CMIDeskDlg::IDD, this);
 mp_MIDeskDlg->SetWindowPos(NULL,rect.TopLeft().x,rect.TopLeft().y,0,0,SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);
 mp_MIDeskDlg->Show(true);

 mp_RSDeskDlg->Create(CRSDeskDlg::IDD, this);
 mp_RSDeskDlg->SetWindowPos(NULL,rect.TopLeft().x,rect.TopLeft().y,0,0,SWP_NOSIZE | SWP_NOZORDER | SWP_HIDEWINDOW);
 mp_RSDeskDlg->Show(true);

 bool check_state = GetEditTablesCheckState();

 GetDlgItem(IDC_PM_PARAMDESK_FRAME)->GetWindowRect(rect);
 ScreenToClient(rect);
 mp_ParamDeskDlg->Create(CParamDeskDlg::IDD,this);
 mp_ParamDeskDlg->SetPosition(rect.TopLeft().x,rect.TopLeft().y);
 mp_ParamDeskDlg->SetTitle(MLL::LoadString(IDS_PM_EEPROM_PARAMETERS));
 mp_ParamDeskDlg->ShowWindow(SW_SHOWNORMAL);
 mp_ParamDeskDlg->Show(!check_state);

 //create tables desk
 GetDlgItem(IDC_PM_PARAMDESK_FRAME)->GetWindowRect(rect);
 ScreenToClient(rect);
 mp_TablesDeskDlg->Create(CTablesDeskDlg::IDD,this);
 mp_TablesDeskDlg->SetPosition(rect.TopLeft().x,rect.TopLeft().y);
 mp_TablesDeskDlg->SetTitle(MLL::LoadString(IDS_PM_TABLES_IN_RAM));
 mp_TablesDeskDlg->ShowWindow(SW_SHOWNORMAL);
 mp_TablesDeskDlg->Show(check_state);
 
 return TRUE;  // return TRUE unless you set the focus to a control
               // EXCEPTION: OCX Property Pages should return FALSE
}

void CParamMonTabDlg::OnPmShowRawSensors()
{//делегируем обработку события чекбокса
 if (m_OnRawSensorsCheck)
  m_OnRawSensorsCheck();
}

void CParamMonTabDlg::OnPmEditTables()
{//делегируем обработку события чекбокса
 bool check_state = GetEditTablesCheckState();
 if (false==floating)
  mp_ParamDeskDlg->Show(!check_state);
 mp_TablesDeskDlg->Show(check_state);
 mp_TablesDeskDlg->ShowOpenedCharts(check_state);
 if (check_state)
  mp_TablesDeskDlg->MakeChartsChildren(floating);

 if (m_OnEditTablesCheck)
  m_OnEditTablesCheck();
}

bool CParamMonTabDlg::GetRawSensorsCheckState(void)
{
 int check = m_raw_sensors_check.GetCheck();
 return (check==BST_CHECKED) ? true : false;
}

bool CParamMonTabDlg::GetEditTablesCheckState(void)
{
 int check = m_edit_tables_check.GetCheck();
 return (check==BST_CHECKED) ? true : false;
}

void CParamMonTabDlg::EnableEditTablesCheck(bool enable)
{
 m_edit_tables_check.EnableWindow(enable);
}

void CParamMonTabDlg::MakePDFloating(bool i_floating)
{
 floating = i_floating;
 ///////////////////////////////////////////////////////////////
 //запоминаем номер последней выбранной вкладки
 int lastSelPD = mp_ParamDeskDlg->GetCurSel();
 int lastSelTD = mp_TablesDeskDlg->GetCurSel();
 ///////////////////////////////////////////////////////////////

 mp_ParamDeskDlg->DestroyWindow();
 mp_ParamDeskDlg->Create(i_floating ? CParamDeskDlg::IDD_F : CParamDeskDlg::IDD, this);
 ::SetClassLong(mp_ParamDeskDlg->m_hWnd ,GCL_STYLE, CS_NOCLOSE); 
 mp_ParamDeskDlg->SetTitle(MLL::LoadString(IDS_PM_EEPROM_PARAMETERS));
 if (i_floating)
 {
  mp_ParamDeskDlg->SetWindowPos(&wndTop, 0,0,0,0, SWP_NOSIZE | SWP_NOMOVE);
  mp_ParamDeskDlg->Show(true);
 }
 else
  mp_ParamDeskDlg->Show(!GetEditTablesCheckState());

 mp_TablesDeskDlg->DestroyWindow();
 mp_TablesDeskDlg->Create(i_floating ? CTablesDeskDlg::IDD_F : CTablesDeskDlg::IDD, this);
 ::SetClassLong(mp_TablesDeskDlg->m_hWnd ,GCL_STYLE, CS_NOCLOSE);
 mp_TablesDeskDlg->SetTitle(MLL::LoadString(IDS_PM_TABLES_IN_RAM));
 mp_TablesDeskDlg->ShowOpenedCharts(true);

 if (i_floating)
 {
  CRect rect;
  mp_ParamDeskDlg->GetWindowRect(rect); 
  mp_TablesDeskDlg->SetPosition(rect.right,rect.top, NULL);
  mp_TablesDeskDlg->SetWindowPos(&wndTop, 0,0,0,0, SWP_NOSIZE | SWP_NOMOVE); 
 }
 mp_TablesDeskDlg->Show(GetEditTablesCheckState());
 mp_TablesDeskDlg->MakeChartsChildren(i_floating);

 ///////////////////////////////////////////////////////////////
 VERIFY(mp_ParamDeskDlg->SetCurSel(lastSelPD));
 VERIFY(mp_TablesDeskDlg->SetCurSel(lastSelTD));
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
  ScreenToClient(check_rect); //check rect
  CRect button_rect;
  m_edit_tables_check.GetWindowRect(button_rect);
  ScreenToClient(button_rect);//button rect 
  rect.bottom-= max(check_rect.Height(), button_rect.Height());
  m_raw_sensors_check.SetWindowPos(0,check_rect.left,rect.bottom,0,0,SWP_NOSIZE|SWP_NOZORDER);
  m_original_check_pos = CPoint(check_rect.left, check_rect.top); //save it!  
  m_edit_tables_check.SetWindowPos(0,button_rect.left,rect.bottom,0,0,SWP_NOSIZE|SWP_NOZORDER);
  m_original_button_pos = CPoint(button_rect.left, button_rect.top); //save it!

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
  m_edit_tables_check.SetWindowPos(0,m_original_button_pos.x,m_original_button_pos.y,0,0,SWP_NOSIZE|SWP_NOZORDER);
  mp_MIDeskDlg->Resize(m_original_mi_rect);
  mp_RSDeskDlg->Resize(m_original_rs_rect);
  m_save_note_text.ShowWindow(SW_SHOW);
 }
}

void CParamMonTabDlg::setOnRawSensorsCheck(EventHandler i_Function)
{
 m_OnRawSensorsCheck = i_Function;
}

void CParamMonTabDlg::setOnEditTablesCheck(EventHandler i_Function)
{
 m_OnEditTablesCheck = i_Function;
}

void CParamMonTabDlg::_ResizeRect(const CRect& i_external, CRect& io_victim)
{
 float Xf = ((float)i_external.Width()) / io_victim.Width();
 float Yf = ((float)i_external.Height()) / io_victim.Height();
 float factor = min(Xf, Yf);
 //масштабируем
 io_victim.right = MathHelpers::Round((io_victim.Width() * factor));
 io_victim.bottom = MathHelpers::Round((io_victim.Height() * factor));
 io_victim.left = 0;
 io_victim.top = 0;
 //центрируем
 CPoint center_external = i_external.CenterPoint();
 CPoint center_victim = io_victim.CenterPoint();
 io_victim.OffsetRect(center_external - center_victim);
}
