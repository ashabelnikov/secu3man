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

/** \file ControlApp.h
 * \author Alexey A. Shabelnikov
 */

#include <vector>
#include <string>
#include "iocore_api.h"
#include "SECU3IO.h"
#include "types.h"

#ifndef _CONTROLAPP_
#define _CONTROLAPP_

#define RAW_BYTES_BUFFER_SIZE  8192
#define RAW_BYTES_TO_READ_MAX  32

//этот интерфейс должен реализовываться классом-обработчиком событий
class IAPPThreadEventHandler
{
  typedef CRITICAL_SECTION CSECTION;
 public:
  IAPPThreadEventHandler() : mp_sync_object(NULL) {};

  //event handlers
  virtual void OnPacketReceived(const BYTE i_descriptor, SECU3IO::SECU3Packet* ip_packet) = 0;
  virtual void OnConnection(const bool i_online) = 0;

 protected:
  void EnterCriticalSection(void)
  {
   _ASSERTE(mp_sync_object);
   ::EnterCriticalSection(mp_sync_object);
  };

  void LeaveCriticalSection(void)
  {
   _ASSERTE(mp_sync_object);
   ::LeaveCriticalSection(mp_sync_object);
  };

 private:
  friend class CControlApp;
  CSECTION* mp_sync_object;
};


class CComPort;
class PacketDataProxy;
class IOCORE_API CControlApp
{
 public:
  typedef CRITICAL_SECTION CSECTION;
  typedef IAPPThreadEventHandler EventHandler;

  CControlApp();
  virtual ~CControlApp();

  bool Initialize(CComPort* p_port, const DWORD uart_seed, const DWORD);
  bool Terminate(void);
  void SwitchOn(bool state, bool i_force_reinit = false);
  void SetProtocolDataMode(bool i_mode);
  void SetQuartzFrq(long frq);
  void SetPortAutoReopen(bool reopen);
  bool SendPacket(const BYTE i_descriptor, const void* i_packet_data);
  bool ChangeContext(const BYTE i_new_descriptor);
  bool StartBootLoader();
  void SetEventHandler(EventHandler* i_pEventHandler);
  inline CSECTION* GetSyncObject(void);
  inline void EnterCriticalSection(void);
  inline void LeaveCriticalSection(void);
  inline bool GetOnlineStatus(void) { return m_online_state; }
  inline bool GetWorkState(void) { return m_work_state; };

  //Required by speed sensor
  //pp1km - number of pulses per 1 km
  void SetNumPulsesPer1Km(int pp1km);

  void SetSpeedUnit(int i_unit); // 0 - km/h, 1 - mi/h
 
  void SetFFFConst(int fffConst);

  static DWORD WINAPI BackgroundProcess(LPVOID lpParameter);

  class xThread {};

  BOOL SetPacketsTimer(int timeout);

 private:
  typedef std::vector<std::vector<BYTE> > Packets;

  enum {PENDING_PACKETS_QUEUE_SIZE = 256};

  SECU3IO::SECU3Packet m_recepted_packet;
  SECU3IO::SECU3Packet m_pending_packets[PENDING_PACKETS_QUEUE_SIZE];
  int m_pending_packets_index;

  CSECTION*    mp_csection; //объект критической секции
  CComPort*    m_p_port;
  HANDLE       m_hThread;
  DWORD        m_ThreadId;
  HANDLE       m_hAwakeEvent;
  HANDLE       m_hSleepEvent;

  DWORD        m_uart_speed;
  int          m_fffConst;
  DWORD        m_dat_packet_timeout;
  HANDLE       m_hTimer;

  EventHandler* m_pEventHandler; //указатель на класс-обработчик событий (реализующий интерфейс IAPPEventHandler)

  volatile bool m_is_thread_must_exit;

  Packets*     m_pPackets;              //хранит списик выделенных пакетов
  std::vector<BYTE>  m_ingoing_packet;  //используется для выделения одного пакета
  std::vector<BYTE>  m_outgoing_packet; //используется для формирования пакетов
  int          m_packets_parse_state;   //хранит состояние конечного автомата используемого для отделения пакетов

  const float m_adc_discrete;
  const float m_angle_multiplier;
  bool m_online_state;                  //хранит текущее состояние (онлайн или оффлайн)
  bool m_force_notify_about_connection; //установка этого флага заставит поток оповестить слушателя об текущем состоянии подключения
  bool m_work_state;                    //хранит состояние устанавливающееся после вызова SwitchOn();
  bool m_portAutoReopen;                //flag, makes thread to do auto reopening of COM port (each 100ms if port is not opened)

  PacketDataProxy* mp_pdp;

  float m_period_distance;              //distance of one period in meters (speed sensor), used in calculations
  long m_quartz_frq;                    //MCU clock frequency
  int m_speedUnit;

  //helper
  void SwitchOnThread(bool state);
  int  SplitPackets(BYTE*, size_t i_size);
  bool ParsePackets();
  bool IsValidDescriptor(const BYTE descriptor) const;

  SECU3IO::SECU3Packet& EndPendingPacket(void);
  SECU3IO::SECU3Packet& PendingPacket(void);

