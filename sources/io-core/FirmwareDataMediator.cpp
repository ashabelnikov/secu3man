/* SECU-3  - An open source, free engine control unit
   Copyright (C) 2007 Alexey A. Shabelnikov. Ukraine, Kiev

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
#include <map>
#include "FirmwareDataMediator.h"
#include "CRC16.h"
#include "ControlApp.h"    //should be removed - it is nearly unnecessary!
#include "common/MathHelpers.h"
#include "FirmwareMapsDataHolder.h"
#include "PlatformParamHolder.h"
#include "SECU3IO.h"
#include "SECU3ParametersDef.h"
#include "SECU3TablesDef.h"
#include "ufcodes.h"

//этот файл содержит платформенно-ориентированные фрагменты кода
#pragma pack(1)

using namespace SECU3IO;
using namespace SECU3IO::SECU3Types;

#define KC_ATTENUATOR_LOOKUP_TABLE_SIZE  128
#define FW_SIGNATURE_INFO_SIZE           48
#define COIL_ON_TIME_LOOKUP_TABLE_SIZE   32
#define THERMISTOR_LOOKUP_TABLE_SIZE     16
#define ATS_CORR_LOOKUP_TABLE_SIZE       16
#define RPM_GRID_SIZE                    16

#define IOREM_MAJ_VER(v) (((v) >> 4) & 0xf)

//------------------------For iorem_slots_t V2.1----------------------------
#define IOREM_SLOTS 25           // Number of slots used for I/O remapping
#define IOREM_PLUGS 56           // Number of plugs used in I/O remapping

//Describes all data related to I/O remapping
typedef struct iorem_slots_t
{
 _fnptr_t i_slots[IOREM_SLOTS];  // initialization slots
 _fnptr_t i_slotsi[IOREM_SLOTS]; // initialization slots (inverted)
 _fnptr_t v_slots[IOREM_SLOTS];  // data slots
 _fnptr_t v_slotsi[IOREM_SLOTS]; // data slots           (inverted)
 _fnptr_t i_plugs[IOREM_PLUGS];  // initialization plugs
 _fnptr_t v_plugs[IOREM_PLUGS];  // data plugs
 _fnptr_t s_stub;                // special pointer used as stub (for outputs)
 _fnptr_t g_stub;                // special pointer used as stub (for inputs)
 _uchar version;                 // A reserved byte
 _uint size;                     // size of this structure
}iorem_slots_t;

//Describes data stored directly in the firmware code
typedef struct cd_data_t
{
 // Arrays which are used for I/O remapping. Some arrays are "slots", some are "plugs"
 iorem_slots_t iorem;

 //holds flags which give information about options were used to build firmware
 //(хранит флаги дающие информацию о том с какими опциями была скомпилирована прошивка)
 _ulong config;

 _uchar reserved[4];             //Reserved bytes

 _uint size;                     // size of this structure (2 bytes)
}cd_data_t;

//описывает дополнительные данные хранимые в прошивке
typedef struct
{
 //таблица усиления аттенюатора (зависимость от оборотов).
 _uchar attenuator_table[KC_ATTENUATOR_LOOKUP_TABLE_SIZE];

 //lookup table containing accumulation time for ignition coils (dependence from voltage)
 //таблица времени накопления энергии в катушках зажигания (зависимость от напряжения)
 _uint coil_on_time[COIL_ON_TIME_LOOKUP_TABLE_SIZE];

 //Coolant temperature sensor lookup table (таблица значений температуры с шагом по напряжению)
 _int cts_curve[THERMISTOR_LOOKUP_TABLE_SIZE];
 //Related voltage limits
 _uint cts_vl_begin;
 _uint cts_vl_end;

 //Choke closing versus coolant temperature
 _uchar choke_closing[CHOKE_CLOSING_LOOKUP_TABLE_SIZE];

 //Air temperature sensor lookup table
 _int ats_curve[THERMISTOR_LOOKUP_TABLE_SIZE];
 //Voltage corresponding to the beginning of axis
 _uint ats_vl_begin;
 //Voltage corresponding to the end of axis
 _uint ats_vl_end;

 //Air temperature correction of advance angle
 _char ats_corr[ATS_CORR_LOOKUP_TABLE_SIZE];

 //Points of the RPM grid
 _int rpm_grid_points[RPM_GRID_SIZE];
 //Sizes of cells in RPM grid (so, we don't need to calculate them at the runtime)
 _int rpm_grid_sizes[RPM_GRID_SIZE-1];

 //Эти зарезервированные байты необходимы для сохранения бинарной совместимости
 //новых версий прошивок с более старыми версиями. При добавлении новых данных
 //в структуру, необходимо расходовать эти байты.
 _uchar reserved[2048];
}fw_ex_data_t;

//Describes all data residing in the firmware
typedef struct fw_data_t
{
 params_t     def_param;                 // Reserve parameters (loaded when instance in EEPROM is broken)
 fw_ex_data_t exdata;                    // Additional data in the firmware
 f_data_t     tables[TABLES_NUMBER];     // Array of tables of advance angle
 //Информация о прошивке
 _uchar       fw_signature_info[FW_SIGNATURE_INFO_SIZE];

 _uchar       version;                   //version of this structure

 //used for checking compatibility with management software. Holds size of all data stored in the firmware.
 //Includes CRC size also.
 _uint fw_data_size;

 _uint        code_crc;                  // Check sum of the whole firmware (except this check sum and boot loader)
}fw_data_t;


namespace {
void _CompensateVRef(params_t* iop_data, bool i_dir)
{
 const float adc_vref_factor = 1.9531f;
 const int _min = 0, _max = SHRT_MAX;
 if (i_dir)
 { //5v <-- 2.56v
  iop_data->map_adc_factor  = MathHelpers::RestrictValue<int>(MathHelpers::Round(iop_data->map_adc_factor * adc_vref_factor),  _min, _max);
  iop_data->ubat_adc_factor = MathHelpers::RestrictValue<int>(MathHelpers::Round(iop_data->ubat_adc_factor * adc_vref_factor), _min, _max); 
  iop_data->temp_adc_factor = MathHelpers::RestrictValue<int>(MathHelpers::Round(iop_data->temp_adc_factor * adc_vref_factor), _min, _max);
  //SECU-3T
  iop_data->tps_adc_factor  = MathHelpers::RestrictValue<int>(MathHelpers::Round(iop_data->tps_adc_factor * adc_vref_factor), _min, _max);
  iop_data->ai1_adc_factor  = MathHelpers::RestrictValue<int>(MathHelpers::Round(iop_data->ai1_adc_factor * adc_vref_factor), _min, _max);
  iop_data->ai2_adc_factor  = MathHelpers::RestrictValue<int>(MathHelpers::Round(iop_data->ai2_adc_factor * adc_vref_factor), _min, _max);
 }
 else
 { //5v --> 2.56v
  iop_data->map_adc_factor  = MathHelpers::RestrictValue<int>(MathHelpers::Round(iop_data->map_adc_factor / adc_vref_factor),  _min, _max);
  iop_data->ubat_adc_factor = MathHelpers::RestrictValue<int>(MathHelpers::Round(iop_data->ubat_adc_factor / adc_vref_factor), _min, _max);
  iop_data->temp_adc_factor = MathHelpers::RestrictValue<int>(MathHelpers::Round(iop_data->temp_adc_factor / adc_vref_factor), _min, _max);
  //SECU-3T
  iop_data->tps_adc_factor  = MathHelpers::RestrictValue<int>(MathHelpers::Round(iop_data->tps_adc_factor / adc_vref_factor), _min, _max);
  iop_data->ai1_adc_factor  = MathHelpers::RestrictValue<int>(MathHelpers::Round(iop_data->ai1_adc_factor / adc_vref_factor), _min, _max);
  iop_data->ai2_adc_factor  = MathHelpers::RestrictValue<int>(MathHelpers::Round(iop_data->ai2_adc_factor / adc_vref_factor), _min, _max);
 }
}
}

class CFirmwareDataMediator::LocInfoProvider
 {
  public:
   LocInfoProvider(const PPFlashParam& i_fpp)
   : CODE_CRC_SIZE(sizeof(fw_data_t().code_crc))
   , BOOT_START(i_fpp.m_app_section_size)
   , CODE_SIZE(BOOT_START - CODE_CRC_SIZE)
   , FIRMWARE_DATA_START(BOOT_START - sizeof(fw_data_t))
   , FIRMWARE_DATA_SIZE(sizeof(fw_data_t) - CODE_CRC_SIZE)
   {
    //empty
   }

   //size of check sum in bytes
   const size_t CODE_CRC_SIZE;
   //starting address of boot loader section
   const size_t BOOT_START;
   //размер кода прошивки без учета байтов контрольной суммы
   const size_t CODE_SIZE;
   //стартовый адрес всех данных прошивки в байтах
   const size_t FIRMWARE_DATA_START;
   //размер всех данных прошивки без учета байтов контрольной суммы прошивки
   const size_t FIRMWARE_DATA_SIZE;
 };

CFirmwareDataMediator::CFirmwareDataMediator(const PPFlashParam& i_fpp)
: m_firmware_size(i_fpp.m_total_size)
, m_is_opened(false)
, m_fpp(new PPFlashParam(i_fpp))
, m_lip(new LocInfoProvider(i_fpp))
, mp_cddata(NULL)
, m_period_distance(0.166666f)//for speed sensor calculations
, m_quartz_frq(20000000)      //default clock is 20mHz
{
 mp_bytes_active = new BYTE[m_fpp->m_total_size + 1];
 ASSERT(mp_bytes_active);
 mp_bytes_original = new BYTE[m_fpp->m_total_size + 1];
 ASSERT(mp_bytes_original);

 memset(mp_bytes_active, 0x00, m_firmware_size);
 memset(mp_bytes_original, 0x00, m_firmware_size);
}

CFirmwareDataMediator::~CFirmwareDataMediator()
{
 delete[] mp_bytes_active;
 delete[] mp_bytes_original;
}

//считает контрольную сумму и записывает результат по соответствующему адресу
//io_data - массив байтов прошивки
void CFirmwareDataMediator::CalculateAndPlaceFirmwareCRC(BYTE* iop_data)
{
 _uint crc = crc16(iop_data, m_lip->CODE_SIZE);
 fw_data_t* p_fd = (fw_data_t*)(&iop_data[m_lip->FIRMWARE_DATA_START]);
 p_fd->code_crc = crc; //сохранили контрольную сумму
}

bool CFirmwareDataMediator::CheckCompatibility(const BYTE* ip_data, const PPFlashParam* ip_fpp /*= NULL*/) const
{
 if (!ip_fpp)
  ip_fpp = m_fpp.get(); //use information form current firmware

 LocInfoProvider lip(*ip_fpp);

 bool compatible = true;

 fw_data_t* p_fd = (fw_data_t*)&ip_data[lip.FIRMWARE_DATA_START]; //firmware data
 cd_data_t* p_cd = (cd_data_t*)&ip_data[lip.FIRMWARE_DATA_START - sizeof(cd_data_t)]; //code data

 //size of code area data
 _uint sizeofcd = *(((_uint*)&ip_data[lip.FIRMWARE_DATA_START])-1);

 if ((sizeof(fw_data_t)) != p_fd->fw_data_size ||
  (sizeofcd != sizeof(cd_data_t)) || (p_cd->iorem.size != sizeof(iorem_slots_t)) ||
  //supported major versions: 2
  ((IOREM_MAJ_VER(p_cd->iorem.version) != 2)))
  compatible = false;

 return compatible;
}

