/* SECU-3  - An open source, free engine control unit
   Copyright (C) 2007 Alexey A. Shabelnikov. Ukraine, Gorlovka

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
#include "BootLoader.h"
#include "ccomport.h"
#include "common/MathHelpers.h"
#include "NumericConv.h"

#define EEPROM_RD_BLOCKS 8   //количество блоков чтения EEPROM,число должно быть степенью двойки
#define EEPROM_WR_BLOCKS 16  //количество блоков записи EEPROM,число должно быть степенью двойки


#define FLASH_PAGE_SIZE (m_ppf.m_page_size)

//-----------------------------------------------------------------------
CBootLoader::CBootLoader()
: m_pEventHandler(NULL)
, m_p_port(NULL)
, m_hThread(NULL)
, m_ThreadId(0)
, m_hAwakeEvent(NULL)
, m_ErrorCode(0)
, m_ThreadBusy(false)
, m_is_thread_must_exit(false)
, m_work_state(false) //контроллер не работает
, m_uart_speed(CBR_9600)
, m_current_pending_data_index(0)
, mp_csection(NULL)
{
 mp_csection = new CSECTION;
 InitializeCriticalSection(GetSyncObject());
}

//-----------------------------------------------------------------------
CBootLoader::~CBootLoader()
{
 DeleteCriticalSection(GetSyncObject());
 delete mp_csection;
}

//-----------------------------------------------------------------------
//Вспомогательная функция для чтения одной страницы FLASH
//n_page - номер страницы для чтения
//o_buf  - буфер для помещения в него прочитаных данных (массив байтов)
//total_size,current - для обновления UI
bool CBootLoader::FLASH_ReadOnePage(int n_page, BYTE* o_buf, int total_size, int* current)
{
 BYTE t_buf[2048];
 BYTE t_byte = 0;
 int  block_size = FLASH_PAGE_SIZE * 2;
 int index = 0;

 t_buf[index++] = '!'; //!R
 t_buf[index++] = 'R';

 if (m_ppf.m_page_count <= 256) //NN
 {
  CNumericConv::Bin8ToHex(n_page, &t_buf[index]); //добавили номер страницы (t_buf[2],t_buf[3])
  index+=2;
 }
 else //need NNN instead of NN
 {
  CNumericConv::Bin12ToHex(n_page, &t_buf[index]); //добавили номер страницы (t_buf[2],t_buf[3],t_buf[4])
  index+=3;
 }

 t_buf[index++] = 0;  //завершили строку
 m_p_port->SendASCII((char*)t_buf);         //послали команду чтения страницы

 //теперь необходимо получить данные страницы
 if (!m_p_port->RecvByte(&t_byte))
 {
  m_ErrorCode = BL_ERROR_NOANSWER;
  return false; //нет смысла продолжать дальше
 }

 if (t_byte!='<')
 {
  m_ErrorCode = BL_ERROR_WRONG_DATA;
  return false; //нет смысла продолжать дальше
 }

 EventHandler_OnUpdateUI(m_opdata.opcode, total_size, ++(*current));  //1 байт получен

 if (!m_p_port->RecvBlock(t_buf, block_size))  //приняли очередную страницу
 {
  m_ErrorCode = BL_ERROR_NOANSWER;
  return false; //часть данных потеряна - нет смысла продолжать дальше
 }

 (*current)+=block_size;
 EventHandler_OnUpdateUI(m_opdata.opcode,total_size,*current); //очередная страница принята

 if (!CNumericConv::HexArrayToBin(t_buf, o_buf, FLASH_PAGE_SIZE))
 {
  m_ErrorCode = BL_ERROR_WRONG_DATA;
  return false;
 }

 if (!m_p_port->RecvBlock(t_buf,2))  //CS - два символа
 {
  m_ErrorCode = BL_ERROR_NOANSWER;
  return false;
 }

 *current+=2;
 EventHandler_OnUpdateUI(m_opdata.opcode, total_size, *current);  //страница принята

 if (!CNumericConv::Hex8ToBin(t_buf, &t_byte))   //t_buf -> symbol
 {
  m_ErrorCode = BL_ERROR_WRONG_DATA;
  return false;
 }

 if (CNumericConv::CheckSum_8_xor(o_buf, FLASH_PAGE_SIZE)!=t_byte) //проверяем контрольную сумму
 {
  m_ErrorCode = BL_ERROR_CHKSUM;
  return false; //контрольные суммы не совпадают
 }
 return true; //All are OK
}


//-----------------------------------------------------------------------
DWORD WINAPI CBootLoader::BackgroundProcess(LPVOID lpParameter)
{
 CBootLoader* p_boot = (CBootLoader*)lpParameter;
 CComPort* p_port = p_boot->m_p_port;
 int opcode = 0, i = 0, j = 0, k = 0;
 int block_size,total_size,current;
 BYTE symbol = 0;  //для принятия символа '<'
 BYTE raw[2048];   //хватит с запасом для любой меги
 BYTE t_buf[1024];
 BYTE* fw_buf = &p_boot->m_fw_buf[0];
#define FLASH_PAGE_SIZE_S (p_boot->m_ppf.m_page_size)
#define EEPROM_SIZE_S (p_boot->m_ppe.m_size)

 while(1)
 {
  WaitForSingleObject(p_boot->m_hAwakeEvent, INFINITE); //sleep until enters a new command

  if (p_boot->m_is_thread_must_exit)
   break;  //поступила команда завершения работы потока

  opcode = p_boot->m_opdata.opcode;
  switch(opcode)  //обработка команд
  {
   //=========================================================================================
   case BL_OP_READ_FLASH:    //чтение FLASH
   {
    if (p_boot->m_opdata.size==0)
    { //попытка выполнения безсмысленной операции
     ASSERT(0);
     break;
    }

    p_boot->m_ErrorCode  = 0;  //перед выполнением новой команды необходимо сбросить ошибки
    symbol     = 0;
    p_boot->EventHandler_OnBegin(p_boot->m_opdata.opcode,true);
    block_size = FLASH_PAGE_SIZE_S * 2;
    current    = 0;

    int end_size = p_boot->m_opdata.addr + p_boot->m_opdata.size;

    //сколько байтов нехватает от начала страницы до стартового адреса
    int bottom_offset = p_boot->m_opdata.addr % FLASH_PAGE_SIZE_S;

    /*//сколько байтов нехватает от конечного адреса до конца страницы
    int top_overhead  = (end_size % FLASH_PAGE_SIZE) ? FLASH_PAGE_SIZE - (end_size % FLASH_PAGE_SIZE) : 0;*/

    int page_start = (p_boot->m_opdata.addr / FLASH_PAGE_SIZE_S);
    int page_end = (end_size / FLASH_PAGE_SIZE_S) + ((end_size % FLASH_PAGE_SIZE_S)!=0) - 1;

    int count_of_pages = (page_end - page_start) + 1;

    total_size = count_of_pages * (block_size + 1 + 2);   //1 byte - '<' + 2 bytes - CS
    p_boot->EventHandler_OnUpdateUI(opcode,total_size,current);

    for(j = 0,i = page_start; i <= page_end; i++,j++) //цикл: количество страниц
    {
     if (false == p_boot->FLASH_ReadOnePage(i,fw_buf+(j*FLASH_PAGE_SIZE_S),total_size,&current))
      break; //ошибка!
    }//for

    //Временный буфер содержит прочитанные страницы (данные этих страниц обрамляют реальные данные),
    //теперь необходимо сохранить данные соответствующие конкретным адресам
    memcpy(p_boot->m_opdata.data,fw_buf + bottom_offset,p_boot->m_opdata.size);

    p_boot->EventHandler_OnEnd(p_boot->m_opdata.opcode,p_boot->Status());
    p_boot->m_opdata.opcode = 0;
   }
   break;

   //=========================================================================================
   case BL_OP_WRITE_FLASH:   //запись FLASH
    p_boot->m_ErrorCode  = 0;  //перед выполнением новой команды необходимо сбросить ошибки
    symbol = 0;
    p_boot->EventHandler_OnBegin(p_boot->m_opdata.opcode,true);
    block_size = FLASH_PAGE_SIZE_S * 2;
    current = 0;
    int incomplete_page_bytes;
    int total_page_number;

    incomplete_page_bytes = p_boot->m_opdata.size % FLASH_PAGE_SIZE_S; //количество байт в неполной странице (равен 0 если количество данных кратно размеру страницы)
    total_page_number = p_boot->m_opdata.size / FLASH_PAGE_SIZE_S;  //количество страниц

    if (incomplete_page_bytes > 0) //есть неполная страница
     ++total_page_number;               //учитываем так же незавершенную страницу

    total_size = total_page_number * (block_size + 1 + 2);   //1 byte - '<' + 2 bytes - CS

    if (incomplete_page_bytes > 0) //если не полная страница, то мы должны ее прочитать и объединить данные
    {
     total_size+= (block_size + 1 + 2);
    }

    p_boot->EventHandler_OnUpdateUI(opcode, total_size,current);

    for(i = 0; i < total_page_number; i++) //цикл: количество страниц
    {
     if ((i==total_page_number-1)&&(incomplete_page_bytes > 0))
     {//есть неполная страница и мы сейчас ее обрабатываем
      if (false == p_boot->FLASH_ReadOnePage(i, t_buf, total_size, &current))
       break;
      //объединяем байты из прочитанной страницы с исходными байтами
      memcpy(t_buf,p_boot->m_opdata.data+(i*FLASH_PAGE_SIZE_S), incomplete_page_bytes);
     }
     else
     { //неполной страницы нету или мы до нее еще не дошли: копируем полную страницу с исходными байтами
      memcpy(t_buf, p_boot->m_opdata.data+(i*FLASH_PAGE_SIZE_S), FLASH_PAGE_SIZE_S);
     }

     int index = 0;
     raw[index++] = '!';	//!P
     raw[index++] = 'P';
     if (p_boot->m_ppf.m_page_count <= 256)
     {
      CNumericConv::Bin8ToHex(i, &raw[index]); //append page number NN
      index+=2;
     }
     else //number of pages more than 256
     {
      CNumericConv::Bin12ToHex(i, &raw[index]); //append page number NNN
      index+=3;
     }

     raw[index++] = 0;
     p_port->SendASCII((char*)raw); //послали команду записи страницы

     //теперь необходимо подождать завершения стирания страницы (несколько мс)
     Sleep(FLASH_PG_ERASE_DELAY);

     //сконвертировали байты в HEX-символы (raw будет содержать HEX-символы)
     CNumericConv::BinToHexArray(t_buf, raw, FLASH_PAGE_SIZE_S);
     raw[block_size] = 0; //завершаем строку

     p_port->SendASCII((char*)raw); //послали данные страницы

     current+=block_size;
     p_boot->EventHandler_OnUpdateUI(opcode,total_size,current);  //была передана страница

     //теперь будем принимать сигнальный символ и два символа контрольной суммы
     //теперь необходимо получить данные
     if (!p_port->RecvByte(&symbol))
     {
      p_boot->m_ErrorCode = BL_ERROR_NOANSWER;
      break; //нет смысла продолжать дальше
     }

     if (symbol!='<')
     {
      p_boot->m_ErrorCode = BL_ERROR_WRONG_DATA;
      break; //нет смысла продолжать дальше
     }

     p_boot->EventHandler_OnUpdateUI(opcode, total_size, ++current);  //1 байт получен

     if (!p_port->RecvBlock(raw,2))  //CS
     {
      p_boot->m_ErrorCode = BL_ERROR_NOANSWER;
      break;
     }

     current+=2;
     p_boot->EventHandler_OnUpdateUI(opcode, total_size, current);   //2 байта получено

     if (!CNumericConv::Hex8ToBin(raw,&symbol))
     {
      p_boot->m_ErrorCode = BL_ERROR_WRONG_DATA;
      break;
     }

     if (CNumericConv::CheckSum_8_xor(t_buf, FLASH_PAGE_SIZE_S)!=symbol)
     {
      p_boot->m_ErrorCode = BL_ERROR_CHKSUM; //контрольная сумма не совпадает
      break;
     }

    }//for

    p_boot->EventHandler_OnEnd(p_boot->m_opdata.opcode,p_boot->Status());
    p_boot->m_opdata.opcode = 0;
    break;

   //=========================================================================================
   case BL_OP_READ_EEPROM:    //чтение EEPROM
    p_boot->m_ErrorCode = 0;  //перед выполнение новой команды необходимо сбросить ошибки
    symbol = 0;
    block_size = (EEPROM_SIZE_S*2)/EEPROM_RD_BLOCKS;
    total_size = (EEPROM_SIZE_S*2)+1+2;   //1 byte - '<' + 2 bytes - CS
    current = 0;

    p_boot->EventHandler_OnBegin(p_boot->m_opdata.opcode,true);
    p_boot->EventHandler_OnUpdateUI(opcode,total_size,current);
    p_port->SendASCII("!J"); //чтение
    if (!p_port->RecvByte(&symbol))
    {
     p_boot->m_ErrorCode = BL_ERROR_NOANSWER;
     goto finish_read_eeprom;  //ошибка принятия сигнального символа
    }
    if (symbol!='<')
    {
     p_boot->m_ErrorCode = BL_ERROR_WRONG_DATA;
     goto finish_read_eeprom;
    }

    p_boot->EventHandler_OnUpdateUI(opcode,total_size,++current);

    for(i = 0; i < EEPROM_RD_BLOCKS; i++) //принимаем данные по блокам
    {
     if (!p_port->RecvBlock(raw+(i*block_size),block_size))  //receive the data
     {
      p_boot->m_ErrorCode = BL_ERROR_NOANSWER;
      goto finish_read_eeprom;//часть данных потеряна - нет смысла продолжать дальше
     }
     current+=block_size;
     p_boot->EventHandler_OnUpdateUI(opcode,total_size,current);
    }//for

    if (!CNumericConv::HexArrayToBin(raw,p_boot->m_opdata.data, EEPROM_SIZE_S))
    {
     p_boot->m_ErrorCode = BL_ERROR_WRONG_DATA;
     goto finish_read_eeprom;
    }

    p_boot->EventHandler_OnUpdateUI(opcode,total_size,current);

    if (!p_port->RecvBlock(raw,2))  //CS
    {
     p_boot->m_ErrorCode = BL_ERROR_NOANSWER;
     goto finish_read_eeprom;
    }

    current+=2;
    p_boot->EventHandler_OnUpdateUI(opcode, total_size, current);

    if (!CNumericConv::Hex8ToBin(raw,&symbol))
    {
     p_boot->m_ErrorCode = BL_ERROR_WRONG_DATA;
     goto finish_read_eeprom;
    }

    if (CNumericConv::CheckSum_8_xor(p_boot->m_opdata.data,EEPROM_SIZE_S)!=symbol)
     p_boot->m_ErrorCode = BL_ERROR_CHKSUM;

