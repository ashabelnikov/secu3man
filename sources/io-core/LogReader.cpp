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
#include "SECU3IO.h"

using namespace SECU3IO;

//maximum length of record including possible text header
#define MAX_REC_BUF 4096

//кол-во переменных в поле времени
#define CSV_COUNT_TIME_VAL 4

//кол-во переменных в поле данных
#define CSV_COUNT_DATA_VAL 56

//смещение данных относительно начала строки
#define CSV_TIME_PANE_LEN 11

//"hh:mm:ss.ms", ms - сотые доли секунды
const char cCSVTimeTemplateString[] = "%02d:%02d:%02d.%02d";
//данные
const char cCSVDataTemplateString[] = "%c%%d%c%%f%c%%f%c%%f%c%%f%c%%f%c%%f%c%%d%c%%d%c%%d%c%%d%c%%d%c%%d%c%%d%c%%d%c%%d%c%%d%c%%d%c%%d%c%%d%c%%d%c%%d%c%%f%c%%f%c%%f%c%%f%c%%f%c%%f%c%%f%c%%f%c%%f%c%%f%c%%f%c%%f%c%%f%c%%f%c%%f%c%%f%c%%f%c%%f%c%%f%c%%d%c%%f%c%%f%c%%f%c%%f%c%%f%c%%f%c%%f%c%%f%c%%f%c%%f%c%%f%c%%f%c%%d%c%%s\r\n";

LogReader::LogReader()
: m_file_handle(NULL)
, m_record_count(0)
, m_record_size(0)
, m_current_record(0)
, m_csv_separating_symbol(',')
, m_fileOffset(0)
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

 //check content of the first line of file
 SYSTEMTIME o_time;
 SECU3IO::SensorDat o_data;
 int o_marks;
 bool result = GetRecord(o_time, o_data, o_marks);
 if (result) 
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

 fseek(f_handle, 0L, SEEK_END);
 long int fsize = ftell(f_handle);
 fseek(f_handle, 0L, SEEK_SET);

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
 m_record_size = length;
 o_error = FE_NA;
 fseek(m_file_handle, 0, SEEK_SET);
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
 VERIFY(!fseek(m_file_handle, m_fileOffset + (m_record_size*m_current_record), SEEK_SET));

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

 int result;
 //use ASCII version, file must not be in unicode
 int wHour, wMinute, wSecond, wMilliseconds;

 result = sscanf(mp_recBuff, cCSVTimeTemplateString, &wHour, &wMinute, &wSecond, &wMilliseconds);
 if (result != CSV_COUNT_TIME_VAL)
  return false;

 o_time.wHour = wHour;
 o_time.wMinute = wMinute;
 o_time.wSecond = wSecond;
 o_time.wMilliseconds = wMilliseconds * 10; //переводим из сотых в миллисекунды

 int frequen,carb,gas,air_flow,ephh_valve,epm_valve,cool_fan,st_block,acceleration,fc_revlim,floodclear,sys_locked,ce_state,ign_i,cond_i,epas_i,log_mark = 0;
 int tpsdot = 0;
 float pressure,voltage,temperat,adv_angle,knock_k, knock_retard, tps, add_i1, add_i2, choke_pos, gasdose_pos;
 float strt_aalt, idle_aalt, work_aalt, temp_aalt, airt_aalt, idlreg_aac, octan_aac;
 float speed, distance, inj_ffd, inj_fff, air_temp, inj_pw, lambda_corr, map2, tmp2, mapd, afr, load, baro_press, inj_tim_begin, inj_tim_end;
 float grts, ftls, egts, ops;
 char ce_errors[35] = {0};

 result = sscanf(mp_recBuff + CSV_TIME_PANE_LEN, m_csv_data_template,
                &frequen,
                &adv_angle,
                &pressure,
                &voltage,
                &temperat,
                &knock_k,
                &knock_retard,
                &air_flow,
                &carb,
                &gas,
                &ephh_valve,
                &epm_valve,
                &cool_fan,
                &st_block,
                &acceleration,
                &fc_revlim,
                &floodclear,
                &sys_locked,
                &ce_state,
                &ign_i,
                &cond_i,
                &epas_i,
                &tps,
                &add_i1,
                &add_i2,
                &choke_pos,
                &gasdose_pos,
                &speed,
                &distance,
                &inj_ffd,
                &inj_fff,
                &air_temp,
                &strt_aalt,
                &idle_aalt,
                &work_aalt,
                &temp_aalt,
                &airt_aalt,
                &idlreg_aac,
                &octan_aac,
                &lambda_corr,
                &inj_pw,
                &tpsdot,
                &map2,
                &tmp2,
                &mapd,
                &afr,
                &load,
                &baro_press,
                &inj_tim_begin,
                &inj_tim_end,
                &grts,
                &ftls,
                &egts,
                &ops,
                &log_mark,
                &ce_errors);

 if ((result != CSV_COUNT_DATA_VAL) || (strlen(ce_errors) != 32))
  return false;
 //Convert CE errors bits from string to binary
 DWORD ce_bits = 0;
 for(size_t i = 0; i < 32; ++i)
 {
  if (ce_errors[i] != '0' && ce_errors[i] != '1')
   return false; //error (wrong char)
  DWORD mask = 2147483648;
  ce_bits|= (ce_errors[i] == '1') ? (mask >> i) : 0;
 }
 //Save all data fields
 o_data.frequen = frequen;
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
 o_data.tpsdot = tpsdot;
 o_data.acceleration = acceleration;
 o_data.map2 = map2;
 o_data.tmp2 = tmp2;
 o_data.mapd = mapd;
 o_data.afr = afr;
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
 o_data.inj_ffh = (3600.0f * inj_fff) / ((float)m_fffConst);
 o_data.grts = grts;
 o_data.ftls = ftls;
 o_data.egts = egts;
 o_data.ops = ops;

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
 int x = m_csv_separating_symbol = i_sep_symbol;
 sprintf (m_csv_data_template, cCSVDataTemplateString, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x);
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
