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
#include "ui-core/fnt_helpers.h"
#include "ui-core/CtrlScaler.h"

const UINT CGMEInjAFRDlg::IDD = IDD_GME_INJ_AFR;

/////////////////////////////////////////////////////////////////////////////
// CGMEInjAFRDlg dialog

BEGIN_MESSAGE_MAP(CGMEInjAFRDlg, Super)
 ON_UPDATE_COMMAND_UI(IDC_GME_INJ_AFR, OnUpdateControls)
 ON_WM_SIZE()
 ON_WM_DESTROY()
END_MESSAGE_MAP()

CGMEInjAFRDlg::CGMEInjAFRDlg(CWnd* pParent /*=NULL*/)
: Super(CGMEInjAFRDlg::IDD, pParent)
, m_afr_map(16, 16, true, true, NULL, 3)
, mp_AFRMap(NULL)
, mp_rpmGrid(NULL)
, mp_loadGrid(NULL)
, mp_cscl(new CtrlScaler)
, m_initialized(false)
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
  CloneWndFont(this, &m_font, -1, false);

 m_afr_map.SetRange(8.0f, 22.0f);
 m_afr_map.AttachMap(mp_AFRMap);
 m_afr_map.AttachLabels(mp_rpmGrid, mp_loadGrid);
 m_afr_map.ShowLabels(true, true);
 m_afr_map.SetDecimalPlaces(1, 0, 0);
 m_afr_map.SetFont(&m_font);
 m_afr_map.EnableAbroadMove(false, false);
 m_afr_map.SetValueIncrement(0.1f);

 //initialize scaler
 mp_cscl->Init(this);
 mp_cscl->Add(&m_afr_map);

 m_initialized = true;

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

void CGMEInjAFRDlg::BindLoadGrid(const float* pGrid, bool updateLabels /*= false*/)
{
 mp_loadGrid = pGrid;
 ASSERT(pGrid);
 if (updateLabels)
 {
  m_afr_map.AttachLabels(mp_rpmGrid, mp_loadGrid);
  if (m_afr_map.GetSafeHwnd())
   m_afr_map.UpdateDisplay(); 
 }
}

void CGMEInjAFRDlg::setOnChange(EventHandler OnCB)
{
 m_afr_map.setOnChange(OnCB);
}

void CGMEInjAFRDlg::UpdateView(bool axisLabels /*= false*/)
{
 if (axisLabels)
  m_afr_map.AttachLabels(mp_rpmGrid, mp_loadGrid); //update axis labels
 m_afr_map.UpdateDisplay();
}

void CGMEInjAFRDlg::SetArguments(int rpm, int air_flow, bool strt_use, float load)
{
 if (m_afr_map.GetSafeHwnd())
 {
  m_afr_map.ShowMarkers(!strt_use, true);
  m_afr_map.SetArguments(load, (float)rpm);
 }
}

void CGMEInjAFRDlg::OnSize( UINT nType, int cx, int cy )
{
 Super::OnSize(nType, cx, cy);
 if (m_initialized)
 {
  mp_cscl->Scale();
 }
}

void CGMEInjAFRDlg::OnDestroy()
{
 Super::OnDestroy();
 m_initialized = false;
}
