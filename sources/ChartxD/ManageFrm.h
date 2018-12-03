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

/** \file ManageFrm.h
 * \author Alexey A. Shabelnikov
 */

#ifndef _MANAGEFRM_H_
#define _MANAGEFRM_H_

#include <Forms.hpp>

//---------------------------------------------------------------------------

bool RemoveInstanceByHWND(HWND hWnd);

bool AddInstanceByHWND(HWND hWnd, TForm* i_pForm);

TForm* GetInstanceByHWND(HWND hWnd);

void CleaupGarbage(void);

//---------------------------------------------------------------------------
#endif //_MANAGEFRM_H_
