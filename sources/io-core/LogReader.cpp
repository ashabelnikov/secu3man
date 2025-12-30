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

/** \file LogReader.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "LogReader.h"

#include <stdio.h>
#include <io.h>
#include <math.h>
#include <algorithm>
#include "SECU3IO.h"
#include "BitMask.h"
#include "s3lrecord.h"
#include "NumericConv.h"

using namespace SECU3IO;

//Attention! Please don't forget to check/update all following values

//maximum length of record including possible text header
#define MAX_REC_BUF 4096

//number of variables in the data field
#define CSV_COUNT_DATA_VAL 84

//offset of the marks value in record
#define CSV_MARKS_OFFSET 510

//offset of the CE flag's value in record
#define CSV_CE_OFFSET 97

LogReader::LogReader(bool standalone /*= false*/)
: m_file_handle(NULL)
, m_record_count(0)
, m_record_size(0)
, m_current_record(0)
, m_csv_separating_symbol(';')
, m_fileOffset(0)
, m_logFmt(false) //csv format
, m_standalone(standalone)
{
 mp_recBuff = new char[MAX_REC_BUF + 1];
 SetSeparatingSymbol(m_csv_separating_symbol);
}

LogReader::~LogReader()
{
 if (m_file_handle)
  fclose(m_file_handle);
 delete[] mp_recBuff;
}

bool LogReader::OpenFile(const _TSTRING& i_file_name, FileError& o_error, FILE* pending_handle, bool i_check /* = false*/)
{
 m_csvTitle.clear();

 FILE* f_handle = _tfopen(i_file_name.c_str(), _T("rb"));
 if (NULL == f_handle)
 {
  o_error = FE_OPEN;
  return false; //can not open file!
 }

 if (pending_handle != NULL)
 {
  if (_CompareFileHandles(f_handle, pending_handle) > 0)
  {
   o_error = FE_PENDING;
   fclose(f_handle);
   return false; //trying to open file which is equal to pending_handle
  }
 }

 m_file_handle = f_handle;
 m_current_record = 0;

 //get file size
 fseek(f_handle, 0L, SEEK_END);
 long int fsize = ftell(f_handle);
 fseek(f_handle, 0L, SEEK_SET);

 if (0 == fsize)
 { //file is empty
  o_error = FE_FORMAT;
  fclose(f_handle);
  m_file_handle = NULL;
  return false;
 }

 size_t pos = i_file_name.rfind(_T(".s3l"));
 if (pos!=_TSTRING::npos && pos == i_file_name.size()-4) //check file extension
 {//binary format (.s3l)
  m_record_size = sizeof(s3l::s3lRecord);
  m_record_count = fsize / m_record_size;
  o_error = FE_NA;
  m_logFmt = true;
  m_fileOffset = 0; //no title - no offset
 }
 else
 {//text format (.csv)
  m_logFmt = false;

  //check if first line of file is title (just search for alphabetic symbols)  
  fgets(mp_recBuff, MAX_REC_BUF, m_file_handle);
  fseek(m_file_handle, 0, SEEK_SET);
  char* p = mp_recBuff;
  int alpha_count = 0;
  while(*p && (p - mp_recBuff) < MAX_REC_BUF)
  {
   if (isalpha(*p))
    ++alpha_count;
   ++p;
  }
  if (alpha_count > 0)
  {
   m_csvTitle = mp_recBuff; //save string with title
   //remove \r\n values
   size_t first = m_csvTitle.find_last_not_of("\r\n");
   if (std::string::npos==first)
    m_csvTitle.clear();
   else
    m_csvTitle.erase(first + 1);
  }
  memset(mp_recBuff, 0, MAX_REC_BUF);
  fseek(m_file_handle, 0, SEEK_SET);

  //check content of the first line of file
  SYSTEMTIME o_time; SECU3IO::SensorDat o_data; int o_marks;
  bool result = GetRecord(o_time, o_data, o_marks);
  if (result && alpha_count == 0) 
  {
   fseek(m_file_handle, 0, SEEK_SET);
   m_fileOffset = 0;
  }
  else
  {//first line contains title, so we will just skip it
   fseek(m_file_handle, 0, SEEK_SET);
   int n = 0, c = 0;
   do 
   {
    c = fgetc(m_file_handle);
    n++;
    if (c == '\n')
     break;
   } while (c != EOF);
   m_fileOffset = n;
  }

  char string[MAX_REC_BUF + 1];
  //определяем кол-во строк в файле и проверяем чтобы они были одинаковой длины
  int prev_length = -1, length = 0;
  unsigned long count = 0;
  while(fgets(string, MAX_REC_BUF, f_handle) != NULL)
  {
   length = strlen(string);
   if (prev_length != -1 && length != prev_length)
   {
    o_error = FE_FORMAT;
    fclose(f_handle);
    m_file_handle = NULL;
    return false;
   }
   prev_length = length;
   ++count;
   if (!i_check && count > 1000) //check only first 1000 records if checking is not specified
    break;
  }

  if (fsize == m_fileOffset)
  { //empty file
   m_record_count = 0;
  }
  else
  {
   //save record count
   if (i_check)
   {
    m_record_count = (fsize - m_fileOffset) / (length);
    if (count != m_record_count)
    {
     o_error = FE_FORMAT;
     fclose(f_handle);
     m_file_handle = NULL;
     return false;
    }
   }
   else
   {
    m_record_count = (fsize - m_fileOffset) / (length);
   }
  }
  m_record_size = length;
  o_error = FE_NA;
  fseek(m_file_handle, 0, SEEK_SET);
 }
 return true;
}

