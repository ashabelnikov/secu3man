/* SECU-3  - An open source, free engine control unit
   Copyright (C) 2007 Alexey A. Shabelnikov. Ukraine, Gorlovka

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
#include "CRC16.h"
#include "EEPROMDataMediator.h"
#include "SECU3ParametersDef.h"

using namespace SECU3IO;
using namespace SECU3IO::SECU3Types;

#define EEPROM_PARAM_START (m_epp.m_param_start)

EEPROMDataMediator::EEPROMDataMediator(const PPEepromParam& i_epp)
: m_epp(i_epp)
{
 //empty
}

EEPROMDataMediator::~EEPROMDataMediator()
{
 //empty
}

bool EEPROMDataMediator::VerifyDefParamsCheckSum(const BYTE* ip_eeprom_buffer)
{
 _uint crc = crc16(ip_eeprom_buffer + EEPROM_PARAM_START, sizeof(params) - sizeof(_uint));
 const _uint* crc_addr = (_uint*)(&ip_eeprom_buffer[EEPROM_PARAM_START + sizeof(params) - sizeof(_uint)]);
 return (*crc_addr)==crc; //check it!
}
