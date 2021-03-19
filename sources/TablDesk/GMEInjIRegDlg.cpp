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

/** \file GMEInjIRegDlg.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "resource.h"
#include "GMEInjIRegDlg.h"
#include "MapIds.h"
#include "common/MathHelpers.h"
#include "ui-core/fnt_helpers.h"
#include "ui-core/CtrlScaler.h"

const UINT CGMEInjIRegDlg::IDD = IDD_GME_INJ_IREG;

const float RigidGrid[8] = {1,2,3,4,5,6,7,8};

static const UINT StaticCtrlBegin = IDC_GME_INJ_IDLC_TEXT;
static const UINT StaticCtrlEnd = IDC_GME_INJ_IACMAT_TEXT;

/////////////////////////////////////////////////////////////////////////////
// CGMEInjVEDlg dialog

BEGIN_MESSAGE_MAP(CGMEInjIRegDlg, Super)
 ON_UPDATE_COMMAND_UI(IDC_GME_INJ_IDLC, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_GME_INJ_IDLR, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_GME_INJ_ITRPM, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_GME_INJ_RIGID, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_GME_INJ_IACC, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_GME_INJ_IACCW, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_GME_INJ_IACMAT, OnUpdateControls)
 ON_WM_SIZE()
 ON_WM_DESTROY()
END_MESSAGE_MAP()

CGMEInjIRegDlg::CGMEInjIRegDlg(CWnd* pParent /*=NULL*/)
: Super(CGMEInjIRegDlg::IDD, pParent)
, m_idlc_map(1, 16)
, m_idlr_map(1, 16)
, m_itrpm_map(1, 16)
, m_rigid_map(1, 8)
, m_iacc_map(1, 8)
, m_iaccw_map(1, 16)
, m_iacmat_map(1, 16)
, mp_IdlcMap(NULL)
, mp_IdlrMap(NULL)
, mp_ITRPMMap(NULL)
, mp_RigidMap(NULL)
, mp_IACCMap(NULL)
, mp_IACCWMap(NULL)
, mp_IACMATMap(NULL)
, mp_cltGrid(NULL)
, mp_temperGrid(NULL)
, mp_cscl(new CtrlScaler)
, m_initialized(false)
{
 m_tpsGrid.reserve(32);
 m_iacGrid.reserve(32);
 m_idlc_map.SetDecimalPlaces(1, 0, 0);
 m_idlr_map.SetDecimalPlaces(1, 0, 0);
 m_itrpm_map.SetDecimalPlaces(0, 0, 0);
 m_rigid_map.SetDecimalPlaces(2, 0, 0);
 m_iacc_map.SetDecimalPlaces(2, 2, 0);
 m_iaccw_map.SetDecimalPlaces(2, 2, 0);
 m_iacmat_map.SetDecimalPlaces(2, 2, 0);
}

CGMEInjIRegDlg::~CGMEInjIRegDlg()
{
 //empty
}

void CGMEInjIRegDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);

 DDX_Control(pDX, IDC_GME_INJ_IDLC, m_idlc_map);
 DDX_Control(pDX, IDC_GME_INJ_IDLR, m_idlr_map);
 DDX_Control(pDX, IDC_GME_INJ_ITRPM, m_itrpm_map);
 DDX_Control(pDX, IDC_GME_INJ_RIGID, m_rigid_map);
 DDX_Control(pDX, IDC_GME_INJ_IACC, m_iacc_map);
 DDX_Control(pDX, IDC_GME_INJ_IACCW, m_iaccw_map);
 DDX_Control(pDX, IDC_GME_INJ_IACMAT, m_iacmat_map);
 for(int i = StaticCtrlBegin; i <= StaticCtrlEnd; ++i)
  DDX_Control(pDX, i, m_ctrls[i-StaticCtrlBegin]);
}

/////////////////////////////////////////////////////////////////////////////
// CGMEInjIRegDlg message handlers

