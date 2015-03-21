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

/** \file ccomport.h
 * \author Alexey A. Shabelnikov
 */

#include <string>
#include "iocore_api.h"
#include "common/unicodesupport.h"

#ifndef _CCOMPORT_
#define _CCOMPORT_

//Класс для работы с COM - портом
class IOCORE_API CComPort
{
 public:   //public functions

  CComPort(const _TSTRING&, UINT, UINT);//конструктор
  CComPort(int, UINT, UINT);
  ~CComPort();                        //деструктор
  void   Set9bitState(bool state);

  bool   Initialize(DWORD, BYTE, BYTE); //инициализация порта
  bool   Initialize(DWORD, BYTE, BYTE, int, int);
  bool   Initialize(const _TSTRING& i_sComPort, DWORD, BYTE, BYTE, int, int);

  VOID   Terminate();                 //закрытие порта
  bool   SendByte(unsigned char);     //передача одного ьайта
  bool   RecvByte(unsigned char*);    //прием одного байта
  bool   SendBlock(BYTE*, UINT);       //передача заданного кол-ва байт
  bool   RecvBlock(BYTE*, UINT);       //прием заданного кол-ва байт
  bool   SendASCII(const char* str);
  HANDLE GetHandle(void) const;       //возвращает хендл порта
  bool   SetDTR(bool);                //уст. в указ. знач. линию DTR
  bool   SetRTS(bool);                //уст. в указ. знач. линию RTS
  bool   SetTXD(bool);                //уст. в указ. знач. линию TXD
  UINT   GetRecvErrNum(void) const;
  UINT   GetSendErrNum(void) const;
  void   ResetPortErrors(void);
  bool   Purge(const DWORD dwFlags = PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR) const;
  bool   SetTimeouts(COMMTIMEOUTS* i_cto);
  bool   GetTimeouts(COMMTIMEOUTS* o_cto);
  bool   GetState(DCB* o_dcb);
  bool   SetState(DCB* i_dcb);
  bool   RecvBlock(BYTE* data, UINT datasize, DWORD* real_read);

  //да, да - именно float т.к число может оказаться нецелым и потеря знаков после запятой будет являтся
  //причиной ошибки в расчете времени необходимого для N байт.
  static float ms_need_for_one_byte_8N1(DWORD baud_rate);

  inline DCB& AccessDCB(void);

  inline bool SetState();

  inline bool IsInitialized(void);

  enum { MAX_PORTS_ALLOWED = 32};

  class IOCORE_API xRuntimeError
  {
    _TSTRING m_detail_str;       //string that contains details about exception
   public:
    xRuntimeError(const _TSTRING& str);
    LPCTSTR GetDetailStr(void) const;
  };

  typedef xRuntimeError xInitialize;
  typedef xRuntimeError xSetTimeout;

 private:
  void _LoadDefaultTimeouts(void);

  bool          m_bPortReady;
  HANDLE        m_hCom;         //хендл порта как файла
  _TSTRING      m_sComPort;     //имя файла - порта
  DCB           m_dcb;          //describes a COM port
  COMMTIMEOUTS  m_CommTimeouts; //timeouts
  UINT          dwInQueue;      //размер буфера приемника
  UINT          dwOutQueue;     //размер буфера передатчика
  UINT          m_snd_err_num;  //хранит кол-во ошибок записи в порт
  UINT          m_rcv_err_num;  //хранит кол-во ошибок чтения из порта
};

#endif  //_CCOMPORT_
