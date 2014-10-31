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
#include "CRC16.h"
#include "EEPROMDataMediator.h"
#include "FirmwareMapsDataHolder.h"
#include "SECU3IO.h"
#include "SECU3ParametersDef.h"
#include "SECU3TablesDef.h"

using namespace SECU3IO;
using namespace SECU3IO::SECU3Types;

//Address of parameters structure in EEPROM (адрес структуры параметров в EEPROM)
#define EEPROM_PARAM_START 0x0001

//Address of errors's array (Check Engine) in EEPROM (адрес массива ошибок (Check Engine) в EEPROM)
#define EEPROM_ECUERRORS_START (EEPROM_PARAM_START + (sizeof(params_t)))

//Address of tables which can be edited in real time
#define EEPROM_REALTIME_TABLES_START (EEPROM_ECUERRORS_START + 5)


EEPROMDataMediator::EEPROMDataMediator(const PPEepromParam& i_epp)
: m_epp(i_epp)
, m_eeprom_size(i_epp.m_size)
, m_is_opened(false)
{
 m_bytes_active = new BYTE[m_epp.m_size + 1];
 ASSERT(m_bytes_active);
 m_bytes_original = new BYTE[m_epp.m_size + 1];
 ASSERT(m_bytes_original);

 memset(m_bytes_active, 0x00, m_eeprom_size);
 memset(m_bytes_original, 0x00, m_eeprom_size);
}

EEPROMDataMediator::~EEPROMDataMediator()
{
 delete m_bytes_active;
 delete m_bytes_original;
}

void EEPROMDataMediator::LoadBytes(const BYTE* i_bytes)
{
 ASSERT(i_bytes);
 memset(m_bytes_active, 0x00, m_eeprom_size);
 memset(m_bytes_original, 0x00, m_eeprom_size);

 memcpy(m_bytes_active, i_bytes, m_eeprom_size);
 memcpy(m_bytes_original, i_bytes, m_eeprom_size);

 m_is_opened = true;
}

bool EEPROMDataMediator::IsModified(void)
{
 return (0 != memcmp(m_bytes_active, m_bytes_original, m_eeprom_size));
}

bool EEPROMDataMediator::IsLoaded(void) 
{
 return m_is_opened;
}

void EEPROMDataMediator::ResetModified(void)
{
 memcpy(m_bytes_original, m_bytes_active, m_eeprom_size);
}

bool EEPROMDataMediator::VerifyParamsCheckSum(const BYTE* ip_eeprom_buffer)
{
 _uint crc = crc16(ip_eeprom_buffer + EEPROM_PARAM_START, sizeof(params_t) - sizeof(_uint));
 const _uint* crc_addr = (_uint*)(&ip_eeprom_buffer[EEPROM_PARAM_START + sizeof(params_t) - sizeof(_uint)]);
 return (*crc_addr)==crc; //check it!
}

size_t EEPROMDataMediator::GetParamsStartAddr(void) const
{
 return EEPROM_PARAM_START;
}

void EEPROMDataMediator::GetStartMap(int i_index, float* o_values, bool i_original/* = false*/)
{
 BYTE* p_bytes = NULL;
 f_data_t* p_maps = NULL;
 ASSERT(o_values);

 if (i_original)
  p_bytes = m_bytes_original;
 else
  p_bytes = m_bytes_active;

 //получаем адрес начала таблиц семейств характеристик
 p_maps = (f_data_t*)(p_bytes + EEPROM_REALTIME_TABLES_START);

 for (int i = 0; i < F_STR_POINTS; i++ )
  o_values[i] = ((float)p_maps[i_index].f_str[i]) / AA_MAPS_M_FACTOR;
}

void EEPROMDataMediator::GetIdleMap(int i_index,  float* o_values, bool i_original/* = false*/)
{
 BYTE* p_bytes = NULL;
 f_data_t* p_maps = NULL;
 ASSERT(o_values);

 if (i_original)
  p_bytes = m_bytes_original;
 else
  p_bytes = m_bytes_active;

 //получаем адрес начала таблиц семейств характеристик
 p_maps = (f_data_t*)(p_bytes + EEPROM_REALTIME_TABLES_START);

 for (int i = 0; i < F_IDL_POINTS; i++ )
  o_values[i] = ((float)p_maps[i_index].f_idl[i]) / AA_MAPS_M_FACTOR;
}

void EEPROMDataMediator::GetWorkMap(int i_index, float* o_values, bool i_original/* = false*/)
{
 BYTE* p_bytes = NULL;
 f_data_t* p_maps = NULL;
 ASSERT(o_values);

 if (i_original)
  p_bytes = m_bytes_original;
 else
  p_bytes = m_bytes_active;

 //получаем адрес начала таблиц семейств характеристик
 p_maps = (f_data_t*)(p_bytes + EEPROM_REALTIME_TABLES_START);

 for (int i = 0; i < (F_WRK_POINTS_F * F_WRK_POINTS_L); i++ )
 {
  _char *p = &(p_maps[i_index].f_wrk[0][0]);
  o_values[i] = ((float) *(p + i)) / AA_MAPS_M_FACTOR;
 }
}

void EEPROMDataMediator::GetTempMap(int i_index, float* o_values, bool i_original/* = false*/)
{
 BYTE* p_bytes = NULL;
 f_data_t* p_maps = NULL;
 ASSERT(o_values);

 if (i_original)
  p_bytes = m_bytes_original;
 else
  p_bytes = m_bytes_active;

 //получаем адрес начала таблиц семейств характеристик
 p_maps = (f_data_t*)(p_bytes + EEPROM_REALTIME_TABLES_START);

 for (int i = 0; i < F_TMP_POINTS; i++ )
  o_values[i] = ((float)p_maps[i_index].f_tmp[i]) / AA_MAPS_M_FACTOR;
}

std::vector<_TSTRING> EEPROMDataMediator::GetFunctionsSetNames(void)
{
 std::vector<_TSTRING> names(TUNABLE_TABLES_NUMBER);

 BYTE* p_bytes = NULL;
 f_data_t* p_maps = NULL;

 p_bytes = m_bytes_active;

 //получаем адрес начала таблиц семейств характеристик
 p_maps = (f_data_t*)(p_bytes + EEPROM_REALTIME_TABLES_START);

 for (int i = 0; i < TUNABLE_TABLES_NUMBER; i++)
 {
  char raw_string[256];
  memset(raw_string, 0, F_NAME_SIZE+1);
  BYTE* addr = p_maps[i].name;
  memcpy(raw_string, addr, F_NAME_SIZE);
  TCHAR string[256];
  OemToChar(raw_string,string);
  names[i] = string;
 }

 return names;
}

void EEPROMDataMediator::SetFunctionsSetName(int i_index, _TSTRING i_new_name)
{
 if (i_index >= TUNABLE_TABLES_NUMBER)
  return;

 BYTE* p_bytes = NULL;
 f_data_t* p_maps = NULL;

 p_bytes = m_bytes_active;

 //получаем адрес начала таблиц семейств характеристик
 p_maps = (f_data_t*)(p_bytes + EEPROM_REALTIME_TABLES_START);

 //дополняем строку пробелами, так чтобы если длина строки меньше F_NAME_SIZE,
 //остальные символы были пробелами
 i_new_name+= _T("                ");

 char raw_string[256];
 memset(raw_string, 0, F_NAME_SIZE + 1);
 CharToOem(i_new_name.c_str(), raw_string);
 memcpy(p_maps[i_index].name, raw_string, F_NAME_SIZE);
}
