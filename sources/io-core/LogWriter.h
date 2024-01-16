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

/** \file LogWriter.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include <map>
#include "ControlAppAdapter.h"
#include "common/unicodesupport.h"
#include "iocore_api.h"

class IOCORE_API LogWriter : public IAPPEventHandler
{
 public:
  LogWriter(bool standalone = false);
  virtual ~LogWriter();

  //Активирует механизм записи. Если активация прошла неудачто, то возвращает false.
  //i_folder - полный путь к каталогу где будет находиться лог-файл.
  //o_full_file_name - переменная которая примет имя файла в который начнется запись
  bool BeginLogging(const _TSTRING& i_folder, _TSTRING* o_full_file_name = NULL);

  //Деактивирует механизм записи
  void EndLogging(void);

  //Возвращает true, если в текущий момент идет запись
  bool IsLoggingInProcess(void);

  //Separating symbol for CSV
  void SetSeparatingSymbol(char i_sep_symbol);

  //marks variable contains bits: 0, 1, 2, so possible values are 0-7
  bool InjectMarks(int marks);

  //Flushes file buffers
  void FlushFileBuffers(void);

  //Set name of the specified field
  void SetFieldName(int fieldId, const _TSTRING& value);

  //Get list log title's fields
  const std::map<int, _TSTRING>& GetFieldList(void) const;

  //Set list log title's fields
  void SetFieldList(const std::map<int, _TSTRING>& lff);

  //enebale/disable writing of fields
  void SetWriteFields(bool value);

  //Get handle of file which is currently used for writing of data
  FILE* GetFileHandle(void);

  //Log file format: false - text, true - binary
  void SetFormat(bool logfmt);

  //Write a record
  void SetRecord(SYSTEMTIME& i_time, SECU3IO::SensorDat& i_data, int& i_marks);

  //Set decimal point symbol for floating point numbers
  void SetDecimalPoint(char decpt);

  //Set title string, used only in the standalone mode
  void SetTitleStr(const std::string& str);

 public:

  //через эти обработчики данные поступают в механизм записи
  virtual void OnPacketReceived(const BYTE i_descriptor, SECU3IO::SECU3Packet* ip_packet);
  virtual void OnConnection(const bool i_online);

 private:
  int WriteTime(char* str, unsigned int wHour, unsigned int wMinute, unsigned int wSecond, unsigned int wMilliseconds);
  int WriteCE(char* str, DWORD value);

  std::map<int, _TSTRING> m_lff;

  //хэндл файла в который идет запись
  FILE* m_out_handle;

  //true - идет процесс записи, false - солдат спит, а служба идет.
  bool  m_is_busy;

  char m_decimal_point;
  char m_csv_separating_symbol;
  char *mp_recBuff;

  int m_pending_marks;
  bool m_writeFields;

  bool m_logFmt;
  bool m_standalone;
  std::string m_title_str;
};
