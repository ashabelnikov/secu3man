#pragma once


//Аппаратно-независимая прослойка над данными EEPROM.

class AFX_EXT_CLASS EEPROMDataMediator
{
  public:
     EEPROMDataMediator();
    ~EEPROMDataMediator();
    
     static int GetDefParamsStartAddress(void);
};
