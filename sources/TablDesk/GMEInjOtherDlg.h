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

/** \file GMEInjOtherDlg.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include "common/FastDelegate.h"
#include "ui-core/TabDialog.h"
#include "ui-core/MapEditorCtrl.h"

class CGMEInjOtherDlg : public CTabDialog
{
  typedef CTabDialog Super;
  typedef fastdelegate::FastDelegate1<int> EventWithCode;

 public:
  CGMEInjOtherDlg(CWnd* pParent = NULL);   // standard constructor
 ~CGMEInjOtherDlg();
  static const UINT IDD;

  void BindMaps(float* pCrnk, float* pDead, float* pEGOCrv, float* pIATCLT, float* pTpsswt, float* pAtsc, float* pGtsc, float* pGpsc);
  void BindTemperGrid(float* pGrid);
  void BindDeadGrid(float* pGrid);
  void BindRPMGrid(float* pGrid);
  void setOnChange(EventWithCode OnCB);
  void UpdateView(void);

  void SetArguments(bool strt_use, float clt, float voltage, float add_i1, int rpm, float tmp2, float iat);

 public:

 // Implementation
 protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  virtual BOOL OnInitDialog();
  afx_msg void OnUpdateControls(CCmdUI* pCmdUI);
  DECLARE_MESSAGE_MAP()

  virtual LPCTSTR GetDialogID(void) const;

  void OnChangeCrnk(void);
  void OnChangeDead(void);
  void OnChangeEGOCrv(void);
  void OnChangeIATCLT(void);
  void OnChangeTpsswt(void);
  void OnChangeAtsc(void);
  void OnChangeGtsc(void);
  void OnChangeGpsc(void);

  void OnAbroadMoveCrnk(CMapEditorCtrl::AbroadDir direction, int column);
  void OnAbroadMoveDead0(CMapEditorCtrl::AbroadDir direction, int column);
  void OnAbroadMoveDead1(CMapEditorCtrl::AbroadDir direction, int column);
  void OnAbroadMoveEGOCrv(CMapEditorCtrl::AbroadDir direction, int column);
  void OnAbroadMoveIATCLT(CMapEditorCtrl::AbroadDir direction, int column);
  void OnAbroadMoveTpsswt(CMapEditorCtrl::AbroadDir direction, int column);
  void OnAbroadMoveAtsc(CMapEditorCtrl::AbroadDir direction, int column);
  void OnAbroadMoveGtsc(CMapEditorCtrl::AbroadDir direction, int column);
  void OnAbroadMoveGpsc(CMapEditorCtrl::AbroadDir direction, int column);

 private:
  void _UpdateDynamicGrids(void);

  CMapEditorCtrl m_crnk_map;
  CMapEditorCtrl m_dead0_map;
  CMapEditorCtrl m_dead1_map;
  CMapEditorCtrl m_egocrv_map;
  CMapEditorCtrl m_iatclt_map;
  CMapEditorCtrl m_tpsswt_map;
  CMapEditorCtrl m_atsc_map;
  CMapEditorCtrl m_gtsc_map;
  CMapEditorCtrl m_gpsc_map;

  float* mp_CrnkMap;
  float* mp_DeadMap;
  float* mp_EGOCrvMap;
  float* mp_IATCLTMap;
  float* mp_TpsswtMap;
  float* mp_AtscMap;
  float* mp_GtscMap;
  float* mp_GpscMap;

  float* mp_temperGrid;
  float* mp_deadGrid;
  float* mp_rpmGrid;
  std::vector<float> m_egocrvGrid;
  std::vector<float> m_iatcltGrid;
  std::vector<float> m_gpscGrid;

  CFont m_font;
  EventWithCode m_OnChange;
};
