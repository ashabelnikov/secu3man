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

/** \file InjDrvFileDataIO.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include <vector>
#include "io-core/SECU3IO.h"

class CInjDrvFileDataIO
{
 public:

 static bool ExportSet(SECU3IO::InjDrvPar* ip_set);

 static bool ImportSet(SECU3IO::InjDrvPar* op_set);

 static bool ImportSet(SECU3IO::InjDrvPar* op_set, BYTE* ip_buff, size_t size);

 static bool LoadSetsFromFirmware(SECU3IO::InjDrvPar* op_set);

 static bool SaveSetsToFirmware(SECU3IO::InjDrvPar* ip_set);

 static bool SaveFirmware(const std::vector<BYTE>& buffer);

 static bool LoadFirmware(std::vector<BYTE>& buffer);

 static bool SaveEEPROM(const std::vector<BYTE>& buffer, size_t size);

 static bool LoadEEPROM(std::vector<BYTE>& buffer, size_t size);

 static bool LoadSetsFromBuffer(SECU3IO::InjDrvPar* op_set, std::vector<BYTE>& i_buff);
};
