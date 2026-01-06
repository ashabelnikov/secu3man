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

/** \file MeasInstrBase.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include <vector>
#include <utility>
#include "common/GDIHelpers.h"
#include "common/SettingsTypes.h"
#include "io-core/SECU3IO.h"
#include "ui-core/AnalogMeterCtrl.h"
#include "ui-core/OscillCtrl.h"
#include "RingBuffer.h"

typedef std::vector<std::pair<float, bool> > GraphVal_t;

#define MIB_VQR 64

class MeasInstrBase
{
 float _averageVector(GraphVal_t* values)
 {
  ASSERT(values);
  float sum = 0;
  size_t size = values->size();
  for (size_t i = 0; i < size; ++i)    
   sum+=(*values)[i].first;
  return sum / values->size();
 }

 public:
  MeasInstrBase() : m_showTLP(false), m_showTRP(false), m_uiID(0),
                    mp_avnumVal(NULL), mp_avnumTlp(NULL), mp_avnumTrp(NULL)
  {
   m_metVal.reserve(MIB_VQR);
  };
  virtual ~MeasInstrBase()
  {
   m_meter.DestroyWindow();
  };

  void SetFontSize(int title, int value, int pane, int label)
  {
   m_meter.SetFontScale(meter_title, title);
   m_meter.SetFontScale(meter_value, value);
   m_meter.SetFontScale(meter_trpane, pane);
   m_meter.SetFontScale(meter_labels, label);
  }
  
  void BindAverageNum(int *avnumVal, int *avnumTlp, int *avnumTrp)
  {
   mp_avnumVal = avnumVal;
   mp_avnumTlp = avnumTlp;
   mp_avnumTrp = avnumTrp;
  }

  //interface for measurement instrument
  virtual void Create(CWnd* pParent, UINT id, bool showTLP, bool showTRP)
  {
   m_showTLP = showTLP;
   m_showTRP = showTRP;

   if (showTLP)
    m_tlpVal.reserve(MIB_VQR);
   if (showTRP)
    m_trpVal.reserve(MIB_VQR);

   if (mp_avnumVal)
    m_ringMetVal.m_avnum = *mp_avnumVal;   
   if (mp_avnumTlp)
    m_ringTlpVal.m_avnum = *mp_avnumTlp;
   if (mp_avnumTrp)
    m_ringTrpVal.m_avnum = *mp_avnumTrp;

   CRect rect(0,0,10,10);
   VERIFY(m_meter.CreateEx(0, _T("Static"), _T(""), WS_CHILD | WS_VISIBLE | SS_NOTIFY | WS_CLIPSIBLINGS, rect, pParent, id));
  }

  virtual void Create(CWnd* pParent, UINT id)
  {
   m_showTLP = false;
   m_showTRP = false; //not applicable in oscilloscope ctrl.

   CRect rect(0,0, 100,100);
   VERIFY(m_scope.Create(WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS, rect, pParent, id));
  }

  virtual void Resize(const CRect& rect, bool redraw = true)
  {
   if (m_meter.GetSafeHwnd())
   {
    m_meter.MoveWindow(rect, redraw);
    m_meter.Redraw();
   }
   else if (m_scope.GetSafeHwnd())
   {
    m_scope.MoveWindow(rect, redraw);
    m_scope.InvalidateCtrl(false, false, true);
   }
  }

  virtual void Append(const SECU3IO::SensorDat* i_values, bool i_revdir = false) = 0;

  void Append(float metVal, float tlpVal, float trpVal, bool i_revdir = false)
  {
   m_ringMetVal.Append(metVal);
   m_ringMetVal.Calculate();
   m_metVal.push_back(std::make_pair(m_ringMetVal.m_result, i_revdir));

   if (m_showTLP)
   {
    m_ringTlpVal.Append(tlpVal);
    m_ringTlpVal.Calculate();
    m_tlpVal.push_back(std::make_pair(m_ringTlpVal.m_result, i_revdir));
   }

   if (m_showTRP)
   {
    m_ringTrpVal.Append(trpVal);
    m_ringTrpVal.Calculate();
    m_trpVal.push_back(std::make_pair(m_ringTrpVal.m_result, i_revdir));
   }
  }

  void Append(float metVal, bool i_revdir = false)
  {
   m_metVal.push_back(std::make_pair(metVal, i_revdir));
  }

  virtual void SetValues(void)
  {
   if (m_meter.GetSafeHwnd())
   { //meter mode
    if (m_metVal.size())
    {
     m_meter.SetNeedleValue((double)_averageVector(&m_metVal));    
     m_metVal.clear();
    }
    if (m_showTLP && m_tlpVal.size())
    {
     CString str;
     _TSTRING template_str = m_tlpFmt + m_tlpUnit;
     str.Format(template_str.c_str(), _averageVector(&m_tlpVal));
     m_meter.SetTLPane(str);
     m_tlpVal.clear();
    }
    if (m_showTRP && m_trpVal.size())
    {
     CString str;
     _TSTRING template_str = m_trpFmt + m_trpUnit;
     str.Format(template_str.c_str(), _averageVector(&m_trpVal));
     m_meter.SetTRPane(str);
     m_trpVal.clear();
    }
    m_meter.Update();
   }
   else if (m_scope.GetSafeHwnd())
   { //oscilloscope mode
    size_t size = m_metVal.size();
    for (size_t i = 0; i < size; ++i) 
     m_scope.AppendPoint(m_metVal[i].first, m_metVal[i].second);
    m_metVal.clear();
   }
  }

  //скрытие/отображение прибора
  virtual void Show(bool show)
  {
   if (m_meter.GetSafeHwnd())
    m_meter.ShowWindow((show) ? SW_SHOW : SW_HIDE);
   else if (m_scope.GetSafeHwnd())
    m_scope.ShowWindow((show) ? SW_SHOW : SW_HIDE);
  }

  //разрешение/запрещение прибора
  virtual void Enable(bool enable, bool redraw = true)
  {
   if (m_meter.GetSafeHwnd())
   {
    m_meter.SetState(meter_needle, enable);
    m_meter.SetState(meter_value, enable);
    m_meter.SetState(meter_grid, enable);
    m_meter.SetState(meter_labels, enable);
    m_meter.SetState(meter_unit, enable);
    m_meter.SetState(meter_tlpane, enable && m_showTLP);
    m_meter.SetState(meter_trpane, enable && m_showTRP);
    COLORREF bk_color;
    m_meter.GetColor(meter_bground, &bk_color);
    m_meter.SetColor(meter_bground, enable ? bk_color : ::GetSysColor(COLOR_BTNFACE));

    if (redraw)
     m_meter.Redraw();
   }
   else if (m_scope.GetSafeHwnd())
    m_scope.EnableWindow(enable);
  }

  //is gauge visible?
  virtual bool IsVisible(void)
  {
   return (m_meter.IsWindowVisible()) ? true : false;
  }

  //is gauge enabled?
  virtual bool IsEnabled(void)
  {
   bool State = false;
   m_meter.GetState(meter_needle, &State);
   return State;
  }

  void Redraw(void)
  {
   if (m_meter.GetSafeHwnd())
    m_meter.Redraw();
   else if (m_scope.GetSafeHwnd())
    m_scope.InvalidateCtrl(false, false, true);
  }

  //Set scale range
  virtual void SetLimits(float loLimit, float upLimit)
  {
   if (m_meter.GetSafeHwnd())
    m_meter.SetRange(loLimit, upLimit);
   else if (m_scope.GetSafeHwnd())
    m_scope.SetRange(loLimit, upLimit);
  }

  virtual void ShowTLP(bool i_show, bool redraw = false)
  {
   if (i_show)
    m_tlpVal.reserve(MIB_VQR);
   m_showTLP = i_show;
   m_meter.SetState(meter_tlpane, IsEnabled() && i_show);
   if (redraw)
    m_meter.Redraw();
  }

  virtual void ShowTRP(bool i_show, bool redraw = false)
  {
   if (i_show)
    m_trpVal.reserve(MIB_VQR);
   m_showTRP = i_show;
   m_meter.SetState(meter_trpane, IsEnabled() && i_show);
   if (redraw)
    m_meter.Redraw();
  }

  virtual void SetPaneUnit(const _TSTRING& tlpUnit, const _TSTRING& trpUnit)
  {
   m_tlpUnit = tlpUnit;
   m_trpUnit = trpUnit;
  }

  virtual void SetMeterUnit(const _TSTRING& metUnit)
  {
   m_meter.SetUnit(metUnit.c_str());
   if (m_scope.GetSafeHwnd())
    m_scope.SetUnitY(metUnit.c_str());
  }

  virtual void UpdateColors(void)
  {
   m_meter.SetColor(meter_bground, GetSysColor(COLOR_BTNFACE));
   m_meter.SetColor(meter_labels, GDIHelpers::InvColor(GetSysColor(COLOR_BTNFACE)));
   m_meter.Redraw();
  }

  virtual CRect GetWindowRect(bool screen = false)
  {
   CRect rc;
   if (m_meter.GetSafeHwnd())
   {
    if (screen)
    {
     m_meter.GetWindowRect(&rc);
     return rc;
    }
    else
     return GDIHelpers::GetChildWndRect(&m_meter);
   }
   else if (m_scope.GetSafeHwnd())
   {
    if (screen)
    {
     m_scope.GetWindowRect(&rc);
     return rc;
    }
    else
     return GDIHelpers::GetChildWndRect(&m_scope);
   }
   return rc;
  }

  virtual void Reset(void)
  {
   if (m_scope.GetSafeHwnd())
    m_scope.Reset();
  }

  virtual ShowCursor(bool show)
  {
   m_scope.ShowCursor(show);
  }

  virtual ShowValue(bool show)
  {
   m_scope.ShowValue(show);
  }

  virtual SetValueHeight(int height)
  {
   m_scope.SetValueHeight(height);
  }

  bool isGraph(void)
  {
   return (m_scope.GetSafeHwnd() != NULL);
  }

  int GetMaxPtCount(void)
  {
   return m_scope.GetMaxPtCount();
  }
 
  void SetShtPixels(int n)
  {
   if (m_scope.GetSafeHwnd())
    m_scope.SetShtPixels(n);
  }

  void SetAlertZones(const std::vector<AlertZone>& az)
  {
   if (az.size() && m_meter.GetSafeHwnd())
   {
    m_meter.ResetAlertZones();
    for(size_t i = 0; i < az.size(); ++i)
     m_meter.AddAlertZone(az[i].start, az[i].end, az[i].color);
   }
  }

  void SetTickNumber(int tn)
  {
   if (m_meter.GetSafeHwnd())
    m_meter.SetTickNumber(tn);
   else if (m_scope.GetSafeHwnd())
    m_scope.SetGridNumberY(tn);     
  }

  void SetLineWidth(int lw)
  {
   if (m_meter.GetSafeHwnd())
    m_meter.SetGridLineWidth(lw);
   else if (m_scope.GetSafeHwnd())
    m_scope.SetPlotWidth(lw);     
  }

  void SetLineColor(COLORREF color)
  {
   if (m_meter.GetSafeHwnd())
    m_meter.SetColor(meter_grid, color);
   else if (m_scope.GetSafeHwnd())
    m_scope.SetPlotColor(color);     
  }

  void SetPieRadius(float radius)
  {
   if (m_meter.GetSafeHwnd())
    m_meter.SetPieRadius(radius);
  }

  UINT m_uiID;

protected:
  CAnalogMeterCtrl m_meter;
  COscillCtrl m_scope;
  _TSTRING m_tlpFmt;
  _TSTRING m_trpFmt;
  _TSTRING m_tlpUnit;
  _TSTRING m_trpUnit;

  GraphVal_t m_metVal; //queues with values
  GraphVal_t m_tlpVal;
  GraphVal_t m_trpVal;

  RingBuffItem m_ringMetVal; //ring buffers for arrow gauges
  RingBuffItem m_ringTlpVal;
  RingBuffItem m_ringTrpVal;

  int *mp_avnumVal; //pointer to a number of averages for gauge
  int *mp_avnumTlp; //... number of averages for left pane
  int *mp_avnumTrp; //... number of averages for right pane

  bool m_showTLP;
  bool m_showTRP;
};
