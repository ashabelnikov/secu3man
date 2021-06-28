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

/** \file FirmwareDataMediator.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include <map>
#include "FirmwareDataMediator.h"
#include "CRC16.h"
#include "common/MathHelpers.h"
#include "FirmwareMapsDataHolder.h"
#include "PlatformParamHolder.h"
#include "SECU3IO.h"
#include "SECU3ParametersDef.h"
#include "SECU3TablesDef.h"
#include "ufcodes.h"
#include "BitMask.h"
#include "Magnitude.h"

//этот файл содержит платформенно-ориентированные фрагменты кода
#pragma pack(1)

using namespace SECU3IO;
using namespace SECU3IO::SECU3Types;

#define IOREM_MAJ_VER(v) (((v) >> 4) & 0xf)

//--------------------------------------------------------------------------
#define IOREM_SLOTS 49           // Number of slots used for I/O remapping
#define IOREM_PLUGS 100          // Number of plugs used in I/O remapping

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

 _uchar reserved[3];             //Reserved bytes

 _uchar fw_version;              //Firmware version

 _uint size;                     // size of this structure (2 bytes)
}cd_data_t;
//--------------------------------------------------------------------------

//настройки ошибок check engine
typedef struct
{
 _uint map_v_min;                //minimum correct value
 _uint map_v_max;                //maximum correct value
 _uint map_v_em;                 //emergency value, used in case of error
 _uchar map_v_flg;

 _uint vbat_v_min;
 _uint vbat_v_max;
 _uint vbat_v_em;
 _uchar vbat_v_flg;

 _uint cts_v_min;
 _uint cts_v_max;
 _uint cts_v_em;
 _uchar cts_v_flg;

 _uint ks_v_min;
 _uint ks_v_max;
 _uint ks_v_em;
 _uchar ks_v_flg;

 _uint tps_v_min;
 _uint tps_v_max;
 _uint tps_v_em;
 _uchar tps_v_flg;

 _uint add_i1_v_min;
 _uint add_i1_v_max;
 _uint add_i1_v_em;
 _uchar add_i1_v_flg;

 _uint add_i2_v_min;
 _uint add_i2_v_max;
 _uint add_i2_v_em;
 _uchar add_i2_v_flg;

 _uint add_i3_v_min;
 _uint add_i3_v_max;
 _uint add_i3_v_em;
 _uchar add_i3_v_flg;

 _uint add_i4_v_min;
 _uint add_i4_v_max;
 _uint add_i4_v_em;
 _uchar add_i4_v_flg;

 _uint add_i5_v_min;
 _uint add_i5_v_max;
 _uint add_i5_v_em;
 _uchar add_i5_v_flg;

 _uint add_i6_v_min;
 _uint add_i6_v_max;
 _uint add_i6_v_em;
 _uchar add_i6_v_flg;

 _uint add_i7_v_min;
 _uint add_i7_v_max;
 _uint add_i7_v_em;
 _uchar add_i7_v_flg;

 _uint add_i8_v_min;
 _uint add_i8_v_max;
 _uint add_i8_v_em;
 _uchar add_i8_v_flg;

 _uint oilpress_thrd;
 _uint oilpress_timer;
}ce_sett_t;

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

 //Air temperature sensor lookup table
 _int ats_curve[THERMISTOR_LOOKUP_TABLE_SIZE];
 //Voltage corresponding to the beginning of axis
 _uint ats_vl_begin;
 //Voltage corresponding to the end of axis
 _uint ats_vl_end;

 //Air temperature correction of advance angle
 _char ats_corr[ATS_CORR_LOOKUP_TABLE_SIZE];

 //Points of the RPM grid
 _int rpm_grid_points[F_RPM_SLOTS];
 //Sizes of cells in RPM grid (so, we don't need to calculate them at the runtime)
 _int rpm_grid_sizes[F_RPM_SLOTS-1];

 //gas dosator position vs TPS,RPM
 _uchar gasdose_pos[GASDOSE_POS_TPS_SIZE][GASDOSE_POS_RPM_SIZE];

 //CE settings data
 ce_sett_t cesd;

 //barometric correction
 _uint barocorr[BAROCORR_SIZE+2];

 //table for manual setting of ign.timing
 _char pa4_igntim_corr[PA4_LOOKUP_TABLE_SIZE];

 //Gas temperature (TEMP2) sensor lookup table, last two values are related voltage limits for x-axis
 _int tmp2_curve[THERMISTOR_LOOKUP_TABLE_SIZE+2];

 //Coolant temperature correction of advance angle on cranking
 _char cts_crkcorr[CTS_CRKCORR_SIZE];

 //pause curve for EGO heater control
 _uchar eh_pause[COIL_ON_TIME_LOOKUP_TABLE_SIZE];

 //Value of RPM (in 10 min-1 units) vs coolant temperature
 _uchar cranking_thrd[CRANK_THRD_SIZE];

 //Number of strokes vs coolant temperature
 _uchar cranking_time[CRANK_TIME_SIZE];

 //Value of RPM (in 10 min-1 units) vs coolant temperature
 _uchar smapaban_thrd[SMAPABAN_THRD_SIZE];

 //Points of the CLT grid
 _int clt_grid_points[F_TMP_SLOTS];
 //Sizes of cells in CLT grid (so, we don't need to calculate them at the runtime)
 _int clt_grid_sizes[F_TMP_SLOTS-1];

 _uint knock_zone[KNKZONE_TPS_SIZE];

 //Gas reducer's temperature (GRTEMP) sensor lookup table, last two values are related voltage limits for x-axis
 _int grts_curve[THERMISTOR_LOOKUP_TABLE_SIZE+2];

 //PWM duty for gas reducer's heater
 _uchar grheat_duty[F_TMP_POINTS];

 //PWM IAC duty coefficient vs board voltage
 _uint pwmiac_ucoef[PWMIAC_UCOEF_SIZE];

 //Points of the load grid
 _int load_grid_points[F_LOAD_SLOTS];
 //Sizes of cells in load grid (so, we don't need to calculate them at the runtime)
 _int load_grid_sizes[F_LOAD_SLOTS-1];

 _uint inj_aftstr_strk0[AFTSTR_STRK_SIZE];
 _uint inj_aftstr_strk1[AFTSTR_STRK_SIZE];

 _uint grv_delay[F_TMP_POINTS];

 // Fuel tank level vs voltage. 16 points of function, plus two values for setting of x-axis range
 _int ftls_curve[FTLS_LOOKUP_TABLE_SIZE+2];

 // Exhaust gas temperature vs voltage. 16 points of function, plus two values for setting of x-axis range
 _int egts_curve[EGTS_LOOKUP_TABLE_SIZE+2];

 // Oil pressure vs voltage. 16 points of function, plus two values for setting of x-axis range
 _int ops_curve[OPS_LOOKUP_TABLE_SIZE+2];

 //injection PW coefficient vs voltage
 _int injpw_coef[INJPWCOEF_LUT_SIZE];

 //firmware constants:
 _int evap_clt;
 _uchar evap_tps_lo;
 _uchar evap_tps_hi;
 _uchar fi_enter_strokes;
 _uchar fi_leave_strokes;
 _uchar iac_cond_add;
 _uint inj_max_pw;
 _int  aircond_clt;
 _uchar aircond_tps;
 _int  idl_ve;
 _uint frap;
 _int  idl_ve_g;
 _uchar stbl_str_cnt;
 _int  reserv_0;        //reserved!
 _int  heating_t_off;
 _uchar heating_time;
 _uchar idltorun_stp_en;
 _uchar idltorun_stp_le;
 _uchar inpavnum[INPAVNUM];
 _uchar vent_delay;
 _uchar vent_iacoff;
 _uchar epas_iacoff;
 _uchar vent_pwmsteps;
 _uchar vent_minband;
 _uchar an_tps_mul;
 _uchar igntim_wrkmap;
 _int   shift_igntim;
 _uchar fldclr_start;
 _uchar hall_predict;
 _uint  vtachom_mult; //value * 8192
 _uint  grheat_time;
 _uchar add_i1_sub;
 _uchar add_i2_sub;
 _uint  idlreg_captrange;
 _uchar manigntim_idl;
 _uchar idlent_timval;
 _uint  gasval_ontime;
 _uint  tdc_angle[8];
 _uint  smp_angle;
 _uint  dwl_dead_time;
 _uchar sfc_tps_thrd;
 _uint evap_map_thrd;
 _uint ckps_skip_trig;
 _uchar maninjpw_idl;
 _uchar oilpress_cut;
 _uint  tpsdot_mindt;
 _uchar irr_k_load;
 _uchar irr_k_rpm;
 _uchar cold_eng_int;
 
 //Эти зарезервированные байты необходимы для сохранения бинарной совместимости
 //новых версий прошивок с более старыми версиями. При добавлении новых данных
 //в структуру, необходимо расходовать эти байты.
 _uchar reserved[3562];
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
 _uint        fw_data_size;

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

BYTE* CFirmwareDataMediator::getBytes(bool i_original /* = false*/)
{
 if (i_original)
  return mp_bytes_original;
 else
  return mp_bytes_active;
}

const BYTE* CFirmwareDataMediator::getBytes(bool i_original /* = false*/) const
{
 if (i_original)
  return mp_bytes_original;
 else
  return mp_bytes_active;
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
  //supported major versions: 3
  ((IOREM_MAJ_VER(p_cd->iorem.version) != 3)))
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

BYTE* CFirmwareDataMediator::GetFWVersion(BYTE* fw_version) const
{
 //if there is no such data in this firmware, then return zero
 fw_version[0] = mp_cddata ? mp_cddata->fw_version & 0x0F : 0;
 fw_version[1] = mp_cddata ? mp_cddata->fw_version >> 4   : 0;
 return fw_version;
}

void CFirmwareDataMediator::GetStartMap(int i_index,float* op_values, bool i_original /* = false */)
{
 ASSERT(op_values);

 //получаем адрес начала таблиц семейств характеристик
 fw_data_t* p_fd = (fw_data_t*)(&getBytes(i_original)[m_lip->FIRMWARE_DATA_START]);

 for (int i = 0; i < F_STR_POINTS; i++ )
  op_values[i] = ((float)p_fd->tables[i_index].f_str[i]) / AA_MAPS_M_FACTOR;
}

void CFirmwareDataMediator::SetStartMap(int i_index,const float* ip_values)
{
 ASSERT(ip_values);

 //получаем адрес начала таблиц семейств характеристик
 fw_data_t* p_fd = (fw_data_t*)(&getBytes()[m_lip->FIRMWARE_DATA_START]);

 for (int i = 0; i < F_STR_POINTS; i++ )
  p_fd->tables[i_index].f_str[i] = MathHelpers::Round((ip_values[i]*AA_MAPS_M_FACTOR));
}

void CFirmwareDataMediator::GetIdleMap(int i_index,float* op_values, bool i_original /* = false */)
{
 ASSERT(op_values);

 //получаем адрес начала таблиц семейств характеристик
 fw_data_t* p_fd = (fw_data_t*)(&getBytes(i_original)[m_lip->FIRMWARE_DATA_START]);

 for (int i = 0; i < F_IDL_POINTS; i++ )
  op_values[i] = ((float)p_fd->tables[i_index].f_idl[i]) / AA_MAPS_M_FACTOR;
}

void CFirmwareDataMediator::SetIdleMap(int i_index,const float* ip_values)
{
 ASSERT(ip_values);

 //получаем адрес начала таблиц семейств характеристик
 fw_data_t* p_fd = (fw_data_t*)(&getBytes()[m_lip->FIRMWARE_DATA_START]);

 for (int i = 0; i < F_IDL_POINTS; i++ )
  p_fd->tables[i_index].f_idl[i] = MathHelpers::Round((ip_values[i]*AA_MAPS_M_FACTOR));
}

std::vector<_TSTRING> CFirmwareDataMediator::GetFunctionsSetNames(void)
{
 std::vector<_TSTRING> names(TABLES_NUMBER);

 //получаем адрес начала таблиц семейств характеристик
 fw_data_t* p_fd = (fw_data_t*)(&getBytes()[m_lip->FIRMWARE_DATA_START]);

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

 //получаем адрес начала таблиц семейств характеристик
 fw_data_t* p_fd = (fw_data_t*)(&getBytes()[m_lip->FIRMWARE_DATA_START]);

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
 size_t dataSize = p_fd->fw_data_size;
 _uint oldFWCRC  = p_fd->code_crc;

 if (ip_fpp)
 {
  LocInfoProvider lip(*ip_fpp);

  //shrink size using value specified in fw_data_size if fw_data_size contain a valid value
  size_t start_d = (0==dataSize) ? m_lip->FIRMWARE_DATA_START : (m_lip->BOOT_START - dataSize);
  size_t start_s = (0==dataSize) ? lip.FIRMWARE_DATA_START : (lip.BOOT_START - dataSize);
  size_t size = (0==dataSize) ? m_lip->FIRMWARE_DATA_SIZE : dataSize; 
  memcpy(mp_bytes_active + start_d, ip_source_bytes + start_s, size);
 }
 else
 {
  //shrink size using value specified in fw_data_size if fw_data_size contain a valid value
  size_t start = (0==dataSize) ? m_lip->FIRMWARE_DATA_START : (m_lip->BOOT_START - dataSize);
  size_t size = (0==dataSize) ? m_lip->FIRMWARE_DATA_SIZE : dataSize; 
  memcpy(mp_bytes_active + start, ip_source_bytes + start, size);  
 }
 //Значение fw_data_size не импортируем, так как оно служебное.
 p_fd->fw_data_size = dataSize;

 //не импортируем контрольную сумму прошивки
 p_fd->code_crc = oldFWCRC;

 mp_cddata = _FindCodeData();

 //Compensate ADC correction factors when destination and source firmware use different ADC Vref
 DWORD dst_fwopt = GetFWOptions();
 DWORD src_fwopt = GetFWOptions(ip_source_bytes, ip_fpp ? ip_fpp : m_fpp.get());
 if (CHECKBIT32(dst_fwopt, SECU3IO::COPT_VREF_5V) && !CHECKBIT32(src_fwopt, SECU3IO::COPT_VREF_5V))
  _CompensateVRef(&p_fd->def_param, true);  //5v <-- 2.56v
 if (!CHECKBIT32(dst_fwopt, SECU3IO::COPT_VREF_5V) && CHECKBIT32(src_fwopt, SECU3IO::COPT_VREF_5V))
  _CompensateVRef(&p_fd->def_param, false); //5v --> 2.56v
}

void CFirmwareDataMediator::LoadDefParametersFromBuffer(const BYTE* ip_source_bytes, EventHandler onVrefUsrConfirm /*= NULL*/)
{
 if (false==IsLoaded())
  return; //некуда загружать...
 fw_data_t* p_fd = (fw_data_t*)(&mp_bytes_active[m_lip->FIRMWARE_DATA_START]);
 _uint fwd_size = p_fd->fw_data_size; //save
 memcpy(&p_fd->def_param, ip_source_bytes, sizeof(params_t));
 p_fd->fw_data_size = fwd_size; //restore
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
 bool dst_vref_5v = CHECKBIT32(GetFWOptions(), SECU3IO::COPT_VREF_5V);
 if (src_vref_5v != dst_vref_5v) //different ADC voltage reference?
 {//Ask user, before applying compensation
  if ((onVrefUsrConfirm) ? onVrefUsrConfirm() : true)
   _CompensateVRef(&par, (!src_vref_5v && dst_vref_5v)); //5v <-- 2.56v or 5v --> 2.56v
 }
}

