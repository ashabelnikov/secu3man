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

/** \file StatusBarEx.h
 * \author Alexey A. Shabelnikov
 */

#pragma once
#include <vector>

class AFX_EXT_CLASS CStatusBarEx : public CStatusBar
{
  typedef CStatusBar Super;
 public:

  class PaneCtrlInfo
  {
   public:
    PaneCtrlInfo();

    CWnd* m_hWnd;
    int m_nPaneID;
    BOOL m_bAutoDel;
  };

  class Pane
  {
   public:
    Pane();
    Pane(const Pane& right);
    Pane& operator=(const Pane& right);

    int  cxText;
    UINT nID;
    UINT nStyle;
    UINT nFlags;
    CString strText;
  };

  ~CStatusBarEx();
  PaneCtrlInfo * GetControl(int nPaneID);
  BOOL AddControl(CWnd * pWnd, int nPaneID, BOOL bAutoDel = TRUE);
  void SetPaneWidth(int index, int Width);
  BOOL AddIndicator(int position, UINT pnID);

 protected:
  afx_msg void OnSize(UINT nType, int cx, int cy);
  DECLARE_MESSAGE_MAP()

 private:
  void _MoveControls();
  BOOL _GetPane(int nIndex, Pane& xfxpane);
  std::vector<PaneCtrlInfo*> m_pans;
};
