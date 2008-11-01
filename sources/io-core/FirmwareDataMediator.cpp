#include "stdafx.h"
#include "FirmwareDataMediator.h"
#include "CRC16.h"
#include "BootLoader.h"
#include "SECU3IO.h"
#include "ufcodes.h"
#include "ControlApp.h"    //should be removed - it is nearly unnecessary!
#include "FirmwareMapsDataHolder.h"


typedef unsigned short _uint;
typedef signed short   _int;
typedef unsigned short _ushort;
typedef signed long    _long;
typedef signed char    _char;
typedef unsigned char  _uchar;


//этот файл содержит платформенно ориентированные фрагменты кода
#pragma pack(1)

typedef struct 
{
  _char f_str[F_STR_POINTS];                       // функция УОЗ на старте
  _char f_idl[F_IDL_POINTS];                       // функция УОЗ для ХХ
  _char f_wrk[F_WRK_POINTS_L][F_WRK_POINTS_F];     // основная функция УОЗ (3D)
  _char f_tmp[F_TMP_POINTS];                       // функция коррект. УОЗ по температуре
  _uchar name[F_NAME_SIZE];                         // ассоциированное имя (имя семейства)
}F_data;


//описывает параметры системы
typedef struct
{
  _uchar tmp_use;                        //признак комплектации ДТОЖ-ом
  _uchar carb_invers;                    //инверсия концевика на карбюраторе
  _uchar idl_regul;                      //поддерживать заданные обороты ХХ регулмрованием УОЗ
  _uchar fn_benzin;                      //номер набора характеристик используемый для бензина
  _uchar fn_gas;                         //номер набора характеристик используемый для газа
  _uint  map_lower_pressure;             //нижнее значене ДАД по оси таблицы(кПа)
  _uint  ephh_lot;                       //нижний порог ЭПХХ (мин-1)
  _uint  ephh_hit;                       //верхний порог ЭПХХ (мин-1)
  _uint  starter_off;                    //порог выключения стартера (мин-1)
  _int   map_upper_pressure;             //верхнее значене ДАД по оси таблицы (кПа)
  _uint  smap_abandon;                   //обороты перехода с пусковой карты на рабочую  (мин-1) 
  _int   max_angle;                      //ограничение максимального УОЗ
  _int   min_angle;                      //ограничение минимального УОЗ
  _int   angle_corr;                     //октан-коррекция УОЗ    
  _uint  idling_rpm;                     //заданные обороты ХХ для поддержания регулмрованием УОЗ   
  _int   ifac1;                          //коэффициенты П-регулятора оборотов ХХ, для положительной и
  _int   ifac2;                          //отрицательной ошибке соответственно.
  _int   MINEFR;                         //зона нечувствительности регулятора (обороты)
  _int   vent_on;                        //температура включения вентилятора
  _int   vent_off;                       //температура выключения вентилятора  

  _int  map_adc_factor;
  _long map_adc_correction;
  _int  ubat_adc_factor;
  _long ubat_adc_correction;
  _int  temp_adc_factor;
  _long temp_adc_correction;
  
  _uchar ckps_edge_type;                
  _uchar ckps_cogs_btdc;
  _uchar ckps_ignit_cogs;

  _int angle_dec_spead;
  _int angle_inc_spead;  
  _int idlreg_min_angle;
  _int idlreg_max_angle;
  _uint map_curve_offset;
  _uint map_curve_gradient;

  //Эти зарезервированные байты необходимы для сохранения бинарной совместимости
  //новых версий прошивок с более старыми версиями. При добавлении новых данных
  //в структуру, необходимо расходовать эти байты.
  _uchar reserved[12];

  _ushort crc;                           //контрольная сумма данных этой структуры (для проверки корректности данных после считывания из EEPROM)  
}params;


#define FW_SIGNATURE_INFO_SIZE 48

