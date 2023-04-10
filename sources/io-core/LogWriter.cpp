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

/** \file LogWriter.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "LogWriter.h"
#include "ufcodes.h"
#include "SECU3IO.h"
#include "common/MathHelpers.h"
#include "BitMask.h"
#include "s3lrecord.h"

using namespace SECU3IO;

const char cCSVTimeTemplateString[] = "%02d:%02d:%02d.%02d";                                                                                                                                                                          //6.3     //6.3
const char cCSVDataTemplateString[] = "%c %%05d%c%%6.2f%c %%6.2f%c %%5.2f%c %%6.2f%c %%4.2f%c %%5.2f%c %%02d%c %%01d%c %%01d%c %%01d%c %%01d%c %%01d%c %%01d%c %%01d%c %%01d%c %%01d%c %%01d%c %%01d%c %%01d%c %%01d%c %%01d%c %%01d%c %%01d%c %%5.1f%c %%6.3f%c %%6.3f%c %%5.1f%c %%5.1f%c %%5.1f%c %%7.2f%c %%7.3f%c %%7.3f%c %%6.2f%c %%6.2f%c %%6.2f%c %%6.2f%c %%6.2f%c %%6.2f%c %%6.2f%c %%6.2f%c %%6.2f%c %%6.2f%c %%05d%c %%6.2f%c %%6.2f%c %%7.2f%c %%5.2f%c %%6.2f%c %%6.2f%c %%5.1f%c %%5.1f%c %%5.1f%c %%5.1f%c %%6.1f%c %%4.2f%c %%5.1f%c %%4.2f%c %%07d%c %%6.2f%c %%5.1f%c %%02d%c %%05d%c %%5.1f%c %%9.3f%c %%01d%c %%05d%c %%s\r\n";

namespace {
void DwordToString(DWORD value, char* str)
{
 DWORD z, i = 0;
 for (z = 2147483648; z > 0; z >>= 1)
  str[i++] = ((value & z) == z) ? '1' : '0';
 str[i] = '\0';
}
}

LogWriter::LogWriter()
: m_is_busy(false)
, m_out_handle(NULL)
, m_csv_separating_symbol(',')
, m_pending_marks(0)
, m_writeFields(false)
, m_logFmt(false) //csv
{
 SetSeparatingSymbol(m_csv_separating_symbol);
}

LogWriter::~LogWriter()
{
 //finish log writing process if it is not finished yet
 if (IsLoggingInProcess())
  EndLogging();
}

void LogWriter::OnPacketReceived(const BYTE i_descriptor, SECU3IO::SECU3Packet* ip_packet)
{
 if (i_descriptor==SENSOR_DAT)
 {
  ASSERT(m_out_handle);
  SensorDat *p_sensors = reinterpret_cast<SensorDat*>(ip_packet);

  SYSTEMTIME time;
  ::GetLocalTime(&time);

  //build service flags variable
  WORD service_flags = 0;
  WRITEBIT16(service_flags, 0, p_sensors->knkret_use);
  WRITEBIT16(service_flags, 1, p_sensors->strt_use);
  WRITEBIT16(service_flags, 2, p_sensors->idle_use);
  WRITEBIT16(service_flags, 3, p_sensors->work_use);
  WRITEBIT16(service_flags, 4, p_sensors->temp_use);
  WRITEBIT16(service_flags, 5, p_sensors->airt_use);
  WRITEBIT16(service_flags, 6, p_sensors->idlreg_use);
  WRITEBIT16(service_flags, 7, p_sensors->octan_use);
  WRITEBIT16(service_flags, 8, p_sensors->rigid_use);

  int uniout_flags = 0;
  for(int i= 0; i < SECU3IO::UNI_OUTPUT_NUM; ++i)
   WRITEBIT8(uniout_flags, i, p_sensors->uniout[i]);

  if (m_logFmt)
  {
   WORD flags = 0;
   WRITEBIT16(flags, 15, p_sensors->carb);
   WRITEBIT16(flags, 14, p_sensors->gas);
   WRITEBIT16(flags, 13, p_sensors->ephh_valve);
   WRITEBIT16(flags, 12, p_sensors->epm_valve);
   WRITEBIT16(flags, 11, p_sensors->cool_fan);
   WRITEBIT16(flags, 10, p_sensors->st_block);
   WRITEBIT16(flags, 9,  p_sensors->acceleration);
   WRITEBIT16(flags, 8,  p_sensors->fc_revlim);
   WRITEBIT16(flags, 7,  p_sensors->floodclear);
   WRITEBIT16(flags, 6,  p_sensors->sys_locked);
   WRITEBIT16(flags, 5,  p_sensors->ce_state);
   WRITEBIT16(flags, 4,  p_sensors->ign_i);
   WRITEBIT16(flags, 3,  p_sensors->cond_i);
   WRITEBIT16(flags, 2,  p_sensors->epas_i);
   WRITEBIT16(flags, 1,  p_sensors->aftstr_enr);
   WRITEBIT16(flags, 0,  p_sensors->iac_cl_loop);
                                            
   s3l::s3lRecord s3l = {
   time.wHour,
   time.wMinute,
   time.wSecond,
   time.wMilliseconds,
   p_sensors->frequen,
   p_sensors->adv_angle,
   p_sensors->pressure,
   p_sensors->voltage,
   p_sensors->temperat,
   p_sensors->knock_k,
   p_sensors->knock_retard,
   p_sensors->air_flow,
   flags,
   p_sensors->tps,
   p_sensors->add_i1,
   p_sensors->add_i2,
   p_sensors->choke_pos,
   p_sensors->gasdose_pos,
   p_sensors->speed,
   p_sensors->distance,
   MathHelpers::RestrictValue(p_sensors->inj_ffd, .0f, 999.999f),
   p_sensors->inj_fff,
   p_sensors->add_i2_mode ? p_sensors->air_temp : 999.99f, //magic number indicates that IAT is not used
   p_sensors->strt_aalt,         // advance angle from start map
   p_sensors->idle_aalt,         // advance angle from idle map
   p_sensors->work_aalt,         // advance angle from work map
   p_sensors->temp_aalt,         // advance angle from coolant temperature correction map
   p_sensors->airt_aalt,         // advance angle from air temperature correction map
   p_sensors->idlreg_aac,        // advance angle correction from idling RPM regulator
   p_sensors->octan_aac,         // octane correction value
   p_sensors->lambda_corr,       // lambda correction
   p_sensors->inj_pw,            // injector pulse width
   p_sensors->tpsdot,            // TPS dot (d%/dt)
   p_sensors->map2,              // MAP2
   p_sensors->tmp2,              // TMP2
   p_sensors->mapd,              // MAP2 - MAP
   p_sensors->afr,               // AFR from lambda sensor
   p_sensors->load,              // Load
   p_sensors->baro_press,        // Barometric pressure
   p_sensors->inj_tim_begin,     // phase of beginning of inj. pulse
   p_sensors->inj_tim_end,       // phase of the end of inj. pulse
   p_sensors->grts,              // gas reducer's temperature
   p_sensors->ftls,
   p_sensors->egts,
   p_sensors->ops,
   p_sensors->inj_duty,          //inj. duty                        
   p_sensors->rigid_arg,         //IAC rigidity argument
   p_sensors->rxlaf,             //RxL air flow                       
   p_sensors->maf,
   p_sensors->vent_duty,         //cooling fan's duty
   uniout_flags,
   p_sensors->mapdot,            // MAP dot (dP/dt), kPa/s
   p_sensors->fts,               //FTS                        
   p_sensors->cons_fuel,         //fuel odometer
   m_pending_marks,
   service_flags,
   p_sensors->ce_errors };

   fwrite(&s3l, sizeof(s3l::s3lRecord), 1, m_out_handle);
  }
  else
  {
   //используем ASCII версию, файл не должен быть юникодным
   //"hh:mm:ss.ms", ms - сотые доли секунды
   fprintf(m_out_handle, cCSVTimeTemplateString,time.wHour,time.wMinute,time.wSecond,time.wMilliseconds/10);

   //Convert binary to string
   char ce_errors[35];
   DwordToString(p_sensors->ce_errors, ce_errors);

   fprintf(m_out_handle, m_csv_data_template,
                        p_sensors->frequen,
                        p_sensors->adv_angle,
                        p_sensors->pressure,          //MAP
                        p_sensors->voltage,
                        p_sensors->temperat,
                        p_sensors->knock_k,
                        p_sensors->knock_retard,
                        (int)p_sensors->air_flow,
                        (int)p_sensors->carb,
                        (int)p_sensors->gas,
                        (int)p_sensors->ephh_valve,
                        (int)p_sensors->epm_valve,
                        (int)p_sensors->cool_fan,
                        (int)p_sensors->st_block,
                        (int)p_sensors->acceleration,
                        (int)p_sensors->fc_revlim,
                        (int)p_sensors->floodclear,
                        (int)p_sensors->sys_locked,
                        (int)p_sensors->ce_state,
                        (int)p_sensors->ign_i,
                        (int)p_sensors->cond_i,
                        (int)p_sensors->epas_i,
                        (int)p_sensors->aftstr_enr,
                        (int)p_sensors->iac_cl_loop,
                        p_sensors->tps,
                        p_sensors->add_i1,
                        p_sensors->add_i2,
                        p_sensors->choke_pos,
                        p_sensors->gasdose_pos,
                        p_sensors->speed,
                        p_sensors->distance,
                        MathHelpers::RestrictValue(p_sensors->inj_ffd, .0f, 999.999f),
                        p_sensors->inj_fff,
                        p_sensors->add_i2_mode ? p_sensors->air_temp : 999.99f, //magic number indicates that IAT is not used

                        p_sensors->strt_aalt,         // advance angle from start map
                        p_sensors->idle_aalt,         // advance angle from idle map
                        p_sensors->work_aalt,         // advance angle from work map
                        p_sensors->temp_aalt,         // advance angle from coolant temperature correction map
                        p_sensors->airt_aalt,         // advance angle from air temperature correction map
                        p_sensors->idlreg_aac,        // advance angle correction from idling RPM regulator
                        p_sensors->octan_aac,         // octane correction value
                        p_sensors->lambda_corr,       // lambda correction

                        p_sensors->inj_pw,            // injector pulse width

                        p_sensors->tpsdot,            // TPS dot (d%/dt)

                        p_sensors->map2,              // MAP2
                        p_sensors->tmp2,              // TMP2
                        p_sensors->mapd,              // MAP2 - MAP

                        p_sensors->afr,               // AFR from lambda sensor
                        p_sensors->load,              // Load
                        p_sensors->baro_press,        // Barometric pressure
                        p_sensors->inj_tim_begin,     // phase of beginning of inj. pulse
                        p_sensors->inj_tim_end,       // phase of the end of inj. pulse

                        p_sensors->grts,              // gas reducer's temperature

                        p_sensors->ftls,
                        p_sensors->egts,
                        p_sensors->ops,

                        p_sensors->inj_duty,          //inj. duty                        
                        p_sensors->rigid_arg,         //IAC rigidity argument
                        p_sensors->rxlaf,             //RxL air flow
                       
                        p_sensors->maf,
                        p_sensors->vent_duty,         //cooling fan's duty
                        uniout_flags,

                        p_sensors->mapdot,            // MAP dot (dP/dt), kPa/s
                        p_sensors->fts,               //FTS
                        
                        p_sensors->cons_fuel,         //fuel odometer

                        m_pending_marks,
                        service_flags,
                        ce_errors);
  }
  m_pending_marks = 0; //reset after injection
 }
}

void LogWriter::OnConnection(const bool i_online)
{
 //empty
}


bool LogWriter::BeginLogging(const _TSTRING& i_folder, _TSTRING* o_full_file_name /* = NULL*/)
{
 //генерируем имя файла и открываем его
 SYSTEMTIME time;
 ::GetLocalTime(&time);

 _TSTRING full_file_name = i_folder;
 CString string;
 //yyyy.mm.dd_hh.mm.ss.csv
 if (m_logFmt)
  string.Format(_T("%04d.%02d.%02d_%02d.%02d.%02d.s3l"),time.wYear,time.wMonth,time.wDay,time.wHour,time.wMinute,time.wSecond); //binary format
 else
  string.Format(_T("%04d.%02d.%02d_%02d.%02d.%02d.csv"),time.wYear,time.wMonth,time.wDay,time.wHour,time.wMinute,time.wSecond); //text format

 full_file_name += _T("\\");
 full_file_name += string;
 *o_full_file_name = full_file_name;

 m_out_handle = _tfopen(full_file_name.c_str(),_T("wb+"));
 if (NULL == m_out_handle)
  return false;

 //Write title fields into a first line of file, write fileds only when text format is selected!
 if (m_writeFields && false == m_logFmt)
 {
  for(size_t i = 0; i < m_lff.size(); ++i)
  {
   if (i == (m_lff.size()-1))
    fprintf(m_out_handle, "%s\r\n", m_lff[i].c_str());
   else
    fprintf(m_out_handle, "%s%c", m_lff[i].c_str(), m_csv_separating_symbol);
  }
 }
  
 m_is_busy = true;
 return true;
}