void CFirmwareDataMediator::GetWorkMap(int i_index, float* op_values, bool i_original /* = false*/)
{
 ASSERT(op_values);

 //получаем адрес начала таблиц семейств характеристик
 fw_data_t* p_fd = (fw_data_t*)(&getBytes(i_original)[m_lip->FIRMWARE_DATA_START]);

 for (int i = 0; i < (F_WRK_POINTS_F * F_WRK_POINTS_L); i++ )
 {
  _char *p = &(p_fd->tables[i_index].f_wrk[0][0]);
  op_values[i] = ((float) *(p + i)) / AA_MAPS_M_FACTOR;
 }
}

void CFirmwareDataMediator::SetWorkMap(int i_index, const float* ip_values)
{
 ASSERT(ip_values);

 //получаем адрес начала таблиц семейств характеристик
 fw_data_t* p_fd = (fw_data_t*)(&getBytes()[m_lip->FIRMWARE_DATA_START]);

 for (int i = 0; i < (F_WRK_POINTS_F * F_WRK_POINTS_L); i++ )
 {
  _char *p = &(p_fd->tables[i_index].f_wrk[0][0]);
  *(p + i) = MathHelpers::Round((ip_values[i]*AA_MAPS_M_FACTOR));
 }
}

void CFirmwareDataMediator::GetTempMap(int i_index,float* op_values, bool i_original /* = false */)
{
 ASSERT(op_values);

 //получаем адрес начала таблиц семейств характеристик
 fw_data_t* p_fd = (fw_data_t*)(&getBytes(i_original)[m_lip->FIRMWARE_DATA_START]);

 for (int i = 0; i < F_TMP_POINTS; i++ )
  op_values[i] = ((float)p_fd->tables[i_index].f_tmp[i]) / AA_MAPS_M_FACTOR;
}

void CFirmwareDataMediator::SetTempMap(int i_index,const float* ip_values)
{
 ASSERT(ip_values);

 //получаем адрес начала таблиц семейств характеристик
 fw_data_t* p_fd = (fw_data_t*)(&getBytes()[m_lip->FIRMWARE_DATA_START]);

 for (int i = 0; i < F_TMP_POINTS; i++ )
  p_fd->tables[i_index].f_tmp[i] = MathHelpers::Round((ip_values[i]*AA_MAPS_M_FACTOR));
}

void CFirmwareDataMediator::GetTempIdlMap(int i_index, float* op_values, bool i_original /* = false */)
{
 ASSERT(op_values);

 //получаем адрес начала таблиц семейств характеристик
 fw_data_t* p_fd = (fw_data_t*)(&getBytes(i_original)[m_lip->FIRMWARE_DATA_START]);

 for (int i = 0; i < F_TMP_POINTS; i++ )
  op_values[i] = ((float)p_fd->tables[i_index].f_tmp_idl[i]) / AA_MAPS_M_FACTOR;
}

void CFirmwareDataMediator::SetTempIdlMap(int i_index, const float* ip_values)
{
 ASSERT(ip_values);

 //получаем адрес начала таблиц семейств характеристик
 fw_data_t* p_fd = (fw_data_t*)(&getBytes()[m_lip->FIRMWARE_DATA_START]);

 for (int i = 0; i < F_TMP_POINTS; i++ )
  p_fd->tables[i_index].f_tmp_idl[i] = MathHelpers::Round((ip_values[i]*AA_MAPS_M_FACTOR));
}

void CFirmwareDataMediator::GetVEMap(int i_index, float* op_values, bool i_original /* = false*/)
{
 ASSERT(op_values);

 //получаем адрес начала таблиц семейств характеристик
 fw_data_t* p_fd = (fw_data_t*)(&getBytes(i_original)[m_lip->FIRMWARE_DATA_START]);

 for (int i = 0; i < (INJ_VE_POINTS_F * INJ_VE_POINTS_L); i++ )
 {
  _uchar *p = &(p_fd->tables[i_index].inj_ve[0][0]);
  op_values[i] = ((float)w12GetCell(p, i)) / VE_MAPS_M_FACTOR;
 }
}

void CFirmwareDataMediator::SetVEMap(int i_index, const float* ip_values)
{
 ASSERT(ip_values);

 //получаем адрес начала таблиц семейств характеристик
 fw_data_t* p_fd = (fw_data_t*)(&getBytes()[m_lip->FIRMWARE_DATA_START]);

 for (int i = 0; i < (INJ_VE_POINTS_F * INJ_VE_POINTS_L); i++ )
 {
  _uchar *p = &(p_fd->tables[i_index].inj_ve[0][0]);
  w12SetCell(p, i, MathHelpers::Round(ip_values[i]*VE_MAPS_M_FACTOR));
 }
}

void CFirmwareDataMediator::GetVE2Map(int i_index, float* op_values, bool i_original /* = false*/)
{
 ASSERT(op_values);

 //получаем адрес начала таблиц семейств характеристик
 fw_data_t* p_fd = (fw_data_t*)(&getBytes(i_original)[m_lip->FIRMWARE_DATA_START]);

 for (int i = 0; i < (INJ_VE_POINTS_F * INJ_VE_POINTS_L); i++ )
 {
  _uchar *p = &(p_fd->tables[i_index].inj_ve2[0][0]);
  op_values[i] = ((float)w12GetCell(p, i)) / VE_MAPS_M_FACTOR;
 }
}

void CFirmwareDataMediator::SetVE2Map(int i_index, const float* ip_values)
{
 ASSERT(ip_values);

 //получаем адрес начала таблиц семейств характеристик
 fw_data_t* p_fd = (fw_data_t*)(&getBytes()[m_lip->FIRMWARE_DATA_START]);

 for (int i = 0; i < (INJ_VE_POINTS_F * INJ_VE_POINTS_L); i++ )
 {
  _uchar *p = &(p_fd->tables[i_index].inj_ve2[0][0]);
  w12SetCell(p, i, MathHelpers::Round(ip_values[i]*VE_MAPS_M_FACTOR));
 }
}

void CFirmwareDataMediator::GetAFRMap(int i_index, float* op_values, bool i_original /* = false*/)
{
 ASSERT(op_values);

 //получаем адрес начала таблиц семейств характеристик
 fw_data_t* p_fd = (fw_data_t*)(&getBytes(i_original)[m_lip->FIRMWARE_DATA_START]);

 for (int i = 0; i < (INJ_VE_POINTS_F * INJ_VE_POINTS_L); i++ )
 {
  _uchar *p = &(p_fd->tables[i_index].inj_afr[0][0]);
  float value = (float) *(p + i);
  op_values[i] = MathHelpers::RoundP1((value / AFR_MAPS_M_FACTOR) + 8.0f);
 }
}

void CFirmwareDataMediator::SetAFRMap(int i_index, const float* ip_values)
{
 ASSERT(ip_values);

 //получаем адрес начала таблиц семейств характеристик
 fw_data_t* p_fd = (fw_data_t*)(&getBytes()[m_lip->FIRMWARE_DATA_START]);

 for (int i = 0; i < (INJ_VE_POINTS_F * INJ_VE_POINTS_L); i++ )
 {
  _uchar *p = &(p_fd->tables[i_index].inj_afr[0][0]);
  *(p + i) = MathHelpers::Round((ip_values[i]-8.0) * AFR_MAPS_M_FACTOR);
 }
}

void CFirmwareDataMediator::GetCrnkMap(int i_index, float* op_values, bool i_original /* = false */)
{
 ASSERT(op_values);
 if (!op_values)
  return;

 //получаем адрес структуры дополнительных данных
 fw_data_t* p_fd = (fw_data_t*)(&getBytes(i_original)[m_lip->FIRMWARE_DATA_START]);

 float discrete = PlatformParamHolder::GetQuartzFact(m_fpp->m_platform_id); //for ATMega644 discrete = 3.2uS, for others - 4.0uS
 for(size_t i = 0; i < INJ_CRANKING_LOOKUP_TABLE_SIZE; i++)
  op_values[i] = (p_fd->tables[i_index].inj_cranking[i] * discrete) / 1000.0f; //convert to ms
}

void CFirmwareDataMediator::SetCrnkMap(int i_index, const float* ip_values)
{
 ASSERT(ip_values);
 if (!ip_values)
  return;

 //получаем адрес структуры дополнительных данных
 fw_data_t* p_fd = (fw_data_t*)(&getBytes()[m_lip->FIRMWARE_DATA_START]);

 float discrete = PlatformParamHolder::GetQuartzFact(m_fpp->m_platform_id); //for ATMega644 discrete = 3.2uS, for others - 4.0uS
 for(size_t i = 0; i < INJ_CRANKING_LOOKUP_TABLE_SIZE; i++)
  p_fd->tables[i_index].inj_cranking[i] = (_uint)MathHelpers::Round((ip_values[i] * 1000.0) / discrete);
}

void CFirmwareDataMediator::GetWrmpMap(int i_index, float* op_values, bool i_original /* = false */)
{
 ASSERT(op_values);

 //получаем адрес начала таблиц семейств характеристик
 fw_data_t* p_fd = (fw_data_t*)(&getBytes(i_original)[m_lip->FIRMWARE_DATA_START]);

 for (int i = 0; i < INJ_WARMUP_LOOKUP_TABLE_SIZE; i++ )
  op_values[i] = ((float)p_fd->tables[i_index].inj_warmup[i]) / WRMP_MAPS_M_FACTOR;
}

void CFirmwareDataMediator::SetWrmpMap(int i_index,const float* ip_values)
{
 ASSERT(ip_values);

 //получаем адрес начала таблиц семейств характеристик
 fw_data_t* p_fd = (fw_data_t*)(&getBytes()[m_lip->FIRMWARE_DATA_START]);

 for (int i = 0; i < INJ_WARMUP_LOOKUP_TABLE_SIZE; i++ )
  p_fd->tables[i_index].inj_warmup[i] = MathHelpers::Round((ip_values[i]*WRMP_MAPS_M_FACTOR));
}

void CFirmwareDataMediator::GetDeadMap(int i_index, float* op_values, bool i_original /* = false */)
{
 ASSERT(op_values);
 if (!op_values)
  return;

 //получаем адрес структуры дополнительных данных
 fw_data_t* p_fd = (fw_data_t*)(&getBytes(i_original)[m_lip->FIRMWARE_DATA_START]);

 float discrete = PlatformParamHolder::GetQuartzFact(m_fpp->m_platform_id); //for ATMega644 discrete = 3.2uS, for others - 4.0uS
 for(size_t i = 0; i < INJ_DT_LOOKUP_TABLE_SIZE; i++)
  op_values[i] = (p_fd->tables[i_index].inj_dead_time[i] * discrete) / 1000.0f; //convert to ms
}

void CFirmwareDataMediator::SetDeadMap(int i_index, const float* ip_values)
{
 ASSERT(ip_values);
 if (!ip_values)
  return;

 //получаем адрес структуры дополнительных данных
 fw_data_t* p_fd = (fw_data_t*)(&getBytes()[m_lip->FIRMWARE_DATA_START]);

 float discrete = PlatformParamHolder::GetQuartzFact(m_fpp->m_platform_id); //for ATMega644 discrete = 3.2uS, for others - 4.0uS
 for(size_t i = 0; i < INJ_DT_LOOKUP_TABLE_SIZE; i++)
  p_fd->tables[i_index].inj_dead_time[i] = (_int)MathHelpers::Round((ip_values[i] * 1000.0) / discrete);
}

void CFirmwareDataMediator::GetIdlrMap(int i_index,float* op_values, bool i_original /* = false */)
{
 ASSERT(op_values);

 //получаем адрес начала таблиц семейств характеристик
 fw_data_t* p_fd = (fw_data_t*)(&getBytes(i_original)[m_lip->FIRMWARE_DATA_START]);

 for (int i = 0; i < INJ_IAC_POS_TABLE_SIZE; i++ )
  op_values[i] = ((float)p_fd->tables[i_index].inj_iac_run_pos[i]) / IACPOS_MAPS_M_FACTOR;
}

void CFirmwareDataMediator::SetIdlrMap(int i_index,const float* ip_values)
{
 ASSERT(ip_values);

 //получаем адрес начала таблиц семейств характеристик
 fw_data_t* p_fd = (fw_data_t*)(&getBytes()[m_lip->FIRMWARE_DATA_START]);

 for (int i = 0; i < INJ_IAC_POS_TABLE_SIZE; i++ )
  p_fd->tables[i_index].inj_iac_run_pos[i] = MathHelpers::Round((ip_values[i]*IACPOS_MAPS_M_FACTOR));
}

void CFirmwareDataMediator::GetIdlcMap(int i_index,float* op_values, bool i_original /* = false */)
{
 ASSERT(op_values);

 //получаем адрес начала таблиц семейств характеристик
 fw_data_t* p_fd = (fw_data_t*)(&getBytes(i_original)[m_lip->FIRMWARE_DATA_START]);

 for (int i = 0; i < INJ_IAC_POS_TABLE_SIZE; i++ )
  op_values[i] = ((float)p_fd->tables[i_index].inj_iac_crank_pos[i]) / IACPOS_MAPS_M_FACTOR;
}

void CFirmwareDataMediator::SetIdlcMap(int i_index,const float* ip_values)
{
 ASSERT(ip_values);

 //получаем адрес начала таблиц семейств характеристик
 fw_data_t* p_fd = (fw_data_t*)(&getBytes()[m_lip->FIRMWARE_DATA_START]);

 for (int i = 0; i < INJ_IAC_POS_TABLE_SIZE; i++ )
  p_fd->tables[i_index].inj_iac_crank_pos[i] = MathHelpers::Round((ip_values[i]*IACPOS_MAPS_M_FACTOR));
}

void CFirmwareDataMediator::GetAETPSMap(int i_index, float* op_values, bool i_original /* = false */)
{
 ASSERT(op_values);

 //получаем адрес начала таблиц семейств характеристик
 fw_data_t* p_fd = (fw_data_t*)(&getBytes(i_original)[m_lip->FIRMWARE_DATA_START]);

 for (int i = 0; i < INJ_AE_TPS_LOOKUP_TABLE_SIZE; i++ )
  op_values[i] = ((float)p_fd->tables[i_index].inj_ae_tps_enr[i]) - AETPSV_MAPS_ADDER; //shift by 55
 for (int i = 0; i < INJ_AE_TPS_LOOKUP_TABLE_SIZE; i++ )
  op_values[i+INJ_AE_TPS_LOOKUP_TABLE_SIZE] = ((float)p_fd->tables[i_index].inj_ae_tps_bins[i]) / AETPSB_MAPS_M_FACTOR;   //convert from %/100ms to %/s
}

void CFirmwareDataMediator::SetAETPSMap(int i_index, const float* ip_values)
{
 ASSERT(ip_values);

 //получаем адрес начала таблиц семейств характеристик
 fw_data_t* p_fd = (fw_data_t*)(&getBytes()[m_lip->FIRMWARE_DATA_START]);

 for (int i = 0; i < INJ_AE_TPS_LOOKUP_TABLE_SIZE; i++ )
  p_fd->tables[i_index].inj_ae_tps_enr[i] = MathHelpers::Round((ip_values[i] + AETPSV_MAPS_ADDER));
 for (int i = 0; i < INJ_AE_TPS_LOOKUP_TABLE_SIZE; i++ )
  p_fd->tables[i_index].inj_ae_tps_bins[i] = MathHelpers::Round((ip_values[i+INJ_AE_TPS_LOOKUP_TABLE_SIZE]*AETPSB_MAPS_M_FACTOR));
}

