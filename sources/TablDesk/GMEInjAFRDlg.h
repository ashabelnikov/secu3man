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

/** \file GMEInjAFRDlg.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include "common/FastDelegate.h"
#include "ui-core/TabDialog.h"
#include "ui-core/MapEditorCtrl.h"

class CGMEInjAFRDlg : public CTabDialog
{
  typedef CTabDialog Super;
  typedef fastdelegate::FastDelegate0<> EventHandler;

 public:
  CGMEInjAFRDlg(CWnd* pParent = NULL);   // standard constructor
 ~CGMEInjAFRDlg();
  static const UINT IDD;

  void BindMaps(float* pAFR);
  void BindRPMGrid(float* pGrid);
  void BindLoadGrid(float* pGrid);
  void setOnChange(EventHandler OnCB);
  void UpdateView(void);

  void SetArguments(int rpm, int air_flow, bool strt_use);

 public:

 // Implementation
 protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  virtual BOOL OnInitDialog();
  afx_msg void OnUpdateControls(CCmdUI* pCmdUI);
  DECLARE_MESSAGE_MAP()

  virtual LPCTSTR GetDialogID(void) const;

 private:
  CMapEditorCtrl m_afr_map;
  CFont m_font;

  float* mp_AFRMap;
  float* mp_rpmGrid;
  float* mp_loadGrid;
};
