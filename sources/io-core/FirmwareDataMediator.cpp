#include "stdafx.h"
#include "FirmwareDataMediator.h"
#include "CRC16.h"
#include "BootLoader.h"


typedef unsigned short _uint;
typedef signed short   _int;
typedef unsigned short _ushort;
typedef signed long    _long;
typedef signed char    _char;
typedef unsigned char  _uchar;


//этот файл содержит платформенно ориентированные фрагменты кода
#pragma pack(1)

//определяем количество узлов интерполяции для каждой функции
#define F_WRK_POINTS_F         16  
#define F_WRK_POINTS_L         16  
#define F_TMP_POINTS           16
#define F_STR_POINTS           16                            
#define F_IDL_POINTS           16     
#define F_NAME_SIZE            16

typedef struct 
{
  _char f_str[F_STR_POINTS];                       // функция УОЗ на старте
  _char f_idl[F_IDL_POINTS];                       // функция УОЗ для ХХ
  _char f_wrk[F_WRK_POINTS_L][F_WRK_POINTS_F];     // основная функция УОЗ (3D)
  _char f_tmp[F_TMP_POINTS];                       // функция коррект. УОЗ по температуре
  _uchar name[F_NAME_SIZE];                         // ассоциированное имя (имя семейства)
}F_data;


#pragma pack(1)
//описывает параметры системы
typedef struct
{
  _uchar tmp_use;                        //признак комплектации ДТОЖ-ом
  _uchar carb_invers;                    //инверсия концевика на карбюраторе
  _uchar idl_regul;                      //поддерживать заданные обороты ХХ регулмрованием УОЗ
  _uchar fn_benzin;                      //номер набора характеристик используемый для бензина
  _uchar fn_gas;                         //номер набора характеристик используемый для газа
  _uint  map_grad;                       //наклон шкалы расхода воздуха (кПа)
  _uint  ephh_lot;                       //нижний порог ЭПХХ (мин-1)
  _uint  ephh_hit;                       //верхний порог ЭПХХ (мин-1)
  _uint  starter_off;                    //порог выключения стартера (мин-1)
  _int  press_swing;                    //перепад давления при полностью открытом дросселе   (кПа)
  _uint  smap_abandon;                   //обороты перехода с пусковой карты на рабочую  (мин-1) 
  _int  max_angle;                      //ограничение максимального УОЗ
  _int  min_angle;                      //ограничение минимального УОЗ
  _int  angle_corr;                     //октан-коррекция УОЗ    
  _uint  idl_turns;                      //заданные обороты ХХ для поддержания регулмрованием УОЗ   
  _int  ifac1;                          //коэффициенты П-регулятора оборотов ХХ, для положительной и
  _int  ifac2;                          //отрицательной ошибке соответственно, 1...100 
  _int  MINEFR;                         //зона нечувствительности регулятора (обороты)
  _int  vent_on;                        //температура включения вентилятора
  _int  vent_off;                       //температура выключения вентилятора  

  _int  map_adc_factor;
  _long map_adc_correction;
  _int  ubat_adc_factor;
  _long ubat_adc_correction;
  _int  temp_adc_factor;
  _long temp_adc_correction;
  
  _ushort crc;                           //контрольная сумма данных этой структуры (для проверки корректности данных после считывания из EEPROM)  
}params;


#define FW_SIGNATURE_INFO_SIZE 48

#pragma pack(1)
//описывает дополнительные данные хранимые в прошивке
typedef struct
{
  _uchar fw_signature_info[FW_SIGNATURE_INFO_SIZE];
}FirmwareData;


#define BOOT_START 0x3E00
#define CODE_CRC_ADDR (BOOT_START-sizeof(unsigned short))
#define CODE_SIZE (BOOT_START-sizeof(unsigned short)) //размер кода прошивки без учета байтов контрольной суммы

//количество наборов характеристик хранимых в памяти программ
#define TABLES_NUMBER          8   

//адрес массива таблиц - семейств характеристик
#define TABLES_START (CODE_CRC_ADDR-(sizeof(F_data)*TABLES_NUMBER))

//адрес структуры дефаултных параметров (параметров EEPROM по умолчанию)
#define DEFPARAM_START (TABLES_START-sizeof(params))

//адрес дополнительных параметров
#define FIRMWARE_DATA_START (DEFPARAM_START-sizeof(FirmwareData))


CFirmwareDataMediator::CFirmwareDataMediator()
: m_firmware_size(CBootLoader::FLASH_TOTAL_SIZE)
, m_is_opened(false)
{
  memset(m_bytes_active,0x00,m_firmware_size);
  memset(m_bytes_original,0x00,m_firmware_size);
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


void CFirmwareDataMediator::LoadBytes(BYTE* i_bytes)
{
  ASSERT(i_bytes);
  memset(m_bytes_active,0x00,m_firmware_size);
  memset(m_bytes_original,0x00,m_firmware_size);

  memcpy(m_bytes_active,i_bytes,m_firmware_size);
  memcpy(m_bytes_original,i_bytes,m_firmware_size);  

  m_is_opened = true;
}
	
void CFirmwareDataMediator::StoreBytes(BYTE* o_bytes)
{
  ASSERT(o_bytes);
  memcpy(o_bytes,m_bytes_active,m_firmware_size);
}

bool CFirmwareDataMediator::IsModified(void)
{
  return (0 != memcmp(m_bytes_active,m_bytes_original,m_firmware_size));
}

void CFirmwareDataMediator::ResetModified(void)
{
  memcpy(m_bytes_original,m_bytes_active,m_firmware_size);
}

_TSTRING CFirmwareDataMediator::GetSignatureInfo(void)
{
  char raw_string[256];
  memset(raw_string,0,FW_SIGNATURE_INFO_SIZE+1);
  BYTE* addr = &m_bytes_active[FIRMWARE_DATA_START];
  memcpy(raw_string,addr,FW_SIGNATURE_INFO_SIZE);  
  TCHAR string[256];
  CharToOem(raw_string,string);
  return _TSTRING(string);
}

void CFirmwareDataMediator::SetSignatureInfo(_TSTRING i_string)
{
  char raw_string[256];
  memset(raw_string,0,FW_SIGNATURE_INFO_SIZE+1);
  BYTE* addr = &m_bytes_active[FIRMWARE_DATA_START];
  OemToChar(i_string.c_str(),raw_string);
  memcpy(addr,raw_string,FW_SIGNATURE_INFO_SIZE);    
}