void CFirmwareDataMediator::GetAERPMMap(int i_index, float* op_values, bool i_original /* = false */)
{
 ASSERT(op_values);

 //получаем адрес начала таблиц семейств характеристик
 fw_data_t* p_fd = (fw_data_t*)(&getBytes(i_original)[m_lip->FIRMWARE_DATA_START]);

 for (int i = 0; i < INJ_AE_RPM_LOOKUP_TABLE_SIZE; i++ )
  op_values[i] = ((float)p_fd->tables[i_index].inj_ae_rpm_enr[i]) / AERPMV_MAPS_M_FACTOR; //convert to %
 for (int i = 0; i < INJ_AE_RPM_LOOKUP_TABLE_SIZE; i++ )
  op_values[i+INJ_AE_RPM_LOOKUP_TABLE_SIZE] = ((float)p_fd->tables[i_index].inj_ae_rpm_bins[i]) / AERPMB_MAPS_M_FACTOR;   //convert from (%*2)/100ms to %/s
}

void CFirmwareDataMediator::SetAERPMMap(int i_index, const float* ip_values)
{
 ASSERT(ip_values);

 //получаем адрес начала таблиц семейств характеристик
 fw_data_t* p_fd = (fw_data_t*)(&getBytes()[m_lip->FIRMWARE_DATA_START]);

 for (int i = 0; i < INJ_AE_RPM_LOOKUP_TABLE_SIZE; i++ )
  p_fd->tables[i_index].inj_ae_rpm_enr[i] = MathHelpers::Round((ip_values[i]*AERPMV_MAPS_M_FACTOR));
 for (int i = 0; i < INJ_AE_RPM_LOOKUP_TABLE_SIZE; i++ )
  p_fd->tables[i_index].inj_ae_rpm_bins[i] = MathHelpers::Round((ip_values[i+INJ_AE_RPM_LOOKUP_TABLE_SIZE]*AERPMB_MAPS_M_FACTOR));
}

void CFirmwareDataMediator::GetAftstrMap(int i_index, float* op_values, bool i_original /* = false */)
{
 ASSERT(op_values);

 //получаем адрес начала таблиц семейств характеристик
 fw_data_t* p_fd = (fw_data_t*)(&getBytes(i_original)[m_lip->FIRMWARE_DATA_START]);

 for (int i = 0; i < INJ_AFTSTR_LOOKUP_TABLE_SIZE; i++ )
  op_values[i] = ((float)p_fd->tables[i_index].inj_aftstr[i]) / AFTSTR_MAPS_M_FACTOR;
}

void CFirmwareDataMediator::SetAftstrMap(int i_index,const float* ip_values)
{
 ASSERT(ip_values);

 //получаем адрес начала таблиц семейств характеристик
 fw_data_t* p_fd = (fw_data_t*)(&getBytes()[m_lip->FIRMWARE_DATA_START]);

 for (int i = 0; i < INJ_AFTSTR_LOOKUP_TABLE_SIZE; i++ )
  p_fd->tables[i_index].inj_aftstr[i] = MathHelpers::Round((ip_values[i]*AFTSTR_MAPS_M_FACTOR));
}

void CFirmwareDataMediator::GetITMap(int i_index, float* op_values, bool i_original /* = false*/)
{
 ASSERT(op_values);

 //получаем адрес начала таблиц семейств характеристик
 fw_data_t* p_fd = (fw_data_t*)(&getBytes(i_original)[m_lip->FIRMWARE_DATA_START]);

 for (int i = 0; i < (INJ_VE_POINTS_F * INJ_VE_POINTS_L); i++ )
 {
  _uchar *p = &(p_fd->tables[i_index].inj_timing[0][0]);
  op_values[i] = ((float)w12GetCell(p, i)) / IT_MAPS_M_FACTOR;
 }
}

void CFirmwareDataMediator::SetITMap(int i_index, const float* ip_values)
{
 ASSERT(ip_values);

 //получаем адрес начала таблиц семейств характеристик
 fw_data_t* p_fd = (fw_data_t*)(&getBytes()[m_lip->FIRMWARE_DATA_START]);

 for (int i = 0; i < (INJ_VE_POINTS_F * INJ_VE_POINTS_L); i++ )
 {
  _uchar *p = &(p_fd->tables[i_index].inj_timing[0][0]);
  w12SetCell(p, i, MathHelpers::Round(ip_values[i] * IT_MAPS_M_FACTOR)); 
 }
}

void CFirmwareDataMediator::GetITRPMMap(int i_index, float* op_values, bool i_original /*= false*/)
{
 ASSERT(op_values);

 //gets address of the sets of maps
 fw_data_t* p_fd = (fw_data_t*)(&getBytes(i_original)[m_lip->FIRMWARE_DATA_START]);

 for (int i = 0; i < INJ_TARGET_RPM_TABLE_SIZE; i++ )
  op_values[i] = ((float)p_fd->tables[i_index].inj_target_rpm[i]) * 10;
}

void CFirmwareDataMediator::SetITRPMMap(int i_index, const float* ip_values)
{
 ASSERT(ip_values);

 //gets address of the sets of maps
 fw_data_t* p_fd = (fw_data_t*)(&getBytes()[m_lip->FIRMWARE_DATA_START]);

 for (int i = 0; i < INJ_TARGET_RPM_TABLE_SIZE; i++ )
  p_fd->tables[i_index].inj_target_rpm[i] = MathHelpers::Round((ip_values[i] / 10.0));
}

void CFirmwareDataMediator::GetRigidMap(int i_index, float* op_values, bool i_original /*= false*/)
{
 ASSERT(op_values);

 //gets address of the sets of maps
 fw_data_t* p_fd = (fw_data_t*)(&getBytes(i_original)[m_lip->FIRMWARE_DATA_START]);

 for (int i = 0; i < INJ_IDL_RIGIDITY_TABLE_SIZE; i++ )
  op_values[i] = ((float)p_fd->tables[i_index].inj_idl_rigidity[i]) / 128.0f;
}

void CFirmwareDataMediator::SetRigidMap(int i_index, const float* ip_values)
{
 ASSERT(ip_values);

 //gets address of the sets of maps
 fw_data_t* p_fd = (fw_data_t*)(&getBytes()[m_lip->FIRMWARE_DATA_START]);

 for (int i = 0; i < INJ_IDL_RIGIDITY_TABLE_SIZE; i++ )
  p_fd->tables[i_index].inj_idl_rigidity[i] = MathHelpers::Round((ip_values[i] * 128.0f));
}

void CFirmwareDataMediator::GetEGOCurveMap(int i_index, float* op_values, bool i_original /*= false*/)
{
 ASSERT(op_values);

 //gets address of the sets of maps
 fw_data_t* p_fd = (fw_data_t*)(&getBytes(i_original)[m_lip->FIRMWARE_DATA_START]);

 int i = 0;
 for (; i < INJ_EGO_CURVE_SIZE; i++ )
 {
  float value = (float)p_fd->tables[i_index].inj_ego_curve[i];
  op_values[i] = (value / EGO_CURVE_M_FACTOR);
 }

 for (; i < INJ_EGO_CURVE_SIZE+2; i++ )
 {
  float value = (float)p_fd->tables[i_index].inj_ego_curve[i];
  op_values[i] = value * ADC_DISCRETE;
 }
}

void CFirmwareDataMediator::SetEGOCurveMap(int i_index, const float* ip_values)
{
 ASSERT(ip_values);

 //gets address of the sets of maps
 fw_data_t* p_fd = (fw_data_t*)(&getBytes()[m_lip->FIRMWARE_DATA_START]);

 int i = 0;
 for (; i < INJ_EGO_CURVE_SIZE; i++ )
  p_fd->tables[i_index].inj_ego_curve[i] = MathHelpers::Round(ip_values[i] * EGO_CURVE_M_FACTOR);
 for (; i < INJ_EGO_CURVE_SIZE+2; i++ )
  p_fd->tables[i_index].inj_ego_curve[i] = MathHelpers::Round(ip_values[i] / ADC_DISCRETE);
}

void CFirmwareDataMediator::GetIACCorrWMap(int i_index, float* op_values, bool i_original /*= false*/)
{
 ASSERT(op_values);

 //gets address of the sets of maps
 fw_data_t* p_fd = (fw_data_t*)(&getBytes(i_original)[m_lip->FIRMWARE_DATA_START]);

 int i = 0;
 for (; i < INJ_IAC_CORR_W_SIZE; i++ )
 {
  float value = (float)p_fd->tables[i_index].inj_iac_corr_w[i];
  op_values[i] = (value / 256.0f);
 }

 for (; i < INJ_IAC_CORR_W_SIZE+2; i++ )
 {
  float value = (float)p_fd->tables[i_index].inj_iac_corr_w[i];
  op_values[i] = value / 2.0f;
 }
}

void CFirmwareDataMediator::SetIACCorrWMap(int i_index, const float* ip_values)
{
 ASSERT(ip_values);

 //gets address of the sets of maps
 fw_data_t* p_fd = (fw_data_t*)(&getBytes()[m_lip->FIRMWARE_DATA_START]);

 int i = 0;
 for (; i < INJ_IAC_CORR_W_SIZE; i++ )
 {
  int value = MathHelpers::Round(ip_values[i] * 256.0f);
  p_fd->tables[i_index].inj_iac_corr_w[i] = (value > 255) ? 255 : value;
 }
 for (; i < INJ_IAC_CORR_W_SIZE+2; i++ )
  p_fd->tables[i_index].inj_iac_corr_w[i] = MathHelpers::Round(ip_values[i] * 2.0f);
}


void CFirmwareDataMediator::GetIACCorrMap(int i_index, float* op_values, bool i_original /*= false*/)
{
 ASSERT(op_values);

 //gets address of the sets of maps
 fw_data_t* p_fd = (fw_data_t*)(&getBytes(i_original)[m_lip->FIRMWARE_DATA_START]);

 int i = 0;
 for (; i < INJ_IAC_CORR_SIZE; i++ )
 {
  float value = (float)p_fd->tables[i_index].inj_iac_corr[i];
  op_values[i] = ((value + 8192) / 8192.0f);
 }

 for (; i < INJ_IAC_CORR_SIZE+2; i++ )
 {
  float value = (float)p_fd->tables[i_index].inj_iac_corr[i];
  op_values[i] = value / 128.0f;
 }
}

void CFirmwareDataMediator::SetIACCorrMap(int i_index, const float* ip_values)
{
 ASSERT(ip_values);

 //gets address of the sets of maps
 fw_data_t* p_fd = (fw_data_t*)(&getBytes()[m_lip->FIRMWARE_DATA_START]);

 int i = 0;
 for (; i < INJ_IAC_CORR_SIZE; i++ )
  p_fd->tables[i_index].inj_iac_corr[i] = MathHelpers::Round((ip_values[i] - 1.0f) * 8192.0f);
 for (; i < INJ_IAC_CORR_SIZE+2; i++ )
  p_fd->tables[i_index].inj_iac_corr[i] = MathHelpers::Round(ip_values[i] * 128.0f);
}

void CFirmwareDataMediator::GetIATCLTMap(int i_index, float* op_values, bool i_original /*= false*/)
{
 ASSERT(op_values);

 //gets address of the sets of maps
 fw_data_t* p_fd = (fw_data_t*)(&getBytes(i_original)[m_lip->FIRMWARE_DATA_START]);

 int i = 0;
 for (; i < INJ_IATCLT_CORR_SIZE; i++ )
 {
  float value = (float)p_fd->tables[i_index].inj_iatclt_corr[i];
  op_values[i] = (value / 8192.0f);
 }

 for (; i < INJ_IATCLT_CORR_SIZE+2; i++ )
 {
  float value = (float)p_fd->tables[i_index].inj_iatclt_corr[i];
  op_values[i] = value * 32.0f;
 }
}

void CFirmwareDataMediator::SetIATCLTMap(int i_index, const float* ip_values)
{
 ASSERT(ip_values);

 //gets address of the sets of maps
 fw_data_t* p_fd = (fw_data_t*)(&getBytes()[m_lip->FIRMWARE_DATA_START]);

 int i = 0;
 for (; i < INJ_IATCLT_CORR_SIZE; i++ )
  p_fd->tables[i_index].inj_iatclt_corr[i] = MathHelpers::Round(ip_values[i] * 8192.0f);
 for (; i < INJ_IATCLT_CORR_SIZE+2; i++ )
  p_fd->tables[i_index].inj_iatclt_corr[i] = MathHelpers::Round(ip_values[i] / 32.0f);
}


void CFirmwareDataMediator::GetTpsswtMap(int i_index,float* op_values, bool i_original /* = false */)
{
 ASSERT(op_values);

 //получаем адрес начала таблиц семейств характеристик
 fw_data_t* p_fd = (fw_data_t*)(&getBytes(i_original)[m_lip->FIRMWARE_DATA_START]);

 for (int i = 0; i < INJ_TPSSWT_SIZE; i++ )
  op_values[i] = ((float)p_fd->tables[i_index].inj_tpsswt[i]) / TPSSWT_MAPS_M_FACTOR;
}

void CFirmwareDataMediator::SetTpsswtMap(int i_index,const float* ip_values)
{
 ASSERT(ip_values);

 //получаем адрес начала таблиц семейств характеристик
 fw_data_t* p_fd = (fw_data_t*)(&getBytes()[m_lip->FIRMWARE_DATA_START]);

 for (int i = 0; i < INJ_TPSSWT_SIZE; i++ )
  p_fd->tables[i_index].inj_tpsswt[i] = MathHelpers::Round((ip_values[i]*TPSSWT_MAPS_M_FACTOR));
}

void CFirmwareDataMediator::GetGtscMap(int i_index,float* op_values, bool i_original /* = false */)
{
 ASSERT(op_values);

 //получаем адрес начала таблиц семейств характеристик
 fw_data_t* p_fd = (fw_data_t*)(&getBytes(i_original)[m_lip->FIRMWARE_DATA_START]);

 for (int i = 0; i < INJ_GTS_CORR_SIZE; i++ )
  op_values[i] = ((float)p_fd->tables[i_index].inj_gts_corr[i]) / 128.0f;
}

void CFirmwareDataMediator::SetGtscMap(int i_index,const float* ip_values)
{
 ASSERT(ip_values);

 //получаем адрес начала таблиц семейств характеристик
 fw_data_t* p_fd = (fw_data_t*)(&getBytes()[m_lip->FIRMWARE_DATA_START]);

 for (int i = 0; i < INJ_GTS_CORR_SIZE; i++ )
  p_fd->tables[i_index].inj_gts_corr[i] = MathHelpers::Round((ip_values[i]*128.0f));
}

void CFirmwareDataMediator::GetGpscMap(int i_index, float* op_values, bool i_original /*= false*/)
{
 ASSERT(op_values);

 //gets address of the sets of maps
 fw_data_t* p_fd = (fw_data_t*)(&getBytes(i_original)[m_lip->FIRMWARE_DATA_START]);

 int i = 0;
 for (; i < INJ_GPS_CORR_SIZE; i++ )
 {
  float value = (float)p_fd->tables[i_index].inj_gps_corr[i];
  op_values[i] = (value / 128.0f);
 }

 for (; i < INJ_GPS_CORR_SIZE+2; i++ )
 {
  float value = (float)p_fd->tables[i_index].inj_gps_corr[i];
  op_values[i] = value * 2.0f;
 }
}

void CFirmwareDataMediator::SetGpscMap(int i_index, const float* ip_values)
{
 ASSERT(ip_values);

 //gets address of the sets of maps
 fw_data_t* p_fd = (fw_data_t*)(&getBytes()[m_lip->FIRMWARE_DATA_START]);

 int i = 0;
 for (; i < INJ_GPS_CORR_SIZE; i++ )
  p_fd->tables[i_index].inj_gps_corr[i] = MathHelpers::Round(ip_values[i] * 128.0f);
 for (; i < INJ_GPS_CORR_SIZE+2; i++ )
  p_fd->tables[i_index].inj_gps_corr[i] = MathHelpers::Round(ip_values[i] / 2.0f);
}

