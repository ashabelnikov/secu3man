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

/** \file PacketDataProxy.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "NumericConv.h"
#include "PacketDataProxy.h"
#include "crc16.h"


PacketDataProxy::PacketDataProxy()
: m_mode(true) //default is Hex
, m_crc(0xFFFF)
{
 //empty
}

bool PacketDataProxy::Hex4ToBin(const BYTE*& ip_hex_number, BYTE* o_byte)
{
 if (m_mode)
 { //hex
  bool result = CNumericConv::Hex4ToBin(*ip_hex_number, o_byte);
  if (result)
   ip_hex_number+=1; //takes 1 byte
  return result;
 }
 else
 { //bin
  *o_byte = *ip_hex_number;
  ip_hex_number+=1;  //takes 1 byte
  return true;
 }
}

bool PacketDataProxy::Hex8ToBin(const BYTE*& ip_hex_number, BYTE* o_byte)
{
 bool result = false;
 if (m_mode)
 { //hex
  result = CNumericConv::Hex8ToBin(ip_hex_number, o_byte);
  if (result)
   ip_hex_number+=2;
 }
 else
 { //bin
  *o_byte = *ip_hex_number;
  ip_hex_number+=1;
  result = true;
 }

 m_crc = crc16(m_crc, o_byte, 1);
 return result;
}

bool PacketDataProxy::Hex8ToBin(const BYTE*& ip_hex_number, int* o_byte) //signed
{
 if (m_mode)
 { //hex
  bool result = CNumericConv::Hex8ToBin(ip_hex_number, o_byte);
  if (result)
   ip_hex_number+=2;
  return result;
 }
 else
 { //bin
  *o_byte = (int)((signed char)(*ip_hex_number));
  ip_hex_number+=1;
  return true;
 }
}

bool PacketDataProxy::Hex16ToBin(const BYTE*& ip_hex_number, int* o_word, bool i_signed /*= false*/)
{
 bool result = false;
 if (m_mode)
 { //hex
  result = CNumericConv::Hex16ToBin(ip_hex_number, o_word, i_signed);
  if (result)
   ip_hex_number+=4;
 }
 else
 { //bin
  WORD word = MAKEWORD(*(ip_hex_number+1), *ip_hex_number);
  //если число знаковое и отрицательное то делаем его отрицательным
  *o_word = i_signed ? (int)((signed short)word) : word;
  ip_hex_number+=2;
  result = true;
 }

 if (i_signed)
 {
  signed short value = *o_word;
  m_crc = crc16(m_crc, (unsigned char*)&value, 2);
 }
 else
 {
  unsigned short value = *o_word;
  m_crc = crc16(m_crc, (unsigned char*)&value, 2);  
 }

 return result;
}

bool PacketDataProxy::Hex32ToBin(const BYTE*& ip_hex_number, signed long *o_dword)
{
 if (m_mode)
 { //hex
  bool result = CNumericConv::Hex32ToBin(ip_hex_number, o_dword);
  if (result)
   ip_hex_number+=8;
  return result;
 }
 else
 { //bin
  WORD hi = MAKEWORD(*(ip_hex_number+1), *(ip_hex_number+0));
  WORD lo = MAKEWORD(*(ip_hex_number+3), *(ip_hex_number+2));
  *o_dword = MAKELONG(lo, hi);
  ip_hex_number+=4;
  return true;
 }
}

bool PacketDataProxy::Hex32ToBin(const BYTE*& ip_hex_number, unsigned long *o_dword)
{
 if (m_mode)
 { //hex
  bool result = CNumericConv::Hex32ToBin(ip_hex_number, o_dword);
  if (result)
   ip_hex_number+=8;
  return result;
 }
 else
 { //bin
  WORD hi = MAKEWORD(*(ip_hex_number+1), *(ip_hex_number+0));
  WORD lo = MAKEWORD(*(ip_hex_number+3), *(ip_hex_number+2));
  *o_dword = MAKELONG(lo, hi);
  ip_hex_number+=4;
  return true;
 }
}

