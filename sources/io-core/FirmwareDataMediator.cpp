/* SECU-3  - An open source, free engine control unit
   Copyright (C) 2007 Alexey A. Shabelnikov. Ukraine, Gorlovka

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

   contacts:
              http://secu-3.org
              email: shabelnikov@secu-3.org
*/

#include "stdafx.h"
#include "FirmwareDataMediator.h"
#include "CRC16.h"
#include "ControlApp.h"    //should be removed - it is nearly unnecessary!
#include "common/MathHelpers.h"
#include "FirmwareMapsDataHolder.h"
#include "PlatformParamHolder.h"
#include "SECU3IO.h"
#include "SECU3ParametersDef.h"
#include "ufcodes.h"

//этот файл содержит платформенно ориентированные фрагменты кода
#pragma pack(1)

using namespace SECU3IO;
using namespace SECU3IO::SECU3Types;

typedef struct
{
 _char f_str[F_STR_POINTS];                       // функция УОЗ на старте
 _char f_idl[F_IDL_POINTS];                       // функция УОЗ для ХХ
 _char f_wrk[F_WRK_POINTS_L][F_WRK_POINTS_F];     // основная функция УОЗ (3D)
 _char f_tmp[F_TMP_POINTS];                       // функция коррект. УОЗ по температуре
 _uchar name[F_NAME_SIZE];                        // ассоциированное имя (имя семейства)
}F_data;


#define KC_ATTENUATOR_LOOKUP_TABLE_SIZE 128
#define FW_SIGNATURE_INFO_SIZE 48
#define COIL_ON_TIME_LOOKUP_TABLE_SIZE 32

//описывает дополнительные данные хранимые в прошивке
typedef struct
{
 _uchar fw_signature_info[FW_SIGNATURE_INFO_SIZE];

 //таблица усиления аттенюатора (зависимость от оборотов).
 _uchar attenuator_table[KC_ATTENUATOR_LOOKUP_TABLE_SIZE];

 //lookup table containing accumulation time for ignition coils (dependence from voltage)
 //таблица времени накопления энергии в катушках зажигания (зависимость от напряжения)
 _uint coil_on_time[COIL_ON_TIME_LOOKUP_TABLE_SIZE];

 //used for checking compatibility with management software. Holds size of all data stored in the firmware.
 //Includes CRC size also.
 _uint fw_data_size;

 //holds flags which give information about options were used to build firmware
 //(хранит флаги дающие информацию о том с какими опциями была скомпилирована прошивка)
 _ulong config;

 //Эти зарезервированные байты необходимы для сохранения бинарной совместимости
 //новых версий прошивок с более старыми версиями. При добавлении новых данных
 //в структуру, необходимо расходовать эти байты.
 _uchar reserved[58];
}FirmwareData;

class CFirmwareDataMediator::LocInfoProvider
 {
  public:
   LocInfoProvider(const PPFlashParam& i_fpp)
    : CODE_CRC_SIZE(sizeof(_uint))
    , BOOT_START(i_fpp.m_app_section_size)
    , CODE_CRC_ADDR(BOOT_START - CODE_CRC_SIZE)
    , CODE_SIZE(BOOT_START - CODE_CRC_SIZE)
    , TABLES_START(CODE_CRC_ADDR - (sizeof(F_data)*TABLES_NUMBER))
    , DEFPARAM_START(TABLES_START - sizeof(params))
    , FIRMWARE_DATA_START(DEFPARAM_START - sizeof(FirmwareData))
    , FIRMWARE_ALL_DATA_START(FIRMWARE_DATA_START)
    , FIRMWARE_ALL_DATA_SIZE((sizeof(F_data)*TABLES_NUMBER) + sizeof(params) + sizeof(FirmwareData))
   {
    //empty
   }

   //size of check sum in bytes
   const size_t CODE_CRC_SIZE;
   //starting address of boot loader section
   const size_t BOOT_START;
   //starting address of check sum
   const size_t CODE_CRC_ADDR;
   //размер кода прошивки без учета байтов контрольной суммы
   const size_t CODE_SIZE;
   //адрес массива таблиц - семейств характеристик
   const size_t TABLES_START;
   //адрес структуры дефаултных параметров (параметров EEPROM по умолчанию)
   const size_t DEFPARAM_START;
   //адрес дополнительных параметров
   const size_t FIRMWARE_DATA_START;
   //---два нижних определения необходимы для экспорта/импорта данных между прошивками--
   //стартовый адрес всех данных прошивки в байтах
   const size_t FIRMWARE_ALL_DATA_START;
   //размер всех данных прошивки без учета байтов контрольной суммы прошивки
   const size_t FIRMWARE_ALL_DATA_SIZE;
 }; 