finish_read_eeprom:
    p_boot->EventHandler_OnEnd(p_boot->m_opdata.opcode,p_boot->Status());
    p_boot->m_opdata.opcode = 0;
    break;

   //=========================================================================================
   case BL_OP_WRITE_EEPROM:    //запись EEPROM
    p_boot->m_ErrorCode = 0;   //перед выполнение новой команды необходимо сбросить ошибки
    symbol = 0;
    block_size = (EEPROM_SIZE_S*2)/EEPROM_WR_BLOCKS;
    total_size = (EEPROM_SIZE_S*2)+1+2;   //1 byte - '<' + 2 bytes - CS
    current = 0;

    p_boot->EventHandler_OnBegin(p_boot->m_opdata.opcode,true);
    p_boot->EventHandler_OnUpdateUI(opcode,total_size,current);
    p_port->SendASCII("!W");  //запись

    //сконвертировали байты в HEX-символы
    CNumericConv::BinToHexArray(p_boot->m_opdata.data, raw, EEPROM_SIZE_S);

     k = 0;
    for(i = 0; i < EEPROM_WR_BLOCKS; i++) //передаем данные по блокам
    {
     for(j = 0; j < block_size; j++) //цикл: на каждый байт по 2 символа
     {
      p_port->SendByte(raw[k]);
      Sleep(EEPROM_WR_DELAY_MULTIPLIER);
      k++;
     }
     current+=block_size;
     p_boot->EventHandler_OnUpdateUI(opcode,total_size,current);
    }//for

    //принимаем ответ
    if (!p_port->RecvByte(&symbol))
    {
     p_boot->m_ErrorCode = BL_ERROR_NOANSWER;
     goto finish_write_eeprom; //ошибка принятия сигнального символа
    }

    if (symbol!='<')  //символ не является сигнальным (хотя и был принят успешно)
    {
     p_boot->m_ErrorCode = BL_ERROR_WRONG_DATA;
     goto finish_write_eeprom;
    }

    p_boot->EventHandler_OnUpdateUI(opcode,total_size,++current);

    if (!p_port->RecvBlock(raw,2))  //CS
    {
     p_boot->m_ErrorCode = BL_ERROR_NOANSWER;
     goto finish_write_eeprom;
    }

    current+=2;
    p_boot->EventHandler_OnUpdateUI(opcode,total_size,current);

    if (!CNumericConv::Hex8ToBin(raw,&symbol))
    {
     p_boot->m_ErrorCode = BL_ERROR_WRONG_DATA;
     goto finish_write_eeprom;
    }

    if (CNumericConv::CheckSum_8_xor(p_boot->m_opdata.data, EEPROM_SIZE_S)!=symbol)
     p_boot->m_ErrorCode = BL_ERROR_CHKSUM;