//описывает дополнительные данные хранимые в прошивке
typedef struct
{
  _uchar fw_signature_info[FW_SIGNATURE_INFO_SIZE];

  //Эти зарезервированные байты необходимы для сохранения бинарной совместимости
  //новых версий прошивок с более старыми версиями. При добавлении новых данных
  //в структуру, необходимо расходовать эти байты.
  _uchar reserved[128];  
}FirmwareData;


#define BOOT_START 0x3E00
#define CODE_CRC_ADDR (BOOT_START-sizeof(unsigned short))
#define CODE_SIZE (BOOT_START-sizeof(unsigned short)) //размер кода прошивки без учета байтов контрольной суммы

//адрес массива таблиц - семейств характеристик
#define TABLES_START (CODE_CRC_ADDR-(sizeof(F_data)*TABLES_NUMBER))

//адрес структуры дефаултных параметров (параметров EEPROM по умолчанию)
#define DEFPARAM_START (TABLES_START-sizeof(params))

//адрес дополнительных параметров
#define FIRMWARE_DATA_START (DEFPARAM_START-sizeof(FirmwareData))

//---два нижних определения необходимы для экспорта/импорта данных между прошивками--
//стартовый адрес всех данных прошивки в байтах
#define FIRMWARE_ALL_DATA_START (FIRMWARE_DATA_START)
//размер всех данных прошивки без учета байтов контрольной суммы прошивки
#define FIRMWARE_ALL_DATA_SIZE  ( (sizeof(F_data)*TABLES_NUMBER) + sizeof(params) + sizeof(FirmwareData))  



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


void CFirmwareDataMediator::LoadBytes(const BYTE* i_bytes)
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
  /*CharToOem(raw_string,string);*/
  OemToChar(raw_string,string);
  return _TSTRING(string);
}

void CFirmwareDataMediator::SetSignatureInfo(_TSTRING i_string)
{
  char raw_string[256];
  memset(raw_string,0,FW_SIGNATURE_INFO_SIZE+1);
  BYTE* addr = &m_bytes_active[FIRMWARE_DATA_START];
  /*OemToChar(i_string.c_str(),raw_string);*/
  CharToOem(i_string.c_str(),raw_string);
  memcpy(addr,raw_string,FW_SIGNATURE_INFO_SIZE);    
}


void CFirmwareDataMediator::GetStartMap(int i_index,float* o_values, bool i_original /* = false */)
{
  BYTE* p_bytes = NULL;
  F_data* p_maps = NULL;
  ASSERT(o_values);

  if (i_original)	  
	p_bytes = m_bytes_original;
  else
    p_bytes = m_bytes_active; 

  //получаем адрес начала таблиц семейств характеристик
  p_maps = (F_data*)(p_bytes + TABLES_START); 
   
  
  for (int i = 0; i < F_STR_POINTS; i++ )
    o_values[i] = ((float)p_maps[i_index].f_str[i]) / 2.0f;

}

void CFirmwareDataMediator::SetStartMap(int i_index,const float* i_values)
{
  BYTE* p_bytes = NULL;
  F_data* p_maps = NULL;
  ASSERT(i_values);

  p_bytes = m_bytes_active; 

  //получаем адрес начала таблиц семейств характеристик
  p_maps = (F_data*)(p_bytes + TABLES_START); 
   
  
  for (int i = 0; i < F_STR_POINTS; i++ )
	p_maps[i_index].f_str[i] = CNumericConv::Round((i_values[i]*2.0f));
}

void CFirmwareDataMediator::GetIdleMap(int i_index,float* o_values, bool i_original /* = false */)
{
  BYTE* p_bytes = NULL;
  F_data* p_maps = NULL;
  ASSERT(o_values);

  if (i_original)	  
	p_bytes = m_bytes_original;
  else
    p_bytes = m_bytes_active; 

  //получаем адрес начала таблиц семейств характеристик
  p_maps = (F_data*)(p_bytes + TABLES_START); 
   
  
  for (int i = 0; i < F_IDL_POINTS; i++ )
    o_values[i] = ((float)p_maps[i_index].f_idl[i]) / 2.0f;

}

