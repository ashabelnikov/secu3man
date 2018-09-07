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

/** \file GMEInjOtherDlg.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "resource.h"
#include "GMEInjOtherDlg.h"
#include "MapIds.h"
#include "common/MathHelpers.h"

const UINT CGMEInjOtherDlg::IDD = IDD_GME_INJ_OTHER;

/////////////////////////////////////////////////////////////////////////////
// CGMEInjOtherDlg dialog

BEGIN_MESSAGE_MAP(CGMEInjOtherDlg, Super)
 ON_UPDATE_COMMAND_UI(IDC_GME_INJ_CRNK, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_GME_INJ_DEAD0, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_GME_INJ_DEAD1, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_GME_INJ_EGOCRV, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_GME_INJ_IATCLT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_GME_INJ_TPSSWT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_GME_INJ_ATSC, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_GME_INJ_GTSC, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_GME_INJ_GPSC, OnUpdateControls)
END_MESSAGE_MAP()

CGMEInjOtherDlg::CGMEInjOtherDlg(CWnd* pParent /*=NULL*/)
: Super(CGMEInjOtherDlg::IDD, pParent)
, m_crnk_map(1, 16)
, m_dead0_map(1, 16)  //first part
, m_dead1_map(1, 16)  //second part
, m_egocrv_map(1, 16)
, m_iatclt_map(1, 8)
, m_tpsswt_map(1, 16)
, m_atsc_map(1, 16)
, m_gtsc_map(1, 16)
, m_gpsc_map(1, 9)
, mp_CrnkMap(NULL)
, mp_DeadMap(NULL)
, mp_EGOCrvMap(NULL)
, mp_IATCLTMap(NULL)
, mp_TpsswtMap(NULL)
, mp_AtscMap(NULL)
, mp_GtscMap(NULL)
, mp_GpscMap(NULL)
, mp_temperGrid(NULL)
, mp_deadGrid(NULL)
, mp_rpmGrid(NULL)
{
 m_crnk_map.SetDecimalPlaces(1, 0, 0);
 m_dead0_map.SetDecimalPlaces(2, 1, 0);
 m_dead1_map.SetDecimalPlaces(2, 1, 0);
 m_egocrv_map.SetDecimalPlaces(2, 2, 0);
 m_iatclt_map.SetDecimalPlaces(3, 0, 0);
 m_tpsswt_map.SetDecimalPlaces(1, 0, 0);
 m_atsc_map.SetDecimalPlaces(2, 0, 0);
 m_gtsc_map.SetDecimalPlaces(2, 0, 0);
 m_gpsc_map.SetDecimalPlaces(2, 0, 0);
}

CGMEInjOtherDlg::~CGMEInjOtherDlg()
{
 //empty
}

void CGMEInjOtherDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);

 DDX_Control(pDX, IDC_GME_INJ_CRNK, m_crnk_map);
 DDX_Control(pDX, IDC_GME_INJ_DEAD0, m_dead0_map);
 DDX_Control(pDX, IDC_GME_INJ_DEAD1, m_dead1_map);
 DDX_Control(pDX, IDC_GME_INJ_EGOCRV, m_egocrv_map);
 DDX_Control(pDX, IDC_GME_INJ_IATCLT, m_iatclt_map);
 DDX_Control(pDX, IDC_GME_INJ_TPSSWT, m_tpsswt_map);
 DDX_Control(pDX, IDC_GME_INJ_ATSC, m_atsc_map);
 DDX_Control(pDX, IDC_GME_INJ_GTSC, m_gtsc_map);
 DDX_Control(pDX, IDC_GME_INJ_GPSC, m_gpsc_map);
}

/////////////////////////////////////////////////////////////////////////////
// CGMEInjOtherDlg message handlers