bool LogReader::CloseFile(void)
{
 if (m_file_handle)
 {
  m_fileOffset = 0;
  m_record_count = 0;
  m_record_size = 0;
  m_current_record = 0;
  int result = fclose(m_file_handle);
  m_file_handle = NULL;
  return (result == 0);
 }
 else
  return false;
}

bool LogReader::IsOpened(void) const
{
 return (m_file_handle != NULL);
}

bool LogReader::GetRecord(SYSTEMTIME& o_time, SECU3IO::SensorDat& o_data, int& o_marks)
{
 if (0!=fseek(m_file_handle, m_fileOffset + (m_record_size*m_current_record), SEEK_SET))
  return false;

 unsigned int service_flags = 0;
 unsigned int uniout_flags = 0;

 if (m_logFmt)
 { //binary format
  s3l::s3lRecord s3l;
  size_t real_count = fread(&s3l, m_record_size, 1, m_file_handle);
  if (real_count != 1)
   return false;

  o_time.wHour = s3l.hh;
  o_time.wMinute = s3l.mm;
  o_time.wSecond = s3l.ss;
  o_time.wMilliseconds = s3l.ms;

  o_data.carb = CHECKBIT16(s3l.flags, 15);
  o_data.gas = CHECKBIT16(s3l.flags, 14);
  o_data.ephh_valve = CHECKBIT16(s3l.flags, 13);
  o_data.epm_valve = CHECKBIT16(s3l.flags, 12);
  o_data.cool_fan = CHECKBIT16(s3l.flags, 11);
  o_data.st_block = CHECKBIT16(s3l.flags, 10);
  o_data.acceleration = CHECKBIT16(s3l.flags, 9);
  o_data.fc_revlim = CHECKBIT16(s3l.flags, 8);
  o_data.floodclear = CHECKBIT16(s3l.flags, 7);
  o_data.sys_locked = CHECKBIT16(s3l.flags, 6);
  o_data.ce_state = CHECKBIT16(s3l.flags, 5);
  o_data.ign_i = CHECKBIT16(s3l.flags, 4);
  o_data.cond_i = CHECKBIT16(s3l.flags, 3);
  o_data.epas_i = CHECKBIT16(s3l.flags, 2);
  o_data.aftstr_enr = CHECKBIT16(s3l.flags, 1);
  o_data.iac_cl_loop = CHECKBIT16(s3l.flags, 0);
  o_data.gpa4_i = CHECKBIT16(s3l.flags1, 0);
  o_data.input1 = CHECKBIT16(s3l.flags1, 1);
  o_data.input2 = CHECKBIT16(s3l.flags1, 2);
  o_data.auto_i = CHECKBIT16(s3l.flags1, 3);
  o_data.mapsel0 = CHECKBIT16(s3l.flags1, 4);
  o_data.refprs_i = CHECKBIT16(s3l.flags1, 5);
  o_data.altrn_i = CHECKBIT16(s3l.flags1, 6);

  o_data.rpm = s3l.rpm;
  o_data.adv_angle = s3l.adv_angle;
  o_data.pressure = s3l.pressure;
  o_data.voltage = s3l.voltage;
  o_data.temperat = s3l.temperat;
  o_data.knock_k = s3l.knock_k;
  o_data.knock_retard = s3l.knock_retard;
  o_data.air_flow = s3l.air_flow;

  o_data.ce_errors = s3l.ce_errors;
  o_data.tps = s3l.tps;
  o_data.add_i1 = s3l.add_i1;
  o_data.add_i2 = s3l.add_i2;
  o_data.choke_pos = s3l.choke_pos;
  o_data.gasdose_pos = s3l.gasdose_pos;
  o_data.speed = s3l.speed;
  o_data.distance = s3l.distance;
  o_data.inj_ffd = s3l.inj_ffd;
  o_data.inj_fff = s3l.inj_fff;
  o_data.add_i2_mode = (fabs(s3l.air_temp - 999.99f) > 0.0001); //Comparing with epsilon
  o_data.air_temp = o_data.add_i2_mode ? s3l.air_temp : .0f;
  o_data.strt_aalt = s3l.strt_aalt;
  o_data.idle_aalt = s3l.idle_aalt;
  o_data.work_aalt = s3l.work_aalt;
  o_data.temp_aalt = s3l.temp_aalt;
  o_data.airt_aalt = s3l.airt_aalt;
  o_data.idlreg_aac = s3l.idlreg_aac;
  o_data.octan_aac = s3l.octan_aac;
  o_marks = s3l.log_marks;  //save log marks
  o_data.inj_pw = s3l.inj_pw;
  o_data.lambda_corr = s3l.lambda_corr;
  o_data.tpsdot = s3l.tpsdot;
  o_data.map2 = s3l.map2;
  o_data.tmp2 = s3l.tmp2;
  o_data.mapd = s3l.mapd;
  o_data.afr = s3l.afr;
  o_data.load = s3l.load;
  o_data.baro_press = s3l.baro_press;
  o_data.inj_tim_begin = s3l.inj_tim_begin;
  o_data.inj_tim_end = s3l.inj_tim_end;
  o_data.inj_ffh = m_standalone ? s3l.inj_fff : ((3600.0f * s3l.inj_fff) / ((float)m_fffConst));
  o_data.grts = s3l.grts;
  o_data.ftls = s3l.ftls;
  o_data.egts = s3l.egts;
  o_data.ops = s3l.ops;
  o_data.inj_duty = s3l.inj_duty;
  o_data.rigid_arg = s3l.rigid_arg;
  o_data.rxlaf = s3l.rxlaf;
  o_data.maf = s3l.maf;
  o_data.vent_duty = s3l.vent_duty;
  o_data.fts = s3l.fts;
  o_data.cons_fuel = s3l.cons_fuel;
  o_data.mapdot = s3l.mapdot;
  o_data.lambda_corr2 = s3l.lambda_corr2;
  o_data.afr2 = s3l.afr2;
  o_data.afrmap = s3l.afrmap;
  o_data.tchrg = s3l.tchrg;
  o_data.gps = s3l.gps;
  o_data.fps = s3l.fps;
  o_data.apps1 = s3l.apps1;
  o_data.ots = s3l.ots;
  uniout_flags = s3l.uniout_flags;
  service_flags = s3l.service_flags;
 }
 else
 {
  if (m_record_size == 0)
  {
   if (fgets(mp_recBuff, MAX_REC_BUF, m_file_handle) == NULL)
    return false;
  }
  else
  {
   size_t real_count = fread(mp_recBuff, sizeof(char), m_record_size, m_file_handle);
   if (real_count != m_record_size)
    return false;
   mp_recBuff[m_record_size] = 0; //terminate string
  }

  int result = false;
  //use ASCII version, file must not be in unicode
  unsigned int wHour, wMinute, wSecond, wMilliseconds;

  unsigned int rpm, air_flow, rxlaf = 0;
  unsigned int carb,gas,ephh_valve,epm_valve,cool_fan,st_block,acceleration,fc_revlim,floodclear,sys_locked,ce_state,ign_i,cond_i,epas_i,gpa4_i,input1, input2, auto_i, mapsel0, refprs_i, altrn_i, log_mark, aftstr_enr, iac_cl_loop = 0;
  int tpsdot = 0, mapdot = 0;
  float pressure,voltage,temperat,adv_angle,knock_k, knock_retard, tps, add_i1, add_i2, choke_pos, gasdose_pos;
  float strt_aalt, idle_aalt, work_aalt, temp_aalt, airt_aalt, idlreg_aac, octan_aac;
  float speed, distance, inj_ffd, inj_fff, air_temp, inj_pw, lambda_corr, map2, tmp2, mapd, afr, load, baro_press, inj_tim_begin, inj_tim_end;
  float grts, ftls, egts, ops, inj_duty, rigid_arg, maf, vent_duty, fts, cons_fuel, lambda_corr2, afr2, afrmap, tchrg, gps, fps, apps1, ots;
  DWORD ce_bits = 0;

  char* p = mp_recBuff; 
  char* b = mp_recBuff;
  int num = 0;  
  while(p != 0)
  {
   if (*p == m_csv_separating_symbol || *p == '\r' || *p == '\n')
   {
    while(*b==' ' && b != p) ++b; //skip spaces
    int size = p - b;
    if (size <= 0)
     return false; //An empty record

    switch(num)
    {
     case  0: result = ParseTime(b, size, wHour, wMinute, wSecond, wMilliseconds); break;
     case  1: result = CNumericConv::secu3_atoi_u32<5>(b, size, rpm); break;
     case  2: result = CNumericConv::secu3_atof_32<5>(b, size, adv_angle); break;
     case  3: result = CNumericConv::secu3_atof_32<6>(b, size, pressure); break;
     case  4: result = CNumericConv::secu3_atof_32<5>(b, size, voltage); break;
     case  5: result = CNumericConv::secu3_atof_32<6>(b, size, temperat); break;
     case  6: result = CNumericConv::secu3_atof_32<4>(b, size, knock_k); break;
     case  7: result = CNumericConv::secu3_atof_32<5>(b, size, knock_retard); break;
     case  8: result = CNumericConv::secu3_atoi_u32<2>(b, size, air_flow); break;
     case  9: result = CNumericConv::secu3_atoi_u1(b, size, carb); break;
     case 10: result = CNumericConv::secu3_atoi_u1(b, size, gas); break;
     case 11: result = CNumericConv::secu3_atoi_u1(b, size, ephh_valve); break;
     case 12: result = CNumericConv::secu3_atoi_u1(b, size, epm_valve); break;
     case 13: result = CNumericConv::secu3_atoi_u1(b, size, cool_fan); break;
     case 14: result = CNumericConv::secu3_atoi_u1(b, size, st_block); break;
     case 15: result = CNumericConv::secu3_atoi_u1(b, size, acceleration); break;
     case 16: result = CNumericConv::secu3_atoi_u1(b, size, fc_revlim); break;
     case 17: result = CNumericConv::secu3_atoi_u1(b, size, floodclear); break;
     case 18: result = CNumericConv::secu3_atoi_u1(b, size, sys_locked); break;
     case 19: result = CNumericConv::secu3_atoi_u1(b, size, ce_state); break;
     case 20: result = CNumericConv::secu3_atoi_u1(b, size, ign_i); break;
     case 21: result = CNumericConv::secu3_atoi_u1(b, size, cond_i); break;
     case 22: result = CNumericConv::secu3_atoi_u1(b, size, epas_i); break;
     case 23: result = CNumericConv::secu3_atoi_u1(b, size, gpa4_i); break;
     case 24: result = CNumericConv::secu3_atoi_u1(b, size, input1); break;
     case 25: result = CNumericConv::secu3_atoi_u1(b, size, input2); break;
     case 26: result = CNumericConv::secu3_atoi_u1(b, size, auto_i); break;
     case 27: result = CNumericConv::secu3_atoi_u1(b, size, mapsel0); break;
     case 28: result = CNumericConv::secu3_atoi_u1(b, size, refprs_i); break;
     case 29: result = CNumericConv::secu3_atoi_u1(b, size, altrn_i); break;
     case 30: result = CNumericConv::secu3_atoi_u1(b, size, aftstr_enr); break;
     case 31: result = CNumericConv::secu3_atoi_u1(b, size, iac_cl_loop); break;
     case 32: result = CNumericConv::secu3_atof_32<5>(b, size, tps); break;
     case 33: result = CNumericConv::secu3_atof_32<5>(b, size, add_i1); break;
     case 34: result = CNumericConv::secu3_atof_32<5>(b, size, add_i2); break;
     case 35: result = CNumericConv::secu3_atof_32<5>(b, size, choke_pos); break;
     case 36: result = CNumericConv::secu3_atof_32<5>(b, size, gasdose_pos); break;
     case 37: result = CNumericConv::secu3_atof_32<5>(b, size, speed); break;
     case 38: result = CNumericConv::secu3_atof_32<8>(b, size, distance); break;
     case 39: result = CNumericConv::secu3_atof_32<7>(b, size, inj_ffd); break;
     case 40: result = CNumericConv::secu3_atof_32<7>(b, size, inj_fff); break;
     case 41: result = CNumericConv::secu3_atof_32<7>(b, size, air_temp); break;
     case 42: result = CNumericConv::secu3_atof_32<5>(b, size, strt_aalt); break;
     case 43: result = CNumericConv::secu3_atof_32<5>(b, size, idle_aalt); break;
     case 44: result = CNumericConv::secu3_atof_32<5>(b, size, work_aalt); break;
     case 45: result = CNumericConv::secu3_atof_32<5>(b, size, temp_aalt); break;
     case 46: result = CNumericConv::secu3_atof_32<5>(b, size, airt_aalt); break;
     case 47: result = CNumericConv::secu3_atof_32<5>(b, size, idlreg_aac); break;
     case 48: result = CNumericConv::secu3_atof_32<5>(b, size, octan_aac); break;
     case 49: result = CNumericConv::secu3_atof_32<5>(b, size, lambda_corr); break;
     case 50: result = CNumericConv::secu3_atof_32<5>(b, size, inj_pw); break;
     case 51: result = CNumericConv::secu3_atoi_32<5>(b, size, tpsdot); break;
     case 52: result = CNumericConv::secu3_atof_32<6>(b, size, map2); break;
     case 53: result = CNumericConv::secu3_atof_32<6>(b, size, tmp2); break;
     case 54: result = CNumericConv::secu3_atof_32<6>(b, size, mapd); break;
     case 55: result = CNumericConv::secu3_atof_32<5>(b, size, afr); break;
     case 56: result = CNumericConv::secu3_atof_32<6>(b, size, load); break;
     case 57: result = CNumericConv::secu3_atof_32<6>(b, size, baro_press); break;
     case 58: result = CNumericConv::secu3_atof_32<5>(b, size, inj_tim_begin); break;
     case 59: result = CNumericConv::secu3_atof_32<5>(b, size, inj_tim_end); break;
     case 60: result = CNumericConv::secu3_atof_32<5>(b, size, grts); break;
     case 61: result = CNumericConv::secu3_atof_32<5>(b, size, ftls); break;
     case 62: result = CNumericConv::secu3_atof_32<6>(b, size, egts); break;
     case 63: result = CNumericConv::secu3_atof_32<5>(b, size, ops); break;
     case 64: result = CNumericConv::secu3_atof_32<5>(b, size, inj_duty); break;
     case 65: result = CNumericConv::secu3_atof_32<4>(b, size, rigid_arg); break;
     case 66: result = CNumericConv::secu3_atoi_u32<7>(b, size, rxlaf); break;
     case 67: result = CNumericConv::secu3_atof_32<6>(b, size, maf); break;
     case 68: result = CNumericConv::secu3_atof_32<5>(b, size, vent_duty); break;
     case 69: result = CNumericConv::secu3_atoi_u32<2>(b, size, uniout_flags); break;
     case 70: result = CNumericConv::secu3_atoi_32<5>(b, size, mapdot); break;
     case 71: result = CNumericConv::secu3_atof_32<5>(b, size, fts); break;
     case 72: result = CNumericConv::secu3_atof_32<8>(b, size, cons_fuel); break;
     case 73: result = CNumericConv::secu3_atof_32<5>(b, size, lambda_corr2); break;
     case 74: result = CNumericConv::secu3_atof_32<5>(b, size, afr2); break;
     case 75: result = CNumericConv::secu3_atof_32<5>(b, size, afrmap); break;
     case 76: result = CNumericConv::secu3_atof_32<5>(b, size, tchrg); break;
     case 77: result = CNumericConv::secu3_atof_32<6>(b, size, gps); break;
     case 78: result = CNumericConv::secu3_atof_32<7>(b, size, fps); break;
     case 79: result = CNumericConv::secu3_atof_32<5>(b, size, apps1); break;
     case 80: result = CNumericConv::secu3_atof_32<5>(b, size, ots); break;
     case 81: result = CNumericConv::secu3_atoi_u1(b, size, log_mark); break;
     case 82: result = CNumericConv::secu3_atoi_u32<5>(b, size, service_flags); break;
     case 83: result = ParseCE(b, size, ce_bits); break;
    }

    b+=size+1;

    if (!result) 
     return false;
    ++num;

    if (*p == '\r' || *p == '\n')
     break; //we have processed the last field
   } 
   ++p;
  }

 if (num != CSV_COUNT_DATA_VAL)
  return false;

 o_time.wHour = wHour;
 o_time.wMinute = wMinute;
 o_time.wSecond = wSecond;
 o_time.wMilliseconds = wMilliseconds * 10; //переводим из сотых в миллисекунды

 //Save all data fields
 o_data.rpm = rpm;
 o_data.adv_angle = adv_angle;
 o_data.pressure = pressure;
 o_data.voltage = voltage;
 o_data.temperat = temperat;
 o_data.knock_k = knock_k;
 o_data.knock_retard = knock_retard;
 o_data.air_flow = air_flow;
 o_data.carb = carb;
 o_data.gas = gas;
 o_data.ephh_valve = ephh_valve;
 o_data.epm_valve = epm_valve;
 o_data.cool_fan = cool_fan;
 o_data.st_block = st_block;
 o_data.ce_errors = ce_bits;
 o_data.tps = tps;
 o_data.add_i1 = add_i1;
 o_data.add_i2 = add_i2;
 o_data.choke_pos = choke_pos;
 o_data.gasdose_pos = gasdose_pos;
 o_data.speed = speed;
 o_data.distance = distance;
 o_data.inj_ffd = inj_ffd;
 o_data.inj_fff = inj_fff;
 o_data.add_i2_mode = (fabs(air_temp - 999.99f) > 0.0001); //Comparing with epsilon
 o_data.air_temp = o_data.add_i2_mode ? air_temp : .0f;
 o_data.strt_aalt = strt_aalt;
 o_data.idle_aalt = idle_aalt;
 o_data.work_aalt = work_aalt;
 o_data.temp_aalt = temp_aalt;
 o_data.airt_aalt = airt_aalt;
 o_data.idlreg_aac = idlreg_aac;
 o_data.octan_aac = octan_aac;
 o_marks = log_mark;  //save log marks
 o_data.inj_pw = inj_pw;
 o_data.lambda_corr = lambda_corr;
 o_data.lambda_corr2 = lambda_corr2;
 o_data.tpsdot = tpsdot;
 o_data.acceleration = acceleration;
 o_data.map2 = map2;
 o_data.tmp2 = tmp2;
 o_data.mapd = mapd;
 o_data.afr = afr;
 o_data.afr2 = afr2;
 o_data.fc_revlim = fc_revlim;
 o_data.floodclear = floodclear;
 o_data.sys_locked = sys_locked;
 o_data.ce_state = ce_state;
 o_data.load = load;
 o_data.baro_press = baro_press;
 o_data.inj_tim_begin = inj_tim_begin;
 o_data.inj_tim_end = inj_tim_end;
 o_data.ign_i = ign_i;
 o_data.cond_i = cond_i;
 o_data.epas_i = epas_i;
 o_data.gpa4_i = gpa4_i;
 o_data.input1 = input1;
 o_data.input2 = input2;
 o_data.auto_i = auto_i;
 o_data.mapsel0 = mapsel0;
 o_data.refprs_i = refprs_i;
 o_data.altrn_i = altrn_i;
 o_data.inj_ffh = m_standalone ? inj_fff : ((3600.0f * inj_fff) / ((float)m_fffConst));
 o_data.grts = grts;
 o_data.ftls = ftls;
 o_data.egts = egts;
 o_data.ops = ops;
 o_data.aftstr_enr = aftstr_enr;
 o_data.iac_cl_loop = iac_cl_loop;
 o_data.inj_duty = inj_duty;
 o_data.rigid_arg = rigid_arg;
 o_data.rxlaf = rxlaf;
 o_data.maf = maf;
 o_data.vent_duty = vent_duty;
 o_data.fts = fts;
 o_data.cons_fuel = cons_fuel;
 o_data.mapdot = mapdot;
 o_data.afrmap = afrmap;
 o_data.tchrg = tchrg;
 o_data.gps = gps;
 o_data.fps = fps;
 o_data.apps1 = apps1;
 o_data.ots = ots;
 }

 //universal outputs
 for(int i = 0; i < SECU3IO::UNI_OUTPUT_NUM; ++i)
  o_data.uniout[i] = CHECKBIT8((int)uniout_flags, i);

 //Encode service flags
 o_data.knkret_use = CHECKBIT32(service_flags, 0);
 o_data.strt_use =   CHECKBIT32(service_flags, 1);
 o_data.idle_use =   CHECKBIT32(service_flags, 2);
 o_data.work_use =   CHECKBIT32(service_flags, 3);
 o_data.temp_use =   CHECKBIT32(service_flags, 4);
 o_data.airt_use =   CHECKBIT32(service_flags, 5);
 o_data.idlreg_use = CHECKBIT32(service_flags, 6);
 o_data.octan_use =  CHECKBIT32(service_flags, 7);
 o_data.rigid_use =  CHECKBIT32(service_flags, 8);
 o_data.idlve_use =  CHECKBIT32(service_flags, 9);

 //all read without errors
 return true;
}

