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

/** \file HotKeysManager.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "HotKeysManager.h"
#include "HotKeysToCmdRouter.h"

#include <algorithm>

HotKeysManager::HotKeysManager()
: m_active(true)
{
 //empty
}

HotKeysManager::~HotKeysManager()
{
 //empty
}

HotKeysManager* HotKeysManager::GetInstance(void)
{
 static HotKeysManager hk_manager;
 return &hk_manager;
}

void HotKeysManager::_AddRouter(CHotKeysToCmdRouter* ip_router)
{
 if (std::find(m_hk_routers.begin(), m_hk_routers.end(),ip_router)!=m_hk_routers.end())
 {
  ASSERT(0);
  return;
 }
 m_hk_routers.push_back(ip_router);
}

void HotKeysManager::_RemoveRouter(CHotKeysToCmdRouter* ip_router)
{
 std::vector<CHotKeysToCmdRouter*>::iterator it = std::find(m_hk_routers.begin(), m_hk_routers.end(),ip_router);
 if (it==m_hk_routers.end())
 {
  ASSERT(0);
  return;
 }
 m_hk_routers.erase(it);
}


void HotKeysManager::DeactivateAllHotKeys(void)
{
 std::vector<CHotKeysToCmdRouter*>::iterator it = m_hk_routers.begin();
 for(; it != m_hk_routers.end(); ++it)
 {
  CHotKeysToCmdRouter* p_router = *it;
  ASSERT(p_router);

  if (p_router->mp_OriginalWnd)
   RegisterOneRouter(p_router, false);
 }
 m_active = false;
}

void HotKeysManager::ActivateAllHotKeys(void)
{
 std::vector<CHotKeysToCmdRouter*>::iterator it = m_hk_routers.begin();
 for(; it != m_hk_routers.end(); ++it)
 {
  CHotKeysToCmdRouter* p_router = *it;
  ASSERT(p_router);
  if (p_router->mp_OriginalWnd) //if window was not created, hot keys also would be not created!
   RegisterOneRouter(p_router, true);
 }
 m_active = true;
}

//Производит регистрацию/антиригистрацию всех горячих клавиш указанного роутера.
//i_register - если true, то регистрация, иначе антирегистрация
bool HotKeysManager::RegisterOneRouter(CHotKeysToCmdRouter* ip_router, bool i_register)
{
 bool result = true;
 bool is_command_exist = false;
 if (NULL==ip_router || NULL==ip_router->mp_OriginalWnd)
 {
  ASSERT(0);
  return false;
 }

 CHotKeysToCmdRouter::HotKeyMap::iterator it;
 for(it = ip_router->m_hot_key_map.begin(); it != ip_router->m_hot_key_map.end(); ++it)
 {
  if (!i_register)
  {
   if (UnregisterHotKey(ip_router->mp_OriginalWnd->m_hWnd, (*it).first))
    result = false;
  }
  else
  {
   BOOL status = RegisterHotKey(ip_router->mp_OriginalWnd->m_hWnd, (*it).first, (*it).second.m_fsModifiers, (*it).second.m_vk);
   result = status ? true : false;
  }
 }

 return result;
}

bool HotKeysManager::IsActive(void) const
{
 return m_active;
}
