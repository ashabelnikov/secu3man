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

class PacketDataProxy
{
 public:
  PacketDataProxy();

  //Hex --> Bin
  bool Hex4ToBin(const BYTE*& ip_hex_number, BYTE* o_byte);
  bool Hex8ToBin(const BYTE*& ip_hex_number, BYTE* o_byte);
  bool Hex8ToBin(const BYTE*& ip_hex_number, int* o_byte); //signed
  bool Hex16ToBin(const BYTE*& ip_hex_number, int* o_word, bool i_signed  = false);
  bool Hex32ToBin(const BYTE*& ip_hex_number, signed long *o_dword); //signed
  bool Hex32ToBin(const BYTE*& ip_hex_number, unsigned long *o_dword);
  bool Hex24ToBin(const BYTE*& ip_hex_number, unsigned long *o_dword);

  //Bin --> Hex
  bool Bin4ToHex(const BYTE i_byte, std::vector<BYTE>& o_hex_number);
  bool Bin8ToHex(const BYTE i_byte, std::vector<BYTE>& o_hex_number);
  bool Bin16ToHex(const int i_word, std::vector<BYTE>& o_hex_number);
  bool Bin32ToHex(const unsigned long i_dword, std::vector<BYTE>& o_hex_number);
  bool Bin32ToHex(const signed long i_dword, std::vector<BYTE>& o_hex_number);

  //Get size of one byte in protocol
  size_t getHex8Size(void) const;

  //Returns current mode
  bool isHex(void) const;

  //Sets proxy mode
  //true - Hex, false - Bin
  void SetMode(bool i_mode);

 private:
  bool m_mode; //operation mode: false - binary, true - hex
};
