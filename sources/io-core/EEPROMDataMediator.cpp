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

/** \file EEPROMDataMediator.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "CRC16.h"
#include "EEPROMDataMediator.h"
#include "FirmwareMapsDataHolder.h"
#include "SECU3IO.h"
#include "SECU3ParametersDef.h"
#include "SECU3TablesDef.h"
#include "common/MathHelpers.h"
#include "ufcodes.h"
#include "BitMask.h"
#include "Magnitude.h"
#include "io-core/ce_errors.h"

using namespace SECU3IO;
using namespace SECU3IO::SECU3Types;

//Address of parameters structure in EEPROM
#define EEPROM_PARAM_START 0x0001

//Address of errors's array (Check Engine) in EEPROM
#define EEPROM_ECUERRORS_START (EEPROM_PARAM_START + (sizeof(params_t)))

//Address of tables which can be edited in real time
#define EEPROM_REALTIME_TABLES_START (EEPROM_ECUERRORS_START + 8)

EEPROMDataMediator::EEPROMDataMediator(const PPEepromParam& i_epp)
: m_epp(i_epp)
, m_eeprom_size(i_epp.m_size)
, m_is_opened(false)
{
 m_bytes_active = new BYTE[m_epp.m_size + 1];
 ASSERT(m_bytes_active);
 m_bytes_original = new BYTE[m_epp.m_size + 1];
 ASSERT(m_bytes_original);

 memset(m_bytes_active, 0x00, m_eeprom_size);
 memset(m_bytes_original, 0x00, m_eeprom_size);
}

EEPROMDataMediator::~EEPROMDataMediator()
{
 delete m_bytes_active;
 delete m_bytes_original;
}

BYTE* EEPROMDataMediator::getBytes(bool i_original /*= false*/)
{
 if (i_original)
  return m_bytes_original;
 else
  return m_bytes_active;
}

void EEPROMDataMediator::LoadBytes(const BYTE* i_bytes)
{
 ASSERT(i_bytes);
 memset(m_bytes_active, 0x00, m_eeprom_size);
 memset(m_bytes_original, 0x00, m_eeprom_size);

 memcpy(m_bytes_active, i_bytes, m_eeprom_size);
 memcpy(m_bytes_original, i_bytes, m_eeprom_size);

 m_is_opened = true;
}

void EEPROMDataMediator::StoreBytes(BYTE* op_bytes)
{
 ASSERT(op_bytes);
 memcpy(op_bytes, m_bytes_active, m_eeprom_size);
}

bool EEPROMDataMediator::IsModified(void)
{
 return (0 != memcmp(m_bytes_active, m_bytes_original, m_eeprom_size));
}

bool EEPROMDataMediator::IsLoaded(void) 
{
 return m_is_opened;
}

void EEPROMDataMediator::ResetModified(void)
{
 memcpy(m_bytes_original, m_bytes_active, m_eeprom_size);
}

bool EEPROMDataMediator::VerifyParamsCheckSum(const BYTE* ip_eeprom_buffer)
{
 _uint crc = crc16(ip_eeprom_buffer + EEPROM_PARAM_START, sizeof(params_t) - sizeof(_uint));
 const _uint* crc_addr = (_uint*)(&ip_eeprom_buffer[EEPROM_PARAM_START + sizeof(params_t) - sizeof(_uint)]);
 return (*crc_addr)==crc; //check it!
}

size_t EEPROMDataMediator::GetParamsStartAddr(void) const
{
 return EEPROM_PARAM_START;
}

void EEPROMDataMediator::GetStartMap(int i_index, float* o_values, bool i_original/* = false*/)
{
 ASSERT(o_values);

 //получаем адрес начала таблиц семейств характеристик
 f_data_t* p_maps = (f_data_t*)(getBytes(i_original) + EEPROM_REALTIME_TABLES_START);

 for (int i = 0; i < F_STR_POINTS; i++ )
  o_values[i] = ((float)p_maps[i_index].f_str[i]) / AA_MAPS_M_FACTOR;
}

void EEPROMDataMediator::SetStartMap(int i_index,const float* ip_values)
{
 ASSERT(ip_values);

 f_data_t* p_maps = (f_data_t*)(getBytes() + EEPROM_REALTIME_TABLES_START);

 for (int i = 0; i < F_STR_POINTS; i++ )
  p_maps[i_index].f_str[i] = MathHelpers::Round((ip_values[i]*AA_MAPS_M_FACTOR));
}

void EEPROMDataMediator::GetIdleMap(int i_index,  float* o_values, bool i_original/* = false*/)
{
 ASSERT(o_values);

 //получаем адрес начала таблиц семейств характеристик
 f_data_t* p_maps = (f_data_t*)(getBytes(i_original) + EEPROM_REALTIME_TABLES_START);

 for (int i = 0; i < F_IDL_POINTS; i++ )
  o_values[i] = ((float)p_maps[i_index].f_idl[i]) / AA_MAPS_M_FACTOR;
}

void EEPROMDataMediator::SetIdleMap(int i_index,const float* ip_values)
{
 ASSERT(ip_values);

 //получаем адрес начала таблиц семейств характеристик
 f_data_t* p_maps = (f_data_t*)(getBytes() + EEPROM_REALTIME_TABLES_START);

 for (int i = 0; i < F_IDL_POINTS; i++ )
  p_maps[i_index].f_idl[i] = MathHelpers::Round((ip_values[i]*AA_MAPS_M_FACTOR));
}