bool LogReader::Next(unsigned long num /*= 1*/)
{
 if (0==m_record_count || m_current_record >= (m_record_count-1))
  return false;
 m_current_record+=num;
 if (m_current_record >= m_record_count)
  m_current_record = m_record_count-1;
 return true;
}

bool LogReader::Prev(unsigned long num /*= 1*/)
{
 if (0==m_record_count || m_current_record == 0)
  return false;
 if (num > m_current_record)
  m_current_record = 0;
 else
  m_current_record-=num;
 return true;
}

unsigned long LogReader::GetCount(void) const
{
 return m_record_count;
}

void LogReader::SetSeparatingSymbol(char i_sep_symbol)
{
 m_csv_separating_symbol = i_sep_symbol;
}

bool LogReader::IsNextPossible(void) const
{
 return (m_current_record + 1) < m_record_count;
}

bool LogReader::IsPrevPossible(void) const
{
 return m_current_record > 0;
}

unsigned long LogReader::GetCurPos(void) const
{
 return m_current_record;
}

void LogReader::SetCurPos(unsigned long pos)
{
 if (0==m_record_count || pos >= (m_record_count-1))
  return;
 m_current_record = pos;
 VERIFY(!fseek(m_file_handle, m_fileOffset + (m_record_size*m_current_record), SEEK_SET));
}