CFirmwareDataMediator::CFirmwareDataMediator(const PPFlashParam& i_fpp)
: m_firmware_size(i_fpp.m_total_size)
, m_is_opened(false)
, m_fpp(new PPFlashParam(i_fpp))
, m_lip(new LocInfoProvider(i_fpp))
{
 m_bytes_active = new BYTE[m_fpp->m_total_size + 1];
 ASSERT(m_bytes_active);
 m_bytes_original = new BYTE[m_fpp->m_total_size + 1];
 ASSERT(m_bytes_original);

 memset(m_bytes_active, 0x00, m_firmware_size);
 memset(m_bytes_original, 0x00, m_firmware_size);
}

CFirmwareDataMediator::~CFirmwareDataMediator()
{
 delete m_bytes_active;
 delete m_bytes_original;
}

//считает контрольную сумму и записывает результат по соответствующему адресу
//io_data - массив байтов прошивки
void CFirmwareDataMediator::CalculateAndPlaceFirmwareCRC(BYTE* io_data)
{
 _uint crc = crc16(io_data, m_lip->CODE_SIZE);
 _uint* crc_addr = (_uint*)(&io_data[m_lip->CODE_CRC_ADDR]);
 *crc_addr = crc; //сохранили контрольную сумму
}

bool CFirmwareDataMediator::CheckCompatibility(const BYTE* i_data) const
{
 bool compatible = true;

 FirmwareData* p_fwdata = (FirmwareData*)&i_data[m_lip->FIRMWARE_DATA_START];

 if ((m_lip->FIRMWARE_ALL_DATA_SIZE + m_lip->CODE_CRC_SIZE) != p_fwdata->fw_data_size)
  compatible = false;

 return compatible;
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
 BYTE* addr = &m_bytes_active[m_lip->FIRMWARE_DATA_START];
 memcpy(raw_string,addr,FW_SIGNATURE_INFO_SIZE);
 TCHAR string[256];
 OemToChar(raw_string,string);
 return _TSTRING(string);
}

void CFirmwareDataMediator::SetSignatureInfo(_TSTRING i_string)
{
 char raw_string[256];
 memset(raw_string,0,FW_SIGNATURE_INFO_SIZE+1);
 BYTE* addr = &m_bytes_active[m_lip->FIRMWARE_DATA_START];
 CharToOem(i_string.c_str(),raw_string);
 memcpy(addr,raw_string,FW_SIGNATURE_INFO_SIZE);
}

DWORD CFirmwareDataMediator::GetFWOptions(void)
{
 BYTE* addr = &m_bytes_active[m_lip->FIRMWARE_DATA_START];
 FirmwareData* data= (FirmwareData*)addr;
 return data->config;
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
 p_maps = (F_data*)(p_bytes + m_lip->TABLES_START);

 for (int i = 0; i < F_STR_POINTS; i++ )
  o_values[i] = ((float)p_maps[i_index].f_str[i]) / AA_MAPS_M_FACTOR;
}

void CFirmwareDataMediator::SetStartMap(int i_index,const float* i_values)
{
 BYTE* p_bytes = NULL;
 F_data* p_maps = NULL;
 ASSERT(i_values);

 p_bytes = m_bytes_active;

 //получаем адрес начала таблиц семейств характеристик
 p_maps = (F_data*)(p_bytes + m_lip->TABLES_START);

 for (int i = 0; i < F_STR_POINTS; i++ )
  p_maps[i_index].f_str[i] = MathHelpers::Round((i_values[i]*AA_MAPS_M_FACTOR));
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
 p_maps = (F_data*)(p_bytes + m_lip->TABLES_START);

 for (int i = 0; i < F_IDL_POINTS; i++ )
  o_values[i] = ((float)p_maps[i_index].f_idl[i]) / AA_MAPS_M_FACTOR;
}