finish_write_eeprom:
    p_boot->EventHandler_OnEnd(p_boot->m_opdata.opcode,p_boot->Status());
    p_boot->m_opdata.opcode = 0; //сброс кода операции
    break;

   //=========================================================================================
   case BL_OP_READ_SIGNATURE:  //чтение сигнатурной информации о бутлоадере
    p_boot->m_ErrorCode  = 0;
    symbol = 0;
    total_size = BL_SIGNATURE_STR_LEN+1; //1 byte - '<'
    current = 0;
    p_boot->EventHandler_OnBegin(p_boot->m_opdata.opcode, true);
    p_boot->EventHandler_OnUpdateUI(opcode, total_size, current);
    p_port->SendASCII("!I");
    if (!p_port->RecvByte(&symbol))
    {
     p_boot->m_ErrorCode = BL_ERROR_NOANSWER;
     goto finish_read_signature; //нет смысла продолжать дальше
    }
    if (symbol != '<')
    {
     p_boot->m_ErrorCode = BL_ERROR_WRONG_DATA;          
    }
    else
    {
     p_boot->EventHandler_OnUpdateUI(opcode,total_size,++current);

     if (!p_port->RecvBlock(p_boot->m_opdata.data,BL_SIGNATURE_STR_LEN))
      p_boot->m_ErrorCode = BL_ERROR_NOANSWER;

     current+=BL_SIGNATURE_STR_LEN;
     p_boot->EventHandler_OnUpdateUI(opcode,total_size,current);
    }
