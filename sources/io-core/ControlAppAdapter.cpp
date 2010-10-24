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
              http://secu-3.narod.ru
              email: secu-3@yandex.ru
*/

#include "stdafx.h"
#include "ControlAppAdapter.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define WM_THREAD_ON_CONNECTON        (WM_USER + 1)
#define WM_THREAD_ON_PACKET_RECEIVED  (WM_USER + 2)


//////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CControlAppAdapter,CWnd)
  ON_MESSAGE(WM_THREAD_ON_CONNECTON, msgOnConnection)
  ON_MESSAGE(WM_THREAD_ON_PACKET_RECEIVED, msgOnPacketReceived)
END_MESSAGE_MAP()


//////////////////////////////////////////////////////////////////////
CControlAppAdapter::CControlAppAdapter() 
: m_switch_on(true)
, m_switch_on_thread_side(false)
{ 
 //na	  
}


CControlAppAdapter::~CControlAppAdapter() 
{
 //na
}

BOOL CControlAppAdapter::Create(CWnd* pParentWnd)
{
 ASSERT(pParentWnd);	
 if (::IsWindow(m_hWnd))
  return TRUE; //уже создано - ничего не делаем
 return CWnd::Create(NULL,_T("CControlApp_Adapter_Wnd"),0,CRect(0,0,0,0),pParentWnd,0);  
}

//////////////////////Thread side/////////////////////////////////////
void CControlAppAdapter::OnPacketReceived(const BYTE i_descriptor, SECU3IO::SECU3Packet* ip_packet)
{
  if (IsWindow(m_hWnd) && m_switch_on_thread_side)
    PostMessage(WM_THREAD_ON_PACKET_RECEIVED,i_descriptor,(LPARAM)ip_packet);	  
}

void CControlAppAdapter::OnConnection(const bool i_online)
{
  if (IsWindow(m_hWnd) && m_switch_on_thread_side)
    PostMessage(WM_THREAD_ON_CONNECTON,i_online,0);	  
}


/////////////////////Application side/////////////////////////////////
LRESULT CControlAppAdapter::msgOnConnection(WPARAM wParam, LPARAM lParam)
{
  //мы не должны "засыпать" получателя пакетами если он "выключил нас", а выключить он нас мог
  //еще тогда когда в очереди сообщений были сообщения для него.
  if (!m_switch_on_thread_side) 
    return 0;
  
 ObserversIterator it;
 for(it = m_observers.begin(); it != m_observers.end(); ++it)
 {
  IAPPEventHandler* p_observer = (*it).second; 
  _ASSERTE(p_observer);
  p_observer->OnConnection(wParam ? true : false);  
 }
 return 0;
}

LRESULT CControlAppAdapter::msgOnPacketReceived(WPARAM wParam, LPARAM lParam)
{
  if (!m_switch_on_thread_side) 
    return 0;
  
  //WARNING! object in stack!
  SECU3IO::SECU3Packet recepted_packet;

  ////////////////////////////////////////////////////////////////////////
  //эксклюзивный доступ, копирывание данных, а затем освобождение ресурса 
  EnterCriticalSection();
  memcpy(&recepted_packet,(SECU3IO::SECU3Packet*)lParam,sizeof(SECU3IO::SECU3Packet));
  LeaveCriticalSection();
  ////////////////////////////////////////////////////////////////////////

 ObserversIterator it;
 for(it = m_observers.begin(); it != m_observers.end(); ++it)
 {
  IAPPEventHandler* p_observer = (*it).second; 
  _ASSERTE(p_observer);
  p_observer->OnPacketReceived((BYTE)wParam, &recepted_packet); 
 }
 return 0;
}

//////////////////////////////////////////////////////////////////////
bool CControlAppAdapter::AddEventHandler(IAPPEventHandler* i_pEventHandler, const _TSTRING &i_observer_key)
{
  m_observers[i_observer_key] = i_pEventHandler;
  _UpdateInternalState();
  return true;
}

bool CControlAppAdapter::RemoveEventHandler(const _TSTRING &i_observer_key)
{
  if (m_observers.find(i_observer_key)==m_observers.end())
    return false; //неверный ключ
  m_observers.erase(i_observer_key);
  _UpdateInternalState();
  return true;
}

void CControlAppAdapter::SwitchOn(bool state)
{
  m_switch_on = state;
  _UpdateInternalState();
}

void CControlAppAdapter::_UpdateInternalState(void)
{
 m_switch_on_thread_side = m_switch_on && m_observers.size() > 0;
}
