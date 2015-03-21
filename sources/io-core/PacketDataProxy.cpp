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


PacketDataProxy::PacketDataProxy()
: m_mode(true) //default is Hex
{
 //empty
}

bool PacketDataProxy::Hex4ToBin(const BYTE*& ip_hex_number, BYTE* o_byte)
{
 if (m_mode)
 { //hex
  bool result = CNumericConv::Hex4ToBin(*ip_hex_number, o_byte);
  if (result)
   ip_hex_number+=1;
  return result;
 }
 else
 { //bin
  *o_byte = *ip_hex_number;
  ip_hex_number+=1;
  return true;
 }
}

bool PacketDataProxy::Hex8ToBin(const BYTE*& ip_hex_number, BYTE* o_byte)
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
  *o_byte = *ip_hex_number;
  ip_hex_number+=1;
  return true;
 }
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
 if (m_mode)
 { //hex
  bool result = CNumericConv::Hex16ToBin(ip_hex_number, o_word, i_signed);
  if (result)
   ip_hex_number+=4;
  return result;
 }
 else
 { //bin
  WORD word = MAKEWORD(*(ip_hex_number+1), *ip_hex_number);
  //если число знаковое и отрицательное то делаем его отрицательным
  *o_word = i_signed ? (int)((signed short)word) : word;
  ip_hex_number+=2;
  return true;
 }
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
