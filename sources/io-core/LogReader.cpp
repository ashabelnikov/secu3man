/****************************************************************
 *       SECU-3  - An open source, free engine control unit
 *    Designed by Alexey A. Shabelnikov. Ukraine, Gorlovka 2007.
 *       Microprocessors systems - design & programming.
 *    contacts:
 *              http://secu-3.narod.ru
 *              ICQ: 405-791-931
 ****************************************************************/

#include "stdafx.h"
#include "LogReader.h"

#include <stdio.h>
#include "SECU3IO.h"

using namespace SECU3IO;

//максимальный размер буфера необходимого для хранения строки одной записи
#define MAX_REC_BUF 80

//кол-во переменных в поле времени
#define CSV_COUNT_TIME_VAL 4

//кол-во переменных в поле данных
#define CSV_COUNT_DATA_VAL 11

//смещение данных относительно начала строки
#define CSV_TIME_PANE_LEN 11

//"hh:mm:ss.ms", ms - сотые доли секунды
const char cCSVTimeTemplateString[] = "%02d:%02d:%02d.%02d";
//данные
const char cCSVDataTemplateString[] = "%c%%d%c%%f%c%%f%c%%f%c%%f%c%%f%c%%d%c%%d%c%%d%c%%d%c%%d\r\n";

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

bool LogReader::GetRecord(SYSTEMTIME& o_time, SECU3IO::SensorDat *op_data)
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

int frequen,carb,gas,air_flow,ephh_valve,epm_valve = 0;                           
float pressure,voltage,temperat,adv_angle,knock_k; 

result = sscanf(string + CSV_TIME_PANE_LEN, m_csv_data_template,
		&frequen,
		&adv_angle,
		&pressure,
		&voltage,
		&temperat,
		&knock_k,
		&air_flow,
		&carb,
		&gas,
		&ephh_valve,
		&epm_valve);

if (result != CSV_COUNT_DATA_VAL)
 return false; 
op_data->frequen = frequen;
op_data->adv_angle = adv_angle;
op_data->pressure = pressure;
op_data->voltage = voltage;
op_data->temperat = temperat;
op_data->knock_k = knock_k;
op_data->air_flow = air_flow;
op_data->carb = carb;
op_data->gas = gas;
op_data->ephh_valve = ephh_valve;
op_data->epm_valve = epm_valve;

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
 sprintf (m_csv_data_template, cCSVDataTemplateString, x, x, x, x, x, x, x, x, x, x, x);
}

bool LogReader::IsNextPossible(void) const
{
 return (m_current_record + 1) < m_record_count;
}

bool LogReader::IsPrevPossible(void) const
{
 return m_current_record > 0;
}