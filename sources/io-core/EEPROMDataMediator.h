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
              http://secu-3.narod.ru
              email: secu-3@yandex.ru
*/

#pragma once


//Аппаратно-независимая прослойка над данными EEPROM.

class AFX_EXT_CLASS EEPROMDataMediator
{
 public:
  EEPROMDataMediator();
 ~EEPROMDataMediator();
    
  static int GetDefParamsStartAddress(void);

  //Calculates CRC16 of default parameters for given buffer with EEPROM
  //ip_eeprom_buffer - contains EEPROM 
  //return true - OK, false - check sum is invalid
  static bool VerifyDefParamsCheckSum(const BYTE* ip_eeprom_buffer);
};
