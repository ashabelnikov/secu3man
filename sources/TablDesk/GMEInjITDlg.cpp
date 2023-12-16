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

/** \file GMEInjITDlg.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "resource.h"
#include "GMEInjITDlg.h"
#include "common/MathHelpers.h"
#include "ui-core/fnt_helpers.h"
#include "ui-core/ToolTipCtrlEx.h"
#include "ui-core/CtrlScaler.h"

/////////////////////////////////////////////////////////////////////////////
// CGMEInjITDlg dialog

BEGIN_MESSAGE_MAP(CGMEInjITDlg, Super)
 ON_CBN_SELCHANGE(IDC_GME_IT_MODE_COMBO, OnChangeITMode)
 ON_UPDATE_COMMAND_UI(IDC_GME_INJ_IT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_GME_IT_MODE_COMBO, OnUpdateControls)
 ON_WM_SIZE()
 ON_WM_DESTROY()
END_MESSAGE_MAP()

CGMEInjITDlg::CGMEInjITDlg()
: m_it_map(16, 16, false, false, NULL, 3)
, mp_rpmGrid(NULL)
, mp_loadGrid(NULL)
, m_it_mode_val(0)
, mp_cscl(new CtrlScaler)
, m_initialized(false)
{
 for(size_t i = 0; i < 2; ++i)
  mp_ITMap[i] = NULL;

 m_it_map.setOnValueTransform(fastdelegate::MakeDelegate(this, &CGMEInjITDlg::OnValueTransform));
}

CGMEInjITDlg::~CGMEInjITDlg()
{
 //empty
}

void CGMEInjITDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);

 DDX_Control(pDX, IDC_GME_INJ_IT, m_it_map);
 DDX_Control(pDX, IDC_GME_IT_MODE_COMBO, m_it_mode);
}

/////////////////////////////////////////////////////////////////////////////
// CGMEInjITDlg message handlers

BOOL CGMEInjITDlg::OnInitDialog()
{
 Super::OnInitDialog();

 if (!m_font.GetSafeHandle())
  CloneWndFont(this, &m_font, -1, false);

 m_it_map.SetRange(.0f, 720.0f);
 m_it_map.AttachMap(mp_ITMap[1], mp_ITMap[0]);
 m_it_map.AttachLabels(mp_rpmGrid, mp_loadGrid);
 m_it_map.ShowLabels(true, true);
 m_it_map.SetDecimalPlaces(1, 0, 0);
 m_it_map.SetFont(&m_font);
 m_it_map.EnableAbroadMove(false, false);
 m_it_map.SetValueIncrement(1.0f);

 //fill IT mode combo
 m_it_mode.AddString(MLL::LoadString(IDS_GME_IT_MODE_720BTDC));
 m_it_mode.AddString(MLL::LoadString(IDS_GME_IT_MODE_720ATDC));
 m_it_mode.AddString(MLL::LoadString(IDS_GME_IT_MODE_M360360));
 m_it_mode.AddString(MLL::LoadString(IDS_GME_IT_MODE_M360360A));

 SetITMode(m_it_mode_val);

 //create a tooltip control and assign tooltips
 mp_ttc.reset(new CToolTipCtrlEx());
 VERIFY(mp_ttc->Create(this, WS_POPUP | TTS_ALWAYSTIP | TTS_BALLOON));
 VERIFY(mp_ttc->AddWindow(&m_it_mode, MLL::GetString(IDS_GME_IT_MODE_COMBO_TT)));
 mp_ttc->SetMaxTipWidth(250); //Enable text wrapping
 mp_ttc->ActivateToolTips(true);

 //initialize scaler
 mp_cscl->Init(this);
 mp_cscl->Add(&m_it_map);
 mp_cscl->Add(&m_it_mode);

 m_initialized = true;

 UpdateDialogControls(this, true);
 UpdateData(FALSE);
 return TRUE;  // return TRUE unless you set the focus to a control
}

void CGMEInjITDlg::OnUpdateControls(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(IsWindowEnabled());
}

LPCTSTR CGMEInjITDlg::GetDialogID(void) const
{
 return (LPCTSTR)IDD_GME_INJ_IT;
}

void CGMEInjITDlg::BindMaps(float* pIT)
{
 ASSERT(pIT);
 mp_ITMap[1] = pIT;
}

void CGMEInjITDlg::BindMapsOrig(float* pIT)
{
 ASSERT(pIT);
 mp_ITMap[0] = pIT;
}

void CGMEInjITDlg::BindRPMGrid(float* pGrid)
{
 mp_rpmGrid = pGrid;
 ASSERT(pGrid);
}

void CGMEInjITDlg::BindLoadGrid(const float* pGrid, bool updateLabels /*= false*/)
{
 mp_loadGrid = pGrid;
 ASSERT(pGrid);
 if (updateLabels)
 {
  m_it_map.AttachLabels(mp_rpmGrid, mp_loadGrid);
  if (m_it_map.GetSafeHwnd())
   m_it_map.UpdateDisplay(); 
 }
}

void CGMEInjITDlg::setOnChange(EventHandler OnCB)
{
 m_it_map.setOnChange(OnCB);
}

void CGMEInjITDlg::setOnChangeSettings(EventHandler OnCB)
{
 m_on_change_sett = OnCB;
}

void CGMEInjITDlg::UpdateView(bool axisLabels /*= false*/)
{
 if (axisLabels)
  m_it_map.AttachLabels(mp_rpmGrid, mp_loadGrid); //update axis labels
 m_it_map.UpdateDisplay();
}

void CGMEInjITDlg::SetArguments(int rpm, int air_flow, bool strt_use, float load)
{
 if (m_it_map.GetSafeHwnd())
 {
  m_it_map.ShowMarkers(!strt_use, true);
  m_it_map.SetArguments(load, (float)rpm);
 }
}

void CGMEInjITDlg::SetITMode(int mode)
{
 m_it_mode_val = mode;
 if (mode <= 3 && ::IsWindow(m_hWnd))
  m_it_mode.SetCurSel(mode);

 float y1, y2;
 _GetITModeRange(y1, y2);
 m_it_map.SetRange(y1, y2);
}

int CGMEInjITDlg::GetITMode(void) const
{
 return m_it_mode_val;
}

void CGMEInjITDlg::OnChangeITMode()
{
 m_it_mode_val = m_it_mode.GetCurSel();
 float y1, y2;
 _GetITModeRange(y1, y2);
 m_it_map.SetRange(y1, y2);
 _TransformValues(); //apply possible changes to map
 m_it_map.UpdateDisplay();
 if (m_on_change_sett)
  m_on_change_sett();
}

float CGMEInjITDlg::OnValueTransform(float source, int direction)
{
 return MathHelpers::InjTimValueTransform(m_it_mode_val, source, direction);
}

void CGMEInjITDlg::_GetITModeRange(float& y1, float& y2)
{
 y1 = (m_it_mode_val < 2) ? .0f : -360.0f;
 y2 = (m_it_mode_val < 2) ? 720.0f : 360.0f;
}

void CGMEInjITDlg::_TransformValues(void)
{
 for (int i = 0; i < 16*16; ++i)
  mp_ITMap[1][i] = OnValueTransform(OnValueTransform(mp_ITMap[1][i], 0), 1);
}

void CGMEInjITDlg::OnSize( UINT nType, int cx, int cy )
{
 Super::OnSize(nType, cx, cy);
 if (m_initialized)
 {
  mp_cscl->Scale();
 }
}

void CGMEInjITDlg::OnDestroy()
{
 Super::OnDestroy();
 m_initialized = false;
}
