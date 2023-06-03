/* SECU-3  - An open source, free engine control unit
   Copyright (C) 2007 Alexey A. Shabelnikov. Ukraine, Kiev

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

/** \file ParamMonTabDlg.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "Resources/resource.h"
#include "ParamMonTabDlg.h"

#include "common/dpiaware.h"
#include "common/FastDelegate.h"
#include "common/gdihelpers.h"
#include "common/MathHelpers.h"
#include "MIDesk/CEDeskDlg.h"
#include "MIDesk/MIDeskDlg.h"
#include "MIDesk/RSDeskDlg.h"
#include "ParamDesk/Params/ParamDeskDlg.h"
#include "ParamDesk/Tables/TablesDeskDlg.h"
#include "ui-core/Label.h"
#include "ui-core/ToolTipCtrlEx.h"
#include "About/secu-3about.h"

using namespace fastdelegate;

BEGIN_MESSAGE_MAP(CParamMonTabDlg, Super)
 ON_BN_CLICKED(IDC_PM_SHOW_RAW_SENSORS, OnPmShowRawSensors)
 ON_BN_CLICKED(IDC_PM_EDIT_TABLES, OnPmEditTables)
 ON_WM_SIZE()
 ON_WM_MOUSEMOVE()
 ON_WM_LBUTTONDOWN()
 ON_WM_LBUTTONUP()
 ON_WM_DESTROY()
END_MESSAGE_MAP()

CParamMonTabDlg::CParamMonTabDlg()
: mp_CEDeskDlg(new CCEDeskDlg())
, mp_MIDeskDlg(new CMIDeskDlg())
, mp_RSDeskDlg(new CRSDeskDlg())
, mp_ParamDeskDlg(new CParamDeskDlg())
, mp_TablesDeskDlg(new CTablesDeskDlg())
, mp_secu3orgLink(new CLabel)
, m_moveSplitter(false)
, m_initialized(false)
, m_enMakeChartsChildren(false)
, m_splitterPos(279)
{
 //=================================================================
 if (!CheckBitmaps() || !CheckAppMenu() || !CheckAbout())
  delete this;
 //=================================================================
}

void CParamMonTabDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);
 DDX_Control(pDX,IDC_PM_SHOW_RAW_SENSORS,m_raw_sensors_check);
 DDX_Control(pDX,IDC_PM_EDIT_TABLES,m_edit_tables_check);
 DDX_Control(pDX,IDC_PM_SECU3ORG_LINK, *mp_secu3orgLink);
}

LPCTSTR CParamMonTabDlg::GetDialogID(void) const
{
 return (LPCTSTR)IDD_PARAMETERS_AND_MONITOR;
}

/////////////////////////////////////////////////////////////////////////////
// CParamMonTabDlg message handlers

BOOL CParamMonTabDlg::OnInitDialog()
{
 Super::OnInitDialog();

 CRect rect = GDIHelpers::GetChildWndRect(this, IDC_PM_MIDESK_FRAME);
 mp_MIDeskDlg->Create(CMIDeskDlg::IDD, this, rect);
 mp_MIDeskDlg->Show(true);

 mp_RSDeskDlg->Create(CRSDeskDlg::IDD, this);
 mp_RSDeskDlg->Show(true);

 rect = GDIHelpers::GetChildWndRect(this, IDC_PM_CEDESK_FRAME);
 mp_CEDeskDlg->Create(CCEDeskDlg::IDD, this);
 mp_CEDeskDlg->SetWindowPos(NULL,rect.TopLeft().x,rect.TopLeft().y,0,0,SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);
 mp_CEDeskDlg->Show(true);

 bool check_state = GetEditTablesCheckState();

 rect = GDIHelpers::GetChildWndRect(this, IDC_PM_PARAMDESK_FRAME);
 mp_ParamDeskDlg->Create(this);
 mp_ParamDeskDlg->SetPosition(rect.TopLeft().x,rect.TopLeft().y);
 mp_ParamDeskDlg->SetTitle(MLL::LoadString(IDS_PM_EEPROM_PARAMETERS));
 mp_ParamDeskDlg->Show(!check_state);

 //create tables desk
 rect = GDIHelpers::GetChildWndRect(this, IDC_PM_PARAMDESK_FRAME);
 mp_TablesDeskDlg->Create(CTablesDeskDlg::IDD,this);
 mp_TablesDeskDlg->SetPosition(rect.TopLeft().x,rect.TopLeft().y);
 mp_TablesDeskDlg->SetTitle(MLL::LoadString(IDS_PM_TABLES_IN_RAM));
 mp_TablesDeskDlg->Show(check_state);
 
 //create a tooltip control and assign tooltips
 mp_ttc.reset(new CToolTipCtrlEx());
 VERIFY(mp_ttc->Create(this, WS_POPUP | TTS_ALWAYSTIP | TTS_BALLOON));
 VERIFY(mp_ttc->AddWindow(&m_edit_tables_check, MLL::GetString(IDS_PM_EDIT_TABLES_TT)));
 VERIFY(mp_ttc->AddWindow(&m_raw_sensors_check, MLL::GetString(IDS_PM_SHOW_RAW_SENSORS_TT))); 
 mp_ttc->SetMaxTipWidth(100); //Enable text wrapping
 mp_ttc->ActivateToolTips(true);

 //init HTTP link
 mp_secu3orgLink->SetLink(true);
 mp_secu3orgLink->SetTextColor(RGB(0, 0, 255));
 mp_secu3orgLink->SetFontUnderline(true);
 mp_secu3orgLink->SetLinkCursor((HCURSOR)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDC_CURSOR_HAND), IMAGE_CURSOR, 0, 0, LR_SHARED));

 //=================================================================
 CString str;
 mp_secu3orgLink->GetWindowText(str);
 if (!CheckAppUrl((LPCTSTR)str))
  DestroyWindow();
 //=================================================================

 ///////////////////////////////////////////////
 CRect rcMI, rc;
 mp_MIDeskDlg->GetWindowRect(&rcMI);
 mp_CEDeskDlg->GetWindowRect(&rc);
 m_miMargin = rc.top - rcMI.bottom;
 ///////////////////////////////////////////////

 m_initialized = true;
 return TRUE;  // return TRUE unless you set the focus to a control
}

void CParamMonTabDlg::OnDestroy()
{
 Super::OnDestroy();
 m_initialized = false;
}

void CParamMonTabDlg::OnPmShowRawSensors()
{//делегируем обработку события чекбокса

 //=================================================================
 CString str;
 mp_secu3orgLink->GetWindowText(str);
 if (!CheckAppUrl((LPCTSTR)str))
  return;
 //=================================================================

 if (m_OnRawSensorsCheck)
  m_OnRawSensorsCheck();
}

void CParamMonTabDlg::OnPmEditTables()
{//делегируем обработку события чекбокса

 //=================================================================
 CString str;
 mp_secu3orgLink->GetWindowText(str);
 if (!CheckAppUrl((LPCTSTR)str))
  return;
 //=================================================================

 bool check_state = GetEditTablesCheckState();
 mp_ParamDeskDlg->Show(!check_state);
 mp_TablesDeskDlg->Show(check_state);
 mp_TablesDeskDlg->ShowOpenedCharts(check_state);
 mp_TablesDeskDlg->MakeChartsChildren(m_enMakeChartsChildren);

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

void CParamMonTabDlg::EnableMakingChartsChildren(bool enable)
{
 m_enMakeChartsChildren = enable;
 if (mp_TablesDeskDlg.get() && ::IsWindow(mp_TablesDeskDlg->m_hWnd))
  mp_TablesDeskDlg->MakeChartsChildren(m_enMakeChartsChildren);
}

void CParamMonTabDlg::setOnRawSensorsCheck(EventHandler i_Function)
{
 m_OnRawSensorsCheck = i_Function;
}

void CParamMonTabDlg::setOnEditTablesCheck(EventHandler i_Function)
{
 m_OnEditTablesCheck = i_Function;
}

void CParamMonTabDlg::OnSize( UINT nType, int cx, int cy )
{
 if (m_initialized)
 {
  CRect rc1, rc2, etc_rc;

  rc1 = GDIHelpers::GetChildWndRect(mp_secu3orgLink.get());
  mp_secu3orgLink->MoveWindow(rc1.left, cy - rc1.Height(), rc1.Width(), rc1.Height());

  rc1 = GDIHelpers::GetChildWndRect(&m_edit_tables_check);
  m_edit_tables_check.MoveWindow(rc1.left, cy - rc1.Height(), rc1.Width(), rc1.Height());

  etc_rc = GDIHelpers::GetChildWndRect(&m_edit_tables_check);

  rc1 = GDIHelpers::GetChildWndRect(&m_raw_sensors_check);
  m_raw_sensors_check.MoveWindow(rc1.left, cy - rc1.Height(), rc1.Width(), rc1.Height());

  rc1 = GDIHelpers::GetChildWndRect(mp_ParamDeskDlg.get());
  mp_ParamDeskDlg->SetWindowPos(NULL, 0, 0, rc1.Width(), etc_rc.top - rc1.top, SWP_NOMOVE | SWP_NOZORDER);

  rc1 = GDIHelpers::GetChildWndRect(mp_TablesDeskDlg.get());
  mp_TablesDeskDlg->SetWindowPos(NULL, 0, 0, rc1.Width(), etc_rc.top - rc1.top, SWP_NOMOVE | SWP_NOZORDER);

  rc1 = GDIHelpers::GetChildWndRect(mp_CEDeskDlg.get());
  mp_CEDeskDlg->MoveWindow(rc1.left, etc_rc.top  - rc1.Height(), rc1.Width(), rc1.Height());

  rc1 = GDIHelpers::GetChildWndRect(mp_MIDeskDlg.get());
  rc2 = GDIHelpers::GetChildWndRect(mp_CEDeskDlg.get());
  mp_MIDeskDlg->SetWindowPos(NULL, 0, 0, cx - rc1.left - m_miMargin, rc2.top - rc1.top - m_miMargin, SWP_NOMOVE | SWP_NOZORDER);

  rc1 = GDIHelpers::GetChildWndRect(mp_RSDeskDlg.get());
  mp_RSDeskDlg->SetWindowPos(NULL, 0, 0, cx - rc1.left - m_miMargin, etc_rc.top - rc1.top, SWP_NOMOVE | SWP_NOZORDER);
 }

 Super::OnSize(nType, cx, cy);
}

void CParamMonTabDlg::OnMouseMove(UINT nFlags, CPoint point)
{
 CRect pd_rc = GDIHelpers::GetChildWndRect(mp_ParamDeskDlg.get());
 CRect ce_rc = GDIHelpers::GetChildWndRect(mp_CEDeskDlg.get());
 CRect captRect(pd_rc.right, pd_rc.top, ce_rc.left, ce_rc.bottom);

 if (captRect.PtInRect(point))
 {
  DPIAware dpi;
  ::SetCursor((HCURSOR)LoadImage(NULL, IDC_SIZEWE, IMAGE_CURSOR, dpi.ScaleX(24), dpi.ScaleY(24), LR_SHARED));
 }

 if (m_moveSplitter)
 {
  _MoveSplitter(point.x, m_moveStart.x);
  
  //Update saved position 
  pd_rc = GDIHelpers::GetChildWndRect(mp_ParamDeskDlg.get());
  ce_rc = GDIHelpers::GetChildWndRect(mp_CEDeskDlg.get());
  m_splitterPos = (pd_rc.right + ce_rc.left) / 2;
 }

 Super::OnMouseMove(nFlags, point);
}

void CParamMonTabDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
 CRect pd_rc = GDIHelpers::GetChildWndRect(mp_ParamDeskDlg.get());
 CRect ce_rc = GDIHelpers::GetChildWndRect(mp_CEDeskDlg.get());
 CRect captRect(pd_rc.right, pd_rc.top, ce_rc.left, ce_rc.bottom);

 if (captRect.PtInRect(point))
 {
  DPIAware dpi;
  ::SetCursor((HCURSOR)LoadImage(NULL, IDC_SIZEWE, IMAGE_CURSOR, dpi.ScaleX(24), dpi.ScaleY(24), LR_SHARED));
  m_moveSplitter = true;
  m_moveStart = point;
  m_moveStrtWidthPD = pd_rc.Width();
  m_moveStrtRectMI = GDIHelpers::GetChildWndRect(mp_MIDeskDlg.get());
  SetCapture();
 }

 return Super::OnLButtonDown(nFlags, point);
}

void CParamMonTabDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
 Super::OnLButtonUp(nFlags, point);
 m_moveSplitter = false;
 ReleaseCapture();
}

void CParamMonTabDlg::EnableToggleMapWnd(bool toggle)
{
 mp_TablesDeskDlg->EnableToggleMapWnd(toggle);
}

void CParamMonTabDlg::SetSplitterPos(int pos)
{
 DPIAware da;
 m_splitterPos = da.ScaleX(pos);

 CRect pd_rc = GDIHelpers::GetChildWndRect(mp_ParamDeskDlg.get());
 CRect ce_rc = GDIHelpers::GetChildWndRect(mp_CEDeskDlg.get());
 m_moveStrtRectMI = GDIHelpers::GetChildWndRect(mp_MIDeskDlg.get());
 m_moveStrtWidthPD = pd_rc.Width();

 _MoveSplitter(m_splitterPos, (pd_rc.right + ce_rc.left) / 2);
}

int CParamMonTabDlg::GetSplitterPos(void) const
{
 DPIAware da;
 return da.UnScaleX(m_splitterPos);
}

void CParamMonTabDlg::_MoveSplitter(int x, int start_x)
{
 CRect pd_rc = GDIHelpers::GetChildWndRect(mp_ParamDeskDlg.get());
 CRect ce_rc = GDIHelpers::GetChildWndRect(mp_CEDeskDlg.get());

 DPIAware da;
 CRect rc;
 GetClientRect(&rc);

 //restrict splitter position
 if (x < da.ScaleX(5))
  x = da.ScaleX(5);
 if (x > rc.right - m_miMargin - da.ScaleX(5))
  x = rc.right - m_miMargin - da.ScaleX(5);

 int dx = x - start_x;

 rc = GDIHelpers::GetChildWndRect(mp_TablesDeskDlg.get());
 mp_TablesDeskDlg->SetWindowPos(NULL, 0, 0, m_moveStrtWidthPD + dx, rc.Height(), SWP_NOMOVE | SWP_NOZORDER);

 mp_ParamDeskDlg->SetWindowPos(NULL, 0, 0, m_moveStrtWidthPD + dx, pd_rc.Height(), SWP_NOMOVE | SWP_NOZORDER);

 rc = GDIHelpers::GetChildWndRect(mp_MIDeskDlg.get());
 mp_MIDeskDlg->MoveWindow(m_moveStrtRectMI.left + dx, rc.top, m_moveStrtRectMI.Width() - dx, rc.Height());

 mp_CEDeskDlg->SetWindowPos(NULL, m_moveStrtRectMI.left + dx, ce_rc.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

 rc = GDIHelpers::GetChildWndRect(mp_RSDeskDlg.get());
 mp_RSDeskDlg->MoveWindow(m_moveStrtRectMI.left + dx, rc.top, m_moveStrtRectMI.Width() - dx, rc.Height());

 UpdateWindow();
}

void CParamMonTabDlg::OnShow(bool show)
{
 mp_TablesDeskDlg->OnShow(show);
}