int LogReader::_CompareFileHandles(FILE* f1, FILE* f2)
{
 HANDLE hand1 = ((HANDLE)_get_osfhandle(fileno(f1)));
 HANDLE hand2 = ((HANDLE)_get_osfhandle(fileno(f2)));
 BY_HANDLE_FILE_INFORMATION info1;
 BY_HANDLE_FILE_INFORMATION info2;
 BOOL res1 = GetFileInformationByHandle(hand1, &info1);
 BOOL res2 = GetFileInformationByHandle(hand2, &info2);

 //from MSDN:
 // The ReFS file system, introduced with Windows Server 2012, includes 128-bit file identifiers. 
 // To retrieve the 128-bit file identifier use the GetFileInformationByHandleEx function with 
 // FileIdInfo to retrieve the FILE_ID_INFO structure. The 64-bit identifier in this structure 
 // is not guaranteed to be unique on ReFS.
 return (info1.dwVolumeSerialNumber == info2.dwVolumeSerialNumber) && (info1.nFileIndexHigh == info2.nFileIndexHigh) && (info1.nFileIndexLow == info2.nFileIndexLow);
}

//-----------------------------------------------------------------------
void LogReader::SetFFFConst(int fffConst)
{
 m_fffConst = fffConst;
}

bool LogReader::GetMRecord(int &o_marks, bool &o_errors)
{
 if (m_logFmt)
 { //binary format
  s3l::s3lRecord s3l;
  size_t real_count = fread(&s3l, m_record_size, 1, m_file_handle);
  if (real_count != 1)
   return false;
  o_marks = s3l.log_marks;
  o_errors = CHECKBIT16(s3l.flags, 5); //ce_state
 }
 else
 { //text format
  size_t real_count = fread(mp_recBuff, sizeof(char), m_record_size, m_file_handle);
  if (real_count != m_record_size)
   return false;

  o_marks = mp_recBuff[CSV_MARKS_OFFSET] - 0x30;
  o_errors = mp_recBuff[CSV_CE_OFFSET] - 0x30;
 }
 return true;
}