BOOL CGMEInjOtherDlg::OnInitDialog()
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

 _UpdateDynamicGrids();
 
 m_crnk_map.setOnChange(fastdelegate::MakeDelegate(this, CGMEInjOtherDlg::OnChangeCrnk));
 m_crnk_map.setOnAbroadMove(fastdelegate::MakeDelegate(this, CGMEInjOtherDlg::OnAbroadMoveCrnk));
 m_crnk_map.SetRange(0.3f, 26.0f);
 m_crnk_map.AttachMap(mp_CrnkMap);
 m_crnk_map.AttachLabels(mp_temperGrid, NULL);
 m_crnk_map.ShowLabels(true, false);
 m_crnk_map.SetFont(&m_font);
 m_crnk_map.EnableAbroadMove(false, true);
 m_crnk_map.SetValueIncrement(0.1f);

 m_dead0_map.setOnChange(fastdelegate::MakeDelegate(this, CGMEInjOtherDlg::OnChangeDead));
 m_dead0_map.setOnAbroadMove(fastdelegate::MakeDelegate(this, CGMEInjOtherDlg::OnAbroadMoveDead0));
 m_dead0_map.SetRange(0.2f, 16.0f);
 m_dead0_map.AttachMap(mp_DeadMap);
 m_dead0_map.AttachLabels(mp_deadGrid, NULL);
 m_dead0_map.ShowLabels(true, false);
 m_dead0_map.SetFont(&m_font);
 m_dead0_map.EnableAbroadMove(true, true);
 m_dead0_map.SetValueIncrement(0.01f);

 m_dead1_map.setOnChange(fastdelegate::MakeDelegate(this, CGMEInjOtherDlg::OnChangeDead));
 m_dead1_map.setOnAbroadMove(fastdelegate::MakeDelegate(this, CGMEInjOtherDlg::OnAbroadMoveDead1));
 m_dead1_map.SetRange(0.2f, 16.0f);
 m_dead1_map.AttachMap(mp_DeadMap + 16);
 m_dead1_map.AttachLabels(mp_deadGrid + 16, NULL);
 m_dead1_map.ShowLabels(true, false);
 m_dead1_map.SetFont(&m_font);
 m_dead1_map.EnableAbroadMove(true, true);
 m_dead1_map.SetValueIncrement(0.01f);

 m_egocrv_map.setOnChange(fastdelegate::MakeDelegate(this, CGMEInjOtherDlg::OnChangeEGOCrv));
 m_egocrv_map.setOnAbroadMove(fastdelegate::MakeDelegate(this, CGMEInjOtherDlg::OnAbroadMoveEGOCrv));
 m_egocrv_map.SetRange(6.0f, 24.0f);
 m_egocrv_map.AttachMap(mp_EGOCrvMap);
 m_egocrv_map.AttachLabels(&m_egocrvGrid[0], NULL);
 m_egocrv_map.ShowLabels(true, false);
 m_egocrv_map.SetFont(&m_font);
 m_egocrv_map.EnableAbroadMove(true, true);
 m_egocrv_map.SetValueIncrement(0.01f);

 m_iatclt_map.setOnChange(fastdelegate::MakeDelegate(this, CGMEInjOtherDlg::OnChangeIATCLT));
 m_iatclt_map.setOnAbroadMove(fastdelegate::MakeDelegate(this, CGMEInjOtherDlg::OnAbroadMoveIATCLT));
 m_iatclt_map.SetRange(.0f, 1.0f);
 m_iatclt_map.AttachMap(mp_IATCLTMap);
 m_iatclt_map.AttachLabels(&m_iatcltGrid[0], NULL);
 m_iatclt_map.ShowLabels(true, false);
 m_iatclt_map.SetFont(&m_font);
 m_iatclt_map.EnableAbroadMove(true, true);
 m_iatclt_map.SetValueIncrement(0.001f);

 m_tpsswt_map.setOnChange(fastdelegate::MakeDelegate(this, CGMEInjOtherDlg::OnChangeTpsswt));
 m_tpsswt_map.setOnAbroadMove(fastdelegate::MakeDelegate(this, CGMEInjOtherDlg::OnAbroadMoveTpsswt));
 m_tpsswt_map.SetRange(.0f, 100.0f);
 m_tpsswt_map.AttachMap(mp_TpsswtMap);
 m_tpsswt_map.AttachLabels(mp_rpmGrid, NULL);
 m_tpsswt_map.ShowLabels(true, false);
 m_tpsswt_map.SetFont(&m_font);
 m_tpsswt_map.EnableAbroadMove(true, true);
 m_tpsswt_map.SetValueIncrement(0.1f);

 m_atsc_map.setOnChange(fastdelegate::MakeDelegate(this, CGMEInjOtherDlg::OnChangeAtsc));
 m_atsc_map.setOnAbroadMove(fastdelegate::MakeDelegate(this, CGMEInjOtherDlg::OnAbroadMoveAtsc));
 m_atsc_map.SetRange(0.5f, 1.5f);
 m_atsc_map.AttachMap(mp_AtscMap);
 m_atsc_map.AttachLabels(mp_temperGrid, NULL);
 m_atsc_map.ShowLabels(true, false);
 m_atsc_map.SetFont(&m_font);
 m_atsc_map.EnableAbroadMove(true, true);
 m_atsc_map.SetValueIncrement(0.01f);

 m_gtsc_map.setOnChange(fastdelegate::MakeDelegate(this, CGMEInjOtherDlg::OnChangeGtsc));
 m_gtsc_map.setOnAbroadMove(fastdelegate::MakeDelegate(this, CGMEInjOtherDlg::OnAbroadMoveGtsc));
 m_gtsc_map.SetRange(0.01f, 1.99f);
 m_gtsc_map.AttachMap(mp_GtscMap);
 m_gtsc_map.AttachLabels(mp_temperGrid, NULL);
 m_gtsc_map.ShowLabels(true, false);
 m_gtsc_map.SetFont(&m_font);
 m_gtsc_map.EnableAbroadMove(true, true);
 m_gtsc_map.SetValueIncrement(0.01f);

 m_gpsc_map.setOnChange(fastdelegate::MakeDelegate(this, CGMEInjOtherDlg::OnChangeGpsc));
 m_gpsc_map.setOnAbroadMove(fastdelegate::MakeDelegate(this, CGMEInjOtherDlg::OnAbroadMoveGpsc));
 m_gpsc_map.SetRange(0.01f, 1.99f);
 m_gpsc_map.AttachMap(mp_GpscMap);
 m_gpsc_map.AttachLabels(&m_gpscGrid[0], NULL);
 m_gpsc_map.ShowLabels(true, false);
 m_gpsc_map.SetFont(&m_font);
 m_gpsc_map.EnableAbroadMove(true, false);
 m_gpsc_map.SetValueIncrement(0.01f);

 UpdateDialogControls(this, true);
 UpdateData(FALSE);
 return TRUE;  // return TRUE unless you set the focus to a control
}

