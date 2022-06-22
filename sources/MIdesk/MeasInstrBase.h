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
#include "ui-core/AnalogMeterCtrl.h"
#include "ui-core/OscillCtrl.h"


typedef std::vector<std::pair<float, bool> > GraphVal_t;

class MeasInstrBase
{
 float _averageVector(GraphVal_t* values)
 {
  ASSERT(values);
  float sum = 0;
  for (size_t i = 0; i < values->size(); ++i)    
   sum+=(*values)[i].first;
  return sum / values->size();
 }

 public:
  MeasInstrBase() : m_metVal(NULL), m_tlpVal(NULL), m_trpVal(NULL), m_showTLP(false), m_showTRP(false), m_uiID(0) {};
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

  //interface for measurement instrument
  virtual void Create(CWnd* pParent, UINT id)
  {
   CRect rect(0,0,10,10);
   VERIFY(m_meter.CreateEx(0, _T("Static"), _T(""), WS_CHILD | WS_VISIBLE | SS_NOTIFY | WS_CLIPSIBLINGS, rect, pParent, id));
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

  virtual void SetValues(void)
  {
   if (m_metVal)
   {    
    if (m_meter.GetSafeHwnd())
    { //meter mode
     if (m_metVal->size())
     {
      m_meter.SetNeedleValue((double)_averageVector(m_metVal));    
     }
     if (m_tlpVal && m_tlpVal->size())
     {
      CString str;
      _TSTRING template_str = m_tlpFmt + m_tlpUnit;
      str.Format(template_str.c_str(), _averageVector(m_tlpVal));
      m_meter.SetTLPane(str);
     }
     if (m_trpVal && m_trpVal->size())
     {
      CString str;
      _TSTRING template_str = m_trpFmt + m_trpUnit;
      str.Format(template_str.c_str(), _averageVector(m_trpVal));
      m_meter.SetTRPane(str);
     }
     m_meter.Update();
    }
    else if (m_scope.GetSafeHwnd())
    { //oscilloscope mode
     for (size_t i = 0; i < m_metVal->size(); ++i) 
      m_scope.AppendPoint((*m_metVal)[i].first, (*m_metVal)[i].second);
    }
   }
  }

  virtual void BindVars(GraphVal_t* meter, GraphVal_t* tlpane, GraphVal_t* trpane)
  {
   m_metVal = meter, m_tlpVal = tlpane, m_trpVal = trpane;
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

  //прибор видим или скрыт ?
  virtual bool IsVisible(void)
  {
   return (m_meter.IsWindowVisible()) ? true : false;
  }

  //прибор разрешен или запрещен ?
  virtual bool IsEnabled(void)
  {
   bool State = false;
   m_meter.GetState(meter_needle, &State);
   return State;
  }

  //установка пределов измерения
  virtual void SetLimits(float loLimit, float upLimit)
  {
   m_meter.SetRange(loLimit, upLimit);
  }

  //установка количества делений
  virtual void SetTicks(int number)
  {
   m_meter.SetTickNumber(number);
  }

  virtual void ShowTLP(bool i_show, bool redraw = false)
  {
   m_showTLP = i_show;
   m_meter.SetState(meter_tlpane, IsEnabled() && i_show);
   if (redraw)
    m_meter.Redraw();
  }

  virtual void ShowTRP(bool i_show, bool redraw = false)
  {
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

  UINT m_uiID;

protected:
  CAnalogMeterCtrl m_meter;
  COscillCtrl m_scope;
  _TSTRING m_tlpFmt;
  _TSTRING m_trpFmt;
  _TSTRING m_tlpUnit;
  _TSTRING m_trpUnit;

  GraphVal_t* m_metVal;
  GraphVal_t* m_tlpVal;
  GraphVal_t* m_trpVal;

  bool m_showTLP;
  bool m_showTRP;
};