bool PacketDataProxy::Hex24ToBin(const BYTE*& ip_hex_number, unsigned long *o_dword)
{
 if (m_mode)
 { //hex
  bool result = CNumericConv::Hex24ToBin(ip_hex_number, o_dword);
  if (result)
   ip_hex_number+=6;
  return result;
 }
 else
 { //bin
  WORD hi = MAKEWORD(*(ip_hex_number+0), 0);
  WORD lo = MAKEWORD(*(ip_hex_number+2), *(ip_hex_number+1));
  *o_dword = MAKELONG(lo, hi);
  ip_hex_number+=3;
  return true;
 }
}

bool PacketDataProxy::HexStrToBin(const BYTE*& ip_hex_str, int size, char *o_str)
{
 if (m_mode)
 { //hex
  strncpy(o_str, (const char*)ip_hex_str, size);
  o_str[size] = 0;
  ip_hex_str+=size;
  return true;
 }
 else
 { //bin
  strncpy(o_str, (const char*)ip_hex_str, size);
  o_str[size] = 0;
  ip_hex_str+=size;
  return true;
 }
}

bool PacketDataProxy::Bin4ToHex(const BYTE i_byte, std::vector<BYTE>& o_hex_number)
{
 if (m_mode)
 { //hex
  return CNumericConv::Bin4ToHex(i_byte, o_hex_number);
 }
 else
 { //bin
  o_hex_number.push_back(i_byte);
  return true;
 }
}

//bool PacketDataProxy::Bin8ToHex(const BYTE i_byte, BYTE* o_hex_number)
//{}

bool PacketDataProxy::Bin8ToHex(const BYTE i_byte, std::vector<BYTE>& o_hex_number)
{
 m_crc = crc16(m_crc, (unsigned char*)&i_byte, 1);
 if (m_mode)
 { //hex
  return CNumericConv::Bin8ToHex(i_byte, o_hex_number);
 }
 else
 { //bin
  o_hex_number.push_back(i_byte);
  return true;
 }
}

//bool PacketDataProxy::Bin12ToHex(const int i_12bit, BYTE* o_hex_number)
//{}

//bool PacketDataProxy::Bin16ToHex(const int i_word, BYTE* o_hex_number)
//{}

bool PacketDataProxy::Bin16ToHex(const int i_word, std::vector<BYTE>& o_hex_number)
{
 m_crc = crc16(m_crc, (unsigned char*)&i_word, 2);
 if (m_mode)
 { //hex
  return CNumericConv::Bin16ToHex(i_word, o_hex_number);
 }
 else
 { //bin
  o_hex_number.push_back(HIBYTE(i_word));
  o_hex_number.push_back(LOBYTE(i_word));
  return true;
 }
}

bool PacketDataProxy::Bin32ToHex(const unsigned long i_dword, std::vector<BYTE>& o_hex_number)
{
 if (m_mode)
 { //hex
  return CNumericConv::Bin32ToHex(i_dword, o_hex_number);
 }
 else
 { //bin
  o_hex_number.push_back(HIBYTE(HIWORD(i_dword)));
  o_hex_number.push_back(LOBYTE(HIWORD(i_dword)));
  o_hex_number.push_back(HIBYTE(LOWORD(i_dword)));
  o_hex_number.push_back(LOBYTE(LOWORD(i_dword)));
  return true;
 }
}

bool PacketDataProxy::Bin32ToHex(const signed long i_dword, std::vector<BYTE>& o_hex_number)
{
 return Bin32ToHex((const unsigned long)i_dword, o_hex_number);
}

size_t PacketDataProxy::getHex8Size(void) const
{
 return m_mode ? 2 : 1;
}

bool PacketDataProxy::isHex(void) const
{
 return m_mode; //true - Hex, false - Bin
}

void PacketDataProxy::SetMode(bool i_mode)
{
 m_mode = i_mode;
}

void PacketDataProxy::resetCRC(void)
{
 m_crc = 0xFFFF;
}

int PacketDataProxy::getCRC(void)
{
 return m_crc;
}