void CFirmwareDataMediator::SetIdleMap(int i_index,const float* i_values)
{
  BYTE* p_bytes = NULL;
  F_data* p_maps = NULL;
  ASSERT(i_values);

  p_bytes = m_bytes_active; 

  //получаем адрес начала таблиц семейств характеристик
  p_maps = (F_data*)(p_bytes + TABLES_START); 
   
  
  for (int i = 0; i < F_IDL_POINTS; i++ )
	p_maps[i_index].f_idl[i] = CNumericConv::Round((i_values[i]*2.0f));
}

std::vector<_TSTRING> CFirmwareDataMediator::GetFunctionsSetNames(void)
{
  std::vector<_TSTRING> names(TABLES_NUMBER);

  BYTE* p_bytes = NULL;
  F_data* p_maps = NULL;  

  p_bytes = m_bytes_active; 

  //получаем адрес начала таблиц семейств характеристик
  p_maps = (F_data*)(p_bytes + TABLES_START); 

  for (int i = 0; i < TABLES_NUMBER; i++)
  {         
    char raw_string[256];
    memset(raw_string,0,F_NAME_SIZE+1);
    BYTE* addr = p_maps[i].name; 
    memcpy(raw_string,addr,F_NAME_SIZE);  
    TCHAR string[256];
    OemToChar(raw_string,string);
	names[i] = string;
  }

return names;
}


void CFirmwareDataMediator::SetFunctionsSetName(int i_index, _TSTRING i_new_name)
{
  if (i_index >= TABLES_NUMBER)       
    return;

  BYTE* p_bytes = NULL;
  F_data* p_maps = NULL; 

  p_bytes = m_bytes_active; 

  //получаем адрес начала таблиц семейств характеристик
  p_maps = (F_data*)(p_bytes + TABLES_START); 
 

  char raw_string[256];
  memset(raw_string,0,FW_SIGNATURE_INFO_SIZE+1);  
  CharToOem(i_new_name.c_str(),raw_string);
  memcpy(p_maps[i_index].name,raw_string,F_NAME_SIZE);     
}

void CFirmwareDataMediator::SetFWFileName(const _TSTRING i_fw_file_name)
{
  m_fw_file_name = i_fw_file_name;
}

_TSTRING CFirmwareDataMediator::GetFWFileName(void)
{
  return m_fw_file_name;
}


unsigned int CFirmwareDataMediator::CalculateCRC16OfActiveFirmware(void)
{
  unsigned short crc_16 = crc16(m_bytes_active,CODE_SIZE);
  return crc_16;
}

unsigned int CFirmwareDataMediator::GetCRC16StoredInActiveFirmware(void)
{
 unsigned short* crc_16_addr = (unsigned short*)(&m_bytes_active[CODE_CRC_ADDR]);
 return *crc_16_addr;
}

void CFirmwareDataMediator::CalculateAndPlaceFirmwareCRC(void)
{
  CalculateAndPlaceFirmwareCRC(m_bytes_active);  
}

void CFirmwareDataMediator::LoadDataBytesFromAnotherFirmware(const BYTE* i_source_bytes)
{
 if (false==IsLoaded())
   return; //некуда загружать...
 memcpy(m_bytes_active + FIRMWARE_ALL_DATA_START,i_source_bytes + FIRMWARE_ALL_DATA_START,FIRMWARE_ALL_DATA_SIZE);
 /*???memcpy(m_bytes_original + FIRMWARE_ALL_DATA_START,i_source_bytes + FIRMWARE_ALL_DATA_START,FIRMWARE_ALL_DATA_SIZE);*/
}

