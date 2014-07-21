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
#include "LogReader.h"

#include <stdio.h>
#include "SECU3IO.h"

using namespace SECU3IO;

//максимальный размер буфера необходимого для хранения строки одной записи
#define MAX_REC_BUF 236

//кол-во переменных в поле времени
#define CSV_COUNT_TIME_VAL 4

//кол-во переменных в поле данных
#define CSV_COUNT_DATA_VAL 33

//смещение данных относительно начала строки
#define CSV_TIME_PANE_LEN 11

//"hh:mm:ss.ms", ms - сотые доли секунды
const char cCSVTimeTemplateString[] = "%02d:%02d:%02d.%02d";
//данные
const char cCSVDataTemplateString[] = "%c%%d%c%%f%c%%f%c%%f%c%%f%c%%f%c%%f%c%%d%c%%d%c%%d%c%%d%c%%d%c%%d%c%%d%c%%d%c%%f%c%%f%c%%f%c%%f%c%%f%c%%f%c%%f%c%%f%c%%f%c%%f%c%%f%c%%f%c%%f%c%%f%c%%f%c%%f%c%%d%c%%s\r\n";

LogReader::LogReader()
: m_file_handle(NULL)
, m_record_count(0)
, m_record_size(0)
, m_current_record(0)
, m_csv_separating_symbol(',')
{
 SetSeparatingSymbol(m_csv_separating_symbol);
}

LogReader::~LogReader()
{
 if (m_file_handle)
  fclose(m_file_handle);
}

bool LogReader::OpenFile(const _TSTRING& i_file_name, FileError& o_error)
{
 FILE* f_handle = _tfopen(i_file_name.c_str(), _T("rb"));
 if (NULL == f_handle)
 {
  o_error = FE_OPEN;
  return false; //can not open file!
 }

 m_file_handle = f_handle;

 char string[MAX_REC_BUF + 1];
 //определяем кол-во строк в файле и проверяем чтобы они были одинаковой длины
 int prev_lengh = -1, lengh = 0;
 unsigned long count = 0;
 while(fgets(string, MAX_REC_BUF, f_handle) != NULL)
 {
  lengh = strlen(string);
  if (prev_lengh != -1 && lengh != prev_lengh)
  {
   o_error = FE_FORMAT;
   return false;
  }
  prev_lengh = lengh;
  ++count;
 }
 //save record count
 m_record_count = count;
 m_record_size = lengh;
 m_current_record = 0;
 o_error = FE_NA;
 fseek(m_file_handle, 0, SEEK_SET);
 return true;
}

bool LogReader::CloseFile(void)
{
 if (m_file_handle)
 {
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
 char string[MAX_REC_BUF + 1];

 VERIFY(!fseek(m_file_handle, m_record_size*m_current_record, SEEK_SET));

 size_t real_count = fread(string, sizeof(char), m_record_size, m_file_handle);
 if (real_count != m_record_size)
  return false;

 int result;
 //используем ASCII версию, файл не должен быть юникодным
 int wHour, wMinute, wSecond, wMilliseconds;

 result = sscanf(string, cCSVTimeTemplateString, &wHour, &wMinute, &wSecond, &wMilliseconds);
 if (result != CSV_COUNT_TIME_VAL)
  return false;

 o_time.wHour = wHour;
 o_time.wMinute = wMinute;
 o_time.wSecond = wSecond;
 o_time.wMilliseconds = wMilliseconds * 10; //переводим из сотых в миллисекунды

 int frequen,carb,gas,air_flow,ephh_valve,epm_valve,cool_fan,st_block,reserved,log_mark = 0;
 float pressure,voltage,temperat,adv_angle,knock_k, knock_retard, tps, add_i1, add_i2, choke_pos;
 float strt_aalt, idle_aalt, work_aalt, temp_aalt, airt_aalt, idlreg_aac, octan_aac;
 float speed, distance, air_temp, inj_pw, lambda_corr;
 char ce_errors[20] = {0};

 result = sscanf(string + CSV_TIME_PANE_LEN, m_csv_data_template,
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
                &reserved,  //reserved bit
                &tps,
                &add_i1,
                &add_i2,
                &choke_pos,
                &speed,
                &distance,
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
                &log_mark,
                &ce_errors);

 if ((result != CSV_COUNT_DATA_VAL) || (strlen(ce_errors) != 16))
  return false;
 //Convert CE errors bits from string to binary
 WORD ce_bits = 0;
 for(size_t i = 0; i < 16; ++i)
 {
  if (ce_errors[i] != '0' && ce_errors[i] != '1')
   return false; //error (wrong char)
  WORD mask = 32768;
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
 o_data.speed = speed;
 o_data.distance = distance;
 o_data.air_temp = air_temp;
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

 //все прочитано без ошибок
 return true;
}

bool LogReader::Next(void)
{
 unsigned long next = m_current_record + 1;
 if (next >= m_record_count)
  return false;
 m_current_record = next;
 /*VERIFY(!fseek(m_file_handle, m_record_size, SEEK_CUR));*/
 return true;
}

bool LogReader::Prev(void)
{
 if (m_current_record == 0)
  return false;
 --m_current_record;
 /*VERIFY(!fseek(m_file_handle, -((long)m_record_size), SEEK_CUR));*/
 return true;
}

unsigned long LogReader::GetCount(void) const
{
 return m_record_count;
}

void LogReader::SetSeparatingSymbol(char i_sep_symbol)
{
 int x = m_csv_separating_symbol = i_sep_symbol;
 sprintf (m_csv_data_template, cCSVDataTemplateString, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x);
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
