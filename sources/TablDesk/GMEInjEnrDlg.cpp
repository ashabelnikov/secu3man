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

/** \file GMEInjEnrDlg.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "resource.h"
#include "GMEInjEnrDlg.h"
#include "MapIds.h"
#include "common/MathHelpers.h"
#include "ui-core/fnt_helpers.h"

const UINT CGMEInjEnrDlg::IDD = IDD_GME_INJ_ENR;

/////////////////////////////////////////////////////////////////////////////
// CGMEInjEnrDlg dialog

BEGIN_MESSAGE_MAP(CGMEInjEnrDlg, Super)
 ON_UPDATE_COMMAND_UI(IDC_GME_INJ_AFTSTR, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_GME_INJ_WRMP, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_GME_INJ_AETPS, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_GME_INJ_AERPM, OnUpdateControls)
END_MESSAGE_MAP()

CGMEInjEnrDlg::CGMEInjEnrDlg(CWnd* pParent /*=NULL*/)
: Super(CGMEInjEnrDlg::IDD, pParent)
, m_aftstr_map(1, 16)
, m_wrmp_map(1, 16)
, m_aetps_map(1, 8)
, m_aerpm_map(1, 4)

, mp_AftstrMap(NULL)
, mp_WrmpMap(NULL)
, mp_AETPSMap(NULL)
, mp_AERPMMap(NULL)
, mp_temperGrid(NULL)
{
 m_aftstr_map.SetDecimalPlaces(1, 0, 0);
 m_wrmp_map.SetDecimalPlaces(1, 0, 0);
 m_aetps_map.SetDecimalPlaces(1, 0, 0);
 m_aerpm_map.SetDecimalPlaces(1, 0, 0);
}

CGMEInjEnrDlg::~CGMEInjEnrDlg()
{
 //empty
}

void CGMEInjEnrDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);

 DDX_Control(pDX, IDC_GME_INJ_AFTSTR, m_aftstr_map);
 DDX_Control(pDX, IDC_GME_INJ_WRMP, m_wrmp_map);
 DDX_Control(pDX, IDC_GME_INJ_AETPS, m_aetps_map);
 DDX_Control(pDX, IDC_GME_INJ_AERPM, m_aerpm_map);
}

/////////////////////////////////////////////////////////////////////////////
// CGMEInjEnrDlg message handlers

BOOL CGMEInjEnrDlg::OnInitDialog()
{
 Super::OnInitDialog();

 if (!m_font.GetSafeHandle())
  CloneWndFont(this, &m_font, -1, false);
 
 m_aftstr_map.setOnChange(fastdelegate::MakeDelegate(this, CGMEInjEnrDlg::OnChangeAftstr));
 m_aftstr_map.setOnAbroadMove(fastdelegate::MakeDelegate(this, CGMEInjEnrDlg::OnAbroadMoveAftstr));
 m_aftstr_map.SetRange(.0f, 199.0f);
 m_aftstr_map.AttachMap(mp_AftstrMap);
 m_aftstr_map.AttachLabels(mp_temperGrid, NULL);
 m_aftstr_map.ShowLabels(true, false);
 m_aftstr_map.SetFont(&m_font);
 m_aftstr_map.EnableAbroadMove(false, true);
 m_aftstr_map.SetValueIncrement(0.5f);

 m_wrmp_map.setOnChange(fastdelegate::MakeDelegate(this, CGMEInjEnrDlg::OnChangeWrmp));
 m_wrmp_map.setOnAbroadMove(fastdelegate::MakeDelegate(this, CGMEInjEnrDlg::OnAbroadMoveWrmp));
 m_wrmp_map.SetRange(.0f, 199.0f);
 m_wrmp_map.AttachMap(mp_WrmpMap);
 m_wrmp_map.AttachLabels(mp_temperGrid, NULL);
 m_wrmp_map.ShowLabels(true, false);
 m_wrmp_map.SetFont(&m_font);
 m_wrmp_map.EnableAbroadMove(true, true);
 m_wrmp_map.SetValueIncrement(0.5f);

 m_aetps_map.setOnChange(fastdelegate::MakeDelegate(this, CGMEInjEnrDlg::OnChangeAETPS));
 m_aetps_map.setOnAbroadMove(fastdelegate::MakeDelegate(this, CGMEInjEnrDlg::OnAbroadMoveAETPS));
 m_aetps_map.SetRange(-55.0f, 199.0f);
 m_aetps_map.AttachMap(mp_AETPSMap);
 m_aetps_map.AttachLabels(mp_AETPSMap + 8, NULL);
 m_aetps_map.ShowLabels(true, false);
 m_aetps_map.SetFont(&m_font);
 m_aetps_map.EnableAbroadMove(true, true);
 m_aetps_map.SetValueIncrement(1.0f);

 m_aerpm_map.setOnChange(fastdelegate::MakeDelegate(this, CGMEInjEnrDlg::OnChangeAERPM));
 m_aerpm_map.setOnAbroadMove(fastdelegate::MakeDelegate(this, CGMEInjEnrDlg::OnAbroadMoveAERPM));
 m_aerpm_map.SetRange(.0f, 199.0f);
 m_aerpm_map.AttachMap(mp_AERPMMap);
 m_aerpm_map.AttachLabels(mp_AERPMMap + 4, NULL);
 m_aerpm_map.ShowLabels(true, false);
 m_aerpm_map.SetFont(&m_font);
 m_aerpm_map.EnableAbroadMove(true, false);
 m_aerpm_map.SetValueIncrement(1.0f);

 UpdateDialogControls(this, true);
 UpdateData(FALSE);
 return TRUE;  // return TRUE unless you set the focus to a control
}