void EEPROMDataMediator::GetWorkMap(int i_index, float* o_values, bool i_original/* = false*/)
{
 ASSERT(o_values);

 //получаем адрес начала таблиц семейств характеристик
 f_data_t* p_maps = (f_data_t*)(getBytes(i_original) + EEPROM_REALTIME_TABLES_START);

 for (int i = 0; i < (F_WRK_POINTS_F * F_WRK_POINTS_L); i++ )
 {
  _char *p = &(p_maps[i_index].f_wrk[0][0]);
  o_values[i] = ((float) *(p + i)) / AA_MAPS_M_FACTOR;
 }
}

void EEPROMDataMediator::SetWorkMap(int i_index, const float* ip_values)
{
 ASSERT(ip_values);

 //получаем адрес начала таблиц семейств характеристик
 f_data_t* p_maps = (f_data_t*)(getBytes() + EEPROM_REALTIME_TABLES_START);

 for (int i = 0; i < (F_WRK_POINTS_F * F_WRK_POINTS_L); i++ )
 {
  _char *p = &(p_maps[i_index].f_wrk[0][0]);
  *(p + i) = MathHelpers::Round((ip_values[i]*AA_MAPS_M_FACTOR));
 }
}

void EEPROMDataMediator::GetTempMap(int i_index, float* o_values, bool i_original/* = false*/)
{
 ASSERT(o_values);

 //получаем адрес начала таблиц семейств характеристик
 f_data_t* p_maps = (f_data_t*)(getBytes(i_original) + EEPROM_REALTIME_TABLES_START);

 for (int i = 0; i < F_TMP_POINTS; i++ )
  o_values[i] = ((float)p_maps[i_index].f_tmp[i]) / AA_MAPS_M_FACTOR;
}

void EEPROMDataMediator::SetTempMap(int i_index,const float* ip_values)
{
 ASSERT(ip_values);

 //получаем адрес начала таблиц семейств характеристик
 f_data_t* p_maps = (f_data_t*)(getBytes() + EEPROM_REALTIME_TABLES_START);

 for (int i = 0; i < F_TMP_POINTS; i++ )
  p_maps[i_index].f_tmp[i] = MathHelpers::Round((ip_values[i]*AA_MAPS_M_FACTOR));
}

void EEPROMDataMediator::GetTempIdlMap(int i_index, float* o_values, bool i_original/* = false*/)
{
 ASSERT(o_values);

 //получаем адрес начала таблиц семейств характеристик
 f_data_t* p_maps = (f_data_t*)(getBytes(i_original) + EEPROM_REALTIME_TABLES_START);

 for (int i = 0; i < F_TMP_POINTS; i++ )
  o_values[i] = ((float)p_maps[i_index].f_tmp_idl[i]) / AA_MAPS_M_FACTOR;
}

void EEPROMDataMediator::SetTempIdlMap(int i_index,const float* ip_values)
{
 ASSERT(ip_values);

 //получаем адрес начала таблиц семейств характеристик
 f_data_t* p_maps = (f_data_t*)(getBytes() + EEPROM_REALTIME_TABLES_START);

 for (int i = 0; i < F_TMP_POINTS; i++ )
  p_maps[i_index].f_tmp_idl[i] = MathHelpers::Round((ip_values[i]*AA_MAPS_M_FACTOR));
}

void EEPROMDataMediator::GetVEMap(int i_index, float* op_values, bool i_original /* = false*/)
{
 ASSERT(op_values);

 //получаем адрес начала таблиц семейств характеристик
 f_data_t* p_maps = (f_data_t*)(getBytes(i_original) + EEPROM_REALTIME_TABLES_START);

 for (int i = 0; i < (INJ_VE_POINTS_F * INJ_VE_POINTS_L); i++ )
 {
  _uchar *p = &(p_maps[i_index].inj_ve[0][0]);
  op_values[i] = ((float)w12GetCell(p, i)) / VE_MAPS_M_FACTOR;
 }
}

void EEPROMDataMediator::SetVEMap(int i_index, const float* ip_values)
{
 ASSERT(ip_values);

 f_data_t* p_maps = (f_data_t*)(getBytes() + EEPROM_REALTIME_TABLES_START);

 for (int i = 0; i < (INJ_VE_POINTS_F * INJ_VE_POINTS_L); i++ )
 {
  _uchar *p = &(p_maps[i_index].inj_ve[0][0]);
  w12SetCell(p, i, MathHelpers::Round(ip_values[i]*VE_MAPS_M_FACTOR));
 }
}

void EEPROMDataMediator::GetAFRMap(int i_index, float* op_values, bool i_original /* = false*/)
{
 ASSERT(op_values);

 //получаем адрес начала таблиц семейств характеристик
 f_data_t* p_maps = (f_data_t*)(getBytes(i_original) + EEPROM_REALTIME_TABLES_START);

 for (int i = 0; i < (INJ_VE_POINTS_F * INJ_VE_POINTS_L); i++ )
 {
  _uchar *p = &(p_maps[i_index].inj_afr[0][0]);
  op_values[i] = (((float) *(p + i)) / AFR_MAPS_M_FACTOR) + 8.0f;
 }
}

