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

/** \file StatusBarEx.cpp
 * \author Alexey A. Shabelnikov
 */

#include "StdAfx.h"
#include <vector>
#include "StatusBarEx.h"

CStatusBarEx::PaneCtrlInfo::PaneCtrlInfo()
: m_hWnd(NULL)
, m_bAutoDel(TRUE)
{
 //empty
}

CStatusBarEx::Pane::Pane(const Pane& right)
{
 *this = right;
}

CStatusBarEx::Pane& CStatusBarEx::Pane::operator=(const Pane& right)
{
 cxText = right.cxText;
 nID  = right.nID;
 nStyle = right.nStyle;
 strText = right.strText;
 return *this;
}

CStatusBarEx::Pane::Pane()
{
 //empty
}

BEGIN_MESSAGE_MAP(CStatusBarEx, Super)
 ON_WM_SIZE()
END_MESSAGE_MAP()

CStatusBarEx::~CStatusBarEx()
{
 for(size_t i = 0; i < m_pans.size(); i++ )
 {
  if (m_pans[i]->m_hWnd && ::IsWindow(m_pans[i]->m_hWnd->m_hWnd))
   m_pans[i]->m_hWnd->CloseWindow();
  if (m_pans[i]->m_bAutoDel)
  {
   delete m_pans[i]->m_hWnd;
  }
  delete m_pans[i];
 }
}

void CStatusBarEx::OnSize(UINT nType, int cx, int cy)
{
 Super::OnSize(nType, cx, cy);
 _MoveControls();
}

void CStatusBarEx::_MoveControls()
{
 int h,v,s;
 GetStatusBarCtrl().GetBorders(h, v, s);

 for(size_t i = 0; i < m_pans.size(); ++i)
 {
  CRect rc;
  int index = CommandToIndex(m_pans[i]->m_nPaneID);
  GetItemRect(index, rc);
  if ((GetPaneStyle(index) & SBPS_NOBORDERS) == SBPS_NOBORDERS) {
   m_pans[i]->m_hWnd->MoveWindow(rc);
  }
  else
  {
   rc.top+=v, rc.bottom-=v;
   rc.left+=h+s, rc.right-=(h+s);
   m_pans[i]->m_hWnd->MoveWindow(rc);
  }
 }
}

CStatusBarEx::PaneCtrlInfo * CStatusBarEx::GetControl(int nPaneID)
{
 for(size_t i = 0; i < m_pans.size(); ++i)
 {
  if (m_pans[i]->m_nPaneID == nPaneID)
   return m_pans[i];
 }
 return NULL;
}

BOOL CStatusBarEx::AddControl(CWnd *pWnd, int paneID, BOOL bAutoDel)
{
 PaneCtrlInfo * pPanInfo = GetControl(paneID);
 if (pPanInfo)
  return FALSE;

 if (-1 != CommandToIndex(paneID))
 {
  PaneCtrlInfo * pPan = new PaneCtrlInfo;
  pPan->m_nPaneID =  paneID;
  pPan->m_hWnd    =  pWnd;
  pPan->m_bAutoDel = bAutoDel;

  m_pans.push_back(pPan);
  _MoveControls();
  Invalidate(TRUE);
  return TRUE;
 }

 return FALSE;
}

void CStatusBarEx::SetPaneWidth(int index, int Width)
{
 UINT nID, nStyle; int w;
 GetPaneInfo(index, nID, nStyle, w);
 SetPaneInfo(index, nID, nStyle, Width);
}


BOOL CStatusBarEx::_GetPane(int nIndex, Pane& pane)
{
 if (nIndex < m_nCount && nIndex >= 0)
 {
  GetPaneInfo(nIndex, pane.nID, pane.nStyle, pane.cxText);
  GetPaneText(nIndex, pane.strText);
  return TRUE;
 }
 return FALSE;
}

BOOL CStatusBarEx::AddIndicator(int position, UINT pnID)
{
 int i = 0;
 std::vector<Pane> panes; Pane pn, new_pn;

 while(_GetPane(i, pn))
 {
  panes.push_back(pn);
  i++;
 }

 if (position < 0) {
  position = 0;
 }
 if (position > (int)panes.size()) {
  position = panes.size() - 1;
 }

 //search for existing item
 for(i = 0; i < (int)panes.size(); i++)
 {
  if (pnID == panes[i].nID)
   return FALSE;
 }
 
 new_pn.nID = pnID;
 if (panes.size())
 {
  std::vector<Pane>::iterator pos = panes.begin() + position;
  panes.insert(pos, new_pn);
 }
 else
  panes.push_back(new_pn);

 UINT* pIndicators = new UINT[panes.size()];
 for(i = 0; i < (int)panes.size(); i++)
  pIndicators[i] = panes[i].nID;

 SetRedraw(FALSE);
 SetIndicators(pIndicators, panes.size());

 for(i = 0; i < (int)panes.size(); i++)
 {
  if(pnID != panes[i].nID)
  {
   SetPaneInfo(i, panes[i].nID, panes[i].nStyle, panes[i].cxText);
   SetPaneText(i, panes[i].strText);
  }
  else
  {
   SetPaneWidth(i, 50);
   SetPaneText(i, panes[i].strText);
  }
 }
 delete[] pIndicators;
 SetRedraw(TRUE);
 _MoveControls();
 Invalidate(TRUE);
 return TRUE;
}
