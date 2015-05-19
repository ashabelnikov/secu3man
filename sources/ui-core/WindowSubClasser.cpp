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

/** \file WindowSubClasser.cpp
 * \author Alexey A. Shabelnikov
 */

#include "StdAfx.h"
#include <list>
#include <map>
#include "WindowSubClasser.h"

//-----------------------------------------------------------
namespace
{
 //Describes one subclassing node
 struct WndSubClasserNode
 {
  LPVOID m_prev_node;
  WPARAM m_wParam;
  LPARAM m_lParam;
  HWND m_hwnd;
  UINT m_uMsg;
  std::list<CWindowSubClasser*>::iterator m_it;
  std::list<CWindowSubClasser*>::iterator m_end;
 };

 std::map<HWND, WNDPROC> g_wndproc_map;
 std::map<HWND, std::list<CWindowSubClasser*> > g_list_map; //global map of instances

 LRESULT CALLBACK NewWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
 {
  std::map<HWND, std::list<CWindowSubClasser*> >::iterator mi = g_list_map.find(hwnd);
  ASSERT(mi != g_list_map.end());

  static CCriticalSection csect;
  CSingleLock lock(&csect, true);

  //Fill node data before calling WndProc
  WndSubClasserNode node;
  node.m_prev_node = NULL;
  node.m_wParam = wParam;
  node.m_lParam = lParam;
  node.m_hwnd = hwnd;
  node.m_uMsg = uMsg;
  node.m_it = mi->second.begin();
  node.m_end = mi->second.end();

  return CWindowSubClasser::CallWndProcSub(&node);
 }
}

//-----------------------------------------------------------
CWindowSubClasser::CWindowSubClasser()
: m_release(false)
, mp_node(NULL)
, m_hwnd(NULL)
{
 //empty
}

CWindowSubClasser::~CWindowSubClasser(void)
{
 Unsubclass(); //clear
}

bool CWindowSubClasser::Subclass(HWND hwnd)
{
 if(m_hwnd || !hwnd) {
  return false;
 }

 m_hwnd = hwnd;
 m_release = false;
 std::list<CWindowSubClasser*>& sub_list = g_list_map[hwnd];
 sub_list.push_front(this);

 WNDPROC& old_wndproc = g_wndproc_map[hwnd];
 if(!old_wndproc)
 {
  old_wndproc = reinterpret_cast<WNDPROC>(GetWindowLong(hwnd, GWL_WNDPROC));
  SetWindowLong(hwnd, GWL_WNDPROC, reinterpret_cast<LONG>(NewWindowProc));
 }

 return true; //Ok
}

bool CWindowSubClasser::Unsubclass()
{
 if(m_hwnd)
 {
  std::list<CWindowSubClasser*>& sub_list = g_list_map[m_hwnd];
  sub_list.remove(this);

  if(sub_list.empty())
  {
   g_list_map.erase(m_hwnd);
   SetWindowLong(m_hwnd, GWL_WNDPROC, reinterpret_cast<LONG>(g_wndproc_map[m_hwnd]));
   g_wndproc_map.erase(m_hwnd);
  }

  m_release = true;
  m_hwnd = NULL;
  return true;  //Ok
 }

 return false;
}

LRESULT CWindowSubClasser::WndProcSub(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 return CallWndProcSub(mp_node);
}

LRESULT CWindowSubClasser::CallWndProcSub(LPVOID ip_node)
{
 WndSubClasserNode* p_node = reinterpret_cast<WndSubClasserNode*>(ip_node);

 if(p_node->m_it == p_node->m_end) {
  return CallWindowProc(g_wndproc_map[p_node->m_hwnd], p_node->m_hwnd, p_node->m_uMsg, p_node->m_wParam, p_node->m_lParam);
 }

 CWindowSubClasser* p_sub = *p_node->m_it;
 p_node->m_it++; 
 p_node->m_prev_node = p_sub->mp_node;
 p_sub->mp_node = ip_node;

 if(WM_NCDESTROY == p_node->m_uMsg) {
  p_sub->m_release = true;
 }

 LRESULT result = p_sub->m_release ? CallWndProcSub(p_sub->mp_node) : p_sub->WndProcSub(p_node->m_uMsg, p_node->m_wParam, p_node->m_lParam);
 p_sub->mp_node = p_node->m_prev_node;
 return result;
}
