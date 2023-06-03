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

/** \file LogPlayerTabDlg.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include <memory>
#include <vector>
#include "common/FastDelegate.h"
#include "ui-core/TabDialog.h"

class CCEDeskDlg;
class CLMDeskDlg;
class CLPControlPanelDlg;
class CMIDeskDlg;
class COscillCtrl;
class CToolTipCtrlEx;

class CLogPlayerTabDlg : public CTabDialog
{
  typedef CTabDialog Super;
  typedef fastdelegate::FastDelegate1<_TSTRING> EventString;

 public:
  CLogPlayerTabDlg();
  virtual LPCTSTR GetDialogID(void) const;

  //Drag and Drop event handler
  void setOnDropFile(EventString OnFunction);

  void EnableAll(bool i_enable);

  void SetSplitterPos(int pos);
  int GetSplitterPos(void) const;

 public:
  std::auto_ptr<CCEDeskDlg> mp_CEDeskDlg;
  std::auto_ptr<CLMDeskDlg> mp_LMDeskDlg;
  std::auto_ptr<CMIDeskDlg> mp_MIDeskDlg;
  std::auto_ptr<CLPControlPanelDlg> mp_LPPanelDlg;

 protected:
  void _ResizeRect(const CRect& i_external, CRect& io_victim);
  void _MoveSplitter(int x, int start_x);

  virtual BOOL OnInitDialog();
  afx_msg void OnSize(UINT nType, int cx, int cy);
  virtual void DoDataExchange(CDataExchange* pDX);
  afx_msg void OnDropFiles(HDROP hDropInfo);
  afx_msg void OnDestroy();
  afx_msg void OnUpdateControls(CCmdUI* pCmdUI);
  afx_msg void OnMouseMove(UINT nFlags, CPoint point);
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
  DECLARE_MESSAGE_MAP()

  EventString m_OnDropFile;

  std::auto_ptr<CToolTipCtrlEx> mp_ttc;
  bool m_all_enabled;
  bool m_initialized;

  //splitter:
  int m_splitterPos;
  bool m_moveSplitter;
  CPoint m_moveStart;
  int m_moveStrtWidthPD;
  CRect m_moveStrtRectMI;
  int m_miMargin;
};
