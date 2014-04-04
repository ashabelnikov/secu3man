/* SECU-3  - An open source, free engine control unit
   Copyright (C) 2007 Alexey A. Shabelnikov. Ukraine, Gorlovka

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

#pragma once

#include <memory>
#include "common/FastDelegate.h"
#include "common/UnicodeSupport.h"

class CEditExCustomKeys;

class CGridModeEditorDlg : public CDialog
{
  typedef CDialog Super;
  typedef fastdelegate::FastDelegate0<bool> EventResult;
  typedef fastdelegate::FastDelegate1<int> EventWithCode;
  typedef fastdelegate::FastDelegate2<HWND, int> EventWithHWND;

 public:
  CGridModeEditorDlg(CWnd* pParent = NULL);   // standard constructor
 ~CGridModeEditorDlg();
  static const UINT IDD;
  
  void BindMaps(float* pStart, float* pIdle, float* pWork, float* pTemp);
  void UpdateView(void);

  void EnableAdvanceAngleIndication(bool i_enable);

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
  afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd *pWnd, UINT nCtlColor);
  afx_msg void OnClose();
  DECLARE_MESSAGE_MAP()

  EventResult   m_IsAllowed;
  EventWithCode m_OnMapChanged;
  EventWithHWND m_OnOpenMapWnd;
  EventWithHWND m_OnCloseMapWnd;

  void OnEditChar(UINT nChar, CEditExCustomKeys*);
  void OnEditKill(CEditExCustomKeys*);
  bool _ValidateItem(CEditExCustomKeys* pItem);

 private:
  std::auto_ptr<CEditExCustomKeys> m_wrk_grid[16][16];
  CStatic m_wrk_map_labels[16];
  std::auto_ptr<CEditExCustomKeys> m_idl_grid[16];
  std::auto_ptr<CEditExCustomKeys> m_str_grid[16];
  std::auto_ptr<CEditExCustomKeys> m_tmp_grid[16];

  float* mp_startMap;
  float* mp_idleMap;
  float* mp_workMap;
  float* mp_tempMap;

  CBrush m_redBrush;
  bool m_closing_wnd;
  bool m_en_aa_indication;
};
