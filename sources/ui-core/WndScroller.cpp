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

/** \file WndScroller.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include <algorithm>
#include "WndScroller.h"

#undef min //fucking stuff
#undef max

CWndScroller::CWndScroller()
: mp_origWnd(NULL)
, m_viewSize(CSize(0,0))
, m_pageSize(CSize(0,0))
, m_scrlPos(CSize(0,0))
, m_lineInc(50)
{
 //empty
}

CWndScroller::~CWndScroller()
{
 Close();
}

bool CWndScroller::Init(CWnd* ip_wnd)
{
 if (!ip_wnd)
 {
  ASSERT(0);
  return false;
 }

 mp_origWnd = ip_wnd;

 if (!Subclass(ip_wnd->m_hWnd)) //attach window
 {
  ASSERT(0);
  return false;
 }

 return true;
}

bool CWndScroller::Close(void)
{
 bool result = true;

 if (!Unsubclass()) //detach window
  result = false;

 mp_origWnd = NULL;

 return result;
}

void CWndScroller::SetViewSize(int width, int height)
{
 m_viewSize = CSize(width, height);
 if (!mp_origWnd) return;

 if (::IsWindow(mp_origWnd->m_hWnd))
  _UpdateScrollInfo();
}

const CSize& CWndScroller::GetScrollPos(void) const
{
 return m_scrlPos;
}

void CWndScroller::_OnHScroll(UINT nSBCode, UINT nPos, HWND hWnd)
{
 if (mp_origWnd == NULL) return;

 int delta = 0;
 switch(nSBCode)
 {
  case SB_THUMBTRACK:
   delta = _GetScrollPos(SB_HORZ, hWnd) - m_scrlPos.cx;
   break;
  case SB_THUMBPOSITION:
   delta = _GetScrollPos(SB_HORZ, hWnd) - m_scrlPos.cx;
   break;
  case SB_LINELEFT:   delta = -m_lineInc;      break;
  case SB_LINERIGHT:  delta = m_lineInc;       break;
  case SB_PAGELEFT:   delta = -m_pageSize.cx;  break;
  case SB_PAGERIGHT:  delta = m_pageSize.cx;   break;
  default: return;
 }

 //Calculate new scroll position and do scroll if necessary
 if (delta != 0)
 {
  int newPos = m_scrlPos.cx + delta;
  if (newPos < 0) delta = -m_scrlPos.cx;
  if (newPos > (m_viewSize.cx - m_pageSize.cx))
   delta = (m_viewSize.cx - m_pageSize.cx) - m_scrlPos.cx;
  m_scrlPos.cx += delta;
  mp_origWnd->SetScrollPos(SB_HORZ, m_scrlPos.cx, TRUE);
  mp_origWnd->ScrollWindow(-delta, 0);
 }
}

void CWndScroller::_OnVScroll(UINT nSBCode, UINT nPos, HWND hWnd)
{
 if (mp_origWnd == NULL) return;

 int delta = 0;
 switch(nSBCode)
 {
  case SB_THUMBTRACK:
   delta = _GetScrollPos(SB_VERT, hWnd) - m_scrlPos.cy;
   break;
  case SB_THUMBPOSITION:
   delta = _GetScrollPos(SB_VERT, hWnd) - m_scrlPos.cy;
   break;
  case SB_LINEUP:    delta = -m_lineInc;      break;
  case SB_LINEDOWN:  delta = m_lineInc;       break;
  case SB_PAGEUP:    delta = -m_pageSize.cy;  break;
  case SB_PAGEDOWN:  delta = m_pageSize.cy;   break;
  default: return;
 }

 //Calculate new scroll position and do scroll if necessary
 if (delta != 0)
 {
  int newPos = m_scrlPos.cy + delta;
  if (newPos < 0)  delta = -m_scrlPos.cy;
  if (newPos > (m_viewSize.cy - m_pageSize.cy))
   delta = (m_viewSize.cy - m_pageSize.cy) - m_scrlPos.cy;
  m_scrlPos.cy += delta;
  mp_origWnd->SetScrollPos(SB_VERT, m_scrlPos.cy, TRUE);
  mp_origWnd->ScrollWindow(0, -delta);
 }
}

bool CWndScroller::_OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
 if (mp_origWnd == NULL) return true;

 //if vertical scrollbar is not enabled than nothing to do
 int scrlMin = 0, scrlMax = 0;
 mp_origWnd->GetScrollRange(SB_VERT, &scrlMin, &scrlMax);
 if (scrlMin == scrlMax) return true;

 int scrlLnPerInc = 0, scrlIncNum = (abs(zDelta) / WHEEL_DELTA);
 ::SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &scrlLnPerInc, 0);

 //check if page scroll has requested.
 if (scrlLnPerInc == WHEEL_PAGESCROLL)
 {
  _OnVScroll(zDelta <= 0 ? SB_PAGEDOWN : SB_PAGEUP, 0, NULL);
  return true;
 }

 int numScrlLines = std::max((scrlIncNum * scrlLnPerInc) / 3, 1);
 for(int i = 0; i < numScrlLines; ++i)
  _OnVScroll(zDelta <= 0 ? SB_LINEDOWN : SB_LINEUP, 0, NULL);

 return true;
}

LRESULT CWndScroller::WndProcSub(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 if (lParam) //Do nothing if it is not standard scroll bar
  return CWindowSubClasser::WndProcSub(uMsg, wParam, lParam);

 switch(uMsg)
 {
  case WM_HSCROLL:
   _OnHScroll(LOWORD(wParam), HIWORD(wParam), reinterpret_cast<HWND>(lParam));
   break;

  case WM_VSCROLL:
   _OnVScroll(LOWORD(wParam), HIWORD(wParam), reinterpret_cast<HWND>(lParam));
   break;

  case WM_MOUSEWHEEL:
   _OnMouseWheel(LOWORD(wParam), HIWORD(wParam), CPoint(LOWORD(lParam), HIWORD(lParam)));
   break;

  case WM_SIZE:
   _UpdateScrollInfo();
   break;

  default:
   return CWindowSubClasser::WndProcSub(uMsg, wParam, lParam);
 }
 return 0;
}

void CWndScroller::_UpdateScrollBar(int bar, int wndSize, int viewSize, long& pageSize, long& scrlPos, long& delta)
{
 int scrlMax = 0;
 delta = 0;
 if (wndSize < viewSize)
 {
  scrlMax = viewSize - 1;
  pageSize = wndSize;
  if (scrlPos > 0 && pageSize > 0)
   scrlPos = (long)((wndSize * (float)(scrlPos))/ pageSize);
  scrlPos = std::min(scrlPos, viewSize - pageSize - 1);
  delta = mp_origWnd->GetScrollPos(bar) - scrlPos;
 }
 else
 { //scroll bar is not necessary
  scrlPos = 0, pageSize = 0;
  delta = mp_origWnd->GetScrollPos(bar);
 }

 SCROLLINFO si;
 memset(&si, 0, sizeof(SCROLLINFO));
 si.cbSize = sizeof(SCROLLINFO);
 si.fMask  = SIF_ALL; //SIF_ALL = SIF_PAGE | SIF_RANGE | SIF_POS;
 si.nMin   = 0;
 si.nMax   = scrlMax;
 si.nPos   = scrlPos;
 si.nPage  = pageSize;
 mp_origWnd->SetScrollInfo(bar, &si, TRUE);
}

void CWndScroller::_UpdateScrollInfo()
{
 CSize delta(0,0);
 CRect wndRect, rect;
 if (mp_origWnd == NULL) return;

 //get scroll bars' width and height
 int sx = ::GetSystemMetrics(SM_CXVSCROLL);
 int sy = ::GetSystemMetrics(SM_CYHSCROLL);

 mp_origWnd->GetWindowRect(&wndRect);
 mp_origWnd->ScreenToClient(&wndRect);
 mp_origWnd->GetClientRect(&rect);
 if (wndRect.right >= (rect.right + sx))
  rect.right += sx;
 if (wndRect.bottom >= (rect.bottom + sy))
  rect.bottom += sy;

 //update horizontal scrollbar
 _UpdateScrollBar(SB_HORZ, rect.Width(), m_viewSize.cx, m_pageSize.cx, m_scrlPos.cx, delta.cx);

 //update vertical scrollbar
 _UpdateScrollBar(SB_VERT, rect.Height(), m_viewSize.cy, m_pageSize.cy, m_scrlPos.cy, delta.cy);

 if (delta.cx != 0 || delta.cy != 0) mp_origWnd->ScrollWindow(delta.cx, delta.cy);
}

int CWndScroller::_GetScrollPos(int bar, HWND hWnd)
{
 SCROLLINFO si;
 si.cbSize = sizeof(SCROLLINFO);
 si.fMask = SIF_TRACKPOS;
 ::GetScrollInfo(NULL==hWnd?mp_origWnd->m_hWnd:hWnd, bar, &si);
 return si.nTrackPos;
}
