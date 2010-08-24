#pragma once


//Аппаратно-независимая прослойка над данными EEPROM.

class AFX_EXT_CLASS EEPROMDataMediator
{
 public:
  EEPROMDataMediator();
 ~EEPROMDataMediator();
    
  static int GetDefParamsStartAddress(void);

  //Calculates CRC16 of default parameters for given buffer with EEPROM
  //ip_eeprom_buffer - contains EEPROM 
  //return true - OK, false - check sum is invalid
  static bool VerifyDefParamsCheckSum(const BYTE* ip_eeprom_buffer);
};