void CFirmwareDataMediator::GetWorkMap(int i_index, float* o_values, bool i_original /* = false*/)
{
  BYTE* p_bytes = NULL;
  F_data* p_maps = NULL;
  ASSERT(o_values);

  if (i_original)	  
	p_bytes = m_bytes_original;
  else
    p_bytes = m_bytes_active; 

  //получаем адрес начала таблиц семейств характеристик
  p_maps = (F_data*)(p_bytes + TABLES_START); 
   
  
  for (int i = 0; i < (F_WRK_POINTS_F * F_WRK_POINTS_L); i++ )
  {
	_char *p = &(p_maps[i_index].f_wrk[0][0]);
	o_values[i] = ((float) *(p + i)) / 2.0f;
  }
}

void CFirmwareDataMediator::SetWorkMap(int i_index, const float* i_values)
{
  BYTE* p_bytes = NULL;
  F_data* p_maps = NULL;
  ASSERT(i_values);

  p_bytes = m_bytes_active; 

  //получаем адрес начала таблиц семейств характеристик
  p_maps = (F_data*)(p_bytes + TABLES_START); 
   
  
  for (int i = 0; i < (F_WRK_POINTS_F * F_WRK_POINTS_L); i++ )
  {
    _char *p = &(p_maps[i_index].f_wrk[0][0]);
	*(p + i) = CNumericConv::Round((i_values[i]*2.0f));
  }	
}

void CFirmwareDataMediator::GetTempMap(int i_index,float* o_values, bool i_original /* = false */)
{
  BYTE* p_bytes = NULL;
  F_data* p_maps = NULL;
  ASSERT(o_values);

  if (i_original)	  
	p_bytes = m_bytes_original;
  else
    p_bytes = m_bytes_active; 

  //получаем адрес начала таблиц семейств характеристик
  p_maps = (F_data*)(p_bytes + TABLES_START); 
   
  
  for (int i = 0; i < F_TMP_POINTS; i++ )
    o_values[i] = ((float)p_maps[i_index].f_tmp[i]) / 2.0f;

}

void CFirmwareDataMediator::SetTempMap(int i_index,const float* i_values)
{
  BYTE* p_bytes = NULL;
  F_data* p_maps = NULL;
  ASSERT(i_values);

  p_bytes = m_bytes_active; 

  //получаем адрес начала таблиц семейств характеристик
  p_maps = (F_data*)(p_bytes + TABLES_START); 
   
  for (int i = 0; i < F_TMP_POINTS; i++ )
	p_maps[i_index].f_tmp[i] = CNumericConv::Round((i_values[i]*2.0f));
}

