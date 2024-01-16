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

/** \file NumericConv.h
 * \author Alexey A. Shabelnikov
 */

#include <vector>
#include "iocore_api.h"
#include "types.h"

#ifndef _NUMERICCONV_
#define _NUMERICCONV_

#define INTP10_SIZE 10

class IOCORE_API CNumericConv
{
 public:
  inline static BYTE HTOD(BYTE h)
  {
   return ((h<0x3A)?h-'0':h-'A'+10);
  }

  inline static BYTE DTOH(BYTE d)
  {
   char hdig[] = "0123456789ABCDEF";
   return hdig[(d & 0x0F)];
  }

  static bool   HexArrayToBin(const BYTE* i_buf, BYTE* o_buf, const int size);
  static bool   Hex4ToBin(const BYTE i_hex_number,BYTE* o_byte);
  static bool   Hex8ToBin(const BYTE* i_hex_number,BYTE* o_byte);
  static bool   Hex8ToBin(const BYTE* i_hex_number,int* o_byte); //signed
  static bool   Hex16ToBin(const BYTE* i_hex_number,int* o_word,bool i_signed  = false);
  static bool   Hex32ToBin(const BYTE* i_hex_number,signed long *o_dword);
  static bool   Hex32ToBin(const BYTE* i_hex_number,unsigned long *o_dword);
  static bool   Hex24ToBin(const BYTE* i_hex_number,unsigned long *o_dword);

  static bool   BinToHexArray(const BYTE* i_buf, BYTE* o_buf, const int size);
  static bool   Bin4ToHex(const BYTE i_byte, std::vector<BYTE>& o_hex_number);
  static bool   Bin8ToHex(const BYTE i_byte, BYTE* o_hex_number);
  static bool   Bin8ToHex(const BYTE i_byte, std::vector<BYTE>& o_hex_number);
  static bool   Bin12ToHex(const int i_12bit, BYTE* o_hex_number);
  static bool   Bin16ToHex(const int i_word, BYTE* o_hex_number);
  static bool   Bin16ToHex(const int i_word, std::vector<BYTE>& o_hex_number);
  static bool   Bin32ToHex(const unsigned long i_dword, std::vector<BYTE>& o_hex_number);
  static bool   Bin32ToHex(const signed long i_dword, std::vector<BYTE>& o_hex_number);

  static BYTE   CheckSum_8_xor(const BYTE* i_buf,const int size);

  static inline SetDecimalPoint(char decpt) { m_decpt = decpt; };

  static inline bool secu3_atoi_u1(const char *str, int size, unsigned int& result)
  {
   if (size != 1 || str[0] < '0' || str[0] > '9')
    return false;
   result = str[0] - '0';
   return true;
  }

  template <int maxDigits>
  static bool secu3_atoi_u32(const char *str, int size, unsigned int& result)
  {
   if (size < 1 || size > maxDigits)
    return false;
   int sum = 0;
   while (size--)
   {
    if ((*str) < '0' || (*str) > '9')
     return false;
    sum = (sum * 10) + ((*str) - '0');
    str++;
   }
   result = sum;
   return true;
  }

  template <int maxDigits>
  static bool secu3_atoi_32(const char *str, int size, signed int& result)
  {
   if (size < 1)
    return false;

   signed int m;
   if (*str == '-')
   {
    if (size < 2 || --size > maxDigits)
     return false;
    m = -1;
    ++str;
   }
   else
   {
    if (size > maxDigits)
     return false;
    m = 1;
   }

   int sum = 0;
   while (size--)
   {
    if ((*str) < '0' || (*str) > '9')
     return false;
    sum = (sum * 10) + ((*str) - '0');
    str++;
   }
   result = sum * m;
   return true;
  }

