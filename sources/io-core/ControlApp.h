 /****************************************************************
 *       SECU-3  - An open source, free engine control unit
 *    Designed by Alexey A. Shabelnikov. Ukraine, Gorlovka 2007.
 *       Microprocessors systems - design & programming.
 *    contacts:
 *              http://secu-3.narod.ru
 *              ICQ: 405-791-931
 ****************************************************************/

#include <windows.h>
#include <vector>
#include <string>
#include "types.h"
#include "NumericConv.h"
#include "SECU3IO.h"

#ifndef _CONTROLAPP_
#define _CONTROLAPP_


#define ADC_DISCRETE        0.0025f   //Вольт
 
#define ANGLE_MULTIPLAYER   32.0f     //коэффициент масштабирования углов поворота коленвала  

#define MAP_PHYSICAL_MAGNITUDE_MULTIPLAYER  64
#define UBAT_PHYSICAL_MAGNITUDE_MULTIPLAYER 400
#define TEMP_PHYSICAL_MAGNITUDE_MULTIPLAYER 4


#define RAW_BYTES_BUFFER_SIZE  8192
#define RAW_BYTES_TO_READ_MAX  32

//этот интерфейс должен реализовываться классом-обработчиком событий
class IAPPEventHandler
{
public:
  //event handlers
  virtual void OnPacketReceived(const BYTE i_descriptor, const void* i_packet_data) = 0;
  virtual void OnConnection(const bool i_online) = 0;
};



class CComPort;
class AFX_EXT_CLASS CControlApp  
{

private:
	enum {PENDING_PACKETS_QUEUE_SIZE = 32};

  struct SECU3Packet
	{
    SECU3IO::SensorDat m_SensorDat;
    SECU3IO::FnNameDat m_FnNameDat;
    SECU3IO::StartrPar m_StartrPar; 
    SECU3IO::AnglesPar m_AnglesPar;
    SECU3IO::FunSetPar m_FunSetPar;
    SECU3IO::IdlRegPar m_IdlRegPar;
    SECU3IO::CarburPar m_CarburPar;
    SECU3IO::TemperPar m_TemperPar;
    SECU3IO::ADCCompenPar m_ADCCompenPar;
    SECU3IO::RawSensDat m_RawSensDat;
    SECU3IO::CKPSPar m_CKPSPar;
	SECU3IO::OPCompNc m_OPCompNc;
	}m_pending_packets[PENDING_PACKETS_QUEUE_SIZE];

   int m_pending_packets_index;

   CComPort*    m_p_port;
   HANDLE       m_hThread;
   DWORD        m_ThreadId;
   HANDLE       m_hAwakeEvent;

   DWORD        m_uart_speed;
   DWORD        m_dat_packet_timeout;
   HANDLE       m_hTimer;

   IAPPEventHandler* m_pEventHandler; //указатель на класс-обработчик событий (реализующий интерфейс IAPPEventHandler)

   bool         m_is_thread_must_exit;

   typedef      std::vector<std::string> Packets;

   Packets*     m_Packets;      //хранит списик выделенных пакетов
   std::string  m_ingoing_packet;       //используется для выделения одного пакета
   std::string  m_outgoing_packet;     //используется для формирования пакетов
   int          m_packets_parse_state; //хранит состояние конечного автомата используемого для отделения пакетов

   float        m_adc_discrete;
   float        m_angle_multiplier;

   //helper
   void SwitchOnThread(bool state);
   int  SplitPackets(BYTE*);
   bool ParsePackets();
   BOOL SetPacketsTimer(int timeout);
   bool IsValidDescriptor(const BYTE descriptor);   

   SECU3Packet& EndPendingPacket(void);
   SECU3Packet& PendingPacket(void);

   //парсеры отдельных пакетов (тех пакетов которые принимаются от SECU-3)
   bool Parse_SENSOR_DAT(BYTE* raw_packet);
   bool Parse_FNNAME_DAT(BYTE* raw_packet);
   bool Parse_STARTR_PAR(BYTE* raw_packet);
   bool Parse_ANGLES_PAR(BYTE* raw_packet);
   bool Parse_FUNSET_PAR(BYTE* raw_packet);
   bool Parse_IDLREG_PAR(BYTE* raw_packet);
   bool Parse_CARBUR_PAR(BYTE* raw_packet);
   bool Parse_TEMPER_PAR(BYTE* raw_packet);
   bool Parse_ADCRAW_DAT(BYTE* raw_packet);
   bool Parse_ADCCOR_PAR(BYTE* raw_packet);
   bool Parse_CKPS_PAR(BYTE* raw_packet);
   bool Parse_OP_COMP_NC(BYTE* raw_packet);


   //сборщики пакетов
   void Build_CARBUR_PAR(SECU3IO::CarburPar* packet_data);
   void Build_IDLREG_PAR(SECU3IO::IdlRegPar* packet_data);
   void Build_STARTR_PAR(SECU3IO::StartrPar* packet_data);
   void Build_TEMPER_PAR(SECU3IO::TemperPar* packet_data);
   void Build_ANGLES_PAR(SECU3IO::AnglesPar* packet_data);
   void Build_FUNSET_PAR(SECU3IO::FunSetPar* packet_data);
   void Build_ADCCOR_PAR(SECU3IO::ADCCompenPar* packet_data);
   void Build_CKPS_PAR(SECU3IO::CKPSPar* packet_data);
   void Build_OP_COMP_NC(SECU3IO::OPCompNc* packet_data);
  
public:
   bool Initialize(CComPort* p_port, const DWORD uart_seed, const DWORD);
   bool Terminate(void);
   void SwitchOn(bool state);
   bool SendPacket(const BYTE i_descriptor, const void* i_packet_data);
   bool ChangeContext(const BYTE i_new_descriptor);
   bool StartBootLoader();
   void SetEventHandler(IAPPEventHandler* i_pEventHandler);        

   bool GetOnlineStatus(void) 
   {
     return m_online_state;
   }

   static DWORD WINAPI BackgroundProcess(LPVOID lpParameter);

   class xThread {};

   CControlApp();
   virtual ~CControlApp();

private:

   bool m_online_state;  //хранит текущее состояние (онлайн или оффлайн)
   bool m_force_notify_about_connection; //установка этого флага заставит поток оповестить слушателя об текущем состоянии подключения
    
};

#endif //_CONTROLAPP_
