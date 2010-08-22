/****************************************************************
 *       SECU-3  - An open source, free engine control unit
 *    Designed by Alexey A. Shabelnikov. Ukraine, Gorlovka 2007.
 *       Microprocessors systems - design & programming.
 *    contacts:
 *              http://secu-3.narod.ru
 *              ICQ: 405-791-931
 ****************************************************************/

#include "stdafx.h"
#include "EEPROMDataMediator.h"
#include "CRC16.h"

//адрес структуры параметров в EEPROM
#define EEPROM_PARAM_START     0x002


EEPROMDataMediator::EEPROMDataMediator()
{

}

EEPROMDataMediator::~EEPROMDataMediator()
{
}

int EEPROMDataMediator::GetDefParamsStartAddress(void)
{
 return EEPROM_PARAM_START;
}
