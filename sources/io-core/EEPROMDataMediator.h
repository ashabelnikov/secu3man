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

#pragma once

#include "iocore_api.h"
#include "common/unicodesupport.h"
#include "PlatformParamHolder.h"

//Аппаратно-независимая прослойка над данными EEPROM.
class IOCORE_API EEPROMDataMediator
{
 public:
  EEPROMDataMediator(const PPEepromParam& i_epp);
 ~EEPROMDataMediator();

  //загрузка байтов EEPROM из указанного буфера
  void LoadBytes(const BYTE* i_bytes);

  //была ли прошивка изменена после последней загрузки
  bool IsModified(void);

  //возвращает true если прошивка загружена (есть что редактировать)
  bool IsLoaded(void);

  //вызывать эту функцию для сброса признака модификации, только
  //после того как данные были успешно сохранены.
  void ResetModified(void);

  //Calculates CRC16 of parameters for given buffer with EEPROM
  //ip_eeprom_buffer - contains EEPROM
  //return true - OK, false - check sum is invalid
  bool VerifyParamsCheckSum(const BYTE* ip_eeprom_buffer);

  //returns address of parameters in EEPROM
  size_t GetParamsStartAddr(void) const;

  void GetStartMap(int i_index, float* o_values, bool i_original = false);
  void GetIdleMap(int i_index,  float* o_values, bool i_original = false);
  void GetWorkMap(int i_index, float* o_values, bool i_original = false);
  void GetTempMap(int i_index, float* o_values, bool i_original = false);

  std::vector<_TSTRING> GetFunctionsSetNames(void);
  void SetFunctionsSetName(int i_index, _TSTRING i_new_name);

 private:
  PPEepromParam m_epp;
  const size_t m_eeprom_size;
  BYTE* m_bytes_active;   //байты EEPROM (копия для модификации)
  BYTE* m_bytes_original; //байты EEPROM (копия для сравнения)
  bool m_is_opened;
};
