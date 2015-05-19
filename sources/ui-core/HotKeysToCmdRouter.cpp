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

/** \file HotKeysToCmdRouter.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "HotKeysToCmdRouter.h"
#include "HotKeysManager.h"
#include <map>

CHotKeysToCmdRouter::CHotKeysToCmdRouter()
: mp_OriginalWnd(NULL)
{
 DWORD id = GetCurrentThreadId();
 TCHAR string[256];
 _ultot(id, string, 16);
 m_thread_id_string = string;

 HotKeysManager::GetInstance()->_AddRouter(this);
}

CHotKeysToCmdRouter::~CHotKeysToCmdRouter()
{
 Close(); //удаляем и чистим все!
 HotKeysManager::GetInstance()->_RemoveRouter(this);
}

bool CHotKeysToCmdRouter::Close(void)
{
 bool result = true;

 if (!Unsubclass()) //отсоединяемся от окна
  result = false;

 //удаляем все зарегистрированные горячие клавиши.
 if (!UnregisterAllCommands())
  result = false;

 mp_OriginalWnd = NULL;

 return result;
}

bool CHotKeysToCmdRouter::Init(CWnd* ip_wnd)
{
 if (!ip_wnd)
 {
  ASSERT(0);
  return false;
 }

 mp_OriginalWnd = ip_wnd;

 if (!Subclass(ip_wnd->m_hWnd))
 {
  ASSERT(0);
  return false;
 }

 return true;
}

bool CHotKeysToCmdRouter::RegisterCommand(UINT i_command_id, UINT i_vk, UINT i_fsModifiers /* = 0*/)
{
 if (!mp_OriginalWnd)
 {
  ASSERT(0);
  return false;
 }

 //<ThreadID> - <Command ID> - <Virtual key> - <fsModifiers>
 _TSTRING atom_name;
 TCHAR string[256];
 atom_name = m_thread_id_string + _T("-") + _itot(i_command_id, string, 10) + _T("-") +
        _itot(i_vk, string, 16) + _T("-") + _itot(i_fsModifiers, string, 16);

 int nIDHotKey = GlobalAddAtom(atom_name.c_str());
 if (0==nIDHotKey)
 {
  ASSERT(0);
  return false;
 }

 BOOL result = RegisterHotKey(mp_OriginalWnd->m_hWnd, nIDHotKey, i_fsModifiers, i_vk);
 if (!result)
 {
  ASSERT(0);
  return false;
 }

 if (m_hot_key_map.find(nIDHotKey)==m_hot_key_map.end())
 {
  HotKeyInfo info;
  info.m_id_command = i_command_id;
  info.m_fsModifiers = i_fsModifiers;
  info.m_vk = i_vk;
  m_hot_key_map.insert(HotKeyMap::value_type(nIDHotKey, info));
 }

 return true;
}

CHotKeysToCmdRouter::HotKeyMap::iterator CHotKeysToCmdRouter::_FindCommandID(UINT i_command_id)
{
 for(HotKeyMap::iterator it = m_hot_key_map.begin(); it != m_hot_key_map.end(); ++it)
 {
  if ((*it).second.m_id_command == i_command_id)
   return it;
 }
 return m_hot_key_map.end();
}


bool CHotKeysToCmdRouter::UnregisterCommand(UINT i_command_id)
{
 bool result = true;
 bool is_command_exist = false;
 if (NULL==mp_OriginalWnd)
 {
  _ASSERTE(0);
  return false;
 }

 //ID-шек c кодом i_command_id в базе данных может быть больше одной, поэтому нам нужен цикл
 HotKeyMap::iterator it;
 while((it = _FindCommandID(i_command_id)) != m_hot_key_map.end())
 {
  is_command_exist = true;
  //нашли ID-шку в базе данных. Надо снять с регистрации горячую клавишу
  if (!UnregisterHotKey(mp_OriginalWnd->m_hWnd, (*it).first))
   result = false;
  if (GlobalDeleteAtom((*it).first) != 0)
   result = false;

  //удаляем из базы данных
  m_hot_key_map.erase(it);
 }

 //если были ошибки или команды нет в базе данных, то возвращаем false.
 return result && is_command_exist;
}

bool CHotKeysToCmdRouter::UnregisterAllCommands()
{
 bool result = true;

 while(!m_hot_key_map.empty())
 {
  if (!UnregisterCommand(m_hot_key_map.begin()->second.m_id_command))
   result = false;
 }

 return result;
}

LRESULT CHotKeysToCmdRouter::WndProcSub(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 switch(uMsg)
 {
  case WM_HOTKEY:
   if (m_hot_key_map.find(wParam)!=m_hot_key_map.end())
   {
    DWORD wnd_pid = 0, this_pid = ::GetCurrentProcessId();
    HWND hwnd = ::GetActiveWindow();
    ::GetWindowThreadProcessId(hwnd, &wnd_pid);
    //не посылаем сообщение если активно окно из другого процесса
    if(wnd_pid==this_pid)
    {
     //send message as it from menu
     mp_OriginalWnd->SendMessage(WM_COMMAND,MAKELONG(m_hot_key_map[wParam].m_id_command,0),NULL);
    }
   }
   else
    ASSERT(0); //unregistered hot key
   break;
 }

 return CWindowSubClasser::WndProcSub(uMsg, wParam, lParam);
}