void CFirmwareDataMediator::GetAtscMap(int i_index,float* op_values, bool i_original /* = false */)
{
 ASSERT(op_values);

 //получаем адрес начала таблиц семейств характеристик
 fw_data_t* p_fd = (fw_data_t*)(&getBytes(i_original)[m_lip->FIRMWARE_DATA_START]);

 for (int i = 0; i < INJ_ATS_CORR_SIZE; i++ )
  op_values[i] = ((float)p_fd->tables[i_index].inj_ats_corr[i]) / 128.0f;
}

void CFirmwareDataMediator::SetAtscMap(int i_index,const float* ip_values)
{
 ASSERT(ip_values);

 //получаем адрес начала таблиц семейств характеристик
 fw_data_t* p_fd = (fw_data_t*)(&getBytes()[m_lip->FIRMWARE_DATA_START]);

 for (int i = 0; i < INJ_ATS_CORR_SIZE; i++ )
  p_fd->tables[i_index].inj_ats_corr[i] = MathHelpers::Round((ip_values[i]*128.0f));
}

void CFirmwareDataMediator::GetIACMATMap(int i_index,float* op_values, bool i_original /* = false */)
{
 ASSERT(op_values);

 //получаем адрес начала таблиц семейств характеристик
 fw_data_t* p_fd = (fw_data_t*)(&getBytes(i_original)[m_lip->FIRMWARE_DATA_START]);

 for (int i = 0; i < INJ_ATS_CORR_SIZE; i++ )
  op_values[i] = ((float)p_fd->tables[i_index].iac_mat_corr[i]) / 4.0f;
}

void CFirmwareDataMediator::SetIACMATMap(int i_index,const float* ip_values)
{
 ASSERT(ip_values);

 //получаем адрес начала таблиц семейств характеристик
 fw_data_t* p_fd = (fw_data_t*)(&getBytes()[m_lip->FIRMWARE_DATA_START]);

 for (int i = 0; i < INJ_ATS_CORR_SIZE; i++ )
  p_fd->tables[i_index].iac_mat_corr[i] = MathHelpers::Round((ip_values[i]*4.0f));
}

//////////////////////////////////////////////////////////////////////////////////////
SECU3IO::params_t* CFirmwareDataMediator::GetParamsPtr(void)
{
 return &((fw_data_t*)(&getBytes()[m_lip->FIRMWARE_DATA_START]))->def_param;
}

EECUPlatform CFirmwareDataMediator::GetPlatformId(void)
{
 return m_fpp->m_platform_id;
}
//////////////////////////////////////////////////////////////////////////////////////

//выдает все таблицы в одной структуре
void CFirmwareDataMediator::GetMapsData(FWMapsDataHolder* op_fwd)
{
 std::vector<_TSTRING> names = GetFunctionsSetNames();
 int i;
 for(i = 0; i < TABLES_NUMBER; i++)
 {
  op_fwd->maps[i].name = names[i];
  //ignition
  GetStartMap(i,op_fwd->maps[i].f_str);
  GetIdleMap(i,op_fwd->maps[i].f_idl);
  GetWorkMap(i,op_fwd->maps[i].f_wrk);
  GetTempMap(i,op_fwd->maps[i].f_tmp);
  GetTempIdlMap(i,op_fwd->maps[i].f_tmp_idl);
  //fuel injection
  GetVEMap(i,op_fwd->maps[i].inj_ve);
  GetVE2Map(i,op_fwd->maps[i].inj_ve2);
  GetAFRMap(i,op_fwd->maps[i].inj_afr);
  GetCrnkMap(i,op_fwd->maps[i].inj_cranking);
  GetWrmpMap(i,op_fwd->maps[i].inj_warmup);
  GetDeadMap(i,op_fwd->maps[i].inj_dead_time);
  GetIdlrMap(i,op_fwd->maps[i].inj_iac_run_pos);
  GetIdlcMap(i,op_fwd->maps[i].inj_iac_crank_pos);
  GetAETPSMap(i,op_fwd->maps[i].inj_ae_tps);
  GetAERPMMap(i,op_fwd->maps[i].inj_ae_rpm);
  GetAftstrMap(i,op_fwd->maps[i].inj_aftstr);  
  GetITMap(i, op_fwd->maps[i].inj_timing);
  GetITRPMMap(i, op_fwd->maps[i].inj_target_rpm);
  GetRigidMap(i, op_fwd->maps[i].inj_idl_rigidity);
  GetEGOCurveMap(i, op_fwd->maps[i].inj_ego_curve);
  GetIACCorrWMap(i, op_fwd->maps[i].inj_iac_corr_w);
  GetIACCorrMap(i, op_fwd->maps[i].inj_iac_corr);
  GetTpsswtMap(i, op_fwd->maps[i].inj_tpsswt);
  GetGtscMap(i, op_fwd->maps[i].inj_gts_corr);
  GetGpscMap(i, op_fwd->maps[i].inj_gps_corr);
  GetAtscMap(i, op_fwd->maps[i].inj_ats_corr);
  GetIATCLTMap(i, op_fwd->maps[i].inj_iatclt_corr);
  GetPwm1Map(i,op_fwd->maps[i].pwm_duty1);
  GetPwm2Map(i,op_fwd->maps[i].pwm_duty2);
  GetIACMATMap(i, op_fwd->maps[i].iac_mat_corr);
 }
 //separate tables
 GetAttenuatorMap(op_fwd->attenuator_table);
 GetDwellCntrlMap(op_fwd->dwellcntrl_table);
 GetCTSCurveMap(op_fwd->ctscurve_table);
 op_fwd->ctscurve_vlimits[0] = GetCTSMapVoltageLimit(0);
 op_fwd->ctscurve_vlimits[1] = GetCTSMapVoltageLimit(1);
 GetATSCurveMap(op_fwd->atscurve_table);
 op_fwd->atscurve_vlimits[0] = GetATSMapVoltageLimit(0);
 op_fwd->atscurve_vlimits[1] = GetATSMapVoltageLimit(1);
 GetATSAACMap(op_fwd->ats_corr_table);
 GetGasdosePosMap(op_fwd->gasdose_pos_table);   //GD
 GetBarocorrMap(op_fwd->barocorr_table);
 GetManIgntimMap(op_fwd->pa4_igntim_corr);
 GetTmp2CurveMap(op_fwd->tmp2_curve);
 GetGrtsCurveMap(op_fwd->grts_curve);
 GetCrkTempMap(op_fwd->ctscrk_corr);
 GetEHPauseMap(op_fwd->eh_pause_table);
 GetCrankingThrdMap(op_fwd->cranking_thrd);
 GetCrankingTimeMap(op_fwd->cranking_time);
 GetSmapabanThrdMap(op_fwd->smapaban_thrd);
 GetCESettingsData(op_fwd->cesd);
 GetKnockZoneMap(op_fwd->knock_zone);
 GetGrHeatDutyMap(op_fwd->grheat_duty);
 GetPwmIacUCoefMap(op_fwd->pwmiac_ucoef);
 GetAftstrStrk0Map(op_fwd->aftstr_strk0);
 GetAftstrStrk1Map(op_fwd->aftstr_strk1);
 GetGrValDelMap(op_fwd->grv_delay);
 GetFtlsCurveMap(op_fwd->ftls_curve);
 GetEgtsCurveMap(op_fwd->egts_curve);
 GetOpsCurveMap(op_fwd->ops_curve);
 GetManInjPwcMap(op_fwd->injpw_coef);

 //Копируем таблицу с сеткой оборотов (Copy table with RPM grid)
 float slots[F_RPM_SLOTS]; GetRPMGridMap(slots);
 for(i = 0; i < F_RPM_SLOTS; ++i)
  op_fwd->rpm_slots[i] = slots[i];

 //Копируем таблицу с сеткой ТОЖ (Copy table with CLT grid)
 float slots1[F_TMP_SLOTS]; GetCLTGridMap(slots1);
 for(i = 0; i < F_TMP_SLOTS; ++i)
  op_fwd->clt_slots[i] = slots1[i];

 //Copy table with load grid
 float slots2[F_LOAD_SLOTS]; GetLoadGridMap(slots2);
 for(i = 0; i < F_LOAD_SLOTS; ++i)
  op_fwd->load_slots[i] = slots2[i];
}

void CFirmwareDataMediator::SetMapsData(const FWMapsDataHolder* ip_fwd)
{
 int i;
 for(i = 0; i < TABLES_NUMBER; i++)
 {
  SetFunctionsSetName(i,ip_fwd->maps[i].name);
  //ignition
  SetStartMap(i,ip_fwd->maps[i].f_str);
  SetIdleMap(i,ip_fwd->maps[i].f_idl);
  SetWorkMap(i,ip_fwd->maps[i].f_wrk);
  SetTempMap(i,ip_fwd->maps[i].f_tmp);
  SetTempIdlMap(i,ip_fwd->maps[i].f_tmp_idl);
  //fuel injection
  SetVEMap(i,ip_fwd->maps[i].inj_ve);
  SetVE2Map(i,ip_fwd->maps[i].inj_ve2);
  SetAFRMap(i,ip_fwd->maps[i].inj_afr);
  SetCrnkMap(i,ip_fwd->maps[i].inj_cranking);
  SetWrmpMap(i,ip_fwd->maps[i].inj_warmup);
  SetDeadMap(i,ip_fwd->maps[i].inj_dead_time);
  SetIdlrMap(i,ip_fwd->maps[i].inj_iac_run_pos);
  SetIdlcMap(i,ip_fwd->maps[i].inj_iac_crank_pos);
  SetAETPSMap(i,ip_fwd->maps[i].inj_ae_tps);
  SetAERPMMap(i,ip_fwd->maps[i].inj_ae_rpm);
  SetAftstrMap(i,ip_fwd->maps[i].inj_aftstr);
  SetITMap(i, ip_fwd->maps[i].inj_timing);
  SetITRPMMap(i, ip_fwd->maps[i].inj_target_rpm);
  SetRigidMap(i, ip_fwd->maps[i].inj_idl_rigidity);
  SetEGOCurveMap(i, ip_fwd->maps[i].inj_ego_curve);
  SetIACCorrWMap(i, ip_fwd->maps[i].inj_iac_corr_w);
  SetIACCorrMap(i, ip_fwd->maps[i].inj_iac_corr);
  SetTpsswtMap(i, ip_fwd->maps[i].inj_tpsswt);
  SetGtscMap(i, ip_fwd->maps[i].inj_gts_corr);
  SetGpscMap(i, ip_fwd->maps[i].inj_gps_corr);
  SetAtscMap(i, ip_fwd->maps[i].inj_ats_corr);
  SetIATCLTMap(i, ip_fwd->maps[i].inj_iatclt_corr);
  SetPwm1Map(i,ip_fwd->maps[i].pwm_duty1);
  SetPwm2Map(i,ip_fwd->maps[i].pwm_duty2);
  SetIACMATMap(i, ip_fwd->maps[i].iac_mat_corr);
 }
 //separate tables
 SetAttenuatorMap(ip_fwd->attenuator_table);
 SetDwellCntrlMap(ip_fwd->dwellcntrl_table);
 SetCTSCurveMap(ip_fwd->ctscurve_table);
 SetCTSMapVoltageLimit(0, ip_fwd->ctscurve_vlimits[0]);
 SetCTSMapVoltageLimit(1, ip_fwd->ctscurve_vlimits[1]);
 SetATSCurveMap(ip_fwd->atscurve_table);
 SetATSMapVoltageLimit(0, ip_fwd->atscurve_vlimits[0]);
 SetATSMapVoltageLimit(1, ip_fwd->atscurve_vlimits[1]);
 SetATSAACMap(ip_fwd->ats_corr_table);
 SetGasdosePosMap(ip_fwd->gasdose_pos_table); //GD
 SetBarocorrMap(ip_fwd->barocorr_table);
 SetManIgntimMap(ip_fwd->pa4_igntim_corr);
 SetTmp2CurveMap(ip_fwd->tmp2_curve);
 SetGrtsCurveMap(ip_fwd->grts_curve);
 SetCrkTempMap(ip_fwd->ctscrk_corr);
 SetEHPauseMap(ip_fwd->eh_pause_table);
 SetCrankingThrdMap(ip_fwd->cranking_thrd);
 SetCrankingTimeMap(ip_fwd->cranking_time);
 SetSmapabanThrdMap(ip_fwd->smapaban_thrd);
 SetCESettingsData(ip_fwd->cesd);
 SetKnockZoneMap(ip_fwd->knock_zone);
 SetGrHeatDutyMap(ip_fwd->grheat_duty);
 SetPwmIacUCoefMap(ip_fwd->pwmiac_ucoef);
 SetAftstrStrk0Map(ip_fwd->aftstr_strk0);
 SetAftstrStrk1Map(ip_fwd->aftstr_strk1);
 SetGrValDelMap(ip_fwd->grv_delay);
 SetFtlsCurveMap(ip_fwd->ftls_curve);
 SetEgtsCurveMap(ip_fwd->egts_curve);
 SetOpsCurveMap(ip_fwd->ops_curve);
 SetManInjPwcMap(ip_fwd->injpw_coef);

 //Check RPM grids compatibility and set RPM grid
 if (CheckRPMGridsCompatibility(ip_fwd->rpm_slots))
  SetRPMGridMap(ip_fwd->rpm_slots);
 //Check CLT grids compatibility and set CLT grid
 if (CheckCLTGridsCompatibility(ip_fwd->clt_slots))
  SetCLTGridMap(ip_fwd->clt_slots);
 //Check load grids compatibility and set load grid
 if (CheckLoadGridsCompatibility(ip_fwd->load_slots))
  SetLoadGridMap(ip_fwd->load_slots);
}

bool CFirmwareDataMediator::CheckRPMGridsCompatibility(const float* rpmGrid)
{
 bool match = true;
 float slots[F_RPM_SLOTS]; GetRPMGridMap(slots);
 for(int i = 0; i < F_RPM_SLOTS; ++i)
  if (rpmGrid[i] != slots[i])
   match = false;
 if (!match)
  return (IDYES==AfxMessageBox(_T("RPM grids from firmware and source are not equal!\n Please inspect idle, work, VE, AFR, inj.timing, PWM1/split angle and PWM2 maps if you accept new RPM grid.\n Accept new RPM grid (Y) or keep old one (N)?"), MB_YESNO|MB_ICONWARNING));
 return true;
}

bool CFirmwareDataMediator::CheckCLTGridsCompatibility(const float* cltGrid)
{
 bool match = true;
 float slots[F_TMP_SLOTS]; GetCLTGridMap(slots);
 for(int i = 0; i < F_TMP_SLOTS; ++i)
  if (cltGrid[i] != slots[i])
   match = false;
 if (!match)
  return (IDYES==AfxMessageBox(_T("CLT grids from firmware and source are not equal!\n Please inspect corresponding maps if you accept new CLT grid.\n Accept new CLT grid (Y) or keep old one (N)?"), MB_YESNO|MB_ICONWARNING));
 return true;
}

bool CFirmwareDataMediator::CheckLoadGridsCompatibility(const float* lodGrid)
{
 bool match = true;
 float slots[F_LOAD_SLOTS]; GetLoadGridMap(slots);
 for(int i = 0; i < F_LOAD_SLOTS; ++i)
  if (lodGrid[i] != slots[i])
   match = false;
 if (!match)
  return (IDYES==AfxMessageBox(_T("Load grids from firmware and source are not equal!\n Please inspect work, VE, AFR, inj.timing, PWM1/split angle and PWM2 maps if you accept new load grid.\n Accept new load grid (Y) or keep old one (N)?"), MB_YESNO|MB_ICONWARNING));
 return true;
}