void CGMEInjOtherDlg::OnUpdateControls(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(IsWindowEnabled());
}

LPCTSTR CGMEInjOtherDlg::GetDialogID(void) const
{
 return (LPCTSTR)IDD;
}

void CGMEInjOtherDlg::BindMaps(float* pCrnk, float* pDead, float* pEGOCrv, float* pIATCLT, float* pTpsswt, float* pAtsc, float* pGtsc, float* pGpsc)
{
 ASSERT(pCrnk);
 mp_CrnkMap = pCrnk;

 ASSERT(pDead);
 mp_DeadMap = pDead;

 ASSERT(pEGOCrv);
 mp_EGOCrvMap = pEGOCrv;

 ASSERT(pIATCLT);
 mp_IATCLTMap = pIATCLT;

 ASSERT(pTpsswt);
 mp_TpsswtMap = pTpsswt;

 ASSERT(pAtsc);
 mp_AtscMap = pAtsc;

 ASSERT(pGtsc);
 mp_GtscMap = pGtsc;

 ASSERT(pGpsc);
 mp_GpscMap = pGpsc;

 _UpdateDynamicGrids();
}

void CGMEInjOtherDlg::BindTemperGrid(float* pGrid)
{
 mp_temperGrid = pGrid;
 ASSERT(pGrid);
}

void CGMEInjOtherDlg::BindDeadGrid(float* pGrid)
{
 mp_deadGrid = pGrid;
 ASSERT(pGrid);
}

void CGMEInjOtherDlg::BindRPMGrid(float* pGrid)
{
 mp_rpmGrid = pGrid;
 ASSERT(pGrid);
}

