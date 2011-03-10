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

#include "stdafx.h"
#include "readhex.h"

#define HEXTONUM(hexbyte)\
 if((hexbyte > 47)&&(hexbyte < 58))\
  hexbyte-=0x30;\
 if ((hexbyte > 64)&&(hexbyte < 71))\
  hexbyte-=55;

#define RECTP_DATA 0x00                       //data record
#define RECTP_EOF  0x01                       //End Of file record
#define RECTP_ESA  0x02                       //Extended Segment Address record
#define RECTP_SSA  0x03                       //Start Segment Address record

EReadHexStatus HexUtils_ConvertHexToBin(BYTE* ip_buff, size_t i_size, BYTE* op_buff, size_t& o_size, size_t i_max_size)
{
 UCHAR  state = 0;                            //for state machine
 UINT   d_ptr = 0;                            //output buffer's iterator
 UCHAR  symbol;                               //current symbol
 WORD   chksum;                               //accumulates calculated check sum
 UCHAR  datasize;                             //counter for record's data
 UCHAR  datacount;                            //stores count of bytes in record
 WORD   raddr;                                //stores read address field
 UCHAR  rectp;                                //type of record (follows data bytes)
 UCHAR  lastb;                                //stores previous HEX symbol
 UINT   maxaddr = 0;                          //maximum appeared address
 UCHAR  chkbyte;                              //byte with check sum at the end of record
 UINT   written = 0;                          //number of bytes written into the output buffer
 int errflag = RH_SUCCESS;                    //there are no errors at start

 UINT esa = 0;                                //segment address
 bool esa_read = false;

 memset(op_buff, 0xFF, i_max_size);           //fill buffer with 0xFF values

 for(size_t i = 0; i < i_size; i++)
 {
  symbol = ip_buff[i];
  //--------------------------------------------------------------------------
  switch(state)
  {
   //check start code
   case 0:
    if (symbol == ':')
    {
     chksum = 0x0000;
     state = 1;
    }
    esa_read = false;                         //ESA address is not initialized (esa variable)
    break;

   //read count
   case 1:
    HEXTONUM(symbol);
    symbol = symbol << 4;
    datasize = symbol;
    state = 2;
    break;
   case 2:
    HEXTONUM(symbol);
    symbol = symbol & 15;
    datasize|=symbol;
    datacount = datasize;
    chksum+=datasize;
    state = 3;
    break;

   //read address
   case 3:
    HEXTONUM(symbol);
    symbol = symbol << 4;
    raddr = ((WORD)symbol) << 8;
    state = 4;
    break;
   case 4:
    HEXTONUM(symbol);
    symbol = symbol & 0x0F;
    raddr|= (((WORD)symbol) << 8);
    chksum+= raddr >> 8;
    state = 5;
    break;
   case 5:
    HEXTONUM(symbol);
    raddr|= symbol << 4;
    state = 6;
    break;
   case 6:
    HEXTONUM(symbol);
    raddr|= symbol & 0x0F;
    chksum+= raddr & 0xFF;
    d_ptr = ((UINT)raddr) + ((esa << 4) & 0xFFFF0); //overwrite address
    state = 7;
    break;

   //read record type
   case 7:
    HEXTONUM(symbol);
    rectp = symbol << 4;
    state = 8;
    break;
   case 8:
    HEXTONUM(symbol);
    rectp|=symbol & 0x0F;
    chksum+=rectp;
    if ((rectp != RECTP_DATA) && (rectp != RECTP_EOF) && (rectp != RECTP_ESA) && (rectp != RECTP_SSA))
      errflag|=RH_UNSUPPORTED_RECORD;         //unsupported type of record encountered
    state = 9;
    break;

   //read data
   case 9:
    if (datacount == 0)
    {
     state = 11;
     --i;
    }
    else
    {
     lastb = symbol;
     --datacount;
     state = 10;
    }
    break;

   case 10:
    HEXTONUM(symbol);//lo
    HEXTONUM(lastb); //hi
    symbol = (lastb << 4) | (symbol & 0x0F);

    if (rectp != RECTP_SSA)                    //skip SSA records
    {
     if (rectp != RECTP_ESA)                   //skip ESA records
     {
      //check for address restriction
      if (d_ptr < i_max_size)
      {
       op_buff[d_ptr] = symbol;                //output
       ++written;
      }
      else
      {
       errflag|=RH_ADDRESS_EXCEDED;            //exceded maximum allowed address
       return (EReadHexStatus)errflag;
      }

      if(datacount==0)                         //if last byte of data written into the output buffer
      {
       if(d_ptr > maxaddr)                     //find maximum appeared addres
       maxaddr = d_ptr;
      }
     }
     else
     {                                         //save segment address
      if (false==esa_read)
      {
       esa = (symbol << 8);
       esa_read = true;
      }
      else
      {
       esa = (esa & 0xFF00) | symbol;

       //The least significant hex digit of the segment address is always 0
       if (symbol & 0x0F)
        errflag|=RH_UNEXPECTED_SYMBOL;
      }
     }
    }
    chksum+=symbol;
    ++d_ptr;
    state = 9;
    break;

   //------read and compare check sum
   case 11:
    HEXTONUM(symbol);
    chkbyte = symbol << 4;
    state = 12;
    break;
   case 12:
    HEXTONUM(symbol);
    chkbyte|=symbol & 0xFF;
    symbol = (~((UCHAR)chksum)) + 1;
    if (chkbyte!=symbol)                      //check sum is not correct
     errflag|=RH_INCORRECT_CHKSUM;
    state = 13;
    break;

   //------read and check CR LF
   case 13:
    if (symbol!=0x0D)
     errflag|=RH_UNEXPECTED_SYMBOL;
    state = 14;
    break;
   case 14:
    if (symbol!=0x0A)
     errflag|=RH_UNEXPECTED_SYMBOL;
    state = 0;
    break;
  }//switch
  //--------------------------------------------------------------------------
 }//for

 o_size = (0 == written) ? 0 : maxaddr + 1;   //Save size of data

 return (EReadHexStatus)errflag;  // return RH_SUCCESS - if there are no errors
}
