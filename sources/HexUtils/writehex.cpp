/* Intel Hex writer (binary to hex)
   Copyright (C) 2005 Alexey A. Shabelnikov. Ukraine, Kiev

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
          e-mail: shabelnikov@secu-3.org
          Microprocessor systems - design & programming.
*/

/** \file writehex.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include <vector>
#include "writehex.h"
#include <stdlib.h>

#define  MAX_CODE_SIZE  1048576

// Places segment record (02) into a file
// segment_addr - segment address in format 0x*000, * - 0...F
void insert02record(FILE* f, long segment_addr)
{
  unsigned short chk = 0;
  chk+= 2; //count
  chk+= 0; //lo addr byte
  chk+= 0; //hi addr byte
  chk+= 2; //record type
  chk+= (unsigned short)(segment_addr & 0xFF);
  chk+= (unsigned short)((segment_addr >> 8) & 0xFF);

  chk = ((~(unsigned char)chk) + 1) & 0xFF; 

  fprintf(f, ":02000002%04X%02X\r\n", segment_addr, (int)chk);
}

// Places specified piece of data as hex-record (00) into file
// data - pointer to data buffer
// addr - start address (offset in data buffer)
// count - number of bytes to convert 
void hexstr(FILE* f, unsigned char *data, long addr, long count)
{
  fprintf(f,":%02X%04X00", count, addr);

  unsigned short chk = 0;
  for(int i = 0; i < count; i++)
  {
    fprintf(f,"%02X",*data);
    chk+=*data;
    ++data;
  }

  chk+= 0;
  chk+= (unsigned char)count;
  chk+= (unsigned short)(addr & 0xFF);
  chk+= (unsigned short)((addr >> 8) & 0xFF);

  chk = ((~(unsigned char)chk) + 1) & 0xFF; 

  fprintf(f,"%02X\r\n",(int)chk);
}

bool WriteHexFile(FILE* fout, std::vector<BYTE>& code)
{
 if (code.size() > MAX_CODE_SIZE)
  return false;

 unsigned char *p = &code[0];   //data
 unsigned long haddr = 0;  //address inside segment  
 unsigned long bsize = 16; //size of block
 unsigned long seg_addr = 0;
 long iterations = (code.size() / bsize) + ((code.size() % bsize) ? 1 : 0);

 for (long i = 0; i < iterations; ++i)
 {
  if ((haddr & 0xF0000) != seg_addr)
  {
   insert02record(fout, ((haddr & 0xF0000) >> 4) & 0xFFFF);
   seg_addr = (haddr & 0xF0000);
  }            

  if ((code.size() - haddr) < bsize)
   hexstr(fout, p, haddr & 0xFFFF, (code.size() - haddr));
  else
   hexstr(fout, p, haddr & 0xFFFF, bsize);
    
  haddr+=bsize;
  p+=bsize;    
 }
 fprintf(fout,":00000001FF\r\n");
 return true;
}
