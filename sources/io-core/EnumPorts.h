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

/** \file EnumPorts.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include <utility>
#include <vector>
#include "common/UnicodeSupport.h"
#include "iocore_api.h"

class ENUMPORTS_API CEnumPorts
{
 public:
 typedef std::pair<_TSTRING, _TSTRING> PortDescItem_t;
 typedef std::vector<std::pair<_TSTRING, _TSTRING> > PortDescList_t;
 
 CEnumPorts();
~CEnumPorts();

 bool Query(void);

 void getPortsList(PortDescList_t& portList); 

 private:
  bool _QueryPortList(const GUID& guid, DWORD flags);
  PortDescList_t m_portList;
};
