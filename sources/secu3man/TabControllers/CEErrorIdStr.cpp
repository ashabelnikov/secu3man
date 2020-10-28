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

/** \file FirmwareFileUtils.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include <algorithm>
#include "Resources/resource.h"
#include "CEErrorIdStr.h"
#include "io-core/ce_errors.h"

CEErrorIdStr::CEErrorIdStr()
{
 //наполняем карту которая будет содержать ошибки отображаемые в списке. Номер бита закодированной ошибки
 //одновременно выступает ключом в этой карте и ID-шкой для идентификации элементов списка.
 DWORD v = 0;
 m_errors_ids.insert(ErrorsIDContainer::value_type(ECUERROR_CKPS_MALFUNCTION, std::make_pair(MLL::GetString(IDS_ECUERROR_CKPS_MALFUNCTION), v)));
 m_errors_ids.insert(ErrorsIDContainer::value_type(ECUERROR_EEPROM_PARAM_BROKEN, std::make_pair(MLL::GetString(IDS_ECUERROR_EEPROM_PARAM_BROKEN), v)));
 m_errors_ids.insert(ErrorsIDContainer::value_type(ECUERROR_PROGRAM_CODE_BROKEN, std::make_pair(MLL::GetString(IDS_ECUERROR_PROGRAM_CODE_BROKEN), v)));
 m_errors_ids.insert(ErrorsIDContainer::value_type(ECUERROR_KSP_CHIP_FAILED, std::make_pair(MLL::GetString(IDS_ECUERROR_KSP_CHIP_FAILED), v)));
 m_errors_ids.insert(ErrorsIDContainer::value_type(ECUERROR_KNOCK_DETECTED, std::make_pair(MLL::GetString(IDS_ECUERROR_KNOCK_DETECTED), v)));
 m_errors_ids.insert(ErrorsIDContainer::value_type(ECUERROR_MAP_SENSOR_FAIL, std::make_pair(MLL::GetString(IDS_ECUERROR_MAP_SENSOR_FAIL), v)));
 m_errors_ids.insert(ErrorsIDContainer::value_type(ECUERROR_TEMP_SENSOR_FAIL, std::make_pair(MLL::GetString(IDS_ECUERROR_TEMP_SENSOR_FAIL), v)));
 m_errors_ids.insert(ErrorsIDContainer::value_type(ECUERROR_VOLT_SENSOR_FAIL, std::make_pair(MLL::GetString(IDS_ECUERROR_VOLT_SENSOR_FAIL), v)));
 m_errors_ids.insert(ErrorsIDContainer::value_type(ECUERROR_DWELL_CONTROL, std::make_pair(MLL::GetString(IDS_ECUERROR_DWELL_CONTROL), v)));
 m_errors_ids.insert(ErrorsIDContainer::value_type(ECUERROR_CAMS_MALFUNCTION, std::make_pair(MLL::GetString(IDS_ECUERROR_CAMS_MALFUNCTION), v))); 
 m_errors_ids.insert(ErrorsIDContainer::value_type(ECUERROR_TPS_SENSOR_FAIL, std::make_pair(MLL::GetString(IDS_ECUERROR_TPS_SENSOR_FAIL), v))); 
 m_errors_ids.insert(ErrorsIDContainer::value_type(ECUERROR_ADD_I1_SENSOR, std::make_pair(MLL::GetString(IDS_ECUERROR_ADD_I1_SENSOR), v))); 
 m_errors_ids.insert(ErrorsIDContainer::value_type(ECUERROR_ADD_I2_SENSOR, std::make_pair(MLL::GetString(IDS_ECUERROR_ADD_I2_SENSOR), v)));
 m_errors_ids.insert(ErrorsIDContainer::value_type(ECUERROR_ADD_I3_SENSOR, std::make_pair(MLL::GetString(IDS_ECUERROR_ADD_I3_SENSOR), v)));
 m_errors_ids.insert(ErrorsIDContainer::value_type(ECUERROR_ADD_I4_SENSOR, std::make_pair(MLL::GetString(IDS_ECUERROR_ADD_I4_SENSOR), v)));
 m_errors_ids.insert(ErrorsIDContainer::value_type(ECUERROR_SYS_START, std::make_pair(MLL::GetString(IDS_ECUERROR_SYS_START), v)));
 m_errors_ids.insert(ErrorsIDContainer::value_type(ECUERROR_ADD_I5_SENSOR, std::make_pair(MLL::GetString(IDS_ECUERROR_ADD_I5_SENSOR), v))); 
 m_errors_ids.insert(ErrorsIDContainer::value_type(ECUERROR_ADD_I6_SENSOR, std::make_pair(MLL::GetString(IDS_ECUERROR_ADD_I6_SENSOR), v)));
 m_errors_ids.insert(ErrorsIDContainer::value_type(ECUERROR_ADD_I7_SENSOR, std::make_pair(MLL::GetString(IDS_ECUERROR_ADD_I7_SENSOR), v)));
 m_errors_ids.insert(ErrorsIDContainer::value_type(ECUERROR_ADD_I8_SENSOR, std::make_pair(MLL::GetString(IDS_ECUERROR_ADD_I8_SENSOR), v)));
}

const CEErrorIdStr::ErrorsIDContainer& CEErrorIdStr::Get(void) const
{
 return m_errors_ids;
}

CEErrorIdStr::ErrorsIDContainer& CEErrorIdStr::Get(void)
{
 return m_errors_ids;
}
