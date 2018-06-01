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

/** \file GMEInjAFRDlg.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "resource.h"
#include "GMEInjAFRDlg.h"

const UINT CGMEInjAFRDlg::IDD = IDD_GME_INJ_AFR;

/////////////////////////////////////////////////////////////////////////////
// CGMEInjAFRDlg dialog

BEGIN_MESSAGE_MAP(CGMEInjAFRDlg, Super)
 ON_UPDATE_COMMAND_UI(IDC_GME_INJ_AFR, OnUpdateControls)
END_MESSAGE_MAP()

CGMEInjAFRDlg::CGMEInjAFRDlg(CWnd* pParent /*=NULL*/)
: Super(CGMEInjAFRDlg::IDD, pParent)
, m_afr_map(16, 16, true)
, mp_AFRMap(NULL)
, mp_rpmGrid(NULL)
, mp_loadGrid(NULL)
{
 //empty
}

CGMEInjAFRDlg::~CGMEInjAFRDlg()
{
 //empty
}

void CGMEInjAFRDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);

 DDX_Control(pDX, IDC_GME_INJ_AFR, m_afr_map);
}

/////////////////////////////////////////////////////////////////////////////
// CGMEInjAFRDlg message handlers

BOOL CGMEInjAFRDlg::OnInitDialog()
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

 m_afr_map.SetRange(.0f, 22.0f);
 m_afr_map.AttachMap(mp_AFRMap);
 m_afr_map.AttachLabels(mp_rpmGrid, mp_loadGrid);
 m_afr_map.ShowLabels(true, true);
 m_afr_map.SetDecimalPlaces(1, 0, 0);
 m_afr_map.SetFont(&m_font);
 m_afr_map.EnableAbroadMove(false, false);
 m_afr_map.SetValueIncrement(0.1f);

 UpdateDialogControls(this, true);
 UpdateData(FALSE);
 return TRUE;  // return TRUE unless you set the focus to a control
}

void CGMEInjAFRDlg::OnUpdateControls(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(IsWindowEnabled());
}

LPCTSTR CGMEInjAFRDlg::GetDialogID(void) const
{
 return (LPCTSTR)IDD;
}

void CGMEInjAFRDlg::BindMaps(float* pAFR)
{
 ASSERT(pAFR);
 mp_AFRMap = pAFR;
}

void CGMEInjAFRDlg::BindRPMGrid(float* pGrid)
{
 mp_rpmGrid = pGrid;
 ASSERT(pGrid);
}

void CGMEInjAFRDlg::BindLoadGrid(float* pGrid)
{
 mp_loadGrid = pGrid;
 ASSERT(pGrid);
}

void CGMEInjAFRDlg::setOnChange(EventHandler OnCB)
{
 m_afr_map.setOnChange(OnCB);
}

void CGMEInjAFRDlg::UpdateView(void)
{
 m_afr_map.UpdateDisplay();
}

void CGMEInjAFRDlg::SetArguments(int rpm, int air_flow, bool strt_use)
{
 if (m_afr_map.GetSafeHwnd())
 {
  m_afr_map.ShowMarkers(!strt_use, false);
  m_afr_map.SetArguments((float)air_flow, (float)rpm);
 }
}