void EEPROMDataMediator::SetAFRMap(int i_index, const float* ip_values)
{
 ASSERT(ip_values);

 f_data_t* p_maps = (f_data_t*)(getBytes() + EEPROM_REALTIME_TABLES_START);

 for (int i = 0; i < (INJ_VE_POINTS_F * INJ_VE_POINTS_L); i++ )
 {
  _uchar *p = &(p_maps[i_index].inj_afr[0][0]);
  *(p + i) = MathHelpers::Round((ip_values[i]-8.0) * AFR_MAPS_M_FACTOR);
 }
}

void EEPROMDataMediator::GetCrnkMap(int i_index, float* op_values, bool i_original /* = false */)
{
 ASSERT(op_values);

 //получаем адрес структуры дополнительных данных
 f_data_t* p_maps = (f_data_t*)(getBytes(i_original) + EEPROM_REALTIME_TABLES_START);

 float discrete = PlatformParamHolder::GetQuartzFact(m_epp.m_platform_id); //for ATMega644 discrete = 3.2uS, for others - 4.0uS
 for(size_t i = 0; i < INJ_CRANKING_LOOKUP_TABLE_SIZE; i++)
  op_values[i] = (p_maps[i_index].inj_cranking[i] * discrete) / 1000.0f; //convert to ms
}

void EEPROMDataMediator::SetCrnkMap(int i_index, const float* ip_values)
{
 ASSERT(ip_values);
 if (!ip_values)
  return;

 //получаем адрес структуры дополнительных данных
 f_data_t* p_maps = (f_data_t*)(getBytes() + EEPROM_REALTIME_TABLES_START);

 float discrete = PlatformParamHolder::GetQuartzFact(m_epp.m_platform_id); //for ATMega644 discrete = 3.2uS, for others - 4.0uS
 for(size_t i = 0; i < INJ_CRANKING_LOOKUP_TABLE_SIZE; i++)
  p_maps[i_index].inj_cranking[i] = (_uint)MathHelpers::Round((ip_values[i] * 1000.0) / discrete);
}

void EEPROMDataMediator::GetWrmpMap(int i_index, float* op_values, bool i_original /* = false */)
{
 ASSERT(op_values);

 //получаем адрес начала таблиц семейств характеристик
 f_data_t* p_maps = (f_data_t*)(getBytes(i_original) + EEPROM_REALTIME_TABLES_START);

 for (int i = 0; i < INJ_WARMUP_LOOKUP_TABLE_SIZE; i++ )
  op_values[i] = ((float)p_maps[i_index].inj_warmup[i]) / WRMP_MAPS_M_FACTOR;
}

void EEPROMDataMediator::SetWrmpMap(int i_index,const float* ip_values)
{
 ASSERT(ip_values);

 f_data_t* p_maps = (f_data_t*)(getBytes() + EEPROM_REALTIME_TABLES_START);

 for (int i = 0; i < INJ_WARMUP_LOOKUP_TABLE_SIZE; i++ )
  p_maps[i_index].inj_warmup[i] = MathHelpers::Round((ip_values[i]*WRMP_MAPS_M_FACTOR));
}

void EEPROMDataMediator::GetDeadMap(int i_index, float* op_values, bool i_original /* = false */)
{
 ASSERT(op_values);
 if (!op_values)
  return;

 //получаем адрес структуры дополнительных данных
 f_data_t* p_maps = (f_data_t*)(getBytes(i_original) + EEPROM_REALTIME_TABLES_START);

 float discrete = PlatformParamHolder::GetQuartzFact(m_epp.m_platform_id); //for ATMega644 discrete = 3.2uS, for others - 4.0uS
 for(size_t i = 0; i < INJ_DT_LOOKUP_TABLE_SIZE; i++)
  op_values[i] = (p_maps[i_index].inj_dead_time[i] * discrete) / 1000.0f; //convert to ms
}

void EEPROMDataMediator::SetDeadMap(int i_index, const float* ip_values)
{
 ASSERT(ip_values);
 if (!ip_values)
  return;

 f_data_t* p_maps = (f_data_t*)(getBytes() + EEPROM_REALTIME_TABLES_START);

 float discrete = PlatformParamHolder::GetQuartzFact(m_epp.m_platform_id); //for ATMega644 discrete = 3.2uS, for others - 4.0uS
 for(size_t i = 0; i < INJ_DT_LOOKUP_TABLE_SIZE; i++)
  p_maps[i_index].inj_dead_time[i] = (_int)MathHelpers::Round((ip_values[i] * 1000.0) / discrete);
}

void EEPROMDataMediator::GetIdlrMap(int i_index,float* op_values, bool i_original /* = false */)
{
 ASSERT(op_values);

 //получаем адрес начала таблиц семейств характеристик
 f_data_t* p_maps = (f_data_t*)(getBytes(i_original) + EEPROM_REALTIME_TABLES_START);

 for (int i = 0; i < INJ_IAC_POS_TABLE_SIZE; i++ )
  op_values[i] = ((float)p_maps[i_index].inj_iac_run_pos[i]) / IACPOS_MAPS_M_FACTOR;
}

