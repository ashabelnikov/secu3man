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
#include "common/ModuleName.h"
#include "BitMask.h"
#include "s3lrecord.h"
#include "NumericConv.h"

using namespace SECU3IO;

//maximum length of record including possible text header
#define MAX_REC_BUF 4096

LogWriter::LogWriter(bool standalone /*= false*/)
: m_is_busy(false)
, m_out_handle(NULL)
, m_csv_separating_symbol(';')
, m_pending_marks(0)
, m_writeFields(false)
, m_logFmt(false) //csv
, m_decimal_point('.')
, m_standalone(standalone)
{
 mp_recBuff = new char[MAX_REC_BUF + 1];
 SetSeparatingSymbol(m_csv_separating_symbol);
}

LogWriter::~LogWriter()
{
 //finish log writing process if it is not finished yet
 if (IsLoggingInProcess())
  EndLogging();
 delete[] mp_recBuff;
}

void LogWriter::SetRecord(SYSTEMTIME& i_time, SECU3IO::SensorDat& i_data, int& i_marks)
{
  //build service flags variable
  WORD service_flags = 0;
  WRITEBIT16(service_flags, 0, i_data.knkret_use);
  WRITEBIT16(service_flags, 1, i_data.strt_use);
  WRITEBIT16(service_flags, 2, i_data.idle_use);
  WRITEBIT16(service_flags, 3, i_data.work_use);
  WRITEBIT16(service_flags, 4, i_data.temp_use);
  WRITEBIT16(service_flags, 5, i_data.airt_use);
  WRITEBIT16(service_flags, 6, i_data.idlreg_use);
  WRITEBIT16(service_flags, 7, i_data.octan_use);
  WRITEBIT16(service_flags, 8, i_data.rigid_use);
  WRITEBIT16(service_flags, 9, i_data.idlve_use);

  int uniout_flags = 0;
  for(int i= 0; i < SECU3IO::UNI_OUTPUT_NUM; ++i)
   WRITEBIT8(uniout_flags, i, i_data.uniout[i]);

  if (m_logFmt)
  {
   WORD flags = 0;
   WRITEBIT16(flags, 15, i_data.carb);
   WRITEBIT16(flags, 14, i_data.gas);
   WRITEBIT16(flags, 13, i_data.ephh_valve);
   WRITEBIT16(flags, 12, i_data.epm_valve);
   WRITEBIT16(flags, 11, i_data.cool_fan);
   WRITEBIT16(flags, 10, i_data.st_block);
   WRITEBIT16(flags, 9,  i_data.acceleration);
   WRITEBIT16(flags, 8,  i_data.fc_revlim);
   WRITEBIT16(flags, 7,  i_data.floodclear);
   WRITEBIT16(flags, 6,  i_data.sys_locked);
   WRITEBIT16(flags, 5,  i_data.ce_state);
   WRITEBIT16(flags, 4,  i_data.ign_i);
   WRITEBIT16(flags, 3,  i_data.cond_i);
   WRITEBIT16(flags, 2,  i_data.epas_i);
   WRITEBIT16(flags, 1,  i_data.aftstr_enr);
   WRITEBIT16(flags, 0,  i_data.iac_cl_loop);

   WORD flags1 = 0;
   WRITEBIT16(flags1, 0, i_data.gpa4_i);
   WRITEBIT16(flags1, 1, i_data.input1);
   WRITEBIT16(flags1, 2, i_data.input2);
   WRITEBIT16(flags1, 3, i_data.auto_i);
   WRITEBIT16(flags1, 4, i_data.mapsel0);
   WRITEBIT16(flags1, 5, i_data.refprs_i);
   WRITEBIT16(flags1, 6, i_data.altrn_i);
                                            
   s3l::s3lRecord s3l = {
   (BYTE)i_time.wHour,
   (BYTE)i_time.wMinute,
   (BYTE)i_time.wSecond,
   i_time.wMilliseconds,
   i_data.rpm,
   i_data.adv_angle,
   i_data.pressure,
   i_data.voltage,
   i_data.temperat,
   i_data.knock_k,
   i_data.knock_retard,
   i_data.air_flow,
   flags,
   flags1,
   i_data.tps,
   i_data.add_i1,
   i_data.add_i2,
   i_data.choke_pos,
   i_data.gasdose_pos,
   i_data.speed,
   i_data.distance,
   MathHelpers::RestrictValue(i_data.inj_ffd, .0f, 999.999f),
   i_data.inj_fff,
   i_data.add_i2_mode ? i_data.air_temp : 999.99f, //magic number indicates that IAT is not used
   i_data.strt_aalt,         // advance angle from start map
   i_data.idle_aalt,         // advance angle from idle map
   i_data.work_aalt,         // advance angle from work map
   i_data.temp_aalt,         // advance angle from coolant temperature correction map
   i_data.airt_aalt,         // advance angle from air temperature correction map
   i_data.idlreg_aac,        // advance angle correction from idling RPM regulator
   i_data.octan_aac,         // octane correction value
   i_data.lambda_corr,       // lambda correction
   i_data.inj_pw,            // injector pulse width
   i_data.tpsdot,            // TPS dot (d%/dt)
   i_data.map2,              // MAP2
   i_data.tmp2,              // TMP2
   i_data.mapd,              // MAP2 - MAP
   i_data.afr,               // AFR from lambda sensor
   i_data.load,              // Load
   i_data.baro_press,        // Barometric pressure
   i_data.inj_tim_begin,     // phase of beginning of inj. pulse
   i_data.inj_tim_end,       // phase of the end of inj. pulse
   i_data.grts,              // gas reducer's temperature
   i_data.ftls,              // level of fuel in the tank
   i_data.egts,              // exhaust gas temperature
   i_data.ops,               // oil pressure
   i_data.inj_duty,          // inj. duty                        
   i_data.rigid_arg,         // IAC rigidity argument
   i_data.rxlaf,             // RxL air flow                       
   i_data.maf,               // mass air flow
   i_data.vent_duty,         // cooling fan's duty
   uniout_flags,                 // states of all universal outputs
   i_data.mapdot,            // MAP dot (dP/dt), kPa/s
   i_data.fts,               // FTS                        
   i_data.cons_fuel,         // fuel odometer
   i_data.lambda_corr2,      // lambda correction #2
   i_data.afr2,              // AFR from lambda sensor
   i_data.afrmap,            // value from AFR map
   i_data.tchrg,             // value of corrected MAT
   i_data.gps,               // gas pressure sensor
   i_data.fps,
   i_marks,
   service_flags,
   i_data.ce_errors};

   fwrite(&s3l, sizeof(s3l::s3lRecord), 1, m_out_handle);
  }
  else
  {
   CNumericConv::SetDecimalPoint(m_decimal_point); //set decimal point before using numeric functions

   char *p = mp_recBuff; int size, total = 0;

   #define secu3_time_stamp(wHour, wMinute, wSecond, wMilliseconds) size = WriteTime(p, wHour, wMinute, wSecond, (wMilliseconds)/10), \
                                                              total+=size, p+=size, *p++ = m_csv_separating_symbol, ++total;

   #define secu3_itoa_u1(value, sz) size = CNumericConv::secu3_itoa_u1<sz>(p, value), \
                                                              total+=size, p+=size, *p++ = m_csv_separating_symbol, ++total;

   #define secu3_itoa_u32(value, sz) size = CNumericConv::secu3_itoa_u32<sz>(p, value), \
                                                              total+=size, p+=size, *p++ = m_csv_separating_symbol, ++total;

   #define secu3_itoa_32(value, sz) size = CNumericConv::secu3_itoa_32<sz>(p, value), \
                                                              total+=size, p+=size, *p++ = m_csv_separating_symbol, ++total;

   #define secu3_ftoa_32(value, sz, dp) size = CNumericConv::secu3_ftoa_32<sz>(p, value, dp), \
                                                              total+=size, p+=size, *p++ = m_csv_separating_symbol, ++total;


   secu3_time_stamp(i_time.wHour, i_time.wMinute, i_time.wSecond, i_time.wMilliseconds);
   secu3_itoa_u32(i_data.rpm, 6);
   secu3_ftoa_32 (i_data.adv_angle, 6, 2);
   secu3_ftoa_32 (i_data.pressure, 7, 2);
   secu3_ftoa_32 (i_data.voltage, 6, 2);
   secu3_ftoa_32 (i_data.temperat, 7, 2);
   secu3_ftoa_32 (i_data.knock_k, 5, 2);
   secu3_ftoa_32 (i_data.knock_retard, 6, 2);
   secu3_itoa_u32(i_data.air_flow, 3);
   secu3_itoa_u1 (i_data.carb, 2);
   secu3_itoa_u1 (i_data.gas, 2);
   secu3_itoa_u1 (i_data.ephh_valve, 2);
   secu3_itoa_u1 (i_data.epm_valve, 2);
   secu3_itoa_u1 (i_data.cool_fan, 2);
   secu3_itoa_u1 (i_data.st_block, 2);
   secu3_itoa_u1 (i_data.acceleration, 2);
   secu3_itoa_u1 (i_data.fc_revlim, 2);
   secu3_itoa_u1 (i_data.floodclear, 2);
   secu3_itoa_u1 (i_data.sys_locked, 2);
   secu3_itoa_u1 (i_data.ce_state, 2);
   secu3_itoa_u1 (i_data.ign_i, 2);
   secu3_itoa_u1 (i_data.cond_i, 2);
   secu3_itoa_u1 (i_data.epas_i, 2);
   secu3_itoa_u1 (i_data.gpa4_i, 2);
   secu3_itoa_u1 (i_data.input1, 2);
   secu3_itoa_u1 (i_data.input2, 2);
   secu3_itoa_u1 (i_data.auto_i, 2);
   secu3_itoa_u1 (i_data.mapsel0, 2);
   secu3_itoa_u1 (i_data.refprs_i, 2);
   secu3_itoa_u1 (i_data.altrn_i, 2);
   secu3_itoa_u1 (i_data.aftstr_enr, 2);
   secu3_itoa_u1 (i_data.iac_cl_loop, 2);
   secu3_ftoa_32 (i_data.tps, 6, 1);
   secu3_ftoa_32 (i_data.add_i1, 7, 3);
   secu3_ftoa_32 (i_data.add_i2, 7, 3);
   secu3_ftoa_32 (i_data.choke_pos, 6, 1);
   secu3_ftoa_32 (i_data.gasdose_pos, 6, 1);
   secu3_ftoa_32 (i_data.speed, 6, 1);
   secu3_ftoa_32 (i_data.distance, 8, 2);
   secu3_ftoa_32 (MathHelpers::RestrictValue(i_data.inj_ffd, .0f, 999.999f), 8, 3);
   secu3_ftoa_32 (i_data.inj_fff, 8, 3);
   secu3_ftoa_32 (i_data.add_i2_mode ? i_data.air_temp : 999.99f, 7, 2); //magic number indicates that IAT is not used
   secu3_ftoa_32 (i_data.strt_aalt, 7, 2);        // advance angle from start map
   secu3_ftoa_32 (i_data.idle_aalt, 7, 2);        // advance angle from idle map
   secu3_ftoa_32 (i_data.work_aalt, 7, 2);        // advance angle from work map
   secu3_ftoa_32 (i_data.temp_aalt, 7, 2);        // advance angle from coolant temperature correction map
   secu3_ftoa_32 (i_data.airt_aalt, 7, 2);        // advance angle from air temperature correction map
   secu3_ftoa_32 (i_data.idlreg_aac,7, 2);        // advance angle correction from idling RPM regulator
   secu3_ftoa_32 (i_data.octan_aac, 7, 2);        // octane correction value
   secu3_ftoa_32 (i_data.lambda_corr, 7, 2);      // lambda correction
   secu3_ftoa_32 (i_data.inj_pw, 7, 2);           // injector pulse width
   secu3_itoa_32 (i_data.tpsdot, 6);              // TPS dot (d%/dt)
   secu3_ftoa_32 (i_data.map2, 7, 2);             // MAP2 (barometer)
   secu3_ftoa_32 (i_data.tmp2, 7, 2);             // TMP2
   secu3_ftoa_32 (i_data.mapd, 8, 2);             // MAP2 - MAP
   secu3_ftoa_32 (i_data.afr,  6, 2);             // AFR from lambda sensor
   secu3_ftoa_32 (i_data.load, 7, 2);             // Load
   secu3_ftoa_32 (i_data.baro_press, 7, 2);       // Barometric pressure
   secu3_ftoa_32 (i_data.inj_tim_begin, 6, 1);    // phase of beginning of inj. pulse
   secu3_ftoa_32 (i_data.inj_tim_end, 6, 1);      // phase of the end of inj. pulse
   secu3_ftoa_32 (i_data.grts, 6, 1);             // gas reducer's temperature
   secu3_ftoa_32 (i_data.ftls, 6, 1);
   secu3_ftoa_32 (i_data.egts, 7, 1);
   secu3_ftoa_32 (i_data.ops, 5, 2);
   secu3_ftoa_32 (i_data.inj_duty, 6, 1);         // inj. duty                        
   secu3_ftoa_32 (i_data.rigid_arg, 5, 2);        // IAC rigidity argument
   secu3_itoa_u32(i_data.rxlaf, 8);               // RxL air flow                       
   secu3_ftoa_32 (i_data.maf, 7, 2);
   secu3_ftoa_32 (i_data.vent_duty, 6, 1);        // cooling fan's duty
   secu3_itoa_u32(uniout_flags, 3);
   secu3_itoa_32 (i_data.mapdot, 6);              // MAP dot (dP/dt), kPa/s
   secu3_ftoa_32 (i_data.fts, 6, 1);              // FTS                        
   secu3_ftoa_32 (i_data.cons_fuel, 10, 3);       // fuel odometer
   secu3_ftoa_32 (i_data.lambda_corr2, 7, 2);     // lambda correction #2
   secu3_ftoa_32 (i_data.afr2, 6, 2);             // AFR from lambda sensor #2
   secu3_ftoa_32 (i_data.afrmap, 6, 2);           // AFR from map
   secu3_ftoa_32 (i_data.tchrg, 6, 1);            // Tcharge            
   secu3_ftoa_32 (i_data.gps, 7, 2);              // gas pressure sensor
   secu3_ftoa_32 (i_data.fps, 8, 2);              // fuel pressure sensor
   secu3_itoa_u1 (i_marks, 2);                    // Log marks
   secu3_itoa_u32(service_flags, 6);              // Service flags
   size = WriteCE(p, i_data.ce_errors);           // CE errors
   total+=size, p+=size; 
   *p++ = '\r', *p++='\n', total+=2; //terminate line
   fwrite(mp_recBuff, total, 1, m_out_handle);
  }
}

