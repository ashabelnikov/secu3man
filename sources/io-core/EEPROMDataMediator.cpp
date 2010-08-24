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
#include "SECU3ParametersDef.h"

//адрес структуры параметров в EEPROM
#define EEPROM_PARAM_START     0x002

using namespace SECU3IO;
using namespace SECU3IO::SECU3Types;

EEPROMDataMediator::EEPROMDataMediator()
{
 //empty
}

EEPROMDataMediator::~EEPROMDataMediator()
{
 //empty
}

int EEPROMDataMediator::GetDefParamsStartAddress(void)
{
 return EEPROM_PARAM_START;
}

bool EEPROMDataMediator::VerifyDefParamsCheckSum(const BYTE* ip_eeprom_buffer)
{
 _uint crc = crc16(ip_eeprom_buffer + EEPROM_PARAM_START, sizeof(params) - sizeof(_uint));
 const _uint* crc_addr = (_uint*)(&ip_eeprom_buffer[EEPROM_PARAM_START + sizeof(params) - sizeof(_uint)]);
 return (*crc_addr)==crc; //check it!
}
