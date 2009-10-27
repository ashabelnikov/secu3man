
#include "types.h"
#include <string>

#ifndef _NUMERICCONV_
#define _NUMERICCONV_

class AFX_EXT_CLASS CNumericConv  
{
public:

   static bool   HexArrayToBin(const BYTE* i_buf, BYTE* o_buf, const int size);

   static bool   Hex4ToBin(const BYTE i_hex_number,BYTE* o_byte);
   static bool   Hex8ToBin(const BYTE* i_hex_number,BYTE* o_byte);
   static bool   Hex16ToBin(const BYTE* i_hex_number,int* o_word,bool i_signed  = false);
   static bool   Hex32ToBin(const BYTE* i_hex_number,signed long *o_dword);
   static bool   Hex32ToBin(const BYTE* i_hex_number,unsigned long *o_dword);

   inline static BYTE HTOD(BYTE h) 
   {
	 return ((h<0x3A)?h-'0':h-'A'+10);
   }

   inline static BYTE DTOH(BYTE d) 
   { 
     char hdig[] = "0123456789ABCDEF"; 
	 return hdig[(d & 0x0F)];
   }

   static BYTE   CheckSum_8_xor(const BYTE* i_buf,const int size);

   static bool   BinToHexArray(const BYTE* i_buf, BYTE* o_buf, const int size);

   static bool   Bin4ToHex(const BYTE i_byte,std::string& o_hex_number);
   static bool   Bin8ToHex(const BYTE i_byte,BYTE* o_hex_number);
   static bool   Bin8ToHex(const BYTE i_byte,std::string& o_hex_number);
   static bool   Bin16ToHex(const int i_word,BYTE* o_hex_number);
   static bool   Bin16ToHex(const int i_word,std::string& o_hex_number);
   static bool   Bin32ToHex(const unsigned long i_dword,std::string& o_hex_number);
   static bool   Bin32ToHex(const signed long i_dword,std::string& o_hex_number);

private:
	static bool CNumericConv::_Hex32ToBin(const BYTE* i_buf,DWORD* o_dword);

};

#endif //_NUMERICCONV_