void CFirmwareDataMediator::GetAttenuatorMap(float* op_values, bool i_original /* = false */)
{
 ASSERT(op_values);
 if (!op_values)
  return;

 //получаем адрес структуры дополнительных данных
 fw_data_t* p_fd = (fw_data_t*)(&getBytes(i_original)[m_lip->FIRMWARE_DATA_START]);

 for(size_t i = 0; i < KC_ATTENUATOR_LOOKUP_TABLE_SIZE; i++)
  op_values[i] = p_fd->exdata.attenuator_table[i];
}

void CFirmwareDataMediator::SetAttenuatorMap(const float* ip_values)
{
 ASSERT(ip_values);
 if (!ip_values)
  return;

 //получаем адрес структуры дополнительных данных
 fw_data_t* p_fd = (fw_data_t*)(&getBytes()[m_lip->FIRMWARE_DATA_START]);

 for(size_t i = 0; i < KC_ATTENUATOR_LOOKUP_TABLE_SIZE; i++)
 {
  ASSERT(ip_values[i] >= 0.0f);
  p_fd->exdata.attenuator_table[i] = (_uchar)MathHelpers::Round(ip_values[i]);
 }
}

void CFirmwareDataMediator::GetDwellCntrlMap(float* op_values, bool i_original /* = false */)
{
 ASSERT(op_values);
 if (!op_values)
  return;

 //получаем адрес структуры дополнительных данных
 fw_data_t* p_fd = (fw_data_t*)(&getBytes(i_original)[m_lip->FIRMWARE_DATA_START]);

 float discrete = PlatformParamHolder::GetQuartzFact(m_fpp->m_platform_id); //for ATMega644 discrete = 3.2uS, for others - 4.0uS
 for(size_t i = 0; i < COIL_ON_TIME_LOOKUP_TABLE_SIZE; i++)
  op_values[i] = (p_fd->exdata.coil_on_time[i] * discrete) / 1000.0f; //convert to ms
}

void CFirmwareDataMediator::SetDwellCntrlMap(const float* ip_values)
{
 ASSERT(ip_values);
 if (!ip_values)
  return;

 //получаем адрес структуры дополнительных данных
 fw_data_t* p_fd = (fw_data_t*)(&getBytes()[m_lip->FIRMWARE_DATA_START]);

 float discrete = PlatformParamHolder::GetQuartzFact(m_fpp->m_platform_id); //for ATMega644 discrete = 3.2uS, for others - 4.0uS
 for(size_t i = 0; i < COIL_ON_TIME_LOOKUP_TABLE_SIZE; i++)
  p_fd->exdata.coil_on_time[i] = (_uint)MathHelpers::Round((ip_values[i] * 1000.0) / discrete);
}

void CFirmwareDataMediator::GetCTSCurveMap(float* op_values, bool i_original /* = false */)
{
 ASSERT(op_values);
 if (!op_values)
  return;

 //получаем адрес структуры дополнительных данных
 fw_data_t* p_fd = (fw_data_t*)(&getBytes(i_original)[m_lip->FIRMWARE_DATA_START]);

 for(size_t i = 0; i < THERMISTOR_LOOKUP_TABLE_SIZE; i++)
  op_values[i] = (p_fd->exdata.cts_curve[i] / 4.0f);
}

void CFirmwareDataMediator::SetCTSCurveMap(const float* ip_values)
{
 ASSERT(ip_values);
 if (!ip_values)
  return;

 //получаем адрес структуры дополнительных данных
 fw_data_t* p_fd = (fw_data_t*)(&getBytes()[m_lip->FIRMWARE_DATA_START]);

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
 ASSERT(op_values);
 if (!op_values)
  return;

 //получаем адрес структуры дополнительных данных
 fw_data_t* p_fd = (fw_data_t*)(&getBytes(i_original)[m_lip->FIRMWARE_DATA_START]);

 for(size_t i = 0; i < THERMISTOR_LOOKUP_TABLE_SIZE; i++)
  op_values[i] = (p_fd->exdata.ats_curve[i] / 4.0f);
}

void CFirmwareDataMediator::SetATSCurveMap(const float* ip_values)
{
 ASSERT(ip_values);
 if (!ip_values)
  return;

 //получаем адрес структуры дополнительных данных
 fw_data_t* p_fd = (fw_data_t*)(&getBytes()[m_lip->FIRMWARE_DATA_START]);

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

 for(size_t i = 0; i < F_RPM_SLOTS; i++)
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
 for(size_t i = 0; i < F_RPM_SLOTS; i++)
  p_fd->exdata.rpm_grid_points[i] = MathHelpers::Round(ip_values[i]);

 //calculate sizes
 for(size_t i = 0; i < F_RPM_SLOTS-1; i++)
  p_fd->exdata.rpm_grid_sizes[i] = p_fd->exdata.rpm_grid_points[i+1] - p_fd->exdata.rpm_grid_points[i];
}

void CFirmwareDataMediator::GetCLTGridMap(float* op_values)
{
 ASSERT(op_values);
 if (!op_values)
  return;

 //получаем адрес структуры дополнительных данных
 fw_data_t* p_fd = (fw_data_t*)(&mp_bytes_active[m_lip->FIRMWARE_DATA_START]);

 for(size_t i = 0; i < F_TMP_SLOTS; i++)
  op_values[i] = ((float)p_fd->exdata.clt_grid_points[i]) / TEMP_PHYSICAL_MAGNITUDE_MULTIPLIER;
}

void CFirmwareDataMediator::SetCLTGridMap(const float* ip_values)
{
 ASSERT(ip_values);
 if (!ip_values)
  return;

 //получаем адрес структуры дополнительных данных
 fw_data_t* p_fd = (fw_data_t*)(&mp_bytes_active[m_lip->FIRMWARE_DATA_START]);

 //store grid points
 for(size_t i = 0; i < F_TMP_SLOTS; i++)
  p_fd->exdata.clt_grid_points[i] = MathHelpers::Round(ip_values[i] * TEMP_PHYSICAL_MAGNITUDE_MULTIPLIER);

 //calculate sizes
 for(size_t i = 0; i < F_TMP_SLOTS-1; i++)
  p_fd->exdata.clt_grid_sizes[i] = p_fd->exdata.clt_grid_points[i+1] - p_fd->exdata.clt_grid_points[i];
}

void CFirmwareDataMediator::GetLoadGridMap(float* op_values)
{
 ASSERT(op_values);
 if (!op_values)
  return;

 //получаем адрес структуры дополнительных данных
 fw_data_t* p_fd = (fw_data_t*)(&mp_bytes_active[m_lip->FIRMWARE_DATA_START]);

 for(size_t i = 0; i < F_LOAD_SLOTS; i++)
  op_values[i] = ((float)p_fd->exdata.load_grid_points[i]) / LOAD_PHYSICAL_MAGNITUDE_MULTIPLIER;
}

void CFirmwareDataMediator::SetLoadGridMap(const float* ip_values)
{
 ASSERT(ip_values);
 if (!ip_values)
  return;

 //получаем адрес структуры дополнительных данных
 fw_data_t* p_fd = (fw_data_t*)(&mp_bytes_active[m_lip->FIRMWARE_DATA_START]);

 //store grid points
 for(size_t i = 0; i < F_LOAD_SLOTS; i++)
  p_fd->exdata.load_grid_points[i] = MathHelpers::Round(ip_values[i] * LOAD_PHYSICAL_MAGNITUDE_MULTIPLIER);

 //calculate sizes (note: reverse order)
 for(size_t i = 0; i < F_LOAD_SLOTS-1; i++)
  p_fd->exdata.load_grid_sizes[i] = p_fd->exdata.load_grid_points[i] - p_fd->exdata.load_grid_points[i+1];
}

void CFirmwareDataMediator::GetATSAACMap(float* op_values, bool i_original /* = false */)
{
 ASSERT(op_values);

 //получаем адрес начала таблиц семейств характеристик
 fw_data_t* p_fd = (fw_data_t*)(&getBytes(i_original)[m_lip->FIRMWARE_DATA_START]);

 for (int i = 0; i < ATS_CORR_LOOKUP_TABLE_SIZE; i++ )
  op_values[i] = ((float)p_fd->exdata.ats_corr[i]) / AA_MAPS_M_FACTOR;
}

void CFirmwareDataMediator::SetATSAACMap(const float* ip_values)
{
 ASSERT(ip_values);

 //получаем адрес начала таблиц семейств характеристик
 fw_data_t* p_fd = (fw_data_t*)(&getBytes()[m_lip->FIRMWARE_DATA_START]);

 for (int i = 0; i < ATS_CORR_LOOKUP_TABLE_SIZE; i++ )
  p_fd->exdata.ats_corr[i] = MathHelpers::Round((ip_values[i]*AA_MAPS_M_FACTOR));
}

void CFirmwareDataMediator::GetGasdosePosMap(float* op_values, bool i_original /*= false*/)
{
 ASSERT(op_values);

 //получаем адрес начала таблиц семейств характеристик
 fw_data_t* p_fd = (fw_data_t*)(&getBytes(i_original)[m_lip->FIRMWARE_DATA_START]);

 for (int i = 0; i < (GASDOSE_POS_TPS_SIZE * GASDOSE_POS_RPM_SIZE); i++ )
 {
  _uchar *p = &(p_fd->exdata.gasdose_pos[0][0]);
  op_values[i] = ((float) *(p + i)) / GD_MAPS_M_FACTOR;
 }
}

void CFirmwareDataMediator::SetGasdosePosMap(const float* ip_values)
{
 ASSERT(ip_values);

 //получаем адрес начала таблиц семейств характеристик
 fw_data_t* p_fd = (fw_data_t*)(&getBytes()[m_lip->FIRMWARE_DATA_START]);

 for (int i = 0; i < (GASDOSE_POS_TPS_SIZE * GASDOSE_POS_RPM_SIZE); i++ )
 {
  _uchar *p = &(p_fd->exdata.gasdose_pos[0][0]);
  *(p + i) = MathHelpers::Round((ip_values[i] * GD_MAPS_M_FACTOR));
 }
}

void CFirmwareDataMediator::GetBarocorrMap(float* op_values, bool i_original /*= false*/)
{
 ASSERT(op_values);

 //gets address of the sets of maps
 fw_data_t* p_fd = (fw_data_t*)(&getBytes(i_original)[m_lip->FIRMWARE_DATA_START]);

 int i = 0;
 for (; i < BAROCORR_SIZE; i++ )
 {
  float value = (float)p_fd->exdata.barocorr[i];
  op_values[i] = (value / BAROCORR_MAPS_M_FACTOR) * 100.0f; // x 100%
 }

 for (; i < BAROCORR_SIZE+2; i++ )
 {
  float value = (float)p_fd->exdata.barocorr[i];
  op_values[i] = (value) / BAROCORR_MAPSX_M_FACTOR;
 } 
}

void CFirmwareDataMediator::SetBarocorrMap(const float* ip_values)
{
 ASSERT(ip_values);

 //gets address of the sets of maps
 fw_data_t* p_fd = (fw_data_t*)(&getBytes()[m_lip->FIRMWARE_DATA_START]);

 int i = 0;
 for (; i < BAROCORR_SIZE; i++ )
  p_fd->exdata.barocorr[i] = MathHelpers::Round((ip_values[i] / 100.0f) * BAROCORR_MAPS_M_FACTOR); // divide by 100%
 for (; i < BAROCORR_SIZE+2; i++ )
  p_fd->exdata.barocorr[i] = MathHelpers::Round(ip_values[i] * BAROCORR_MAPSX_M_FACTOR);
}

void CFirmwareDataMediator::GetManIgntimMap(float* op_values, bool i_original /* = false */)
{
 ASSERT(op_values);

 //получаем адрес начала таблиц семейств характеристик
 fw_data_t* p_fd = (fw_data_t*)(&getBytes(i_original)[m_lip->FIRMWARE_DATA_START]);

 for (int i = 0; i < PA4_LOOKUP_TABLE_SIZE; i++ )
  op_values[i] = ((float)p_fd->exdata.pa4_igntim_corr[i]) / AA_MAPS_M_FACTOR;
}

void CFirmwareDataMediator::SetManIgntimMap(const float* ip_values)
{
 ASSERT(ip_values);

 //получаем адрес начала таблиц семейств характеристик
 fw_data_t* p_fd = (fw_data_t*)(&getBytes()[m_lip->FIRMWARE_DATA_START]);

 for (int i = 0; i < PA4_LOOKUP_TABLE_SIZE; i++ )
  p_fd->exdata.pa4_igntim_corr[i] = MathHelpers::Round((ip_values[i]*AA_MAPS_M_FACTOR));
}

void CFirmwareDataMediator::GetTmp2CurveMap(float* op_values, bool i_original /*= false*/)
{
 ASSERT(op_values);

 //gets address of the sets of maps
 fw_data_t* p_fd = (fw_data_t*)(&getBytes(i_original)[m_lip->FIRMWARE_DATA_START]);

 int i = 0;
 for (; i < THERMISTOR_LOOKUP_TABLE_SIZE; i++ )
  op_values[i] = ((float)p_fd->exdata.tmp2_curve[i]) / 4.0f;

 for (; i < THERMISTOR_LOOKUP_TABLE_SIZE+2; i++ )
  op_values[i] = ((float)p_fd->exdata.tmp2_curve[i]) * ADC_DISCRETE;
}

void CFirmwareDataMediator::SetTmp2CurveMap(const float* ip_values)
{
 ASSERT(ip_values);

 //gets address of the sets of maps
 fw_data_t* p_fd = (fw_data_t*)(&getBytes()[m_lip->FIRMWARE_DATA_START]);

 int i = 0;
 for (; i < THERMISTOR_LOOKUP_TABLE_SIZE; i++ )
  p_fd->exdata.tmp2_curve[i] = MathHelpers::Round(ip_values[i] * 4.0f);
 for (; i < THERMISTOR_LOOKUP_TABLE_SIZE+2; i++ )
  p_fd->exdata.tmp2_curve[i] = MathHelpers::Round(ip_values[i] / ADC_DISCRETE);
}

void CFirmwareDataMediator::GetGrtsCurveMap(float* op_values, bool i_original /*= false*/)
{
 ASSERT(op_values);

 //gets address of the sets of maps
 fw_data_t* p_fd = (fw_data_t*)(&getBytes(i_original)[m_lip->FIRMWARE_DATA_START]);

 int i = 0;
 for (; i < THERMISTOR_LOOKUP_TABLE_SIZE; i++ )
  op_values[i] = ((float)p_fd->exdata.grts_curve[i]) / 4.0f;

 for (; i < THERMISTOR_LOOKUP_TABLE_SIZE+2; i++ )
  op_values[i] = ((float)p_fd->exdata.grts_curve[i]) * ADC_DISCRETE;
}

void CFirmwareDataMediator::SetGrtsCurveMap(const float* ip_values)
{
 ASSERT(ip_values);

 //gets address of the sets of maps
 fw_data_t* p_fd = (fw_data_t*)(&getBytes()[m_lip->FIRMWARE_DATA_START]);

 int i = 0;
 for (; i < THERMISTOR_LOOKUP_TABLE_SIZE; i++ )
  p_fd->exdata.grts_curve[i] = MathHelpers::Round(ip_values[i] * 4.0f);
 for (; i < THERMISTOR_LOOKUP_TABLE_SIZE+2; i++ )
  p_fd->exdata.grts_curve[i] = MathHelpers::Round(ip_values[i] / ADC_DISCRETE);
}

