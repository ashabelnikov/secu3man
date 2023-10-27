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

/** \file TablesPanelBase.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "TablesPanelBase.h"
#include "common/GDIHelpers.h"
#include "DLLLinkedFunctions.h"
#include "MapIds.h"
#include "ui-core/ToolTipCtrlEx.h"
#include "ui-core/WndScroller.h"

/////////////////////////////////////////////////////////////////////////////
// CTablesPanelBase class

CTablesPanelBase::CTablesPanelBase()
: mp_scr(new CWndScroller)
, m_children_charts(false)
, m_toggleMapWnd(false)
, m_openedChart(NULL)
, m_fuel_injection(false)
, m_gasdose(false)
, m_charts_enabled(-1)
{
 memset(m_rpm_grid_values, 0, 16 * sizeof(float));
 memset(m_clt_grid_values, 0, 16 * sizeof(float));
 memset(m_load_grid_values, 0, 16 * sizeof(float));
}

CTablesPanelBase::~CTablesPanelBase()
{
}

bool CTablesPanelBase::IsAllowed(void)
{
 if (m_IsAllowed)
  return m_IsAllowed();
 return false;
}

void CTablesPanelBase::MakeChartsChildren(bool children)
{
 m_children_charts = children;
}

void CTablesPanelBase::setIsAllowed(EventResult IsFunction)
{m_IsAllowed = IsFunction;}

void CTablesPanelBase::setOnMapChanged(EventWithCode OnFunction)
{m_OnMapChanged = OnFunction;}

void CTablesPanelBase::setOnCloseMapWnd(EventWithHWND OnFunction)
{ m_OnCloseMapWnd = OnFunction; }

void CTablesPanelBase::setOnOpenMapWnd(EventWithHWND OnFunction)
{ m_OnOpenMapWnd = OnFunction; }

void CTablesPanelBase::setOnWndActivation(EventWithHWNDLong OnFunction)
{ m_OnWndActivation = OnFunction; }

void CTablesPanelBase::setOnChangeSettings(EventHandler OnCB)
{
 m_OnChangeSettings = OnCB;
}

float* CTablesPanelBase::GetRPMGrid(void)
{
 return m_rpm_grid_values;
}

float* CTablesPanelBase::GetCLTGrid(void)
{
 return m_clt_grid_values;
}

float* CTablesPanelBase::GetLoadGrid(void)
{
 return m_load_grid_values;
}

HWND CTablesPanelBase::_ChartParentHwnd(void)
{
 return m_children_charts ? AfxGetMainWnd()->GetSafeHwnd() : NULL;
}

void CTablesPanelBase::OnOpenMapWnd(HWND i_hwnd, int i_mapType)
{
 if (m_toggleMapWnd)
 {
  if (m_openedChart)
   ::SendMessage(m_openedChart,WM_CLOSE,0,0);
  m_openedChart = i_hwnd;
 }

 GDIHelpers::MakeWindowChild(i_hwnd, m_children_charts);
 ::SendMessage(i_hwnd, WM_NCACTIVATE, TRUE, -1);

 if (m_OnOpenMapWnd)
  m_OnOpenMapWnd(i_hwnd, i_mapType);
}

void CTablesPanelBase::OnWndActivation(HWND i_hwnd, long cmd)
{
 if (m_OnWndActivation)
  m_OnWndActivation(i_hwnd, cmd);

 if (m_children_charts && cmd == SC_MINIMIZE && NULL != i_hwnd)
  CWnd::FromHandle(i_hwnd)->SetWindowPos(&CWnd::wndTop, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
}

void CTablesPanelBase::EnableToggleMapWnd(bool toggle)
{
 m_toggleMapWnd = toggle;
}

bool CTablesPanelBase::Is3DMap(int i)
{
 return (i == TYPE_MAP_DA_WORK || i == TYPE_MAP_INJ_VE || i == TYPE_MAP_INJ_VE2 || i == TYPE_MAP_INJ_AFR || 
         i == TYPE_MAP_INJ_IT || i == TYPE_MAP_PWM1 || i == TYPE_MAP_PWM2 || i == TYPE_MAP_GASDOSE);
}

float CTablesPanelBase::GetPtMovStep(int wndType)
{
 return m_md[wndType].ptMovStep;
}

void CTablesPanelBase::SetPtMovStep(int wndType, float value)
{
 m_md[wndType].ptMovStep = value;
 if (m_md[wndType].state)
 {
  if (Is3DMap(wndType))
   DLL::Chart3DSetPtMovingStep(m_md[wndType].handle, value);
  else
   DLL::Chart2DSetPtMovingStep(m_md[wndType].handle, value); 
 }
}

void CTablesPanelBase::SetCSVSepSymbol(char sepsymb)
{
 DLL::Chart2DSetCSVSepSymbol(sepsymb);
 DLL::Chart3DSetCSVSepSymbol(sepsymb);
}

//------------------------------------------------------------------------
void __cdecl CTablesPanelBase::OnGetXAxisLabelRPM(LPTSTR io_label_string, int index, void* i_param)
{
 CTablesPanelBase* _this = static_cast<CTablesPanelBase*>(i_param);
 if (!_this)
 {
  ASSERT(0); //WTF?
  return;
 }
 _stprintf(io_label_string, _T("%d"), MathHelpers::Round(_this->GetRPMGrid()[index]));
}

//------------------------------------------------------------------------
void __cdecl CTablesPanelBase::OnGetXAxisLabelCLT(LPTSTR io_label_string, int index, void* i_param)
{
 CTablesPanelBase* _this = static_cast<CTablesPanelBase*>(i_param);
 if (!_this)
 {
  ASSERT(0); //WTF?
  return;
 }
 _stprintf(io_label_string, _T("%d"), MathHelpers::Round(_this->GetCLTGrid()[index]));
}

//------------------------------------------------------------------------

