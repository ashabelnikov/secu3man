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

/** \file HotKeysToCmdRouter.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include "WndSubclasser.h"
#include "../common/unicodesupport.h"
#include <map>

class HotKeysManager;

//¬нимание! ќбъект должен создаватьс€ только с использованием heap!
class AFX_EXT_CLASS CHotKeysToCmdRouter : private CWndSubclasser
{
 public:
  CHotKeysToCmdRouter();
  virtual ~CHotKeysToCmdRouter();

  //Eсли Init() не вызван, то роутинг не осуществл€етс€.  аждому вызову Init() должен
  //соответствовать вызов Close().
  bool Init(CWnd* ip_wnd);

  //ќтсоединение от окна, удаление зарегистрированных гор€чих клавиш. Close() автоматически
  //вызываетс€ деструктором.
  bool Close(void);

  // i_command_id - ID команды (например IDM_OPEN_FILE)
  // i_vk - код гор€чей клавиши.
  // i_fsModifiers - Ќабор модификаторов определ€ющих клавиши которые должны быть нажаты вместе
  // с гор€чей клавишей. «начени€: MOD_ALT, MOD_CONTROL, MOD_SHIFT, MOD_WIN.
  // «а детальной информацией обращайтесь к описанию ф. RegisterHotKey() в MSDN.
  bool RegisterCommand(UINT i_command_id, UINT i_vk, UINT i_fsModifiers = 0);

  //”даление зарегистрированной гор€чей клавиши.
  //i_command_id - ID-шка команды дл€ сн€ти€ с регистрации. ≈сли зарегистрировано несколько
  //команд с одинаковыми ID-шками, то все они будут сн€ты с регистрации.
  bool UnregisterCommand(UINT i_command_id);

  //”даление (сн€тие с регистрации) всех зарегистрированных гор€чих клавиш.
  bool UnregisterAllCommands();

 private:
  friend HotKeysManager;

  //from CWndSubclasser
  virtual LRESULT WndProcSub(UINT uMsg, WPARAM wParam, LPARAM lParam);

  struct HotKeyInfo
  {
   UINT m_id_command; //ID-шка команды дл€ WM_COMMNAND
   UINT m_fsModifiers;
   UINT m_vk;
  };

  //<ID-шка гор€чей клавиши, info>
  typedef std::map<int, HotKeyInfo> HotKeyMap;
  HotKeyMap::iterator _FindCommandID(UINT i_command_id);

  //pointer to original window
  CWnd*   mp_OriginalWnd;

  //база данных ID-шек
  HotKeyMap m_hot_key_map;

  _TSTRING m_thread_id_string;
};