void CFirmwareDataMediator::GetCrkTempMap(float* op_values, bool i_original /* = false */)
{
 ASSERT(op_values);

 //получаем адрес структуры дополнительных данных
 fw_data_t* p_fd = (fw_data_t*)(&getBytes(i_original)[m_lip->FIRMWARE_DATA_START]);

 for(size_t i = 0; i < CTS_CRKCORR_SIZE; i++)
  op_values[i] = (p_fd->exdata.cts_crkcorr[i] / AA_MAPS_M_FACTOR);
}

void CFirmwareDataMediator::SetCrkTempMap(const float* ip_values)
{
 ASSERT(ip_values);

 //получаем адрес структуры дополнительных данных
 fw_data_t* p_fd = (fw_data_t*)(&getBytes()[m_lip->FIRMWARE_DATA_START]);

 for(size_t i = 0; i < CTS_CRKCORR_SIZE; i++)
  p_fd->exdata.cts_crkcorr[i] = MathHelpers::Round(ip_values[i] * AA_MAPS_M_FACTOR);
}

void CFirmwareDataMediator::GetEHPauseMap(float* op_values, bool i_original /* = false */)
{
 ASSERT(op_values);
 if (!op_values)
  return;

 //получаем адрес структуры дополнительных данных
 fw_data_t* p_fd = (fw_data_t*)(&getBytes(i_original)[m_lip->FIRMWARE_DATA_START]);

 for(size_t i = 0; i < COIL_ON_TIME_LOOKUP_TABLE_SIZE; i++)
  op_values[i] = p_fd->exdata.eh_pause[i] / 100.0f; //convert to seconds
}

void CFirmwareDataMediator::SetEHPauseMap(const float* ip_values)
{
 ASSERT(ip_values);
 if (!ip_values)
  return;

 //получаем адрес структуры дополнительных данных
 fw_data_t* p_fd = (fw_data_t*)(&getBytes()[m_lip->FIRMWARE_DATA_START]);

 for(size_t i = 0; i < COIL_ON_TIME_LOOKUP_TABLE_SIZE; i++)
  p_fd->exdata.eh_pause[i] = (_uchar)MathHelpers::Round(ip_values[i] * 100.0f);
}

void CFirmwareDataMediator::GetCrankingThrdMap(float* op_values, bool i_original /* = false */)
{
 ASSERT(op_values);
 fw_data_t* p_fd = (fw_data_t*)(&getBytes(i_original)[m_lip->FIRMWARE_DATA_START]);

 for(size_t i = 0; i < CRANK_THRD_SIZE; i++)
  op_values[i] = (p_fd->exdata.cranking_thrd[i] * 10.0f);
}

void CFirmwareDataMediator::SetCrankingThrdMap(const float* ip_values)
{
 ASSERT(ip_values);
 fw_data_t* p_fd = (fw_data_t*)(&getBytes()[m_lip->FIRMWARE_DATA_START]);

 for(size_t i = 0; i < CRANK_THRD_SIZE; i++)
  p_fd->exdata.cranking_thrd[i] = MathHelpers::Round(ip_values[i] / 10.0f);
}

void CFirmwareDataMediator::GetCrankingTimeMap(float* op_values, bool i_original /* = false */)
{
 ASSERT(op_values);
 fw_data_t* p_fd = (fw_data_t*)(&getBytes(i_original)[m_lip->FIRMWARE_DATA_START]);

 for(size_t i = 0; i < CRANK_TIME_SIZE; i++)
  op_values[i] = (p_fd->exdata.cranking_time[i]);
}

void CFirmwareDataMediator::SetCrankingTimeMap(const float* ip_values)
{
 ASSERT(ip_values);
 fw_data_t* p_fd = (fw_data_t*)(&getBytes()[m_lip->FIRMWARE_DATA_START]);

 for(size_t i = 0; i < CRANK_TIME_SIZE; i++)
  p_fd->exdata.cranking_time[i] = MathHelpers::Round(ip_values[i]);
}

void CFirmwareDataMediator::GetSmapabanThrdMap(float* op_values, bool i_original /* = false */)
{
 ASSERT(op_values);
 fw_data_t* p_fd = (fw_data_t*)(&getBytes(i_original)[m_lip->FIRMWARE_DATA_START]);

 for(size_t i = 0; i < SMAPABAN_THRD_SIZE; i++)
  op_values[i] = (p_fd->exdata.smapaban_thrd[i] * 10.0f);
}

void CFirmwareDataMediator::SetSmapabanThrdMap(const float* ip_values)
{
 ASSERT(ip_values);
 fw_data_t* p_fd = (fw_data_t*)(&getBytes()[m_lip->FIRMWARE_DATA_START]);

 for(size_t i = 0; i < SMAPABAN_THRD_SIZE; i++)
  p_fd->exdata.smapaban_thrd[i] = MathHelpers::Round(ip_values[i] / 10.0f);
}

void CFirmwareDataMediator::GetPwm1Map(int i_index, float* op_values, bool i_original /* = false*/)
{
 ASSERT(op_values);
 fw_data_t* p_fd = (fw_data_t*)(&getBytes(i_original)[m_lip->FIRMWARE_DATA_START]);

 bool splitAng = CHECKBIT32(GetFWOptions(), COPT_SPLIT_ANGLE);
 for (int i = 0; i < (F_WRK_POINTS_F * F_WRK_POINTS_L); i++ )
 {
  _uchar *p = &(p_fd->tables[i_index].pwm_duty1[0][0]);
  //we interpret data depending on firmware option (split angle or PWM duty)
  op_values[i] = splitAng ? ((float) *(p + i)) / AA_MAPS_M_FACTOR : (((float) *(p + i)) * 100.0f) / 255.0f;
 }
}

void CFirmwareDataMediator::SetPwm1Map(int i_index, const float* ip_values)
{
 ASSERT(ip_values);
 fw_data_t* p_fd = (fw_data_t*)(&getBytes()[m_lip->FIRMWARE_DATA_START]);

 bool splitAng = CHECKBIT32(GetFWOptions(), COPT_SPLIT_ANGLE);
 for (int i = 0; i < (F_WRK_POINTS_F * F_WRK_POINTS_L); i++ )
 {
  _uchar *p = &(p_fd->tables[i_index].pwm_duty1[0][0]);
  //we interpret data depending on firmware option (split angle or PWM duty)
  *(p + i) = splitAng ? MathHelpers::Round((ip_values[i]*AA_MAPS_M_FACTOR)) : MathHelpers::Round((ip_values[i]*255.0f)/100.0f);
 }
}

void CFirmwareDataMediator::GetPwm2Map(int i_index, float* op_values, bool i_original /* = false*/)
{
 ASSERT(op_values);
 fw_data_t* p_fd = (fw_data_t*)(&getBytes(i_original)[m_lip->FIRMWARE_DATA_START]);

 for (int i = 0; i < (F_WRK_POINTS_F * F_WRK_POINTS_L); i++ )
 {
  _uchar *p = &(p_fd->tables[i_index].pwm_duty2[0][0]);
  op_values[i] = (((float) *(p + i)) * 100.0f) / 255.0f;
 }
}

void CFirmwareDataMediator::SetPwm2Map(int i_index, const float* ip_values)
{
 ASSERT(ip_values);
 fw_data_t* p_fd = (fw_data_t*)(&getBytes()[m_lip->FIRMWARE_DATA_START]);

 for (int i = 0; i < (F_WRK_POINTS_F * F_WRK_POINTS_L); i++ )
 {
  _uchar *p = &(p_fd->tables[i_index].pwm_duty2[0][0]);
  *(p + i) = MathHelpers::Round((ip_values[i]*255.0f)/100.0f);
 }
}

void CFirmwareDataMediator::GetKnockZoneMap(float* op_values, bool i_original /*= false*/)
{
 ASSERT(op_values);

 //получаем адрес начала таблиц семейств характеристик
 fw_data_t* p_fd = (fw_data_t*)(&getBytes(i_original)[m_lip->FIRMWARE_DATA_START]);

 for (int i = 0; i < F_WRK_POINTS_L; i++)
  for (int b = 0; b < F_WRK_POINTS_F; b++)
   op_values[(i*F_WRK_POINTS_F)+b] = (float)CHECKBIT16(p_fd->exdata.knock_zone[i], b);
}

void CFirmwareDataMediator::SetKnockZoneMap(const float* ip_values)
{
 ASSERT(ip_values);

 //получаем адрес начала таблиц семейств характеристик
 fw_data_t* p_fd = (fw_data_t*)(&getBytes()[m_lip->FIRMWARE_DATA_START]);

 for (int i = 0; i < F_WRK_POINTS_L; i++)
  for (int b = 0; b < F_WRK_POINTS_F; b++)   
   WRITEBIT16(p_fd->exdata.knock_zone[i], b, (ip_values[(i*F_WRK_POINTS_F)+b] > 0.5));
}

void CFirmwareDataMediator::GetGrHeatDutyMap(float* op_values, bool i_original /* = false */)
{
 ASSERT(op_values);
 if (!op_values)
  return;

 //получаем адрес структуры дополнительных данных
 fw_data_t* p_fd = (fw_data_t*)(&getBytes(i_original)[m_lip->FIRMWARE_DATA_START]);

 for(size_t i = 0; i < F_TMP_POINTS; i++)
  op_values[i] = (p_fd->exdata.grheat_duty[i] / 2.0f);
}

void CFirmwareDataMediator::SetGrHeatDutyMap(const float* ip_values)
{
 ASSERT(ip_values);
 if (!ip_values)
  return;

 //получаем адрес структуры дополнительных данных
 fw_data_t* p_fd = (fw_data_t*)(&getBytes()[m_lip->FIRMWARE_DATA_START]);

 for(size_t i = 0; i < F_TMP_POINTS; i++)
  p_fd->exdata.grheat_duty[i] = (_uchar)MathHelpers::Round(ip_values[i] * 2.0);
}

void CFirmwareDataMediator::GetPwmIacUCoefMap(float* op_values, bool i_original /*= false*/)
{
 ASSERT(op_values);
 //gets address of the sets of maps
 fw_data_t* p_fd = (fw_data_t*)(&getBytes(i_original)[m_lip->FIRMWARE_DATA_START]); 
 for (int i = 0; i < PWMIAC_UCOEF_SIZE; i++ )
  op_values[i] = ((float)p_fd->exdata.pwmiac_ucoef[i]) / PWMIAC_UCOEF_MAPS_M_FACTOR;
}

void CFirmwareDataMediator::SetPwmIacUCoefMap(const float* ip_values)
{
 ASSERT(ip_values);
 //gets address of the sets of maps
 fw_data_t* p_fd = (fw_data_t*)(&getBytes()[m_lip->FIRMWARE_DATA_START]);
 for (int i = 0; i < PWMIAC_UCOEF_SIZE; i++ )
  p_fd->exdata.pwmiac_ucoef[i] = MathHelpers::Round(ip_values[i] * PWMIAC_UCOEF_MAPS_M_FACTOR);
}

void CFirmwareDataMediator::GetAftstrStrk0Map(float* op_values, bool i_original /*= false*/)
{
 ASSERT(op_values);
 fw_data_t* p_fd = (fw_data_t*)(&getBytes(i_original)[m_lip->FIRMWARE_DATA_START]);

 for(size_t i = 0; i < AFTSTR_STRK_SIZE; i++)
  op_values[i] = p_fd->exdata.inj_aftstr_strk0[i];
}

void CFirmwareDataMediator::SetAftstrStrk0Map(const float* ip_values)
{
 ASSERT(ip_values);
 fw_data_t* p_fd = (fw_data_t*)(&getBytes()[m_lip->FIRMWARE_DATA_START]);

 for(size_t i = 0; i < AFTSTR_STRK_SIZE; i++)
  p_fd->exdata.inj_aftstr_strk0[i] = MathHelpers::Round(ip_values[i]);
}

void CFirmwareDataMediator::GetAftstrStrk1Map(float* op_values, bool i_original /*= false*/)
{
 ASSERT(op_values);
 fw_data_t* p_fd = (fw_data_t*)(&getBytes(i_original)[m_lip->FIRMWARE_DATA_START]);

 for(size_t i = 0; i < AFTSTR_STRK_SIZE; i++)
  op_values[i] = p_fd->exdata.inj_aftstr_strk1[i];
}

void CFirmwareDataMediator::SetAftstrStrk1Map(const float* ip_values)
{
 ASSERT(ip_values);
 fw_data_t* p_fd = (fw_data_t*)(&getBytes()[m_lip->FIRMWARE_DATA_START]);

 for(size_t i = 0; i < AFTSTR_STRK_SIZE; i++)
  p_fd->exdata.inj_aftstr_strk1[i] = MathHelpers::Round(ip_values[i]);
}

void CFirmwareDataMediator::GetGrValDelMap(float* op_values, bool i_original /*= false*/)
{
 ASSERT(op_values);
 fw_data_t* p_fd = (fw_data_t*)(&getBytes(i_original)[m_lip->FIRMWARE_DATA_START]);

 for(size_t i = 0; i < F_TMP_POINTS; i++)
  op_values[i] = ((float)p_fd->exdata.grv_delay[i]) / 100.0f;
}

void CFirmwareDataMediator::SetGrValDelMap(const float* ip_values)
{
 ASSERT(ip_values);
 fw_data_t* p_fd = (fw_data_t*)(&getBytes()[m_lip->FIRMWARE_DATA_START]);

 for(size_t i = 0; i < F_TMP_POINTS; i++)
  p_fd->exdata.grv_delay[i] = MathHelpers::Round(ip_values[i]*100.0f);
}

void CFirmwareDataMediator::GetFtlsCurveMap(float* op_values, bool i_original /*= false*/)
{
 ASSERT(op_values);

 //gets address of the sets of maps
 fw_data_t* p_fd = (fw_data_t*)(&getBytes(i_original)[m_lip->FIRMWARE_DATA_START]);

 int i = 0;
 for (; i < FTLS_LOOKUP_TABLE_SIZE; i++ )
  op_values[i] = ((float)p_fd->exdata.ftls_curve[i]) / 64.0f;

 for (; i < FTLS_LOOKUP_TABLE_SIZE+2; i++ )
  op_values[i] = ((float)p_fd->exdata.ftls_curve[i]) * ADC_DISCRETE;
}

void CFirmwareDataMediator::SetFtlsCurveMap(const float* ip_values)
{
 ASSERT(ip_values);

 //gets address of the sets of maps
 fw_data_t* p_fd = (fw_data_t*)(&getBytes()[m_lip->FIRMWARE_DATA_START]);

 int i = 0;
 for (; i < FTLS_LOOKUP_TABLE_SIZE; i++ )
  p_fd->exdata.ftls_curve[i] = MathHelpers::Round(ip_values[i] * 64.0f);
 for (; i < FTLS_LOOKUP_TABLE_SIZE+2; i++ )
  p_fd->exdata.ftls_curve[i] = MathHelpers::Round(ip_values[i] / ADC_DISCRETE);
}

void CFirmwareDataMediator::GetEgtsCurveMap(float* op_values, bool i_original /*= false*/)
{
 ASSERT(op_values);

 //gets address of the sets of maps
 fw_data_t* p_fd = (fw_data_t*)(&getBytes(i_original)[m_lip->FIRMWARE_DATA_START]);

 int i = 0;
 for (; i < EGTS_LOOKUP_TABLE_SIZE; i++ )
  op_values[i] = ((float)p_fd->exdata.egts_curve[i]) / 4.0f;

 for (; i < EGTS_LOOKUP_TABLE_SIZE+2; i++ )
  op_values[i] = ((float)p_fd->exdata.egts_curve[i]) * ADC_DISCRETE;
}

