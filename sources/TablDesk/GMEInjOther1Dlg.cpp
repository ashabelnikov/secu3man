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

/** \file GMEInjOther1Dlg.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "resource.h"
#include "GMEInjOther1Dlg.h"
#include "MapIds.h"
#include "common/MathHelpers.h"
#include "ui-core/fnt_helpers.h"
#include "ui-core/CtrlScaler.h"

const UINT CGMEInjOther1Dlg::IDD = IDD_GME_INJ_OTHER1;

static const UINT StaticCtrlBegin = IDC_GME_INJ_TPSSWT_TEXT;
static const UINT StaticCtrlEnd = IDC_GME_INJ_CYLADD_TEXT;

static const float inj_cyladd_slots[8] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f};

/////////////////////////////////////////////////////////////////////////////
// CGMEInjOther1Dlg dialog

BEGIN_MESSAGE_MAP(CGMEInjOther1Dlg, Super)
 ON_UPDATE_COMMAND_UI(IDC_GME_INJ_TPSSWT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_GME_INJ_TPSZON, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_GME_INJ_CYLMULT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_GME_INJ_CYLADD, OnUpdateControls)
 ON_WM_SIZE()
 ON_WM_DESTROY()
END_MESSAGE_MAP()

CGMEInjOther1Dlg::CGMEInjOther1Dlg(CWnd* pParent /*=NULL*/)
: Super(CGMEInjOther1Dlg::IDD, pParent)
, m_tpsswt_map(1, 16)
, m_tpszon_map(1, 16)
, m_cylmult_map(1, 8)
, m_cyladd_map(1, 8)
, mp_TpsswtMap(NULL)
, mp_TpszonMap(NULL)
, mp_CylMultMap(NULL)
, mp_CylAddMap(NULL)
, mp_rpmGrid(NULL)
, mp_cscl(new CtrlScaler)
, m_initialized(false)
{
 m_tpsswt_map.SetDecimalPlaces(1, 0, 0);
 m_tpszon_map.SetDecimalPlaces(1, 0, 0);
 m_cylmult_map.SetDecimalPlaces(3, 0, 0);
 m_cyladd_map.SetDecimalPlaces(2, 0, 0);
}

CGMEInjOther1Dlg::~CGMEInjOther1Dlg()
{
 //empty
}

void CGMEInjOther1Dlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);

 DDX_Control(pDX, IDC_GME_INJ_TPSSWT, m_tpsswt_map);
 DDX_Control(pDX, IDC_GME_INJ_TPSZON, m_tpszon_map);
 DDX_Control(pDX, IDC_GME_INJ_CYLMULT, m_cylmult_map);
 DDX_Control(pDX, IDC_GME_INJ_CYLADD, m_cyladd_map);

 for(int i = StaticCtrlBegin; i <= StaticCtrlEnd; ++i)
  DDX_Control(pDX, i, m_ctrls[i-StaticCtrlBegin]);
}

/////////////////////////////////////////////////////////////////////////////
// CGMEInjOther1Dlg message handlers

BOOL CGMEInjOther1Dlg::OnInitDialog()
{
 Super::OnInitDialog();

 if (!m_font.GetSafeHandle())
  CloneWndFont(this, &m_font, -1, false);
 
 m_tpsswt_map.setOnChange(fastdelegate::MakeDelegate(this, CGMEInjOther1Dlg::OnChangeTpsswt));
 m_tpsswt_map.setOnAbroadMove(fastdelegate::MakeDelegate(this, CGMEInjOther1Dlg::OnAbroadMoveTpsswt));
 m_tpsswt_map.SetRange(.0f, 100.0f);
 m_tpsswt_map.AttachMap(mp_TpsswtMap);
 m_tpsswt_map.AttachLabels(mp_rpmGrid, NULL);
 m_tpsswt_map.ShowLabels(true, false);
 m_tpsswt_map.SetFont(&m_font);
 m_tpsswt_map.EnableAbroadMove(false, true);
 m_tpsswt_map.SetValueIncrement(0.1f);

 m_tpszon_map.setOnChange(fastdelegate::MakeDelegate(this, CGMEInjOther1Dlg::OnChangeTpszon));
 m_tpszon_map.setOnAbroadMove(fastdelegate::MakeDelegate(this, CGMEInjOther1Dlg::OnAbroadMoveTpszon));
 m_tpszon_map.SetRange(.0f, 100.0f);
 m_tpszon_map.AttachMap(mp_TpszonMap);
 m_tpszon_map.AttachLabels(mp_rpmGrid, NULL);
 m_tpszon_map.ShowLabels(true, false);
 m_tpszon_map.SetFont(&m_font);
 m_tpszon_map.EnableAbroadMove(true, true);
 m_tpszon_map.SetValueIncrement(0.1f);

 m_cylmult_map.setOnChange(fastdelegate::MakeDelegate(this, CGMEInjOther1Dlg::OnChangeCylMult));
 m_cylmult_map.setOnAbroadMove(fastdelegate::MakeDelegate(this, CGMEInjOther1Dlg::OnAbroadMoveCylMult));
 m_cylmult_map.SetRange(.5f, 1.49f);
 m_cylmult_map.AttachMap(mp_CylMultMap);
 m_cylmult_map.AttachLabels(inj_cyladd_slots, NULL);
 m_cylmult_map.ShowLabels(true, false);
 m_cylmult_map.SetFont(&m_font);
 m_cylmult_map.EnableAbroadMove(true, true);
 m_cylmult_map.SetValueIncrement(0.01f);

 m_cyladd_map.setOnChange(fastdelegate::MakeDelegate(this, CGMEInjOther1Dlg::OnChangeCylAdd));
 m_cyladd_map.setOnAbroadMove(fastdelegate::MakeDelegate(this, CGMEInjOther1Dlg::OnAbroadMoveCylAdd));
 m_cyladd_map.SetRange(-3.2f, 3.2f);
 m_cyladd_map.AttachMap(mp_CylAddMap);
 m_cyladd_map.AttachLabels(inj_cyladd_slots, NULL);
 m_cyladd_map.ShowLabels(true, false);
 m_cyladd_map.SetFont(&m_font);
 m_cyladd_map.EnableAbroadMove(true, false);
 m_cyladd_map.SetValueIncrement(0.01f);

 //initialize scaler
 mp_cscl->Init(this);
 mp_cscl->Add(&m_tpsswt_map);
 mp_cscl->Add(&m_tpszon_map);
 mp_cscl->Add(&m_cylmult_map);
 mp_cscl->Add(&m_cyladd_map);
 mp_cscl->Add(m_ctrls, (StaticCtrlEnd - StaticCtrlBegin) + 1);

 m_initialized = true;

 UpdateDialogControls(this, true);
 UpdateData(FALSE);
 return TRUE;  // return TRUE unless you set the focus to a control
}