void CFirmwareDataMediator::LoadBytes(const BYTE* ip_bytes)
{
 ASSERT(ip_bytes);
 memset(mp_bytes_active,0x00,m_firmware_size);
 memset(mp_bytes_original,0x00,m_firmware_size);

 memcpy(mp_bytes_active,ip_bytes,m_firmware_size);
 memcpy(mp_bytes_original,ip_bytes,m_firmware_size);

 mp_cddata = _FindCodeData(); //find data residing directly in the code
 m_is_opened = true;
}

void CFirmwareDataMediator::StoreBytes(BYTE* op_bytes)
{
 ASSERT(op_bytes);
 memcpy(op_bytes,mp_bytes_active,m_firmware_size);
}

bool CFirmwareDataMediator::IsModified(void)
{
 return (0 != memcmp(mp_bytes_active,mp_bytes_original,m_firmware_size));
}

void CFirmwareDataMediator::ResetModified(void)
{
 memcpy(mp_bytes_original,mp_bytes_active,m_firmware_size);
}

_TSTRING CFirmwareDataMediator::GetSignatureInfo(void)
{
 char raw_string[256];
 memset(raw_string,0,FW_SIGNATURE_INFO_SIZE+1);
 fw_data_t* p_fd = (fw_data_t*)(&mp_bytes_active[m_lip->FIRMWARE_DATA_START]); 
 memcpy(raw_string, &p_fd->fw_signature_info, FW_SIGNATURE_INFO_SIZE);
 TCHAR string[256];
 OemToChar(raw_string, string);
 return _TSTRING(string);
}

void CFirmwareDataMediator::SetSignatureInfo(const _TSTRING& i_string)
{
 _TSTRING str = i_string;
 //string must be fixed length
 while(str.length() < FW_SIGNATURE_INFO_SIZE)
  str+=' ';
 char raw_string[256];
 memset(raw_string, 0, FW_SIGNATURE_INFO_SIZE+1);
 fw_data_t* p_fd = (fw_data_t*)(&mp_bytes_active[m_lip->FIRMWARE_DATA_START]); 
 CharToOem(str.c_str(), raw_string);
 memcpy(&p_fd->fw_signature_info, raw_string, FW_SIGNATURE_INFO_SIZE);
}

DWORD CFirmwareDataMediator::GetFWOptions(void)
{
  //if there is no such data in this firmware, then return zero
  return (mp_cddata) ? mp_cddata->config : 0;
}

DWORD CFirmwareDataMediator::GetFWOptions(const BYTE* ip_source_bytes, const PPFlashParam* ip_fpp)
{
 ASSERT(ip_source_bytes && ip_fpp);
 cd_data_t* p_cd = _FindCodeData(ip_source_bytes, ip_fpp); 
 //there is no such data in this firmware, then return 0
 return (p_cd) ? p_cd->config : 0;
}

void CFirmwareDataMediator::GetStartMap(int i_index,float* op_values, bool i_original /* = false */)
{
 BYTE* p_bytes = NULL;
 f_data_t* p_maps = NULL;
 ASSERT(op_values);

 if (i_original)
  p_bytes = mp_bytes_original;
 else
  p_bytes = mp_bytes_active;

 //получаем адрес начала таблиц семейств характеристик
 fw_data_t* p_fd = (fw_data_t*)(p_bytes + m_lip->FIRMWARE_DATA_START);

 for (int i = 0; i < F_STR_POINTS; i++ )
  op_values[i] = ((float)p_fd->tables[i_index].f_str[i]) / AA_MAPS_M_FACTOR;
}

void CFirmwareDataMediator::SetStartMap(int i_index,const float* ip_values)
{
 BYTE* p_bytes = NULL;
 f_data_t* p_maps = NULL;
 ASSERT(ip_values);

 p_bytes = mp_bytes_active;

 //получаем адрес начала таблиц семейств характеристик
 fw_data_t* p_fd = (fw_data_t*)(p_bytes + m_lip->FIRMWARE_DATA_START);

 for (int i = 0; i < F_STR_POINTS; i++ )
  p_fd->tables[i_index].f_str[i] = MathHelpers::Round((ip_values[i]*AA_MAPS_M_FACTOR));
}

void CFirmwareDataMediator::GetIdleMap(int i_index,float* op_values, bool i_original /* = false */)
{
 BYTE* p_bytes = NULL;
 f_data_t* p_maps = NULL;
 ASSERT(op_values);

 if (i_original)
  p_bytes = mp_bytes_original;
 else
  p_bytes = mp_bytes_active;

 //получаем адрес начала таблиц семейств характеристик
 fw_data_t* p_fd = (fw_data_t*)(p_bytes + m_lip->FIRMWARE_DATA_START);

 for (int i = 0; i < F_IDL_POINTS; i++ )
  op_values[i] = ((float)p_fd->tables[i_index].f_idl[i]) / AA_MAPS_M_FACTOR;
}

void CFirmwareDataMediator::SetIdleMap(int i_index,const float* ip_values)
{
 BYTE* p_bytes = NULL;
 f_data_t* p_maps = NULL;
 ASSERT(ip_values);

 p_bytes = mp_bytes_active;

 //получаем адрес начала таблиц семейств характеристик
 fw_data_t* p_fd = (fw_data_t*)(p_bytes + m_lip->FIRMWARE_DATA_START);

 for (int i = 0; i < F_IDL_POINTS; i++ )
  p_fd->tables[i_index].f_idl[i] = MathHelpers::Round((ip_values[i]*AA_MAPS_M_FACTOR));
}