bool LogReader::ParseCE(char* str, int size, DWORD& result)
{
 if (size != 32)
  return false;
 //Convert CE errors bits from string to binary
 DWORD ce_bits = 0;
 for(size_t i = 0; i < 32; ++i)
 {
  if (str[i] != '0' && str[i] != '1')
   return false; //error (wrong char)
  DWORD mask = 2147483648;
  ce_bits|= (str[i] == '1') ? (mask >> i) : 0;
 }

 result = ce_bits;
 return true;
}

bool LogReader::ParseTime(char* str, int size, unsigned int& wHour, unsigned int& wMinute, unsigned int& wSecond, unsigned int& wMilliseconds)
{
 if (size!=11 || str[2]!=':' || str[5]!=':')
  return false;

 if (!CNumericConv::secu3_atoi_u32<2>(str, 2, wHour))
  return false;
 str+=3;
 if (!CNumericConv::secu3_atoi_u32<2>(str, 2, wMinute))
  return false;
 str+=3;
 if (!CNumericConv::secu3_atoi_u32<2>(str, 2, wSecond))
  return false;

 CNumericConv::SetDecimalPoint(str[2]); //set decimal point before using numeric functions
 str+=3;

 if (!CNumericConv::secu3_atoi_u32<2>(str, 2, wMilliseconds))
  return false;

 return true;
}

const _TSTRING& LogReader::GetTitleStr(void) const
{
 return m_csvTitle;
}