void CGMEInjOtherDlg::setOnChange(EventWithCode OnCB)
{
 m_OnChange = OnCB;
}

void CGMEInjOtherDlg::UpdateView(void)
{
 _UpdateDynamicGrids();
 m_egocrv_map.AttachLabels(&m_egocrvGrid[0], NULL);
 m_iatclt_map.AttachLabels(&m_iatcltGrid[0], NULL);
 m_gpsc_map.AttachLabels(&m_gpscGrid[0], NULL);

 m_crnk_map.UpdateDisplay();
 m_dead0_map.UpdateDisplay();
 m_dead1_map.UpdateDisplay();
 m_egocrv_map.UpdateDisplay();
 m_iatclt_map.UpdateDisplay();
 m_tpsswt_map.UpdateDisplay();
 m_atsc_map.UpdateDisplay();
 m_gtsc_map.UpdateDisplay();
 m_gpsc_map.UpdateDisplay();
}


void CGMEInjOtherDlg::SetArguments(bool strt_use, float clt, float voltage, float add_i1, int rpm, float tmp2, float iat)
{
 if (m_crnk_map.GetSafeHwnd())
 {
  m_crnk_map.ShowMarkers(strt_use, false);
  m_crnk_map.SetArguments(0, clt);
 }

 if (m_dead0_map.GetSafeHwnd())
 {
  m_dead0_map.ShowMarkers(voltage < mp_deadGrid[16]-(mp_deadGrid[1]-mp_deadGrid[0]) / 5, false);
  m_dead0_map.SetArguments(0, voltage);
 }
 if (m_dead1_map.GetSafeHwnd())
 {
  m_dead1_map.ShowMarkers(voltage > mp_deadGrid[15]+(mp_deadGrid[1]-mp_deadGrid[0]) / 5, false);
  m_dead1_map.SetArguments(0, voltage);
 }

 if (m_egocrv_map.GetSafeHwnd())
 {
  m_egocrv_map.ShowMarkers(true, false);
  m_egocrv_map.SetArguments(0, add_i1);
 }
 /*
 if (m_iatclt_map.GetSafeHwnd())
 {
  m_iatclt_map.ShowMarkers(!strt_use, false);
  m_iatclt_map.SetArguments(0, airflow);
 }*/

 if (m_tpsswt_map.GetSafeHwnd())
 {
  m_tpsswt_map.ShowMarkers(!strt_use, false);
  m_tpsswt_map.SetArguments(0, (float)rpm);
 }

 if (m_atsc_map.GetSafeHwnd())
 {
  m_atsc_map.ShowMarkers(!strt_use, false);
  m_atsc_map.SetArguments(0, iat);
 }

 if (m_gtsc_map.GetSafeHwnd())
 {
  m_gtsc_map.ShowMarkers(true, false);
  m_gtsc_map.SetArguments(0, tmp2);
 }
/*
 if (m_gpsc_map.GetSafeHwnd())
 {
  m_gpsc_map.ShowMarkers(!strt_use, false);
  m_gpsc_map.SetArguments(0, map2);
 }*/
}

void CGMEInjOtherDlg::OnChangeCrnk(void)
{
 if (m_OnChange)
  m_OnChange(TYPE_MAP_INJ_CRNK);
}

void CGMEInjOtherDlg::OnChangeDead(void)
{
 if (m_OnChange)
  m_OnChange(TYPE_MAP_INJ_DEAD);
}

void CGMEInjOtherDlg::OnChangeEGOCrv(void)
{
 if (m_OnChange)
  m_OnChange(TYPE_MAP_INJ_EGOCRV);
}

void CGMEInjOtherDlg::OnChangeIATCLT(void)
{
 if (m_OnChange)
  m_OnChange(TYPE_MAP_INJ_IATCLT);
}

void CGMEInjOtherDlg::OnChangeTpsswt(void)
{
 if (m_OnChange)
  m_OnChange(TYPE_MAP_INJ_TPSSWT);
}

void CGMEInjOtherDlg::OnChangeAtsc(void)
{
 if (m_OnChange)
  m_OnChange(TYPE_MAP_INJ_ATSC);
}

