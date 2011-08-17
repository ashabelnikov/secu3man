/* Intel Hex to binary convertor
   Copyright (C) 2005 Alexey A. Shabelnikov. Ukraine, Gorlovka

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
          ICQ: 405-791-931. e-mail: shabelnikov-stc@mail.ru
          Microprocessors systems - design & programming.
*/

#pragma once

#ifdef HEXUTILS_EXPORTS
 #define HEXUTILS_API __declspec(dllexport)
#else
 #define HEXUTILS_API __declspec(dllimport)
#endif

//errors
enum EReadHexStatus
{
 RH_SUCCESS = 0,
 RH_INCORRECT_CHKSUM = 1,
 RH_UNEXPECTED_SYMBOL = 2,
 RH_UNSUPPORTED_RECORD = 4,
 RH_ADDRESS_EXCEDED = 5
};

//types
#ifndef BYTE
#define BYTE unsigned char
#endif

#ifndef UCHAR
#define UCHAR unsigned char
#endif

#ifndef UINT
#define UINT unsigned int
#endif

#ifndef WORD
#define WORD unsigned short
#endif

// Converts Intel Hex data into binary form. Supports up to 1 megabyte address space.

//ip_buff - input buffer containing symbols from Hex-file
//i_size - size of input buffer in symbols
//op_buff - output buffer which will receive converted (binary) data
//o_size - receives size of converted binary data
//i_max_size - maximum size of output binary, if exceded, error will be returned
//i_unix_lfcr - accept also UNIX LFCRs (only 0xA)

//Returns RH_SUCCESS - if there was no errors, otherwise corresponding error code
HEXUTILS_API EReadHexStatus HexUtils_ConvertHexToBin(BYTE* ip_buff, size_t i_size, BYTE* op_buff, size_t& o_size, size_t i_max_size, bool i_unix_lfcr = true);
