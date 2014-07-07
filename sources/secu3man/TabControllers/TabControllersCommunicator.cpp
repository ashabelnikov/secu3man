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

#include "stdafx.h"
#include "TabControllersCommunicator.h"
#include "TabsManagement/ITabController.h"

TabControllersCommunicator::TabControllersCommunicator()
{
 //todo
}

TabControllersCommunicator::~TabControllersCommunicator()
{
 //todo
}

/*static*/ TabControllersCommunicator* TabControllersCommunicator::GetInstance(void)
{
 static TabControllersCommunicator communicator;
 return &communicator;
}

void TabControllersCommunicator::AddReference(ITabController* ip_controller, int i_key)
{
 std::map<int, ITabController*>::const_iterator it = m_references.find(i_key);
 if (it==m_references.end())
  m_references.insert(std::make_pair(i_key, ip_controller));
 else
  ASSERT(0);
}

ITabController* TabControllersCommunicator::GetReference(int i_key)
{
 std::map<int, ITabController*>::const_iterator it = m_references.find(i_key);
 if (it!=m_references.end())
  return (*it).second;

 return NULL;
}
