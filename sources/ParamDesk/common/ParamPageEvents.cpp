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

/** \file ParamPageEvents.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "ParamPageEvents.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ParamPageEvents::ParamPageEvents()
: m_OnChange(NULL)
{
 //empty
}

ParamPageEvents::~ParamPageEvents()
{
 //empty
}

void ParamPageEvents::setFunctionOnChange(EventHandler OnChange)
{
 m_OnChange = OnChange;
}

void ParamPageEvents::OnChangeNotify(void)
{
 if (m_OnChange)  //call only if handler present!
  m_OnChange();
}
