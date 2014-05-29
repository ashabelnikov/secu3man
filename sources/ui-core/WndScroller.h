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
#include "WndSubclasser.h"

class AFX_EXT_CLASS CWndScroller : private CWndSubclasser
{
 public:
  //Note: this object must be allocated on heap!
  CWndScroller();
 ~CWndScroller();

  //Attach/Detach CWnd derived object. Each call of Init() must be followed by Close().
  //Close() automatically called in destructor
  bool  Init(CWnd* ip_wnd);
  bool  Close(void);

  //View size values determine how much to scroll
  void   SetViewSize(int width, int height);

  //Get current scroll position
  const CSize& GetScrollPos(void) const;

 private:
  //from CWndSubclasser
  virtual LRESULT WndProcSub(UINT uMsg, WPARAM wParam, LPARAM lParam);

  void   _OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
  void   _OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
  bool   _OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
  void   _UpdateScrollBar(int bar, int wndSize, int viewSize, long& pageSize, long& scrollPos, long& deltaPos);
  void   _UpdateScrollInfo(void);
  int    _GetScrollPos(int bar, CScrollBar* pScrollBar);

 private:
  CWnd*  mp_origWnd;
  CSize  m_viewSize;
  CSize  m_pageSize;
  CSize  m_scrlPos;
  const int m_lineInc;
};