finish_read_signature:
    p_boot->EventHandler_OnEnd(p_boot->m_opdata.opcode,p_boot->Status());
    p_boot->m_opdata.opcode = 0;
    break;

   //=========================================================================================
   case BL_OP_EXIT:            //выход из загрузчика
    p_boot->m_ErrorCode = 0;
    symbol = 0;
    total_size = 1+1; //1 byte - '<', 1 byte - '@'
    current = 0;
    p_boot->EventHandler_OnBegin(p_boot->m_opdata.opcode, true);
    p_boot->EventHandler_OnUpdateUI(opcode, total_size, current);
    //посылаем команду
    p_port->SendASCII("!T");
    //принимаем и анализируем ответ
    if (!p_port->RecvByte(&symbol))
    {
     p_boot->m_ErrorCode = BL_ERROR_NOANSWER;
     goto finish_exit;
    }
    if (symbol!='<')
    {
     p_boot->m_ErrorCode = BL_ERROR_WRONG_DATA;
     goto finish_exit;
    }
    else
    {
     p_boot->EventHandler_OnUpdateUI(opcode,total_size,++current); //рапортуем о принятии первого байта
    }

    symbol = 0;
    //второй байт
    if (!p_port->RecvByte(&symbol))
    {
     p_boot->m_ErrorCode = BL_ERROR_NOANSWER;
     goto finish_exit;
    }
    if (symbol!='<')
    {
     p_boot->m_ErrorCode = BL_ERROR_WRONG_DATA;
    }
    else
    {
     p_boot->EventHandler_OnUpdateUI(opcode,total_size,++current); //рапортуем о принятии второго байта
    }

