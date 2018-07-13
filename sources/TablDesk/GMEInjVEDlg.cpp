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

/** \file GMEInjVEDlg.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "resource.h"
#include "GMEInjVEDlg.h"

const UINT CGMEInjVEDlg::IDD = IDD_GME_INJ_VE;

/////////////////////////////////////////////////////////////////////////////
// CGMEInjVEDlg dialog

BEGIN_MESSAGE_MAP(CGMEInjVEDlg, Super)
 ON_UPDATE_COMMAND_UI(IDC_GME_INJ_VE, OnUpdateControls)
END_MESSAGE_MAP()

CGMEInjVEDlg::CGMEInjVEDlg(CWnd* pParent /*=NULL*/)
: Super(CGMEInjVEDlg::IDD, pParent)
, m_ve_map(16, 16, true, NULL, 3)
, mp_VEMap(NULL)
, mp_rpmGrid(NULL)
, mp_loadGrid(NULL)
{
 //empty
}

CGMEInjVEDlg::~CGMEInjVEDlg()
{
 //empty
}

void CGMEInjVEDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);

 DDX_Control(pDX, IDC_GME_INJ_VE, m_ve_map);
}

/////////////////////////////////////////////////////////////////////////////
// CGMEInjVEDlg message handlers

BOOL CGMEInjVEDlg::OnInitDialog()
{
 Super::OnInitDialog();

 if (!m_font.GetSafeHandle())
 {
  LOGFONT LogFont;
  memset(&LogFont, 0x00, sizeof(LogFont));
  _tcsncpy(LogFont.lfFaceName, _T("MS Sans Serif"), LF_FACESIZE);
  LogFont.lfHeight = 8;
  m_font.CreateFontIndirect(&LogFont);
 }

 m_ve_map.SetRange(.0f, 1.99f);
 m_ve_map.AttachMap(mp_VEMap);
 m_ve_map.AttachLabels(mp_rpmGrid, mp_loadGrid);
 m_ve_map.ShowLabels(true, true);
 m_ve_map.SetDecimalPlaces(2, 0, 0);
 m_ve_map.SetFont(&m_font);
 m_ve_map.EnableAbroadMove(false, false);
 m_ve_map.SetValueIncrement(0.01f);

 UpdateDialogControls(this, true);
 UpdateData(FALSE);
 return TRUE;  // return TRUE unless you set the focus to a control
}

void CGMEInjVEDlg::OnUpdateControls(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(IsWindowEnabled());
}

LPCTSTR CGMEInjVEDlg::GetDialogID(void) const
{
 return (LPCTSTR)IDD;
}

void CGMEInjVEDlg::BindMaps(float* pVE)
{
 ASSERT(pVE);
 mp_VEMap = pVE;
}

void CGMEInjVEDlg::BindRPMGrid(float* pGrid)
{
 mp_rpmGrid = pGrid;
 ASSERT(pGrid);
}

void CGMEInjVEDlg::BindLoadGrid(float* pGrid, bool updateLabels /*= false*/)
{
 mp_loadGrid = pGrid;
 ASSERT(pGrid);
 if (updateLabels)
 {
  m_ve_map.AttachLabels(mp_rpmGrid, mp_loadGrid);
  if (m_ve_map.GetSafeHwnd())
   m_ve_map.UpdateDisplay(); 
 }
}

void CGMEInjVEDlg::setOnChange(EventHandler OnCB)
{
 m_ve_map.setOnChange(OnCB);
}

void CGMEInjVEDlg::UpdateView(void)
{
 m_ve_map.UpdateDisplay();
}

void CGMEInjVEDlg::SetArguments(int rpm, int air_flow, bool strt_use, float load)
{
 if (m_ve_map.GetSafeHwnd())
 {
  m_ve_map.ShowMarkers(!strt_use, false);
  m_ve_map.SetArguments(load, (float)rpm);
 }
}

