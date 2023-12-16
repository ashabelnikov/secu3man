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

/** \file GMEInjEnrDlg.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include "common/FastDelegate.h"
#include "ui-core/TabDialog.h"
#include "ui-core/MapEditorCtrl.h"

class CtrlScaler;

class CGMEInjEnrDlg : public CTabDialog
{
  typedef CTabDialog Super;
  typedef fastdelegate::FastDelegate1<int> EventWithCode;

 public:
  CGMEInjEnrDlg();
 ~CGMEInjEnrDlg();

  void BindMaps(float* pAftstr, float* pWrmp, float* pAETPS, float* pAERPM, float* pAEMAP);
  void BindMapsOrig(float* pAftstr, float* pWrmp, float* pAETPS, float* pAERPM, float* pAEMAP);
  void BindCLTGrid(float* pGrid);
  void setOnChange(EventWithCode OnCB);
  void UpdateView(bool axisLabels = false);

  void SetArguments(bool strt_use, float clt, int tpsdot, int rpm, bool aftstr_enr, int mapdot);

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

  void OnChangeAftstr(void);
  void OnChangeWrmp(void);
  void OnChangeAETPS(void);
  void OnChangeAERPM(void);
  void OnChangeAEMAP(void);

  void OnAbroadMoveAftstr(CMapEditorCtrl::AbroadDir direction, int column);
  void OnAbroadMoveWrmp(CMapEditorCtrl::AbroadDir direction, int column);
  void OnAbroadMoveAETPS(CMapEditorCtrl::AbroadDir direction, int column);
  void OnAbroadMoveAERPM(CMapEditorCtrl::AbroadDir direction, int column);
  void OnAbroadMoveAEMAP(CMapEditorCtrl::AbroadDir direction, int column);

 private:
  CMapEditorCtrl m_aftstr_map;
  CMapEditorCtrl m_wrmp_map;
  CMapEditorCtrl m_aetps_map;
  CMapEditorCtrl m_aerpm_map;
  CMapEditorCtrl m_aemap_map;
  CStatic m_ctrls[5];
  std::auto_ptr<CtrlScaler> mp_cscl;  

  float* mp_AftstrMap[2];
  float* mp_WrmpMap[2];
  float* mp_AETPSMap[2];
  float* mp_AERPMMap[2];
  float* mp_AEMAPMap[2];

  float* mp_cltGrid;

  CFont m_font;
  EventWithCode m_OnChange;
  bool m_initialized;
};
