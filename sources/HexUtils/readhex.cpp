/****************************************************************
 *
 *  Created by Alexey A. Shabelnikov. Ukraine, Gorlovka 2005. 
 *   ICQ: 405-791-931. e-mail: shabelnikov-stc@mail.ru
 *  Microprocessors systems - design & programming.
 *
 *****************************************************************/

#include "stdafx.h"
#include "readhex.h"

#define HEXTONUM(hexbyte)\
 if((hexbyte > 47)&&(hexbyte < 58))\
  hexbyte-=0x30;\
 if ((hexbyte > 64)&&(hexbyte < 71))\
  hexbyte-=55;

#define RECTP_DATA 0x00 //data record
#define RECTP_EOF  0x01 //End Of file record
#define RECTP_ESA  0x02 //Extended Segment Address record

#define MAXIMUM_ADDRESS 0xFFFF

EReadHexStatus HexUtils_ConvertHexToBin(BYTE* ip_buff, size_t i_size, BYTE* op_buff, size_t& o_size)
{
 UCHAR  state = 0;
 UINT   d_ptr = 0;   //pointer to output buffer
 UCHAR  symbol;
 WORD   chksum;      //accumulates calculated check sum
 UCHAR  datasize;
 UCHAR  datacount;
 WORD   raddr;
 UCHAR  rectp;       //type of record (follows data bytes)
 UCHAR  lastb;
 WORD   maxaddr = 0; //maximum appeared address
 UCHAR  chkbyte;     //byte with check sum at the end of record
 UINT   written = 0; //number of bytes written into the output buffer
 int errflag = RH_SUCCESS;

 memset(op_buff, 0xFF, MAXIMUM_ADDRESS + 1); //забиваем буфер значениями 0xFF

 for(size_t i = 0; i < i_size; i++)
 {
  symbol = ip_buff[i];
  //--------------------------------------------------------------------------
  switch(state)
  {
   case 0:   
    if (symbol == ':')
    {
     chksum = 0x0000;  
     state = 1;
    }    
    break;

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

   case 3:
    HEXTONUM(symbol);
    symbol = symbol << 4;
    *((UCHAR*)(&raddr)+1)  = symbol;
    state = 4;
    break;

   case 4:
    HEXTONUM(symbol);
    symbol = symbol & 15;
    (*((UCHAR*)(&raddr)+1))|=symbol;
    chksum+=(*((UCHAR*)(&raddr)+1));
    state = 5;
    break;

   case 5: 
    HEXTONUM(symbol);
    symbol = symbol << 4;
    (*((UCHAR*)(&raddr))) = symbol;
    state = 6;
    break;

   case 6:
    HEXTONUM(symbol);
    symbol = symbol & 15;
    (*((UCHAR*)(&raddr)))|= symbol;
    chksum+=(*((UCHAR*)(&raddr)));
    d_ptr = raddr;         //overwrite address
    state = 7;
    break;

   case 7:
    HEXTONUM(symbol);
    symbol = symbol << 4;
    rectp = symbol;
    state = 8;
    break;

   case 8:
    HEXTONUM(symbol);
    symbol = symbol & 15;
    rectp|=symbol;
    chksum+=rectp;

    if ((rectp != RECTP_DATA) && (rectp != RECTP_EOF) && (rectp != RECTP_ESA))
      errflag|=RH_UNSUPPORTED_RECORD; //unsupported type of record encountered

    state = 9;
    break;

   case 9:
    if (datacount == 0)
    {
     state = 11;
     i--;		  
    }
    else
    {
     lastb = symbol;
     datacount--;
     state = 10;
    }
    break; 

   case 10:
    HEXTONUM(symbol);//l 
    HEXTONUM(lastb); //h
    lastb = lastb << 4;
    symbol = symbol & 15;
    symbol = lastb | symbol;

    if (rectp != RECTP_ESA)          //skip ESA records
    {
     if (d_ptr <= MAXIMUM_ADDRESS)
     {
      op_buff[d_ptr] = symbol;       //output
      ++written;
     }
     else
     {
      errflag|=RH_ADDRESS_EXCEDED;   //exceded maximum allowed address
      return (EReadHexStatus)errflag;
     }

     if(datacount==0)                //if last byte of data written into the output buffer
     {
      if(d_ptr > maxaddr)            //find maximum appeared addres
      {
       maxaddr = d_ptr;
      }
     }

    }
    chksum+=symbol;
    ++d_ptr;
    state = 9;
    break;

   case 11:
    HEXTONUM(symbol);
    symbol = symbol << 4;
    chkbyte = symbol;
    state = 12;
    break;

   case 12:
    HEXTONUM(symbol);
    symbol = symbol & 15;
    chkbyte|=symbol;
    symbol = (UCHAR)chksum;
    symbol = (~symbol)+1;
    if (chkbyte!=symbol) //check sum is not correct
    { 
     errflag|=RH_INCORRECT_CHKSUM;   
    }
    state = 13;
    break;

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

 o_size = (0 == written) ? 0 : maxaddr + 1;   //сохраняем размер данных

 return (EReadHexStatus)errflag;  // 0 - если нет ошибок
}