void CFirmwareDataMediator::SetIdleMap(int i_index,const float* i_values)
{
 BYTE* p_bytes = NULL;
 F_data* p_maps = NULL;
 ASSERT(i_values);

 p_bytes = m_bytes_active;

 //получаем адрес начала таблиц семейств характеристик
 p_maps = (F_data*)(p_bytes + m_lip->TABLES_START);

 for (int i = 0; i < F_IDL_POINTS; i++ )
  p_maps[i_index].f_idl[i] = MathHelpers::Round((i_values[i]*AA_MAPS_M_FACTOR));
}

std::vector<_TSTRING> CFirmwareDataMediator::GetFunctionsSetNames(void)
{
 std::vector<_TSTRING> names(TABLES_NUMBER);

 BYTE* p_bytes = NULL;
 F_data* p_maps = NULL;

 p_bytes = m_bytes_active;

 //получаем адрес начала таблиц семейств характеристик
 p_maps = (F_data*)(p_bytes + m_lip->TABLES_START);

 for (int i = 0; i < TABLES_NUMBER; i++)
 {
  char raw_string[256];
  memset(raw_string, 0, F_NAME_SIZE+1);
  BYTE* addr = p_maps[i].name;
  memcpy(raw_string, addr, F_NAME_SIZE);
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
 p_maps = (F_data*)(p_bytes + m_lip->TABLES_START);

 //дополняем строку пробелами, так чтобы если длина строки меньше F_NAME_SIZE,
 //остальные символы были пробелами
 i_new_name+= _T("                ");

 char raw_string[256];
 memset(raw_string, 0, FW_SIGNATURE_INFO_SIZE+1);
 CharToOem(i_new_name.c_str(), raw_string);
 memcpy(p_maps[i_index].name, raw_string, F_NAME_SIZE);
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
 _uint crc_16 = crc16(m_bytes_active,m_lip->CODE_SIZE);
 return crc_16;
}

unsigned int CFirmwareDataMediator::GetCRC16StoredInActiveFirmware(void)
{
 _uint* crc_16_addr = (_uint*)(&m_bytes_active[m_lip->CODE_CRC_ADDR]);
 return *crc_16_addr;
}

void CFirmwareDataMediator::CalculateAndPlaceFirmwareCRC(void)
{
 CalculateAndPlaceFirmwareCRC(m_bytes_active);
}

void CFirmwareDataMediator::LoadDataBytesFromAnotherFirmware(const BYTE* i_source_bytes, const PPFlashParam* ip_fpp /*= NULL*/)
{
 if (false==IsLoaded())
  return; //некуда загружать...

 if (ip_fpp)
 {
  LocInfoProvider lip(*ip_fpp);
  ASSERT(m_lip->FIRMWARE_ALL_DATA_SIZE == lip.FIRMWARE_ALL_DATA_SIZE);
  memcpy(m_bytes_active + m_lip->FIRMWARE_ALL_DATA_START, i_source_bytes + lip.FIRMWARE_ALL_DATA_START, m_lip->FIRMWARE_ALL_DATA_SIZE);
 }
 else
  memcpy(m_bytes_active + m_lip->FIRMWARE_ALL_DATA_START, i_source_bytes + m_lip->FIRMWARE_ALL_DATA_START, m_lip->FIRMWARE_ALL_DATA_SIZE);
}

void CFirmwareDataMediator::LoadDefParametersFromBuffer(const BYTE* i_source_bytes)
{
 if (false==IsLoaded())
  return; //некуда загружать...
 memcpy(m_bytes_active + m_lip->DEFPARAM_START, i_source_bytes, sizeof(params));
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
 p_maps = (F_data*)(p_bytes + m_lip->TABLES_START);

 for (int i = 0; i < (F_WRK_POINTS_F * F_WRK_POINTS_L); i++ )
 {
  _char *p = &(p_maps[i_index].f_wrk[0][0]);
  o_values[i] = ((float) *(p + i)) / AA_MAPS_M_FACTOR;
 }
}

void CFirmwareDataMediator::SetWorkMap(int i_index, const float* i_values)
{
 BYTE* p_bytes = NULL;
 F_data* p_maps = NULL;
 ASSERT(i_values);

 p_bytes = m_bytes_active;

 //получаем адрес начала таблиц семейств характеристик
 p_maps = (F_data*)(p_bytes + m_lip->TABLES_START);

 for (int i = 0; i < (F_WRK_POINTS_F * F_WRK_POINTS_L); i++ )
 {
  _char *p = &(p_maps[i_index].f_wrk[0][0]);
  *(p + i) = MathHelpers::Round((i_values[i]*AA_MAPS_M_FACTOR));
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
 p_maps = (F_data*)(p_bytes + m_lip->TABLES_START);

 for (int i = 0; i < F_TMP_POINTS; i++ )
  o_values[i] = ((float)p_maps[i_index].f_tmp[i]) / AA_MAPS_M_FACTOR;
}

void CFirmwareDataMediator::SetTempMap(int i_index,const float* i_values)
{
 BYTE* p_bytes = NULL;
 F_data* p_maps = NULL;
 ASSERT(i_values);

 p_bytes = m_bytes_active;

 //получаем адрес начала таблиц семейств характеристик
 p_maps = (F_data*)(p_bytes + m_lip->TABLES_START);

 for (int i = 0; i < F_TMP_POINTS; i++ )
  p_maps[i_index].f_tmp[i] = MathHelpers::Round((i_values[i]*AA_MAPS_M_FACTOR));
}

bool CFirmwareDataMediator::SetDefParamValues(BYTE i_descriptor, const void* i_values)
{
 using namespace SECU3IO;

 BYTE* p_bytes = NULL;
 params* p_params = NULL;
 p_bytes = m_bytes_active;
 //получаем адрес структуры дефаултных параметров
 p_params = (params*)(p_bytes + m_lip->DEFPARAM_START);

 //TODO: Remove these copy/paste! (conversions should be implemented in functions, as certain class)
 switch(i_descriptor)
 {
  case TEMPER_PAR:
   {
    TemperPar* p_in = (TemperPar*)i_values;
    p_params->tmp_use  = p_in->tmp_use;
    p_params->vent_pwm = p_in->vent_pwm;
    p_params->vent_on  = MathHelpers::Round(p_in->vent_on * TEMP_PHYSICAL_MAGNITUDE_MULTIPLAYER);
    p_params->vent_off = MathHelpers::Round(p_in->vent_off * TEMP_PHYSICAL_MAGNITUDE_MULTIPLAYER);
   }
   break;
  case CARBUR_PAR:
   {
    CarburPar* p_in = (CarburPar*)i_values;
    p_params->ephh_hit    = p_in->ephh_hit;
    p_params->ephh_lot    = p_in->ephh_lot;
    p_params->carb_invers = p_in->carb_invers;
    p_params->epm_on_threshold = MathHelpers::Round(p_in->epm_ont * MAP_PHYSICAL_MAGNITUDE_MULTIPLAYER);
    p_params->ephh_hit_g  = p_in->ephh_hit_g;
    p_params->ephh_lot_g  = p_in->ephh_lot_g;
    p_params->shutoff_delay = MathHelpers::Round(p_in->shutoff_delay * 100); //переводим в десятки мс
   }
   break;
  case IDLREG_PAR:
   {
    IdlRegPar* p_in = (IdlRegPar*)i_values;
    p_params->idl_regul  = p_in->idl_regul;
    p_params->idling_rpm = p_in->idling_rpm;
    p_params->MINEFR     = p_in->MINEFR;
    p_params->ifac1      = MathHelpers::Round(p_in->ifac1 * ANGLE_MULTIPLAYER);
    p_params->ifac2      = MathHelpers::Round(p_in->ifac2 * ANGLE_MULTIPLAYER);
    p_params->idlreg_min_angle = MathHelpers::Round(p_in->min_angle * ANGLE_MULTIPLAYER);
    p_params->idlreg_max_angle = MathHelpers::Round(p_in->max_angle * ANGLE_MULTIPLAYER);
   }
   break;
  case ANGLES_PAR:
   {
    AnglesPar* p_in = (AnglesPar*)i_values;
    p_params->angle_corr = MathHelpers::Round(p_in->angle_corr * ANGLE_MULTIPLAYER);
    p_params->max_angle  = MathHelpers::Round(p_in->max_angle * ANGLE_MULTIPLAYER);
    p_params->min_angle  = MathHelpers::Round(p_in->min_angle * ANGLE_MULTIPLAYER);
    p_params->angle_dec_spead = MathHelpers::Round(p_in->dec_spead * ANGLE_MULTIPLAYER);
    p_params->angle_inc_spead = MathHelpers::Round(p_in->inc_spead * ANGLE_MULTIPLAYER);
   }
   break;
  case FUNSET_PAR:
   {
    FunSetPar* p_in = (FunSetPar*)i_values;
    p_params->fn_benzin = p_in->fn_benzin;
    p_params->fn_gas    = p_in->fn_gas;
    p_params->map_lower_pressure = MathHelpers::Round(p_in->map_lower_pressure * MAP_PHYSICAL_MAGNITUDE_MULTIPLAYER);
    p_params->map_upper_pressure = MathHelpers::Round(p_in->map_upper_pressure * MAP_PHYSICAL_MAGNITUDE_MULTIPLAYER);
    p_params->map_curve_offset = MathHelpers::Round(p_in->map_curve_offset / ADC_DISCRETE);
    p_params->map_curve_gradient = MathHelpers::Round(128.0f * p_in->map_curve_gradient * MAP_PHYSICAL_MAGNITUDE_MULTIPLAYER * ADC_DISCRETE);
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
    p_params->map_adc_factor      = MathHelpers::Round(p_in->map_adc_factor * 16384);
    //-------------------------------------------------------------------------
    signed long map_correction_d  = MathHelpers::Round((-p_in->map_adc_correction) / ADC_DISCRETE); //переводим из вольтов в дискреты АЦП
    p_params->map_adc_correction  = MathHelpers::Round(16384 * (0.5f - map_correction_d * p_in->map_adc_factor));
    //-------------------------------------------------------------------------

    p_params->temp_adc_factor     = MathHelpers::Round(p_in->temp_adc_factor * 16384);
    //-------------------------------------------------------------------------
    signed long temp_correction_d = MathHelpers::Round((-p_in->temp_adc_correction) / ADC_DISCRETE); //переводим из вольтов в дискреты АЦП
    p_params->temp_adc_correction = MathHelpers::Round(16384 * (0.5f - temp_correction_d * p_in->temp_adc_factor));
    //-------------------------------------------------------------------------

    p_params->ubat_adc_factor     = MathHelpers::Round(p_in->ubat_adc_factor * 16384);
    //-------------------------------------------------------------------------
    signed long ubat_correction_d = MathHelpers::Round((-p_in->ubat_adc_correction) / ADC_DISCRETE); //переводим из вольтов в дискреты АЦП
    p_params->ubat_adc_correction = MathHelpers::Round(16384 * (0.5f - ubat_correction_d * p_in->ubat_adc_factor));
    //-------------------------------------------------------------------------
   }
   break;
  case CKPS_PAR:
   {
    CKPSPar* p_in = (CKPSPar*)i_values;
    p_params->ckps_cogs_btdc  = p_in->ckps_cogs_btdc;
    p_params->ckps_ignit_cogs = p_in->ckps_ignit_cogs;
    p_params->ckps_edge_type  = p_in->ckps_edge_type;
    p_params->ckps_engine_cyl = p_in->ckps_engine_cyl;
   }
   break;
  case KNOCK_PAR:
   {
    KnockPar* p_in = (KnockPar*)i_values;
    p_params->knock_use_knock_channel = p_in->knock_use_knock_channel;
    p_params->knock_k_wnd_begin_angle = MathHelpers::Round(p_in->knock_k_wnd_begin_angle * ANGLE_MULTIPLAYER);
    p_params->knock_k_wnd_end_angle = MathHelpers::Round(p_in->knock_k_wnd_end_angle  * ANGLE_MULTIPLAYER);
    ASSERT(p_in->knock_bpf_frequency >= 0.0f);
    p_params->knock_bpf_frequency = MathHelpers::Round(p_in->knock_bpf_frequency);
    p_params->knock_int_time_const = p_in->knock_int_time_const;

    p_params->knock_retard_step = MathHelpers::Round(p_in->knock_retard_step * ANGLE_MULTIPLAYER);
    p_params->knock_advance_step = MathHelpers::Round(p_in->knock_advance_step * ANGLE_MULTIPLAYER);
    p_params->knock_max_retard = MathHelpers::Round(p_in->knock_max_retard * ANGLE_MULTIPLAYER);
    p_params->knock_threshold = MathHelpers::Round(p_in->knock_threshold / ADC_DISCRETE);
    p_params->knock_recovery_delay = p_in->knock_recovery_delay;
   }
   break;
  case MISCEL_PAR:
   {
    MiscelPar* p_in = (MiscelPar*)i_values;

    int divisor = 0;
    for(size_t i = 0; i < SECU3IO::SECU3_ALLOWABLE_UART_DIVISORS_COUNT; ++i)
     if (SECU3IO::secu3_allowable_uart_divisors[i].first == p_in->baud_rate)
      divisor = SECU3IO::secu3_allowable_uart_divisors[i].second;
    if (0==divisor)
    {
     secu3_allowable_uart_divisors[0].second;
     ASSERT(0);
    }

    p_params->uart_divisor = divisor;
    p_params->uart_period_t_ms = p_in->period_ms / 10;
    p_params->ign_cutoff = p_in->ign_cutoff;
    p_params->ign_cutoff_thrd = p_in->ign_cutoff_thrd;
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
 p_params = (params*)(p_bytes + m_lip->DEFPARAM_START);

 //TODO: Remove these copy/paste! (conversions should be implemented in functions, as certain class)
 switch(i_descriptor)
  {
   case TEMPER_PAR:
    {
     TemperPar* p_out = (TemperPar*)o_values;
     p_out->tmp_use  = p_params->tmp_use;
     p_out->vent_pwm = p_params->vent_pwm;
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
     p_out->epm_ont =  ((float)p_params->epm_on_threshold) / MAP_PHYSICAL_MAGNITUDE_MULTIPLAYER;
     p_out->ephh_hit_g  = p_params->ephh_hit_g;
     p_out->ephh_lot_g  = p_params->ephh_lot_g;
     p_out->shutoff_delay = ((float)p_params->shutoff_delay) / 100.0f; //переводим в секунды
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
     p_out->ckps_engine_cyl = p_params->ckps_engine_cyl;
    }
    break;
   case KNOCK_PAR:
    {
     KnockPar* p_out = (KnockPar*)o_values;
     p_out->knock_use_knock_channel = p_params->knock_use_knock_channel;
     p_out->knock_k_wnd_begin_angle = ((float)p_params->knock_k_wnd_begin_angle) / ANGLE_MULTIPLAYER;
     p_out->knock_k_wnd_end_angle = ((float)p_params->knock_k_wnd_end_angle) / ANGLE_MULTIPLAYER;
     p_out->knock_bpf_frequency = p_params->knock_bpf_frequency;
     p_out->knock_int_time_const = p_params->knock_int_time_const;

     p_out->knock_retard_step = ((float)p_params->knock_retard_step) / ANGLE_MULTIPLAYER;
     p_out->knock_advance_step = ((float)p_params->knock_advance_step) / ANGLE_MULTIPLAYER;
     p_out->knock_max_retard = ((float)p_params->knock_max_retard) / ANGLE_MULTIPLAYER;
     p_out->knock_threshold = ((float)p_params->knock_threshold) * ADC_DISCRETE;
     p_out->knock_recovery_delay = p_params->knock_recovery_delay;
    }
    break;
   case MISCEL_PAR:
    {
     MiscelPar* p_out = (MiscelPar*)o_values;

     int baud_rate = 0;
     for(size_t i = 0; i < SECU3IO::SECU3_ALLOWABLE_UART_DIVISORS_COUNT; ++i)
     if (SECU3IO::secu3_allowable_uart_divisors[i].second == p_params->uart_divisor)
      baud_rate = SECU3IO::secu3_allowable_uart_divisors[i].first;
     ASSERT(baud_rate);

     p_out->baud_rate = baud_rate;
     p_out->period_ms = p_params->uart_period_t_ms * 10;
     p_out->ign_cutoff = p_params->ign_cutoff;
     p_out->ign_cutoff_thrd = p_params->ign_cutoff_thrd;
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
 //separate tables
 GetAttenuatorMap(op_fwd->attenuator_table);
 GetCoilRegulMap(op_fwd->coilregul_table);
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
 //separate tables
 SetAttenuatorMap(ip_fwd->attenuator_table);
 SetCoilRegulMap(ip_fwd->coilregul_table);
}

void CFirmwareDataMediator::GetAttenuatorMap(float* o_values, bool i_original /* = false */)
{
 BYTE* p_bytes = NULL;
 FirmwareData* p_fw_data = NULL;
 ASSERT(o_values);
 if (!o_values)
  return;

 if (i_original)
  p_bytes = m_bytes_original;
 else
  p_bytes = m_bytes_active;

 //получаем адрес структуры дополнительных данных
 p_fw_data = (FirmwareData*)(p_bytes + m_lip->FIRMWARE_DATA_START);

 for(size_t i = 0; i < KC_ATTENUATOR_LOOKUP_TABLE_SIZE; i++)
  o_values[i] = p_fw_data->attenuator_table[i];
}

void CFirmwareDataMediator::SetAttenuatorMap(const float* i_values)
{
 BYTE* p_bytes = NULL;
 FirmwareData* p_fw_data = NULL;
 ASSERT(i_values);
 if (!i_values)
  return;

 p_bytes = m_bytes_active;

 //получаем адрес структуры дополнительных данных
 p_fw_data = (FirmwareData*)(p_bytes + m_lip->FIRMWARE_DATA_START);

 for(size_t i = 0; i < KC_ATTENUATOR_LOOKUP_TABLE_SIZE; i++)
 {
  ASSERT(i_values[i] >= 0.0f);
  p_fw_data->attenuator_table[i] = (_uchar)MathHelpers::Round(i_values[i]);
 }
}

void CFirmwareDataMediator::GetCoilRegulMap(float* o_values, bool i_original /* = false */)
{
 BYTE* p_bytes = NULL;
 FirmwareData* p_fw_data = NULL;
 ASSERT(o_values);
 if (!o_values)
  return;

 if (i_original)
  p_bytes = m_bytes_original;
 else
  p_bytes = m_bytes_active;

 //получаем адрес структуры дополнительных данных
 p_fw_data = (FirmwareData*)(p_bytes + m_lip->FIRMWARE_DATA_START);

 for(size_t i = 0; i < COIL_ON_TIME_LOOKUP_TABLE_SIZE; i++)
  o_values[i] = (p_fw_data->coil_on_time[i] * 4.0f) / 1000.0f; //convert to ms, discrete = 4us
}

void CFirmwareDataMediator::SetCoilRegulMap(const float* i_values)
{
 BYTE* p_bytes = NULL;
 FirmwareData* p_fw_data = NULL;
 ASSERT(i_values);
 if (!i_values)
  return;

 p_bytes = m_bytes_active;

 //получаем адрес структуры дополнительных данных
 p_fw_data = (FirmwareData*)(p_bytes + m_lip->FIRMWARE_DATA_START);

 for(size_t i = 0; i < COIL_ON_TIME_LOOKUP_TABLE_SIZE; i++)
  p_fw_data->coil_on_time[i] = (_uint)MathHelpers::Round((i_values[i] * 1000.0) / 4.0);
}

const PPFlashParam& CFirmwareDataMediator::GetPlatformParams(void) const
{
 return *m_fpp;
}
