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

/** \file BootLoaderAdapter.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "BootLoaderAdapter.h"

#define WM_THREAD_ON_UPDATE_UI  (WM_USER + 1)
#define WM_THREAD_ON_BEGIN      (WM_USER + 2)
#define WM_THREAD_ON_END        (WM_USER + 3)

//////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CBootLoaderAdapter,CWnd)
 ON_MESSAGE(WM_THREAD_ON_UPDATE_UI, msgOnUpdateUI)
 ON_MESSAGE(WM_THREAD_ON_BEGIN, msgOnBegin)
 ON_MESSAGE(WM_THREAD_ON_END, msgOnEnd)
END_MESSAGE_MAP()

CBootLoaderAdapter::CBootLoaderAdapter()
: m_destination_handler(NULL)
{
 //empty
}

CBootLoaderAdapter::~CBootLoaderAdapter()
{
 //empty
}

BOOL CBootLoaderAdapter::Create(CWnd* pParentWnd)
{
 ASSERT(pParentWnd);
 if (::IsWindow(m_hWnd))
  return TRUE; //already created - do nothing
 return CWnd::Create(NULL,_T("CBootLoader_Adapter_Wnd"),0,CRect(0,0,0,0),pParentWnd,0);
}

//////////////////////Thread side////////////////////////////////////
void CBootLoaderAdapter::OnUpdateUI(IBLDEventHandler::poolUpdateUI* ip_data)
{
 if (IsWindow(m_hWnd))
 {
  PostMessage(WM_THREAD_ON_UPDATE_UI,(WPARAM)ip_data,0);
 }
}

void CBootLoaderAdapter::OnBegin(const int opcode,const int status)
{
 if (IsWindow(m_hWnd))
  PostMessage(WM_THREAD_ON_BEGIN,opcode,status);
}

void CBootLoaderAdapter::OnEnd(const int opcode,const int status)
{
 if (IsWindow(m_hWnd))
  PostMessage(WM_THREAD_ON_END,opcode,status);
}

/////////////////////Application side////////////////////////////////
LRESULT CBootLoaderAdapter::msgOnUpdateUI(WPARAM wParam, LPARAM lParam)
{
 poolUpdateUI* p_data = reinterpret_cast<poolUpdateUI*>(wParam);
 ASSERT(p_data);
 if (NULL==p_data)
  return 0; //what is this?
 if (m_destination_handler)
  m_destination_handler->OnUpdateUI(p_data);
 return 0;
}

LRESULT CBootLoaderAdapter::msgOnBegin(WPARAM wParam, LPARAM lParam)
{
 if (m_destination_handler)
  m_destination_handler->OnBegin(wParam,(int)lParam);
 return 0;
}

LRESULT CBootLoaderAdapter::msgOnEnd(WPARAM wParam, LPARAM lParam)
{
 if (m_destination_handler)
  m_destination_handler->OnEnd(wParam,(int)lParam);
 return 0;
}

//////////////////////////////////////////////////////////////////////

void CBootLoaderAdapter::SetEventHandler(IBLDEventHandler* ip_destination_handler)
{
 ASSERT(ip_destination_handler);
 m_destination_handler = ip_destination_handler;
}

void CBootLoaderAdapter::DetachEventHandler(void)
{
 m_destination_handler = NULL;
}
