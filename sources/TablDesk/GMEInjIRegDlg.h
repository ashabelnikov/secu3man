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

/** \file GMEInjIRegDlg.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include "common/FastDelegate.h"
#include "ui-core/TabDialog.h"
#include "ui-core/MapEditorCtrl.h"

class CGMEInjIRegDlg : public CTabDialog
{
  typedef CTabDialog Super;
  typedef fastdelegate::FastDelegate1<int> EventWithCode;

 public:
  CGMEInjIRegDlg(CWnd* pParent = NULL);   // standard constructor
 ~CGMEInjIRegDlg();
  static const UINT IDD;

  void BindMaps(float* pIdlc, float* pIdlr, float* pITPRM, float* pRigid, float* pIACC, float* pIACCW, float* pIACMAT);
  void BindTemperGrid(float* pGrid); //rest temperature
  void BindCLTGrid(float* pGrid);
  void setOnChange(EventWithCode OnCB);
  void UpdateView(bool axisLabels = false);

  void SetArguments(bool strt_use, float clt, float tps, float iac_pos, float rigid_arg, bool rigid_use, float iat);

 public:

 // Implementation
 protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  virtual BOOL OnInitDialog();
  afx_msg void OnUpdateControls(CCmdUI* pCmdUI);
  DECLARE_MESSAGE_MAP()

  virtual LPCTSTR GetDialogID(void) const;

  void OnChangeIdlc(void);
  void OnChangeIdlr(void);
  void OnChangeITRPM(void);
  void OnChangeRigid(void);
  void OnChangeIACC(void);
  void OnChangeIACCW(void);
  void OnChangeIACMAT(void);

  void OnAbroadMoveIdlc(CMapEditorCtrl::AbroadDir direction, int column);
  void OnAbroadMoveIdlr(CMapEditorCtrl::AbroadDir direction, int column);
  void OnAbroadMoveITRPM(CMapEditorCtrl::AbroadDir direction, int column);
  void OnAbroadMoveRigid(CMapEditorCtrl::AbroadDir direction, int column);
  void OnAbroadMoveIACC(CMapEditorCtrl::AbroadDir direction, int column);
  void OnAbroadMoveIACCW(CMapEditorCtrl::AbroadDir direction, int column);
  void OnAbroadMoveIACMAT(CMapEditorCtrl::AbroadDir direction, int column);

 private:
  void _UpdateDynamicGrids(void);

  CMapEditorCtrl m_idlc_map;
  CMapEditorCtrl m_idlr_map;
  CMapEditorCtrl m_itrpm_map;
  CMapEditorCtrl m_rigid_map;
  CMapEditorCtrl m_iacc_map;
  CMapEditorCtrl m_iaccw_map;
  CMapEditorCtrl m_iacmat_map;

  float* mp_IdlcMap;
  float* mp_IdlrMap;
  float* mp_ITRPMMap;
  float* mp_RigidMap;
  float* mp_IACCMap;
  float* mp_IACCWMap;
  float* mp_IACMATMap;

  float* mp_temperGrid;
  float* mp_cltGrid;
  std::vector<float> m_tpsGrid;
  std::vector<float> m_iacGrid;

  CFont m_font;
  EventWithCode m_OnChange;
};