std::vector<_TSTRING> CFirmwareDataMediator::GetFunctionsSetNames(void)
{
 std::vector<_TSTRING> names(TABLES_NUMBER);

 BYTE* p_bytes = NULL;
 f_data_t* p_maps = NULL;

 p_bytes = mp_bytes_active;

 //получаем адрес начала таблиц семейств характеристик
 fw_data_t* p_fd = (fw_data_t*)(p_bytes + m_lip->FIRMWARE_DATA_START);

 for (int i = 0; i < TABLES_NUMBER; i++)
 {
  char raw_string[256];
  memset(raw_string, 0, F_NAME_SIZE+1);
  BYTE* addr = p_fd->tables[i].name;
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
 f_data_t* p_maps = NULL;

 p_bytes = mp_bytes_active;

 //получаем адрес начала таблиц семейств характеристик
 fw_data_t* p_fd = (fw_data_t*)(p_bytes + m_lip->FIRMWARE_DATA_START);

 //дополняем строку пробелами, так чтобы если длина строки меньше F_NAME_SIZE,
 //остальные символы были пробелами
 i_new_name+= _T("                ");

 char raw_string[256];
 memset(raw_string, 0, F_NAME_SIZE+1);
 CharToOem(i_new_name.c_str(), raw_string);
 memcpy(p_fd->tables[i_index].name, raw_string, F_NAME_SIZE);
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
 _uint crc_16 = crc16(mp_bytes_active, m_lip->CODE_SIZE);
 return crc_16;
}

unsigned int CFirmwareDataMediator::GetCRC16StoredInActiveFirmware(void)
{
 fw_data_t* p_fd = (fw_data_t*)(&mp_bytes_active[m_lip->FIRMWARE_DATA_START]);
 return p_fd->code_crc;
}

void CFirmwareDataMediator::CalculateAndPlaceFirmwareCRC(void)
{
 CalculateAndPlaceFirmwareCRC(mp_bytes_active);
}

void CFirmwareDataMediator::LoadDataBytesFromAnotherFirmware(const BYTE* ip_source_bytes, const PPFlashParam* ip_fpp /*= NULL*/)
{
 if (false==IsLoaded())
  return; //некуда загружать...

 //Now we need to load data stored in the code area to the current firmware
 LoadCodeData(ip_source_bytes, (ip_fpp ? ip_fpp : m_fpp.get())->m_app_section_size);

 fw_data_t* p_fd = (fw_data_t*)(&mp_bytes_active[m_lip->FIRMWARE_DATA_START]);
 size_t dataSize = p_fd->def_param.crc;
 _uint oldFWCRC  = p_fd->code_crc;

 if (ip_fpp)
 {
  LocInfoProvider lip(*ip_fpp);

  //shrink size using value specified in def_param.crc if def_param.crc contain a valid value
  size_t start_d = (0==dataSize) ? m_lip->FIRMWARE_DATA_START : (m_lip->BOOT_START - dataSize);
  size_t start_s = (0==dataSize) ? lip.FIRMWARE_DATA_START : (lip.BOOT_START - dataSize);
  size_t size = (0==dataSize) ? m_lip->FIRMWARE_DATA_SIZE : dataSize; 
  memcpy(mp_bytes_active + start_d, ip_source_bytes + start_s, size);
 }
 else
 {
  //shrink size using value specified in def_param.crc if def_param.crc contain a valid value
  size_t start = (0==dataSize) ? m_lip->FIRMWARE_DATA_START : (m_lip->BOOT_START - dataSize);
  size_t size = (0==dataSize) ? m_lip->FIRMWARE_DATA_SIZE : dataSize; 
  memcpy(mp_bytes_active + start, ip_source_bytes + start, size);  
 }
 //Значение def_param.crc не импортируем, так как оно служебное.
 p_fd->def_param.crc = dataSize;

 //не импортируем контрольную сумму прошивки
 p_fd->code_crc = oldFWCRC;

 mp_cddata = _FindCodeData();

 //Compensate ADC correction factors when destination and source firmware use different ADC Vref
 DWORD dst_fwopt = GetFWOptions();
 DWORD src_fwopt = GetFWOptions(ip_source_bytes, ip_fpp ? ip_fpp : m_fpp.get());
 if ((dst_fwopt & (1 << SECU3IO::COPT_VREF_5V)) && !(src_fwopt & (1 << SECU3IO::COPT_VREF_5V)))
  _CompensateVRef(&p_fd->def_param, true);  //5v <-- 2.56v
 if (!(dst_fwopt & (1 << SECU3IO::COPT_VREF_5V)) && (src_fwopt & (1 << SECU3IO::COPT_VREF_5V)))
  _CompensateVRef(&p_fd->def_param, false); //5v --> 2.56v
}

void CFirmwareDataMediator::LoadDefParametersFromBuffer(const BYTE* ip_source_bytes, EventHandler onVrefUsrConfirm /*= NULL*/)
{
 if (false==IsLoaded())
  return; //некуда загружать...
 fw_data_t* p_fd = (fw_data_t*)(&mp_bytes_active[m_lip->FIRMWARE_DATA_START]);
 _uint fwd_size = p_fd->def_param.crc; //save
 memcpy(&p_fd->def_param, ip_source_bytes, sizeof(params_t));
 p_fd->def_param.crc = fwd_size; //restore
 mp_cddata = _FindCodeData(); //find data residing directly in the code

 //Use heuristic check, ask user and apply ADC factor compensation
 std::vector<float> factors;
 params_t& par = p_fd->def_param;
 factors.push_back(((float)par.map_adc_factor)  / 16384.0f);
 factors.push_back(((float)par.ubat_adc_factor) / 16384.0f);
 factors.push_back(((float)par.temp_adc_factor) / 16384.0f);
 factors.push_back(((float)par.tps_adc_factor)  / 16384.0f);
 factors.push_back(((float)par.ai1_adc_factor)  / 16384.0f);
 factors.push_back(((float)par.ai2_adc_factor)  / 16384.0f);
 bool src_vref_5v = true;
 for(size_t i = 0; i < factors.size(); ++i)
  if (factors[i] < 1.70f)
   src_vref_5v = false;
 bool dst_vref_5v = (GetFWOptions() & (1 << SECU3IO::COPT_VREF_5V)) > 0;
 if (src_vref_5v != dst_vref_5v) //different ADC voltage reference?
 {//Ask user, before applying compensation
  if ((onVrefUsrConfirm) ? onVrefUsrConfirm() : true)
   _CompensateVRef(&par, (!src_vref_5v && dst_vref_5v)); //5v <-- 2.56v or 5v --> 2.56v
 }
}

void CFirmwareDataMediator::GetWorkMap(int i_index, float* op_values, bool i_original /* = false*/)
{
 BYTE* p_bytes = NULL;
 f_data_t* p_maps = NULL;
 ASSERT(op_values);

 if (i_original)
  p_bytes = mp_bytes_original;
 else
  p_bytes = mp_bytes_active;

 //получаем адрес начала таблиц семейств характеристик
 fw_data_t* p_fd = (fw_data_t*)(&p_bytes[m_lip->FIRMWARE_DATA_START]);

 for (int i = 0; i < (F_WRK_POINTS_F * F_WRK_POINTS_L); i++ )
 {
  _char *p = &(p_fd->tables[i_index].f_wrk[0][0]);
  op_values[i] = ((float) *(p + i)) / AA_MAPS_M_FACTOR;
 }
}

void CFirmwareDataMediator::SetWorkMap(int i_index, const float* ip_values)
{
 BYTE* p_bytes = NULL;
 f_data_t* p_maps = NULL;
 ASSERT(ip_values);

 p_bytes = mp_bytes_active;

 //получаем адрес начала таблиц семейств характеристик
 fw_data_t* p_fd = (fw_data_t*)(&p_bytes[m_lip->FIRMWARE_DATA_START]);

 for (int i = 0; i < (F_WRK_POINTS_F * F_WRK_POINTS_L); i++ )
 {
  _char *p = &(p_fd->tables[i_index].f_wrk[0][0]);
  *(p + i) = MathHelpers::Round((ip_values[i]*AA_MAPS_M_FACTOR));
 }
}

void CFirmwareDataMediator::GetTempMap(int i_index,float* op_values, bool i_original /* = false */)
{
 BYTE* p_bytes = NULL;
 f_data_t* p_maps = NULL;
 ASSERT(op_values);

 if (i_original)
  p_bytes = mp_bytes_original;
 else
  p_bytes = mp_bytes_active;

 //получаем адрес начала таблиц семейств характеристик
 fw_data_t* p_fd = (fw_data_t*)(&p_bytes[m_lip->FIRMWARE_DATA_START]);

 for (int i = 0; i < F_TMP_POINTS; i++ )
  op_values[i] = ((float)p_fd->tables[i_index].f_tmp[i]) / AA_MAPS_M_FACTOR;
}

void CFirmwareDataMediator::SetTempMap(int i_index,const float* ip_values)
{
 BYTE* p_bytes = NULL;
 f_data_t* p_maps = NULL;
 ASSERT(ip_values);

 p_bytes = mp_bytes_active;

 //получаем адрес начала таблиц семейств характеристик
 fw_data_t* p_fd = (fw_data_t*)(&p_bytes[m_lip->FIRMWARE_DATA_START]);

 for (int i = 0; i < F_TMP_POINTS; i++ )
  p_fd->tables[i_index].f_tmp[i] = MathHelpers::Round((ip_values[i]*AA_MAPS_M_FACTOR));
}

void CFirmwareDataMediator::GetVEMap(int i_index, float* op_values, bool i_original /* = false*/)
{
 BYTE* p_bytes = NULL;
 f_data_t* p_maps = NULL;
 ASSERT(op_values);

 if (i_original)
  p_bytes = mp_bytes_original;
 else
  p_bytes = mp_bytes_active;

 //получаем адрес начала таблиц семейств характеристик
 fw_data_t* p_fd = (fw_data_t*)(&p_bytes[m_lip->FIRMWARE_DATA_START]);

 for (int i = 0; i < (INJ_VE_POINTS_F * INJ_VE_POINTS_L); i++ )
 {
  _uchar *p = &(p_fd->tables[i_index].inj_ve[0][0]);
  op_values[i] = ((float) *(p + i)) / VE_MAPS_M_FACTOR;
 }
}

void CFirmwareDataMediator::SetVEMap(int i_index, const float* ip_values)
{
 BYTE* p_bytes = NULL;
 f_data_t* p_maps = NULL;
 ASSERT(ip_values);

 p_bytes = mp_bytes_active;

 //получаем адрес начала таблиц семейств характеристик
 fw_data_t* p_fd = (fw_data_t*)(&p_bytes[m_lip->FIRMWARE_DATA_START]);

 for (int i = 0; i < (INJ_VE_POINTS_F * INJ_VE_POINTS_L); i++ )
 {
  _uchar *p = &(p_fd->tables[i_index].inj_ve[0][0]);
  *(p + i) = MathHelpers::Round((ip_values[i]*VE_MAPS_M_FACTOR));
 }
}

void CFirmwareDataMediator::GetAFRMap(int i_index, float* op_values, bool i_original /* = false*/)
{
 BYTE* p_bytes = NULL;
 f_data_t* p_maps = NULL;
 ASSERT(op_values);

 if (i_original)
  p_bytes = mp_bytes_original;
 else
  p_bytes = mp_bytes_active;

 //получаем адрес начала таблиц семейств характеристик
 fw_data_t* p_fd = (fw_data_t*)(&p_bytes[m_lip->FIRMWARE_DATA_START]);

 for (int i = 0; i < (INJ_VE_POINTS_F * INJ_VE_POINTS_L); i++ )
 {
  _uchar *p = &(p_fd->tables[i_index].inj_afr[0][0]);
  op_values[i] = AFR_MAPS_M_FACTOR / ((float) *(p + i));
 }
}

void CFirmwareDataMediator::SetAFRMap(int i_index, const float* ip_values)
{
 BYTE* p_bytes = NULL;
 f_data_t* p_maps = NULL;
 ASSERT(ip_values);

 p_bytes = mp_bytes_active;

 //получаем адрес начала таблиц семейств характеристик
 fw_data_t* p_fd = (fw_data_t*)(&p_bytes[m_lip->FIRMWARE_DATA_START]);

 for (int i = 0; i < (INJ_VE_POINTS_F * INJ_VE_POINTS_L); i++ )
 {
  _uchar *p = &(p_fd->tables[i_index].inj_afr[0][0]);
  *(p + i) = MathHelpers::Round((AFR_MAPS_M_FACTOR / ip_values[i]));
 }
}

void CFirmwareDataMediator::GetCrnkMap(int i_index, float* op_values, bool i_original /* = false */)
{
 BYTE* p_bytes = NULL;
 ASSERT(op_values);
 if (!op_values)
  return;

 if (i_original)
  p_bytes = mp_bytes_original;
 else
  p_bytes = mp_bytes_active;

 //получаем адрес структуры дополнительных данных
 fw_data_t* p_fd = (fw_data_t*)(&p_bytes[m_lip->FIRMWARE_DATA_START]);

 float discrete = (m_fpp->m_platform_id == EP_ATMEGA644) ? 3.2f : 4.0f; //for ATMega644 discrete = 3.2uS, for others - 4.0uS
 for(size_t i = 0; i < INJ_CRANKING_LOOKUP_TABLE_SIZE; i++)
  op_values[i] = (p_fd->tables[i_index].inj_cranking[i] * discrete) / 1000.0f; //convert to ms
}

void CFirmwareDataMediator::SetCrnkMap(int i_index, const float* ip_values)
{
 BYTE* p_bytes = NULL;
 ASSERT(ip_values);
 if (!ip_values)
  return;

 p_bytes = mp_bytes_active;

 //получаем адрес структуры дополнительных данных
 fw_data_t* p_fd = (fw_data_t*)(&p_bytes[m_lip->FIRMWARE_DATA_START]);

 float discrete = (m_fpp->m_platform_id == EP_ATMEGA644) ? 3.2f : 4.0f; //for ATMega644 discrete = 3.2uS, for others - 4.0uS
 for(size_t i = 0; i < INJ_CRANKING_LOOKUP_TABLE_SIZE; i++)
  p_fd->tables[i_index].inj_cranking[i] = (_uint)MathHelpers::Round((ip_values[i] * 1000.0) / discrete);
}

void CFirmwareDataMediator::GetWrmpMap(int i_index, float* op_values, bool i_original /* = false */)
{
 BYTE* p_bytes = NULL;
 f_data_t* p_maps = NULL;
 ASSERT(op_values);

 if (i_original)
  p_bytes = mp_bytes_original;
 else
  p_bytes = mp_bytes_active;

 //получаем адрес начала таблиц семейств характеристик
 fw_data_t* p_fd = (fw_data_t*)(&p_bytes[m_lip->FIRMWARE_DATA_START]);

 for (int i = 0; i < INJ_WARMUP_LOOKUP_TABLE_SIZE; i++ )
  op_values[i] = ((float)p_fd->tables[i_index].inj_warmup[i]) / WRMP_MAPS_M_FACTOR;
}

void CFirmwareDataMediator::SetWrmpMap(int i_index,const float* ip_values)
{
 BYTE* p_bytes = NULL;
 f_data_t* p_maps = NULL;
 ASSERT(ip_values);

 p_bytes = mp_bytes_active;

 //получаем адрес начала таблиц семейств характеристик
 fw_data_t* p_fd = (fw_data_t*)(&p_bytes[m_lip->FIRMWARE_DATA_START]);

 for (int i = 0; i < INJ_WARMUP_LOOKUP_TABLE_SIZE; i++ )
  p_fd->tables[i_index].inj_warmup[i] = MathHelpers::Round((ip_values[i]*WRMP_MAPS_M_FACTOR));
}

void CFirmwareDataMediator::GetDeadMap(int i_index, float* op_values, bool i_original /* = false */)
{
 BYTE* p_bytes = NULL;
 ASSERT(op_values);
 if (!op_values)
  return;

 if (i_original)
  p_bytes = mp_bytes_original;
 else
  p_bytes = mp_bytes_active;

 //получаем адрес структуры дополнительных данных
 fw_data_t* p_fd = (fw_data_t*)(&p_bytes[m_lip->FIRMWARE_DATA_START]);

 float discrete = (m_fpp->m_platform_id == EP_ATMEGA644) ? 3.2f : 4.0f; //for ATMega644 discrete = 3.2uS, for others - 4.0uS
 for(size_t i = 0; i < INJ_DT_LOOKUP_TABLE_SIZE; i++)
  op_values[i] = (p_fd->tables[i_index].inj_dead_time[i] * discrete) / 1000.0f; //convert to ms
}

void CFirmwareDataMediator::SetDeadMap(int i_index, const float* ip_values)
{
 BYTE* p_bytes = NULL;
 ASSERT(ip_values);
 if (!ip_values)
  return;

 p_bytes = mp_bytes_active;

 //получаем адрес структуры дополнительных данных
 fw_data_t* p_fd = (fw_data_t*)(&p_bytes[m_lip->FIRMWARE_DATA_START]);

 float discrete = (m_fpp->m_platform_id == EP_ATMEGA644) ? 3.2f : 4.0f; //for ATMega644 discrete = 3.2uS, for others - 4.0uS
 for(size_t i = 0; i < INJ_DT_LOOKUP_TABLE_SIZE; i++)
  p_fd->tables[i_index].inj_dead_time[i] = (_uint)MathHelpers::Round((ip_values[i] * 1000.0) / discrete);
}

void CFirmwareDataMediator::GetIdlrMap(int i_index,float* op_values, bool i_original /* = false */)
{
 BYTE* p_bytes = NULL;
 f_data_t* p_maps = NULL;
 ASSERT(op_values);

 if (i_original)
  p_bytes = mp_bytes_original;
 else
  p_bytes = mp_bytes_active;

 //получаем адрес начала таблиц семейств характеристик
 fw_data_t* p_fd = (fw_data_t*)(&p_bytes[m_lip->FIRMWARE_DATA_START]);

 for (int i = 0; i < INJ_IAC_POS_TABLE_SIZE; i++ )
  op_values[i] = ((float)p_fd->tables[i_index].inj_iac_run_pos[i]) / IACPOS_MAPS_M_FACTOR;
}

void CFirmwareDataMediator::SetIdlrMap(int i_index,const float* ip_values)
{
 BYTE* p_bytes = NULL;
 f_data_t* p_maps = NULL;
 ASSERT(ip_values);

 p_bytes = mp_bytes_active;

 //получаем адрес начала таблиц семейств характеристик
 fw_data_t* p_fd = (fw_data_t*)(&p_bytes[m_lip->FIRMWARE_DATA_START]);

 for (int i = 0; i < INJ_IAC_POS_TABLE_SIZE; i++ )
  p_fd->tables[i_index].inj_iac_run_pos[i] = MathHelpers::Round((ip_values[i]*IACPOS_MAPS_M_FACTOR));
}

void CFirmwareDataMediator::GetIdlcMap(int i_index,float* op_values, bool i_original /* = false */)
{
 BYTE* p_bytes = NULL;
 f_data_t* p_maps = NULL;
 ASSERT(op_values);

 if (i_original)
  p_bytes = mp_bytes_original;
 else
  p_bytes = mp_bytes_active;

 //получаем адрес начала таблиц семейств характеристик
 fw_data_t* p_fd = (fw_data_t*)(&p_bytes[m_lip->FIRMWARE_DATA_START]);

 for (int i = 0; i < INJ_IAC_POS_TABLE_SIZE; i++ )
  op_values[i] = ((float)p_fd->tables[i_index].inj_iac_crank_pos[i]) / IACPOS_MAPS_M_FACTOR;
}

void CFirmwareDataMediator::SetIdlcMap(int i_index,const float* ip_values)
{
 BYTE* p_bytes = NULL;
 f_data_t* p_maps = NULL;
 ASSERT(ip_values);

 p_bytes = mp_bytes_active;

 //получаем адрес начала таблиц семейств характеристик
 fw_data_t* p_fd = (fw_data_t*)(&p_bytes[m_lip->FIRMWARE_DATA_START]);

 for (int i = 0; i < INJ_IAC_POS_TABLE_SIZE; i++ )
  p_fd->tables[i_index].inj_iac_crank_pos[i] = MathHelpers::Round((ip_values[i]*IACPOS_MAPS_M_FACTOR));
}

bool CFirmwareDataMediator::SetDefParamValues(BYTE i_descriptor, const void* ip_values)
{
 using namespace SECU3IO;

 BYTE* p_bytes = NULL;
 params_t* p_params = NULL;
 p_bytes = mp_bytes_active;
 //получаем адрес структуры дефаултных параметров
 p_params = &((fw_data_t*)(&p_bytes[m_lip->FIRMWARE_DATA_START]))->def_param;

 //TODO: Remove these copy/paste! (conversions should be implemented in functions, as certain class)
 switch(i_descriptor)
 {
  case TEMPER_PAR:
   {
    TemperPar* p_in = (TemperPar*)ip_values;
    p_params->tmp_use  = p_in->tmp_use;
    p_params->vent_pwm = p_in->vent_pwm;
    p_params->cts_use_map = p_in->cts_use_map;
    p_params->vent_on  = MathHelpers::Round(p_in->vent_on * TEMP_PHYSICAL_MAGNITUDE_MULTIPLAYER);
    p_params->vent_off = MathHelpers::Round(p_in->vent_off * TEMP_PHYSICAL_MAGNITUDE_MULTIPLAYER);
   }
   break;
  case CARBUR_PAR:
   {
    CarburPar* p_in = (CarburPar*)ip_values;
    p_params->ephh_hit    = p_in->ephh_hit;
    p_params->ephh_lot    = p_in->ephh_lot;
    p_params->carb_invers = p_in->carb_invers;
    p_params->epm_on_threshold = MathHelpers::Round(p_in->epm_ont * MAP_PHYSICAL_MAGNITUDE_MULTIPLAYER);
    p_params->ephh_hit_g  = p_in->ephh_hit_g;
    p_params->ephh_lot_g  = p_in->ephh_lot_g;
    p_params->shutoff_delay = MathHelpers::Round(p_in->shutoff_delay * 100); //переводим в десятки мс
    p_params->tps_threshold = MathHelpers::Round(p_in->tps_threshold * TPS_PHYSICAL_MAGNITUDE_MULTIPLAYER);
   }
   break;
  case IDLREG_PAR:
   {
    IdlRegPar* p_in = (IdlRegPar*)ip_values;
    p_params->idl_regul  = p_in->idl_regul;
    p_params->idling_rpm = p_in->idling_rpm;
    p_params->MINEFR     = p_in->MINEFR;
    p_params->ifac1      = MathHelpers::Round(p_in->ifac1 * ANGLE_MULTIPLAYER);
    p_params->ifac2      = MathHelpers::Round(p_in->ifac2 * ANGLE_MULTIPLAYER);
    p_params->idlreg_min_angle = MathHelpers::Round(p_in->min_angle * ANGLE_MULTIPLAYER);
    p_params->idlreg_max_angle = MathHelpers::Round(p_in->max_angle * ANGLE_MULTIPLAYER);
    p_params->idlreg_turn_on_temp = MathHelpers::Round(p_in->turn_on_temp * TEMP_PHYSICAL_MAGNITUDE_MULTIPLAYER);
   }
   break;
  case ANGLES_PAR:
   {
    AnglesPar* p_in = (AnglesPar*)ip_values;
    p_params->angle_corr = MathHelpers::Round(p_in->angle_corr * ANGLE_MULTIPLAYER);
    p_params->max_angle  = MathHelpers::Round(p_in->max_angle * ANGLE_MULTIPLAYER);
    p_params->min_angle  = MathHelpers::Round(p_in->min_angle * ANGLE_MULTIPLAYER);
    p_params->angle_dec_spead = MathHelpers::Round(p_in->dec_spead * ANGLE_MULTIPLAYER);
    p_params->angle_inc_spead = MathHelpers::Round(p_in->inc_spead * ANGLE_MULTIPLAYER);
    p_params->zero_adv_ang = p_in->zero_adv_ang;
   }
   break;
  case FUNSET_PAR:
   {
    FunSetPar* p_in = (FunSetPar*)ip_values;
    p_params->fn_benzin = p_in->fn_benzin;
    p_params->fn_gas    = p_in->fn_gas;
    p_params->map_lower_pressure = MathHelpers::Round(p_in->map_lower_pressure * MAP_PHYSICAL_MAGNITUDE_MULTIPLAYER);
    p_params->map_upper_pressure = MathHelpers::Round(p_in->map_upper_pressure * MAP_PHYSICAL_MAGNITUDE_MULTIPLAYER);
    p_params->map_curve_offset = MathHelpers::Round(p_in->map_curve_offset / ADC_DISCRETE);
    p_params->map_curve_gradient = MathHelpers::Round(128.0f * p_in->map_curve_gradient * MAP_PHYSICAL_MAGNITUDE_MULTIPLAYER * ADC_DISCRETE);
    p_params->tps_curve_offset = MathHelpers::Round(p_in->tps_curve_offset / ADC_DISCRETE);
    p_params->tps_curve_gradient = MathHelpers::Round(128.0f * p_in->tps_curve_gradient * (TPS_PHYSICAL_MAGNITUDE_MULTIPLAYER*64) * ADC_DISCRETE);
   }
   break;
  case STARTR_PAR:
   {
    StartrPar* p_in = (StartrPar*)ip_values;
    p_params->starter_off  = p_in->starter_off;
    p_params->smap_abandon = p_in->smap_abandon;
    p_params->inj_cranktorun_time = MathHelpers::Round(p_in->inj_cranktorun_time * 100.0f);
    p_params->inj_aftstr_enrich = MathHelpers::Round(((p_in->inj_aftstr_enrich + 100.0f) * 128.0f) / 100.0f);
    p_params->inj_aftstr_strokes = p_in->inj_aftstr_strokes;
   }
   break;
  case ADCCOR_PAR:
   {
    ADCCompenPar* p_in = (ADCCompenPar*)ip_values;
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

    p_params->tps_adc_factor     = MathHelpers::Round(p_in->tps_adc_factor * 16384);
    //-------------------------------------------------------------------------
    signed long tps_correction_d = MathHelpers::Round((-p_in->tps_adc_correction) / ADC_DISCRETE); //переводим из вольтов в дискреты АЦП
    p_params->tps_adc_correction = MathHelpers::Round(16384 * (0.5f - tps_correction_d * p_in->tps_adc_factor));
    //-------------------------------------------------------------------------

    p_params->ai1_adc_factor     = MathHelpers::Round(p_in->ai1_adc_factor * 16384);
    //-------------------------------------------------------------------------
    signed long ai1_correction_d = MathHelpers::Round((-p_in->ai1_adc_correction) / ADC_DISCRETE); //переводим из вольтов в дискреты АЦП
    p_params->ai1_adc_correction = MathHelpers::Round(16384 * (0.5f - ai1_correction_d * p_in->ai1_adc_factor));
    //-------------------------------------------------------------------------

    p_params->ai2_adc_factor     = MathHelpers::Round(p_in->ai2_adc_factor * 16384);
    //-------------------------------------------------------------------------
    signed long ai2_correction_d = MathHelpers::Round((-p_in->ai2_adc_correction) / ADC_DISCRETE); //переводим из вольтов в дискреты АЦП
    p_params->ai2_adc_correction = MathHelpers::Round(16384 * (0.5f - ai2_correction_d * p_in->ai2_adc_factor));
    //-------------------------------------------------------------------------
   }
   break;
  case CKPS_PAR:
   {
    CKPSPar* p_in = (CKPSPar*)ip_values;
    p_params->ckps_cogs_btdc  = p_in->ckps_cogs_btdc;
    p_params->ckps_ignit_cogs = p_in->ckps_ignit_cogs;
    p_params->ckps_edge_type  = p_in->ckps_edge_type;
    p_params->ref_s_edge_type  = p_in->ref_s_edge_type;
    p_params->ckps_engine_cyl = p_in->ckps_engine_cyl;
    p_params->ckps_merge_ign_outs = p_in->ckps_merge_ign_outs;
    p_params->ckps_cogs_num = p_in->ckps_cogs_num;
    p_params->ckps_miss_num = p_in->ckps_miss_num;
    p_params->hall_flags = (p_params->hall_flags & 0xFE) | ((p_in->use_ckps_for_hall != 0) << 0);
    p_params->hall_wnd_width = MathHelpers::Round(p_in->hall_wnd_width * ANGLE_MULTIPLAYER);
   }
   break;
  case KNOCK_PAR:
   {
    KnockPar* p_in = (KnockPar*)ip_values;
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
    MiscelPar* p_in = (MiscelPar*)ip_values;

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
    p_params->hop_start_cogs = p_in->hop_start_cogs;
    p_params->hop_durat_cogs = p_in->hop_durat_cogs;
   }
   break;
  case CHOKE_PAR:
   {
    ChokePar* p_in = (ChokePar*)ip_values;
    p_params->sm_steps  = p_in->sm_steps;
    p_params->choke_rpm[0] = p_in->choke_rpm[0];
    p_params->choke_rpm[1] = p_in->choke_rpm[1];
    p_params->choke_startup_corr = MathHelpers::Round(p_in->strt_add * 2.0f);
    p_params->choke_rpm_if = MathHelpers::Round(p_in->choke_rpm_if * 1024.0f);
    p_params->choke_corr_time = MathHelpers::Round(p_in->choke_corr_time * 100.0f);
    p_params->choke_corr_temp = MathHelpers::Round(p_in->choke_corr_temp * TEMP_PHYSICAL_MAGNITUDE_MULTIPLAYER);
   }
   break;
  case SECUR_PAR:
   {
    SecurPar* p_in = (SecurPar*)ip_values;
    p_params->bt_flags = (p_params->bt_flags & 0xFA) | ((p_in->use_imm != 0) << 2) | ((p_in->use_bt != 0) << 0);
    for(int j = 0; j < SECU3IO::IBTN_KEYS_NUM; ++j)
     memcpy(p_params->ibtn_keys[j], p_in->ibtn_keys[j], SECU3IO::IBTN_KEY_SIZE);
   }
   break;
  case UNIOUT_PAR:
   {
    CondEncoder cen(m_quartz_frq, m_period_distance);
    UniOutPar* p_in = (UniOutPar*)ip_values;
    for(int oi = 0; oi < UNI_OUTPUT_NUM; ++oi)
    {
     p_params->uni_output[oi].flags = ((p_in->out[oi].logicFunc) << 4) | (p_in->out[oi].invers_2?2:0) | (p_in->out[oi].invers_1?1:0);
     p_params->uni_output[oi].condition1 = p_in->out[oi].condition1;
     p_params->uni_output[oi].condition2 = p_in->out[oi].condition2;
     p_params->uni_output[oi].on_thrd_1 = cen.UniOutEncodeCondVal(p_in->out[oi].on_thrd_1, p_in->out[oi].condition1);
     p_params->uni_output[oi].off_thrd_1 = cen.UniOutEncodeCondVal(p_in->out[oi].off_thrd_1, p_in->out[oi].condition1);
     p_params->uni_output[oi].on_thrd_2 = cen.UniOutEncodeCondVal(p_in->out[oi].on_thrd_2, p_in->out[oi].condition2);
     p_params->uni_output[oi].off_thrd_2 = cen.UniOutEncodeCondVal(p_in->out[oi].off_thrd_2, p_in->out[oi].condition2);
    }
    p_params->uniout_12lf = p_in->logicFunc12;
   }
   break;
  case INJCTR_PAR:
   {
    InjctrPar* p_in = (InjctrPar*)ip_values;
    p_params->inj_config = (p_in->inj_config << 4) | (p_in->inj_squirt_num & 0x0F);
    p_params->inj_flow_rate = MathHelpers::Round(p_in->inj_flow_rate * 64.0f);
    p_params->inj_cyl_disp = MathHelpers::Round(p_in->inj_cyl_disp * 16384.0f);
    p_params->inj_sd_igl_const = MathHelpers::Round(p_in->inj_sd_igl_const);    
   }
   break;
  case LAMBDA_PAR:
   {
    LambdaPar* p_in = (LambdaPar*)ip_values;
    p_params->inj_lambda_str_per_stp = p_in->lam_str_per_stp;
    p_params->inj_lambda_step_size = MathHelpers::Round(p_in->lam_step_size * 512.0f / 100.0f);
    p_params->inj_lambda_corr_limit = MathHelpers::Round(p_in->lam_corr_limit * 512.0f / 100.0f);
    p_params->inj_lambda_swt_point = MathHelpers::Round(p_in->lam_swt_point / ADC_DISCRETE);
    p_params->inj_lambda_temp_thrd = MathHelpers::Round(p_in->lam_temp_thrd * TEMP_PHYSICAL_MAGNITUDE_MULTIPLAYER);
    p_params->inj_lambda_rpm_thrd = p_in->lam_rpm_thrd;
   }
   break;

  default:
   return false; //неизвестный или неподдерживаемый дескриптор
 }//switch

 return true;
}

bool CFirmwareDataMediator::GetDefParamValues(BYTE i_descriptor, void* op_values)
{
 using namespace SECU3IO;

 BYTE* p_bytes = NULL;
 params_t* p_params = NULL;
 p_bytes = mp_bytes_active;
 //получаем адрес структуры дефаултных параметров
 p_params = &((fw_data_t*)(&p_bytes[m_lip->FIRMWARE_DATA_START]))->def_param;

 //TODO: Remove these copy/paste! (conversions should be implemented in functions, as certain class)
 switch(i_descriptor)
  {
   case TEMPER_PAR:
    {
     TemperPar* p_out = (TemperPar*)op_values;
     p_out->tmp_use  = p_params->tmp_use;
     p_out->vent_pwm = p_params->vent_pwm;
     p_out->cts_use_map = p_params->cts_use_map;
     p_out->vent_on  = ((float)p_params->vent_on) / TEMP_PHYSICAL_MAGNITUDE_MULTIPLAYER;
     p_out->vent_off = ((float)p_params->vent_off) / TEMP_PHYSICAL_MAGNITUDE_MULTIPLAYER;
    }
    break;
   case CARBUR_PAR:
    {
     CarburPar* p_out = (CarburPar*)op_values;
     p_out->ephh_hit    = p_params->ephh_hit;
     p_out->ephh_lot    = p_params->ephh_lot;
     p_out->carb_invers = p_params->carb_invers;
     p_out->epm_ont =  ((float)p_params->epm_on_threshold) / MAP_PHYSICAL_MAGNITUDE_MULTIPLAYER;
     p_out->ephh_hit_g  = p_params->ephh_hit_g;
     p_out->ephh_lot_g  = p_params->ephh_lot_g;
     p_out->shutoff_delay = ((float)p_params->shutoff_delay) / 100.0f; //переводим в секунды
     p_out->tps_threshold = ((float)p_params->tps_threshold) / TPS_PHYSICAL_MAGNITUDE_MULTIPLAYER;
    }
    break;
   case IDLREG_PAR:
    {
     IdlRegPar* p_out = (IdlRegPar*)op_values;
     p_out->idl_regul  = p_params->idl_regul;
     p_out->idling_rpm = p_params->idling_rpm;
     p_out->MINEFR     = p_params->MINEFR;
     p_out->ifac1      = ((float)p_params->ifac1) / ANGLE_MULTIPLAYER;
     p_out->ifac2      = ((float)p_params->ifac2) / ANGLE_MULTIPLAYER;
     p_out->min_angle  = ((float)p_params->idlreg_min_angle) / ANGLE_MULTIPLAYER;
     p_out->max_angle  = ((float)p_params->idlreg_max_angle) / ANGLE_MULTIPLAYER;
     p_out->turn_on_temp = ((float)p_params->idlreg_turn_on_temp) / TEMP_PHYSICAL_MAGNITUDE_MULTIPLAYER;
    }
    break;
   case ANGLES_PAR:
    {
     AnglesPar* p_out = (AnglesPar*)op_values;
     p_out->angle_corr = ((float)p_params->angle_corr) / ANGLE_MULTIPLAYER;
     p_out->max_angle  = ((float)p_params->max_angle)  / ANGLE_MULTIPLAYER;
     p_out->min_angle  = ((float)p_params->min_angle) / ANGLE_MULTIPLAYER;
     p_out->dec_spead = ((float)p_params->angle_dec_spead)  / ANGLE_MULTIPLAYER;
     p_out->inc_spead = ((float)p_params->angle_inc_spead)  / ANGLE_MULTIPLAYER;
     p_out->zero_adv_ang = p_params->zero_adv_ang;
    }
    break;
   case FUNSET_PAR:
    {
     FunSetPar* p_out = (FunSetPar*)op_values;
     p_out->fn_benzin = p_params->fn_benzin;
     p_out->fn_gas    = p_params->fn_gas;
     p_out->map_lower_pressure = ((float)p_params->map_lower_pressure) / MAP_PHYSICAL_MAGNITUDE_MULTIPLAYER;
     p_out->map_upper_pressure = ((float)p_params->map_upper_pressure) / MAP_PHYSICAL_MAGNITUDE_MULTIPLAYER;
     p_out->map_curve_offset = ((float)p_params->map_curve_offset) * ADC_DISCRETE;
     p_out->map_curve_gradient = ((float)p_params->map_curve_gradient) / (MAP_PHYSICAL_MAGNITUDE_MULTIPLAYER * ADC_DISCRETE * 128.0f);
     p_out->tps_curve_offset = ((float)p_params->tps_curve_offset) * ADC_DISCRETE;
     p_out->tps_curve_gradient = ((float)p_params->tps_curve_gradient) / ((TPS_PHYSICAL_MAGNITUDE_MULTIPLAYER*64) * ADC_DISCRETE * 128.0f);
    }
    break;
   case STARTR_PAR:
    {
     StartrPar* p_out = (StartrPar*)op_values;
     p_out->starter_off = p_params->starter_off;
     p_out->smap_abandon = p_params->smap_abandon;
     p_out->inj_cranktorun_time = float(p_params->inj_cranktorun_time) / 100.0f;
     p_out->inj_aftstr_enrich = (float(p_params->inj_aftstr_enrich) / 128.0f) * 100.0f - 100.0f;
     p_out->inj_aftstr_strokes = p_params->inj_aftstr_strokes;
    }
    break;
   case ADCCOR_PAR:
    {
     ADCCompenPar* p_out = (ADCCompenPar*)op_values;
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

     p_out->tps_adc_factor     = ((float)p_params->tps_adc_factor) / 16384;
     //-------------------------------------------------------------------------
     if (p_out->tps_adc_factor > 0)
     {
      p_out->tps_adc_correction = ((((float)p_params->tps_adc_correction)/16384.0f) - 0.5f) / p_out->tps_adc_factor;
      p_out->tps_adc_correction*=ADC_DISCRETE; //в вольты
     }
     else
      p_out->tps_adc_correction = 0;
     //-------------------------------------------------------------------------

     p_out->ai1_adc_factor     = ((float)p_params->ai1_adc_factor) / 16384;
     //-------------------------------------------------------------------------
     if (p_out->ai1_adc_factor > 0)
     {
      p_out->ai1_adc_correction = ((((float)p_params->ai1_adc_correction)/16384.0f) - 0.5f) / p_out->ai1_adc_factor;
      p_out->ai1_adc_correction*=ADC_DISCRETE; //в вольты
     }
     else
      p_out->ai1_adc_correction = 0;
     //-------------------------------------------------------------------------

     p_out->ai2_adc_factor     = ((float)p_params->ai2_adc_factor) / 16384;
     //-------------------------------------------------------------------------
     if (p_out->ai2_adc_factor > 0)
     {
      p_out->ai2_adc_correction = ((((float)p_params->ai2_adc_correction)/16384.0f) - 0.5f) / p_out->ai2_adc_factor;
      p_out->ai2_adc_correction*=ADC_DISCRETE; //в вольты
     }
     else
      p_out->ai2_adc_correction = 0;
     //-------------------------------------------------------------------------
    }
    break;
   case CKPS_PAR:
    {
     CKPSPar* p_out = (CKPSPar*)op_values;
     p_out->ckps_cogs_btdc  = p_params->ckps_cogs_btdc;
     p_out->ckps_ignit_cogs = p_params->ckps_ignit_cogs;
     p_out->ckps_edge_type  = p_params->ckps_edge_type;
     p_out->ref_s_edge_type  = p_params->ref_s_edge_type;
     p_out->ckps_engine_cyl = p_params->ckps_engine_cyl;
     p_out->ckps_merge_ign_outs = p_params->ckps_merge_ign_outs;
     p_out->ckps_cogs_num = p_params->ckps_cogs_num;
     p_out->ckps_miss_num = p_params->ckps_miss_num;
     p_out->use_ckps_for_hall = p_params->hall_flags & (1 << 0);
     p_out->hall_wnd_width = ((float)p_params->hall_wnd_width) / ANGLE_MULTIPLAYER;
    }
    break;
   case KNOCK_PAR:
    {
     KnockPar* p_out = (KnockPar*)op_values;
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
     MiscelPar* p_out = (MiscelPar*)op_values;

     int baud_rate = 0;
     for(size_t i = 0; i < SECU3IO::SECU3_ALLOWABLE_UART_DIVISORS_COUNT; ++i)
     if (SECU3IO::secu3_allowable_uart_divisors[i].second == p_params->uart_divisor)
      baud_rate = SECU3IO::secu3_allowable_uart_divisors[i].first;
     ASSERT(baud_rate);

     p_out->baud_rate = baud_rate;
     p_out->period_ms = p_params->uart_period_t_ms * 10;
     p_out->ign_cutoff = p_params->ign_cutoff;
     p_out->ign_cutoff_thrd = p_params->ign_cutoff_thrd;
     p_out->hop_start_cogs = p_params->hop_start_cogs;
     p_out->hop_durat_cogs = p_params->hop_durat_cogs;
    }
    break;
   case CHOKE_PAR:
    {
     ChokePar* p_out = (ChokePar*)op_values;
     p_out->sm_steps = p_params->sm_steps;
     p_out->choke_rpm[0] = p_params->choke_rpm[0];
     p_out->choke_rpm[1] = p_params->choke_rpm[1];
     p_out->strt_add = p_params->choke_startup_corr / 2.0f;
     p_out->choke_rpm_if = p_params->choke_rpm_if / 1024.0f;
     p_out->choke_corr_time = p_params->choke_corr_time / 100.0f;
     p_out->choke_corr_temp = ((float)p_params->choke_corr_temp) / TEMP_PHYSICAL_MAGNITUDE_MULTIPLAYER;
    }
    break;
   case SECUR_PAR:
    {
     SecurPar* p_out = (SecurPar*)op_values;
     p_out->use_bt = p_params->bt_flags & (1 << 0);
     p_out->use_imm = p_params->bt_flags & (1 << 2);
    for(int j = 0; j < SECU3IO::IBTN_KEYS_NUM; ++j)
     memcpy(p_out->ibtn_keys[j], p_params->ibtn_keys[j], SECU3IO::IBTN_KEY_SIZE);
    }
    break;
  case UNIOUT_PAR:
   {
    CondEncoder cen(m_quartz_frq, m_period_distance);
    UniOutPar* p_out = (UniOutPar*)op_values;
    for(int oi = 0; oi < UNI_OUTPUT_NUM; ++oi)
    {
     p_out->out[oi].logicFunc = p_params->uni_output[oi].flags >> 4;
     p_out->out[oi].invers_1 = (p_params->uni_output[oi].flags & 0x01) != 0;
     p_out->out[oi].invers_2 = (p_params->uni_output[oi].flags & 0x02) != 0;
     p_out->out[oi].condition1 = p_params->uni_output[oi].condition1;
     p_out->out[oi].condition2 = p_params->uni_output[oi].condition2;
     p_out->out[oi].on_thrd_1 = cen.UniOutDecodeCondVal(p_params->uni_output[oi].on_thrd_1, p_params->uni_output[oi].condition1);
     p_out->out[oi].off_thrd_1 = cen.UniOutDecodeCondVal(p_params->uni_output[oi].off_thrd_1, p_params->uni_output[oi].condition1);
     p_out->out[oi].on_thrd_2 = cen.UniOutDecodeCondVal(p_params->uni_output[oi].on_thrd_2, p_params->uni_output[oi].condition2);
     p_out->out[oi].off_thrd_2 = cen.UniOutDecodeCondVal(p_params->uni_output[oi].off_thrd_2, p_params->uni_output[oi].condition2);
    }
    p_out->logicFunc12 = p_params->uniout_12lf;
   }
   break;
  case INJCTR_PAR:
   {
    InjctrPar* p_out = (InjctrPar*)op_values;
    p_out->inj_config = p_params->inj_config >> 4;      //fuel injection configuration
    p_out->inj_squirt_num = p_params->inj_config & 0x0F; //number of squirts per cycle
    p_out->inj_flow_rate = float(p_params->inj_flow_rate) / 64.0f;
    p_out->inj_cyl_disp = float(p_params->inj_cyl_disp) / 16384.0f;
    p_out->inj_sd_igl_const = (float)p_params->inj_sd_igl_const;
    p_out->cyl_num = p_params->ckps_engine_cyl; //read-only parameter, its value required for calculations
   }
   break;
  case LAMBDA_PAR:
   {
    LambdaPar* p_out = (LambdaPar*)op_values;
    p_out->lam_str_per_stp = p_params->inj_lambda_str_per_stp;
    p_out->lam_step_size = ((float)p_params->inj_lambda_step_size * 100.0f)/512.0f;
    p_out->lam_corr_limit = ((float)p_params->inj_lambda_corr_limit * 100.0f)/512.0f;
    p_out->lam_swt_point = ((float)p_params->inj_lambda_swt_point) * ADC_DISCRETE;
    p_out->lam_temp_thrd = ((float)p_params->inj_lambda_temp_thrd) / TEMP_PHYSICAL_MAGNITUDE_MULTIPLAYER;
    p_out->lam_rpm_thrd = p_params->inj_lambda_rpm_thrd;
   }
   break;

   default:
    return false; //неизвестный или неподдерживаемый дескриптор
  }//switch

  return true;
}

void CFirmwareDataMediator::SetNumPulsesPer1Km(int pp1km)
{
 double value = MathHelpers::RestrictValue(pp1km, 1, 60000);
 m_period_distance = (float)(1000.0 / value); //distance of one period in meters
}

void CFirmwareDataMediator::SetQuartzFrq(long frq)
{
 m_quartz_frq = frq;
}

//выдает все таблицы в одной структуре
void CFirmwareDataMediator::GetMapsData(FWMapsDataHolder* op_fwd)
{
 std::vector<_TSTRING> names = GetFunctionsSetNames();
 int i;
 for(i = 0; i < TABLES_NUMBER; i++)
 {
  GetStartMap(i,op_fwd->maps[i].f_str);
  GetIdleMap(i,op_fwd->maps[i].f_idl);
  GetWorkMap(i,op_fwd->maps[i].f_wrk);
  GetTempMap(i,op_fwd->maps[i].f_tmp);
  op_fwd->maps[i].name = names[i];
 }
 //separate tables
 GetAttenuatorMap(op_fwd->attenuator_table);
 GetDwellCntrlMap(op_fwd->dwellcntrl_table);

 GetCTSCurveMap(op_fwd->ctscurve_table);
 op_fwd->ctscurve_vlimits[0] = GetCTSMapVoltageLimit(0);
 op_fwd->ctscurve_vlimits[1] = GetCTSMapVoltageLimit(1);

 GetChokeOpMap(op_fwd->choke_op_table);

 //Копируем таблицу с сеткой оборотов (Copy table with RPM grid)
 float slots[F_RPM_SLOTS]; GetRPMGridMap(slots);
 for(i = 0; i < F_RPM_SLOTS; ++i)
  op_fwd->rpm_slots[i] = slots[i];
}

void CFirmwareDataMediator::SetMapsData(const FWMapsDataHolder* ip_fwd)
{
 int i;
 for(i = 0; i < TABLES_NUMBER; i++)
 {
  SetStartMap(i,ip_fwd->maps[i].f_str);
  SetIdleMap(i,ip_fwd->maps[i].f_idl);
  SetWorkMap(i,ip_fwd->maps[i].f_wrk);
  SetTempMap(i,ip_fwd->maps[i].f_tmp);
  SetFunctionsSetName(i,ip_fwd->maps[i].name);
 }
 //separate tables
 SetAttenuatorMap(ip_fwd->attenuator_table);
 SetDwellCntrlMap(ip_fwd->dwellcntrl_table);

 SetCTSCurveMap(ip_fwd->ctscurve_table);
 SetCTSMapVoltageLimit(0, ip_fwd->ctscurve_vlimits[0]);
 SetCTSMapVoltageLimit(1, ip_fwd->ctscurve_vlimits[1]);

 SetChokeOpMap(ip_fwd->choke_op_table);

 //Check RPM grids compatibility and set RPM grid
 if (CheckRPMGridsCompatibility(ip_fwd->rpm_slots))
  SetRPMGridMap(ip_fwd->rpm_slots);
}

bool CFirmwareDataMediator::CheckRPMGridsCompatibility(const float* rpmGrid)
{
 bool match = true;
 float slots[F_RPM_SLOTS]; GetRPMGridMap(slots);
 for(int i = 0; i < F_RPM_SLOTS; ++i)
  if (rpmGrid[i] != slots[i])
   match = false;
 if (!match)
  return (IDYES==AfxMessageBox(_T("RPM grids from firmware and source are not equal!\n Please inspect idle and work maps if you accept new RPM grid.\n Accept new RPM grid (Y) or keep old one (N)?"), MB_YESNO|MB_ICONWARNING));
 return true;
}

void CFirmwareDataMediator::GetAttenuatorMap(float* op_values, bool i_original /* = false */)
{
 BYTE* p_bytes = NULL;
 ASSERT(op_values);
 if (!op_values)
  return;

 if (i_original)
  p_bytes = mp_bytes_original;
 else
  p_bytes = mp_bytes_active;

 //получаем адрес структуры дополнительных данных
 fw_data_t* p_fd = (fw_data_t*)(&p_bytes[m_lip->FIRMWARE_DATA_START]);

 for(size_t i = 0; i < KC_ATTENUATOR_LOOKUP_TABLE_SIZE; i++)
  op_values[i] = p_fd->exdata.attenuator_table[i];
}

void CFirmwareDataMediator::SetAttenuatorMap(const float* ip_values)
{
 BYTE* p_bytes = NULL;
 ASSERT(ip_values);
 if (!ip_values)
  return;

 p_bytes = mp_bytes_active;

 //получаем адрес структуры дополнительных данных
 fw_data_t* p_fd = (fw_data_t*)(&p_bytes[m_lip->FIRMWARE_DATA_START]);

 for(size_t i = 0; i < KC_ATTENUATOR_LOOKUP_TABLE_SIZE; i++)
 {
  ASSERT(ip_values[i] >= 0.0f);
  p_fd->exdata.attenuator_table[i] = (_uchar)MathHelpers::Round(ip_values[i]);
 }
}

void CFirmwareDataMediator::GetDwellCntrlMap(float* op_values, bool i_original /* = false */)
{
 BYTE* p_bytes = NULL;
 ASSERT(op_values);
 if (!op_values)
  return;

 if (i_original)
  p_bytes = mp_bytes_original;
 else
  p_bytes = mp_bytes_active;

 //получаем адрес структуры дополнительных данных
 fw_data_t* p_fd = (fw_data_t*)(&p_bytes[m_lip->FIRMWARE_DATA_START]);

 float discrete = (m_fpp->m_platform_id == EP_ATMEGA644) ? 3.2f : 4.0f; //for ATMega644 discrete = 3.2uS, for others - 4.0uS
 for(size_t i = 0; i < COIL_ON_TIME_LOOKUP_TABLE_SIZE; i++)
  op_values[i] = (p_fd->exdata.coil_on_time[i] * discrete) / 1000.0f; //convert to ms
}

void CFirmwareDataMediator::SetDwellCntrlMap(const float* ip_values)
{
 BYTE* p_bytes = NULL;
 ASSERT(ip_values);
 if (!ip_values)
  return;

 p_bytes = mp_bytes_active;

 //получаем адрес структуры дополнительных данных
 fw_data_t* p_fd = (fw_data_t*)(&p_bytes[m_lip->FIRMWARE_DATA_START]);

 float discrete = (m_fpp->m_platform_id == EP_ATMEGA644) ? 3.2f : 4.0f; //for ATMega644 discrete = 3.2uS, for others - 4.0uS
 for(size_t i = 0; i < COIL_ON_TIME_LOOKUP_TABLE_SIZE; i++)
  p_fd->exdata.coil_on_time[i] = (_uint)MathHelpers::Round((ip_values[i] * 1000.0) / discrete);
}

void CFirmwareDataMediator::GetCTSCurveMap(float* op_values, bool i_original /* = false */)
{
 BYTE* p_bytes = NULL;
 ASSERT(op_values);
 if (!op_values)
  return;

 if (i_original)
  p_bytes = mp_bytes_original;
 else
  p_bytes = mp_bytes_active;

 //получаем адрес структуры дополнительных данных
 fw_data_t* p_fd = (fw_data_t*)(&p_bytes[m_lip->FIRMWARE_DATA_START]);

 for(size_t i = 0; i < THERMISTOR_LOOKUP_TABLE_SIZE; i++)
  op_values[i] = (p_fd->exdata.cts_curve[i] / 4.0f);
}

void CFirmwareDataMediator::SetCTSCurveMap(const float* ip_values)
{
 BYTE* p_bytes = NULL;
 ASSERT(ip_values);
 if (!ip_values)
  return;

 p_bytes = mp_bytes_active;

 //получаем адрес структуры дополнительных данных
 fw_data_t* p_fd = (fw_data_t*)(&p_bytes[m_lip->FIRMWARE_DATA_START]);

 for(size_t i = 0; i < THERMISTOR_LOOKUP_TABLE_SIZE; i++)
  p_fd->exdata.cts_curve[i] = (_uint)MathHelpers::Round(ip_values[i] * 4.0);
}

float CFirmwareDataMediator::GetCTSMapVoltageLimit(int i_type)
{
 //получаем адрес структуры дополнительных данных
 fw_data_t* p_fd = (fw_data_t*)(&mp_bytes_active[m_lip->FIRMWARE_DATA_START]);
 if (0 == i_type)
  return p_fd->exdata.cts_vl_begin * ADC_DISCRETE;
 else if (1 == i_type)
  return p_fd->exdata.cts_vl_end * ADC_DISCRETE;
 else
 {
  ASSERT(0); //wrong i_type value
  return 0;
 }
}

void  CFirmwareDataMediator::SetCTSMapVoltageLimit(int i_type, float i_value)
{
 fw_data_t* p_fd = (fw_data_t*)(&mp_bytes_active[m_lip->FIRMWARE_DATA_START]);
 if (0 == i_type)
  p_fd->exdata.cts_vl_begin = MathHelpers::Round(i_value / ADC_DISCRETE);
 else if (1 == i_type)
  p_fd->exdata.cts_vl_end = MathHelpers::Round(i_value / ADC_DISCRETE);
 else
 {
  ASSERT(0); //wrong i_type value
 }
}

void CFirmwareDataMediator::GetATSCurveMap(float* op_values, bool i_original /* = false */)
{
 BYTE* p_bytes = NULL;
 ASSERT(op_values);
 if (!op_values)
  return;

 if (i_original)
  p_bytes = mp_bytes_original;
 else
  p_bytes = mp_bytes_active;

 //получаем адрес структуры дополнительных данных
 fw_data_t* p_fd = (fw_data_t*)(&p_bytes[m_lip->FIRMWARE_DATA_START]);

 for(size_t i = 0; i < THERMISTOR_LOOKUP_TABLE_SIZE; i++)
  op_values[i] = (p_fd->exdata.ats_curve[i] / 4.0f);
}

void CFirmwareDataMediator::SetATSCurveMap(const float* ip_values)
{
 BYTE* p_bytes = NULL;
 ASSERT(ip_values);
 if (!ip_values)
  return;

 p_bytes = mp_bytes_active;

 //получаем адрес структуры дополнительных данных
 fw_data_t* p_fd = (fw_data_t*)(&p_bytes[m_lip->FIRMWARE_DATA_START]);

 for(size_t i = 0; i < THERMISTOR_LOOKUP_TABLE_SIZE; i++)
  p_fd->exdata.ats_curve[i] = (_uint)MathHelpers::Round(ip_values[i] * 4.0);
}

float CFirmwareDataMediator::GetATSMapVoltageLimit(int i_type)
{
 //получаем адрес структуры дополнительных данных
 fw_data_t* p_fd = (fw_data_t*)(&mp_bytes_active[m_lip->FIRMWARE_DATA_START]);
 if (0 == i_type)
  return p_fd->exdata.ats_vl_begin * ADC_DISCRETE;
 else if (1 == i_type)
  return p_fd->exdata.ats_vl_end * ADC_DISCRETE;
 else
 {
  ASSERT(0); //wrong i_type value
  return 0;
 }
}

void  CFirmwareDataMediator::SetATSMapVoltageLimit(int i_type, float i_value)
{
 fw_data_t* p_fd = (fw_data_t*)(&mp_bytes_active[m_lip->FIRMWARE_DATA_START]);
 if (0 == i_type)
  p_fd->exdata.ats_vl_begin = MathHelpers::Round(i_value / ADC_DISCRETE);
 else if (1 == i_type)
  p_fd->exdata.ats_vl_end = MathHelpers::Round(i_value / ADC_DISCRETE);
 else
 {
  ASSERT(0); //wrong i_type value
 }
}

void CFirmwareDataMediator::GetChokeOpMap(float* op_values, bool i_original /* = false */)
{
 BYTE* p_bytes = NULL;
 ASSERT(op_values);
 if (!op_values)
  return;

 if (i_original)
  p_bytes = mp_bytes_original;
 else
  p_bytes = mp_bytes_active;

 //получаем адрес структуры дополнительных данных
 fw_data_t* p_fd = (fw_data_t*)(&p_bytes[m_lip->FIRMWARE_DATA_START]);

 for(size_t i = 0; i < CHOKE_CLOSING_LOOKUP_TABLE_SIZE; i++)
  op_values[i] = (p_fd->exdata.choke_closing[i] / 2.0f);
}

void CFirmwareDataMediator::SetChokeOpMap(const float* ip_values)
{
 BYTE* p_bytes = NULL;
 ASSERT(ip_values);
 if (!ip_values)
  return;

 p_bytes = mp_bytes_active;

 //получаем адрес структуры дополнительных данных
 fw_data_t* p_fd = (fw_data_t*)(&p_bytes[m_lip->FIRMWARE_DATA_START]);

 for(size_t i = 0; i < CHOKE_CLOSING_LOOKUP_TABLE_SIZE; i++)
  p_fd->exdata.choke_closing[i] = (_uchar)MathHelpers::Round(ip_values[i] * 2.0);
}

const PPFlashParam& CFirmwareDataMediator::GetPlatformParams(void) const
{
 return *m_fpp;
}

void CFirmwareDataMediator::GetRPMGridMap(float* op_values)
{
 ASSERT(op_values);
 if (!op_values)
  return;

 //получаем адрес структуры дополнительных данных
 fw_data_t* p_fd = (fw_data_t*)(&mp_bytes_active[m_lip->FIRMWARE_DATA_START]);

 for(size_t i = 0; i < RPM_GRID_SIZE; i++)
  op_values[i] = p_fd->exdata.rpm_grid_points[i];
}

void CFirmwareDataMediator::SetRPMGridMap(const float* ip_values)
{
 ASSERT(ip_values);
 if (!ip_values)
  return;

 //получаем адрес структуры дополнительных данных
 fw_data_t* p_fd = (fw_data_t*)(&mp_bytes_active[m_lip->FIRMWARE_DATA_START]);

 //store grid points
 for(size_t i = 0; i < RPM_GRID_SIZE; i++)
  p_fd->exdata.rpm_grid_points[i] = MathHelpers::Round(ip_values[i]);

 //calculate sizes
 for(size_t i = 0; i < RPM_GRID_SIZE-1; i++)
  p_fd->exdata.rpm_grid_sizes[i] = p_fd->exdata.rpm_grid_points[i+1] - p_fd->exdata.rpm_grid_points[i];
}

void CFirmwareDataMediator::GetATSAACMap(float* op_values, bool i_original /* = false */)
{
 BYTE* p_bytes = NULL;
 f_data_t* p_maps = NULL;
 ASSERT(op_values);

 if (i_original)
  p_bytes = mp_bytes_original;
 else
  p_bytes = mp_bytes_active;

 //получаем адрес начала таблиц семейств характеристик
 fw_data_t* p_fd = (fw_data_t*)(&p_bytes[m_lip->FIRMWARE_DATA_START]);

 for (int i = 0; i < ATS_CORR_LOOKUP_TABLE_SIZE; i++ )
  op_values[i] = ((float)p_fd->exdata.ats_corr[i]) / AA_MAPS_M_FACTOR;
}

void CFirmwareDataMediator::SetATSAACMap(const float* ip_values)
{
 BYTE* p_bytes = NULL;
 f_data_t* p_maps = NULL;
 ASSERT(ip_values);

 p_bytes = mp_bytes_active;

 //получаем адрес начала таблиц семейств характеристик
 fw_data_t* p_fd = (fw_data_t*)(&p_bytes[m_lip->FIRMWARE_DATA_START]);

 for (int i = 0; i < ATS_CORR_LOOKUP_TABLE_SIZE; i++ )
  p_fd->exdata.ats_corr[i] = MathHelpers::Round((ip_values[i]*AA_MAPS_M_FACTOR));
}

DWORD CFirmwareDataMediator::GetIOPlug(IOXtype type, IOPid id)
{
 if (!mp_cddata)
  return 0;
 DWORD value = 0;
 switch(type)
 {
  case IOX_INIT:
   if (id < IOP_COUNT)
    value = mp_cddata->iorem.i_plugs[id]; 
   break;
  case IOX_DATA:
   if (id < IOP_COUNT)
    value = mp_cddata->iorem.v_plugs[id]; 
   break;
 }
 return value;
}

DWORD CFirmwareDataMediator::GetIOSlot(IOXtype type, IOSid id, bool inv)
{
 if (!mp_cddata)
  return 0;
 DWORD value = 0;
 switch(type)
 {
  case IOX_INIT:
   if (id < IOS_COUNT)
    value = inv ? mp_cddata->iorem.i_slotsi[id] : mp_cddata->iorem.i_slots[id];
   break;
  case IOX_DATA:
   if (id < IOS_COUNT)
    value = inv ? mp_cddata->iorem.v_slotsi[id] : mp_cddata->iorem.v_slots[id];
   break;
 }
 return value;
}

DWORD CFirmwareDataMediator::GetSStub(void) const
{
 if (!mp_cddata)
  return 0;
 return mp_cddata->iorem.s_stub;
}

DWORD CFirmwareDataMediator::GetGStub(void) const
{
 if (!mp_cddata)
  return 0;
 return mp_cddata->iorem.g_stub;
}

void  CFirmwareDataMediator::SetIOPlug(IOXtype type, IOPid id, DWORD value)
{
 if (!mp_cddata)
  return;
 switch(type)
 {
  case IOX_INIT:
   if (id < IOP_COUNT)
    mp_cddata->iorem.i_plugs[id] = (_fnptr_t)value;
   break;
  case IOX_DATA:
   if (id < IOP_COUNT)
    mp_cddata->iorem.v_plugs[id] = (_fnptr_t)value;
   break;
 }
}

CFirmwareDataMediator::IORemVer CFirmwareDataMediator::GetIORemVersion(void) const
{
 ASSERT(mp_cddata);
 if (!mp_cddata)
  return IOV_V00; //error
 return (IORemVer)mp_cddata->iorem.version;
}

cd_data_t* CFirmwareDataMediator::_FindCodeData(const BYTE* ip_bytes /*= NULL*/, const PPFlashParam* ip_fpp /*= NULL*/)
{
 LocInfoProvider* p_lip = m_lip.get();
 const BYTE* p_bytes = mp_bytes_active;
 std::auto_ptr<LocInfoProvider> lip;

 if (ip_bytes && ip_fpp)
 {
  lip.reset(new LocInfoProvider(*ip_fpp));
  p_bytes = ip_bytes;
  p_lip = lip.get();
 }
  
 //obtain pointer to cd_data_t structure
 cd_data_t* p_cd = (cd_data_t*)(&p_bytes[p_lip->FIRMWARE_DATA_START - sizeof(cd_data_t)]);
 //obtain pointer to fw_data_t structure
 fw_data_t* p_fd = (fw_data_t*)(&p_bytes[p_lip->FIRMWARE_DATA_START]);

 if (0==p_fd->fw_data_size)
  return NULL; //there is no code data
 else
  return p_cd;
}

bool CFirmwareDataMediator::HasCodeData(const BYTE* ip_source_bytes /*= NULL*/) const
{
 if (!ip_source_bytes) //current firmware (check loaded firmware)
  return (NULL != mp_cddata);
 else
 {
  //obtain pointer to fw_data_t structure
  fw_data_t* p_fd = (fw_data_t*)(&ip_source_bytes[m_lip->FIRMWARE_DATA_START]);
  _uint dataSize = p_fd->def_param.crc;
  return (dataSize > 0); //has code data?
 }
}

size_t CFirmwareDataMediator::GetOnlyCodeSize(const BYTE* ip_bytes) const
{
 ASSERT(ip_bytes);
 //obtain pointer to fw_data_t structure
 fw_data_t* p_fd = (fw_data_t*)(&ip_bytes[m_lip->FIRMWARE_DATA_START]);

 size_t sizeoffd = p_fd->def_param.crc;

 if ((sizeoffd > 0) && (sizeoffd >= m_fpp->m_only_data_size) && (sizeoffd < m_fpp->m_app_section_size))
  return m_fpp->m_app_section_size - sizeoffd;
 else //old versions of firmware
  return m_fpp->m_only_code_size;
}

void CFirmwareDataMediator::LoadCodeData(const BYTE* ip_source_bytes, size_t i_srcSize, BYTE* op_destin_bytes /*= NULL*/)
{
 ASSERT(ip_source_bytes);
 if (!op_destin_bytes)
  op_destin_bytes = mp_bytes_active; //use current firmware bytes as destination

 //obtain actual size of data (source and destination data)
 _uint dataSizeSrc = ((fw_data_t*)((ip_source_bytes + i_srcSize)-sizeof(fw_data_t)))->def_param.crc;
 _uint dataSizeDst = ((fw_data_t*)((op_destin_bytes + m_lip->FIRMWARE_DATA_START)))->def_param.crc;
 //Check compatibility and copy data from source to destination
 if ((dataSizeSrc > 0) && dataSizeSrc && (dataSizeDst > 0) && dataSizeDst)
 { //code area data is present
  const BYTE *p_dataSrc = (ip_source_bytes + i_srcSize) - dataSizeSrc;
  BYTE *p_dataDst = (op_destin_bytes + m_lip->BOOT_START) - dataSizeDst;
  size_t szSrc = *(((_uint*)p_dataSrc)-1), szDst = *(((_uint*)p_dataDst)-1);
  cd_data_t* pSrc = (cd_data_t*)(p_dataSrc - sizeof(cd_data_t));
  cd_data_t* pDst = (cd_data_t*)(p_dataDst - sizeof(cd_data_t));
  if (szSrc == szDst) //compatible?
  {
   //Transfer values (don't copy it as memory block, because pointers may be different!)
   //So, we copy logical references. Not actual bits.
   for(size_t p = 0; p < IOREM_PLUGS; ++p)
   {
    if (pSrc->iorem.i_plugs[p] == 0)
     continue; //skip not implemented plugs
    size_t s = 0;
    //Ищем есть ли подключение к какому-нибудь слоту на стороне исходных данных
    bool inv = false;
    for(; s < IOREM_SLOTS; ++s)
    {
     if (pSrc->iorem.i_slots[s] == pSrc->iorem.i_plugs[p])
      break;
     if (pSrc->iorem.i_slotsi[s] == pSrc->iorem.i_plugs[p])
     { inv = true; break; }
    }
    if ((s < IOREM_SLOTS) && (pSrc->iorem.i_plugs[p] != pSrc->iorem.s_stub)) //slot?
    {
     pDst->iorem.i_plugs[p] = inv ? pDst->iorem.i_slotsi[s] : pDst->iorem.i_slots[s];
     pDst->iorem.v_plugs[p] = inv ? pDst->iorem.v_slotsi[s] : pDst->iorem.v_slots[s];
    }
    else if (pSrc->iorem.i_plugs[p] == pSrc->iorem.s_stub) //stub?
    {
     pDst->iorem.i_plugs[p] = pDst->iorem.s_stub;
     //Use input stub if source has input stub
     if (pSrc->iorem.v_plugs[p] == pDst->iorem.g_stub)
      pDst->iorem.v_plugs[p] = pDst->iorem.g_stub;
     else
      pDst->iorem.v_plugs[p] = pDst->iorem.s_stub;
    }
   }
  }
  else
  { //incompatible
   //do nothing
  }
 }
}
