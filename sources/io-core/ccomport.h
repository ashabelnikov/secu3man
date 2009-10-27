 /****************************************************************
 *       SECU-3  - An open source, free engine control unit
 *    Designed by Alexey A. Shabelnikov. Ukraine, Gorlovka 2007.
 *       Microprocessors systems - design & programming.
 *    contacts:
 *              http://secu-3.narod.ru
 *              ICQ: 405-791-931
 ****************************************************************/

#include <windows.h>
#include <string>
#include "common/unicodesupport.h"

#ifndef _CCOMPORT_
#define _CCOMPORT_

//Класс для работы с COM - портом
class AFX_EXT_CLASS CComPort 
{
private:  //local variables
	bool            m_bPortReady;
    HANDLE          m_hCom;         //хендл порта как файла
	_TSTRING        m_sComPort;     //имя файла - порта
    DCB             m_dcb;          //describes a COM port
    COMMTIMEOUTS    m_CommTimeouts;
	UINT            dwInQueue;      //размер буфера приемника
	UINT            dwOutQueue;     //размер буфера передатчика
    UINT            m_snd_err_num;  //хранит кол-во ошибок записи в порт
	UINT            m_rcv_err_num;  //хранит кол-во ошибок чтения из порта

	void _LoadDefaultTimeouts(void);

public:   //public functions
    CComPort(const _TSTRING&,UINT,UINT);        //конструктор
    CComPort(int,UINT,UINT);
    ~CComPort();                        //деструктор
	void   Set9bitState(BOOL state);    
	
	BOOL   Initialize(DWORD,BYTE,BYTE); //инициализация порта
	BOOL   Initialize(DWORD,BYTE,BYTE,char,char);
	BOOL   Initialize(const _TSTRING& i_sComPort,DWORD,BYTE,BYTE,char,char);


    VOID   Terminate();                 //закрытие порта 
    bool   SendByte(unsigned char);     //передача одного ьайта
    bool   RecvByte(unsigned char*);    //прием одного байта
	bool   SendBlock(BYTE*,UINT);       //передача заданного кол-ва байт
    bool   RecvBlock(BYTE*,UINT);       //прием заданного кол-ва байт 
    bool   SendASCII(const char* str);
    HANDLE GetHandle(void) const;       //возвращает хендл порта
    bool   SetDTR(bool);                //уст. в указ. знач. линию DTR
    bool   SetRTS(bool);                //уст. в указ. знач. линию RTS
    bool   SetTXD(bool);                //уст. в указ. знач. линию TXD
    UINT   GetRecvErrNum(void) const;
    UINT   GetSendErrNum(void) const;
    void   ResetPortErrors(void);
	BOOL   Purge(const DWORD dwFlags = PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR) const;
    BOOL   SetTimeouts(COMMTIMEOUTS* i_cto);
	BOOL   GetTimeouts(COMMTIMEOUTS* o_cto);
    bool   GetState(DCB* o_dcb);
    bool   SetState(DCB* i_dcb);
    bool   RecvBlock(BYTE* data,UINT datasize,DWORD* real_readed);

	//да, да - именно float т.к число может оказаться нецелым и потеря знаков после запятой будет являтся 
	//причиной ошибки в расчете времени необходимого для N байт.
	inline static float ms_need_for_one_byte_8N1(DWORD baud_rate)
	{
	  return ((1.0f / (((float)baud_rate) / 10.0f)) * 1000.0f); 
	}

	inline DCB* AccessDCB(void)
	{
	  return &m_dcb;
	}

	inline BOOL SetState()
	{
	  return SetCommState(m_hCom,&m_dcb);
	}


	inline bool IsInitialized(void) 
	{ 
		return ((m_hCom==INVALID_HANDLE_VALUE) ? false : true);
	}

    enum { MAX_PORTS_ALLOWED = 32};    

	class xInitialize
	{
		_TSTRING m_detail_str;       //string that contain details about exception
	public: 
		xInitialize(const _TSTRING& str) : m_detail_str(str){};
		LPCTSTR GetDetailStr(void) const 
		{
			return m_detail_str.c_str(); //save details
		};
	};

	typedef xInitialize xSetTimeout;
};
 
#endif  //_CCOMPORT_