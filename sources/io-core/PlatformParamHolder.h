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

/** \file PlatformParamHolder.h
 * \author Alexey A. Shabelnikov
 */

#include "iocore_api.h"
#include <vector>

#pragma once

static const int PLATFORM_MN_SIZE = 4;

typedef enum EECUPlatform
{
 EP_ATMEGA16 = 0,
 EP_ATMEGA32,
 EP_ATMEGA64,
 EP_ATMEGA128,
 EP_ATMEGA644,            //redundant to EP_ATMEGA64 by firmware and EEPROM sizes
 EP_ATMEGA1284,           //redundant to EP_ATMEGA128 by firmware and EEPROM sizes
 EP_NR_OF_PLATFORMS,      //must be last!
 EP_ATMEGA328PB           //Inj. Driver
};

struct PPFlashParam
{
 size_t m_page_size;       //size of program memory page in bytes
 size_t m_page_count;      //number of pages
 size_t m_total_size;      //общий размер памяти программ микроконтроллера
 size_t m_bl_section_size; //кол-во байт отведенное для бутлоадера (из секции бутлоадера этот блок можно читать)
 size_t m_app_section_size;//part of firmware, except boot loader
 size_t m_fcpu_hz;         //MCU clock frequency (e.g. 16000000)
 char   m_magic[PLATFORM_MN_SIZE]; //Magic number identifying platform (holds last 4 bytes in FLASH)
 EECUPlatform m_platform_id;//platform ID
 TCHAR m_flAnnex[8];       //filename annex (e.g. "m644", "m1284" etc)
};

struct PPEepromParam
{
 size_t m_size;             //size of EEPROM
 EECUPlatform m_platform_id;//platform ID
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
  //get platform ID by magic number placed in last 4 bytes of firmware
  static bool GetPlatformIdByFirmwareMagic(const BYTE* p_buff, int fwSize, EECUPlatform& o_platform);

  //get list of all allowed sizes of EEPROM
  static std::vector<int> GetEEPROMSizes(void);

  //get platform ID by EEPROM size
  static bool GetPlatformIdByEEPROMSize(int fwSize, EECUPlatform& o_platform);
  //get platform ID by magic number placed in last 4 bytes of EEPROM
  static bool GetPlatformIdByEEPROMMagic(const BYTE* p_buff, int fwSize, EECUPlatform& o_platform);

  //returns quartz frequency in Hz
  static size_t GetQuartzFreq(EECUPlatform platid);

  //returns quartz frequency factor
  static float GetQuartzFact(EECUPlatform platid);

 private:
  PPEepromParam m_ep;
  PPFlashParam m_fp;
};

