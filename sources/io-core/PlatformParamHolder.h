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

#include "iocore_api.h"
#include <vector>

#pragma once

struct PPFlashParam
{
 size_t m_page_size;       //размер страницы памяти программ в байтах
 size_t m_page_count;      //number of pages
 size_t m_total_size;      //общий размер памяти программ микроконтроллера
 size_t m_bl_section_size; //кол-во байт отведенное для бутлоадера (из секции бутлоадера этот блок можно читать)
 size_t m_app_section_size;//часть прошивки кроме бутлоадера
 size_t m_only_code_size;  //размер кода без данных, которые в конце
 //часть прошивки располагающаяся после кода программы и перед бутлоадером. Сюда входят пустое пространство, данные
 //и контрольная сумма прошивки.
 size_t m_only_overhead_size;
};

struct PPEepromParam
{
 size_t m_size;           //size of EEPROM
};

typedef enum EECUPlatform
{
 EP_ATMEGA16 = 0,
 EP_ATMEGA32,
 EP_ATMEGA64,
 EP_ATMEGA128,
 EP_NR_OF_PLATFORMS       //must be last!
};

class IOCORE_API PlatformParamHolder
{
 public:
  PlatformParamHolder(EECUPlatform i_platform);

  const PPEepromParam& GetEepromParameters(void) const;
  const PPFlashParam& GetFlashParameters(void) const;

  //get list of all allowed sizes of firmware
  static std::vector<int> GetFirmwareSizes(void);

  //get platform ID by firmware size
  static bool GetPlatformIdByFirmwareSize(int fwSize, EECUPlatform& o_platform);

  //get list of all allowed sizes of EEPROM
  static std::vector<int> GetEEPROMSizes(void);

  //get platform ID by EEPROM size
  static bool GetPlatformIdByEEPROMSize(int fwSize, EECUPlatform& o_platform);

 private:
  PPEepromParam m_ep;
  PPFlashParam m_fp;
};