  //парсеры отдельных пакетов (тех пакетов которые принимаются от SECU-3)
  bool Parse_SENSOR_DAT(const BYTE* raw_packet, size_t size);
  bool Parse_DBGVAR_DAT(const BYTE* raw_packet, size_t size);
  bool Parse_FNNAME_DAT(const BYTE* raw_packet, size_t size);
  bool Parse_STARTR_PAR(const BYTE* raw_packet, size_t size);
  bool Parse_ANGLES_PAR(const BYTE* raw_packet, size_t size);
  bool Parse_FUNSET_PAR(const BYTE* raw_packet, size_t size);
  bool Parse_IDLREG_PAR(const BYTE* raw_packet, size_t size);
  bool Parse_CARBUR_PAR(const BYTE* raw_packet, size_t size);
  bool Parse_TEMPER_PAR(const BYTE* raw_packet, size_t size);
  bool Parse_ADCRAW_DAT(const BYTE* raw_packet, size_t size);
  bool Parse_ADCCOR_PAR(const BYTE* raw_packet, size_t size);
  bool Parse_CKPS_PAR(const BYTE* raw_packet, size_t size);
  bool Parse_OP_COMP_NC(const BYTE* raw_packet, size_t size);
  bool Parse_KNOCK_PAR(const BYTE* raw_packet, size_t size);
  bool Parse_CE_ERR_CODES(const BYTE* raw_packet, size_t size);
  bool Parse_CE_SAVED_ERR(const BYTE* raw_packet, size_t size);
  bool Parse_FWINFO_DAT(const BYTE* raw_packet, size_t size);
  bool Parse_MISCEL_PAR(const BYTE* raw_packet, size_t size);
  bool Parse_EDITAB_PAR(const BYTE* raw_packet, size_t size);
  bool Parse_DIAGINP_DAT(const BYTE* raw_packet, size_t size);
  bool Parse_ATTTAB_PAR(const BYTE* raw_packet, size_t size);
  bool Parse_RPMGRD_PAR(const BYTE* raw_packet, size_t size);
  bool Parse_CHOKE_PAR(const BYTE* raw_packet, size_t size);
  bool Parse_SECUR_PAR(const BYTE* raw_packet, size_t size);
  bool Parse_UNIOUT_PAR(const BYTE* raw_packet, size_t size);
  bool Parse_INJCTR_PAR(const BYTE* raw_packet, size_t size);
  bool Parse_LAMBDA_PAR(const BYTE* raw_packet, size_t size);
  bool Parse_ACCEL_PAR(const BYTE* raw_packet, size_t size);
  bool Parse_GASDOSE_PAR(const BYTE* raw_packet, size_t size);
  bool Parse_INJDRV_PAR(const BYTE* raw_packet, size_t size);

  //сборщики отдельных пакетов
  void Build_CARBUR_PAR(SECU3IO::CarburPar* packet_data);
  void Build_IDLREG_PAR(SECU3IO::IdlRegPar* packet_data);
  void Build_STARTR_PAR(SECU3IO::StartrPar* packet_data);
  void Build_TEMPER_PAR(SECU3IO::TemperPar* packet_data);
  void Build_ANGLES_PAR(SECU3IO::AnglesPar* packet_data);
  void Build_FUNSET_PAR(SECU3IO::FunSetPar* packet_data);
  void Build_ADCCOR_PAR(SECU3IO::ADCCompenPar* packet_data);
  void Build_CKPS_PAR(SECU3IO::CKPSPar* packet_data);
  void Build_OP_COMP_NC(SECU3IO::OPCompNc* packet_data);
  void Build_KNOCK_PAR(SECU3IO::KnockPar* packet_data);
  void Build_CE_SAVED_ERR(SECU3IO::CEErrors* packet_data);
  void Build_MISCEL_PAR(SECU3IO::MiscelPar* packet_data);
  void Build_EDITAB_PAR(SECU3IO::EditTabPar* packet_data);
  void Build_DIAGOUT_DAT(SECU3IO::DiagOutDat* packet_data);
  void Build_CHOKE_PAR(SECU3IO::ChokePar* packet_data);
  void Build_SECUR_PAR(SECU3IO::SecurPar* packet_data);
  void Build_UNIOUT_PAR(SECU3IO::UniOutPar* packet_data);
  void Build_INJCTR_PAR(SECU3IO::InjctrPar* packet_data);
  void Build_LAMBDA_PAR(SECU3IO::LambdaPar* packet_data);
  void Build_ACCEL_PAR(SECU3IO::AccelPar* packet_data);
  void Build_GASDOSE_PAR(SECU3IO::GasdosePar* packet_data);
  void Build_INJDRV_PAR(SECU3IO::InjDrvPar* packet_data);
};

class CondEncoder
{
 public:
  CondEncoder(long quartz_frq, float period_distance);
 public:
  float UniOutDecodeCondVal(int val, int cond);
  int UniOutEncodeCondVal(float val, int cond);
  bool isSigned(int cond) const;
 private:
  long m_quartz_frq;
  float m_period_distance;
};

#endif //_CONTROLAPP_