  template <int maxDigits>
  static bool secu3_atof_32(const char *str, int size, float& result)
  {
   if (0 == size)
    return false; //at least one digit must exist

   signed int m;
   if (*str == '-')
   { //negative
    --size;  //skip sign character
    ++str;
    if (0 == size)
     return false; //at least one digit must exist
    m = -1;
   }
   else
   { //positive
    m = 1;
   }

   if (size > maxDigits)
    return false; //number of digits must not exceed limit

   const char* end = str + size;

   signed int intpart = 0;
   while (str != end)
   {
    if ((*str) == m_decpt)
    {
     ++str;
     size = end - str; //calculate size of fractional part
     break;
    }
    if ((*str) < '0' || (*str) > '9')
     return false; //error, wrong character
    intpart = (intpart * 10) + ((*str) - '0');
    ++str;
   }
   intpart*= m; //apply sign

   if (0==size)
   {
    result = (float)intpart;
    return true; //ok, no fractional part
   }

   signed int fracpart = 0;
   while (str != end)
   {
    if ((*str) < '0' || (*str) > '9')
     return false; //error, wrong character
    fracpart = (fracpart * 10) + ((*str) - '0');
    ++str;
   }

   if (intpart <= 0)
    fracpart = -fracpart; //copy sign

   result = intpart + (fracpart * scales[size]);
   return true; //ok
  }

  template <int size>
  static int secu3_ftoa_32(char* str, float value, unsigned int decplaces)
  {
   int c = 0, tens = 0;
   int negative = 0;
   float d = (value < 0) ? -.5f : .5f;
   d*= neg_p10[decplaces];

   if (value < 0)
   {
    value = -(value + d);
    ++negative;
   }
   else
   {
    value = (value + d);
   }

   while(pos_p10[1 + tens++] <= value);

   int sz = negative + tens + decplaces;
   if (0 == tens)
    sz++;
   if (decplaces > 0)
    sz++;

   //fill left side with spaces to achieve required size
   while(sz < size) str[c++] = ' ', ++sz;

   if (negative)
    str[c++] = '-';

   if (0 == tens)
    str[c++] = '0';

   //process integer part
   for (int i = 0; i < tens; ++i)
   {
    int idx = tens - i;
    int digit = (int)(value * neg_p10[idx - 1]);
    str[c++] = '0' + digit;
    value-= ((float)digit * pos_p10[idx - 1]);
   }

   if (decplaces > 0)
    str[c++] = m_decpt;

   //process fractional part
   for (unsigned int i = 0; i < decplaces; ++i)
   {
    value*= 10.0f;
    int digit = (int)value;
    str[c++] = '0' + digit;
    value-= (float)digit;
   }

   str[c] = 0; //terminate string
   return c;
  }

  template <int size>
  static int secu3_itoa_u1(char* str, unsigned int value)
  {
   int start = 0, c = 0, i = size;
   while(--i > 0) str[c++] = ' ';
   str[c++] = '0' + value;
   str[c] = 0;
   return c;
  }

  template <int size>
  static int secu3_itoa_u32(char* str, unsigned int value)
  {
   int start = 0, c = 0, i = size;
   while(--i > INTP10_SIZE-1) str[c++] = ' ';

   i = INTP10_SIZE-1;
   while(i >= 0)
   {
    unsigned int v = value / int_p10[i];
    value = value % int_p10[i];

    if (v > 0 || start)
    {
     start = 1;
     str[c++] = '0' + v;
    }
    else
    {
     if (i > 0)
     {
      if (i < size)
       str[c++] = ' ';
     }
     else
      str[c++] = '0';
    }
    --i;
   }

   str[c] = 0; //terminate string
   return c;
  }

  template <int size>
  static int secu3_itoa_32(char* str, signed int value)
  {
   int start = 0, c = 0, i = size;
   bool sign = false;
   if (value < 0)
   {
    value = -value;
    sign = true;
    --i;
   }

   while(--i > INTP10_SIZE-1) str[c++] = ' ';

   i = INTP10_SIZE-1;
   while(i >= 0)
   {
    signed int v = value / int_p10[i];
    value = value % int_p10[i];
 
    if (v > 0 || start)
    {
     start = 1;
     if (sign)
     {
      str[c++] = '-';
      sign = false;
     }
     str[c++] = '0' + v;
    }
    else
    {
     if (i > 0)
     {
      if (i < size - sign)
       str[c++] = ' ';
     }
     else
      str[c++] = '0';
    }
    --i;
   }

   str[c] = 0; //terminate string
   return c;
  }

  static const char IntToStr[100][3];

 private:
  static bool CNumericConv::_Hex32ToBin(const BYTE* i_buf,DWORD* o_dword);
  static char m_decpt;
  static const float neg_p10[];
  static const float pos_p10[];
  static unsigned int int_p10[INTP10_SIZE];
  static const float scales[];
};

#endif //_NUMERICCONV_
