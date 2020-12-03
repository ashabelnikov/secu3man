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

/** \file GridModeEditorIgnDlg.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include <memory>
#include <vector>
#include <limits>
#include "common/FastDelegate.h"
#include "common/UnicodeSupport.h"
#include "DynamicValues.h"
#include "ui-core/MapEditorCtrl.h"

#undef max

class CLabel;
class CToolTipCtrlEx;

class CStaticDisp : public CStatic
{
 public:
  CStaticDisp() : m_value(std::numeric_limits<float>::max()) {}

  void SetValue(float value)
  {
   if (value != m_value)
   {
    CString str;
    str.Format(_T("%0.2f"), value), SetWindowText(str);
    m_value = value;
   }
  }

 private:
  float m_value; //cached value
};


class CGridModeEditorIgnDlg : public CDialog
{
  typedef CDialog Super;
  typedef fastdelegate::FastDelegate0<bool> EventResult;
  typedef fastdelegate::FastDelegate1<int> EventWithCode;
  typedef fastdelegate::FastDelegate2<HWND, int> EventWithHWND;

 public:
  CGridModeEditorIgnDlg(CWnd* pParent = NULL);   // standard constructor
 ~CGridModeEditorIgnDlg();
  static const UINT IDD;

  void BindMaps(float* pStart, float* pIdle, float* pWork, float* pTemp, float* pTempIdl);
  void BindRPMGrid(float* pGrid);
  void BindCLTGrid(float* pGrid);
  void BindLoadGrid(float* pGrid);
  void UpdateView(bool axisLabels = false);

  void EnableAdvanceAngleIndication(bool i_enable);

  void SetDynamicValues(const TablDesk::DynVal& dv);

  void SetLoadAxisCfg(float minVal, float maxVal, bool useTable, bool forceUpdate = false);

 public:
  void setIsAllowed(EventResult IsFunction);
  void setOnMapChanged(EventWithCode OnFunction);
  void setOnOpenMapWnd(EventWithHWND OnFunction); 
  void setOnCloseMapWnd(EventWithHWND OnFunction);

 // Implementation
 protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  virtual BOOL OnInitDialog();
  afx_msg void OnUpdateControls(CCmdUI* pCmdUI);
  afx_msg void OnUpdateAAControls(CCmdUI* pCmdUI);
  afx_msg void OnSelectCLTMap();
  afx_msg void OnClose();
  DECLARE_MESSAGE_MAP()

  EventResult   m_IsAllowed;
  EventWithCode m_OnMapChanged;
  EventWithHWND m_OnOpenMapWnd;
  EventWithHWND m_OnCloseMapWnd;

 private:
  void _ResetUseFlags(void);

  void OnAcronymsLinkClick();

  void OnChangeStart(void);
  void OnChangeIdle(void);
  void OnChangeWork(void);
  void OnChangeTemp(void);
  void OnChangeTempIdl(void);

  void OnAbroadMoveStart(CMapEditorCtrl::AbroadDir direction, int column);
  void OnAbroadMoveIdle(CMapEditorCtrl::AbroadDir direction, int column);
  void OnAbroadMoveWork(CMapEditorCtrl::AbroadDir direction, int column);
  void OnAbroadMoveTemp(CMapEditorCtrl::AbroadDir direction, int column);
  void OnAbroadMoveTempIdl(CMapEditorCtrl::AbroadDir direction, int column);

  CMapEditorCtrl m_start_map;
  CMapEditorCtrl m_idle_map;
  CMapEditorCtrl m_work_map;
  CMapEditorCtrl m_temp_map;
  CMapEditorCtrl m_tempi_map;

  CStaticDisp m_aa_value;
  CStaticDisp m_wm_value;
  CStaticDisp m_oc_value;
  CStaticDisp m_tc_value;
  CStaticDisp m_kc_value;
  CStaticDisp m_im_value;
  CStaticDisp m_ic_value;
  CStaticDisp m_ac_value;
  CButton m_cltmap_sel;
  std::auto_ptr<CToolTipCtrlEx> mp_ttc;

  float* mp_startMap;
  float* mp_idleMap;
  float* mp_workMap;
  float* mp_tempMap;
  float* mp_tempIdlMap;
  float* mp_rpmGrid;
  float* mp_cltGrid;
  float* mp_lodGrid;
  bool m_en_aa_indication;
  CFont m_font;
  CFont m_fieldFont;
  TablDesk::DynVal m_curDV;

  float m_ldaxMin;
  float m_ldaxMax;
  bool m_ldaxUseTable;
  bool m_ldaxNeedsUpdate;
  std::vector<float> m_work_map_load_slots;

  std::auto_ptr<CLabel> mp_acronLink;
};
