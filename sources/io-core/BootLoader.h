 /****************************************************************
 *       SECU-3  - An open source, free engine control unit
 *    Designed by Alexey A. Shabelnikov. Ukraine, Gorlovka 2007.
 *       Microprocessors systems - design & programming.
 *    contacts:
 *              http://secu-3.narod.ru
 *              ICQ: 405-791-931
 ****************************************************************/

#include <windows.h>
#include "types.h"
#include "NumericConv.h"

#ifndef _BOOTLOADER_
#define _BOOTLOADER_

//для обработки событий при работе с бутлоадером необходимо наследоваться от этого класса
class AFX_EXT_CLASS IBLDEventHandler
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

	bool Initialize(CComPort* p_port, const DWORD uart_seed);
	void SetEventHandler(IBLDEventHandler* i_pEventHandler) 
	{ 
	  m_pEventHandler = i_pEventHandler;
	};

	void SwitchOn(bool state, bool i_force_reinit = false);
    bool GetWorkState(void) {return m_work_state;};

	bool Terminate(void);

    //starts the specified operation
	//!!! i_addr используется только при чтении FLASH
    bool StartOperation(const int opcode,BYTE* io_data,int i_size,int i_addr = 0); 
 
	inline bool IsIdle(void) const {return (m_ThreadBusy)?false:true;};

	inline HANDLE    GetThreadHandle(void) const {return m_hThread;}
	inline DWORD     GetThreadId(void) const     {return m_ThreadId;}
	inline CComPort* GetPortHandle(void) const   {return m_p_port;}
	inline int       GetLastError() const        {return m_ErrorCode;}

	inline CSECTION* GetSyncObject(void) const;
    inline void EnterCriticalSection(void) const;
    inline void LeaveCriticalSection(void) const;

	//возвращает true если все ОК
	inline int Status(void) const {return ((m_ErrorCode==0)?true:false);}     

	static DWORD WINAPI BackgroundProcess(LPVOID lpParameter);

	enum {BL_SIGNATURE_STR_LEN = 24};
	enum {EEPROM_SIZE = 512};         //size of EEPROM
	enum {EEPROM_WR_DELAY_MULTIPLIER = 3};
	enum {FLASH_PG_ERASE_DELAY = 30}; 

    enum 
	{
	  FLASH_PAGE_SIZE = 128,         //размер страницы памяти программ в байтах
	  FLASH_TOTAL_SIZE = 16384,      //общий размер памяти программ микроконтроллера
	  FLASH_BL_SECTION_SIZE = 512,   //кол-во байт отведенное для бутлоадера (из секции бутлоадера этот блок можно читать)
	  FLASH_APP_SECTION_SIZE = FLASH_TOTAL_SIZE - FLASH_BL_SECTION_SIZE, //часть прошивки кроме бутлоадера
	  FLASH_ONLY_CODE_SIZE = 0x32F0,  //размер кода без данных, которые в конце

	  //часть прошивки располагающаяся после кода программы и перед бутлоадером. Сюда входят пустое пространство, данные 
	  //и контрольная сумма прошивки.
	  FLASH_ONLY_OVERHEAD_SIZE = FLASH_APP_SECTION_SIZE - FLASH_ONLY_CODE_SIZE
	};

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
      
   struct  //сохраняет данные необходимые для выполнения операции
   { 
     int   opcode;
	 BYTE* data;
	 int   addr;
	 int   size;
   }m_opdata;

   bool IsOpcodeValid(const int opcode);
   bool FLASH_ReadOnePage(int n_page,BYTE* o_buf,int total_size,int* current);

   void EventHandler_OnUpdateUI(const int i_opcode,const int i_total,const int i_current);
   void EventHandler_OnBegin(const int i_opcode, const int i_status);
   void EventHandler_OnEnd(const int i_opcode, const int i_status);
};

#endif //_BOOTLOADER_