void EEPROMDataMediator::SetIdlrMap(int i_index,const float* ip_values)
{
 ASSERT(ip_values);

 f_data_t* p_maps = (f_data_t*)(getBytes() + EEPROM_REALTIME_TABLES_START);

 for (int i = 0; i < INJ_IAC_POS_TABLE_SIZE; i++ )
  p_maps[i_index].inj_iac_run_pos[i] = MathHelpers::Round((ip_values[i]*IACPOS_MAPS_M_FACTOR));
}

void EEPROMDataMediator::GetIdlcMap(int i_index,float* op_values, bool i_original /* = false */)
{
 ASSERT(op_values);

 //получаем адрес начала таблиц семейств характеристик
 f_data_t* p_maps = (f_data_t*)(getBytes(i_original) + EEPROM_REALTIME_TABLES_START);

 for (int i = 0; i < INJ_IAC_POS_TABLE_SIZE; i++ )
  op_values[i] = ((float)p_maps[i_index].inj_iac_crank_pos[i]) / IACPOS_MAPS_M_FACTOR;
}

void EEPROMDataMediator::SetIdlcMap(int i_index,const float* ip_values)
{
 ASSERT(ip_values);

 f_data_t* p_maps = (f_data_t*)(getBytes() + EEPROM_REALTIME_TABLES_START);

 for (int i = 0; i < INJ_IAC_POS_TABLE_SIZE; i++ )
  p_maps[i_index].inj_iac_crank_pos[i] = MathHelpers::Round((ip_values[i]*IACPOS_MAPS_M_FACTOR));
}

void EEPROMDataMediator::GetAETPSMap(int i_index, float* op_values, bool i_original /* = false */)
{
 ASSERT(op_values);

 //получаем адрес начала таблиц семейств характеристик
 f_data_t* p_maps = (f_data_t*)(getBytes(i_original) + EEPROM_REALTIME_TABLES_START);

 for (int i = 0; i < INJ_AE_TPS_LOOKUP_TABLE_SIZE; i++ )
  op_values[i] = ((float)p_maps[i_index].inj_ae_tps_enr[i]) - AETPSV_MAPS_ADDER; //shift by 55
 for (int i = 0; i < INJ_AE_TPS_LOOKUP_TABLE_SIZE; i++ )
  op_values[i+INJ_AE_TPS_LOOKUP_TABLE_SIZE] = ((float)p_maps[i_index].inj_ae_tps_bins[i]) / AETPSB_MAPS_M_FACTOR;   //convert from %/100ms to %/s
}

void EEPROMDataMediator::SetAETPSMap(int i_index, const float* ip_values)
{
 ASSERT(ip_values);

 f_data_t* p_maps = (f_data_t*)(getBytes() + EEPROM_REALTIME_TABLES_START);

 for (int i = 0; i < INJ_AE_TPS_LOOKUP_TABLE_SIZE; i++ )
  p_maps[i_index].inj_ae_tps_enr[i] = MathHelpers::Round((ip_values[i] + AETPSV_MAPS_ADDER));
 for (int i = 0; i < INJ_AE_TPS_LOOKUP_TABLE_SIZE; i++ )
  p_maps[i_index].inj_ae_tps_bins[i] = MathHelpers::Round((ip_values[i+INJ_AE_TPS_LOOKUP_TABLE_SIZE]*AETPSB_MAPS_M_FACTOR));
}

void EEPROMDataMediator::GetAERPMMap(int i_index, float* op_values, bool i_original /* = false */)
{
 ASSERT(op_values);

 //получаем адрес начала таблиц семейств характеристик
 f_data_t* p_maps = (f_data_t*)(getBytes(i_original) + EEPROM_REALTIME_TABLES_START);

 for (int i = 0; i < INJ_AE_RPM_LOOKUP_TABLE_SIZE; i++ )
  op_values[i] = ((float)p_maps[i_index].inj_ae_rpm_enr[i]) / AERPMV_MAPS_M_FACTOR; //convert to %
 for (int i = 0; i < INJ_AE_RPM_LOOKUP_TABLE_SIZE; i++ )
  op_values[i+INJ_AE_RPM_LOOKUP_TABLE_SIZE] = ((float)p_maps[i_index].inj_ae_rpm_bins[i]) / AERPMB_MAPS_M_FACTOR;   //convert from (%*2)/100ms to %/s
}

void EEPROMDataMediator::SetAERPMMap(int i_index, const float* ip_values)
{
 ASSERT(ip_values);

 f_data_t* p_maps = (f_data_t*)(getBytes() + EEPROM_REALTIME_TABLES_START);

 for (int i = 0; i < INJ_AE_RPM_LOOKUP_TABLE_SIZE; i++ )
  p_maps[i_index].inj_ae_rpm_enr[i] = MathHelpers::Round((ip_values[i]*AERPMV_MAPS_M_FACTOR));
 for (int i = 0; i < INJ_AE_RPM_LOOKUP_TABLE_SIZE; i++ )
  p_maps[i_index].inj_ae_rpm_bins[i] = MathHelpers::Round((ip_values[i+INJ_AE_RPM_LOOKUP_TABLE_SIZE]*AERPMB_MAPS_M_FACTOR));
}

void EEPROMDataMediator::GetAftstrMap(int i_index, float* op_values, bool i_original /* = false */)
{
 ASSERT(op_values);

 //получаем адрес начала таблиц семейств характеристик
 f_data_t* p_maps = (f_data_t*)(getBytes(i_original) + EEPROM_REALTIME_TABLES_START);

 for (int i = 0; i < INJ_AFTSTR_LOOKUP_TABLE_SIZE; i++ )
  op_values[i] = ((float)p_maps[i_index].inj_aftstr[i]) / AFTSTR_MAPS_M_FACTOR;
}

