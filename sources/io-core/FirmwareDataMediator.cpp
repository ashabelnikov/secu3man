#include "stdafx.h"
#include "FirmwareDataMediator.h"
#include "CRC16.h"
#include "BootLoader.h"

#define BOOT_START 0x3E00
#define CODE_CRC_ADDR (BOOT_START-sizeof(unsigned short))
#define CODE_SIZE (BOOT_START-sizeof(unsigned short)) //размер кода прошивки без учета байтов контрольной суммы



CFirmwareDataMediator::CFirmwareDataMediator()
{
}

CFirmwareDataMediator::~CFirmwareDataMediator()
{
}

//считает контрольную сумму и записывает результат по соответствующему адресу
//io_data - массив байтов прошивки 
void CFirmwareDataMediator::CalculateAndPlaceFirmwareCRC(BYTE* io_data)
{
  unsigned short crc = crc16(io_data,CODE_SIZE);
  unsigned short* crc_addr = (unsigned short*)(&io_data[CODE_CRC_ADDR]);
  *crc_addr = crc; //сохранили контрольную сумму
}
