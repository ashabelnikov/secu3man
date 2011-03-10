/* Intel Hex to binary convertor
   Copyright (C) 2005 Alexey A. Shabelnikov. Ukraine, Gorlovka

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
          ICQ: 405-791-931. e-mail: shabelnikov-stc@mail.ru
          Microprocessors systems - design & programming.
*/

#pragma once

#ifndef __AFXWIN_H__
 #error include 'stdafx.h' before including this file for PCH
#endif

/////////////////////////////////////////////////////////////////////////////
// CHexUtilsApp
// See HexUtils.cpp for the implementation of this class
//

class CHexUtilsApp : public CWinApp
{
 public:
  CHexUtilsApp();

  DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