BOOL CGMEInjIRegDlg::OnInitDialog()
{
 Super::OnInitDialog();

 if (!m_font.GetSafeHandle())
  CloneWndFont(this, &m_font, -1, false);
 
 m_idlc_map.setOnChange(fastdelegate::MakeDelegate(this, CGMEInjIRegDlg::OnChangeIdlc));
 m_idlc_map.setOnAbroadMove(fastdelegate::MakeDelegate(this, CGMEInjIRegDlg::OnAbroadMoveIdlc));
 m_idlc_map.SetRange(.0f, 100.0f);
 m_idlc_map.AttachMap(mp_IdlcMap);
 m_idlc_map.AttachLabels(mp_cltGrid, NULL);
 m_idlc_map.ShowLabels(true, false);
 m_idlc_map.SetFont(&m_font);
 m_idlc_map.EnableAbroadMove(false, true);
 m_idlc_map.SetValueIncrement(0.5f);

 m_idlr_map.setOnChange(fastdelegate::MakeDelegate(this, CGMEInjIRegDlg::OnChangeIdlr));
 m_idlr_map.setOnAbroadMove(fastdelegate::MakeDelegate(this, CGMEInjIRegDlg::OnAbroadMoveIdlr));
 m_idlr_map.SetRange(.0f, 100.0f);
 m_idlr_map.AttachMap(mp_IdlrMap);
 m_idlr_map.AttachLabels(mp_cltGrid, NULL);
 m_idlr_map.ShowLabels(true, false);
 m_idlr_map.SetFont(&m_font);
 m_idlr_map.EnableAbroadMove(true, true);
 m_idlr_map.SetValueIncrement(0.5f);

 m_itrpm_map.setOnChange(fastdelegate::MakeDelegate(this, CGMEInjIRegDlg::OnChangeITRPM));
 m_itrpm_map.setOnAbroadMove(fastdelegate::MakeDelegate(this, CGMEInjIRegDlg::OnAbroadMoveITRPM));
 m_itrpm_map.SetRange(.0f, 2500.0f);
 m_itrpm_map.AttachMap(mp_ITRPMMap);
 m_itrpm_map.AttachLabels(mp_cltGrid, NULL);
 m_itrpm_map.ShowLabels(true, false);
 m_itrpm_map.SetFont(&m_font);
 m_itrpm_map.EnableAbroadMove(true, true);
 m_itrpm_map.SetValueIncrement(10.0f);

 m_rigid_map.setOnChange(fastdelegate::MakeDelegate(this, CGMEInjIRegDlg::OnChangeRigid));
 m_rigid_map.setOnAbroadMove(fastdelegate::MakeDelegate(this, CGMEInjIRegDlg::OnAbroadMoveRigid));
 m_rigid_map.SetRange(.0f, 5.0f);
 m_rigid_map.AttachMap(mp_RigidMap);
 m_rigid_map.AttachLabels(RigidGrid, NULL);
 m_rigid_map.ShowLabels(true, false);
 m_rigid_map.SetFont(&m_font);
 m_rigid_map.EnableAbroadMove(true, true);
 m_rigid_map.SetValueIncrement(0.01f);

 _UpdateDynamicGrids();

 m_iacc_map.setOnChange(fastdelegate::MakeDelegate(this, CGMEInjIRegDlg::OnChangeIACC));
 m_iacc_map.setOnAbroadMove(fastdelegate::MakeDelegate(this, CGMEInjIRegDlg::OnAbroadMoveIACC));
 m_iacc_map.SetRange(.0f, 1.99f);
 m_iacc_map.AttachMap(mp_IACCMap);
 m_iacc_map.AttachLabels(&m_tpsGrid[0], NULL);
 m_iacc_map.ShowLabels(true, false);
 m_iacc_map.SetFont(&m_font);
 m_iacc_map.EnableAbroadMove(true, true);
 m_iacc_map.SetValueIncrement(0.01f);

 m_iaccw_map.setOnChange(fastdelegate::MakeDelegate(this, CGMEInjIRegDlg::OnChangeIACCW));
 m_iaccw_map.setOnAbroadMove(fastdelegate::MakeDelegate(this, CGMEInjIRegDlg::OnAbroadMoveIACCW));
 m_iaccw_map.SetRange(.0f, 1.00f);
 m_iaccw_map.AttachMap(mp_IACCWMap);
 m_iaccw_map.AttachLabels(&m_iacGrid[0], NULL);
 m_iaccw_map.ShowLabels(true, false);
 m_iaccw_map.SetFont(&m_font);
 m_iaccw_map.EnableAbroadMove(true, true);
 m_iaccw_map.SetValueIncrement(0.01f);

 m_iacmat_map.setOnChange(fastdelegate::MakeDelegate(this, CGMEInjIRegDlg::OnChangeIACMAT));
 m_iacmat_map.setOnAbroadMove(fastdelegate::MakeDelegate(this, CGMEInjIRegDlg::OnAbroadMoveIACMAT));
 m_iacmat_map.SetRange(-25.0f, 25.00f);
 m_iacmat_map.AttachMap(mp_IACMATMap);
 m_iacmat_map.AttachLabels(&mp_temperGrid[0], NULL);
 m_iacmat_map.ShowLabels(true, false);
 m_iacmat_map.SetFont(&m_font);
 m_iacmat_map.EnableAbroadMove(true, false);
 m_iacmat_map.SetValueIncrement(0.25f);

 //initialize scaler
 mp_cscl->Init(this);
 mp_cscl->Add(&m_idlc_map);
 mp_cscl->Add(&m_idlr_map);
 mp_cscl->Add(&m_itrpm_map);
 mp_cscl->Add(&m_rigid_map);
 mp_cscl->Add(&m_iacc_map);
 mp_cscl->Add(&m_iaccw_map);
 mp_cscl->Add(&m_iacmat_map);
 mp_cscl->Add(m_ctrls, (StaticCtrlEnd - StaticCtrlBegin) + 1);

 m_initialized = true;

 UpdateDialogControls(this, true);
 UpdateData(FALSE);
 return TRUE;  // return TRUE unless you set the focus to a control
}