void EEPROMDataMediator::SetAftstrMap(int i_index,const float* ip_values)
{
 ASSERT(ip_values);

 f_data_t* p_maps = (f_data_t*)(getBytes() + EEPROM_REALTIME_TABLES_START);

 for (int i = 0; i < INJ_AFTSTR_LOOKUP_TABLE_SIZE; i++ )
  p_maps[i_index].inj_aftstr[i] = MathHelpers::Round((ip_values[i]*AFTSTR_MAPS_M_FACTOR));
}

void EEPROMDataMediator::GetITMap(int i_index, float* op_values, bool i_original /*= false*/)
{
 ASSERT(op_values);

 //получаем адрес начала таблиц семейств характеристик
 f_data_t* p_maps = (f_data_t*)(getBytes(i_original) + EEPROM_REALTIME_TABLES_START);

 for (int i = 0; i < (INJ_VE_POINTS_F * INJ_VE_POINTS_L); i++ )
 {
  _uchar *p = &(p_maps[i_index].inj_timing[0][0]);
  op_values[i] = ((float)w12GetCell(p, i)) / IT_MAPS_M_FACTOR;
 }
}

void EEPROMDataMediator::SetITMap(int i_index, const float* ip_values)
{
 ASSERT(ip_values);

 f_data_t* p_maps = (f_data_t*)(getBytes() + EEPROM_REALTIME_TABLES_START);

 for (int i = 0; i < (INJ_VE_POINTS_F * INJ_VE_POINTS_L); i++ )
 {
  _uchar *p = &(p_maps[i_index].inj_timing[0][0]);
  w12SetCell(p, i, MathHelpers::Round(ip_values[i] * IT_MAPS_M_FACTOR));
 }
}

void EEPROMDataMediator::GetITRPMMap(int i_index, float* op_values, bool i_original /*= false*/)
{
 ASSERT(op_values);

 //gets address of the sets of maps
 f_data_t* p_maps = (f_data_t*)(getBytes(i_original) + EEPROM_REALTIME_TABLES_START);

 for (int i = 0; i < INJ_TARGET_RPM_TABLE_SIZE; i++ )
  op_values[i] = ((float)p_maps[i_index].inj_target_rpm[i]) * 10;
}

void EEPROMDataMediator::SetITRPMMap(int i_index, const float* ip_values)
{
 ASSERT(ip_values);

 f_data_t* p_maps = (f_data_t*)(getBytes() + EEPROM_REALTIME_TABLES_START);

 for (int i = 0; i < INJ_TARGET_RPM_TABLE_SIZE; i++ )
  p_maps[i_index].inj_target_rpm[i] = MathHelpers::Round((ip_values[i] / 10.0));
}

void EEPROMDataMediator::GetRigidMap(int i_index, float* op_values, bool i_original /*= false*/)
{
 ASSERT(op_values);

 //gets address of the sets of maps
 f_data_t* p_maps = (f_data_t*)(getBytes(i_original) + EEPROM_REALTIME_TABLES_START);

 for (int i = 0; i < INJ_IDL_RIGIDITY_TABLE_SIZE; i++ )
  op_values[i] = ((float)p_maps[i_index].inj_idl_rigidity[i]) / 128.0f;
}

void EEPROMDataMediator::SetRigidMap(int i_index, const float* ip_values)
{
 ASSERT(ip_values);

 f_data_t* p_maps = (f_data_t*)(getBytes() + EEPROM_REALTIME_TABLES_START);

 for (int i = 0; i < INJ_IDL_RIGIDITY_TABLE_SIZE; i++ )
  p_maps[i_index].inj_idl_rigidity[i] = MathHelpers::Round((ip_values[i] * 128.0f));
}

void EEPROMDataMediator::GetEGOCurveMap(int i_index, float* op_values, bool i_original /*= false*/)
{
 ASSERT(op_values);

 //gets address of the sets of maps
 f_data_t* p_maps = (f_data_t*)(getBytes(i_original) + EEPROM_REALTIME_TABLES_START);

 int i = 0;
 for (; i < INJ_EGO_CURVE_SIZE; i++ )
 {
  float value = (float)p_maps[i_index].inj_ego_curve[i];
  if (0==value) value = 1; //prevent division by zero
  op_values[i] = (value / EGO_CURVE_M_FACTOR);
 }

 for (; i < INJ_EGO_CURVE_SIZE+2; i++ )
 {
  float value = (float)p_maps[i_index].inj_ego_curve[i];
  op_values[i] = value * ADC_DISCRETE;
 }
}

void EEPROMDataMediator::SetEGOCurveMap(int i_index, const float* ip_values)
{
 ASSERT(ip_values);

 f_data_t* p_maps = (f_data_t*)(getBytes() + EEPROM_REALTIME_TABLES_START);

 int i = 0;
 for (; i < INJ_EGO_CURVE_SIZE; i++ )
  p_maps[i_index].inj_ego_curve[i] = MathHelpers::Round(ip_values[i] * EGO_CURVE_M_FACTOR);
 for (; i < INJ_EGO_CURVE_SIZE+2; i++ )
  p_maps[i_index].inj_ego_curve[i] = MathHelpers::Round(ip_values[i] / ADC_DISCRETE);
}

