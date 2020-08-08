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

/** \file BootLoader.h
 * \author Alexey A. Shabelnikov
 */

#include <vector>
#include "iocore_api.h"
#include "PlatformParamHolder.h"
#include "types.h"

#ifndef _BOOTLOADER_
#define _BOOTLOADER_

//для обработки событий при работе с бутлоадером необходимо наследоваться от этого класса
class IOCORE_API IBLDEventHandler
{
 public:
  struct poolUpdateUI
  {
   void Set(int i_opcode,int i_total,int i_current)
   {
    opcode  = i_opcode;
    total   = i_total;
    current = i_current;
   }

   void operator=(poolUpdateUI& i_other)
   {
    opcode  = i_other.opcode;
    total   = i_other.total;
    current = i_other.current;
   }

   int opcode;
   int total;
   int current;
  };

  //Обработчики событий - пользователь этого класса толжен обязательно перегрузить эти функции!
  virtual void OnUpdateUI(IBLDEventHandler::poolUpdateUI* ip_data) = 0;
  virtual void OnBegin(const int opcode, const int status) = 0;
  virtual void OnEnd(const int opcode, const int status) = 0;

};


class CComPort;
class AFX_EXT_CLASS CBootLoader
{
 public:
  typedef CRITICAL_SECTION CSECTION;

  CBootLoader();
  virtual ~CBootLoader();

  void SetPlatformParameters(const PlatformParamHolder& i_pph);
  bool Initialize(CComPort* p_port, const DWORD uart_seed);
  void SetEventHandler(IBLDEventHandler* i_pEventHandler);

  void SwitchOn(bool state, bool i_force_reinit = false);

  bool Terminate(void);

  //starts the specified operation
  //!!! i_addr используется только при чтении FLASH
  bool StartOperation(const int opcode,BYTE* io_data,int i_size,int i_addr = 0);
  void AbortOperation(void);
  
  inline bool GetWorkState(void) const;
  inline bool IsIdle(void) const;
  inline HANDLE GetThreadHandle(void) const;
  inline DWORD GetThreadId(void) const;
  inline CComPort* GetPortHandle(void) const;
  inline int GetLastError() const;
  //возвращает true если все ОК
  inline int Status(void) const;

  inline CSECTION* GetSyncObject(void) const;
  inline void EnterCriticalSection(void) const;
  inline void LeaveCriticalSection(void) const;

  static DWORD WINAPI BackgroundProcess(LPVOID lpParameter);

  enum {BL_SIGNATURE_STR_LEN = 24};
  enum {EEPROM_WR_DELAY_MULTIPLIER = 8};
  enum {FLASH_PG_ERASE_DELAY = 30};

  enum //список поддерживаемых команд
  {
   BL_OP_READ_FLASH     = 1,
   BL_OP_WRITE_FLASH    = 2,
   BL_OP_READ_EEPROM    = 3,
   BL_OP_WRITE_EEPROM   = 4,
   BL_OP_READ_SIGNATURE = 5,
   BL_OP_EXIT = 6
  };

  enum
  {
   BL_ERROR_NOANSWER   = 1,
   BL_ERROR_CHKSUM     = 2,
   BL_ERROR_WRONG_DATA = 3
  };

  class xThread {};

 private:

  std::vector<BYTE> m_fw_buf;
  PPFlashParam m_ppf;
  PPEepromParam m_ppe;
  CSECTION* mp_csection;
  CComPort* m_p_port;
  HANDLE    m_hThread;
  HANDLE    m_hAwakeEvent;
  DWORD     m_ThreadId;
  bool      m_ThreadBusy;
  int       m_ErrorCode;

  volatile bool m_is_thread_must_exit;
  bool      m_work_state;
  DWORD     m_uart_speed;

  enum {PENDING_DATA_QUEUE_SIZE = 256};
  struct PendingData
  {
   IBLDEventHandler::poolUpdateUI m_update_ui;
  }m_pending_data[PENDING_DATA_QUEUE_SIZE];

  int m_current_pending_data_index;

  IBLDEventHandler* m_pEventHandler; //указатель на класс-обработчик событий (реализующий интерфейс IBLDEventHandler)

  struct  //holds data necessary for execution of command
  {
   int   opcode;
   BYTE* data;
   int   addr;
   int   size;
  }m_opdata;

  bool IsOpcodeValid(const int opcode);
  bool FLASH_ReadOnePage(int n_page, BYTE* o_buf, int total_size, int* current);
  bool FLASH_WriteOnePage(int n_page, BYTE* i_buf, int total_size, int* current);
  bool EEPROM_Read(BYTE* o_buf, int total_size, int* current);
  bool EEPROM_Write(BYTE* i_buf, int total_size, int* current);
  bool ReadSignature(BYTE* o_buf, int total_size, int* current);
  bool ExitBootLoader(int total_size, int* current);

  void EventHandler_OnUpdateUI(const int i_opcode,const int i_total,const int i_current);
  void EventHandler_OnBegin(const int i_opcode, const int i_status);
  void EventHandler_OnEnd(const int i_opcode, const int i_status);
};

#endif //_BOOTLOADER_