finish_exit:
    p_boot->EventHandler_OnEnd(p_boot->m_opdata.opcode,p_boot->Status());
    p_boot->m_opdata.opcode = 0;
    break;
  }//switch
  ResetEvent(p_boot->m_hAwakeEvent); //сбрасываем событие пробуждения потока - он опять войдет в спячку
  p_boot->m_ThreadBusy = false;      //operation is completed - thread is not busy

 }//while

 return 0;
}


//-----------------------------------------------------------------------
//каждому вызову Initialize должен соответствовать вызов этой функции
bool CBootLoader::Terminate(void)
{
 bool status = true;
 DWORD ExitCode;
 ExitCode = 0;

 m_is_thread_must_exit = true;
 SetEvent(m_hAwakeEvent);       //выводим поток из спячки - проснется и сразу завершится

 int i = 0;
 do  //а если он не хочет завершатся по хорошему, через некоторое время изнасилуем его...
 {
  if (i >= 20)
   break;
  Sleep(50);
  GetExitCodeThread(m_hThread,&ExitCode);
  ++i;
 }while(ExitCode == STILL_ACTIVE);

 if (!TerminateThread(m_hThread,0))
  status = false;

 if (!CloseHandle(m_hAwakeEvent))
  status = false;

 m_ThreadBusy = false;
 return status;
}

