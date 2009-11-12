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

EReadHexStatus HexUtils_ConvertHexToBin(BYTE* ip_buff, size_t i_size, BYTE* op_buff, size_t& o_size)
{
 UCHAR  state = 0;
 UINT   d = 0;
 UCHAR  symbol;
 WORD   chksum;
 UCHAR  datasize;
 UCHAR  datacount;
 WORD   raddr;
 UCHAR  rectp;
 UCHAR  lastb;
 WORD   maxaddr = 0;
 UCHAR  chkbyte;
 int errflag = RH_SUCCESS;

 memset(op_buff, 0xFF, 0x10000); //забиваем буфер значениями 0xFF

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
    d = raddr;         //overwrite address
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
    op_buff[d] = symbol;    //output
    d++;
    chksum+=symbol;
    state = 9;

    if(datacount==0)
    {
     if(d > maxaddr)
     {
      maxaddr = d;
     }
    }
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

 o_size = maxaddr;   //сохраняем размер данных 

 return (EReadHexStatus)errflag;  // 0 - если нет ошибок
}
