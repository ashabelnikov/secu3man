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

/** \file ITabController.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

//через этот интерфейс главный контроллер управляет контроллерами вкладок (Tab control)
class ITabController
{
 public:
  virtual void OnActivate(void) = 0;
  virtual void OnDeactivate(void) = 0;
  virtual bool OnClose(void) = 0;
  virtual void OnFullScreen(bool i_what) = 0;
  virtual bool OnAskChangeTab(void) = 0;
  virtual void OnCloseNotify(void) = 0;
};