bool CFirmwareDataMediator::SetDefParamValues(BYTE i_descriptor, const void* i_values)
{
  using namespace SECU3IO;

  BYTE* p_bytes = NULL;
  params* p_params = NULL; 
  p_bytes = m_bytes_active; 
  //получаем адрес структуры дефаултных параметров
  p_params = (params*)(p_bytes + DEFPARAM_START);

  //TODO: Remove these copy/paste! (conversions should be implemented in functions, as certain class)
  switch(i_descriptor)
  {
    case TEMPER_PAR:
		{
        TemperPar* p_in = (TemperPar*)i_values; 
	    p_params->tmp_use  = p_in->tmp_use;
	    p_params->vent_on  = CNumericConv::Round(p_in->vent_on * TEMP_PHYSICAL_MAGNITUDE_MULTIPLAYER);
        p_params->vent_off = CNumericConv::Round(p_in->vent_off * TEMP_PHYSICAL_MAGNITUDE_MULTIPLAYER);
		}      
       break;
	case CARBUR_PAR:
		{
        CarburPar* p_in = (CarburPar*)i_values;
		p_params->ephh_hit    = p_in->ephh_hit;
        p_params->ephh_lot    = p_in->ephh_lot;
		p_params->carb_invers = p_in->carb_invers;
		}
      break;
	case IDLREG_PAR: 
		{
		IdlRegPar* p_in = (IdlRegPar*)i_values;
	    p_params->idl_regul  = p_in->idl_regul;
		p_params->idling_rpm = p_in->idling_rpm;
		p_params->MINEFR     = p_in->MINEFR;
		p_params->ifac1      = CNumericConv::Round(p_in->ifac1 * ANGLE_MULTIPLAYER);
		p_params->ifac2      = CNumericConv::Round(p_in->ifac2 * ANGLE_MULTIPLAYER);
		p_params->idlreg_min_angle = CNumericConv::Round(p_in->min_angle * ANGLE_MULTIPLAYER);
		p_params->idlreg_max_angle = CNumericConv::Round(p_in->max_angle * ANGLE_MULTIPLAYER);
		}
      break;
	case ANGLES_PAR:
		{
        AnglesPar* p_in = (AnglesPar*)i_values;
		p_params->angle_corr = CNumericConv::Round(p_in->angle_corr * ANGLE_MULTIPLAYER); 
        p_params->max_angle  = CNumericConv::Round(p_in->max_angle * ANGLE_MULTIPLAYER);
		p_params->min_angle  = CNumericConv::Round(p_in->min_angle * ANGLE_MULTIPLAYER);
		p_params->angle_dec_spead = CNumericConv::Round(p_in->dec_spead * ANGLE_MULTIPLAYER);
		p_params->angle_inc_spead = CNumericConv::Round(p_in->inc_spead * ANGLE_MULTIPLAYER);
		}
      break;
	case FUNSET_PAR:
	  	{
		FunSetPar* p_in = (FunSetPar*)i_values;
		p_params->fn_benzin = p_in->fn_benzin;
		p_params->fn_gas    = p_in->fn_gas;
		p_params->map_lower_pressure = CNumericConv::Round(p_in->map_lower_pressure * MAP_PHYSICAL_MAGNITUDE_MULTIPLAYER);
		p_params->map_upper_pressure = CNumericConv::Round(p_in->map_upper_pressure * MAP_PHYSICAL_MAGNITUDE_MULTIPLAYER);
		p_params->map_curve_offset = CNumericConv::Round(p_in->map_curve_offset / ADC_DISCRETE);
		p_params->map_curve_gradient = CNumericConv::Round(128.0f * p_in->map_curve_gradient * MAP_PHYSICAL_MAGNITUDE_MULTIPLAYER * ADC_DISCRETE);
		}
      break;
	case STARTR_PAR:
		{
        StartrPar* p_in = (StartrPar*)i_values;             
        p_params->starter_off  = p_in->starter_off ;
		p_params->smap_abandon = p_in->smap_abandon;
		}
      break;
	case ADCCOR_PAR:
		{
        ADCCompenPar* p_in = (ADCCompenPar*)i_values;
        p_params->map_adc_factor      = CNumericConv::Round(p_in->map_adc_factor * 16384);
        //-------------------------------------------------------------------------
        signed long map_correction_d  = CNumericConv::Round((-p_in->map_adc_correction) / ADC_DISCRETE); //переводим из вольтов в дискреты АЦП
        p_params->map_adc_correction  = CNumericConv::Round(16384 * (0.5f - map_correction_d * p_in->map_adc_factor));		
        //-------------------------------------------------------------------------

		p_params->temp_adc_factor     = CNumericConv::Round(p_in->temp_adc_factor * 16384);
        //-------------------------------------------------------------------------
        signed long temp_correction_d = CNumericConv::Round((-p_in->temp_adc_correction) / ADC_DISCRETE); //переводим из вольтов в дискреты АЦП
        p_params->temp_adc_correction = CNumericConv::Round(16384 * (0.5f - temp_correction_d * p_in->temp_adc_factor));
        //-------------------------------------------------------------------------

        p_params->ubat_adc_factor     = CNumericConv::Round(p_in->ubat_adc_factor * 16384);
        //-------------------------------------------------------------------------
        signed long ubat_correction_d = CNumericConv::Round((-p_in->ubat_adc_correction) / ADC_DISCRETE); //переводим из вольтов в дискреты АЦП
        p_params->ubat_adc_correction = CNumericConv::Round(16384 * (0.5f - ubat_correction_d * p_in->ubat_adc_factor));
        //-------------------------------------------------------------------------
		}
      break;
	case CKPS_PAR:
		{
        CKPSPar* p_in = (CKPSPar*)i_values; 
		p_params->ckps_cogs_btdc  = p_in->ckps_cogs_btdc;
        p_params->ckps_ignit_cogs = p_in->ckps_ignit_cogs;
		p_params->ckps_edge_type  = p_in->ckps_edge_type;
		}
      break;						      
    default:
      return false; //неизвестный или неподдерживаемый дескриптор
  }//switch 
  
  return true;
}

