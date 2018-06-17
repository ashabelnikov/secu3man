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
#include "common/FastDelegate.h"
#include "common/UnicodeSupport.h"
#include "DynamicValues.h"
#include "ui-core/MapEditorCtrl.h"

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

  void BindMaps(float* pStart, float* pIdle, float* pWork, float* pTemp);
  void BindRPMGrid(float* pGrid);
  void UpdateView(void);

  void EnableAdvanceAngleIndication(bool i_enable);

  void SetDynamicValues(const TablDesk::DynVal& dv);

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
  afx_msg void OnClose();
  DECLARE_MESSAGE_MAP()

  EventResult   m_IsAllowed;
  EventWithCode m_OnMapChanged;
  EventWithHWND m_OnOpenMapWnd;
  EventWithHWND m_OnCloseMapWnd;

 private:
  void _ResetUseFlags(void);

  void OnChangeStart(void);
  void OnChangeIdle(void);
  void OnChangeWork(void);
  void OnChangeTemp(void);

  void OnAbroadMoveStart(CMapEditorCtrl::AbroadDir direction, int column);
  void OnAbroadMoveIdle(CMapEditorCtrl::AbroadDir direction, int column);
  void OnAbroadMoveWork(CMapEditorCtrl::AbroadDir direction, int column);
  void OnAbroadMoveTemp(CMapEditorCtrl::AbroadDir direction, int column);

  CMapEditorCtrl m_start_map;
  CMapEditorCtrl m_idle_map;
  CMapEditorCtrl m_work_map;
  CMapEditorCtrl m_temp_map;

  CStatic m_aa_value;
  CStatic m_wm_value;
  CStatic m_oc_value;
  CStatic m_tc_value;
  CStatic m_kc_value;
  CStatic m_im_value;
  CStatic m_ic_value;
  CStatic m_ac_value;

  float* mp_startMap;
  float* mp_idleMap;
  float* mp_workMap;
  float* mp_tempMap;
  float* mp_rpmGrid;
  bool m_en_aa_indication;
  CFont m_font;
  CFont m_fieldFont;
  TablDesk::DynVal m_curDV;
};
