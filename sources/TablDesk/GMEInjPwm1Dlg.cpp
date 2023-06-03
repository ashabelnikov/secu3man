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

/** \file GMEInjPwm1Dlg.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "resource.h"
#include "GMEInjPwm1Dlg.h"
#include "ui-core/fnt_helpers.h"
#include "ui-core/CtrlScaler.h"

static const float splitAngMin = -20.0f;
static const float splitAngMax =  20.0f;

/////////////////////////////////////////////////////////////////////////////
// CGMEInjPwm1Dlg dialog

BEGIN_MESSAGE_MAP(CGMEInjPwm1Dlg, Super)
 ON_UPDATE_COMMAND_UI(IDC_GME_INJ_PWM1, OnUpdateControls)
 ON_WM_SIZE()
 ON_WM_DESTROY()
END_MESSAGE_MAP()

CGMEInjPwm1Dlg::CGMEInjPwm1Dlg()
: m_pwm1_map(16, 16, false, false, NULL, 3)
, mp_pwm1Map(NULL)
, mp_rpmGrid(NULL)
, mp_loadGrid(NULL)
, m_splitAng(false)
, mp_cscl(new CtrlScaler)
, m_initialized(false)
{
 //empty
}

CGMEInjPwm1Dlg::~CGMEInjPwm1Dlg()
{
 //empty
}

void CGMEInjPwm1Dlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);

 DDX_Control(pDX, IDC_GME_INJ_PWM1, m_pwm1_map);
}

/////////////////////////////////////////////////////////////////////////////
// CGMEInjPwm1Dlg message handlers

BOOL CGMEInjPwm1Dlg::OnInitDialog()
{
 Super::OnInitDialog();

 if (!m_font.GetSafeHandle())
  CloneWndFont(this, &m_font, -1, false);

 if (m_splitAng)
  m_pwm1_map.SetRange(splitAngMin, splitAngMax); //split angle
 else
  m_pwm1_map.SetRange(0.0f, 100.0f);  //PWM duty

 m_pwm1_map.AttachMap(mp_pwm1Map);
 m_pwm1_map.AttachLabels(mp_rpmGrid, mp_loadGrid);
 m_pwm1_map.ShowLabels(true, true);
 m_pwm1_map.SetDecimalPlaces(1, 0, 0);
 m_pwm1_map.SetFont(&m_font);
 m_pwm1_map.EnableAbroadMove(false, false);
 m_pwm1_map.SetValueIncrement(0.1f);

 //initialize scaler
 mp_cscl->Init(this);
 mp_cscl->Add(&m_pwm1_map);

 m_initialized = true;

 UpdateDialogControls(this, true);
 UpdateData(FALSE);
 return TRUE;  // return TRUE unless you set the focus to a control
}

void CGMEInjPwm1Dlg::OnUpdateControls(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(IsWindowEnabled());
}

LPCTSTR CGMEInjPwm1Dlg::GetDialogID(void) const
{
 return (LPCTSTR)IDD_GME_INJ_PWM1;
}

void CGMEInjPwm1Dlg::BindMaps(float* pPWM1)
{
 ASSERT(pPWM1);
 mp_pwm1Map = pPWM1;
}

void CGMEInjPwm1Dlg::BindRPMGrid(float* pGrid)
{
 mp_rpmGrid = pGrid;
 ASSERT(pGrid);
}

void CGMEInjPwm1Dlg::BindLoadGrid(const float* pGrid, bool updateLabels /*= false*/)
{
 mp_loadGrid = pGrid;
 ASSERT(pGrid);
 if (updateLabels)
 {
  m_pwm1_map.AttachLabels(mp_rpmGrid, mp_loadGrid);
  if (m_pwm1_map.GetSafeHwnd())
   m_pwm1_map.UpdateDisplay(); 
 }
}

void CGMEInjPwm1Dlg::setOnChange(EventHandler OnCB)
{
 m_pwm1_map.setOnChange(OnCB);
}

void CGMEInjPwm1Dlg::UpdateView(bool axisLabels /*= false*/)
{
 if (axisLabels)
  m_pwm1_map.AttachLabels(mp_rpmGrid, mp_loadGrid); //update axis labels
 m_pwm1_map.UpdateDisplay();
}

void CGMEInjPwm1Dlg::SetArguments(int rpm, int air_flow, bool strt_use, float load)
{
 if (m_pwm1_map.GetSafeHwnd())
 {
  m_pwm1_map.ShowMarkers(!strt_use, true);
  m_pwm1_map.SetArguments(load, (float)rpm);
 }
}

void CGMEInjPwm1Dlg::SetSplitAngMode(bool mode)
{
 m_splitAng = mode;
 if (m_pwm1_map.GetSafeHwnd())
 {
  if (m_splitAng)
   m_pwm1_map.SetRange(splitAngMin, splitAngMax); //split angle
  else
   m_pwm1_map.SetRange(0.0f, 100.0f);  //PWM duty
 }
}

void CGMEInjPwm1Dlg::OnSize( UINT nType, int cx, int cy )
{
 Super::OnSize(nType, cx, cy);
 if (m_initialized)
 {
  mp_cscl->Scale();
 }
}

void CGMEInjPwm1Dlg::OnDestroy()
{
 Super::OnDestroy();
 m_initialized = false;
}
