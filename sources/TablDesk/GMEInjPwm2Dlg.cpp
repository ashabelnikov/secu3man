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

/** \file GMEInjPwm2Dlg.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "resource.h"
#include "GMEInjPwm2Dlg.h"
#include "ui-core/fnt_helpers.h"
#include "ui-core/CtrlScaler.h"

/////////////////////////////////////////////////////////////////////////////
// CGMEInjPwm2Dlg dialog

BEGIN_MESSAGE_MAP(CGMEInjPwm2Dlg, Super)
 ON_UPDATE_COMMAND_UI(IDC_GME_INJ_PWM2, OnUpdateControls)
 ON_WM_SIZE()
 ON_WM_DESTROY()
END_MESSAGE_MAP()

CGMEInjPwm2Dlg::CGMEInjPwm2Dlg()
: m_pwm2_map(16, 16, false, false, NULL, 3)
, mp_rpmGrid(NULL)
, mp_loadGrid(NULL)
, mp_cscl(new CtrlScaler)
, m_initialized(false)
{
 for(size_t i = 0; i < 2; ++i)
  mp_pwm2Map[i] = NULL;
}

CGMEInjPwm2Dlg::~CGMEInjPwm2Dlg()
{
 //empty
}

void CGMEInjPwm2Dlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);

 DDX_Control(pDX, IDC_GME_INJ_PWM2, m_pwm2_map);
}

/////////////////////////////////////////////////////////////////////////////
// CGMEInjPwm2Dlg message handlers

BOOL CGMEInjPwm2Dlg::OnInitDialog()
{
 Super::OnInitDialog();

 if (!m_font.GetSafeHandle())
  CloneWndFont(this, &m_font, -1, false);

 m_pwm2_map.SetRange(0.0f, 100.0f);
 m_pwm2_map.AttachMap(mp_pwm2Map[1], mp_pwm2Map[0]);
 m_pwm2_map.AttachLabels(mp_rpmGrid, mp_loadGrid);
 m_pwm2_map.ShowLabels(true, true);
 m_pwm2_map.SetDecimalPlaces(1, 0, 0);
 m_pwm2_map.SetFont(&m_font);
 m_pwm2_map.EnableAbroadMove(false, false);
 m_pwm2_map.SetValueIncrement(0.1f);

 //initialize scaler
 mp_cscl->Init(this);
 mp_cscl->Add(&m_pwm2_map);

 m_initialized = true;

 UpdateDialogControls(this, true);
 UpdateData(FALSE);
 return TRUE;  // return TRUE unless you set the focus to a control
}

void CGMEInjPwm2Dlg::OnUpdateControls(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(IsWindowEnabled());
}

LPCTSTR CGMEInjPwm2Dlg::GetDialogID(void) const
{
 return (LPCTSTR)IDD_GME_INJ_PWM2;
}

void CGMEInjPwm2Dlg::BindMaps(float* pPWM2)
{
 ASSERT(pPWM2);
 mp_pwm2Map[1] = pPWM2;
}

void CGMEInjPwm2Dlg::BindMapsOrig(float* pPWM2)
{
 ASSERT(pPWM2);
 mp_pwm2Map[0] = pPWM2;
}

void CGMEInjPwm2Dlg::BindRPMGrid(float* pGrid)
{
 mp_rpmGrid = pGrid;
 ASSERT(pGrid);
}

void CGMEInjPwm2Dlg::BindLoadGrid(const float* pGrid, bool updateLabels /*= false*/)
{
 mp_loadGrid = pGrid;
 ASSERT(pGrid);
 if (updateLabels)
 {
  m_pwm2_map.AttachLabels(mp_rpmGrid, mp_loadGrid);
  if (m_pwm2_map.GetSafeHwnd())
   m_pwm2_map.UpdateDisplay(); 
 }
}

void CGMEInjPwm2Dlg::setOnChange(EventHandler OnCB)
{
 m_pwm2_map.setOnChange(OnCB);
}

void CGMEInjPwm2Dlg::UpdateView(bool axisLabels /*= false*/)
{
 if (axisLabels)
  m_pwm2_map.AttachLabels(mp_rpmGrid, mp_loadGrid); //update axis labels
 m_pwm2_map.UpdateDisplay();
}

void CGMEInjPwm2Dlg::SetArguments(int rpm, int air_flow, bool strt_use, float load)
{
 if (m_pwm2_map.GetSafeHwnd())
 {
  m_pwm2_map.ShowMarkers(!strt_use, true);
  m_pwm2_map.SetArguments(load, (float)rpm);
 }
}

void CGMEInjPwm2Dlg::OnSize( UINT nType, int cx, int cy )
{
 Super::OnSize(nType, cx, cy);
 if (m_initialized)
 {
  mp_cscl->Scale();
 }
}

void CGMEInjPwm2Dlg::OnDestroy()
{
 Super::OnDestroy();
 m_initialized = false;
}