void CFirmwareDataMediator::SetEgtsCurveMap(const float* ip_values)
{
 ASSERT(ip_values);

 //gets address of the sets of maps
 fw_data_t* p_fd = (fw_data_t*)(&getBytes()[m_lip->FIRMWARE_DATA_START]);

 int i = 0;
 for (; i < EGTS_LOOKUP_TABLE_SIZE; i++ )
  p_fd->exdata.egts_curve[i] = MathHelpers::Round(ip_values[i] * 4.0f);
 for (; i < EGTS_LOOKUP_TABLE_SIZE+2; i++ )
  p_fd->exdata.egts_curve[i] = MathHelpers::Round(ip_values[i] / ADC_DISCRETE);
}

void CFirmwareDataMediator::GetOpsCurveMap(float* op_values, bool i_original /*= false*/)
{
 ASSERT(op_values);

 //gets address of the sets of maps
 fw_data_t* p_fd = (fw_data_t*)(&getBytes(i_original)[m_lip->FIRMWARE_DATA_START]);

 int i = 0;
 for (; i < OPS_LOOKUP_TABLE_SIZE; i++ )
  op_values[i] = ((float)p_fd->exdata.ops_curve[i]) / 256.0f;

 for (; i < OPS_LOOKUP_TABLE_SIZE+2; i++ )
  op_values[i] = ((float)p_fd->exdata.ops_curve[i]) * ADC_DISCRETE;
}

void CFirmwareDataMediator::SetOpsCurveMap(const float* ip_values)
{
 ASSERT(ip_values);

 //gets address of the sets of maps
 fw_data_t* p_fd = (fw_data_t*)(&getBytes()[m_lip->FIRMWARE_DATA_START]);

 int i = 0;
 for (; i < OPS_LOOKUP_TABLE_SIZE; i++ )
  p_fd->exdata.ops_curve[i] = MathHelpers::Round(ip_values[i] * 256.0f);
 for (; i < OPS_LOOKUP_TABLE_SIZE+2; i++ )
  p_fd->exdata.ops_curve[i] = MathHelpers::Round(ip_values[i] / ADC_DISCRETE);
}

void CFirmwareDataMediator::GetManInjPwcMap(float* op_values, bool i_original /* = false */)
{
 ASSERT(op_values);

 //get address of the beginning of set of maps
 fw_data_t* p_fd = (fw_data_t*)(&getBytes(i_original)[m_lip->FIRMWARE_DATA_START]);

 for (int i = 0; i < INJPWCOEF_LUT_SIZE; i++ )
  op_values[i] = ((float)p_fd->exdata.injpw_coef[i]) / INJPWCOEF_MULT;
}

void CFirmwareDataMediator::SetManInjPwcMap(const float* ip_values)
{
 ASSERT(ip_values);

 //get address of the beginning of set of maps
 fw_data_t* p_fd = (fw_data_t*)(&getBytes()[m_lip->FIRMWARE_DATA_START]);

 for (int i = 0; i < INJPWCOEF_LUT_SIZE; i++ )
  p_fd->exdata.injpw_coef[i] = MathHelpers::Round((ip_values[i]*INJPWCOEF_MULT));
}

