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

/** \file MIFuelConsum.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "resource.h"
#include "MIFuelConsum.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMIFuelConsum::CMIFuelConsum()
{
 //empty
}

CMIFuelConsum::~CMIFuelConsum()
{
 //empty
}

void CMIFuelConsum::Create(CWnd* pParent)
{
 MeasInstrBase::Create(pParent, IDC_MI_FUELCONSUM); //create window

 m_meter.SetRange (0.0, 50.0);
 m_meter.SetLabelsDecimals(0);
 m_meter.SetValueDecimals(2);
 m_meter.SetTitle(MLL::LoadString(IDS_MI_FUELCONSUM_TITLE));
 m_meter.SetColor(meter_value,RGB(10,80,255));
 m_meter.SetColor(meter_bground, GetSysColor(COLOR_BTNFACE));
 m_meter.SetUnit(MLL::LoadString(IDS_MI_FUELCONSUM_UNIT));
 m_meter.SetTickNumber(25);
 m_meter.AddAlertZone(0,10,RGB(100,220,100));
 m_meter.AddAlertZone(10,20,RGB(180,180,100));
 m_meter.AddAlertZone(20,50,RGB(220,100,100));
 m_meter.SetNeedleValue(0.0);
 m_meter.Update();
}
