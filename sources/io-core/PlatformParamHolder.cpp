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

/** \file PlatformParamHolder.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include <algorithm>
#include <string.h>
#include "PlatformParamHolder.h"


PlatformParamHolder::PlatformParamHolder(EECUPlatform i_platform)
{
 //initialize data depending on platform
 switch(i_platform)
 {
  case EP_ATMEGA16:
   //flash
   m_fp.m_page_size = 128;
   m_fp.m_total_size = 16384;
   m_fp.m_page_count = m_fp.m_total_size / m_fp.m_page_size;
   m_fp.m_bl_section_size = 512; //256 words
   m_fp.m_app_section_size = m_fp.m_total_size - m_fp.m_bl_section_size;
   //eeprom
   m_ep.m_size = 512;
   m_ep.m_platform_id = EP_ATMEGA16;
   //MCU clock frequency
   m_fp.m_fcpu_hz = 16000000;
   //Magic number
   strncpy(m_fp.m_magic, "16  ", PLATFORM_MN_SIZE);
   m_fp.m_platform_id = EP_ATMEGA16;
   break;

  case EP_ATMEGA32:
   m_fp.m_page_size = 128;
   m_fp.m_total_size = 32768;
   m_fp.m_page_count = m_fp.m_total_size / m_fp.m_page_size;
   m_fp.m_bl_section_size = 1024; //512 words
   m_fp.m_app_section_size = m_fp.m_total_size - m_fp.m_bl_section_size;
   //eeprom
   m_ep.m_size = 1024;
   m_ep.m_platform_id = EP_ATMEGA32;
   //MCU clock frequency
   m_fp.m_fcpu_hz = 16000000;
   //Magic number
   strncpy(m_fp.m_magic, "32  ", PLATFORM_MN_SIZE);
   m_fp.m_platform_id = EP_ATMEGA32;
   break;

   //ATmega644 and ATmega64 have same FLASH and EEPROM parameters
  case EP_ATMEGA644: 
   m_fp.m_page_size = 256;
   m_fp.m_total_size = 65536;
   m_fp.m_page_count = m_fp.m_total_size / m_fp.m_page_size;
   m_fp.m_bl_section_size = 2048; //1024 words
   m_fp.m_app_section_size = m_fp.m_total_size - m_fp.m_bl_section_size;
   //eeprom
   m_ep.m_size = 2048;
   m_ep.m_platform_id = EP_ATMEGA644;
   //MCU clock frequency
   m_fp.m_fcpu_hz = 20000000;
   //Magic number
   strncpy(m_fp.m_magic, "644 ", PLATFORM_MN_SIZE);
   m_fp.m_platform_id = EP_ATMEGA644;
   break;

  case EP_ATMEGA64:
   m_fp.m_page_size = 256;
   m_fp.m_total_size = 65536;
   m_fp.m_page_count = m_fp.m_total_size / m_fp.m_page_size;
   m_fp.m_bl_section_size = 2048; //1024 words
   m_fp.m_app_section_size = m_fp.m_total_size - m_fp.m_bl_section_size;
   //eeprom
   m_ep.m_size = 2048;
   m_ep.m_platform_id = EP_ATMEGA64;
   //MCU clock frequency
   m_fp.m_fcpu_hz = 16000000;
   //Magic number
   strncpy(m_fp.m_magic, "64  ", PLATFORM_MN_SIZE);
   m_fp.m_platform_id = EP_ATMEGA64;
   break;

  case EP_ATMEGA128:
   m_fp.m_page_size = 256;
   m_fp.m_total_size = 131072;
   m_fp.m_page_count = m_fp.m_total_size / m_fp.m_page_size;
   m_fp.m_bl_section_size = 2048; //1024 words
   m_fp.m_app_section_size = m_fp.m_total_size - m_fp.m_bl_section_size;
   //eeprom
   m_ep.m_size = 4096;
   m_ep.m_platform_id = EP_ATMEGA128;
   //MCU clock frequency
   m_fp.m_fcpu_hz = 16000000;
   //Magic number
   strncpy(m_fp.m_magic, "128 ", PLATFORM_MN_SIZE);
   m_fp.m_platform_id = EP_ATMEGA128;
   break;

  default:
   ASSERT(0);
   memset(&m_fp, 0, sizeof(PPFlashParam));
   memset(&m_ep, 0, sizeof(PPEepromParam));
   break;
 }
}

const PPEepromParam& PlatformParamHolder::GetEepromParameters(void) const
{
 return m_ep;
}

const PPFlashParam& PlatformParamHolder::GetFlashParameters(void) const
{
 return m_fp;
}

std::vector<int> PlatformParamHolder::GetFirmwareSizes(void)
{
 std::vector<int> sizes;
 for(size_t i = 0; i < EP_NR_OF_PLATFORMS; ++i)
 {
  PlatformParamHolder params((EECUPlatform)i);
  if (sizes.end()==std::find(sizes.begin(), sizes.end(), params.GetFlashParameters().m_total_size)) //already exists?
   sizes.push_back(params.GetFlashParameters().m_total_size);
 }
 return sizes;
}

bool PlatformParamHolder::GetPlatformIdByFirmwareSize(int fwSize, EECUPlatform& o_platform)
{
 for(size_t i = 0; i < EP_NR_OF_PLATFORMS; ++i)
 {
  PlatformParamHolder params((EECUPlatform)i);
  if (fwSize == params.GetFlashParameters().m_total_size)
  {
   o_platform = (EECUPlatform)i;
   return true;
  }
 }
 return false; //error
}

bool PlatformParamHolder::GetPlatformIdByFirmwareMagic(const BYTE* p_buff, int fwSize, EECUPlatform& o_platform)
{
 for(size_t i = 0; i < EP_NR_OF_PLATFORMS; ++i)
 {
  PlatformParamHolder params((EECUPlatform)i);
  if (0==strncmp(params.GetFlashParameters().m_magic, (const char*)(p_buff + fwSize - PLATFORM_MN_SIZE), PLATFORM_MN_SIZE))
  {
   o_platform = (EECUPlatform)i;
   return true;
  }
 }
 return false; //error
}

std::vector<int> PlatformParamHolder::GetEEPROMSizes(void)
{
 std::vector<int> sizes;
 for(size_t i = 0; i < EP_NR_OF_PLATFORMS; ++i)
 {
  PlatformParamHolder params((EECUPlatform)i);
  if (sizes.end()==std::find(sizes.begin(), sizes.end(), params.GetEepromParameters().m_size)) //already exists?
   sizes.push_back(params.GetEepromParameters().m_size);
 }
 return sizes;
}

bool PlatformParamHolder::GetPlatformIdByEEPROMSize(int fwSize, EECUPlatform& o_platform)
{
 for(size_t i = 0; i < EP_NR_OF_PLATFORMS; ++i)
 {
  PlatformParamHolder params((EECUPlatform)i);
  if (fwSize == params.GetEepromParameters().m_size)
  {
   o_platform = (EECUPlatform)i;
   return true;
  }
 }
 return false; //error
}

bool PlatformParamHolder::GetPlatformIdByEEPROMMagic(const BYTE* p_buff, int fwSize, EECUPlatform& o_platform)
{
 for(size_t i = 0; i < EP_NR_OF_PLATFORMS; ++i)
 {
  PlatformParamHolder params((EECUPlatform)i);
  if (0==strncmp(params.GetFlashParameters().m_magic, (const char*)(p_buff + fwSize - PLATFORM_MN_SIZE), PLATFORM_MN_SIZE))
  {
   o_platform = (EECUPlatform)i;
   return true;
  }
 }
 return false; //error
}
