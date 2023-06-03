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

/** \file GMEInjOther1Dlg.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include "common/FastDelegate.h"
#include "ui-core/TabDialog.h"
#include "ui-core/MapEditorCtrl.h"

class CtrlScaler;

class CGMEInjOther1Dlg : public CTabDialog
{
  typedef CTabDialog Super;
  typedef fastdelegate::FastDelegate1<int> EventWithCode;

 public:
  CGMEInjOther1Dlg();
 ~CGMEInjOther1Dlg();

  void BindMaps(float* pTpsswt, float* pTpszon, float* pCylMult, float* pCylAdd, float* pThrass);
  void BindRPMGrid(float* pGrid);
  void setOnChange(EventWithCode OnCB);
  void UpdateView(bool axisLabels = false);

  void SetArguments(bool strt_use, int rpm);

 public:

 // Implementation
 protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  virtual BOOL OnInitDialog();
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnUpdateControls(CCmdUI* pCmdUI);
  afx_msg void OnDestroy();
  DECLARE_MESSAGE_MAP()

  virtual LPCTSTR GetDialogID(void) const;

  void OnChangeTpsswt(void);
  void OnChangeTpszon(void);
  void OnChangeCylMult(void);
  void OnChangeCylAdd(void);
  void OnChangeThrass(void);

  void OnAbroadMoveTpsswt(CMapEditorCtrl::AbroadDir direction, int column);
  void OnAbroadMoveTpszon(CMapEditorCtrl::AbroadDir direction, int column);
  void OnAbroadMoveCylMult(CMapEditorCtrl::AbroadDir direction, int column);
  void OnAbroadMoveCylAdd(CMapEditorCtrl::AbroadDir direction, int column);
  void OnAbroadMoveThrass(CMapEditorCtrl::AbroadDir direction, int column);

 private:
  CMapEditorCtrl m_tpsswt_map;
  CMapEditorCtrl m_tpszon_map;
  CMapEditorCtrl m_cylmult_map;
  CMapEditorCtrl m_cyladd_map;
  CMapEditorCtrl m_thrass_map;
  CStatic m_ctrls[5];

  float* mp_TpsswtMap;
  float* mp_TpszonMap;
  float* mp_CylMultMap;
  float* mp_CylAddMap;
  float* mp_ThrassMap;

  float* mp_rpmGrid;
  std::auto_ptr<CtrlScaler> mp_cscl;

  CFont m_font;
  EventWithCode m_OnChange;
  bool m_initialized;
};