void CGMEInjIRegDlg::OnUpdateControls(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(IsWindowEnabled());
}

LPCTSTR CGMEInjIRegDlg::GetDialogID(void) const
{
 return (LPCTSTR)IDD;
}

void CGMEInjIRegDlg::BindMaps(float* pIdlc, float* pIdlr, float* pITRPM, float* pRigid, float* pIACC, float* pIACCW, float* pIACMAT)
{
 ASSERT(pIdlc);
 mp_IdlcMap = pIdlc;

 ASSERT(pIdlr);
 mp_IdlrMap = pIdlr;

 ASSERT(pITRPM);
 mp_ITRPMMap = pITRPM;

 ASSERT(pRigid);
 mp_RigidMap = pRigid;

 ASSERT(pIACC);
 mp_IACCMap = pIACC;

 ASSERT(pIACCW);
 mp_IACCWMap = pIACCW;

 ASSERT(pIACMAT);
 mp_IACMATMap = pIACMAT;

 _UpdateDynamicGrids();
}

void CGMEInjIRegDlg::BindTemperGrid(float* pGrid)
{
 mp_temperGrid = pGrid;
 ASSERT(pGrid);
}

void CGMEInjIRegDlg::BindCLTGrid(float* pGrid)
{
 mp_cltGrid = pGrid;
 ASSERT(pGrid);
}

void CGMEInjIRegDlg::setOnChange(EventWithCode OnCB)
{
 m_OnChange = OnCB;
}

void CGMEInjIRegDlg::UpdateView(bool axisLabels /*= false*/)
{
 _UpdateDynamicGrids();

 if (axisLabels)
 {
  m_idlc_map.AttachLabels(mp_cltGrid, NULL);  //update axis labels
  m_idlr_map.AttachLabels(mp_cltGrid, NULL);  //update axis labels
  m_itrpm_map.AttachLabels(mp_cltGrid, NULL); //update axis labels
 }

 m_iacc_map.AttachLabels(&m_tpsGrid[0], NULL);
 m_iaccw_map.AttachLabels(&m_iacGrid[0], NULL);

 m_idlc_map.UpdateDisplay();
 m_idlr_map.UpdateDisplay();
 m_itrpm_map.UpdateDisplay();
 m_rigid_map.UpdateDisplay();
 m_iacc_map.UpdateDisplay();
 m_iaccw_map.UpdateDisplay();
 m_iacmat_map.UpdateDisplay();
}

void CGMEInjIRegDlg::SetArguments(bool strt_use, float clt, float tps, float iac_pos, float rigid_arg, bool rigid_use, float iat)
{
 if (m_idlc_map.GetSafeHwnd())
 {
  m_idlc_map.ShowMarkers(strt_use, true);
  m_idlc_map.SetArguments(0, clt);
 }

 if (m_idlr_map.GetSafeHwnd())
 {
  m_idlr_map.ShowMarkers(!strt_use, true);
  m_idlr_map.SetArguments(0, clt);
 }

 if (m_iacc_map.GetSafeHwnd())
 {
  m_iacc_map.ShowMarkers(!strt_use, true);
  m_iacc_map.SetArguments(0, iac_pos);
 }

 if (m_iaccw_map.GetSafeHwnd())
 {
  m_iaccw_map.ShowMarkers(!strt_use, true);
  m_iaccw_map.SetArguments(0, tps);
 }

 if (m_rigid_map.GetSafeHwnd())
 {
  m_rigid_map.ShowMarkers(rigid_use, true);
  m_rigid_map.SetArguments(0, rigid_arg);
 }

 if (m_itrpm_map.GetSafeHwnd())
 {
  m_itrpm_map.ShowMarkers(rigid_use, true);
  m_itrpm_map.SetArguments(0, clt);
 }

 if (m_iacmat_map.GetSafeHwnd())
 {
  m_iacmat_map.ShowMarkers(true, true);
  m_iacmat_map.SetArguments(0, iat);
 }
}

