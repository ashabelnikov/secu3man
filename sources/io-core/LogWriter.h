
#pragma once

#include "ControlApp.h"
#include "common/unicodesupport.h"


class AFX_EXT_CLASS LogWriter : public IAPPEventHandler
{
public:
 LogWriter();
 virtual ~LogWriter();

 //јктивирует механизм записи. ≈сли активаци€ прошла неудачто, то возвращает false.
 //i_folder - полный путь к каталогу где будет находтс€ лог-файл.
 //o_full_file_name - переменна€ котора€ примет им€ файла в который начнетс€ запись
 bool BeginLogging(const _TSTRING& i_folder, _TSTRING* o_full_file_name = NULL);

 //ƒеактивирует механизм записи
 void EndLogging(void);

 //¬озвращает true, если в текущий момент идет запись 
 bool IsLoggingInProcess(void);

public:

  //через эти обработчики данные поступают в механизм записи
  virtual void OnPacketReceived(const BYTE i_descriptor, SECU3IO::SECU3Packet* ip_packet);
  virtual void OnConnection(const bool i_online);

private:
  FILE* m_out_handle;
  bool  m_is_busy;
};