void CBootLoader::SetPlatformParameters(const PlatformParamHolder& i_pph)
{
 m_ppf = i_pph.GetFlashParameters();
 m_ppe = i_pph.GetEepromParameters();
 //realloc memory for new platform
 m_fw_buf.resize(m_ppf.m_total_size + 1, 0x00);
}

//-----------------------------------------------------------------------
//exception: xThread
bool CBootLoader::Initialize(CComPort* p_port, const DWORD uart_seed)
{
 if (!p_port)
  return false;
 m_p_port = p_port;

 m_hAwakeEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
 if (m_hAwakeEvent==NULL)
 {
  return false;        //event creation error
 }

 m_hThread = CreateThread(0,0,(LPTHREAD_START_ROUTINE)BackgroundProcess,(void*)this,0,&m_ThreadId);
 if (m_hThread==NULL)
 {
  throw xThread();
  return false;        //thread creation error
 }

 m_is_thread_must_exit = false;
 m_uart_speed = uart_seed;

 return true;
}

void CBootLoader::SetEventHandler(IBLDEventHandler* i_pEventHandler)
{
 m_pEventHandler = i_pEventHandler;
}

//-----------------------------------------------------------------------
//проверяет код команды на корректность
bool CBootLoader::IsOpcodeValid(const int opcode)
{
 switch(opcode) //сравнение с допустимыми кодами команд
 {
  case BL_OP_READ_FLASH:
  case BL_OP_WRITE_FLASH:
  case BL_OP_READ_EEPROM:
  case BL_OP_WRITE_EEPROM:
  case BL_OP_READ_SIGNATURE:
  case BL_OP_EXIT:
   return true;
  default:
   return false;
 }//switch
}


//-----------------------------------------------------------------------
//Запускает указанную операцию на выполнение. Эта функция не блокирует вызвавший ее поток, а только
//инициализирует данные, запускает выполнение операции и сразу возвращает управление.
// io_data - буфер для чтения/записи данных
// i_size  - размер данных
// i_addr  - адрес для чтения/записи
//Return: Операция запущена на выполнение успешно - true, иначе - false
bool CBootLoader::StartOperation(const int opcode,BYTE* io_data,int i_size, int i_addr /*= 0*/)
{
 if (false==m_work_state)
  return false;

 if (m_ThreadBusy)              //не закончена обработка предыдущей команды ?
 {
  m_pEventHandler->OnBegin(opcode, false);
  return false;                 //ошибка запуска на выполнение новой операци (команды)
 }

 if (!IsOpcodeValid(opcode))
  return false;                 //неправильный опкод

 m_ThreadBusy = true;           //set busy flag - since current moment thread is busy

 //сохраняем данные о область данных внутреннего потока
 m_opdata.opcode = opcode;
 m_opdata.data   = io_data;
 m_opdata.size   = i_size;
 m_opdata.addr   = i_addr;

 m_ErrorCode = 0;
 SetEvent(m_hAwakeEvent);       //выводим поток из спячки - настало время поработать :-)

 return true;
}