//--------------------------------------------------------------------------------
DWORD CFirmwareDataMediator::GetIOPlug(IOXtype type, IOPid id)
{
 if (!mp_cddata)
  return 0;
 DWORD value = 0;
 bool secu3t = CHECKBIT32(GetFWOptions(), SECU3IO::COPT_SECU3T);
 switch(type)
 {
  case IOX_INIT:
   if (id < (secu3t ? IOP_COUNT : IOP3I_COUNT))
    value = mp_cddata->iorem.i_plugs[id]; 
   break;
  case IOX_DATA:
   if (id < (secu3t ? IOP_COUNT : IOP3I_COUNT))
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
 bool secu3t = CHECKBIT32(GetFWOptions(), SECU3IO::COPT_SECU3T);
 switch(type)
 {
  case IOX_INIT:
   if (id < (secu3t ? IOS_COUNT : IOS3I_COUNT))
    value = inv ? mp_cddata->iorem.i_slotsi[id] : mp_cddata->iorem.i_slots[id];
   break;
  case IOX_DATA:
   if (id < (secu3t ? IOS_COUNT : IOS3I_COUNT))
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
 bool secu3t = CHECKBIT32(GetFWOptions(), SECU3IO::COPT_SECU3T);
 switch(type)
 {
  case IOX_INIT:
   if (id < (secu3t ? IOP_COUNT : IOP3I_COUNT))
    mp_cddata->iorem.i_plugs[id] = (_fnptr_t)value;
   break;
  case IOX_DATA:
   if (id < (secu3t ? IOP_COUNT : IOP3I_COUNT))
    mp_cddata->iorem.v_plugs[id] = (_fnptr_t)value;
   break;
 }
}

CFirmwareDataMediator::IORemVer CFirmwareDataMediator::GetIORemVersion(void) const
{
 ASSERT(mp_cddata);
 if (!mp_cddata)
  return IOV_V23; //error, return current version
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

//note; value returned by this function includes also size of cd_data_t
size_t CFirmwareDataMediator::GetOnlyCodeSize(const BYTE* ip_bytes) const
{
 ASSERT(ip_bytes);
 //obtain pointer to fw_data_t structure
 fw_data_t* p_fd = (fw_data_t*)(&ip_bytes[m_lip->FIRMWARE_DATA_START]);
 return (m_fpp->m_app_section_size - p_fd->fw_data_size);
}

void CFirmwareDataMediator::LoadCodeData(const BYTE* ip_source_bytes, size_t i_srcSize, BYTE* op_destin_bytes /*= NULL*/)
{
 ASSERT(ip_source_bytes);
 if (!op_destin_bytes)
  op_destin_bytes = mp_bytes_active; //use current firmware bytes as destination

 //obtain actual size of data (source and destination data)
 _uint dataSizeSrc = ((fw_data_t*)((ip_source_bytes + i_srcSize)-sizeof(fw_data_t)))->fw_data_size;
 _uint dataSizeDst = ((fw_data_t*)((op_destin_bytes + m_lip->FIRMWARE_DATA_START)))->fw_data_size;
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
     if (pSrc->iorem.v_plugs[p] == pSrc->iorem.g_stub)
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

void CFirmwareDataMediator::GetCESettingsData(CESettingsData& o_data) const
{
 const fw_data_t* p_fd = (const fw_data_t*)(&getBytes()[m_lip->FIRMWARE_DATA_START]);

 o_data.map_v_min = ((float)p_fd->exdata.cesd.map_v_min) * ADC_DISCRETE;
 o_data.map_v_max = ((float)p_fd->exdata.cesd.map_v_max) * ADC_DISCRETE;
 o_data.map_v_em  = ((float)p_fd->exdata.cesd.map_v_em) * ADC_DISCRETE;
 o_data.map_v_useem = CHECKBIT8(p_fd->exdata.cesd.map_v_flg, 0); 

 o_data.vbat_v_min = ((float)p_fd->exdata.cesd.vbat_v_min) * ADC_DISCRETE;
 o_data.vbat_v_max = ((float)p_fd->exdata.cesd.vbat_v_max) * ADC_DISCRETE;
 o_data.vbat_v_em  = ((float)p_fd->exdata.cesd.vbat_v_em) * ADC_DISCRETE;
 o_data.vbat_v_useem = CHECKBIT8(p_fd->exdata.cesd.vbat_v_flg, 0); 

 o_data.cts_v_min = ((float)p_fd->exdata.cesd.cts_v_min) * ADC_DISCRETE;
 o_data.cts_v_max = ((float)p_fd->exdata.cesd.cts_v_max) * ADC_DISCRETE;
 o_data.cts_v_em  = ((float)p_fd->exdata.cesd.cts_v_em) * ADC_DISCRETE;
 o_data.cts_v_useem = CHECKBIT8(p_fd->exdata.cesd.cts_v_flg, 0); 

 o_data.ks_v_min = ((float)p_fd->exdata.cesd.ks_v_min) * ADC_DISCRETE;
 o_data.ks_v_max = ((float)p_fd->exdata.cesd.ks_v_max) * ADC_DISCRETE;
 o_data.ks_v_em  = ((float)p_fd->exdata.cesd.ks_v_em) * ADC_DISCRETE;
 o_data.ks_v_useem = CHECKBIT8(p_fd->exdata.cesd.ks_v_flg, 0); 

 o_data.tps_v_min = ((float)p_fd->exdata.cesd.tps_v_min) * ADC_DISCRETE;
 o_data.tps_v_max = ((float)p_fd->exdata.cesd.tps_v_max) * ADC_DISCRETE;
 o_data.tps_v_em  = ((float)p_fd->exdata.cesd.tps_v_em) * ADC_DISCRETE;
 o_data.tps_v_useem = CHECKBIT8(p_fd->exdata.cesd.tps_v_flg, 0); 

 o_data.add_i1_v_min = ((float)p_fd->exdata.cesd.add_i1_v_min) * ADC_DISCRETE;
 o_data.add_i1_v_max = ((float)p_fd->exdata.cesd.add_i1_v_max) * ADC_DISCRETE;
 o_data.add_i1_v_em  = ((float)p_fd->exdata.cesd.add_i1_v_em) * ADC_DISCRETE;
 o_data.add_i1_v_useem = CHECKBIT8(p_fd->exdata.cesd.add_i1_v_flg, 0); 

 o_data.add_i2_v_min = ((float)p_fd->exdata.cesd.add_i2_v_min) * ADC_DISCRETE;
 o_data.add_i2_v_max = ((float)p_fd->exdata.cesd.add_i2_v_max) * ADC_DISCRETE;
 o_data.add_i2_v_em  = ((float)p_fd->exdata.cesd.add_i2_v_em) * ADC_DISCRETE;
 o_data.add_i2_v_useem = CHECKBIT8(p_fd->exdata.cesd.add_i2_v_flg, 0); 

 o_data.add_i3_v_min = ((float)p_fd->exdata.cesd.add_i3_v_min) * ADC_DISCRETE;
 o_data.add_i3_v_max = ((float)p_fd->exdata.cesd.add_i3_v_max) * ADC_DISCRETE;
 o_data.add_i3_v_em  = ((float)p_fd->exdata.cesd.add_i3_v_em) * ADC_DISCRETE;
 o_data.add_i3_v_useem = CHECKBIT8(p_fd->exdata.cesd.add_i3_v_flg, 0); 

 o_data.add_i4_v_min = ((float)p_fd->exdata.cesd.add_i4_v_min) * ADC_DISCRETE;
 o_data.add_i4_v_max = ((float)p_fd->exdata.cesd.add_i4_v_max) * ADC_DISCRETE;
 o_data.add_i4_v_em  = ((float)p_fd->exdata.cesd.add_i4_v_em) * ADC_DISCRETE;
 o_data.add_i4_v_useem = CHECKBIT8(p_fd->exdata.cesd.add_i4_v_flg, 0); 

 o_data.add_i5_v_min = ((float)p_fd->exdata.cesd.add_i5_v_min) * ADC_DISCRETE;
 o_data.add_i5_v_max = ((float)p_fd->exdata.cesd.add_i5_v_max) * ADC_DISCRETE;
 o_data.add_i5_v_em  = ((float)p_fd->exdata.cesd.add_i5_v_em) * ADC_DISCRETE;
 o_data.add_i5_v_useem = CHECKBIT8(p_fd->exdata.cesd.add_i5_v_flg, 0); 

 o_data.add_i6_v_min = ((float)p_fd->exdata.cesd.add_i6_v_min) * ADC_DISCRETE;
 o_data.add_i6_v_max = ((float)p_fd->exdata.cesd.add_i6_v_max) * ADC_DISCRETE;
 o_data.add_i6_v_em  = ((float)p_fd->exdata.cesd.add_i6_v_em) * ADC_DISCRETE;
 o_data.add_i6_v_useem = CHECKBIT8(p_fd->exdata.cesd.add_i6_v_flg, 0); 

 o_data.add_i7_v_min = ((float)p_fd->exdata.cesd.add_i7_v_min) * ADC_DISCRETE;
 o_data.add_i7_v_max = ((float)p_fd->exdata.cesd.add_i7_v_max) * ADC_DISCRETE;
 o_data.add_i7_v_em  = ((float)p_fd->exdata.cesd.add_i7_v_em) * ADC_DISCRETE;
 o_data.add_i7_v_useem = CHECKBIT8(p_fd->exdata.cesd.add_i7_v_flg, 0); 

 o_data.add_i8_v_min = ((float)p_fd->exdata.cesd.add_i8_v_min) * ADC_DISCRETE;
 o_data.add_i8_v_max = ((float)p_fd->exdata.cesd.add_i8_v_max) * ADC_DISCRETE;
 o_data.add_i8_v_em  = ((float)p_fd->exdata.cesd.add_i8_v_em) * ADC_DISCRETE;
 o_data.add_i8_v_useem = CHECKBIT8(p_fd->exdata.cesd.add_i8_v_flg, 0); 

 o_data.oilpress_thrd = ((float)p_fd->exdata.cesd.oilpress_thrd) / 256.0f;
 o_data.oilpress_timer = p_fd->exdata.cesd.oilpress_timer;
}

void CFirmwareDataMediator::SetCESettingsData(const CESettingsData& i_data)
{
 fw_data_t* p_fd = (fw_data_t*)(&getBytes()[m_lip->FIRMWARE_DATA_START]);

 p_fd->exdata.cesd.map_v_min = MathHelpers::Round((i_data.map_v_min / ADC_DISCRETE));
 p_fd->exdata.cesd.map_v_max = MathHelpers::Round((i_data.map_v_max / ADC_DISCRETE));
 p_fd->exdata.cesd.map_v_em  = MathHelpers::Round((i_data.map_v_em / ADC_DISCRETE));
 WRITEBIT8(p_fd->exdata.cesd.map_v_flg, 0, i_data.map_v_useem);

 p_fd->exdata.cesd.vbat_v_min = MathHelpers::Round((i_data.vbat_v_min / ADC_DISCRETE));
 p_fd->exdata.cesd.vbat_v_max = MathHelpers::Round((i_data.vbat_v_max / ADC_DISCRETE));
 p_fd->exdata.cesd.vbat_v_em  = MathHelpers::Round((i_data.vbat_v_em / ADC_DISCRETE));
 WRITEBIT8(p_fd->exdata.cesd.vbat_v_flg, 0, i_data.vbat_v_useem);

 p_fd->exdata.cesd.cts_v_min = MathHelpers::Round((i_data.cts_v_min / ADC_DISCRETE));
 p_fd->exdata.cesd.cts_v_max = MathHelpers::Round((i_data.cts_v_max / ADC_DISCRETE));
 p_fd->exdata.cesd.cts_v_em  = MathHelpers::Round((i_data.cts_v_em / ADC_DISCRETE));
 WRITEBIT8(p_fd->exdata.cesd.cts_v_flg, 0, i_data.cts_v_useem);

 p_fd->exdata.cesd.ks_v_min = MathHelpers::Round((i_data.ks_v_min / ADC_DISCRETE));
 p_fd->exdata.cesd.ks_v_max = MathHelpers::Round((i_data.ks_v_max / ADC_DISCRETE));
 p_fd->exdata.cesd.ks_v_em  = MathHelpers::Round((i_data.ks_v_em / ADC_DISCRETE));
 WRITEBIT8(p_fd->exdata.cesd.ks_v_flg, 0, i_data.ks_v_useem);

 p_fd->exdata.cesd.tps_v_min = MathHelpers::Round((i_data.tps_v_min / ADC_DISCRETE));
 p_fd->exdata.cesd.tps_v_max = MathHelpers::Round((i_data.tps_v_max / ADC_DISCRETE));
 p_fd->exdata.cesd.tps_v_em  = MathHelpers::Round((i_data.tps_v_em / ADC_DISCRETE));
 WRITEBIT8(p_fd->exdata.cesd.tps_v_flg, 0, i_data.tps_v_useem);

 p_fd->exdata.cesd.add_i1_v_min = MathHelpers::Round((i_data.add_i1_v_min / ADC_DISCRETE));
 p_fd->exdata.cesd.add_i1_v_max = MathHelpers::Round((i_data.add_i1_v_max / ADC_DISCRETE));
 p_fd->exdata.cesd.add_i1_v_em  = MathHelpers::Round((i_data.add_i1_v_em / ADC_DISCRETE));
 WRITEBIT8(p_fd->exdata.cesd.add_i1_v_flg, 0, i_data.add_i1_v_useem);

 p_fd->exdata.cesd.add_i2_v_min = MathHelpers::Round((i_data.add_i2_v_min / ADC_DISCRETE));
 p_fd->exdata.cesd.add_i2_v_max = MathHelpers::Round((i_data.add_i2_v_max / ADC_DISCRETE));
 p_fd->exdata.cesd.add_i2_v_em  = MathHelpers::Round((i_data.add_i2_v_em / ADC_DISCRETE));
 WRITEBIT8(p_fd->exdata.cesd.add_i2_v_flg, 0, i_data.add_i2_v_useem);

 p_fd->exdata.cesd.add_i3_v_min = MathHelpers::Round((i_data.add_i3_v_min / ADC_DISCRETE));
 p_fd->exdata.cesd.add_i3_v_max = MathHelpers::Round((i_data.add_i3_v_max / ADC_DISCRETE));
 p_fd->exdata.cesd.add_i3_v_em  = MathHelpers::Round((i_data.add_i3_v_em / ADC_DISCRETE));
 WRITEBIT8(p_fd->exdata.cesd.add_i3_v_flg, 0, i_data.add_i3_v_useem);

 p_fd->exdata.cesd.add_i4_v_min = MathHelpers::Round((i_data.add_i4_v_min / ADC_DISCRETE));
 p_fd->exdata.cesd.add_i4_v_max = MathHelpers::Round((i_data.add_i4_v_max / ADC_DISCRETE));
 p_fd->exdata.cesd.add_i4_v_em  = MathHelpers::Round((i_data.add_i4_v_em / ADC_DISCRETE));
 WRITEBIT8(p_fd->exdata.cesd.add_i4_v_flg, 0, i_data.add_i4_v_useem);

 p_fd->exdata.cesd.add_i5_v_min = MathHelpers::Round((i_data.add_i5_v_min / ADC_DISCRETE));
 p_fd->exdata.cesd.add_i5_v_max = MathHelpers::Round((i_data.add_i5_v_max / ADC_DISCRETE));
 p_fd->exdata.cesd.add_i5_v_em  = MathHelpers::Round((i_data.add_i5_v_em / ADC_DISCRETE));
 WRITEBIT8(p_fd->exdata.cesd.add_i5_v_flg, 0, i_data.add_i5_v_useem);

 p_fd->exdata.cesd.add_i6_v_min = MathHelpers::Round((i_data.add_i6_v_min / ADC_DISCRETE));
 p_fd->exdata.cesd.add_i6_v_max = MathHelpers::Round((i_data.add_i6_v_max / ADC_DISCRETE));
 p_fd->exdata.cesd.add_i6_v_em  = MathHelpers::Round((i_data.add_i6_v_em / ADC_DISCRETE));
 WRITEBIT8(p_fd->exdata.cesd.add_i6_v_flg, 0, i_data.add_i6_v_useem);

 p_fd->exdata.cesd.add_i7_v_min = MathHelpers::Round((i_data.add_i7_v_min / ADC_DISCRETE));
 p_fd->exdata.cesd.add_i7_v_max = MathHelpers::Round((i_data.add_i7_v_max / ADC_DISCRETE));
 p_fd->exdata.cesd.add_i7_v_em  = MathHelpers::Round((i_data.add_i7_v_em / ADC_DISCRETE));
 WRITEBIT8(p_fd->exdata.cesd.add_i7_v_flg, 0, i_data.add_i7_v_useem);

 p_fd->exdata.cesd.add_i8_v_min = MathHelpers::Round((i_data.add_i8_v_min / ADC_DISCRETE));
 p_fd->exdata.cesd.add_i8_v_max = MathHelpers::Round((i_data.add_i8_v_max / ADC_DISCRETE));
 p_fd->exdata.cesd.add_i8_v_em  = MathHelpers::Round((i_data.add_i8_v_em / ADC_DISCRETE));
 WRITEBIT8(p_fd->exdata.cesd.add_i8_v_flg, 0, i_data.add_i8_v_useem);

 p_fd->exdata.cesd.oilpress_thrd = MathHelpers::Round((i_data.oilpress_thrd * 256.0f));
 p_fd->exdata.cesd.oilpress_timer = i_data.oilpress_timer;
}

void CFirmwareDataMediator::GetFwConstsData(SECU3IO::FwConstsData& o_data) const
{
 const fw_ex_data_t& exd = ((const fw_data_t*)(&getBytes()[m_lip->FIRMWARE_DATA_START]))->exdata;
 //convert data
 o_data.fi_enter_strokes = exd.fi_enter_strokes;
 o_data.fi_leave_strokes = exd.fi_leave_strokes;
 o_data.iac_cond_add = (exd.iac_cond_add / 2.0f);
 o_data.inj_max_pw = (exd.inj_max_pw * 3.2f) / 1000.0f; //from 3.2 us units to ms
 o_data.aircond_clt = ((float)exd.aircond_clt) / TEMP_PHYSICAL_MAGNITUDE_MULTIPLIER;
 o_data.aircond_tps = ((float)exd.aircond_tps) / TPS_PHYSICAL_MAGNITUDE_MULTIPLIER;
 o_data.idl_ve = ((float)exd.idl_ve) / VE_MAPS_M_FACTOR;
 o_data.evap_clt = ((float)exd.evap_clt) / TEMP_PHYSICAL_MAGNITUDE_MULTIPLIER;
 o_data.evap_tps_lo = ((float)exd.evap_tps_lo) / TPS_PHYSICAL_MAGNITUDE_MULTIPLIER;
 o_data.evap_tps_hi = ((float)exd.evap_tps_hi) / TPS_PHYSICAL_MAGNITUDE_MULTIPLIER;
 o_data.frap = ((float)exd.frap) / MAP_PHYSICAL_MAGNITUDE_MULTIPLIER;
 o_data.idl_ve_g = ((float)exd.idl_ve_g) / VE_MAPS_M_FACTOR;
 o_data.stbl_str_cnt = exd.stbl_str_cnt;
 o_data.heating_t_off = ((float)exd.heating_t_off) / TEMP_PHYSICAL_MAGNITUDE_MULTIPLIER;
 o_data.heating_time = ((float)exd.heating_time) / 10.0f; //convert from 6 sec units to minutes
 o_data.idltorun_stp_en = ((float)exd.idltorun_stp_en) / 32.0f; //convert to %
 o_data.idltorun_stp_le = ((float)exd.idltorun_stp_le) / 32.0f; //convert to %
 for(int i = 0; i < INPAVNUM; i++) o_data.inpavnum[i] = exd.inpavnum[i]; 
 o_data.vent_delay = ((float)exd.vent_delay) / 100.0f; //convert to sec
 o_data.vent_iacoff = ((float)exd.vent_iacoff) / 2.0f; //convert to %
 o_data.epas_iacoff = ((float)exd.epas_iacoff) / 2.0f; //convert to %
 o_data.vent_pwmsteps = exd.vent_pwmsteps;
 o_data.vent_minband = exd.vent_minband;
 o_data.an_tps_mul = exd.an_tps_mul;
 o_data.igntim_wrkmap = exd.igntim_wrkmap;
 o_data.shift_igntim = ((float)exd.shift_igntim) / ANGLE_MULTIPLIER;
 o_data.fldclr_start = exd.fldclr_start;
 o_data.hall_predict = exd.hall_predict;

 int vtachom_mult = exd.vtachom_mult;
 if (vtachom_mult == 0)
  vtachom_mult = 1; //prevent division by zero
 o_data.vtachom_mult = 1.0f / ((float)vtachom_mult / 8192.0f); //1/x

 o_data.grheat_time = ((float)exd.grheat_time) / 6000.0f; //convert from 1/100 sec units to minutes
 o_data.add_i1_sub = exd.add_i1_sub;
 o_data.add_i2_sub = exd.add_i2_sub;
 o_data.idlreg_captrange = exd.idlreg_captrange;
 o_data.manigntim_idl = exd.manigntim_idl;
 o_data.idlent_timval = ((float)exd.idlent_timval) / 100.0f; //convert to sec;
 o_data.gasval_ontime = ((float)exd.gasval_ontime) / 100.0f; //convert to sec;

 for(int i = 0; i < 8; ++i)
  o_data.tdc_angle[i] = ((float)exd.tdc_angle[i]) / ANGLE_MULTIPLIER;
 o_data.smp_angle = ((float)exd.smp_angle) / ANGLE_MULTIPLIER;

 float discrete = PlatformParamHolder::GetQuartzFact(m_fpp->m_platform_id); //for ATMega1284 discrete = 3.2uS, for others - 4.0uS
 o_data.dwl_dead_time = (((float)exd.dwl_dead_time) * discrete) / 1000.0f; // convert to ms

 o_data.sfc_tps_thrd = ((float)exd.sfc_tps_thrd) / TPS_PHYSICAL_MAGNITUDE_MULTIPLIER;
 o_data.evap_map_thrd = ((float)exd.evap_map_thrd) / MAP_PHYSICAL_MAGNITUDE_MULTIPLIER;
 o_data.ckps_skip_trig = exd.ckps_skip_trig;
 o_data.maninjpw_idl = exd.maninjpw_idl;

 o_data.oilpress_cut = exd.oilpress_cut;
 o_data.tpsdot_mindt = (exd.tpsdot_mindt * 3.2f) / 1000.0f; //convert from 3.2 us units to ms
 
 o_data.irr_k_load = exd.irr_k_load / 32.0f;
 o_data.irr_k_rpm = exd.irr_k_rpm / 32.0f;

 o_data.cold_eng_int = exd.cold_eng_int;
}

void CFirmwareDataMediator::SetFwConstsData(const SECU3IO::FwConstsData& i_data)
{
 fw_ex_data_t& exd = ((fw_data_t*)(&getBytes()[m_lip->FIRMWARE_DATA_START]))->exdata;
 //convert data
 exd.fi_enter_strokes = i_data.fi_enter_strokes;
 exd.fi_leave_strokes = i_data.fi_leave_strokes;
 exd.iac_cond_add = MathHelpers::Round(i_data.iac_cond_add * 2.0f);
 exd.inj_max_pw = MathHelpers::Round((i_data.inj_max_pw * 1000.0f) / 3.2f); //from ms to 3.2us units
 exd.aircond_clt = MathHelpers::Round(i_data.aircond_clt * TEMP_PHYSICAL_MAGNITUDE_MULTIPLIER);
 exd.aircond_tps = MathHelpers::Round(i_data.aircond_tps * TPS_PHYSICAL_MAGNITUDE_MULTIPLIER);
 exd.idl_ve = MathHelpers::Round(i_data.idl_ve * VE_MAPS_M_FACTOR);
 exd.evap_clt = MathHelpers::Round(i_data.evap_clt * TEMP_PHYSICAL_MAGNITUDE_MULTIPLIER);
 exd.evap_tps_lo = MathHelpers::Round(i_data.evap_tps_lo * TPS_PHYSICAL_MAGNITUDE_MULTIPLIER);
 exd.evap_tps_hi = MathHelpers::Round(i_data.evap_tps_hi * TPS_PHYSICAL_MAGNITUDE_MULTIPLIER);
 exd.frap = MathHelpers::Round(i_data.frap * MAP_PHYSICAL_MAGNITUDE_MULTIPLIER);
 exd.idl_ve_g = MathHelpers::Round(i_data.idl_ve_g * VE_MAPS_M_FACTOR);
 exd.stbl_str_cnt = i_data.stbl_str_cnt;
 exd.heating_t_off = MathHelpers::Round(i_data.heating_t_off * TEMP_PHYSICAL_MAGNITUDE_MULTIPLIER);
 exd.heating_time = MathHelpers::Round(i_data.heating_time * 10.0f);
 exd.idltorun_stp_en = MathHelpers::Round(i_data.idltorun_stp_en * 32.0f);
 exd.idltorun_stp_le = MathHelpers::Round(i_data.idltorun_stp_le * 32.0f);
 for(int i = 0; i < INPAVNUM; i++) exd.inpavnum[i] = i_data.inpavnum[i]; 
 exd.vent_delay = MathHelpers::Round(i_data.vent_delay * 100.0f);
 exd.vent_iacoff = MathHelpers::Round(i_data.vent_iacoff * 2.0f);
 exd.epas_iacoff = MathHelpers::Round(i_data.epas_iacoff * 2.0f);
 exd.vent_pwmsteps = i_data.vent_pwmsteps;
 exd.vent_minband = i_data.vent_minband;
 exd.an_tps_mul = i_data.an_tps_mul;
 exd.igntim_wrkmap = i_data.igntim_wrkmap;
 exd.shift_igntim = MathHelpers::Round(i_data.shift_igntim * ANGLE_MULTIPLIER);
 exd.fldclr_start = i_data.fldclr_start;
 exd.hall_predict = i_data.hall_predict;
 exd.vtachom_mult = MathHelpers::Round((1.0f / i_data.vtachom_mult) * 8192.0f);
 exd.grheat_time = MathHelpers::Round(i_data.grheat_time * 6000.0f);
 exd.add_i1_sub = i_data.add_i1_sub;
 exd.add_i2_sub = i_data.add_i2_sub;
 exd.idlreg_captrange = i_data.idlreg_captrange;
 exd.manigntim_idl = i_data.manigntim_idl;
 exd.idlent_timval = MathHelpers::Round(i_data.idlent_timval * 100.0f);
 exd.gasval_ontime = MathHelpers::Round(i_data.gasval_ontime * 100.0f);

 for(int i = 0; i < 8; ++i)
  exd.tdc_angle[i] = MathHelpers::Round(i_data.tdc_angle[i] * ANGLE_MULTIPLIER);
 exd.smp_angle = MathHelpers::Round(i_data.smp_angle * ANGLE_MULTIPLIER);

 float discrete = PlatformParamHolder::GetQuartzFact(m_fpp->m_platform_id); //for ATMega1284 discrete = 3.2uS, for others - 4.0uS
 exd.dwl_dead_time = (_uint)MathHelpers::Round((i_data.dwl_dead_time * 1000.0) / discrete);

 exd.sfc_tps_thrd = MathHelpers::Round(i_data.sfc_tps_thrd * TPS_PHYSICAL_MAGNITUDE_MULTIPLIER);
 exd.evap_map_thrd = MathHelpers::Round(i_data.evap_map_thrd * MAP_PHYSICAL_MAGNITUDE_MULTIPLIER);
 exd.ckps_skip_trig = i_data.ckps_skip_trig;
 exd.maninjpw_idl = i_data.maninjpw_idl;

 exd.oilpress_cut = i_data.oilpress_cut;
 exd.tpsdot_mindt = MathHelpers::Round((i_data.tpsdot_mindt * 1000.0f) / 3.2f); //from ms to 3.2us units

 exd.irr_k_load = MathHelpers::Round(i_data.irr_k_load * 32.0f);
 exd.irr_k_rpm = MathHelpers::Round(i_data.irr_k_rpm * 32.0f);

 exd.cold_eng_int = i_data.cold_eng_int;
}
