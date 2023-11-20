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
#include "common/dpiaware.h"
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
, m_embed_charts(false)
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

 if (m_embed_charts)
  ::SetWindowPos(i_hwnd, NULL, m_embed_rect.left, m_embed_rect.top, m_embed_rect.Width(), m_embed_rect.Height(), SWP_NOZORDER);  
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

void CTablesPanelBase::EnableEmbedMapWnd(bool embed, const CRect& rc, int begin, int end)
{
 m_embed_charts = embed;
 m_embed_rect = rc;

 if (embed)
 {
  for(int i = begin; i <= end; ++i)
  {
   if (m_md[i].state && IsWindow(m_md[i].handle))
   {
    ::SetWindowPos(m_md[i].handle, NULL, rc.left, rc.top, rc.Width(), rc.Height(), SWP_NOZORDER);  
   }
  }
 }
}

bool CTablesPanelBase::Is3DMap(int i)
{
 return (i == TYPE_MAP_DA_WORK || i == TYPE_MAP_INJ_VE || i == TYPE_MAP_INJ_VE2 || i == TYPE_MAP_INJ_AFR || 
         i == TYPE_MAP_INJ_IT || i == TYPE_MAP_PWM1 || i == TYPE_MAP_PWM2 || i == TYPE_MAP_GASDOSE || i == TYPE_MAP_KNOCK_ZONE || i == TYPE_MAP_LAMBDA_ZONE);
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
void CTablesPanelBase::AlignButtons(CWnd *p_parent, int cx, int id1st, int id2nd, int idHalf, int begin, int end)
{
 DPIAware da;
 CRect rc1 = GDIHelpers::GetChildWndRect(m_md[id1st].mp_button);
 int c = da.ScaleX(8), half_id = idHalf;
  
 if ((cx - rc1.left) - rc1.Width() > rc1.Width()*0.6f)
 {
  if (m_btnMovIds.empty())
  {//move
   CRect rc2 = GDIHelpers::GetChildWndRect(m_md[id2nd].mp_button);
   CRect rc3 = GDIHelpers::GetChildWndRect(m_md[half_id].mp_button);
   int decr = (rc3.top - rc1.top) + (rc2.top - rc1.top);

   for (int i = begin; i <= end; ++i)
   {
    CRect rc4 = GDIHelpers::GetChildWndRect(m_md[i].mp_button);
    if (rc4.top > rc3.top)
     m_btnMovIds.push_back(i);
   }  
   for(size_t t = 0; t < m_btnMovIds.size(); ++t)
   {
    CRect rc = GDIHelpers::GetChildWndRect(m_md[m_btnMovIds[t]].mp_button);
    m_md[m_btnMovIds[t]].mp_button->SetWindowPos(NULL, rc1.right + c, rc.top - decr, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
   }
  }
 }
 else
 {
  if (!m_btnMovIds.empty())
  {//restore
   CRect rc2 = GDIHelpers::GetChildWndRect(m_md[id2nd].mp_button);
   CRect rc3 = GDIHelpers::GetChildWndRect(m_md[half_id].mp_button);
   int decr = (rc3.top - rc1.top) + (rc2.top - rc1.top);

   for(size_t t = 0; t < m_btnMovIds.size(); ++t)
   {
    CRect rc = GDIHelpers::GetChildWndRect(m_md[m_btnMovIds[t]].mp_button);
    m_md[m_btnMovIds[t]].mp_button->SetWindowPos(NULL, rc1.left, rc.top + decr, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
   }
   m_btnMovIds.clear();   
  }
 }
}