std::vector<_TSTRING> EEPROMDataMediator::GetFunctionsSetNames(void)
{
 std::vector<_TSTRING> names(TUNABLE_TABLES_NUMBER);

 //получаем адрес начала таблиц семейств характеристик
 f_data_t* p_maps = (f_data_t*)(getBytes() + EEPROM_REALTIME_TABLES_START);

 for (int i = 0; i < TUNABLE_TABLES_NUMBER; i++)
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

void EEPROMDataMediator::SetFunctionsSetName(int i_index, _TSTRING i_new_name)
{
 if (i_index >= TUNABLE_TABLES_NUMBER)
  return;

 //получаем адрес начала таблиц семейств характеристик
 f_data_t* p_maps = (f_data_t*)(getBytes() + EEPROM_REALTIME_TABLES_START);

 //дополняем строку пробелами, так чтобы если длина строки меньше F_NAME_SIZE,
 //остальные символы были пробелами
 i_new_name+= _T("                ");

 char raw_string[256];
 memset(raw_string, 0, F_NAME_SIZE + 1);
 CharToOem(i_new_name.c_str(), raw_string);
 memcpy(p_maps[i_index].name, raw_string, F_NAME_SIZE);
}

void EEPROMDataMediator::SetEEFileName(const _TSTRING i_ee_file_name)
{
 m_ee_file_name = i_ee_file_name;
}

_TSTRING EEPROMDataMediator::GetEEFileName(void)
{
 return m_ee_file_name;
}

SECU3IO::params_t* EEPROMDataMediator::GetParamsPtr(void)
{
 return (params_t*)(getBytes() + EEPROM_PARAM_START);
}

EECUPlatform EEPROMDataMediator::GetPlatformId(void)
{
 return m_epp.m_platform_id;
}

//считает контрольную сумму и записывает результат по соответствующему адресу
//io_data - массив байтов EEPROM
void EEPROMDataMediator::CalculateAndPlaceParamsCRC(BYTE* iop_data)
{
 params_t* params = (params_t*)(iop_data + EEPROM_PARAM_START);
 _uint crc = crc16((BYTE*)params, sizeof(params_t) - sizeof(_uint)); //skip last two byte of CRC
 params->crc = crc;
}

void EEPROMDataMediator::CalculateAndPlaceParamsCRC(void)
{
 CalculateAndPlaceParamsCRC(m_bytes_active);
}

std::set<int> EEPROMDataMediator::GetCEErrorsList(void)
{
 //получаем адрес начала таблиц семейств характеристик
 DWORD errors = *((_uint*)(getBytes() + EEPROM_ECUERRORS_START));
 std::set<int> el;

 for(int i = 0; i < SECU3_CE_ERRCODES_COUNT; ++i)
 {
  if (CHECKBIT32(errors, i))
   el.insert(i);
 }
 return el;
}

void EEPROMDataMediator::ResetCEErrorsList(void)
{
 _uint* p_errors = (_uint*)(getBytes() + EEPROM_ECUERRORS_START);
 *p_errors = 0;
}

void EEPROMDataMediator::GetIACCorrWMap(int i_index, float* op_values, bool i_original /*= false*/)
{
 ASSERT(op_values);

 //gets address of the sets of maps
 f_data_t* p_maps = (f_data_t*)(getBytes(i_original) + EEPROM_REALTIME_TABLES_START);

 int i = 0;
 for (; i < INJ_IAC_CORR_W_SIZE; i++ )
 {
  float value = (float)p_maps[i_index].inj_iac_corr_w[i];
  op_values[i] = (value / 256.0f);
 }

 for (; i < INJ_IAC_CORR_W_SIZE+2; i++ )
 {
  float value = (float)p_maps[i_index].inj_iac_corr_w[i];
  op_values[i] = value / 2.0f;
 }
}

void EEPROMDataMediator::SetIACCorrWMap(int i_index, const float* ip_values)
{
 ASSERT(ip_values);

 //gets address of the sets of maps
 f_data_t* p_maps = (f_data_t*)(getBytes() + EEPROM_REALTIME_TABLES_START);

 int i = 0;
 for (; i < INJ_IAC_CORR_W_SIZE; i++ )
 {
  int value = MathHelpers::Round(ip_values[i] * 256.0f);
  p_maps[i_index].inj_iac_corr_w[i] = (value > 255) ? 255 : value;
 }
 for (; i < INJ_IAC_CORR_W_SIZE+2; i++ )
  p_maps[i_index].inj_iac_corr_w[i] = MathHelpers::Round(ip_values[i] * 2.0f);
}


void EEPROMDataMediator::GetIACCorrMap(int i_index, float* op_values, bool i_original /*= false*/)
{
 ASSERT(op_values);

 //gets address of the sets of maps
 f_data_t* p_maps = (f_data_t*)(getBytes(i_original) + EEPROM_REALTIME_TABLES_START);

 int i = 0;
 for (; i < INJ_IAC_CORR_SIZE; i++ )
 {
  float value = (float)p_maps[i_index].inj_iac_corr[i];
  op_values[i] = ((value + 8192) / 8192.0f);
 }

 for (; i < INJ_IAC_CORR_SIZE+2; i++ )
 {
  float value = (float)p_maps[i_index].inj_iac_corr[i];
  op_values[i] = value / 128.0f;
 }
}

void EEPROMDataMediator::SetIACCorrMap(int i_index, const float* ip_values)
{
 ASSERT(ip_values);

 //gets address of the sets of maps
 f_data_t* p_maps = (f_data_t*)(getBytes() + EEPROM_REALTIME_TABLES_START);

 int i = 0;
 for (; i < INJ_IAC_CORR_SIZE; i++ )
  p_maps[i_index].inj_iac_corr[i] = MathHelpers::Round((ip_values[i] - 1.0f) * 8192.0f);
 for (; i < INJ_IAC_CORR_SIZE+2; i++ )
  p_maps[i_index].inj_iac_corr[i] = MathHelpers::Round(ip_values[i] * 128.0f);
}

void EEPROMDataMediator::GetIATCLTMap(int i_index, float* op_values, bool i_original /*= false*/)
{
 ASSERT(op_values);

 //gets address of the sets of maps
 f_data_t* p_maps = (f_data_t*)(getBytes(i_original) + EEPROM_REALTIME_TABLES_START);

 int i = 0;
 for (; i < INJ_IATCLT_CORR_SIZE; i++ )
 {
  float value = (float)p_maps[i_index].inj_iatclt_corr[i];
  op_values[i] = (value / 8192.0f);
 }

 for (; i < INJ_IATCLT_CORR_SIZE+2; i++ )
 {
  float value = (float)p_maps[i_index].inj_iatclt_corr[i];
  op_values[i] = value * 32.0f;
 }
}

void EEPROMDataMediator::SetIATCLTMap(int i_index, const float* ip_values)
{
 ASSERT(ip_values);

 //gets address of the sets of maps
 f_data_t* p_maps = (f_data_t*)(getBytes() + EEPROM_REALTIME_TABLES_START);

 int i = 0;
 for (; i < INJ_IATCLT_CORR_SIZE; i++ )
  p_maps[i_index].inj_iatclt_corr[i] = MathHelpers::Round(ip_values[i] * 8192.0f);
 for (; i < INJ_IATCLT_CORR_SIZE+2; i++ )
  p_maps[i_index].inj_iatclt_corr[i] = MathHelpers::Round(ip_values[i] / 32.0f);
}

void EEPROMDataMediator::GetTpsswtMap(int i_index,float* op_values, bool i_original /* = false */)
{
 ASSERT(op_values);

 //gets address of the sets of maps
 f_data_t* p_maps = (f_data_t*)(getBytes(i_original) + EEPROM_REALTIME_TABLES_START);

 for (int i = 0; i < INJ_TPSSWT_SIZE; i++ )
  op_values[i] = ((float)p_maps[i_index].inj_tpsswt[i]) / TPSSWT_MAPS_M_FACTOR;
}

void EEPROMDataMediator::SetTpsswtMap(int i_index,const float* ip_values)
{
 ASSERT(ip_values);

 //gets address of the sets of maps
 f_data_t* p_maps = (f_data_t*)(getBytes() + EEPROM_REALTIME_TABLES_START);

 for (int i = 0; i < INJ_TPSSWT_SIZE; i++ )
  p_maps[i_index].inj_tpsswt[i] = MathHelpers::Round((ip_values[i]*TPSSWT_MAPS_M_FACTOR));
}

void EEPROMDataMediator::GetGtscMap(int i_index,float* op_values, bool i_original /* = false */)
{
 ASSERT(op_values);

 //gets address of the sets of maps
 f_data_t* p_maps = (f_data_t*)(getBytes(i_original) + EEPROM_REALTIME_TABLES_START);

 for (int i = 0; i < INJ_GTS_CORR_SIZE; i++ )
  op_values[i] = ((float)p_maps->inj_gts_corr[i]) / 128.0f;
}

void EEPROMDataMediator::SetGtscMap(int i_index,const float* ip_values)
{
 ASSERT(ip_values);

 //gets address of the sets of maps
 f_data_t* p_maps = (f_data_t*)(getBytes() + EEPROM_REALTIME_TABLES_START);

 for (int i = 0; i < INJ_GTS_CORR_SIZE; i++ )
  p_maps->inj_gts_corr[i] = MathHelpers::Round((ip_values[i]*128.0f));
}

void EEPROMDataMediator::GetGpscMap(int i_index, float* op_values, bool i_original /*= false*/)
{
 ASSERT(op_values);

 //gets address of the sets of maps
 f_data_t* p_maps = (f_data_t*)(getBytes(i_original) + EEPROM_REALTIME_TABLES_START);

 int i = 0;
 for (; i < INJ_GPS_CORR_SIZE; i++ )
 {
  float value = (float)p_maps->inj_gps_corr[i];
  op_values[i] = (value / 128.0f);
 }

 for (; i < INJ_GPS_CORR_SIZE+2; i++ )
 {
  float value = (float)p_maps->inj_gps_corr[i];
  op_values[i] = value * 2.0f;
 }
}

void EEPROMDataMediator::SetGpscMap(int i_index, const float* ip_values)
{
 ASSERT(ip_values);

 //gets address of the sets of maps
 f_data_t* p_maps = (f_data_t*)(getBytes() + EEPROM_REALTIME_TABLES_START);

 int i = 0;
 for (; i < INJ_GPS_CORR_SIZE; i++ )
  p_maps->inj_gps_corr[i] = MathHelpers::Round(ip_values[i] * 128.0f);
 for (; i < INJ_GPS_CORR_SIZE+2; i++ )
  p_maps->inj_gps_corr[i] = MathHelpers::Round(ip_values[i] / 2.0f);
}

void EEPROMDataMediator::GetAtscMap(int i_index,float* op_values, bool i_original /* = false */)
{
 ASSERT(op_values);

 //gets address of the sets of maps
 f_data_t* p_maps = (f_data_t*)(getBytes(i_original) + EEPROM_REALTIME_TABLES_START);

 for (int i = 0; i < INJ_ATS_CORR_SIZE; i++ )
  op_values[i] = ((float)p_maps->inj_ats_corr[i]) / 128.0f;
}

void EEPROMDataMediator::SetAtscMap(int i_index,const float* ip_values)
{
 ASSERT(ip_values);

 //gets address of the sets of maps
 f_data_t* p_maps = (f_data_t*)(getBytes() + EEPROM_REALTIME_TABLES_START);

 for (int i = 0; i < INJ_ATS_CORR_SIZE; i++ )
  p_maps->inj_ats_corr[i] = MathHelpers::Round((ip_values[i]*128.0f));
}

void EEPROMDataMediator::GetPwm1Map(int i_index, float* op_values, bool i_original /* = false*/)
{
 ASSERT(op_values);
 //gets address of the sets of maps
 f_data_t* p_maps = (f_data_t*)(getBytes() + EEPROM_REALTIME_TABLES_START);

 bool splitAng = GetSplitAngMode(i_index);
 for (int i = 0; i < (F_WRK_POINTS_F * F_WRK_POINTS_L); i++ )
 {
  _uchar *p = &(p_maps[i_index].pwm_duty1[0][0]);
  op_values[i] = splitAng ? ((float) *(p + i)) / AA_MAPS_M_FACTOR : (((float) *(p + i)) * 100.0f) / 255.0f;
 }
}

void EEPROMDataMediator::SetPwm1Map(int i_index, const float* ip_values)
{
 ASSERT(ip_values);
 //gets address of the sets of maps
 f_data_t* p_maps = (f_data_t*)(getBytes() + EEPROM_REALTIME_TABLES_START);

 bool splitAng = GetSplitAngMode(i_index);
 for (int i = 0; i < (F_WRK_POINTS_F * F_WRK_POINTS_L); i++ )
 {
  _uchar *p = &(p_maps[i_index].pwm_duty1[0][0]);
  *(p + i) = splitAng ? MathHelpers::Round((ip_values[i]*AA_MAPS_M_FACTOR)) : MathHelpers::Round((ip_values[i]*255.0f)/100.0f);
 }
}

void EEPROMDataMediator::GetPwm2Map(int i_index, float* op_values, bool i_original /* = false*/)
{
 ASSERT(op_values);
 //gets address of the sets of maps
 f_data_t* p_maps = (f_data_t*)(getBytes() + EEPROM_REALTIME_TABLES_START);

 for (int i = 0; i < (F_WRK_POINTS_F * F_WRK_POINTS_L); i++ )
 {
  _uchar *p = &(p_maps[i_index].pwm_duty2[0][0]);
  op_values[i] = (((float) *(p + i)) * 100.0f) / 255.0f;
 }
}

void EEPROMDataMediator::SetPwm2Map(int i_index, const float* ip_values)
{
 ASSERT(ip_values);
 //gets address of the sets of maps
 f_data_t* p_maps = (f_data_t*)(getBytes() + EEPROM_REALTIME_TABLES_START);

 for (int i = 0; i < (F_WRK_POINTS_F * F_WRK_POINTS_L); i++ )
 {
  _uchar *p = &(p_maps[i_index].pwm_duty2[0][0]);
  *(p + i) = MathHelpers::Round((ip_values[i]*255.0f)/100.0f);
 }
}

bool EEPROMDataMediator::GetSplitAngMode(int i_index)
{
 //gets address of the sets of maps
 f_data_t* p_maps = (f_data_t*)(getBytes() + EEPROM_REALTIME_TABLES_START);

 return (p_maps[i_index].map_mode != 0);
}

void EEPROMDataMediator::GetIACMATMap(int i_index,float* op_values, bool i_original /* = false */)
{
 ASSERT(op_values);

 //gets address of the sets of maps
 f_data_t* p_maps = (f_data_t*)(getBytes(i_original) + EEPROM_REALTIME_TABLES_START);

 for (int i = 0; i < INJ_ATS_CORR_SIZE; i++ )
  op_values[i] = ((float)p_maps->iac_mat_corr[i]) / 4.0f;
}

void EEPROMDataMediator::SetIACMATMap(int i_index,const float* ip_values)
{
 ASSERT(ip_values);

 //gets address of the sets of maps
 f_data_t* p_maps = (f_data_t*)(getBytes() + EEPROM_REALTIME_TABLES_START);

 for (int i = 0; i < INJ_ATS_CORR_SIZE; i++ )
  p_maps->iac_mat_corr[i] = MathHelpers::Round((ip_values[i]*4.0f));
}
