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

#pragma once

#include <vector>

class CHotKeysToCmdRouter;

class AFX_EXT_CLASS HotKeysManager
{
  HotKeysManager(); //закрытый конструктор!

 public:
  ~HotKeysManager();

  static HotKeysManager* GetInstance(void);

  void DeactivateAllHotKeys(void);
  void ActivateAllHotKeys(void);

  bool IsActive(void) const;

 private:
  friend CHotKeysToCmdRouter;
  void _AddRouter(CHotKeysToCmdRouter* ip_router);
  void _RemoveRouter(CHotKeysToCmdRouter* ip_router);

  bool RegisterOneRouter(CHotKeysToCmdRouter* ip_router, bool i_register);

  std::vector<CHotKeysToCmdRouter*> m_hk_routers;

  bool m_active;
};