void CGMEInjOther1Dlg::OnUpdateControls(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(IsWindowEnabled());
}

LPCTSTR CGMEInjOther1Dlg::GetDialogID(void) const
{
 return (LPCTSTR)IDD;
}

void CGMEInjOther1Dlg::BindMaps(float* pTpsswt, float* pTpszon, float* pCylMult, float* pCylAdd)
{
 ASSERT(pTpsswt);
 mp_TpsswtMap = pTpsswt;

 ASSERT(pTpszon);
 mp_TpszonMap = pTpszon;

 ASSERT(pCylMult);
 mp_CylMultMap = pCylMult;

 ASSERT(pCylAdd);
 mp_CylAddMap = pCylAdd;
}

void CGMEInjOther1Dlg::BindRPMGrid(float* pGrid)
{
 mp_rpmGrid = pGrid;
 ASSERT(pGrid);
}

void CGMEInjOther1Dlg::setOnChange(EventWithCode OnCB)
{
 m_OnChange = OnCB;
}

void CGMEInjOther1Dlg::UpdateView(bool axisLabels /*= false*/)
{
 m_tpsswt_map.UpdateDisplay();
 m_tpszon_map.UpdateDisplay();
 m_cylmult_map.UpdateDisplay();
 m_cyladd_map.UpdateDisplay();
}

void CGMEInjOther1Dlg::SetArguments(bool strt_use, int rpm)
{
 if (m_tpsswt_map.GetSafeHwnd())
 {
  m_tpsswt_map.ShowMarkers(!strt_use, true);
  m_tpsswt_map.SetArguments(0, (float)rpm);
 }

 if (m_tpszon_map.GetSafeHwnd())
 {
  m_tpszon_map.ShowMarkers(!strt_use, true);
  m_tpszon_map.SetArguments(0, (float)rpm);
 }
}

void CGMEInjOther1Dlg::OnChangeTpsswt(void)
{
 if (m_OnChange)
  m_OnChange(TYPE_MAP_INJ_TPSSWT);
}

void CGMEInjOther1Dlg::OnChangeTpszon(void)
{
 if (m_OnChange)
  m_OnChange(TYPE_MAP_INJ_TPSZON);
}


void CGMEInjOther1Dlg::OnChangeCylMult(void)
{
 if (m_OnChange)
  m_OnChange(TYPE_MAP_INJ_CYLMULT);
}

void CGMEInjOther1Dlg::OnChangeCylAdd(void)
{
 if (m_OnChange)
  m_OnChange(TYPE_MAP_INJ_CYLADD);
}

void CGMEInjOther1Dlg::OnAbroadMoveTpsswt(CMapEditorCtrl::AbroadDir direction, int column)
{
 if (direction==CMapEditorCtrl::ABROAD_DOWN)
  m_tpszon_map.SetSelection(0, column);
}

void CGMEInjOther1Dlg::OnAbroadMoveTpszon(CMapEditorCtrl::AbroadDir direction, int column)
{
 if (direction==CMapEditorCtrl::ABROAD_UP)
  m_tpsswt_map.SetSelection(0, column);
 if (direction==CMapEditorCtrl::ABROAD_DOWN)
  m_cylmult_map.SetSelection(0, column);
}

void CGMEInjOther1Dlg::OnAbroadMoveCylMult(CMapEditorCtrl::AbroadDir direction, int column)
{
 if (direction==CMapEditorCtrl::ABROAD_UP)
  m_tpsswt_map.SetSelection(0, column);
 if (direction==CMapEditorCtrl::ABROAD_DOWN)
  m_cyladd_map.SetSelection(0, column);
}

void CGMEInjOther1Dlg::OnAbroadMoveCylAdd(CMapEditorCtrl::AbroadDir direction, int column)
{
 if (direction==CMapEditorCtrl::ABROAD_UP)
  m_cylmult_map.SetSelection(0, column);
}

void CGMEInjOther1Dlg::OnSize(UINT nType, int cx, int cy)
{
 Super::OnSize(nType, cx, cy);
 if (m_initialized)
 {
  mp_cscl->Scale();
 }
}

void CGMEInjOther1Dlg::OnDestroy()
{
 Super::OnDestroy();
 m_initialized = false;
}
