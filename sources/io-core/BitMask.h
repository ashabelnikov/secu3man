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

/** \file BitMask.h
 * \author Alexey A. Shabelnikov
 */

#include "iocore_api.h"

// Write specified bit into a 8-bit variable
// variable - Variable
// bitNum - Number of bit for writing in
// value - value of bit
void BITMASK_API WRITEBIT8(BYTE& variable, int bitNum, bool value);

void BITMASK_API WRITEBIT16(WORD& variable, int bitNum, bool value);

void BITMASK_API WRITEBIT32(DWORD& variable, int bitNum, bool value);

//Check specified bit of the 32-bit variable
// variable - Variable
// bitNum - Number of bit to check
// return - value of bit
bool BITMASK_API CHECKBIT32(const DWORD& variable, int bitNum);

//Check specified bit of the 16-bit variable
// variable - Variable
// bitNum - Number of bit to check
// return - value of bit
bool BITMASK_API CHECKBIT16(const WORD& variable, int bitNum);
bool BITMASK_API CHECKBIT16(const int& variable, int bitNum);

bool BITMASK_API CHECKBIT8(const BYTE& variable, int bitNum);

//Makes byte from two 4-bit values
BYTE BITMASK_API MAKEBYTE(int hi, int lo);

//Get low 4 bits from a byte
BYTE BITMASK_API GETLO4BITS(BYTE byte);
//Get high 4 bits from a byte
BYTE BITMASK_API GETHI4BITS(BYTE byte);