void CGMEInjEnrDlg::OnUpdateControls(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(IsWindowEnabled());
}

LPCTSTR CGMEInjEnrDlg::GetDialogID(void) const
{
 return (LPCTSTR)IDD;
}

void CGMEInjEnrDlg::BindMaps(float* pAftstr, float* pWrmp, float* pAETPS, float* pAERPM)
{
 ASSERT(pAftstr);
 mp_AftstrMap = pAftstr;

 ASSERT(pWrmp);
 mp_WrmpMap = pWrmp;

 ASSERT(pAETPS);
 mp_AETPSMap = pAETPS;

 ASSERT(pAERPM);
 mp_AERPMMap = pAERPM;
}

void CGMEInjEnrDlg::BindTemperGrid(float* pGrid)
{
 mp_temperGrid = pGrid;
 ASSERT(pGrid);
}

void CGMEInjEnrDlg::setOnChange(EventWithCode OnCB)
{
 m_OnChange = OnCB;
}

void CGMEInjEnrDlg::UpdateView(void)
{
 m_aetps_map.AttachLabels(mp_AETPSMap + 8, NULL);
 m_aerpm_map.AttachLabels(mp_AERPMMap + 4, NULL);

 m_aftstr_map.UpdateDisplay();
 m_wrmp_map.UpdateDisplay();
 m_aetps_map.UpdateDisplay();
 m_aerpm_map.UpdateDisplay();
}


void CGMEInjEnrDlg::SetArguments(bool strt_use, float clt, int tpsdot, int rpm)
{/*
 if (m_aftstr_map.GetSafeHwnd())
 {
  m_aftstr_map.ShowMarkers(!strt_use, false);
  m_aftstr_map.SetArguments(0, clt);
 }*/

 if (m_wrmp_map.GetSafeHwnd())
 {
  m_wrmp_map.ShowMarkers(!strt_use, false);
  m_wrmp_map.SetArguments(0, clt);
 }

 if (m_aetps_map.GetSafeHwnd())
 {
  m_aetps_map.ShowMarkers(!strt_use, false);
  m_aetps_map.SetArguments(0, (float)tpsdot);
 }

 if (m_aerpm_map.GetSafeHwnd())
 {
  m_aerpm_map.ShowMarkers(!strt_use, false);
  m_aerpm_map.SetArguments(0, (float)rpm);
 }
}

void CGMEInjEnrDlg::OnChangeAftstr(void)
{
 if (m_OnChange)
  m_OnChange(TYPE_MAP_INJ_AFTSTR);
}

void CGMEInjEnrDlg::OnChangeWrmp(void)
{
 if (m_OnChange)
  m_OnChange(TYPE_MAP_INJ_WRMP);
}

void CGMEInjEnrDlg::OnChangeAETPS(void)
{
 if (m_OnChange)
  m_OnChange(TYPE_MAP_INJ_AETPS);
}

void CGMEInjEnrDlg::OnChangeAERPM(void)
{
 if (m_OnChange)
  m_OnChange(TYPE_MAP_INJ_AERPM);
}

void CGMEInjEnrDlg::OnAbroadMoveAftstr(CMapEditorCtrl::AbroadDir direction, int column)
{
 if (direction==CMapEditorCtrl::ABROAD_DOWN)
  m_wrmp_map.SetSelection(0, column);
}

void CGMEInjEnrDlg::OnAbroadMoveWrmp(CMapEditorCtrl::AbroadDir direction, int column)
{
 if (direction==CMapEditorCtrl::ABROAD_UP)
  m_aftstr_map.SetSelection(0, column);
 if (direction==CMapEditorCtrl::ABROAD_DOWN)
  m_aetps_map.SetSelection(0, column);
}

void CGMEInjEnrDlg::OnAbroadMoveAETPS(CMapEditorCtrl::AbroadDir direction, int column)
{
 if (direction==CMapEditorCtrl::ABROAD_UP)
  m_wrmp_map.SetSelection(0, column);
 if (direction==CMapEditorCtrl::ABROAD_DOWN)
  m_aerpm_map.SetSelection(0, column);
}

void CGMEInjEnrDlg::OnAbroadMoveAERPM(CMapEditorCtrl::AbroadDir direction, int column)
{
 if (direction==CMapEditorCtrl::ABROAD_UP)
  m_aetps_map.SetSelection(0, column);
}
