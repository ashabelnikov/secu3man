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
#include "LogWriter.h"
#include "ufcodes.h"
#include "SECU3IO.h"

using namespace SECU3IO;

const char cCSVTimeTemplateString[] = "%02d:%02d:%02d.%02d";
const char cCSVDataTemplateString[] = "%c %%05d%c%%6.2f%c %%6.2f%c %%5.2f%c %%6.2f%c %%4.2f%c %%5.2f%c %%02d%c %%01d%c %%01d%c %%01d%c %%01d%c %%01d%c %%01d%c %%01d%c %%5.1f%c %%6.3f%c %%6.3f%c %%5.1f%c %%5.1f%c %%7.2f%c %%6.2f%c %%6.2f%c %%6.2f%c %%6.2f%c %%6.2f%c %%6.2f%c %%6.2f%c %%6.2f%c %%6.2f%c %%6.2f%c %%01d%c %%s\r\n";

namespace {
void wordToString(WORD value, char* str)
{
 WORD z, i = 0;
 for (z = 32768; z > 0; z >>= 1)
  str[i++] = ((value & z) == z) ? '1' : '0';
 str[i] = '\0';
}
}

LogWriter::LogWriter()
: m_is_busy(false)
, m_out_handle(NULL)
, m_csv_separating_symbol(',')
, m_pending_marks(0)
{
 SetSeparatingSymbol(m_csv_separating_symbol);
}

LogWriter::~LogWriter()
{
 //завершаем процесс записи лога если он еще не закончен
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

  //используем ASCII версию, файл не должен быть юникодным
  //"hh:mm:ss.ms", ms - сотые доли секунды
  fprintf(m_out_handle, cCSVTimeTemplateString,time.wHour,time.wMinute,time.wSecond,time.wMilliseconds/10);

  //Convert binary to string
  char ce_errors[20];
  wordToString(p_sensors->ce_errors, ce_errors);

  fprintf(m_out_handle, m_csv_data_template,
                        p_sensors->frequen,
                        p_sensors->adv_angle,
                        p_sensors->pressure,
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
                        (int)0, //reserved bit
                        p_sensors->tps,
                        p_sensors->add_i1,
                        p_sensors->add_i2,
                        p_sensors->choke_pos,
                        p_sensors->speed,
                        p_sensors->distance,
                        p_sensors->add_i2_mode ? p_sensors->air_temp : 999.99f, //magic nubmer indicates that IAT is not used

                        p_sensors->strt_aalt,         // advance angle from start map
                        p_sensors->idle_aalt,         // advance angle from idle map
                        p_sensors->work_aalt,         // advance angle from work map
                        p_sensors->temp_aalt,         // advance angle from coolant temperature correction map
                        p_sensors->airt_aalt,         // advance angle from air temperature correction map
                        p_sensors->idlreg_aac,        // advance angle correction from idling RPM regulator
                        p_sensors->octan_aac,         // octane correction value
                        p_sensors->lambda_corr,       // lambda correction

                        p_sensors->inj_pw,            // injector pulse width

                        m_pending_marks,
                        ce_errors);
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
 string.Format(_T("%04d.%02d.%02d_%02d.%02d.%02d.csv"),time.wYear,time.wMonth,time.wDay,time.wHour,time.wMinute,time.wSecond);

 full_file_name += _T("\\");
 full_file_name += string;
 *o_full_file_name = full_file_name;

 m_out_handle = _tfopen(full_file_name.c_str(),_T("wb+"));
 if (NULL == m_out_handle)
  return false;

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
 sprintf (m_csv_data_template, cCSVDataTemplateString, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x);
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