void CGMEInjOtherDlg::OnChangeGtsc(void)
{
 if (m_OnChange)
  m_OnChange(TYPE_MAP_INJ_GTSC);
}

void CGMEInjOtherDlg::OnChangeGpsc(void)
{
 if (m_OnChange)
  m_OnChange(TYPE_MAP_INJ_GPSC);
}

void CGMEInjOtherDlg::_UpdateDynamicGrids(void)
{
 m_egocrvGrid = MathHelpers::BuildGridFromRange(mp_EGOCrvMap[16], mp_EGOCrvMap[16+1], 16);
 m_iatcltGrid = MathHelpers::BuildGridFromRange(mp_IATCLTMap[8], mp_IATCLTMap[8+1], 8);
 m_gpscGrid = MathHelpers::BuildGridFromRange(mp_GpscMap[9], mp_GpscMap[9+1], 9);
}

void CGMEInjOtherDlg::OnAbroadMoveCrnk(CMapEditorCtrl::AbroadDir direction, int column)
{
 if (direction==CMapEditorCtrl::ABROAD_DOWN)
  m_dead0_map.SetSelection(0, column);
}

void CGMEInjOtherDlg::OnAbroadMoveDead0(CMapEditorCtrl::AbroadDir direction, int column)
{
 if (direction==CMapEditorCtrl::ABROAD_UP)
  m_crnk_map.SetSelection(0, column);
 if (direction==CMapEditorCtrl::ABROAD_DOWN)
  m_dead1_map.SetSelection(0, column);
}

void CGMEInjOtherDlg::OnAbroadMoveDead1(CMapEditorCtrl::AbroadDir direction, int column)
{
 if (direction==CMapEditorCtrl::ABROAD_UP)
  m_dead0_map.SetSelection(0, column);
 if (direction==CMapEditorCtrl::ABROAD_DOWN)
  m_egocrv_map.SetSelection(0, column);
}

void CGMEInjOtherDlg::OnAbroadMoveEGOCrv(CMapEditorCtrl::AbroadDir direction, int column)
{
 if (direction==CMapEditorCtrl::ABROAD_UP)
  m_dead1_map.SetSelection(0, column);
 if (direction==CMapEditorCtrl::ABROAD_DOWN)
  m_iatclt_map.SetSelection(0, column);
}

void CGMEInjOtherDlg::OnAbroadMoveIATCLT(CMapEditorCtrl::AbroadDir direction, int column)
{
 if (direction==CMapEditorCtrl::ABROAD_UP)
  m_egocrv_map.SetSelection(0, column);
 if (direction==CMapEditorCtrl::ABROAD_DOWN)
  m_tpsswt_map.SetSelection(0, column);
}

void CGMEInjOtherDlg::OnAbroadMoveTpsswt(CMapEditorCtrl::AbroadDir direction, int column)
{
 if (direction==CMapEditorCtrl::ABROAD_UP)
  m_iatclt_map.SetSelection(0, column);
 if (direction==CMapEditorCtrl::ABROAD_DOWN)
  m_atsc_map.SetSelection(0, column);
}

void CGMEInjOtherDlg::OnAbroadMoveAtsc(CMapEditorCtrl::AbroadDir direction, int column)
{
 if (direction==CMapEditorCtrl::ABROAD_UP)
  m_tpsswt_map.SetSelection(0, column);
 if (direction==CMapEditorCtrl::ABROAD_DOWN)
  m_gtsc_map.SetSelection(0, column);
}

void CGMEInjOtherDlg::OnAbroadMoveGtsc(CMapEditorCtrl::AbroadDir direction, int column)
{
 if (direction==CMapEditorCtrl::ABROAD_UP)
  m_atsc_map.SetSelection(0, column);
 if (direction==CMapEditorCtrl::ABROAD_DOWN)
  m_gpsc_map.SetSelection(0, column);
}

void CGMEInjOtherDlg::OnAbroadMoveGpsc(CMapEditorCtrl::AbroadDir direction, int column)
{
 if (direction==CMapEditorCtrl::ABROAD_UP)
  m_gtsc_map.SetSelection(0, column);
}