bool CFirmwareDataMediator::GetDefParamValues(BYTE i_descriptor, void* o_values)
{
  using namespace SECU3IO;

  BYTE* p_bytes = NULL;
  params* p_params = NULL; 
  p_bytes = m_bytes_active; 
  //получаем адрес структуры дефаултных параметров
  p_params = (params*)(p_bytes + DEFPARAM_START);

  //TODO: Remove these copy/paste! (conversions should be implemented in functions, as certain class)
  switch(i_descriptor)
  {
    case TEMPER_PAR:
		{
        TemperPar* p_out = (TemperPar*)o_values; 
	    p_out->tmp_use  = p_params->tmp_use;
	    p_out->vent_on  = ((float)p_params->vent_on) / TEMP_PHYSICAL_MAGNITUDE_MULTIPLAYER;
        p_out->vent_off = ((float)p_params->vent_off) / TEMP_PHYSICAL_MAGNITUDE_MULTIPLAYER;
		}
      break;
	case CARBUR_PAR:
		{
        CarburPar* p_out = (CarburPar*)o_values;
		p_out->ephh_hit    = p_params->ephh_hit;
        p_out->ephh_lot    = p_params->ephh_lot;
		p_out->carb_invers = p_params->carb_invers;
		}
      break;
	case IDLREG_PAR: 
		{
		IdlRegPar* p_out = (IdlRegPar*)o_values;
	    p_out->idl_regul  = p_params->idl_regul;
		p_out->idling_rpm = p_params->idling_rpm;
		p_out->MINEFR     = p_params->MINEFR;
		p_out->ifac1      = ((float)p_params->ifac1) / ANGLE_MULTIPLAYER;
		p_out->ifac2      = ((float)p_params->ifac2) / ANGLE_MULTIPLAYER;
		p_out->min_angle  = ((float)p_params->idlreg_min_angle) / ANGLE_MULTIPLAYER;
        p_out->max_angle  = ((float)p_params->idlreg_max_angle) / ANGLE_MULTIPLAYER;
		}
      break;
	case ANGLES_PAR:
		{
        AnglesPar* p_out = (AnglesPar*)o_values;
		p_out->angle_corr = ((float)p_params->angle_corr) / ANGLE_MULTIPLAYER; 
        p_out->max_angle  = ((float)p_params->max_angle)  / ANGLE_MULTIPLAYER;
		p_out->min_angle  = ((float)p_params->min_angle) / ANGLE_MULTIPLAYER;
		p_out->dec_spead = ((float)p_params->angle_dec_spead)  / ANGLE_MULTIPLAYER;
		p_out->inc_spead = ((float)p_params->angle_inc_spead)  / ANGLE_MULTIPLAYER;
		}
      break;
	case FUNSET_PAR:
		{
		FunSetPar* p_out = (FunSetPar*)o_values;
		p_out->fn_benzin = p_params->fn_benzin;
		p_out->fn_gas    = p_params->fn_gas;
		p_out->map_lower_pressure = ((float)p_params->map_lower_pressure) / MAP_PHYSICAL_MAGNITUDE_MULTIPLAYER;
		p_out->map_upper_pressure = ((float)p_params->map_upper_pressure) / MAP_PHYSICAL_MAGNITUDE_MULTIPLAYER;
		p_out->map_curve_offset = ((float)p_params->map_curve_offset) * ADC_DISCRETE;
        p_out->map_curve_gradient = ((float)p_params->map_curve_gradient) / (MAP_PHYSICAL_MAGNITUDE_MULTIPLAYER * ADC_DISCRETE * 128.0f);
		}
      break;
	case STARTR_PAR:
		{
        StartrPar* p_out = (StartrPar*)o_values;             
        p_out->starter_off = p_params->starter_off;
		p_out->smap_abandon = p_params->smap_abandon;
		}
      break;
	case ADCCOR_PAR:
		{
		ADCCompenPar* p_out = (ADCCompenPar*)o_values;
		p_out->map_adc_factor      = ((float)p_params->map_adc_factor) / 16384;
        //-------------------------------------------------------------------------
		p_out->map_adc_correction  = ((((float)p_params->map_adc_correction)/16384.0f) - 0.5f) / p_out->map_adc_factor;
        p_out->map_adc_correction*=ADC_DISCRETE; //в вольты 
        //-------------------------------------------------------------------------

        p_out->temp_adc_factor     = ((float)p_params->temp_adc_factor) / 16384;
        //-------------------------------------------------------------------------
		p_out->temp_adc_correction = ((((float)p_params->temp_adc_correction)/16384.0f) - 0.5f) / p_out->temp_adc_factor;
        p_out->temp_adc_correction*=ADC_DISCRETE; //в вольты 
        //-------------------------------------------------------------------------

        p_out->ubat_adc_factor     = ((float)p_params->ubat_adc_factor) / 16384; 
        //-------------------------------------------------------------------------
		p_out->ubat_adc_correction = ((((float)p_params->ubat_adc_correction)/16384.0f) - 0.5f) / p_out->ubat_adc_factor;
        p_out->ubat_adc_correction*=ADC_DISCRETE; //в вольты 
        //-------------------------------------------------------------------------
		}
      break;
    case CKPS_PAR:
		{
        CKPSPar* p_out = (CKPSPar*)o_values; 
		p_out->ckps_cogs_btdc  = p_params->ckps_cogs_btdc;
        p_out->ckps_ignit_cogs = p_params->ckps_ignit_cogs;
		p_out->ckps_edge_type  = p_params->ckps_edge_type;
		}
      break;					      
    default:
      return false; //неизвестный или неподдерживаемый дескриптор
  }//switch        

  return true;
}        

//выдает все таблицы в одной структуре
void CFirmwareDataMediator::GetMapsData(FWMapsDataHolder* op_fwd)
{
  std::vector<_TSTRING> names = GetFunctionsSetNames();
  for(int i = 0; i < TABLES_NUMBER; i++)
  {
   GetStartMap(i,op_fwd->maps[i].f_str);
   GetIdleMap(i,op_fwd->maps[i].f_idl);
   GetWorkMap(i,op_fwd->maps[i].f_wrk);
   GetTempMap(i,op_fwd->maps[i].f_tmp);
   op_fwd->maps[i].name = names[i];
  }
}

void CFirmwareDataMediator::SetMapsData(const FWMapsDataHolder* ip_fwd)
{
  for(int i = 0; i < TABLES_NUMBER; i++)
  {
   SetStartMap(i,ip_fwd->maps[i].f_str);
   SetIdleMap(i,ip_fwd->maps[i].f_idl);
   SetWorkMap(i,ip_fwd->maps[i].f_wrk);
   SetTempMap(i,ip_fwd->maps[i].f_tmp);
   SetFunctionsSetName(i,ip_fwd->maps[i].name);
  }
}

