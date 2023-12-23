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

/** \file TablesPanelBase.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include <memory>
#include <map>
#include <vector>
#include "common/FastDelegate.h"
#include "LdaxCfg.h"

struct MapData
{
 MapData()
 : state(0), handle(NULL), ptMovStep(0.5f), mp_button(NULL)
 {
  std::fill(original, original + 256, .0f);
  std::fill(active, active + 256, .0f);
 }

 float original[256];
 float active[256];
 int state;
 HWND handle;
 float ptMovStep;
 CButton *mp_button;
};

class CWndScroller;
class CToolTipCtrlEx;

/////////////////////////////////////////////////////////////////////////////
// CTablesPanelBase class

class AFX_EXT_CLASS CTablesPanelBase : public LdaxCfg
{
 public:
  typedef fastdelegate::FastDelegate0<> EventHandler;
  typedef fastdelegate::FastDelegate1<int> EventWithCode;
  typedef fastdelegate::FastDelegate0<bool> EventResult;
  typedef fastdelegate::FastDelegate2<HWND, int> EventWithHWND;
  typedef fastdelegate::FastDelegate2<HWND, long> EventWithHWNDLong;

  CTablesPanelBase();
  virtual ~CTablesPanelBase();

  float* GetRPMGrid(void);
  float* GetCLTGrid(void);
  float* GetLoadGrid(void);

  void EnableToggleMapWnd(bool toggle);

  float GetPtMovStep(int wndType);
  void SetPtMovStep(int wndType, float value);

  void SetCSVSepSymbol(char sepsymb);

 public: //установка обработчиков событий
  void setOnChangeSettings(EventHandler OnCB);
  void setOnMapChanged(EventWithCode OnFunction);
  void setOnCloseMapWnd(EventWithHWND OnFunction);
  void setOnOpenMapWnd(EventWithHWND OnFunction); 
  void setOnWndActivation(EventWithHWNDLong OnFunction); 
  void setIsAllowed(EventResult IsFunction);

 protected:
  bool IsAllowed(void);
  bool Is3DMap(int i);
  virtual void MakeChartsChildren(bool children);
  void EnableEmbedMapWnd(bool embed, const CRect& rc, int begin, int end);
  void AlignButtons(CWnd *p_parent, int cx, int id1st, int id2nd, int idHalf, int begin, int end);

  static void __cdecl OnGetXAxisLabelRPM(LPTSTR io_label_string, int index, void* i_param);
  static void __cdecl OnGetXAxisLabelCLT(LPTSTR io_label_string, int index, void* i_param);
  static void __cdecl OnGetXAxisLabelLoad(LPTSTR io_label_string, int index, void* i_param);

  HWND _ChartParentHwnd(void);
  void OnOpenMapWnd(HWND i_hwnd, int i_mapType);
  void OnWndActivation(HWND i_hwnd, long cmd);

  EventWithCode m_OnMapChanged;
  EventWithHWND m_OnCloseMapWnd;
  EventWithHWND m_OnOpenMapWnd;
  EventWithHWNDLong m_OnWndActivation;
  EventResult   m_IsAllowed;
  EventHandler  m_OnChangeSettings;

 protected:
  std::auto_ptr<CWndScroller> mp_scr;
  std::auto_ptr<CToolTipCtrlEx> mp_ttc;
  std::map<int, MapData> m_md;

  bool m_gasdose;
  bool m_fuel_injection;

  bool m_children_charts;
  bool m_embed_charts;
  CRect m_embed_rect;
  int m_charts_enabled;

  std::vector<int> m_btnMovIds;
  bool m_disable_vscroll;

 private:
  float m_rpm_grid_values[16];
  float m_clt_grid_values[16];
  float m_load_grid_values[16];

  bool m_toggleMapWnd;
  HWND m_openedChart;
};
