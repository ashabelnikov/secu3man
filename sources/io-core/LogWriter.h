
#pragma once

#include "ControlAppAdapter.h"
#include "common/unicodesupport.h"


class AFX_EXT_CLASS LogWriter : public IAPPEventHandler
{
public:

 LogWriter();
 virtual ~LogWriter();

 //Активирует механизм записи. Если активация прошла неудачто, то возвращает false.
 //i_folder - полный путь к каталогу где будет находтся лог-файл.
 //o_full_file_name - переменная которая примет имя файла в который начнется запись
 bool BeginLogging(const _TSTRING& i_folder, _TSTRING* o_full_file_name = NULL);

 //Деактивирует механизм записи
 void EndLogging(void);

 //Возвращает true, если в текущий момент идет запись 
 bool IsLoggingInProcess(void);

 //Separating symbol for CSV 
 void SetSeparatingSymbol(char i_sep_symbol);

public:

  //через эти обработчики данные поступают в механизм записи
  virtual void OnPacketReceived(const BYTE i_descriptor, SECU3IO::SECU3Packet* ip_packet);          
  virtual void OnConnection(const bool i_online);

private:

  //хэндл файла в который идет запись
  FILE* m_out_handle;

  //true - идет процесс записи, false - солдат спит, а служба идет. 
  bool  m_is_busy;

  char m_csv_separating_symbol;
  char m_csv_data_template[1024];
};