void CGMEInjIRegDlg::OnChangeIdlc(void)
{
 if (m_OnChange)
  m_OnChange(TYPE_MAP_INJ_IDLC);
}

void CGMEInjIRegDlg::OnChangeIdlr(void)
{
 if (m_OnChange)
  m_OnChange(TYPE_MAP_INJ_IDLR);
}

void CGMEInjIRegDlg::OnChangeITRPM(void)
{
 if (m_OnChange)
  m_OnChange(TYPE_MAP_INJ_ITRPM);
}

void CGMEInjIRegDlg::OnChangeRigid(void)
{
 if (m_OnChange)
  m_OnChange(TYPE_MAP_INJ_RIGID);
}

void CGMEInjIRegDlg::OnChangeIACC(void)
{
 if (m_OnChange)
  m_OnChange(TYPE_MAP_INJ_IACC);
}

void CGMEInjIRegDlg::OnChangeIACCW(void)
{
 if (m_OnChange)
  m_OnChange(TYPE_MAP_INJ_IACCW);
}

void CGMEInjIRegDlg::OnChangeIACMAT(void)
{
 if (m_OnChange)
  m_OnChange(TYPE_MAP_INJ_IACMAT);
}

void CGMEInjIRegDlg::_UpdateDynamicGrids(void)
{
 m_tpsGrid = MathHelpers::BuildGridFromRange(mp_IACCMap[8], mp_IACCMap[8+1], 8);
 m_iacGrid = MathHelpers::BuildGridFromRange(mp_IACCWMap[16], mp_IACCWMap[16+1], 16);
}

void CGMEInjIRegDlg::OnAbroadMoveIdlc(CMapEditorCtrl::AbroadDir direction, int column)
{
 if (direction==CMapEditorCtrl::ABROAD_DOWN)
  m_idlr_map.SetSelection(0, column);
}

void CGMEInjIRegDlg::OnAbroadMoveIdlr(CMapEditorCtrl::AbroadDir direction, int column)
{
 if (direction==CMapEditorCtrl::ABROAD_UP)
  m_idlc_map.SetSelection(0, column);
 if (direction==CMapEditorCtrl::ABROAD_DOWN)
  m_itrpm_map.SetSelection(0, column);
}

void CGMEInjIRegDlg::OnAbroadMoveITRPM(CMapEditorCtrl::AbroadDir direction, int column)
{
 if (direction==CMapEditorCtrl::ABROAD_UP)
  m_idlr_map.SetSelection(0, column);
 if (direction==CMapEditorCtrl::ABROAD_DOWN)
  m_rigid_map.SetSelection(0, column);
}

void CGMEInjIRegDlg::OnAbroadMoveRigid(CMapEditorCtrl::AbroadDir direction, int column)
{
 if (direction==CMapEditorCtrl::ABROAD_UP)
  m_itrpm_map.SetSelection(0, column);
 if (direction==CMapEditorCtrl::ABROAD_DOWN)
  m_iacc_map.SetSelection(0, column);
}

void CGMEInjIRegDlg::OnAbroadMoveIACC(CMapEditorCtrl::AbroadDir direction, int column)
{
 if (direction==CMapEditorCtrl::ABROAD_UP)
  m_rigid_map.SetSelection(0, column);
 if (direction==CMapEditorCtrl::ABROAD_DOWN)
  m_iaccw_map.SetSelection(0, column);
}

void CGMEInjIRegDlg::OnAbroadMoveIACCW(CMapEditorCtrl::AbroadDir direction, int column)
{
 if (direction==CMapEditorCtrl::ABROAD_UP)
  m_iacc_map.SetSelection(0, column);
 if (direction==CMapEditorCtrl::ABROAD_DOWN)
  m_iacmat_map.SetSelection(0, column);
}

void CGMEInjIRegDlg::OnAbroadMoveIACMAT(CMapEditorCtrl::AbroadDir direction, int column)
{
 if (direction==CMapEditorCtrl::ABROAD_UP)
  m_iaccw_map.SetSelection(0, column);
}

void CGMEInjIRegDlg::OnSize( UINT nType, int cx, int cy )
{
 Super::OnSize(nType, cx, cy);
 if (m_initialized)
 {
  mp_cscl->Scale();
 }
}

void CGMEInjIRegDlg::OnDestroy()
{
 Super::OnDestroy();
 m_initialized = false;
}
