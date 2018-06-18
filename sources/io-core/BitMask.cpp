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

/** \file BitMask.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "BitMask.h"


// Write specified bit into a 8-bit variable
// variable - Variable
// bitNum - Number of bit for writing in
// value - value of bit
void BITMASK_API WRITEBIT8(BYTE& variable, int bitNum, bool value)
{
 ASSERT(bitNum < 8);
 variable = (variable & (~(1 << bitNum))) | ((value?1:0) << bitNum);
}

void BITMASK_API WRITEBIT32(DWORD& variable, int bitNum, bool value)
{
 ASSERT(bitNum < 32);
 variable = (variable & (~(1 << bitNum))) | ((value?1:0) << bitNum);
}

//Check bit in the 32-bit variable
bool BITMASK_API CHECKBIT32(const DWORD& variable, int bitNum)
{
 ASSERT(bitNum < 32);
 return (variable & (1 << bitNum)) != 0;
}

void BITMASK_API WRITEBIT16(WORD& variable, int bitNum, bool value)
{
 ASSERT(bitNum < 16);
 variable = (variable & (~(1 << bitNum))) | ((value?1:0) << bitNum);
}

//Check bit in the 16-bit variable
bool BITMASK_API CHECKBIT16(const WORD& variable, int bitNum)
{
 ASSERT(bitNum < 16);
 return (variable & (1 << bitNum)) != 0;
}

bool BITMASK_API CHECKBIT16(const int& variable, int bitNum)
{
 ASSERT(bitNum < 16);
 return (variable & (1 << bitNum)) != 0;
}

bool BITMASK_API CHECKBIT8(const BYTE& variable, int bitNum)
{
 ASSERT(bitNum < 8);
 return (variable & (1 << bitNum)) != 0;
}

BYTE BITMASK_API MAKEBYTE(int hi, int lo)
{
 return ((hi & 0xF) << 4) | (lo & 0xF);
}

BYTE BITMASK_API GETLO4BITS(BYTE byte)
{
 return byte & 0x0F;
}

BYTE BITMASK_API GETHI4BITS(BYTE byte)
{
 return (byte >> 4) & 0xF;
}