void LogWriter::EndLogging(void)
{
 //закрываем файл
 if (m_out_handle)
 {
  fclose(m_out_handle);
 }
 else
  _ASSERTE(0);

 m_is_busy = false;
 m_out_handle = NULL;
}

bool LogWriter::IsLoggingInProcess(void)
{
 return m_is_busy;
}

void LogWriter::SetSeparatingSymbol(char i_sep_symbol)
{
 int x = m_csv_separating_symbol = i_sep_symbol;
 sprintf (m_csv_data_template, cCSVDataTemplateString, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x);
}

bool LogWriter::InjectMarks(int marks)
{
 if (!m_is_busy)
  return false;
 m_pending_marks|= (marks & 7); //value must not be above 7
 return true;
}

void LogWriter::FlushFileBuffers(void)
{
 if (m_out_handle)
  fflush(m_out_handle);
}

void LogWriter::SetFieldName(int fieldId, const _TSTRING& value)
{
 std::map<int, _TSTRING>::iterator it = m_lff.find(fieldId);
 if (it == m_lff.end())
  m_lff.insert(std::make_pair(fieldId, value));
 else
  m_lff[fieldId] = value;
}

void LogWriter::SetWriteFields(bool value)
{
 m_writeFields = value;
}

FILE* LogWriter::GetFileHandle(void)
{
 return m_out_handle;
}

void LogWriter::SetFormat(bool logfmt)
{
 m_logFmt = logfmt;
}