//-----------------------------------------------------------------------
void CBootLoader::SwitchOn(bool state, bool i_force_reinit /* = false*/)
{
 COMMTIMEOUTS timeouts;
 float ms_need_for_one_byte;

 if (m_work_state==state && false==i_force_reinit)
  return;

 //кол-во мс необходимое для приема/передачи одного байта
 ms_need_for_one_byte = CComPort::ms_need_for_one_byte_8N1(m_uart_speed);

 if (state)
 {
  //перед возобновлением работы необходимо установить параметры (сброс операции при ошибке и таймауты)
  m_p_port->Purge();

  m_p_port->AccessDCB().fAbortOnError = FALSE;     //прекращение операции при ошибке
  m_p_port->AccessDCB().BaudRate = m_uart_speed;   //для работы с бутлоадером своя скорость
  m_p_port->SetState();

  //теперь необходимо настроить таймауты (я нихрена так и не понял ничего в этих таймаутах)
  timeouts.ReadIntervalTimeout         = 200;
  timeouts.ReadTotalTimeoutMultiplier  = 200;
  timeouts.ReadTotalTimeoutConstant    = 200;
  timeouts.WriteTotalTimeoutConstant   = 200;
  timeouts.WriteTotalTimeoutMultiplier = 200;
  m_p_port->SetTimeouts(&timeouts);
  Sleep(MathHelpers::Round(ms_need_for_one_byte * 5));
 }
 else
 {
  Sleep(MathHelpers::Round(ms_need_for_one_byte * 5));
 }

 m_work_state = state;
}

bool CBootLoader::GetWorkState(void) const
{
 return m_work_state;
}

bool CBootLoader::IsIdle(void) const
{
 return (m_ThreadBusy) ? false : true;
}

HANDLE CBootLoader::GetThreadHandle(void) const
{
 return m_hThread;
}

DWORD CBootLoader::GetThreadId(void) const
{
 return m_ThreadId;
}

CComPort* CBootLoader::GetPortHandle(void) const
{
 return m_p_port;
}

int CBootLoader::GetLastError() const
{
 return m_ErrorCode;
}

int CBootLoader::Status(void) const
{
 return (m_ErrorCode==0) ? true : false;
}

//-----------------------------------------------------------------------
void CBootLoader::EventHandler_OnUpdateUI(const int i_opcode, const int i_total, const int i_current)
{
 ASSERT(m_pEventHandler);
 if (NULL==m_pEventHandler)
  return;

 ////////////////////////////////////////////////////////////////////////////
 __try
 {
  EnterCriticalSection();
  m_pending_data[m_current_pending_data_index].m_update_ui.Set(i_opcode,i_total,i_current);
 }
 __finally
 {
  LeaveCriticalSection();
 }
 ////////////////////////////////////////////////////////////////////////////

 //посылка сообщения
 m_pEventHandler->OnUpdateUI(&m_pending_data[m_current_pending_data_index].m_update_ui);
 //для следующего сообщения новый индекс
 m_current_pending_data_index++;
 if (m_current_pending_data_index >= PENDING_DATA_QUEUE_SIZE)
  m_current_pending_data_index = 0;
}

//-----------------------------------------------------------------------
void CBootLoader::EventHandler_OnBegin(const int i_opcode, const int i_status)
{ //заглушка
 ASSERT(m_pEventHandler);
 if (NULL==m_pEventHandler)
  return;
 m_pEventHandler->OnBegin(i_opcode,i_status);
}

//-----------------------------------------------------------------------
void CBootLoader::EventHandler_OnEnd(const int i_opcode, const int i_status)
{ //заглушка
 ASSERT(m_pEventHandler);
 if (NULL==m_pEventHandler)
  return;
 m_pEventHandler->OnEnd(i_opcode, i_status);
}

//-----------------------------------------------------------------------
//for external use
inline CBootLoader::CSECTION* CBootLoader::GetSyncObject(void) const
{
 ASSERT(mp_csection);
 return mp_csection;
}

//-----------------------------------------------------------------------
inline void CBootLoader::EnterCriticalSection(void) const
{
 ::EnterCriticalSection(GetSyncObject());
}

//-----------------------------------------------------------------------
inline void CBootLoader::LeaveCriticalSection(void) const
{
 ::LeaveCriticalSection(GetSyncObject());
}

//-----------------------------------------------------------------------