void LogWriter::OnPacketReceived(const BYTE i_descriptor, SECU3IO::SECU3Packet* ip_packet)
{
 if (i_descriptor==SENSOR_DAT)
 {
  ASSERT(m_out_handle);
  SensorDat *p_sensors = reinterpret_cast<SensorDat*>(ip_packet);

  SYSTEMTIME time;
  ::GetLocalTime(&time);

  SetRecord(time, *p_sensors, m_pending_marks);
  m_pending_marks = 0; //reset after injection
 }
}

void LogWriter::OnConnection(const bool i_online)
{
 //empty
}

bool LogWriter::BeginLogging(const _TSTRING& i_folder, _TSTRING* o_full_file_name /* = NULL*/)
{
 if (m_standalone)
 { //standalone
  m_out_handle = _tfopen(i_folder.c_str(),_T("wb+"));
 }
 else
 {
  m_decimal_point = localeconv()->decimal_point[0]; //update decimal point's character, use ASCII version

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

  ModuleName::AddSlashToPath(full_file_name); //add '\' if absent
  full_file_name += string;
  *o_full_file_name = full_file_name;

  m_out_handle = _tfopen(full_file_name.c_str(),_T("wb+"));
 }

 if (NULL == m_out_handle)
  return false;

 //Write title fields into a first line of file, write fileds only when text format is selected!
 if (m_writeFields && false == m_logFmt)
 {
  if (m_title_str.size() && m_standalone)
  {
   fprintf(m_out_handle, "%s\r\n", m_title_str.c_str());
  }
  else
  {
   for(size_t i = 0; i < m_lff.size(); ++i)
   {
    if (i == (m_lff.size()-1))
     fprintf(m_out_handle, "%s\r\n", m_lff[i].c_str());
    else
     fprintf(m_out_handle, "%s%c", m_lff[i].c_str(), m_csv_separating_symbol);
   }
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
 m_csv_separating_symbol = i_sep_symbol;
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

int LogWriter::WriteTime(char* str, unsigned int wHour, unsigned int wMinute, unsigned int wSecond, unsigned int wMilliseconds)
{
 int total = 0;

 *str++ = CNumericConv::IntToStr[wHour][0];
 *str++ = CNumericConv::IntToStr[wHour][1];
 total+=2;
 *str++ = ':';

 *str++ = CNumericConv::IntToStr[wMinute][0];
 *str++ = CNumericConv::IntToStr[wMinute][1];
 total+=2;
 *str++ = ':';

 *str++ = CNumericConv::IntToStr[wSecond][0];
 *str++ = CNumericConv::IntToStr[wSecond][1];
 total+=2;
 *str++ = m_decimal_point;

 *str++ = CNumericConv::IntToStr[wMilliseconds][0];
 *str++ = CNumericConv::IntToStr[wMilliseconds][1];
 total+=2;

 return total + 3;
}

int LogWriter::WriteCE(char* str, DWORD value)
{
 *str++ = ' ';
 DWORD z, i = 0;
 for (z = 2147483648; z > 0; z >>= 1)
  str[i++] = ((value & z) == z) ? '1' : '0';
 str[i] = '\0';
 return 33;
}

void LogWriter::SetDecimalPoint(char decpt)
{
 m_decimal_point = decpt;
}

void LogWriter::SetTitleStr(const std::string& str)
{
 m_title_str = str;
}

const std::map<int, _TSTRING>& LogWriter::GetFieldList(void) const
{
 return m_lff;
}

void LogWriter::SetFieldList(const std::map<int, _TSTRING>& lff)
{
 m_lff = lff;
}
