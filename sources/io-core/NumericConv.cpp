 /****************************************************************
 *       SECU-3  - An open source, free engine control unit
 *    Designed by Alexey A. Shabelnikov. Ukraine, Gorlovka 2007.
 *       Microprocessors systems - design & programming.
 *    contacts:
 *              http://secu-3.narod.ru
 *              ICQ: 405-791-931
 ****************************************************************/

#include "stdafx.h"
#include "NumericConv.h"
#include "ctype.h"


// онвертирует массив шестнадцатерично заданных чисел (два ASCII символа представл€ют один байт) в массив бинарно
//заданных числел (unsigned char).
//size - размер в байтах!!!
bool CNumericConv::HexArrayToBin(const BYTE* i_buf, BYTE* o_buf, const int size)
{  
	int j=0;
	for(int i = 0; i < size; i++)
	{
      if (isxdigit(i_buf[j])&&(isxdigit(i_buf[j+1])))
	  {
	   o_buf[i] = HTOD(i_buf[j])*16 + HTOD(i_buf[j+1]);
	  }
	  else
		return false;  //error: not a HEX digit!
	  j+=2;
	} //for
 return true;
}


//ѕереводит шестнадцатеричное число заданное двум€ ASCII символами в байт.
//≈сли шестнадцатеричное число содержит ошибки то возвращает false
bool CNumericConv::Hex8ToBin(const BYTE* i_buf,BYTE* o_byte)
{
  if ((i_buf==NULL)||(o_byte==NULL)) 
   return false;
  if (isxdigit(i_buf[0])&&(isxdigit(i_buf[1])))
  {
   *o_byte = HTOD(i_buf[0])*16 + HTOD(i_buf[1]);
   return true;
  }
  else
   return false;
}


//считает контрольную сумму (8 бит) дл€ буфера байтов i_buf размером size простым методом
//(использу€ сложение по модулю 2)
//возвращает байт контрольной суммы
BYTE CNumericConv::CheckSum_8_xor(const BYTE* i_buf,const int size)
{
  BYTE sum=0;
  for(int i = 0; i < size; i++)
  {
   sum = sum ^ i_buf[i]; //XOR
  }//for
 return sum;
}

// онвертирует массив бинарно заданных числел (unsigned char)  в массив шестнадцатерично заданных чисел
//(два ASCII символа представл€ют один байт)
//size - размер в байтах!!!
bool CNumericConv::BinToHexArray(const BYTE* i_buf, BYTE* o_buf, const int size)
{
	int j=0;
	for(int i = 0; i < size; i++)
	{
      Bin8ToHex(i_buf[i],&o_buf[j]);	  //сохран€ем по 2 символа
	  j+=2;
	} //for
 return true;
}

bool CNumericConv::Bin8ToHex(const BYTE i_byte,BYTE* o_hex_number)
{
   if (o_hex_number==NULL) 
	   return false;

   o_hex_number[0] = DTOH(i_byte >> 4); //store HI part
   o_hex_number[1] = DTOH(i_byte);      //store LO part
   return true;
}


bool CNumericConv::Bin8ToHex(const BYTE i_byte,std::string& o_hex_number)
{ 
   o_hex_number+= DTOH(i_byte >> 4); //store HI part
   o_hex_number+= DTOH(i_byte);      //store LO part
   return true;
}


bool CNumericConv::Hex16ToBin(const BYTE* i_buf,int* o_word,bool i_signed /* = false*/)
{
  if (o_word==NULL) 
    return false;

  BYTE lo,hi;
  int word;
  if (isxdigit(i_buf[0])&&isxdigit(i_buf[1])&&isxdigit(i_buf[2])&&isxdigit(i_buf[3]))
  {
    if (false==Hex8ToBin(i_buf,&hi))
		return false;

    if (false==Hex8ToBin(i_buf+2,&lo))
		return false;

    word = MAKEWORD(lo,hi);

	//если число знаковое и отрицательное то делаем его отрицательным
    *o_word = (i_signed && word > 0x7FFF) ? -(0x10000 - word) : word;
    return true;
  }
    return false;
}

bool CNumericConv::Hex4ToBin(const BYTE i_hex_number,BYTE* o_byte)
{
	if (isxdigit(i_hex_number))
	{
     *o_byte = HTOD(i_hex_number);
	 return true;
	}
 return false;
}


bool CNumericConv::Bin16ToHex(const int i_word,BYTE* o_hex_number)
{
  if (!Bin8ToHex(HIBYTE(((WORD)i_word)),o_hex_number))
	  return false;

  if (!Bin8ToHex(LOBYTE(((WORD)i_word)),o_hex_number+2))
	  return false;

 return true;
}


bool CNumericConv::Bin16ToHex(const int i_word,std::string& o_hex_number)
{
  Bin8ToHex(HIBYTE(((WORD)i_word)),o_hex_number);
  Bin8ToHex(LOBYTE(((WORD)i_word)),o_hex_number);
 return true;
}

bool CNumericConv::Bin4ToHex(const BYTE i_byte,std::string& o_hex_number)
{
 o_hex_number+=DTOH(i_byte);
 return true;
}
