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

/** \file FirmwareFileUtils.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

class CFirmwareDataMediator;

namespace FirmwareFileUtils { 

void SaveEEPROMToFile(const BYTE* p_data, const int size);
bool SaveFLASHToFile(const BYTE* p_data, const int size, CFirmwareDataMediator* p_fwdm, CString* o_file_name = NULL, bool calculate_and_place_crc16 = false);
bool LoadEEPROMFromFile(BYTE* p_data, const std::vector<int>& sizes, int* o_selected_size = NULL, _TSTRING* o_file_name = NULL);
bool LoadFLASHFromFile(BYTE* p_data, const std::vector<int>& sizes, _TSTRING* i_title = NULL, int* o_selected_size = NULL, _TSTRING* o_file_name = NULL, _TSTRING* o_file_path = NULL);

}