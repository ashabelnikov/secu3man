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

/** \file ControlApp.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include <vector>
#include "ControlApp.h"
#include "ccomport.h"
#include "common/MathHelpers.h"
#include "FirmwareMapsDataHolder.h"
#include "PacketDataProxy.h"
#include "ufcodes.h"
#include "Magnitude.h"
#include "io-core/bitmask.h"
#include "SECU3TablesDef.h"

using namespace SECU3IO;

namespace {

// There are several special reserved symbols in binary mode: 0x21, 0x40, 0x0D, 0x0A
const BYTE FIBEGIN = 0x21;  // '!' indicates beginning of the ingoing packet
const BYTE FOBEGIN = 0x40;  // '@' indicates beginning of the outgoing packet
const BYTE FIOEND = 0x0D;   // '\r' indicates ending of the ingoing/outgoing packet
const BYTE FESC = 0x0A;     // '\n' Packet escape (FESC)
// Following bytes are used only in escape sequeces and may appear in the data without any problems
const BYTE TFIBEGIN = 0x81; // Transposed FIBEGIN
const BYTE TFOBEGIN = 0x82; // Transposed FOBEGIN
const BYTE TFIOEND = 0x83;  // Transposed FIOEND
const BYTE TFESC = 0x84;    // Transposed FESC


void Esc_Rx_Packet(std::vector<BYTE>& io_data, size_t offset, size_t size)
{
 for(size_t i = 0; i < size; ++i)
 {
  size_t index = i + offset;
  if (io_data[index] == FESC)
  {
   io_data.erase(io_data.begin() + index);
   if (io_data[index] == TFOBEGIN)
    io_data[index] = FOBEGIN;
   else if (io_data[index] == TFIOEND)
    io_data[index] = FIOEND;
   else if (io_data[index] == TFESC)
    io_data[index] = FESC;
   --size;
  }
 }
}

void Esc_Tx_Packet(std::vector<BYTE>& io_data, size_t offset, size_t size)
{
 for(size_t i = 0; i < size; ++i)
 {
  size_t index = i + offset;
  if (io_data[index] == FIBEGIN)
  {
   io_data[index] = FESC;
   io_data.insert(io_data.begin() + (index+1), TFIBEGIN);
   ++size;
  }
  else if (io_data[index] == FIOEND)
  {
   io_data[index] = FESC;
   io_data.insert(io_data.begin() + (index+1), TFIOEND);
   ++size;
  }
  else if (io_data[index] == FESC)
  {
   io_data[index] = FESC;
   io_data.insert(io_data.begin() + (index+1), TFESC);
   ++size;
  }
 }
}
}

//-----------------------------------------------------------------------
CControlApp::CControlApp()
: m_adc_discrete(ADC_DISCRETE)
, m_angle_multiplier(ANGLE_MULTIPLIER)
, m_pEventHandler(NULL)
, m_online_state(false)
, m_force_notify_about_connection(false)
, m_pending_packets_index(0)
, m_p_port(NULL)
, m_hThread(NULL)
, m_ThreadId(0)
, m_hAwakeEvent(NULL)
, m_hSleepEvent(NULL)
, m_is_thread_must_exit(false)
, m_uart_speed(CBR_57600)
, m_packets_parse_state(0)
, m_hTimer(NULL)
, mp_csection(NULL)
, m_work_state(false)
, m_period_distance(0.166666f)   //for speed sensor calculations
, m_quartz_frq(20000000)    //default clock is 20mHz
, m_speedUnit(0) //km/h
, m_fffConst(16000)
, m_portAutoReopen(true)
{
 m_pPackets = new Packets();
 memset(&m_recepted_packet,0,sizeof(SECU3Packet));
 memset(&m_pending_packets,0,sizeof(SECU3Packet) * PENDING_PACKETS_QUEUE_SIZE);

 mp_csection = new CSECTION;
 InitializeCriticalSection(GetSyncObject());

 mp_pdp = new PacketDataProxy();
}

//-----------------------------------------------------------------------
CControlApp::~CControlApp()
{
 delete m_pPackets;
 DeleteCriticalSection(GetSyncObject());
 delete mp_csection;
 delete mp_pdp;
}


//-----------------------------------------------------------------------
//каждому вызову Initialize должен соответствовать вызов этой функции
bool CControlApp::Terminate(void)
{
 bool status=true;
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

 if (!TerminateThread(m_hThread,0)) //убиваем поток насильно
  status = false;

 if (!CloseHandle(m_hAwakeEvent))
  status = false;

 if (!CloseHandle(m_hSleepEvent))
  status = false;

 if (NULL!=m_hTimer)
  CancelWaitableTimer(m_hTimer);

 if (!CloseHandle(m_hTimer))
  status = false;

 return status;
}



//-----------------------------------------------------------------------
//exceptions: xThread()
bool CControlApp::Initialize(CComPort* p_port,const DWORD uart_speed, const DWORD dat_packet_timeout)
{
 if (!p_port)
  return false;
 m_p_port = p_port;

 m_hAwakeEvent = CreateEvent(NULL,TRUE,FALSE,NULL);  //manual reset, nonsignaled state!
 if (m_hAwakeEvent==NULL)
 {
  return false;        //event creation error
 }

 m_hSleepEvent = CreateEvent(NULL,TRUE,FALSE,NULL);  //manual reset, nonsignaled state!
 if (m_hSleepEvent==NULL)
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

 m_uart_speed = uart_speed;
 m_dat_packet_timeout = dat_packet_timeout;

 m_ingoing_packet.clear();
 m_packets_parse_state = 0;

 if (NULL==(m_hTimer = CreateWaitableTimer(NULL,TRUE,_T("packet_wt"))))
 {
  return false;
 }

 return true;
}


//-----------------------------------------------------------------------
//m_Packets будут содержаться обнаруженные пакеты
//Return: кол-во обнаруженых пакетов
int CControlApp::SplitPackets(BYTE* i_buff, size_t i_size)
{
 ASSERT(m_pPackets);
 m_pPackets->clear();

 BYTE* p = i_buff;
 BYTE* end = i_buff + i_size;

 while(p != end)
 {
  switch(m_packets_parse_state) //я люблю автоматное программирование...
  {
   case 0:       //search '@'
    if (*p=='@')
    {
     m_ingoing_packet.push_back(*p);
     m_packets_parse_state = 1;
    }
    break;
   case 1:       //wait '\r'
    if (*p=='\r')
    {
     m_ingoing_packet.push_back(*p);
     m_pPackets->push_back(m_ingoing_packet);
     m_ingoing_packet.clear();
     m_packets_parse_state = 0;
    }
    else
    {
     m_ingoing_packet.push_back(*p);
    }
    break;
  }//switch
  ++p;
 };

 return m_pPackets->size();
}

//-----------------------------------------------------------------------
bool CControlApp::Parse_SENSOR_DAT(const BYTE* raw_packet, size_t size)
{
 SECU3IO::SensorDat& m_SensorDat = m_recepted_packet.m_SensorDat;
 if (size != (mp_pdp->isHex() ? 130 : 65))  //размер пакета без сигнального символа, дескриптора и символа-конца пакета
  return false;

 //частота вращения двигателя
 if (false == mp_pdp->Hex16ToBin(raw_packet, &m_SensorDat.frequen))
  return false;

 //давление во впускном коллекторе
 int pressure = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &pressure))
  return false;
 m_SensorDat.pressure = ((float)pressure) / MAP_PHYSICAL_MAGNITUDE_MULTIPLIER;

 //напряжение бортовой сети
 int voltage = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&voltage))
  return false;
 m_SensorDat.voltage = ((float)voltage) / UBAT_PHYSICAL_MAGNITUDE_MULTIPLIER;

 //Температура охлаждающей жидкости
 int temperature = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&temperature,true))
  return false;
 m_SensorDat.temperat = ((float)temperature) / TEMP_PHYSICAL_MAGNITUDE_MULTIPLIER;
 m_SensorDat.temperat = MathHelpers::RestrictValue(m_SensorDat.temperat, -99.9f, 999.0f);

 //Текущий УОЗ (число со знаком)
 int adv_angle = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&adv_angle,true))
  return false;
 m_SensorDat.adv_angle = ((float)adv_angle) / m_angle_multiplier;

 //Уровень детонации двигателя
 int knock_k = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&knock_k))
  return false;
 m_SensorDat.knock_k = ((float)knock_k) * m_adc_discrete;

 //Корректировка УОЗ при детонации
 int knock_retard = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&knock_retard, true))
  return false;
 m_SensorDat.knkret_use = (knock_retard != 32767);
 m_SensorDat.knock_retard = ((float)knock_retard) / m_angle_multiplier;

 //Расход воздуха
 if (false == mp_pdp->Hex8ToBin(raw_packet,&m_SensorDat.air_flow))
  return false;

 //16 bit flags
 int flags = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &flags))
  return false;

 //flags
 m_SensorDat.ephh_valve   = CHECKBIT16(flags, 0);
 m_SensorDat.carb         = CHECKBIT16(flags, 1);
 m_SensorDat.gas          = CHECKBIT16(flags, 2);
 m_SensorDat.epm_valve    = CHECKBIT16(flags, 3);
 m_SensorDat.ce_state     = CHECKBIT16(flags, 4);
 m_SensorDat.cool_fan     = CHECKBIT16(flags, 5);
 m_SensorDat.st_block     = CHECKBIT16(flags, 6);
 m_SensorDat.acceleration = CHECKBIT16(flags, 7);
 m_SensorDat.fc_revlim    = CHECKBIT16(flags, 8);
 m_SensorDat.floodclear   = CHECKBIT16(flags, 9);
 m_SensorDat.sys_locked   = CHECKBIT16(flags, 10);

 //TPS sensor
 unsigned char tps = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet,&tps))
  return false;
 m_SensorDat.tps = ((float)tps) / TPS_PHYSICAL_MAGNITUDE_MULTIPLIER;

 //ADD_I1 input
 int add_i1_v = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&add_i1_v))
  return false;
 m_SensorDat.add_i1 = ((float)add_i1_v) * m_adc_discrete;

 //ADD_I2 input
 int add_i2_v = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&add_i2_v))
  return false;
 m_SensorDat.add_i2 = ((float)add_i2_v) * m_adc_discrete;

 //Биты ошибок СЕ
 int ce_errors = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &ce_errors))
  return false;
 m_SensorDat.ce_errors = ce_errors;

 //Choke position
 unsigned char choke_pos = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &choke_pos))
  return false;
 m_SensorDat.choke_pos = ((float)choke_pos) / CHOKE_PHYSICAL_MAGNITUDE_MULTIPLIER;

 //gas dosator position
 unsigned char gasdose_pos = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &gasdose_pos))
  return false;
 m_SensorDat.gasdose_pos = ((float)gasdose_pos) / GD_PHYSICAL_MAGNITUDE_MULTIPLIER;

 //Vehicle speed
 int speed = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&speed))
  return false;
 if (0 != speed && 65535 != speed)
 { //speed sensor is used, value is correct
  float period_s = ((float)speed / ((m_quartz_frq==20000000) ? 312500.0f: 250000.0f)); //period in seconds
  m_SensorDat.speed = ((m_period_distance / period_s) * 3600.0f) / 1000.0f; //Km/h
  if (m_SensorDat.speed > 999.9f)
   m_SensorDat.speed = 999.9f;
  //convert to selected unit
  if (m_speedUnit == 1)
   m_SensorDat.speed/= 1.609344f;
 }
 else //speed sensor is not used or speed is too low
  m_SensorDat.speed = 0;

 //Distance
 unsigned long distance = 0;
 if (false == mp_pdp->Hex24ToBin(raw_packet,&distance))
  return false;
 m_SensorDat.distance = (m_period_distance * distance) / 1000.0f;
 if (m_SensorDat.distance > 9999.99f)
  m_SensorDat.distance = 9999.99f;
 //convert to selected unit
 if (m_speedUnit == 1)
  m_SensorDat.distance/= 1.609344f;

 //Fuel flow frequency
 int inj_fff = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &inj_fff))
  return false;
 m_SensorDat.inj_fff = ((float)inj_fff) / 256.0f; //because raw value multiplied by 256

 //calculate value of fuel flow in L/100km
 if (m_SensorDat.speed > .0f) 
  m_SensorDat.inj_ffd = (m_SensorDat.inj_fff / m_SensorDat.speed) * ((3600.0f * 100.0f) / ((float)m_fffConst));
 else
  m_SensorDat.inj_ffd = .0f;

 //Intake air temperature
 int air_temp = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&air_temp,true))
  return false;

 if (air_temp!=0x7FFF)
 {
  m_SensorDat.air_temp = ((float)air_temp) / TEMP_PHYSICAL_MAGNITUDE_MULTIPLIER;
  m_SensorDat.air_temp = MathHelpers::RestrictValue(m_SensorDat.air_temp, -99.9f, 999.0f);
  m_SensorDat.add_i2_mode = 1;
 }
 else //do not show air temperature
 {
  m_SensorDat.air_temp = .0f;
  m_SensorDat.add_i2_mode = 0;
 }

 // Advance angle from start map (signed value)
 int strt_aalt = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&strt_aalt,true))
  return false;
 m_SensorDat.strt_use = (strt_aalt != 32767);
 m_SensorDat.strt_aalt = m_SensorDat.strt_use ? (((float)strt_aalt) / m_angle_multiplier) : 0;

 //Advance angle from idle map (signed value)
 int idle_aalt = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&idle_aalt,true))
  return false;
 m_SensorDat.idle_use = (idle_aalt != 32767);
 m_SensorDat.idle_aalt = m_SensorDat.idle_use ? (((float)idle_aalt) / m_angle_multiplier) : 0;

 // Advance angle from work map (signed value)
 int work_aalt = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&work_aalt,true))
  return false;
 m_SensorDat.work_use = (work_aalt != 32767);
 m_SensorDat.work_aalt = m_SensorDat.work_use ? (((float)work_aalt) / m_angle_multiplier) : 0;

 // Advance angle from coolant temperature correction map (signed value)
 int temp_aalt = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&temp_aalt,true))
  return false;
 m_SensorDat.temp_use = (temp_aalt != 32767);
 m_SensorDat.temp_aalt = m_SensorDat.temp_use ? (((float)temp_aalt) / m_angle_multiplier) : 0;

 // Advance angle from air temperature correction map (signed value)
 int airt_aalt = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&airt_aalt,true))
  return false;
 m_SensorDat.airt_use = (airt_aalt != 32767);
 m_SensorDat.airt_aalt = m_SensorDat.airt_use ? (((float)airt_aalt) / m_angle_multiplier) : 0;

 // Advance angle from correction from idling RPM regulator (signed value)
 int idlreg_aac = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&idlreg_aac,true))
  return false;
 m_SensorDat.idlreg_use = (idlreg_aac != 32767);
 m_SensorDat.idlreg_aac = m_SensorDat.idlreg_use ? (((float)idlreg_aac) / m_angle_multiplier) : 0;

 // Octane correction value (signed value)
 int octan_aac = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&octan_aac,true))
  return false;
 m_SensorDat.octan_use = (octan_aac != 32767);
 m_SensorDat.octan_aac = m_SensorDat.octan_use ? (((float)octan_aac) / m_angle_multiplier) : 0;

 // Lambda correction value (signed value)
 int lambda_corr = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&lambda_corr,true))
  return false;
 m_SensorDat.lambda_corr = (((float)lambda_corr) / 512.0f) * 100.0f; //obtain value in %

 //Injector pulse width (ms)
 int inj_pw = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &inj_pw))
  return false;
 m_SensorDat.inj_pw = (inj_pw * 3.2f) / 1000.0f;

 //TPS opening/clising speed (d%/dt = %/s), signed value
 int tpsdot = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &tpsdot, true))
  return false;
 m_SensorDat.tpsdot = tpsdot;

 //Давление газа
 int map2 = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &map2))
  return false;
 m_SensorDat.map2 = ((float)map2) / MAP_PHYSICAL_MAGNITUDE_MULTIPLIER;
 //calculate here differential pressure
 m_SensorDat.mapd = (m_SensorDat.map2 - m_SensorDat.pressure);

 //Температура газа
 int tmp2 = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &tmp2,true))
  return false;
 m_SensorDat.tmp2 = ((float)tmp2) / TEMP_PHYSICAL_MAGNITUDE_MULTIPLIER;
 m_SensorDat.tmp2 = MathHelpers::RestrictValue(m_SensorDat.tmp2, -99.9f, 999.0f);

 //AFR value
 int afr = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &afr))
  return false;
 m_SensorDat.afr = ((float)afr) / AFR_PHYSICAL_MAGNITUDE_MULTIPLIER;

 //Load value
 int load = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &load))
  return false;
 m_SensorDat.load = ((float)load) / LOAD_PHYSICAL_MAGNITUDE_MULTIPLIER;

 //baro pressure
 int baro_press = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &baro_press))
  return false;
 m_SensorDat.baro_press = ((float)baro_press) / MAP_PHYSICAL_MAGNITUDE_MULTIPLIER;

 return true;
}

//-----------------------------------------------------------------------
bool CControlApp::Parse_DBGVAR_DAT(const BYTE* raw_packet, size_t size)
{
 SECU3IO::DbgvarDat& m_DbgvarDat = m_recepted_packet.m_DbgvarDat;
 if (size != (mp_pdp->isHex() ? 16 : 8))  //размер пакета без сигнального символа, дескриптора и символа-конца пакета
  return false;

 //переменная 1
 if (false == mp_pdp->Hex16ToBin(raw_packet, &m_DbgvarDat.var1))
  return false;

 //переменная 2
 if (false == mp_pdp->Hex16ToBin(raw_packet, &m_DbgvarDat.var2))
  return false;

 //переменная 3
 if (false == mp_pdp->Hex16ToBin(raw_packet, &m_DbgvarDat.var3))
  return false;

 //переменная 4
 if (false == mp_pdp->Hex16ToBin(raw_packet, &m_DbgvarDat.var4))
  return false;

 return true;
}

//-----------------------------------------------------------------------
bool CControlApp::Parse_FNNAME_DAT(const BYTE* raw_packet, size_t size)
{
 SECU3IO::FnNameDat& m_FnNameDat = m_recepted_packet.m_FnNameDat;
 if (size != (mp_pdp->isHex() ? 20 : 18))  //размер пакета без сигнального символа, дескриптора и символа-конца пакета
  return false;

 //Общее кол-во наборов (семейств характеристик)
 if (false == mp_pdp->Hex8ToBin(raw_packet,&m_FnNameDat.tables_num))
  return false;

 //номер этого набора характеристик
 if (false == mp_pdp->Hex8ToBin(raw_packet,&m_FnNameDat.index))
  return false;

 //имя этого набора характеристик
 size_t fn_name_size = size - (mp_pdp->getHex8Size()*2);
 strncpy(m_FnNameDat.name, (const char*)raw_packet, fn_name_size);
 m_FnNameDat.name[fn_name_size] = 0;

 //Заменяем символы FF на 0x20 
 for(size_t i = 0; i < fn_name_size; ++i)
  if (((unsigned char)m_FnNameDat.name[i]) == 0xFF)
   m_FnNameDat.name[i] = 0x20;

 return true;
}


//-----------------------------------------------------------------------
bool CControlApp::Parse_STARTR_PAR(const BYTE* raw_packet, size_t size)
{
 SECU3IO::StartrPar& m_StartrPar = m_recepted_packet.m_StartrPar;
 if (size != (mp_pdp->isHex() ? 26 : 13))  //размер пакета без сигнального символа, дескриптора и символа-конца пакета
  return false;

 //Обороты при которых стартер будет выключен
 if (false == mp_pdp->Hex16ToBin(raw_packet,&m_StartrPar.starter_off))
  return false;

 //Обороты перехода с пусковой карты
 if (false == mp_pdp->Hex16ToBin(raw_packet,&m_StartrPar.smap_abandon))
  return false;

 //IAC Crank to run time
 int cranktorun_time = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &cranktorun_time))
  return false;
 m_StartrPar.inj_cranktorun_time = float(cranktorun_time) / 100.0f;

 //Time of afterstart enrichment in strokes
 unsigned char aftstr_strokes = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &aftstr_strokes))
  return false;
 m_StartrPar.inj_aftstr_strokes = aftstr_strokes * 2;

 float discrete = (m_quartz_frq == 20000000 ? 3.2f : 4.0f);
 //prime pulse at -30C
 int prime_cold = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &prime_cold))
  return false;
 m_StartrPar.inj_prime_cold = (float(prime_cold) * discrete) / 1000.0f; //convert to ms
 //prime pulse at 70C
 int prime_hot = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &prime_hot))
  return false;
 m_StartrPar.inj_prime_hot = (float(prime_hot) * discrete) / 1000.0f; //convert to ms
 //prime pulse delay
 unsigned char prime_delay = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &prime_delay))
  return false;
 m_StartrPar.inj_prime_delay = float(prime_delay) / 10.0f;            //convert to seconds

 //flood clear mode entering threshold (% of TPS)
 unsigned char inj_floodclear_tps = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &inj_floodclear_tps))
  return false;
 m_StartrPar.inj_floodclear_tps = float(inj_floodclear_tps) / 2.0f;

 return true;
}


//-----------------------------------------------------------------------
bool CControlApp::Parse_ANGLES_PAR(const BYTE* raw_packet, size_t size)
{
 SECU3IO::AnglesPar& m_AnglesPar = m_recepted_packet.m_AnglesPar;
 if (size != (mp_pdp->isHex() ? 21 : 11))  //размер пакета без сигнального символа, дескриптора и символа-конца пакета
  return false;

 //Максимальный, допустимый УОЗ (число со знаком)
 int max_angle;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&max_angle,true))
  return false;
 m_AnglesPar.max_angle = ((float)max_angle) / m_angle_multiplier;

 //Минимальный, допустимый УОЗ (число со знаком)
 int  min_angle;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&min_angle,true))
  return false;
 m_AnglesPar.min_angle = ((float)min_angle) / m_angle_multiplier;

 //Октан-коррекция УОЗ (число со знаком)
 int angle_corr;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&angle_corr,true))
  return false;
 m_AnglesPar.angle_corr = ((float)angle_corr) / m_angle_multiplier;

 //Скорость уменьшения УОЗ (число со знаком)
 int dec_speed;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&dec_speed,true))
  return false;
 m_AnglesPar.dec_speed = ((float)dec_speed) / m_angle_multiplier;

//Скорость увеличения УОЗ (число со знаком)
 int inc_speed;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&inc_speed,true))
  return false;
 m_AnglesPar.inc_speed = ((float)inc_speed) / m_angle_multiplier;

 //Признак нулевого УОЗ
 if (false == mp_pdp->Hex4ToBin(raw_packet, &m_AnglesPar.zero_adv_ang))
  return false;

 return true;
}

//-----------------------------------------------------------------------
bool CControlApp::Parse_FUNSET_PAR(const BYTE* raw_packet, size_t size)
{
 SECU3IO::FunSetPar& m_FunSetPar = m_recepted_packet.m_FunSetPar;
 if (size != (mp_pdp->isHex() ? 41 : 21))  //размер пакета без сигнального символа, дескриптора и символа-конца пакета
  return false;

 //Номер семейства характеристик используемого для бензина
 if (false == mp_pdp->Hex8ToBin(raw_packet,&m_FunSetPar.fn_benzin))
  return false;

 //Номер семейства характеристик используемого для газа
 if (false == mp_pdp->Hex8ToBin(raw_packet,&m_FunSetPar.fn_gas))
  return false;

 //Нижнее значение давления по оси ДАД
 int map_lower_pressure = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&map_lower_pressure))
  return false;
 m_FunSetPar.map_lower_pressure = ((float)map_lower_pressure) / MAP_PHYSICAL_MAGNITUDE_MULTIPLIER;

 //Верхнее значение давления по оси ДАД
 int map_upper_pressure = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&map_upper_pressure))
  return false;
 m_FunSetPar.map_upper_pressure = ((float)map_upper_pressure) / MAP_PHYSICAL_MAGNITUDE_MULTIPLIER;

 //Смещение кривой ДАД
 int map_curve_offset = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &map_curve_offset, true))
  return false;
 m_FunSetPar.map_curve_offset = ((float)map_curve_offset) * m_adc_discrete;

 //Наклон кривой ДАД
 int map_curve_gradient = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &map_curve_gradient, true))
  return false;
 m_FunSetPar.map_curve_gradient = ((float)map_curve_gradient) / (MAP_PHYSICAL_MAGNITUDE_MULTIPLIER * m_adc_discrete * 128.0f);

 //Смещение кривой ДАД2
 int map2_curve_offset = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &map2_curve_offset, true))
  return false;
 m_FunSetPar.map2_curve_offset = ((float)map2_curve_offset) * m_adc_discrete;

 //Наклон кривой ДАД2
 int map2_curve_gradient = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &map2_curve_gradient, true))
  return false;
 m_FunSetPar.map2_curve_gradient = ((float)map2_curve_gradient) / (MAP_PHYSICAL_MAGNITUDE_MULTIPLIER * m_adc_discrete * 128.0f);

 //TPS sensor curve offset
 int tps_curve_offset = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &tps_curve_offset, true))
  return false;
 m_FunSetPar.tps_curve_offset = ((float)tps_curve_offset) * m_adc_discrete;

 //TPS sensor curve slope
 int tps_curve_gradient = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &tps_curve_gradient, true))
  return false;
 m_FunSetPar.tps_curve_gradient = ((float)tps_curve_gradient) / ((TPS_PHYSICAL_MAGNITUDE_MULTIPLIER*64) * m_adc_discrete * 128.0f);

 //Engine load measurement source
 if (false == mp_pdp->Hex4ToBin(raw_packet,&m_FunSetPar.load_src_cfg))
  return false;

 //Uni.outputs selected for map selection
 BYTE mapsel_uni = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &mapsel_uni))
  return false;
 m_FunSetPar.uni_gas = mapsel_uni >> 4;
 m_FunSetPar.uni_benzin = mapsel_uni & 0xF;
 if (m_FunSetPar.uni_gas == 0xF)
  m_FunSetPar.uni_gas = UNI_OUTPUT_NUM; //disabled
 if (m_FunSetPar.uni_benzin == 0xF)
  m_FunSetPar.uni_benzin = UNI_OUTPUT_NUM; //disabled

 BYTE barocorr_type = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &barocorr_type))
  return false;
 m_FunSetPar.barocorr_type = barocorr_type;

 return true;
}


//-----------------------------------------------------------------------
bool CControlApp::Parse_IDLREG_PAR(const BYTE* raw_packet, size_t size)
{
 SECU3IO::IdlRegPar& m_IdlRegPar = m_recepted_packet.m_IdlRegPar;
 if (size != (mp_pdp->isHex() ? 56 : 28))  //размер пакета без сигнального символа, дескриптора и символа-конца пакета
  return false;

 //Idling regulator flags
 BYTE idl_flags = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet,&idl_flags))
  return false;

 m_IdlRegPar.idl_regul = CHECKBIT8(idl_flags, 0);
 m_IdlRegPar.use_regongas = CHECKBIT8(idl_flags, 1);
 m_IdlRegPar.closed_loop = CHECKBIT8(idl_flags, 2);
 m_IdlRegPar.preg_mode = CHECKBIT8(idl_flags, 3);
 m_IdlRegPar.idl_useiacclongas = CHECKBIT8(idl_flags, 4);

 //Коэффициент регулятора при  положительной ошибке (число со знаком)
 int ifac1;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&ifac1,true))
  return false;
 m_IdlRegPar.ifac1 = ((float)ifac1) / 256.0f;

 //Коэффициент регулятора при  отрицательной ошибке (число со знаком)
 int ifac2;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&ifac2,true))
  return false;
 m_IdlRegPar.ifac2 = ((float)ifac2) / 256.0f;

 //Зона нечувствительности регулятора
 if (false == mp_pdp->Hex16ToBin(raw_packet,&m_IdlRegPar.MINEFR))
  return false;

 //Поддерживаемые обороты
 if (false == mp_pdp->Hex16ToBin(raw_packet,&m_IdlRegPar.idling_rpm))
  return false;

 //Минимальный УОЗ (число со знаком)
 int min_angle;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&min_angle,true))
  return false;
 m_IdlRegPar.min_angle = ((float)min_angle) / m_angle_multiplier;

 //Максимальный УОЗ (число со знаком)
 int max_angle;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&max_angle,true))
  return false;
 m_IdlRegPar.max_angle = ((float)max_angle) / m_angle_multiplier;

 //Порог включения регулятора ХХ по температуре (число со знаком)
 int turn_on_temp = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&turn_on_temp,true))
  return false;
 m_IdlRegPar.turn_on_temp = ((float)turn_on_temp) / TEMP_PHYSICAL_MAGNITUDE_MULTIPLIER;

 //CLOSED LOOP PARAMETERS (FUEL INJECTION)

 //Value (in %) added to IAC position when exiting from closed loop
 unsigned char idl_to_run_add = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &idl_to_run_add))
  return false;
 m_IdlRegPar.idl_to_run_add = ((float)idl_to_run_add) / 2.0f;

 //Value added to target RPM when vehicle starts to run
 unsigned char rpm_on_run_add = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &rpm_on_run_add))
  return false;
 m_IdlRegPar.rpm_on_run_add = rpm_on_run_add * 10;

 //IAC closeed loop proportional coefficient
 int idl_reg_p = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &idl_reg_p))
  return false;
 m_IdlRegPar.idl_reg_p = ((float)idl_reg_p) / 256.0f;

 //IAC closed loop integral coefficient
 int idl_reg_i = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &idl_reg_i))
  return false;
 m_IdlRegPar.idl_reg_i = ((float)idl_reg_i) / 256.0f;

 //coefficient for calculating closed loop entering RPM threshold
 unsigned char idl_coef_thrd1 = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &idl_coef_thrd1))
  return false;
 m_IdlRegPar.idl_coef_thrd1 = (((float)idl_coef_thrd1) / 128.0f) + 1.0f;

 //coefficient for calculating closed loop leaving RPM threshold
 unsigned char idl_coef_thrd2 = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &idl_coef_thrd2))
  return false;
 m_IdlRegPar.idl_coef_thrd2 = (((float)idl_coef_thrd2) / 128.0f) + 1.0f;

 //RPM error limit for integrator
 unsigned char idl_intrpm_lim = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &idl_intrpm_lim))
  return false;
 m_IdlRegPar.idl_intrpm_lim = idl_intrpm_lim * 10;

 //intake manifold pressure on idling
 int idl_map_value = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &idl_map_value))
  return false;
 m_IdlRegPar.idl_map_value = ((float)idl_map_value) / MAP_PHYSICAL_MAGNITUDE_MULTIPLIER;

 //minimum IAC position
 unsigned char idl_iacminpos = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &idl_iacminpos))
  return false;
 m_IdlRegPar.idl_iacminpos = ((float)idl_iacminpos / 2.0f);

 //maximum IAC position
 unsigned char idl_iacmaxpos = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &idl_iacmaxpos))
  return false;
 m_IdlRegPar.idl_iacmaxpos = ((float)idl_iacmaxpos / 2.0f);

 return true;
}

//-----------------------------------------------------------------------
bool CControlApp::Parse_CARBUR_PAR(const BYTE* raw_packet, size_t size)
{
 SECU3IO::CarburPar& m_CarburPar = m_recepted_packet.m_CarburPar;
 if (size != (mp_pdp->isHex() ? 41 : 21))  //размер пакета без сигнального символа, дескриптора и символа-конца пакета
  return false;

 //Нижний порог ЭПХХ (бензин)
 if (false == mp_pdp->Hex16ToBin(raw_packet, &m_CarburPar.ephh_lot))
  return false;

 //Верхний порог ЭПХХ (бензин)
 if (false == mp_pdp->Hex16ToBin(raw_packet, &m_CarburPar.ephh_hit))
  return false;

 //Признак инверсии концевика карбюратора
 if (false == mp_pdp->Hex4ToBin(raw_packet, &m_CarburPar.carb_invers))
  return false;

 //Порог разрежения ЭМР
 int epm_on_threshold = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &epm_on_threshold, true))
  return false;
 m_CarburPar.epm_ont = ((float)epm_on_threshold) / MAP_PHYSICAL_MAGNITUDE_MULTIPLIER;

 //Нижний порог ЭПХХ (газ)
 if (false == mp_pdp->Hex16ToBin(raw_packet, &m_CarburPar.ephh_lot_g))
  return false;

 //Верхний порог ЭПХХ (газ)
 if (false == mp_pdp->Hex16ToBin(raw_packet, &m_CarburPar.ephh_hit_g))
  return false;

 //Задержка выключения клапана ЭПХХ
 unsigned char shutoff_delay;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &shutoff_delay))
  return false;
 m_CarburPar.shutoff_delay = ((float)shutoff_delay) / 100.0f; //переводим в секунды

 //Порог переключения в режим ХХ по ДПДЗ
 unsigned char tps_threshold;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &tps_threshold))
  return false;
 m_CarburPar.tps_threshold = ((float)tps_threshold) / TPS_PHYSICAL_MAGNITUDE_MULTIPLIER;

 int fuelcut_map_thrd=0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &fuelcut_map_thrd))
  return false;
 m_CarburPar.fuelcut_map_thrd = ((float)fuelcut_map_thrd) / MAP_PHYSICAL_MAGNITUDE_MULTIPLIER;

 int fuelcut_cts_thrd=0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &fuelcut_cts_thrd, true))
  return false;
 m_CarburPar.fuelcut_cts_thrd = ((float)fuelcut_cts_thrd) / TEMP_PHYSICAL_MAGNITUDE_MULTIPLIER;

 //Rev.limitting lo threshold
 if (false == mp_pdp->Hex16ToBin(raw_packet, &m_CarburPar.revlim_lot))
  return false;

 //Rev.limitting hi threshold
 if (false == mp_pdp->Hex16ToBin(raw_packet, &m_CarburPar.revlim_hit))
  return false;

 return true;
}

//-----------------------------------------------------------------------
bool CControlApp::Parse_TEMPER_PAR(const BYTE* raw_packet, size_t size)
{
 SECU3IO::TemperPar& m_TemperPar = m_recepted_packet.m_TemperPar;
 if (size != (mp_pdp->isHex() ? 30 : 15))  //размер пакета без сигнального символа, дескриптора и символа-конца пакета
  return false;

 //Temperature flags
 unsigned char tmp_flags = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet,&tmp_flags))
  return false;

 //Признак комплектации ДТОЖ (использования ДТОЖ)
 m_TemperPar.tmp_use = CHECKBIT8(tmp_flags, 0);
 //Флаг использования таблицы для задания зависимости температуры от напрящения ДТОЖ
 m_TemperPar.cts_use_map = CHECKBIT8(tmp_flags, 1);
 //Флаг использования ШИМ для управления вентилятором охлаждения двигателя
 m_TemperPar.vent_pwm = CHECKBIT8(tmp_flags, 2);

 //Для удобства и повышения скорости обработки SECU-3 оперирует с температурой представленной
 //дискретами АЦП (как измеренное значение прямо с датчика)

 //Порог включения вентилятора (число со знаком)
 int vent_on = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&vent_on,true))
  return false;
 m_TemperPar.vent_on = ((float)vent_on) / TEMP_PHYSICAL_MAGNITUDE_MULTIPLIER;

 //Порог выключения вентилятора (число со знаком)
 int vent_off = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&vent_off,true))
  return false;
 m_TemperPar.vent_off = ((float)vent_off) / TEMP_PHYSICAL_MAGNITUDE_MULTIPLIER;

 int vent_pwmfrq = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&vent_pwmfrq))
  return false;
 m_TemperPar.vent_pwmfrq = MathHelpers::Round(1.0/(((double)vent_pwmfrq) / 524288.0));

 //conditioner clutch turn on threshold (voltage from the refrigerant pressure sensor)
 int cond_pvt_on = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &cond_pvt_on))
  return false;
 m_TemperPar.cond_pvt_on = ((float)cond_pvt_on) * m_adc_discrete;

 //conditioner clutch turn off threshold (voltage from the refrigerant pressure sensor)
 int cond_pvt_off = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &cond_pvt_off))
  return false;
 m_TemperPar.cond_pvt_off = ((float)cond_pvt_off) * m_adc_discrete;

 //Minimum RPM for turning on of conditioner clutch
 int cond_min_rpm = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &cond_min_rpm))
  return false;
 m_TemperPar.cond_min_rpm = cond_min_rpm;

 //Cooling fan's timer (sec.)
 int vent_tmr = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &vent_tmr))
  return false;
 m_TemperPar.vent_tmr = vent_tmr / 100.0f;

 return true;
}

//-----------------------------------------------------------------------
bool CControlApp::Parse_ADCRAW_DAT(const BYTE* raw_packet, size_t size)
{
 SECU3IO::RawSensDat& m_RawSensDat = m_recepted_packet.m_RawSensDat;
 if (size != (mp_pdp->isHex() ? 36 : 18))  //размер пакета без сигнального символа, дескриптора и символа-конца пакета
  return false;

 //MAP sensor
 signed int map = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&map,true))
  return false;
 m_RawSensDat.map_value = map * m_adc_discrete;

 //напряжение бортовой сети
 signed int ubat = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&ubat,true))
  return false;
 m_RawSensDat.ubat_value = ubat * m_adc_discrete;

 //температура ОЖ (ДТОЖ)
 signed int temp = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&temp,true))
  return false;
 m_RawSensDat.temp_value = temp * m_adc_discrete;

 //Уровень сигнала детонации
 signed int knock = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&knock,true))
  return false;
 m_RawSensDat.knock_value = knock * m_adc_discrete;

 //Throttle position sensor
 signed int tps = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&tps,true))
  return false;
 m_RawSensDat.tps_value = tps * m_adc_discrete;

 //ADD_I1 input
 signed int add_i1 = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&add_i1,true))
  return false;
 m_RawSensDat.add_i1_value = add_i1 * m_adc_discrete;

 //ADD_I2 input
 signed int add_i2 = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&add_i2,true))
  return false;
 m_RawSensDat.add_i2_value = add_i2 * m_adc_discrete;

 //ADD_I3 input
 signed int add_i3 = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&add_i3,true))
  return false;
 m_RawSensDat.add_i3_value = add_i3 * m_adc_discrete;

 //ADD_I4 input
 signed int add_i4 = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&add_i4,true))
  return false;
 m_RawSensDat.add_i4_value = add_i4 * m_adc_discrete;

 return true;
}


//-----------------------------------------------------------------------
//note: for more information see AVR120 application note.
bool CControlApp::Parse_ADCCOR_PAR(const BYTE* raw_packet, size_t size)
{
 SECU3IO::ADCCompenPar& m_ADCCompenPar = m_recepted_packet.m_ADCCompenPar;
 if (size != (mp_pdp->isHex() ? 96 : 48))  //размер пакета без сигнального символа, дескриптора и символа-конца пакета
  return false;

 signed int map_adc_factor = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&map_adc_factor))
  return false;
 m_ADCCompenPar.map_adc_factor = ((float)map_adc_factor) / 16384;

 signed long map_adc_correction = 0;
 if (false == mp_pdp->Hex32ToBin(raw_packet,&map_adc_correction))
  return false;
 m_ADCCompenPar.map_adc_correction = ((((float)map_adc_correction)/16384.0f) - 0.5f) / m_ADCCompenPar.map_adc_factor;
 m_ADCCompenPar.map_adc_correction*=m_adc_discrete; //в вольты

 signed int ubat_adc_factor = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&ubat_adc_factor))
  return false;
 m_ADCCompenPar.ubat_adc_factor = ((float)ubat_adc_factor) / 16384;

 signed long ubat_adc_correction = 0;
 if (false == mp_pdp->Hex32ToBin(raw_packet,&ubat_adc_correction))
  return false;
 m_ADCCompenPar.ubat_adc_correction = ((((float)ubat_adc_correction)/16384.0f) - 0.5f) / m_ADCCompenPar.ubat_adc_factor;
 m_ADCCompenPar.ubat_adc_correction*=m_adc_discrete; //в вольты

 signed int temp_adc_factor = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&temp_adc_factor))
  return false;
 m_ADCCompenPar.temp_adc_factor = ((float)temp_adc_factor) / 16384;

 signed long temp_adc_correction = 0;
 if (false == mp_pdp->Hex32ToBin(raw_packet,&temp_adc_correction))
  return false;
 m_ADCCompenPar.temp_adc_correction = ((((float)temp_adc_correction)/16384.0f) - 0.5f) / m_ADCCompenPar.temp_adc_factor;
 m_ADCCompenPar.temp_adc_correction*=m_adc_discrete; //в вольты
 
 //TPS sensor
 signed int tps_adc_factor = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&tps_adc_factor))
  return false;
 m_ADCCompenPar.tps_adc_factor = ((float)tps_adc_factor) / 16384;

 signed long tps_adc_correction = 0;
 if (false == mp_pdp->Hex32ToBin(raw_packet,&tps_adc_correction))
  return false;
 m_ADCCompenPar.tps_adc_correction = ((((float)tps_adc_correction)/16384.0f) - 0.5f) / m_ADCCompenPar.tps_adc_factor;
 m_ADCCompenPar.tps_adc_correction*=m_adc_discrete; //в вольты

 //ADD_I1 input
 signed int ai1_adc_factor = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&ai1_adc_factor))
  return false;
 m_ADCCompenPar.ai1_adc_factor = ((float)ai1_adc_factor) / 16384;

 signed long ai1_adc_correction = 0;
 if (false == mp_pdp->Hex32ToBin(raw_packet,&ai1_adc_correction))
  return false;
 m_ADCCompenPar.ai1_adc_correction = ((((float)ai1_adc_correction)/16384.0f) - 0.5f) / m_ADCCompenPar.ai1_adc_factor;
 m_ADCCompenPar.ai1_adc_correction*=m_adc_discrete; //в вольты

 //ADD_I2 input
 signed int ai2_adc_factor = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&ai2_adc_factor))
  return false;
 m_ADCCompenPar.ai2_adc_factor = ((float)ai2_adc_factor) / 16384;

 signed long ai2_adc_correction = 0;
 if (false == mp_pdp->Hex32ToBin(raw_packet,&ai2_adc_correction))
  return false;
 m_ADCCompenPar.ai2_adc_correction = ((((float)ai2_adc_correction)/16384.0f) - 0.5f) / m_ADCCompenPar.ai2_adc_factor;
 m_ADCCompenPar.ai2_adc_correction*=m_adc_discrete; //в вольты

 //ADD_I3 input
 signed int ai3_adc_factor = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&ai3_adc_factor))
  return false;
 m_ADCCompenPar.ai3_adc_factor = ((float)ai3_adc_factor) / 16384;

 signed long ai3_adc_correction = 0;
 if (false == mp_pdp->Hex32ToBin(raw_packet,&ai3_adc_correction))
  return false;
 m_ADCCompenPar.ai3_adc_correction = ((((float)ai3_adc_correction)/16384.0f) - 0.5f) / m_ADCCompenPar.ai3_adc_factor;
 m_ADCCompenPar.ai3_adc_correction*=m_adc_discrete; //в вольты

 //ADD_I4 input
 signed int ai4_adc_factor = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&ai4_adc_factor))
  return false;
 m_ADCCompenPar.ai4_adc_factor = ((float)ai4_adc_factor) / 16384;

 signed long ai4_adc_correction = 0;
 if (false == mp_pdp->Hex32ToBin(raw_packet,&ai4_adc_correction))
  return false;
 m_ADCCompenPar.ai4_adc_correction = ((((float)ai4_adc_correction)/16384.0f) - 0.5f) / m_ADCCompenPar.ai4_adc_factor;
 m_ADCCompenPar.ai4_adc_correction*=m_adc_discrete; //в вольты

 return true;
}

//-----------------------------------------------------------------------
bool CControlApp::Parse_CKPS_PAR(const BYTE* raw_packet, size_t size)
{
 SECU3IO::CKPSPar& m_CKPSPar = m_recepted_packet.m_CKPSPar;
 if (size != (mp_pdp->isHex() ? 20 : 10))  //размер пакета без сигнального символа, дескриптора и символа-конца пакета
  return false;

 //Количество зубьев до в.м.т.
 if (false == mp_pdp->Hex8ToBin(raw_packet,&m_CKPSPar.ckps_cogs_btdc))
  return false;

 //Длительность импульса запуска коммутаторов в зубьях шкива
 if (false == mp_pdp->Hex8ToBin(raw_packet,&m_CKPSPar.ckps_ignit_cogs))
  return false;

 //Кол-во цилиндров двигателя
 if (false == mp_pdp->Hex8ToBin(raw_packet,&m_CKPSPar.ckps_engine_cyl))
  return false;

 //Кол-во зубьев задающего шкива, включая пропущенные
 if (false == mp_pdp->Hex8ToBin(raw_packet, &m_CKPSPar.ckps_cogs_num))
  return false;

 //Кол-во пропущенных зубьев задающего шкива (допустимые значения: 0, 1, 2)
 if (false == mp_pdp->Hex8ToBin(raw_packet, &m_CKPSPar.ckps_miss_num))
  return false;

 //Hall sensor flags
 unsigned char flags = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &flags))
  return false;
 m_CKPSPar.ckps_rising_spark = CHECKBIT8(flags, 0);
 m_CKPSPar.ckps_use_cam_ref = CHECKBIT8(flags, 1);
 m_CKPSPar.ckps_edge_type = CHECKBIT8(flags, 2);
 m_CKPSPar.ref_s_edge_type = CHECKBIT8(flags, 3);
 m_CKPSPar.ckps_merge_ign_outs = CHECKBIT8(flags, 4);

 //Hall shutter's window width
 int wnd_width;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&wnd_width,true))
  return false;
 m_CKPSPar.hall_wnd_width = ((float)wnd_width) / m_angle_multiplier;

 //Hall degrees BTDC
 int degrees_btdc;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&degrees_btdc,true))
  return false;
 m_CKPSPar.hall_degrees_btdc = ((float)degrees_btdc) / m_angle_multiplier;

 return true;
}

//-----------------------------------------------------------------------
bool CControlApp::Parse_OP_COMP_NC(const BYTE* raw_packet, size_t size)
{
 SECU3IO::OPCompNc& m_OPCompNc = m_recepted_packet.m_OPCompNc;
 if (size != (mp_pdp->isHex() ? 4 : 2))  //размер пакета без сигнального символа, дескриптора и символа-конца пакета
  return false;

 //Код завершенной операции
 if (false == mp_pdp->Hex8ToBin(raw_packet, &m_OPCompNc.opdata))
  return false;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &m_OPCompNc.opcode))
  return false;

 return true;
}

//-----------------------------------------------------------------------
bool CControlApp::Parse_KNOCK_PAR(const BYTE* raw_packet, size_t size)
{
 SECU3IO::KnockPar& m_KnockPar = m_recepted_packet.m_KnockPar;
 if (size != (mp_pdp->isHex() ? (14+17) : 16))  //размер пакета без сигнального символа, дескриптора и символа-конца пакета
  return false;

 //Разрешен/запрещен
 if (false == mp_pdp->Hex4ToBin(raw_packet,&m_KnockPar.knock_use_knock_channel))
  return false;

 //Частота ПФ
 unsigned char knock_bpf_frequency;
 if (false == mp_pdp->Hex8ToBin(raw_packet,&knock_bpf_frequency))
  return false;
 m_KnockPar.knock_bpf_frequency = knock_bpf_frequency;

 //Начало фазового окна
 int  knock_k_wnd_begin_angle;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&knock_k_wnd_begin_angle,true))
  return false;
 m_KnockPar.knock_k_wnd_begin_angle = ((float)knock_k_wnd_begin_angle) / m_angle_multiplier;

 //Конец фазового окна
 int  knock_k_wnd_end_angle;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&knock_k_wnd_end_angle,true))
  return false;
 m_KnockPar.knock_k_wnd_end_angle = ((float)knock_k_wnd_end_angle) / m_angle_multiplier;

 //Постоянная времени интегрирования
 unsigned char knock_int_time_const;
 if (false == mp_pdp->Hex8ToBin(raw_packet,&knock_int_time_const))
  return false;
 m_KnockPar.knock_int_time_const = knock_int_time_const;

 //-----------------
 //Шаг смещения УОЗ при детонации
 int knock_retard_step;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&knock_retard_step,true))
  return false;
 m_KnockPar.knock_retard_step = ((float)knock_retard_step) / m_angle_multiplier;

 //Шаг восстановления УОЗ
 int knock_advance_step;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&knock_advance_step,true))
  return false;
 m_KnockPar.knock_advance_step = ((float)knock_advance_step) / m_angle_multiplier;

 //Максимальное смещение УОЗ
 int knock_max_retard;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&knock_max_retard,true))
  return false;
 m_KnockPar.knock_max_retard = ((float)knock_max_retard) / m_angle_multiplier;

 //Порог детонации
 int knock_threshold;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&knock_threshold, false))
  return false;
 m_KnockPar.knock_threshold = ((float)knock_threshold) * m_adc_discrete;

 //Задержка восстановления УОЗ
 unsigned char knock_recovery_delay;
 if (false == mp_pdp->Hex8ToBin(raw_packet,&knock_recovery_delay))
  return false;
 m_KnockPar.knock_recovery_delay = knock_recovery_delay;

 return true;
}

//-----------------------------------------------------------------------
bool CControlApp::Parse_CE_ERR_CODES(const BYTE* raw_packet, size_t size)
{
 SECU3IO::CEErrors& m_CEErrors = m_recepted_packet.m_CEErrors;
 if (size != (mp_pdp->isHex() ? 4 : 2))  //размер пакета без сигнального символа, дескриптора и символа-конца пакета
  return false;

 int flags = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&flags))
  return false;
 m_CEErrors.flags = flags;

 return true;
}

//-----------------------------------------------------------------------
bool CControlApp::Parse_CE_SAVED_ERR(const BYTE* raw_packet, size_t size)
{
 SECU3IO::CEErrors& m_CEErrors = m_recepted_packet.m_CEErrors;
 if (size != (mp_pdp->isHex() ? 4 : 2))  //размер пакета без сигнального символа, дескриптора и символа-конца пакета
  return false;

 int flags = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&flags))
  return false;
 m_CEErrors.flags = flags;

 return true;
}

//-----------------------------------------------------------------------
bool CControlApp::Parse_FWINFO_DAT(const BYTE* raw_packet, size_t size)
{
 SECU3IO::FWInfoDat& m_FWInfoDat = m_recepted_packet.m_FWInfoDat;
 if (size != (FW_SIGNATURE_INFO_SIZE + (mp_pdp->isHex() ? 10 : 5)))  //размер пакета без сигнального символа, дескриптора и символа-конца пакета
  return false;

 //строка с информацией 
 strncpy(m_FWInfoDat.info,(const char*)raw_packet, FW_SIGNATURE_INFO_SIZE);
 m_FWInfoDat.info[FW_SIGNATURE_INFO_SIZE] = 0;
 raw_packet+=FW_SIGNATURE_INFO_SIZE;

 //биты опций прошивки
 DWORD options = 0;
 if (false == mp_pdp->Hex32ToBin(raw_packet,&options))
  return false;
 m_FWInfoDat.options = options;

 unsigned char fw_version;
 if (false == mp_pdp->Hex8ToBin(raw_packet,&fw_version))
  return false;
 m_FWInfoDat.fw_version[0] = fw_version & 0x0F;
 m_FWInfoDat.fw_version[1] = fw_version >> 4;

 return true;
}

//-----------------------------------------------------------------------
bool CControlApp::Parse_MISCEL_PAR(const BYTE* raw_packet, size_t size)
{
 SECU3IO::MiscelPar& m_MiscPar = m_recepted_packet.m_MiscelPar;
 if (size != (mp_pdp->isHex() ? 27 : 14))  //размер пакета без сигнального символа, дескриптора и символа-конца пакета
  return false;

 //Делитель для UART-а
 int divisor = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &divisor))
  return false;

 m_MiscPar.baud_rate = 0;

 for(size_t i = 0; i < SECU3IO::SECU3_ALLOWABLE_UART_DIVISORS_COUNT; ++i)
  if (SECU3IO::secu3_allowable_uart_divisors[i].second == divisor)
    m_MiscPar.baud_rate = SECU3IO::secu3_allowable_uart_divisors[i].first;
 
 ASSERT(m_MiscPar.baud_rate);

 //Период посылки пакетов в десятках миллисекунд
 unsigned char period_t_ms = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet,&period_t_ms))
  return false;

 m_MiscPar.period_ms = period_t_ms * 10;

 //Отсечка разрешена/запрещена
 if (false == mp_pdp->Hex4ToBin(raw_packet, &m_MiscPar.ign_cutoff))
  return false;

 //Обороты отсечки
 if (false == mp_pdp->Hex16ToBin(raw_packet, &m_MiscPar.ign_cutoff_thrd, true))
  return false;

 //Выход ДХ: Начало импульса в зубьях шкива относительно ВМТ
 signed int start = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &start))
  return false;

 m_MiscPar.hop_start_cogs = start;

 //Выход ДХ: Длительность импульса в зубьях шкива
 unsigned char duration = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &duration))
  return false;
 m_MiscPar.hop_durat_cogs = duration;

 //Fuel pump flags
 unsigned char flpmp_flags = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &flpmp_flags))
  return false;
 m_MiscPar.flpmp_offongas = (flpmp_flags & 0x1) != 0;
 m_MiscPar.inj_offongas = (flpmp_flags & 0x2) != 0;

 //Начальный порог расхода воздуха для EVAP
 int evap_afbegin = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &evap_afbegin))
  return false;
 m_MiscPar.evap_afbegin = ((float)evap_afbegin) * 32.0f;

 //Наклон прямой расхода воздуха для EVAP
 int evap_afslope = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &evap_afslope))
  return false;
 m_MiscPar.evap_afslope = (((float)evap_afslope) / 1048576.0f) / 32.0f; //2^20

 //Fuel pump timeout for cranking
 BYTE fp_timeout_strt = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &fp_timeout_strt))
  return false;
 m_MiscPar.fp_timeout_strt = (((float)fp_timeout_strt) / 10.0f);

 return true;
}

//-----------------------------------------------------------------------
bool CControlApp::Parse_EDITAB_PAR(const BYTE* raw_packet, size_t size)
{
 SECU3IO::EditTabPar& m_EditTabPar = m_recepted_packet.m_EditTabPar;
 //check for 16 and 24 byte packets
 if (mp_pdp->isHex() ? (size < 6 || size > 52) : (size < 3 || size > 26))  //размер пакета без сигнального символа, дескриптора и символа-конца пакета
  return false;

 //код таблицы в наборе
 if (false == mp_pdp->Hex8ToBin(raw_packet, &m_EditTabPar.tab_id))
  return false;

 if (m_EditTabPar.tab_id != ETMT_STRT_MAP && m_EditTabPar.tab_id != ETMT_IDLE_MAP && m_EditTabPar.tab_id != ETMT_WORK_MAP &&
     m_EditTabPar.tab_id != ETMT_TEMP_MAP && m_EditTabPar.tab_id != ETMT_NAME_STR && m_EditTabPar.tab_id != ETMT_VE_MAP &&
     m_EditTabPar.tab_id != ETMT_AFR_MAP && m_EditTabPar.tab_id != ETMT_CRNK_MAP && m_EditTabPar.tab_id != ETMT_WRMP_MAP &&
     m_EditTabPar.tab_id != ETMT_DEAD_MAP && m_EditTabPar.tab_id != ETMT_IDLR_MAP && m_EditTabPar.tab_id != ETMT_IDLC_MAP &&
     m_EditTabPar.tab_id != ETMT_AETPS_MAP && m_EditTabPar.tab_id != ETMT_AERPM_MAP && m_EditTabPar.tab_id != ETMT_AFTSTR_MAP &&
     m_EditTabPar.tab_id != ETMT_IT_MAP && m_EditTabPar.tab_id != ETMT_ITRPM_MAP && m_EditTabPar.tab_id != ETMT_RIGID_MAP &&
     m_EditTabPar.tab_id != ETMT_EGOCRV_MAP && m_EditTabPar.tab_id != ETMT_IACC_MAP && m_EditTabPar.tab_id != ETMT_IACCW_MAP &&
     m_EditTabPar.tab_id != ETMT_IATCLT_MAP && m_EditTabPar.tab_id != ETMT_TPSSWT_MAP && m_EditTabPar.tab_id != ETMT_GTSC_MAP &&
     m_EditTabPar.tab_id != ETMT_GPSC_MAP && m_EditTabPar.tab_id != ETMT_ATSC_MAP)
  return false;

 //check for 16-byte packets
 if ((m_EditTabPar.tab_id != ETMT_VE_MAP) && (m_EditTabPar.tab_id != ETMT_IT_MAP) && (mp_pdp->isHex() ? (size < 6 || size > 36) : (size < 3 || size > 18))) 
  return false;

 //адрес фрагмента данных в таблице (смещение в таблице)
 unsigned char address;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &address))
  return false;
 m_EditTabPar.address = address;

 size-=(2 * mp_pdp->getHex8Size());

 if (m_EditTabPar.tab_id != ETMT_NAME_STR)
 {
  size_t div;
  size_t data_size = 0;
  float discrete = (m_quartz_frq == 20000000 ? 3.2f : 4.0f);
  if (m_EditTabPar.tab_id == ETMT_CRNK_MAP || m_EditTabPar.tab_id == ETMT_DEAD_MAP || m_EditTabPar.tab_id == ETMT_RIGID_MAP || m_EditTabPar.tab_id == ETMT_EGOCRV_MAP || m_EditTabPar.tab_id == ETMT_IACC_MAP || m_EditTabPar.tab_id == ETMT_IATCLT_MAP)
  {
   div = mp_pdp->isHex() ? 4 : 2;
   if (size % div) // 1 byte in HEX is 2 symbols
    return false;

   //фрагмент с данными (float)
   for(size_t i = 0; i < size / div; ++i)
   {
    int value;
    if (false == mp_pdp->Hex16ToBin(raw_packet, &value))
     return false;

    if (m_EditTabPar.tab_id == ETMT_IATCLT_MAP)
     m_EditTabPar.table_data[i] = (address >= INJ_IATCLT_CORR_SIZE) ? (((float)value) * 32.0f) : (((float)value) / 8192.0f);
    else if (m_EditTabPar.tab_id == ETMT_IACC_MAP)
     m_EditTabPar.table_data[i] = (address >= INJ_IAC_CORR_SIZE) ? (((float)value) / 128.0f) : (((float)(((signed short)value) + 8192)) / 8192.0f);
    else if (m_EditTabPar.tab_id == ETMT_RIGID_MAP)
     m_EditTabPar.table_data[i] = ((float)value) / 128.0f;  //convert to user readble value
    else if (m_EditTabPar.tab_id == ETMT_EGOCRV_MAP)
     m_EditTabPar.table_data[i] = (address > 15) ? (value * ADC_DISCRETE) : (((float)value) / EGO_CURVE_M_FACTOR);
    else
     m_EditTabPar.table_data[i] = (((float)value) * discrete) / 1000.0f;  //convert to ms
    ++data_size;
   }
  }
  else
  {
   div = mp_pdp->isHex() ? 2 : 1;
   if (size % div) // 1 byte in HEX is 2 symbols
    return false;

   if (m_EditTabPar.tab_id == ETMT_VE_MAP) //volumetric efficiency map
   { //VE
    BYTE buff[32]; unsigned char value;
    for(size_t i = 0; i < size / div; ++i)
    {     
     if (false == mp_pdp->Hex8ToBin(raw_packet, &value))
      return false;
     buff[i] = value;
    }

    if (size % 3) //must contain whole number of cells
     return false;

    size_t numOfCells = (size / 3) * 2;
    for(size_t i = 0; i < numOfCells; ++i)
     m_EditTabPar.table_data[i] = ((float)w12GetCell(buff, i)) / VE_MAPS_M_FACTOR;
    data_size = numOfCells;
   }
   else if (m_EditTabPar.tab_id == ETMT_IT_MAP) //injection timing map
   {
    BYTE buff[32]; unsigned char value;
    for(size_t i = 0; i < size / div; ++i)
    {     
     if (false == mp_pdp->Hex8ToBin(raw_packet, &value))
      return false;
     buff[i] = value;
    }

    if (size % 3) //must contain whole number of cells
     return false;

    size_t numOfCells = (size / 3) * 2;
    for(size_t i = 0; i < numOfCells; ++i)
     m_EditTabPar.table_data[i] = ((float)w12GetCell(buff, i)) / IT_MAPS_M_FACTOR;
    data_size = numOfCells;
   }
   else
   {
    //data fragment (float)
    for(size_t i = 0; i < size / div; ++i)
    {
     unsigned char value;
     if (false == mp_pdp->Hex8ToBin(raw_packet, &value))
      return false;
     else if (m_EditTabPar.tab_id == ETMT_AFR_MAP)
      m_EditTabPar.table_data[i] = MathHelpers::RoundP1((((float)value) / AFR_MAPS_M_FACTOR) + 8.0f);
     else if (m_EditTabPar.tab_id == ETMT_WRMP_MAP)
      m_EditTabPar.table_data[i] = ((float)value) / WRMP_MAPS_M_FACTOR;
     else if (m_EditTabPar.tab_id == ETMT_AFTSTR_MAP)
      m_EditTabPar.table_data[i] = ((float)value) / AFTSTR_MAPS_M_FACTOR;
     else if (m_EditTabPar.tab_id == ETMT_IDLR_MAP || m_EditTabPar.tab_id == ETMT_IDLC_MAP)
      m_EditTabPar.table_data[i] = ((float)value) / IACPOS_MAPS_M_FACTOR;
     else if (m_EditTabPar.tab_id == ETMT_AETPS_MAP)
      m_EditTabPar.table_data[i] = (i >= INJ_AE_TPS_LOOKUP_TABLE_SIZE)?(float((signed char)value)/AETPSB_MAPS_M_FACTOR):(float(value)-AETPSV_MAPS_ADDER);
     else if (m_EditTabPar.tab_id == ETMT_AERPM_MAP)
      m_EditTabPar.table_data[i] = ((float)value) / ((i >= INJ_AE_RPM_LOOKUP_TABLE_SIZE)?AERPMB_MAPS_M_FACTOR:AERPMV_MAPS_M_FACTOR);
     else if (m_EditTabPar.tab_id == ETMT_ITRPM_MAP)
      m_EditTabPar.table_data[i] = ((float)((unsigned char)value)) * 10.0f;
     else if (m_EditTabPar.tab_id == ETMT_IACCW_MAP)
      m_EditTabPar.table_data[i] =  (address >= INJ_IAC_CORR_W_SIZE) ? (((float)value) / 2.0f) : (((float)value) / 256.0f);
     else if (m_EditTabPar.tab_id == ETMT_TPSSWT_MAP)
      m_EditTabPar.table_data[i] = ((float)value) / TPSSWT_MAPS_M_FACTOR;
     else if (m_EditTabPar.tab_id == ETMT_GTSC_MAP)
      m_EditTabPar.table_data[i] = ((float)value) / 128.0f;
     else if (m_EditTabPar.tab_id == ETMT_GPSC_MAP)
      m_EditTabPar.table_data[i] =  (i >= INJ_GPS_CORR_SIZE) ? (((float)value) * 2.0f) : (((float)value) / 128.0f);
     else if (m_EditTabPar.tab_id == ETMT_ATSC_MAP)
      m_EditTabPar.table_data[i] = ((float)value) / 128.0f;
     else
      m_EditTabPar.table_data[i] = ((float)((signed char)value)) / AA_MAPS_M_FACTOR;
     ++data_size;
    }
   }
  }
  m_EditTabPar.data_size = data_size;
 }
 else
 {
  //фрагмент с данными (текстовая информация)  
  std::string raw_string((char*)raw_packet, size);
  OemToChar(raw_string.c_str(), m_EditTabPar.name_data);
  m_EditTabPar.data_size = raw_string.size();
 }
 
 return true;
}

//-----------------------------------------------------------------------
bool CControlApp::Parse_ATTTAB_PAR(const BYTE* raw_packet, size_t size)
{
 SECU3IO::SepTabPar& m_AttTabPar = m_recepted_packet.m_SepTabPar;
 if (mp_pdp->isHex() ? (size < 4 || size > 34) : (size < 2 || size > 17))  //размер пакета без сигнального символа, дескриптора и символа-конца пакета
  return false;

 //адрес фрагмента данных в таблице (смещение в таблице)
 unsigned char address;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &address))
  return false;
 m_AttTabPar.address = address;

 size-=mp_pdp->getHex8Size();
 size_t div = mp_pdp->isHex() ? 2 : 1;
 if (size % div) // 1 byte in HEX is 2 symbols
  return false;

 //фрагмент с данными (коды коэффициентов усиления)
 size_t data_size = 0;
 for(size_t i = 0; i < size / div; ++i)
 {
  unsigned char value;
  if (false == mp_pdp->Hex8ToBin(raw_packet, &value))
   return false;
  m_AttTabPar.table_data[i] = value;
  ++data_size;
 }
 m_AttTabPar.data_size = data_size;
 
 return true;
}

//-----------------------------------------------------------------------
bool CControlApp::Parse_RPMGRD_PAR(const BYTE* raw_packet, size_t size)
{
 SECU3IO::SepTabPar& m_RpmGrdPar = m_recepted_packet.m_SepTabPar;
 if (mp_pdp->isHex() ? (size != 66) : (size != 33))  //размер пакета без сигнального символа, дескриптора и символа-конца пакета
  return false;

 //адрес фрагмента данных в таблице (смещение в таблице)
 unsigned char address;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &address))
  return false;
 m_RpmGrdPar.address = address;
 if (0!=address)
  return false;  //address must be always zero

 size-=mp_pdp->getHex8Size();
 size_t div = mp_pdp->isHex() ? 2*2 : 1*2;
 if (size % div) // 1 byte in HEX is 2 symbols
  return false;

 //фрагмент с данными (сетка оборотов)
 size_t data_size = 0;
 for(size_t i = 0; i < size / div; ++i)
 {
  unsigned char lo_byte, hi_byte;
  if (false == mp_pdp->Hex8ToBin(raw_packet, &lo_byte))
   return false;
  if (false == mp_pdp->Hex8ToBin(raw_packet, &hi_byte))
   return false;

  m_RpmGrdPar.table_data[i] = MAKEWORD(lo_byte, hi_byte);
  ++data_size;
 }
 m_RpmGrdPar.data_size = data_size;
 
 return true;
}

//-----------------------------------------------------------------------
bool CControlApp::Parse_DIAGINP_DAT(const BYTE* raw_packet, size_t size)
{
 SECU3IO::DiagInpDat& m_DiagInpDat = m_recepted_packet.m_DiagInpDat;
 if (size != (mp_pdp->isHex() ? 46 : 23))  //размер пакета без сигнального символа, дескриптора и символа-конца пакета
  return false;

 //flags variable
 BYTE flags = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &flags))
  return false;
 m_DiagInpDat.f_secu3t = flags;

 //напряжение бортовой сети
 int voltage = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &voltage))
  return false;
 m_DiagInpDat.voltage = ((float)voltage) * m_adc_discrete;

 //датчик абсолютного давления
 int map = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &map))
  return false;
 m_DiagInpDat.map = ((float)map) * m_adc_discrete;

 //датчик температуры охлаждающей жидкости
 int temp = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &temp))
  return false;
 m_DiagInpDat.temp = ((float)temp) * m_adc_discrete;

 //дополнительный I1
 int add_i1 = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &add_i1))
  return false;
 m_DiagInpDat.add_i1 = ((float)add_i1) * m_adc_discrete;

 //дополнительный I2
 int add_i2 = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &add_i2))
  return false;
 m_DiagInpDat.add_i2 = ((float)add_i2) * m_adc_discrete;

 //дополнительный I3 (SECU-3i)
 int add_i3 = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &add_i3))
  return false;
 m_DiagInpDat.add_i3 = ((float)add_i3) * m_adc_discrete;

 //дополнительный I4 (SECU-3i, reserved)
 int add_i4 = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &add_i4))
  return false;
 m_DiagInpDat.add_i4 = ((float)add_i4) * m_adc_discrete;

 //датчик положения дроссельной заслонки (концевик карбюратора)
 int carb = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &carb))
  return false;
 m_DiagInpDat.carb = ((float)carb) * m_adc_discrete;

 //Датчик детонации 1
 int ks_1 = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &ks_1))
  return false;
 m_DiagInpDat.ks_1 = ((float)ks_1) * m_adc_discrete;

 //Датчик детонации 2
 int ks_2 = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &ks_2))
  return false;
 m_DiagInpDat.ks_2 = ((float)ks_2) * m_adc_discrete;

 //байт с состоянием цифровых входов
 int word = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &word))
  return false;

 //газовый клапан, ДПКВ, ДНО(VR), ДФ, "Bootloader", "Default EEPROM"
 m_DiagInpDat.gas    = CHECKBIT16(word, 0);
 m_DiagInpDat.ckps   = CHECKBIT16(word, 1);
 m_DiagInpDat.ref_s  = CHECKBIT16(word, 2);
 m_DiagInpDat.ps     = CHECKBIT16(word, 3);
 m_DiagInpDat.bl     = CHECKBIT16(word, 4);
 m_DiagInpDat.de     = CHECKBIT16(word, 5);
 m_DiagInpDat.ign_i  = CHECKBIT16(word, 6); //SECU-3i
 m_DiagInpDat.cond_i = CHECKBIT16(word, 7); //SECU-3i
 m_DiagInpDat.epas_i = CHECKBIT16(word, 8); //SECU-3i

 return true;
}

//-----------------------------------------------------------------------
bool CControlApp::Parse_CHOKE_PAR(const BYTE* raw_packet, size_t size)
{
 SECU3IO::ChokePar& m_ChokePar = m_recepted_packet.m_ChokePar;
 if (size != (mp_pdp->isHex() ? 27 : 14))  //размер пакета без сигнального символа, дескриптора и символа-конца пакета
  return false;

 //Number of stepper motor steps
 if (false == mp_pdp->Hex16ToBin(raw_packet, &m_ChokePar.sm_steps))
  return false;

 //choke testing mode command/state (it is fake parameter)
 if (false == mp_pdp->Hex4ToBin(raw_packet, &m_ChokePar.testing))
  return false;

 //manual position setting delta (it is fake parameter and must be zero in this ingoing packet)
 int delta = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &delta))
  return false;
 m_ChokePar.manual_delta = delta;

 //Choke RPM regulator integral factor
 int choke_rpm_if;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &choke_rpm_if))
  return false;
 m_ChokePar.choke_rpm_if = ((float)choke_rpm_if) / 1024.0f;

 //Startup correction apply time (2 points)
 int choke_corr_time;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &choke_corr_time))
  return false;
 m_ChokePar.choke_corr_time[0] = ((float)choke_corr_time / 100.0f);

 if (false == mp_pdp->Hex16ToBin(raw_packet, &choke_corr_time))
  return false;
 m_ChokePar.choke_corr_time[1] = ((float)choke_corr_time / 100.0f);

 //Choke flags
 BYTE choke_flags = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &choke_flags))
  return false;
 m_ChokePar.useclrpmreg = CHECKBIT8(choke_flags, 0);
 m_ChokePar.offrpmreg_ongas = CHECKBIT8(choke_flags, 1);
 m_ChokePar.usethrottle_pos = CHECKBIT8(choke_flags, 2);
 m_ChokePar.sm_maxfreqinit = CHECKBIT8(choke_flags, 3);

 //frequency of stepper motor's pulses
 int sm_freq = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &sm_freq))
  return false;
 m_ChokePar.sm_freq = sm_freq;

 //IAC Crank to run time
 int cranktorun_time = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &cranktorun_time))
  return false;
 m_ChokePar.inj_cranktorun_time = float(cranktorun_time) / 100.0f;

 return true;
}

//-----------------------------------------------------------------------
bool CControlApp::Parse_GASDOSE_PAR(const BYTE* raw_packet, size_t size)
{
 SECU3IO::GasdosePar& m_GasdosePar = m_recepted_packet.m_GasdosePar;
 if (size != (mp_pdp->isHex() ? 24 : 13))  //размер пакета без сигнального символа, дескриптора и символа-конца пакета
  return false;

 //Number of stepper motor steps
 if (false == mp_pdp->Hex16ToBin(raw_packet, &m_GasdosePar.gd_steps))
  return false;

 //choke testing mode command/state (it is fake parameter)
 if (false == mp_pdp->Hex4ToBin(raw_packet, &m_GasdosePar.testing))
  return false;

 //manual position setting delta (it is fake parameter and must be zero in this ingoing packet)
 int delta = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &delta))
  return false;
 m_GasdosePar.manual_delta = delta;

 //Closing in fuel cut mode
 BYTE fc_closing = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &fc_closing))
  return false;
 m_GasdosePar.fc_closing = ((float)fc_closing) / 2.0f;

 int corrlimit_p = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &corrlimit_p))
  return false;
 m_GasdosePar.lam_corr_limit_p = (float(corrlimit_p) / 512.0f) * 100.0f;

 int corrlimit_m = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &corrlimit_m))
  return false;
 m_GasdosePar.lam_corr_limit_m = (float(corrlimit_m) / 512.0f) * 100.0f;

 //Stoichiometric value for gas
 int lam_stoichval = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &lam_stoichval))
  return false;
 m_GasdosePar.lam_stoichval = ((float)lam_stoichval) / 128.0f;

 //frequency of stepper motor's pulses
 int gd_freq = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &gd_freq))
  return false;
 m_GasdosePar.gd_freq = gd_freq;

 //Flag
 int gd_maxfreqinit = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &gd_maxfreqinit))
  return false;
 m_GasdosePar.gd_maxfreqinit = gd_maxfreqinit;

 return true;
}

//-----------------------------------------------------------------------
bool CControlApp::Parse_SECUR_PAR(const BYTE* raw_packet, size_t size)
{
 SECU3IO::SecurPar& m_SecurPar = m_recepted_packet.m_SecurPar;
 if (size != (mp_pdp->isHex() ? 28 : 15))  //размер пакета без сигнального символа, дескриптора и символа-конца пакета
  return false;

 //Number of characters in name (must be zero)
 BYTE numName = 0;
 if (false == mp_pdp->Hex4ToBin(raw_packet, &numName))
  return false;

 //Number of characters in password (must be zero)
 BYTE numPass = 0;
 if (false == mp_pdp->Hex4ToBin(raw_packet, &numPass))
  return false;

 if (numName || numPass)
  return false;

 _tcscpy(m_SecurPar.bt_name, _T(""));
 _tcscpy(m_SecurPar.bt_pass, _T(""));

 //Bluetooth and security flags
 unsigned char flags = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &flags))
  return false;

 m_SecurPar.use_bt   = CHECKBIT8(flags, 0);
 m_SecurPar.set_btbr = CHECKBIT8(flags, 1);
 m_SecurPar.use_imm  = CHECKBIT8(flags, 2);
 m_SecurPar.use_respar = CHECKBIT8(flags, 3);
 m_SecurPar.chk_fwcrc = CHECKBIT8(flags, 4);
 m_SecurPar.bt_type = CHECKBIT8(flags, 5);

 //Parse out iButton keys
 BYTE key[IBTN_KEY_SIZE]; int i, j;
 for(j = 0; j < IBTN_KEYS_NUM; ++j)
 {
  for(i = 0; i < IBTN_KEY_SIZE; ++i)
  {
   if (false == mp_pdp->Hex8ToBin(raw_packet, &key[i]))
    return false;
  }
  memcpy(m_SecurPar.ibtn_keys[j], key, IBTN_KEY_SIZE);
 }

 return true;
}

//-----------------------------------------------------------------------
bool CControlApp::Parse_UNIOUT_PAR(const BYTE* raw_packet, size_t size)
{
 SECU3IO::UniOutPar& m_UniOutPar = m_recepted_packet.m_UniOutPar;
 if (size != (mp_pdp->isHex() ? 67 : 34))  //размер пакета без сигнального символа, дескриптора и символа-конца пакета
  return false;

 CondEncoder cen(m_quartz_frq, m_period_distance);

 for(int oi = 0; oi < UNI_OUTPUT_NUM; ++oi)
 {
  unsigned char flags = 0;
  if (false == mp_pdp->Hex8ToBin(raw_packet, &flags))
   return false;
  m_UniOutPar.out[oi].logicFunc = flags >> 4;
  m_UniOutPar.out[oi].invers_1 = (flags & 0x01) != 0;
  m_UniOutPar.out[oi].invers_2 = (flags & 0x02) != 0;

  unsigned char cond1 = 0;
  if (false == mp_pdp->Hex8ToBin(raw_packet, &cond1))
   return false;
  m_UniOutPar.out[oi].condition1 = cond1;

  unsigned char cond2 = 0;
  if (false == mp_pdp->Hex8ToBin(raw_packet, &cond2))
   return false;
  m_UniOutPar.out[oi].condition2 = cond2;

  int on_thrd_1 = 0;
  if (false == mp_pdp->Hex16ToBin(raw_packet, &on_thrd_1, cen.isSigned(cond1)))
   return false;
  m_UniOutPar.out[oi].on_thrd_1 = cen.UniOutDecodeCondVal(on_thrd_1, cond1);

  int off_thrd_1 = 0;
  if (false == mp_pdp->Hex16ToBin(raw_packet, &off_thrd_1, cen.isSigned(cond1)))
   return false;
  m_UniOutPar.out[oi].off_thrd_1 = cen.UniOutDecodeCondVal(off_thrd_1, cond1);

  int on_thrd_2 = 0;
  if (false == mp_pdp->Hex16ToBin(raw_packet, &on_thrd_2, cen.isSigned(cond2)))
   return false;
  m_UniOutPar.out[oi].on_thrd_2 = cen.UniOutDecodeCondVal(on_thrd_2, cond2);

  int off_thrd_2 = 0;
  if (false == mp_pdp->Hex16ToBin(raw_packet, &off_thrd_2, cen.isSigned(cond2)))
   return false;
  m_UniOutPar.out[oi].off_thrd_2 = cen.UniOutDecodeCondVal(off_thrd_2, cond2);
 }

 //logic function for 1st and 2nd outputs
 BYTE lf12 = 0;
 if (false == mp_pdp->Hex4ToBin(raw_packet, &lf12))
  return false;
 m_UniOutPar.logicFunc12 = lf12;

 return true;
}

//-----------------------------------------------------------------------
bool CControlApp::Parse_INJCTR_PAR(const BYTE* raw_packet, size_t size)
{
 SECU3IO::InjctrPar& m_InjctrPar = m_recepted_packet.m_InjctrPar;
 if (size != (mp_pdp->isHex() ? 56 : 29))  //размер пакета без сигнального символа, дескриптора и символа-конца пакета
  return false;

 unsigned char inj_flags = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &inj_flags))
  return false;
 m_InjctrPar.inj_usetimingmap[0] = CHECKBIT8(inj_flags, 0);
 m_InjctrPar.inj_usetimingmap[1] = CHECKBIT8(inj_flags, 1);
 m_InjctrPar.inj_useaddcorrs = CHECKBIT8(inj_flags, 2);
 m_InjctrPar.inj_useairden = CHECKBIT8(inj_flags, 3);
 m_InjctrPar.inj_usediffpress = CHECKBIT8(inj_flags, 4);

 unsigned char inj_config = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &inj_config))
  return false;
 m_InjctrPar.inj_config[0] = GETHI4BITS(inj_config);      //configuration
 m_InjctrPar.inj_squirt_num[0] = GETLO4BITS(inj_config);  //number of squirts per cycle
 //second fuel:
 if (false == mp_pdp->Hex8ToBin(raw_packet, &inj_config))
  return false;
 m_InjctrPar.inj_config[1] = GETHI4BITS(inj_config);      //configuration
 m_InjctrPar.inj_squirt_num[1] = GETLO4BITS(inj_config);  //number of squirts per cycle

 int inj_flow_rate = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &inj_flow_rate))
  return false;
 m_InjctrPar.inj_flow_rate[0] = float(inj_flow_rate) / 64.0f;
 //second fuel:
 if (false == mp_pdp->Hex16ToBin(raw_packet, &inj_flow_rate))
  return false;
 m_InjctrPar.inj_flow_rate[1] = float(inj_flow_rate) / 64.0f;

 int inj_cyl_disp = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &inj_cyl_disp))
  return false;
 m_InjctrPar.inj_cyl_disp = float(inj_cyl_disp) / 16384.0f;

 unsigned long inj_sd_igl_const = 0;
 if (false == mp_pdp->Hex32ToBin(raw_packet, &inj_sd_igl_const))
  return false;
 m_InjctrPar.inj_sd_igl_const[0] = (float)inj_sd_igl_const;
 //second fuel:
 if (false == mp_pdp->Hex32ToBin(raw_packet, &inj_sd_igl_const))
  return false;
 m_InjctrPar.inj_sd_igl_const[1] = (float)inj_sd_igl_const;

 //read-only parameter: number of engine cylinders
 unsigned char cyl_num = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &cyl_num))
  return false;
 m_InjctrPar.cyl_num = cyl_num;

 //injection timing (phase)
 int  inj_begin_angle;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &inj_begin_angle, true))
  return false;
 m_InjctrPar.inj_timing[0] = ((float)inj_begin_angle) / m_angle_multiplier;
 //second fuel:
 if (false == mp_pdp->Hex16ToBin(raw_packet, &inj_begin_angle, true))
  return false;
 m_InjctrPar.inj_timing[1] = ((float)inj_begin_angle) / m_angle_multiplier;

 //injection timing (phase) on cranking
 int  inj_begin_angle_crk;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &inj_begin_angle_crk, true))
  return false;
 m_InjctrPar.inj_timing_crk[0] = ((float)inj_begin_angle_crk) / m_angle_multiplier;
 //second fuel:
 if (false == mp_pdp->Hex16ToBin(raw_packet, &inj_begin_angle_crk, true))
  return false;
 m_InjctrPar.inj_timing_crk[1] = ((float)inj_begin_angle_crk) / m_angle_multiplier;

 //inj.pulse origin
 unsigned char anglespec = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &anglespec))
  return false;
 m_InjctrPar.inj_anglespec[0] = GETLO4BITS(anglespec);
 m_InjctrPar.inj_anglespec[1] = GETHI4BITS(anglespec);  //second fuel

 //fff_const (fuel consumption)
 int  fff;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &fff))
  return false;
 m_InjctrPar.fff_const = ((float)fff / 65536.0f) * (1000.0f * 60.0f);

 return true;
}

//-----------------------------------------------------------------------
bool CControlApp::Parse_LAMBDA_PAR(const BYTE* raw_packet, size_t size)
{
 SECU3IO::LambdaPar& m_LambdaPar = m_recepted_packet.m_LambdaPar;
 if (size != (mp_pdp->isHex() ? 54 : 27))  //размер пакета без сигнального символа, дескриптора и символа-конца пакета
  return false;

 unsigned char strperstp = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &strperstp))
  return false;
 m_LambdaPar.lam_str_per_stp = strperstp;

 unsigned char stepsize_p = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &stepsize_p))
  return false;
 m_LambdaPar.lam_step_size_p = (float(stepsize_p) / 512.0f) * 100.0f;

 unsigned char stepsize_m = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &stepsize_m))
  return false;
 m_LambdaPar.lam_step_size_m = (float(stepsize_m) / 512.0f) * 100.0f;

 int corrlimit_p = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &corrlimit_p))
  return false;
 m_LambdaPar.lam_corr_limit_p = (float(corrlimit_p) / 512.0f) * 100.0f;

 int corrlimit_m = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &corrlimit_m))
  return false;
 m_LambdaPar.lam_corr_limit_m = (float(corrlimit_m) / 512.0f) * 100.0f;

 int swtpoint = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &swtpoint))
  return false;
 m_LambdaPar.lam_swt_point = float(swtpoint) * ADC_DISCRETE;

 int tempthrd = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &tempthrd))
  return false;
 m_LambdaPar.lam_temp_thrd = float(tempthrd) / TEMP_PHYSICAL_MAGNITUDE_MULTIPLIER;

 int rpmthrd = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &rpmthrd))
  return false;
 m_LambdaPar.lam_rpm_thrd = rpmthrd;

 int activdelay = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &activdelay))
  return false;
 m_LambdaPar.lam_activ_delay = activdelay;

 int deadband = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &deadband))
  return false;
 m_LambdaPar.lam_dead_band = float(deadband) * ADC_DISCRETE;

 unsigned char senstype = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &senstype))
  return false;
 m_LambdaPar.lam_senstype = senstype;

 unsigned char msperstp = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &msperstp))
  return false;
 m_LambdaPar.lam_ms_per_stp = msperstp * 10;

 unsigned char htgdet = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &htgdet))
  return false;
 m_LambdaPar.lam_htgdet = htgdet;

 //Stoichiometric value for second fuel
 int lam_2stoichval = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &lam_2stoichval))
  return false;
 m_LambdaPar.lam_2stoichval = ((float)lam_2stoichval) / 128.0f;

 //heating:
 unsigned char eh_heating_time[2] = {0, 0};
 if (false == mp_pdp->Hex8ToBin(raw_packet, &eh_heating_time[0]))
  return false;
 m_LambdaPar.eh_heating_time[0] = eh_heating_time[0];

 if (false == mp_pdp->Hex8ToBin(raw_packet, &eh_heating_time[1]))
  return false;
 m_LambdaPar.eh_heating_time[1] = eh_heating_time[1];

 unsigned char eh_temper_thrd = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &eh_temper_thrd))
  return false;
 m_LambdaPar.eh_temper_thrd = (float)eh_temper_thrd;

 unsigned char eh_heating_act = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &eh_heating_act))
  return false;
 m_LambdaPar.eh_heating_act = ((float)eh_heating_act) / 100.0f;

 int eh_aflow_thrd = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &eh_aflow_thrd))
  return false;
 m_LambdaPar.eh_aflow_thrd = float(eh_aflow_thrd) * 32.0f;

 return true;
}

//-----------------------------------------------------------------------
bool CControlApp::Parse_ACCEL_PAR(const BYTE* raw_packet, size_t size)
{
 SECU3IO::AccelPar& m_AccelPar = m_recepted_packet.m_AccelPar;
 if (size != (mp_pdp->isHex() ? 6 : 3))  //размер пакета без сигнального символа, дескриптора и символа-конца пакета
  return false;

 unsigned char tpsdot_thrd = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &tpsdot_thrd))
  return false;
 m_AccelPar.ae_tpsdot_thrd = (float)tpsdot_thrd;

 unsigned char coldacc_mult = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &coldacc_mult))
  return false;
 m_AccelPar.ae_coldacc_mult = ((((float)coldacc_mult) + 128.0f) / 128.0f) * 100.0f; //convert to %

 unsigned char ae_decay_time = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &ae_decay_time))
  return false;
 m_AccelPar.ae_decay_time = ae_decay_time; //strokes

 return true;
}

//-----------------------------------------------------------------------
//Return: true - если хотя бы один пакет был получен
bool CControlApp::ParsePackets()
{
 Packets::iterator it;
 bool status = false;

 ASSERT(m_pPackets);
 for(it = m_pPackets->begin(); it!=m_pPackets->end(); ++it)
 {
  if (it->size() < 3 || (*it)[0] != '@')
   continue;
  if (it->back() != '\r')
   continue;

  if (!mp_pdp->isHex())
   Esc_Rx_Packet(*it, 2, it->size() - 3); //byte stuffing

  size_t p_size = it->size() - 3;
  const BYTE* p_start = &(*it)[2];
  switch((*it)[1])
  {
   case TEMPER_PAR:
    if (Parse_TEMPER_PAR(p_start, p_size))
     break; //пакет успешно разобран по составляющим
    continue;//пакет не прошел сурового отбора нашим жюри :-)
   case CARBUR_PAR:
    if (Parse_CARBUR_PAR(p_start, p_size))
     break;
    continue;
   case IDLREG_PAR:
    if (Parse_IDLREG_PAR(p_start, p_size))
     break;
    continue;
   case ANGLES_PAR:
    if (Parse_ANGLES_PAR(p_start, p_size))
     break;
    continue;
   case FUNSET_PAR:
    if (Parse_FUNSET_PAR(p_start, p_size))
     break;
    continue;
   case STARTR_PAR:
    if (Parse_STARTR_PAR(p_start, p_size))
     break;
    continue;
   case FNNAME_DAT:
    if (Parse_FNNAME_DAT(p_start, p_size))
     break;
    continue;
   case SENSOR_DAT:
    if (Parse_SENSOR_DAT(p_start, p_size))
     break;
    continue;
   case DBGVAR_DAT:
    if (Parse_DBGVAR_DAT(p_start, p_size))
     break;
    continue;
   case ADCRAW_DAT:
    if (Parse_ADCRAW_DAT(p_start, p_size))
     break;
    continue;
   case ADCCOR_PAR:
    if (Parse_ADCCOR_PAR(p_start, p_size))
     break;
    continue;
   case CKPS_PAR:
    if (Parse_CKPS_PAR(p_start, p_size))
     break;
    continue;
   case OP_COMP_NC:
    if (Parse_OP_COMP_NC(p_start, p_size))
     break;
    continue;
   case KNOCK_PAR:
    if (Parse_KNOCK_PAR(p_start, p_size))
     break;
    continue;
   case CE_ERR_CODES:
    if (Parse_CE_ERR_CODES(p_start, p_size))
     break;
    continue;
   case CE_SAVED_ERR:
    if (Parse_CE_SAVED_ERR(p_start, p_size))
     break;
    continue;
   case FWINFO_DAT:
    if (Parse_FWINFO_DAT(p_start, p_size))
     break;
    continue;
   case MISCEL_PAR:
    if (Parse_MISCEL_PAR(p_start, p_size))
     break;
    continue;
   case EDITAB_PAR:
    if (Parse_EDITAB_PAR(p_start, p_size))
     break;
    continue;
   case ATTTAB_PAR:
    if (Parse_ATTTAB_PAR(p_start, p_size))
     break;
    continue;
   case RPMGRD_PAR:
    if (Parse_RPMGRD_PAR(p_start, p_size))
     break;
    continue;
   case DIAGINP_DAT:
    if (Parse_DIAGINP_DAT(p_start, p_size))
     break;
    continue;
   case CHOKE_PAR:
    if (Parse_CHOKE_PAR(p_start, p_size))
     break;
    continue;
   case GASDOSE_PAR:
    if (Parse_GASDOSE_PAR(p_start, p_size)) //GD
     break;
    continue;
   case SECUR_PAR:
    if (Parse_SECUR_PAR(p_start, p_size))
     break;
    continue;
   case UNIOUT_PAR:
    if (Parse_UNIOUT_PAR(p_start, p_size))
     break;
    continue;
   case INJCTR_PAR:
    if (Parse_INJCTR_PAR(p_start, p_size))
     break;
    continue;
   case LAMBDA_PAR:
    if (Parse_LAMBDA_PAR(p_start, p_size))
     break;
    continue;
   case ACCEL_PAR:
    if (Parse_ACCEL_PAR(p_start, p_size))
     break;
    continue;

   default:
    continue;
  }//switch

  ////////////////////////////////////////////////////////////////////////////
   EnterCriticalSection();
   memcpy(&PendingPacket(),&m_recepted_packet,sizeof(SECU3Packet));
   LeaveCriticalSection();
  ////////////////////////////////////////////////////////////////////////////
  //так как все возможные структуры данных пакетов собраны в union, то нам достаточно оперировать
  //только адресом union.
  m_pEventHandler->OnPacketReceived((*it)[1], &EndPendingPacket());
  status = true;
 }//for

 return status;
}


//-----------------------------------------------------------------------
DWORD WINAPI CControlApp::BackgroundProcess(LPVOID lpParameter)
{
 CControlApp* p_capp = (CControlApp*)lpParameter;
 CComPort* p_port = p_capp->m_p_port;
 EventHandler* pEventHandler;
 BYTE read_buf[RAW_BYTES_TO_READ_MAX+1];

 DWORD actual_received = 0;

 while(1)
 {
  SetEvent(p_capp->m_hSleepEvent);
  WaitForSingleObject(p_capp->m_hAwakeEvent,INFINITE); //sleep if need

  if (p_capp->m_is_thread_must_exit)
   break;  //поступила команда завершения работы потока

  //если порт не открыт, то для того чтобы не грузить процессор, засыпаем
  //на 100мс в каждом цикле
  if (p_port->GetHandle()==INVALID_HANDLE_VALUE)
  {
   Sleep(100);
   if (p_capp->m_portAutoReopen)
   {
    bool initialized = true;
    try
    {
     p_port->Initialize(p_capp->m_uart_speed, NOPARITY, ONESTOPBIT, false, true);
    }
    catch(CComPort::xInitialize e)
    {
     initialized = false;
    }
    if (initialized && p_port->IsInitialized())
     p_capp->SetPacketsTimer(p_capp->m_dat_packet_timeout);  //reset timeout timer, so port will not be incidentally terminated
   }
   continue;
  }

  pEventHandler = p_capp->m_pEventHandler;
  ASSERT(pEventHandler); //перед тем как обрабатывать пакеты, необходимо приаттачить обработчик событий

  //читаем блок данных
  actual_received = 0;
  p_port->RecvBlock(read_buf,RAW_BYTES_TO_READ_MAX, &actual_received);

  //парсим данные и пытаемся выделить пакеты
  p_capp->SplitPackets(read_buf, actual_received);

  //проводим разбор каждого пакета
  if (true==p_capp->ParsePackets()) //хотя бы один пакет обработан успешно ?
  {
   p_capp->SetPacketsTimer(p_capp->m_dat_packet_timeout);  //reset timeout timer
   if ((p_capp->m_online_state==false)||(p_capp->m_force_notify_about_connection))  //мы были в оффлайне, надо известить пользователя о переходе в онлайн...
   {
    p_capp->m_online_state = true;
    pEventHandler->OnConnection(p_capp->m_online_state);
    p_capp->m_force_notify_about_connection = false; //updated
   }
  }

  if (WaitForSingleObject(p_capp->m_hTimer,0)==WAIT_OBJECT_0) //сработал ли таймер (вышло отведенное время а ни одного пакета так и не было принято) ?
  {
   if (p_capp->m_portAutoReopen && p_port->IsInitialized())
    p_port->Terminate(); //termitate port only if auto reopen enabled and it has/had a valid handle

   if ((p_capp->m_online_state==true)||(p_capp->m_force_notify_about_connection)) //мы были в онлайне... надо известить пользователя о переходе в оффлайн...
   {
    p_capp->m_online_state = false;
    pEventHandler->OnConnection(p_capp->m_online_state);
    p_capp->m_force_notify_about_connection = false;
   }
   p_capp->SetPacketsTimer(p_capp->m_dat_packet_timeout);  //reset timeout timer
  }

 }//while

 return 0;
}


//-----------------------------------------------------------------------
void CControlApp::SwitchOnThread(bool state)
{
 if (state)
  SetEvent(m_hAwakeEvent);    //активизация работы потока немедленно
 else
 {
  ResetEvent(m_hAwakeEvent);  //поток заснет и не будет отнимать процессорное время

  //Необходимо дождатся подтверждения того что работа потока остановлена
  ResetEvent(m_hSleepEvent);
  WaitForSingleObject(m_hSleepEvent,2500);
 }
}


//-----------------------------------------------------------------------
BOOL CControlApp::SetPacketsTimer(int timeout)
{
 static LARGE_INTEGER liDueTime;
 liDueTime.QuadPart = timeout;      //время в милисекундах
 liDueTime.QuadPart*=-10000;        //в одной мс 10000 интервалов по 100нс

 return SetWaitableTimer(m_hTimer,     // handle to timer
                         &liDueTime,   // timer due time
                         1,            // period (лишь бы не 0)
                         NULL,         // completion routine
                         NULL,         // completion routine parameter
                         0);           // resume state
}

//-----------------------------------------------------------------------
void CControlApp::SwitchOn(bool state, bool i_force_reinit /* = false*/)
{
 COMMTIMEOUTS timeouts;
 float ms_need_for_one_byte;

 if (m_work_state==state && false==i_force_reinit)
  return;

 //кол-во мс необходимое для приема/передачи одного байта
 ms_need_for_one_byte = CComPort::ms_need_for_one_byte_8N1(m_uart_speed);

 if (state)
 { //возобновить работу
  //перед возобновлением работы необходимо установить параметры (сброс операции при ошибке и таймауты)
  m_p_port->Purge();

  m_p_port->AccessDCB().fAbortOnError = FALSE;     //прекращение операции при ошибке
  m_p_port->AccessDCB().BaudRate = m_uart_speed;   //для работы с приложением своя скорость
  m_p_port->SetState();

  //теперь необходимо настроить таймауты (я нихрена так и не понял ничего в этих таймаутах)
  timeouts.ReadIntervalTimeout = 0;
  timeouts.ReadTotalTimeoutMultiplier = MathHelpers::Round(ms_need_for_one_byte * 2);
  timeouts.ReadTotalTimeoutConstant = 1;

  timeouts.WriteTotalTimeoutConstant = 500;
  timeouts.WriteTotalTimeoutMultiplier = MathHelpers::Round(ms_need_for_one_byte * 5);
  m_p_port->SetTimeouts(&timeouts);

  Sleep(MathHelpers::Round(ms_need_for_one_byte * 5));
  m_force_notify_about_connection = true;
  SwitchOnThread(true);
  SetPacketsTimer(m_dat_packet_timeout);
 }
 else
 { //остановить работу
  SwitchOnThread(false);
  Sleep(MathHelpers::Round(ms_need_for_one_byte * 5));

  //без этой проверки под Windows 98 возникает "Abnormal program termination"
  //Странное поведение CancelWaitableTimer() с нулевым хэндлом???
  if (NULL!=m_hTimer)
   CancelWaitableTimer(m_hTimer);

  if (m_pEventHandler)
   m_pEventHandler->OnConnection(false);
 }

 m_work_state = state; //сохраняем состояние для последующего использования
}

//-----------------------------------------------------------------------
//А вдруг на верхнем уровне иерархии кто-нибудь решит использовать некорректные дескрипторы
bool CControlApp::IsValidDescriptor(const BYTE descriptor) const
{
 switch(descriptor) //сравнение со всеми допустимыми дескрипторами
 {
  case CHANGEMODE:
  case BOOTLOADER:
  case TEMPER_PAR:
  case CARBUR_PAR:
  case IDLREG_PAR:
  case ANGLES_PAR:
  case FUNSET_PAR:
  case STARTR_PAR:
  case FNNAME_DAT:
  case SENSOR_DAT:
  case DBGVAR_DAT:
  case ADCRAW_DAT:
  case ADCCOR_PAR:
  case CKPS_PAR:
  case OP_COMP_NC:
  case KNOCK_PAR:
  case CE_ERR_CODES:
  case CE_SAVED_ERR:
  case FWINFO_DAT:
  case MISCEL_PAR:
  case EDITAB_PAR:
  case ATTTAB_PAR:
  case RPMGRD_PAR:
  case DIAGINP_DAT:
  case DIAGOUT_DAT:
  case CHOKE_PAR:
  case GASDOSE_PAR: //GD
  case SECUR_PAR:
  case UNIOUT_PAR:
  case INJCTR_PAR:
  case LAMBDA_PAR:
  case ACCEL_PAR:
   return true;
  default:
   return false;
 }//switch
}

//-----------------------------------------------------------------------
void CControlApp::SetProtocolDataMode(bool i_mode)
{
 mp_pdp->SetMode(i_mode);
}

//-----------------------------------------------------------------------
void CControlApp::SetQuartzFrq(long frq)
{
 m_quartz_frq = frq;
}

//-----------------------------------------------------------------------
void CControlApp::SetPortAutoReopen(bool reopen)
{
 m_portAutoReopen = reopen;
}

//-----------------------------------------------------------------------
bool CControlApp::SendPacket(const BYTE i_descriptor, const void* i_packet_data)
{
 if (false==IsValidDescriptor(i_descriptor))
  return false;

 m_outgoing_packet.clear();
 m_outgoing_packet.push_back('!');
 m_outgoing_packet.push_back(i_descriptor);

 //формирование пакетов и их передача, WriteFile будет заблокирована пока не завершится ReadFile...
 switch(i_descriptor)
 {
  case BOOTLOADER:
   return StartBootLoader();         //no data need
  case CHANGEMODE:
   return ChangeContext(i_descriptor);  //no data need, only a descriptor
  case TEMPER_PAR:
   Build_TEMPER_PAR((TemperPar*)i_packet_data);
   break;
  case CARBUR_PAR:
   Build_CARBUR_PAR((CarburPar*)i_packet_data);
   break;
  case IDLREG_PAR:
   Build_IDLREG_PAR((IdlRegPar*)i_packet_data);
   break;
  case ANGLES_PAR:
   Build_ANGLES_PAR((AnglesPar*)i_packet_data);
   break;
  case FUNSET_PAR:
   Build_FUNSET_PAR((FunSetPar*)i_packet_data);
   break;
  case STARTR_PAR:
   Build_STARTR_PAR((StartrPar*)i_packet_data);
   break;
  case ADCCOR_PAR:
   Build_ADCCOR_PAR((ADCCompenPar*)i_packet_data);
   break;
  case CKPS_PAR:
   Build_CKPS_PAR((CKPSPar*)i_packet_data);
   break;
  case OP_COMP_NC:
   Build_OP_COMP_NC((OPCompNc*)i_packet_data);
   break;
  case KNOCK_PAR:
   Build_KNOCK_PAR((KnockPar*)i_packet_data);
   break;
  case CE_SAVED_ERR:
   Build_CE_SAVED_ERR((CEErrors*)i_packet_data);
   break;
  case MISCEL_PAR:
   Build_MISCEL_PAR((MiscelPar*)i_packet_data);
   break;
  case EDITAB_PAR:
   Build_EDITAB_PAR((EditTabPar*)i_packet_data);
   break;
  case DIAGOUT_DAT:
   Build_DIAGOUT_DAT((DiagOutDat*)i_packet_data);
   break;
  case CHOKE_PAR:
   Build_CHOKE_PAR((ChokePar*)i_packet_data);
   break;
  case GASDOSE_PAR:
   Build_GASDOSE_PAR((GasdosePar*)i_packet_data);
   break;
  case SECUR_PAR:
   Build_SECUR_PAR((SecurPar*)i_packet_data);
   break;
  case UNIOUT_PAR:
   Build_UNIOUT_PAR((UniOutPar*)i_packet_data);
   break;
  case INJCTR_PAR:
   Build_INJCTR_PAR((InjctrPar*)i_packet_data);
   break;
  case LAMBDA_PAR:
   Build_LAMBDA_PAR((LambdaPar*)i_packet_data);
   break;
  case ACCEL_PAR:
   Build_ACCEL_PAR((AccelPar*)i_packet_data);
   break;

  default:
   return false; //invalid descriptor
 }//switch
 m_outgoing_packet.push_back('\r');

 if (!mp_pdp->isHex())
  Esc_Tx_Packet(m_outgoing_packet, 2, m_outgoing_packet.size() - 3); //byte stuffing

 return m_p_port->SendBlock(&m_outgoing_packet[0], m_outgoing_packet.size());
}

//-----------------------------------------------------------------------
//посылает команду изменения контекста на новый контекст специфицированный i_new_descriptor-ом
bool CControlApp::ChangeContext(const BYTE i_new_descriptor)
{
 m_outgoing_packet.clear();

 if (false==IsValidDescriptor(i_new_descriptor))  //передали правильный дескриптор ?
  return false;

 m_outgoing_packet.push_back('!');
 m_outgoing_packet.push_back(CHANGEMODE);
 m_outgoing_packet.push_back(i_new_descriptor);
 m_outgoing_packet.push_back('\r');
 return m_p_port->SendBlock(&m_outgoing_packet[0], m_outgoing_packet.size()); //посылаем команду изменения дескриптора
}
//-----------------------------------------------------------------------
//Посылает команду запуска бутлоадера
bool CControlApp::StartBootLoader()
{
 m_outgoing_packet.clear();

 m_outgoing_packet.push_back('!');
 m_outgoing_packet.push_back(BOOTLOADER);
 m_outgoing_packet.push_back('l');
 m_outgoing_packet.push_back('\r');
 return m_p_port->SendBlock(&m_outgoing_packet[0], m_outgoing_packet.size()); //посылаем команду запуска бутлоадера
}

//-----------------------------------------------------------------------
void CControlApp::Build_CARBUR_PAR(CarburPar* packet_data)
{
 mp_pdp->Bin16ToHex(packet_data->ephh_lot,m_outgoing_packet);
 mp_pdp->Bin16ToHex(packet_data->ephh_hit,m_outgoing_packet);
 mp_pdp->Bin4ToHex(packet_data->carb_invers,m_outgoing_packet);
 int epm_on_threshold = MathHelpers::Round(packet_data->epm_ont * MAP_PHYSICAL_MAGNITUDE_MULTIPLIER);
 mp_pdp->Bin16ToHex(epm_on_threshold,m_outgoing_packet);
 mp_pdp->Bin16ToHex(packet_data->ephh_lot_g,m_outgoing_packet);
 mp_pdp->Bin16ToHex(packet_data->ephh_hit_g,m_outgoing_packet);
 unsigned char shutoff_delay = MathHelpers::Round(packet_data->shutoff_delay * 100.0f);
 mp_pdp->Bin8ToHex(shutoff_delay,m_outgoing_packet);
 unsigned char tps_threshold = MathHelpers::Round(packet_data->tps_threshold * TPS_PHYSICAL_MAGNITUDE_MULTIPLIER);
 mp_pdp->Bin8ToHex(tps_threshold, m_outgoing_packet);
 int fuelcut_map_thrd = MathHelpers::Round((packet_data->fuelcut_map_thrd * MAP_PHYSICAL_MAGNITUDE_MULTIPLIER));
 mp_pdp->Bin16ToHex(fuelcut_map_thrd, m_outgoing_packet);
 int fuelcut_cts_thrd = MathHelpers::Round((packet_data->fuelcut_cts_thrd * TEMP_PHYSICAL_MAGNITUDE_MULTIPLIER));
 mp_pdp->Bin16ToHex(fuelcut_cts_thrd, m_outgoing_packet);
 mp_pdp->Bin16ToHex(packet_data->revlim_lot, m_outgoing_packet);
 mp_pdp->Bin16ToHex(packet_data->revlim_hit, m_outgoing_packet);
}

//-----------------------------------------------------------------------
void CControlApp::Build_IDLREG_PAR(IdlRegPar* packet_data)
{
 unsigned char flags = 0;
 WRITEBIT8(flags, 0, packet_data->idl_regul);
 WRITEBIT8(flags, 1, packet_data->use_regongas);
 WRITEBIT8(flags, 2, packet_data->closed_loop);
 WRITEBIT8(flags, 3, packet_data->preg_mode);
 WRITEBIT8(flags, 4, packet_data->idl_useiacclongas);
 
 mp_pdp->Bin8ToHex(flags, m_outgoing_packet);

 int ifac1 =  MathHelpers::Round((packet_data->ifac1 * 256.0f));
 mp_pdp->Bin16ToHex(ifac1,m_outgoing_packet);

 int ifac2 = MathHelpers::Round((packet_data->ifac2 * 256.0f));
 mp_pdp->Bin16ToHex(ifac2,m_outgoing_packet);

 mp_pdp->Bin16ToHex(packet_data->MINEFR,m_outgoing_packet);
 mp_pdp->Bin16ToHex(packet_data->idling_rpm,m_outgoing_packet);

 int min_angle = MathHelpers::Round((packet_data->min_angle * m_angle_multiplier));
 mp_pdp->Bin16ToHex(min_angle,m_outgoing_packet);
 int max_angle = MathHelpers::Round((packet_data->max_angle * m_angle_multiplier));
 mp_pdp->Bin16ToHex(max_angle,m_outgoing_packet);

 int turn_on_temp = MathHelpers::Round((packet_data->turn_on_temp * TEMP_PHYSICAL_MAGNITUDE_MULTIPLIER));
 mp_pdp->Bin16ToHex(turn_on_temp, m_outgoing_packet);

 //CLOSED LOOP PARAMETERS: 
 unsigned char idl_to_run_add = MathHelpers::Round(packet_data->idl_to_run_add * 2.0f);
 mp_pdp->Bin8ToHex(idl_to_run_add, m_outgoing_packet);

 unsigned char rpm_on_run_add = MathHelpers::Round(packet_data->rpm_on_run_add / 10.0f);
 mp_pdp->Bin8ToHex(rpm_on_run_add, m_outgoing_packet);

 int idl_reg_p = MathHelpers::Round(packet_data->idl_reg_p * 256.0f);
 mp_pdp->Bin16ToHex(idl_reg_p, m_outgoing_packet);

 int idl_reg_i = MathHelpers::Round(packet_data->idl_reg_i * 256.0f);
 mp_pdp->Bin16ToHex(idl_reg_i, m_outgoing_packet);

 unsigned char idl_coef_thrd1 = MathHelpers::Round((packet_data->idl_coef_thrd1 - 1.0f) * 128.0f);
 mp_pdp->Bin8ToHex(idl_coef_thrd1, m_outgoing_packet);

 unsigned char idl_coef_thrd2 = MathHelpers::Round((packet_data->idl_coef_thrd2 - 1.0f)* 128.0f);
 mp_pdp->Bin8ToHex(idl_coef_thrd2, m_outgoing_packet);

 unsigned char idl_intrpm_lim = MathHelpers::Round(packet_data->idl_intrpm_lim / 10.0f);
 mp_pdp->Bin8ToHex(idl_intrpm_lim, m_outgoing_packet);

 int idl_map_value = MathHelpers::Round(packet_data->idl_map_value * MAP_PHYSICAL_MAGNITUDE_MULTIPLIER);
 mp_pdp->Bin16ToHex(idl_map_value, m_outgoing_packet);

 unsigned char idl_iacminpos = MathHelpers::Round(packet_data->idl_iacminpos * 2.0f);
 mp_pdp->Bin8ToHex(idl_iacminpos, m_outgoing_packet);

 unsigned char idl_iacmaxpos = MathHelpers::Round(packet_data->idl_iacmaxpos * 2.0f);
 mp_pdp->Bin8ToHex(idl_iacmaxpos, m_outgoing_packet);
}

//-----------------------------------------------------------------------
void CControlApp::Build_STARTR_PAR(StartrPar* packet_data)
{
 mp_pdp->Bin16ToHex(packet_data->starter_off,m_outgoing_packet);
 mp_pdp->Bin16ToHex(packet_data->smap_abandon,m_outgoing_packet);
 int cranktorun_time = MathHelpers::Round(packet_data->inj_cranktorun_time * 100.0f);
 mp_pdp->Bin16ToHex(cranktorun_time, m_outgoing_packet);
 int inj_aftstr_strokes = MathHelpers::Round(packet_data->inj_aftstr_strokes / 2.0f);
 mp_pdp->Bin8ToHex(inj_aftstr_strokes, m_outgoing_packet);
 float discrete = (m_quartz_frq == 20000000 ? 3.2f : 4.0f);
 int prime_cold = MathHelpers::Round((packet_data->inj_prime_cold * 1000.0f) / discrete);
 mp_pdp->Bin16ToHex(prime_cold, m_outgoing_packet);
 int prime_hot = MathHelpers::Round((packet_data->inj_prime_hot * 1000.0f) / discrete);
 mp_pdp->Bin16ToHex(prime_hot, m_outgoing_packet);
 int prime_delay = MathHelpers::Round(packet_data->inj_prime_delay * 10.0f);
 mp_pdp->Bin8ToHex(prime_delay, m_outgoing_packet);
 int inj_floodclear_tps = MathHelpers::Round(packet_data->inj_floodclear_tps * 2.0f);
 mp_pdp->Bin8ToHex(inj_floodclear_tps, m_outgoing_packet);
}
//-----------------------------------------------------------------------

void CControlApp::Build_TEMPER_PAR(TemperPar* packet_data)
{
 unsigned char flags = 0; //not used now
 WRITEBIT8(flags, 0, packet_data->tmp_use);
 WRITEBIT8(flags, 1, packet_data->cts_use_map);
 WRITEBIT8(flags, 2, packet_data->vent_pwm);
 mp_pdp->Bin8ToHex(flags, m_outgoing_packet);
 int vent_on = MathHelpers::Round(packet_data->vent_on * TEMP_PHYSICAL_MAGNITUDE_MULTIPLIER);
 mp_pdp->Bin16ToHex(vent_on, m_outgoing_packet);
 int vent_off = MathHelpers::Round(packet_data->vent_off * TEMP_PHYSICAL_MAGNITUDE_MULTIPLIER);
 mp_pdp->Bin16ToHex(vent_off, m_outgoing_packet);
 int vent_pwmfrq = MathHelpers::Round((1.0/packet_data->vent_pwmfrq) * 524288.0);
 mp_pdp->Bin16ToHex(vent_pwmfrq, m_outgoing_packet);
 //air conditioner related:
 int cond_pvt_on = MathHelpers::Round(((float)packet_data->cond_pvt_on) / m_adc_discrete);
 mp_pdp->Bin16ToHex(cond_pvt_on, m_outgoing_packet);
 int cond_pvt_off = MathHelpers::Round(((float)packet_data->cond_pvt_off) / m_adc_discrete);
 mp_pdp->Bin16ToHex(cond_pvt_off, m_outgoing_packet);
 mp_pdp->Bin16ToHex(packet_data->cond_min_rpm, m_outgoing_packet);
 int vent_tmr = MathHelpers::Round(((float)packet_data->vent_tmr) * 100.0f);
 mp_pdp->Bin16ToHex(vent_tmr, m_outgoing_packet);
}

//-----------------------------------------------------------------------
void CControlApp::Build_ANGLES_PAR(AnglesPar* packet_data)
{
 int max_angle = MathHelpers::Round(packet_data->max_angle * m_angle_multiplier);
 mp_pdp->Bin16ToHex(max_angle,m_outgoing_packet);
 int min_angle = MathHelpers::Round(packet_data->min_angle * m_angle_multiplier);
 mp_pdp->Bin16ToHex(min_angle,m_outgoing_packet);
 int angle_corr = MathHelpers::Round(packet_data->angle_corr * m_angle_multiplier);
 mp_pdp->Bin16ToHex(angle_corr,m_outgoing_packet);
 int dec_speed = MathHelpers::Round(packet_data->dec_speed * m_angle_multiplier);
 mp_pdp->Bin16ToHex(dec_speed,m_outgoing_packet);
 int inc_speed = MathHelpers::Round(packet_data->inc_speed * m_angle_multiplier);
 mp_pdp->Bin16ToHex(inc_speed,m_outgoing_packet);
 mp_pdp->Bin4ToHex(packet_data->zero_adv_ang, m_outgoing_packet);
}

//-----------------------------------------------------------------------
void CControlApp::Build_FUNSET_PAR(FunSetPar* packet_data)
{
 mp_pdp->Bin8ToHex(packet_data->fn_benzin,m_outgoing_packet);
 mp_pdp->Bin8ToHex(packet_data->fn_gas,m_outgoing_packet);
 unsigned int map_lower_pressure = MathHelpers::Round(packet_data->map_lower_pressure * MAP_PHYSICAL_MAGNITUDE_MULTIPLIER);
 mp_pdp->Bin16ToHex(map_lower_pressure,m_outgoing_packet);
 int map_upper_pressure = MathHelpers::Round(packet_data->map_upper_pressure * MAP_PHYSICAL_MAGNITUDE_MULTIPLIER);
 mp_pdp->Bin16ToHex(map_upper_pressure,m_outgoing_packet);
 int map_curve_offset = MathHelpers::Round(packet_data->map_curve_offset / m_adc_discrete);
 mp_pdp->Bin16ToHex(map_curve_offset, m_outgoing_packet);
 int map_curve_gradient = MathHelpers::Round(128.0f * packet_data->map_curve_gradient * MAP_PHYSICAL_MAGNITUDE_MULTIPLIER * m_adc_discrete);
 mp_pdp->Bin16ToHex(map_curve_gradient, m_outgoing_packet);
 int map2_curve_offset = MathHelpers::Round(packet_data->map2_curve_offset / m_adc_discrete);
 mp_pdp->Bin16ToHex(map2_curve_offset, m_outgoing_packet);
 int map2_curve_gradient = MathHelpers::Round(128.0f * packet_data->map2_curve_gradient * MAP_PHYSICAL_MAGNITUDE_MULTIPLIER * m_adc_discrete);
 mp_pdp->Bin16ToHex(map2_curve_gradient, m_outgoing_packet);
 int tps_curve_offset = MathHelpers::Round(packet_data->tps_curve_offset / m_adc_discrete);
 mp_pdp->Bin16ToHex(tps_curve_offset, m_outgoing_packet);
 int tps_curve_gradient = MathHelpers::Round(128.0f * packet_data->tps_curve_gradient * (TPS_PHYSICAL_MAGNITUDE_MULTIPLIER*64) * m_adc_discrete);
 mp_pdp->Bin16ToHex(tps_curve_gradient, m_outgoing_packet);
 mp_pdp->Bin4ToHex(packet_data->load_src_cfg, m_outgoing_packet);
 int uni_gas = (packet_data->uni_gas==UNI_OUTPUT_NUM) ? 0xF : packet_data->uni_gas;
 int uni_benzin = (packet_data->uni_benzin==UNI_OUTPUT_NUM) ? 0xF : packet_data->uni_benzin;
 BYTE mapsel_uni = MAKEBYTE(uni_gas, uni_benzin);
 mp_pdp->Bin8ToHex(mapsel_uni, m_outgoing_packet);
 BYTE barocorr_type = packet_data->barocorr_type;
 mp_pdp->Bin8ToHex(barocorr_type, m_outgoing_packet);
}

//-----------------------------------------------------------------------
void CControlApp::Build_ADCCOR_PAR(ADCCompenPar* packet_data)
{
 signed int map_adc_factor = MathHelpers::Round(packet_data->map_adc_factor * 16384);
 mp_pdp->Bin16ToHex(map_adc_factor,m_outgoing_packet);
 signed long map_correction_d = MathHelpers::Round((-packet_data->map_adc_correction) / m_adc_discrete); //переводим из вольтов в дискреты АЦП
 signed long map_adc_correction = MathHelpers::Round(16384 * (0.5f - map_correction_d * packet_data->map_adc_factor));
 mp_pdp->Bin32ToHex(map_adc_correction,m_outgoing_packet);

 signed int ubat_adc_factor = MathHelpers::Round(packet_data->ubat_adc_factor * 16384);
 mp_pdp->Bin16ToHex(ubat_adc_factor,m_outgoing_packet);
 signed long ubat_correction_d = MathHelpers::Round((-packet_data->ubat_adc_correction) / m_adc_discrete); //переводим из вольтов в дискреты АЦП
 signed long ubat_adc_correction = MathHelpers::Round(16384 * (0.5f - ubat_correction_d * packet_data->ubat_adc_factor));
 mp_pdp->Bin32ToHex(ubat_adc_correction,m_outgoing_packet);

 signed int temp_adc_factor = MathHelpers::Round(packet_data->temp_adc_factor * 16384);
 mp_pdp->Bin16ToHex(temp_adc_factor,m_outgoing_packet);
 signed long temp_correction_d = MathHelpers::Round((-packet_data->temp_adc_correction) / m_adc_discrete); //переводим из вольтов в дискреты АЦП
 signed long temp_adc_correction = MathHelpers::Round(16384 * (0.5f - temp_correction_d * packet_data->temp_adc_factor));
 mp_pdp->Bin32ToHex(temp_adc_correction,m_outgoing_packet);
 //TPS sensor
 signed int tps_adc_factor = MathHelpers::Round(packet_data->tps_adc_factor * 16384);
 mp_pdp->Bin16ToHex(tps_adc_factor,m_outgoing_packet);
 signed long tps_correction_d = MathHelpers::Round((-packet_data->tps_adc_correction) / m_adc_discrete); //переводим из вольтов в дискреты АЦП
 signed long tps_adc_correction = MathHelpers::Round(16384 * (0.5f - tps_correction_d * packet_data->tps_adc_factor));
 mp_pdp->Bin32ToHex(tps_adc_correction,m_outgoing_packet);
 //ADD_I1 input
 signed int ai1_adc_factor = MathHelpers::Round(packet_data->ai1_adc_factor * 16384);
 mp_pdp->Bin16ToHex(ai1_adc_factor,m_outgoing_packet);
 signed long ai1_correction_d = MathHelpers::Round((-packet_data->ai1_adc_correction) / m_adc_discrete); //переводим из вольтов в дискреты АЦП
 signed long ai1_adc_correction = MathHelpers::Round(16384 * (0.5f - ai1_correction_d * packet_data->ai1_adc_factor));
 mp_pdp->Bin32ToHex(ai1_adc_correction,m_outgoing_packet);
 //ADD_I2 input
 signed int ai2_adc_factor = MathHelpers::Round(packet_data->ai2_adc_factor * 16384);
 mp_pdp->Bin16ToHex(ai2_adc_factor,m_outgoing_packet);
 signed long ai2_correction_d = MathHelpers::Round((-packet_data->ai2_adc_correction) / m_adc_discrete); //переводим из вольтов в дискреты АЦП
 signed long ai2_adc_correction = MathHelpers::Round(16384 * (0.5f - ai2_correction_d * packet_data->ai2_adc_factor));
 mp_pdp->Bin32ToHex(ai2_adc_correction,m_outgoing_packet);
 //ADD_I3 input
 signed int ai3_adc_factor = MathHelpers::Round(packet_data->ai3_adc_factor * 16384);
 mp_pdp->Bin16ToHex(ai3_adc_factor,m_outgoing_packet);
 signed long ai3_correction_d = MathHelpers::Round((-packet_data->ai3_adc_correction) / m_adc_discrete); //переводим из вольтов в дискреты АЦП
 signed long ai3_adc_correction = MathHelpers::Round(16384 * (0.5f - ai3_correction_d * packet_data->ai3_adc_factor));
 mp_pdp->Bin32ToHex(ai3_adc_correction,m_outgoing_packet);
 //ADD_I4 input
 signed int ai4_adc_factor = MathHelpers::Round(packet_data->ai4_adc_factor * 16384);
 mp_pdp->Bin16ToHex(ai4_adc_factor,m_outgoing_packet);
 signed long ai4_correction_d = MathHelpers::Round((-packet_data->ai4_adc_correction) / m_adc_discrete); //переводим из вольтов в дискреты АЦП
 signed long ai4_adc_correction = MathHelpers::Round(16384 * (0.5f - ai4_correction_d * packet_data->ai4_adc_factor));
 mp_pdp->Bin32ToHex(ai4_adc_correction,m_outgoing_packet);
}

//-----------------------------------------------------------------------
void CControlApp::Build_CKPS_PAR(CKPSPar* packet_data)
{
 mp_pdp->Bin8ToHex(packet_data->ckps_cogs_btdc, m_outgoing_packet);
 mp_pdp->Bin8ToHex(packet_data->ckps_ignit_cogs, m_outgoing_packet);
 mp_pdp->Bin8ToHex(packet_data->ckps_engine_cyl, m_outgoing_packet);
 mp_pdp->Bin8ToHex(packet_data->ckps_cogs_num, m_outgoing_packet);
 mp_pdp->Bin8ToHex(packet_data->ckps_miss_num, m_outgoing_packet);
 unsigned char flags = 0; //not used now
 WRITEBIT8(flags, 0, packet_data->ckps_rising_spark);
 WRITEBIT8(flags, 1, packet_data->ckps_use_cam_ref);
 WRITEBIT8(flags, 2, packet_data->ckps_edge_type);
 WRITEBIT8(flags, 3, packet_data->ref_s_edge_type);
 WRITEBIT8(flags, 4, packet_data->ckps_merge_ign_outs);
 mp_pdp->Bin8ToHex(flags, m_outgoing_packet);
 int wnd_width = MathHelpers::Round(packet_data->hall_wnd_width * m_angle_multiplier);
 mp_pdp->Bin16ToHex(wnd_width, m_outgoing_packet);
 int degrees_btdc = MathHelpers::Round(packet_data->hall_degrees_btdc * m_angle_multiplier);
 mp_pdp->Bin16ToHex(degrees_btdc, m_outgoing_packet);
}

//-----------------------------------------------------------------------
void CControlApp::Build_KNOCK_PAR(KnockPar* packet_data)
{
 mp_pdp->Bin4ToHex(packet_data->knock_use_knock_channel,m_outgoing_packet);
 unsigned char knock_bpf_frequency = (unsigned char)packet_data->knock_bpf_frequency;
 mp_pdp->Bin8ToHex(knock_bpf_frequency,m_outgoing_packet);
 int knock_k_wnd_begin_angle = MathHelpers::Round(packet_data->knock_k_wnd_begin_angle * m_angle_multiplier);
 mp_pdp->Bin16ToHex(knock_k_wnd_begin_angle,m_outgoing_packet);
 int knock_k_wnd_end_angle = MathHelpers::Round(packet_data->knock_k_wnd_end_angle * m_angle_multiplier);
 mp_pdp->Bin16ToHex(knock_k_wnd_end_angle,m_outgoing_packet);
 unsigned char knock_int_time_const = (unsigned char)packet_data->knock_int_time_const;
 mp_pdp->Bin8ToHex(knock_int_time_const, m_outgoing_packet);

 int knock_retard_step = MathHelpers::Round(packet_data->knock_retard_step * m_angle_multiplier);
 mp_pdp->Bin16ToHex(knock_retard_step, m_outgoing_packet);
 int knock_advance_step = MathHelpers::Round(packet_data->knock_advance_step * m_angle_multiplier);
 mp_pdp->Bin16ToHex(knock_advance_step, m_outgoing_packet);
 int knock_max_retard = MathHelpers::Round(packet_data->knock_max_retard * m_angle_multiplier);
 mp_pdp->Bin16ToHex(knock_max_retard, m_outgoing_packet);
 int knock_threshold = MathHelpers::Round(packet_data->knock_threshold / m_adc_discrete);
 mp_pdp->Bin16ToHex(knock_threshold, m_outgoing_packet);
 unsigned char knock_recovery_delay = (unsigned char)packet_data->knock_recovery_delay;
 mp_pdp->Bin8ToHex(knock_recovery_delay, m_outgoing_packet);
}

//-----------------------------------------------------------------------
void CControlApp::Build_OP_COMP_NC(SECU3IO::OPCompNc* packet_data)
{
 mp_pdp->Bin8ToHex(packet_data->opdata, m_outgoing_packet);
 mp_pdp->Bin8ToHex(packet_data->opcode, m_outgoing_packet);
}

//-----------------------------------------------------------------------
void CControlApp::Build_CE_SAVED_ERR(SECU3IO::CEErrors* packet_data)
{
 mp_pdp->Bin16ToHex(packet_data->flags, m_outgoing_packet);
}

//-----------------------------------------------------------------------
void CControlApp::Build_MISCEL_PAR(MiscelPar* packet_data)
{
 int divisor = 0;
 for(size_t i = 0; i < SECU3IO::SECU3_ALLOWABLE_UART_DIVISORS_COUNT; ++i)
  if (SECU3IO::secu3_allowable_uart_divisors[i].first == packet_data->baud_rate)
   divisor = SECU3IO::secu3_allowable_uart_divisors[i].second;

 if (0==divisor)
 {
  secu3_allowable_uart_divisors[0].second;
  ASSERT(0);
 }

 mp_pdp->Bin16ToHex(divisor, m_outgoing_packet);
 unsigned char perid_ms = packet_data->period_ms / 10;
 mp_pdp->Bin8ToHex(perid_ms, m_outgoing_packet);
 mp_pdp->Bin4ToHex(packet_data->ign_cutoff, m_outgoing_packet);
 mp_pdp->Bin16ToHex(packet_data->ign_cutoff_thrd, m_outgoing_packet);
 mp_pdp->Bin8ToHex(packet_data->hop_start_cogs, m_outgoing_packet);
 mp_pdp->Bin8ToHex(packet_data->hop_durat_cogs, m_outgoing_packet);
 BYTE fpf_flags = ((packet_data->inj_offongas != 0) << 1) | ((packet_data->flpmp_offongas != 0) << 0);
 mp_pdp->Bin8ToHex(fpf_flags, m_outgoing_packet);

 int evap_afbegin = MathHelpers::Round(packet_data->evap_afbegin / 32.0f);
 mp_pdp->Bin16ToHex(evap_afbegin, m_outgoing_packet);
 int evap_afslope = MathHelpers::Round(packet_data->evap_afslope * 1048576.0f * 32.0f); //2^20
 mp_pdp->Bin16ToHex(evap_afslope, m_outgoing_packet);

 BYTE fp_timeout_strt = MathHelpers::Round(packet_data->fp_timeout_strt * 10.0f);
 mp_pdp->Bin8ToHex(fp_timeout_strt, m_outgoing_packet);
}

//-----------------------------------------------------------------------
void CControlApp::Build_EDITAB_PAR(EditTabPar* packet_data)
{
 mp_pdp->Bin8ToHex(packet_data->tab_id, m_outgoing_packet);
 mp_pdp->Bin8ToHex(packet_data->address, m_outgoing_packet);

 if (packet_data->tab_id == ETMT_VE_MAP) //Volumetric efficiency map
 {
  BYTE buff[64];  
  int byte_idx_start = packet_data->address & 0x1;

  //padding at the beginning
  w12SetCell(buff, 0, MathHelpers::Round(packet_data->beginPadding * VE_MAPS_M_FACTOR));

  //body
  int cellIdx = byte_idx_start;
  for(unsigned int i = 0; i < packet_data->data_size; ++i)
  {
   int value = MathHelpers::Round(packet_data->table_data[i] * VE_MAPS_M_FACTOR);
   w12SetCell(buff, cellIdx++, value);
  }
  //padding at the end
  w12SetCell(buff, cellIdx, MathHelpers::Round(packet_data->endPadding * VE_MAPS_M_FACTOR));
  
  int endIdx = ((cellIdx*3) / 2) + ((cellIdx*3) % 2);
  //finally, write out bytes into the packet
  for(int idx = byte_idx_start; idx < endIdx; ++idx)
   mp_pdp->Bin8ToHex(buff[idx], m_outgoing_packet);
 }
 else if (packet_data->tab_id == ETMT_IT_MAP) //injection timing map
 {
  BYTE buff[64];  
  int byte_idx_start = packet_data->address & 0x1;

  //padding at the beginning
  w12SetCell(buff, 0, MathHelpers::Round(packet_data->beginPadding * IT_MAPS_M_FACTOR));

  //body
  int cellIdx = byte_idx_start;
  for(unsigned int i = 0; i < packet_data->data_size; ++i)
  {
   int value = MathHelpers::Round(packet_data->table_data[i] * IT_MAPS_M_FACTOR);
   w12SetCell(buff, cellIdx++, value);
  }
  //padding at the end
  w12SetCell(buff, cellIdx, MathHelpers::Round(packet_data->endPadding * IT_MAPS_M_FACTOR));
  
  int endIdx = ((cellIdx*3) / 2) + ((cellIdx*3) % 2);
  //finally, write out bytes into the packet
  for(int idx = byte_idx_start; idx < endIdx; ++idx)
   mp_pdp->Bin8ToHex(buff[idx], m_outgoing_packet);
 }
 else if (packet_data->tab_id != ETMT_NAME_STR)
 {
  float discrete = (m_quartz_frq == 20000000 ? 3.2f : 4.0f);
  for(unsigned int i = 0; i < packet_data->data_size; ++i)
  {
   if (packet_data->tab_id == ETMT_CRNK_MAP || packet_data->tab_id == ETMT_DEAD_MAP)
   {
    int value = MathHelpers::Round((packet_data->table_data[i] * 1000.0f) / discrete);
    mp_pdp->Bin16ToHex(value, m_outgoing_packet);
   }
   else if (packet_data->tab_id == ETMT_AFR_MAP)
   {
    unsigned char value = MathHelpers::Round((packet_data->table_data[i]-8) * AFR_MAPS_M_FACTOR);
    mp_pdp->Bin8ToHex(value, m_outgoing_packet);
   }
   else if (packet_data->tab_id == ETMT_WRMP_MAP)
   {
    unsigned char value = MathHelpers::Round(packet_data->table_data[i] * WRMP_MAPS_M_FACTOR);
    mp_pdp->Bin8ToHex(value, m_outgoing_packet);
   }
   else if (packet_data->tab_id == ETMT_AFTSTR_MAP)
   {
    unsigned char value = MathHelpers::Round(packet_data->table_data[i] * AFTSTR_MAPS_M_FACTOR);
    mp_pdp->Bin8ToHex(value, m_outgoing_packet);
   }
   else if (packet_data->tab_id == ETMT_IDLR_MAP || packet_data->tab_id == ETMT_IDLC_MAP)
   {
    unsigned char value = MathHelpers::Round(packet_data->table_data[i] * IACPOS_MAPS_M_FACTOR);
    mp_pdp->Bin8ToHex(value, m_outgoing_packet);
   }
   else if (packet_data->tab_id == ETMT_AETPS_MAP)
   {
    unsigned char value = MathHelpers::Round((packet_data->address>=INJ_AE_TPS_LOOKUP_TABLE_SIZE)?(packet_data->table_data[i]*AETPSB_MAPS_M_FACTOR):(packet_data->table_data[i]+AETPSV_MAPS_ADDER));
    mp_pdp->Bin8ToHex(value, m_outgoing_packet);
   }
   else if (packet_data->tab_id == ETMT_AERPM_MAP)
   {
    unsigned char value = MathHelpers::Round(packet_data->table_data[i] * ((packet_data->address>=INJ_AE_RPM_LOOKUP_TABLE_SIZE)?AERPMB_MAPS_M_FACTOR:AERPMV_MAPS_M_FACTOR));
    mp_pdp->Bin8ToHex(value, m_outgoing_packet);
   }
   else if (packet_data->tab_id == ETMT_ITRPM_MAP)
   {
    unsigned char value = MathHelpers::Round(packet_data->table_data[i] / 10.0f);
    mp_pdp->Bin8ToHex(value, m_outgoing_packet);
   }
   else if (packet_data->tab_id == ETMT_RIGID_MAP)
   {
    int value = MathHelpers::Round(packet_data->table_data[i] * 128.0f);
    mp_pdp->Bin16ToHex(value, m_outgoing_packet);
   }
   else if (packet_data->tab_id == ETMT_EGOCRV_MAP)
   {
    int value = MathHelpers::Round((packet_data->address > 15) ? (packet_data->table_data[i] / ADC_DISCRETE) : (packet_data->table_data[i] * EGO_CURVE_M_FACTOR));
    mp_pdp->Bin16ToHex(value, m_outgoing_packet);
   }
   else if (packet_data->tab_id == ETMT_IACC_MAP)
   {
    int value = MathHelpers::Round((packet_data->address >= INJ_IAC_CORR_SIZE) ? (packet_data->table_data[i] * 128.0f) : ((packet_data->table_data[i] - 1.0f) * 8192.0f));
    mp_pdp->Bin16ToHex(value, m_outgoing_packet);
   }
   else if (packet_data->tab_id == ETMT_IACCW_MAP)
   {
    int value;
    if (packet_data->address >= INJ_IAC_CORR_W_SIZE)
     value = MathHelpers::Round(packet_data->table_data[i] * 2.0f);
    else
    {
     value = MathHelpers::Round(packet_data->table_data[i] * 256.0f);
     if (value > 255) value = 255;
    }
    mp_pdp->Bin8ToHex(value, m_outgoing_packet);
   }
   else if (packet_data->tab_id == ETMT_IATCLT_MAP)
   {
    int value = MathHelpers::Round((packet_data->address >= INJ_IATCLT_CORR_SIZE) ? (packet_data->table_data[i] / 32.0f) : (packet_data->table_data[i] * 8192.0f));
    mp_pdp->Bin16ToHex(value, m_outgoing_packet);
   }
   else if (packet_data->tab_id == ETMT_TPSSWT_MAP)
   {
    unsigned char value = MathHelpers::Round(packet_data->table_data[i] * TPSSWT_MAPS_M_FACTOR);
    mp_pdp->Bin8ToHex(value, m_outgoing_packet);
   }
   else if (packet_data->tab_id == ETMT_GTSC_MAP)
   {
    unsigned char value = MathHelpers::Round(packet_data->table_data[i] * 128.0f);
    mp_pdp->Bin8ToHex(value, m_outgoing_packet);
   }
   else if (packet_data->tab_id == ETMT_GPSC_MAP)
   {
    int value = MathHelpers::Round((packet_data->address >= INJ_GPS_CORR_SIZE) ? (packet_data->table_data[i] / 2.0f) : (packet_data->table_data[i] * 128.0f));
    mp_pdp->Bin8ToHex(value, m_outgoing_packet);
   }
   else if (packet_data->tab_id == ETMT_ATSC_MAP)
   {
    unsigned char value = MathHelpers::Round(packet_data->table_data[i] * 128.0f);
    mp_pdp->Bin8ToHex(value, m_outgoing_packet);
   }
   else
   {  //default case
    signed char value = MathHelpers::Round(packet_data->table_data[i] * AA_MAPS_M_FACTOR);
    mp_pdp->Bin8ToHex(value, m_outgoing_packet);
   }
  }
 }
 else //string
 {
  char raw_string[64];
  CharToOem(packet_data->name_data, raw_string);
  for(size_t i = 0; i < packet_data->data_size; ++i)
   m_outgoing_packet.push_back(raw_string[i]);
 }
}

//-----------------------------------------------------------------------
void CControlApp::Build_DIAGOUT_DAT(DiagOutDat* packet_data)
{
 DWORD bits = 0;
 int bitNum = packet_data->mode ? 21 : 13;
 int bitIdx = 0;
 for(int i = 0; i < bitNum; ++i)
 {
  if (i==11 || i==12) //BL or DE (special cases)
  {
   if (packet_data->out[i]!=0) {
    WRITEBIT32(bits, bitIdx++, (packet_data->out[i]==2)); //output state: 0 or 1
    WRITEBIT32(bits, bitIdx++, 1);                        //configure as output 
   }
   else
    bitIdx+=2; //00
  }
  else { //other
   WRITEBIT32(bits, bitIdx++, packet_data->out[i]); 
  }
 }
 
 mp_pdp->Bin32ToHex(bits, m_outgoing_packet);
}

//-----------------------------------------------------------------------
void CControlApp::Build_CHOKE_PAR(ChokePar* packet_data)
{
 mp_pdp->Bin16ToHex(packet_data->sm_steps, m_outgoing_packet);
 mp_pdp->Bin4ToHex(packet_data->testing, m_outgoing_packet); //fake parameter (actually it is command)
 mp_pdp->Bin8ToHex(packet_data->manual_delta, m_outgoing_packet); //fake parameter
 int choke_rpm_if = MathHelpers::Round(packet_data->choke_rpm_if * 1024.0f);
 mp_pdp->Bin16ToHex(choke_rpm_if, m_outgoing_packet);
 int choke_corr_time = MathHelpers::Round(packet_data->choke_corr_time[0] * 100.0f);
 mp_pdp->Bin16ToHex(choke_corr_time, m_outgoing_packet);
 choke_corr_time = MathHelpers::Round(packet_data->choke_corr_time[1] * 100.0f);
 mp_pdp->Bin16ToHex(choke_corr_time, m_outgoing_packet);
 //choke flags
 unsigned char flags = 0;
 WRITEBIT8(flags, 0, packet_data->useclrpmreg);
 WRITEBIT8(flags, 1, packet_data->offrpmreg_ongas);
 WRITEBIT8(flags, 2, packet_data->usethrottle_pos);
 WRITEBIT8(flags, 3, packet_data->sm_maxfreqinit);

 mp_pdp->Bin8ToHex(flags, m_outgoing_packet);

 BYTE sm_freq = packet_data->sm_freq;
 mp_pdp->Bin8ToHex(sm_freq, m_outgoing_packet);

 int cranktorun_time = MathHelpers::Round(packet_data->inj_cranktorun_time * 100.0f);
 mp_pdp->Bin16ToHex(cranktorun_time, m_outgoing_packet);
}

//-----------------------------------------------------------------------
void CControlApp::Build_GASDOSE_PAR(GasdosePar* packet_data)
{
 mp_pdp->Bin16ToHex(packet_data->gd_steps, m_outgoing_packet);
 mp_pdp->Bin4ToHex(packet_data->testing, m_outgoing_packet); //fake parameter (actually it is command)
 mp_pdp->Bin8ToHex(packet_data->manual_delta, m_outgoing_packet); //fake parameter
 BYTE fc_closing = MathHelpers::Round(packet_data->fc_closing * 2.0f);
 mp_pdp->Bin8ToHex(fc_closing, m_outgoing_packet);
 int corr_limit_p = MathHelpers::Round(packet_data->lam_corr_limit_p * 512.0f / 100.0f);
 mp_pdp->Bin16ToHex(corr_limit_p, m_outgoing_packet);
 int corr_limit_m = MathHelpers::Round(packet_data->lam_corr_limit_m * 512.0f / 100.0f);
 mp_pdp->Bin16ToHex(corr_limit_m, m_outgoing_packet);
 int lam_stoichval = MathHelpers::Round(packet_data->lam_stoichval * 128.0f);
 mp_pdp->Bin16ToHex(lam_stoichval, m_outgoing_packet);
 BYTE gd_freq = packet_data->gd_freq;
 mp_pdp->Bin8ToHex(gd_freq, m_outgoing_packet);
 BYTE gd_maxfreqinit = packet_data->gd_maxfreqinit;
 mp_pdp->Bin8ToHex(gd_maxfreqinit, m_outgoing_packet);
}

//-----------------------------------------------------------------------
void CControlApp::Build_SECUR_PAR(SecurPar* packet_data)
{
 char raw_name[32], raw_pass[32];
 CharToOem(packet_data->bt_name, raw_name);
 size_t numName = strlen(raw_name);
 CharToOem(packet_data->bt_pass, raw_pass);
 size_t numPass = strlen(raw_pass);

 mp_pdp->Bin4ToHex(numName, m_outgoing_packet); 
 mp_pdp->Bin4ToHex(numPass, m_outgoing_packet); 

 for(size_t i = 0; i < numName; ++i)
  m_outgoing_packet.push_back(raw_name[i]);

 for(size_t i = 0; i < numPass; ++i)
  m_outgoing_packet.push_back(raw_pass[i]);

 unsigned char flags = 0;
 WRITEBIT8(flags, 0, packet_data->use_bt);
 WRITEBIT8(flags, 1, packet_data->set_btbr);
 WRITEBIT8(flags, 2, packet_data->use_imm);
 WRITEBIT8(flags, 3, packet_data->use_respar);
 WRITEBIT8(flags, 4, packet_data->chk_fwcrc);
 WRITEBIT8(flags, 5, packet_data->bt_type);

 mp_pdp->Bin8ToHex(flags, m_outgoing_packet);

 //iButton keys
 for(int j = 0; j < IBTN_KEYS_NUM; ++j)
  for(int i = 0; i < IBTN_KEY_SIZE; ++i)
   mp_pdp->Bin8ToHex(packet_data->ibtn_keys[j][i], m_outgoing_packet);
}

//-----------------------------------------------------------------------
void CControlApp::Build_UNIOUT_PAR(UniOutPar* packet_data)
{
 CondEncoder cen(m_quartz_frq, m_period_distance);
 for(int oi = 0; oi < UNI_OUTPUT_NUM; ++oi)
 {
  unsigned char flags = ((packet_data->out[oi].logicFunc) << 4) | ((int)packet_data->out[oi].invers_2 << 1) | ((int)packet_data->out[oi].invers_1);
  mp_pdp->Bin8ToHex(flags, m_outgoing_packet);
  mp_pdp->Bin8ToHex(packet_data->out[oi].condition1, m_outgoing_packet);
  mp_pdp->Bin8ToHex(packet_data->out[oi].condition2, m_outgoing_packet);
  mp_pdp->Bin16ToHex(cen.UniOutEncodeCondVal(packet_data->out[oi].on_thrd_1, packet_data->out[oi].condition1), m_outgoing_packet);
  mp_pdp->Bin16ToHex(cen.UniOutEncodeCondVal(packet_data->out[oi].off_thrd_1, packet_data->out[oi].condition1), m_outgoing_packet);
  mp_pdp->Bin16ToHex(cen.UniOutEncodeCondVal(packet_data->out[oi].on_thrd_2, packet_data->out[oi].condition2), m_outgoing_packet);
  mp_pdp->Bin16ToHex(cen.UniOutEncodeCondVal(packet_data->out[oi].off_thrd_2, packet_data->out[oi].condition2), m_outgoing_packet);
 }
 mp_pdp->Bin4ToHex(packet_data->logicFunc12, m_outgoing_packet);
}

//-----------------------------------------------------------------------
void CControlApp::Build_INJCTR_PAR(InjctrPar* packet_data)
{
 unsigned char inj_flags = 0;
 WRITEBIT8(inj_flags, 0, packet_data->inj_usetimingmap[0]); 
 WRITEBIT8(inj_flags, 1, packet_data->inj_usetimingmap[1]); 
 WRITEBIT8(inj_flags, 2, packet_data->inj_useaddcorrs); 
 WRITEBIT8(inj_flags, 3, packet_data->inj_useairden); 
 WRITEBIT8(inj_flags, 4, packet_data->inj_usediffpress); 
 mp_pdp->Bin8ToHex(inj_flags, m_outgoing_packet);

 unsigned char inj_config = MAKEBYTE(packet_data->inj_config[0], packet_data->inj_squirt_num[0]);
 mp_pdp->Bin8ToHex(inj_config, m_outgoing_packet);
 inj_config = MAKEBYTE(packet_data->inj_config[1], packet_data->inj_squirt_num[1]);
 mp_pdp->Bin8ToHex(inj_config, m_outgoing_packet);

 int inj_flow_rate = MathHelpers::Round(packet_data->inj_flow_rate[0] * 64.0f);
 mp_pdp->Bin16ToHex(inj_flow_rate, m_outgoing_packet);
 inj_flow_rate = MathHelpers::Round(packet_data->inj_flow_rate[1] * 64.0f);
 mp_pdp->Bin16ToHex(inj_flow_rate, m_outgoing_packet);

 int inj_cyl_disp = MathHelpers::Round(packet_data->inj_cyl_disp * 16384.0f);
 mp_pdp->Bin16ToHex(inj_cyl_disp, m_outgoing_packet);

 mp_pdp->Bin32ToHex((unsigned long)packet_data->inj_sd_igl_const[0], m_outgoing_packet);
 mp_pdp->Bin32ToHex((unsigned long)packet_data->inj_sd_igl_const[1], m_outgoing_packet);

 mp_pdp->Bin8ToHex(0, m_outgoing_packet); //stub for cyl_num

 int inj_begin_angle = MathHelpers::Round(packet_data->inj_timing[0] * m_angle_multiplier);
 mp_pdp->Bin16ToHex(inj_begin_angle, m_outgoing_packet);
 inj_begin_angle = MathHelpers::Round(packet_data->inj_timing[1] * m_angle_multiplier);
 mp_pdp->Bin16ToHex(inj_begin_angle, m_outgoing_packet);

 int inj_begin_angle_crk = MathHelpers::Round(packet_data->inj_timing_crk[0] * m_angle_multiplier);
 mp_pdp->Bin16ToHex(inj_begin_angle_crk, m_outgoing_packet);
 inj_begin_angle_crk = MathHelpers::Round(packet_data->inj_timing_crk[1] * m_angle_multiplier);
 mp_pdp->Bin16ToHex(inj_begin_angle_crk, m_outgoing_packet);

 unsigned char inj_anglespec = MAKEBYTE(packet_data->inj_anglespec[1], packet_data->inj_anglespec[0]);
 mp_pdp->Bin8ToHex(inj_anglespec, m_outgoing_packet);

 int fff = MathHelpers::Round((packet_data->fff_const / (1000.0f*60.0f))*65536.0f);
 mp_pdp->Bin16ToHex(fff, m_outgoing_packet);
}

//-----------------------------------------------------------------------
void CControlApp::Build_LAMBDA_PAR(LambdaPar* packet_data)
{
 mp_pdp->Bin8ToHex(packet_data->lam_str_per_stp, m_outgoing_packet);
 int step_size_p = MathHelpers::Round(packet_data->lam_step_size_p * 512.0f / 100.0f);
 mp_pdp->Bin8ToHex(step_size_p, m_outgoing_packet);
 int step_size_m = MathHelpers::Round(packet_data->lam_step_size_m * 512.0f / 100.0f);
 mp_pdp->Bin8ToHex(step_size_m, m_outgoing_packet);
 int corr_limit_p = MathHelpers::Round(packet_data->lam_corr_limit_p * 512.0f / 100.0f);
 mp_pdp->Bin16ToHex(corr_limit_p, m_outgoing_packet);
 int corr_limit_m = MathHelpers::Round(packet_data->lam_corr_limit_m * 512.0f / 100.0f);
 mp_pdp->Bin16ToHex(corr_limit_m, m_outgoing_packet);
 int swt_point = MathHelpers::Round(packet_data->lam_swt_point / ADC_DISCRETE);
 mp_pdp->Bin16ToHex(swt_point, m_outgoing_packet);
 int temp_thrd = MathHelpers::Round(packet_data->lam_temp_thrd * TEMP_PHYSICAL_MAGNITUDE_MULTIPLIER);
 mp_pdp->Bin16ToHex(temp_thrd, m_outgoing_packet);
 mp_pdp->Bin16ToHex(packet_data->lam_rpm_thrd, m_outgoing_packet);
 mp_pdp->Bin8ToHex(packet_data->lam_activ_delay, m_outgoing_packet);
 int deadband = MathHelpers::Round(packet_data->lam_dead_band / ADC_DISCRETE);
 mp_pdp->Bin16ToHex(deadband, m_outgoing_packet);
 mp_pdp->Bin8ToHex(packet_data->lam_senstype, m_outgoing_packet);
 mp_pdp->Bin8ToHex(packet_data->lam_ms_per_stp / 10, m_outgoing_packet);
 unsigned char lam_htgdet = packet_data->lam_htgdet;
 mp_pdp->Bin8ToHex(lam_htgdet, m_outgoing_packet);
 int lam_2stoichval = MathHelpers::Round(packet_data->lam_2stoichval * 128.0f);
 mp_pdp->Bin16ToHex(lam_2stoichval, m_outgoing_packet);
 //heating
 mp_pdp->Bin8ToHex((BYTE)packet_data->eh_heating_time[0], m_outgoing_packet);
 mp_pdp->Bin8ToHex((BYTE)packet_data->eh_heating_time[1], m_outgoing_packet);
 mp_pdp->Bin8ToHex((BYTE)packet_data->eh_temper_thrd, m_outgoing_packet);
 BYTE eh_heating_act = MathHelpers::Round(packet_data->eh_heating_act * 100.0f);
 mp_pdp->Bin8ToHex(eh_heating_act, m_outgoing_packet);
 int eh_aflow_thrd = MathHelpers::Round(packet_data->eh_aflow_thrd / 32.0f);
 mp_pdp->Bin16ToHex(eh_aflow_thrd, m_outgoing_packet);
}

//-----------------------------------------------------------------------
void CControlApp::Build_ACCEL_PAR(AccelPar* packet_data)
{
 unsigned char tpsdot_thrd = MathHelpers::Round(packet_data->ae_tpsdot_thrd);
 mp_pdp->Bin8ToHex(tpsdot_thrd, m_outgoing_packet);

 unsigned char coldacc_mult = MathHelpers::Round(((packet_data->ae_coldacc_mult/100.0f) - 1.00f) * 128.0f);
 mp_pdp->Bin8ToHex(coldacc_mult, m_outgoing_packet);

 unsigned char ae_decay_time = packet_data->ae_decay_time;
 mp_pdp->Bin8ToHex(ae_decay_time, m_outgoing_packet);
}

//-----------------------------------------------------------------------
void CControlApp::SetEventHandler(EventHandler* i_pEventHandler)
{
 m_pEventHandler = i_pEventHandler;
 m_pEventHandler->mp_sync_object = GetSyncObject(); //link to synchronization object
};

//-----------------------------------------------------------------------
SECU3Packet& CControlApp::EndPendingPacket(void)
{
 SECU3IO::SECU3Packet& packet = m_pending_packets[m_pending_packets_index++];
 if (m_pending_packets_index >= PENDING_PACKETS_QUEUE_SIZE)
  m_pending_packets_index = 0;
 return packet;
}

//-----------------------------------------------------------------------
SECU3Packet& CControlApp::PendingPacket(void)
{
 return m_pending_packets[m_pending_packets_index];
}

//-----------------------------------------------------------------------
//for external use
inline CControlApp::CSECTION* CControlApp::GetSyncObject(void)
{
 ASSERT(mp_csection);
 return mp_csection;
}

//-----------------------------------------------------------------------
inline void CControlApp::EnterCriticalSection(void)
{
 ::EnterCriticalSection(GetSyncObject());
}

//-----------------------------------------------------------------------
inline void CControlApp::LeaveCriticalSection(void)
{
 ::LeaveCriticalSection(GetSyncObject());
}

//-----------------------------------------------------------------------
void CControlApp::SetNumPulsesPer1Km(int pp1km)
{
 double value = MathHelpers::RestrictValue(pp1km, 1, 60000);
 m_period_distance = (float)(1000.0 / value); //distance of one period in meters
}


//-----------------------------------------------------------------------
void CControlApp::SetSpeedUnit(int i_unit)
{
 m_speedUnit = i_unit;
}

//-----------------------------------------------------------------------
void CControlApp::SetFFFConst(int fffConst)
{
 m_fffConst = fffConst;
}

//-----------------------------------------------------------------------

CondEncoder::CondEncoder(long quartz_frq, float period_distance)
: m_quartz_frq(quartz_frq)
, m_period_distance(period_distance)
{
 //empty
}

int CondEncoder::UniOutEncodeCondVal(float val, int cond)
{
 switch(cond)
 {
  case UNIOUT_COND_CTS:  return MathHelpers::Round(val * TEMP_PHYSICAL_MAGNITUDE_MULTIPLIER);
  case UNIOUT_COND_RPM:  return MathHelpers::Round(val);
  case UNIOUT_COND_MAP:  return MathHelpers::Round(val * MAP_PHYSICAL_MAGNITUDE_MULTIPLIER);
  case UNIOUT_COND_UBAT: return MathHelpers::Round(val * UBAT_PHYSICAL_MAGNITUDE_MULTIPLIER);
  case UNIOUT_COND_CARB: return MathHelpers::Round(val);
  case UNIOUT_COND_VSPD:
   return MathHelpers::Round(((m_period_distance * (3600.0f / 1000.0f)) / val) * ((m_quartz_frq==20000000) ? 312500.0f: 250000.0f));
  case UNIOUT_COND_AIRFL: return MathHelpers::Round(val);
  case UNIOUT_COND_TMR: return MathHelpers::Round(val * 100.0f);
  case UNIOUT_COND_ITTMR: return MathHelpers::Round(val * 100.0f);
  case UNIOUT_COND_ESTMR: return MathHelpers::Round(val * 100.0f);
  case UNIOUT_COND_CPOS: return MathHelpers::Round(val * CHOKE_PHYSICAL_MAGNITUDE_MULTIPLIER);
  case UNIOUT_COND_AANG: return MathHelpers::Round(val * ANGLE_MULTIPLIER);
  case UNIOUT_COND_KLEV: return MathHelpers::Round(val * (1.0 / ADC_DISCRETE));
  case UNIOUT_COND_TPS: return MathHelpers::Round(val * TPS_PHYSICAL_MAGNITUDE_MULTIPLIER);
  case UNIOUT_COND_ATS: return MathHelpers::Round(val * TEMP_PHYSICAL_MAGNITUDE_MULTIPLIER);
  case UNIOUT_COND_AI1: return MathHelpers::Round(val * (1.0 / ADC_DISCRETE));
  case UNIOUT_COND_AI2: return MathHelpers::Round(val * (1.0 / ADC_DISCRETE));
  case UNIOUT_COND_AI3: return MathHelpers::Round(val * (1.0 / ADC_DISCRETE));
  case UNIOUT_COND_AI4: return MathHelpers::Round(val * (1.0 / ADC_DISCRETE));
  case UNIOUT_COND_GASV: return MathHelpers::Round(val);
  case UNIOUT_COND_IPW: return MathHelpers::Round((val * 1000.0f) / 3.2);
  case UNIOUT_COND_CE: return MathHelpers::Round(val);
  case UNIOUT_COND_OFTMR: return MathHelpers::Round(val * 100.0f);
 }
 return 0;
}

float CondEncoder::UniOutDecodeCondVal(int val, int cond)
{
 switch(cond)
 {
  case UNIOUT_COND_CTS:  return (((float)val) / TEMP_PHYSICAL_MAGNITUDE_MULTIPLIER);
  case UNIOUT_COND_RPM:  return (float)val;
  case UNIOUT_COND_MAP:  return (((float)val) / MAP_PHYSICAL_MAGNITUDE_MULTIPLIER);
  case UNIOUT_COND_UBAT: return (((float)val) / UBAT_PHYSICAL_MAGNITUDE_MULTIPLIER);
  case UNIOUT_COND_CARB: return (float)val;
  case UNIOUT_COND_VSPD:
  {
   float period_s = ((float)val / ((m_quartz_frq==20000000) ? 312500.0f: 250000.0f)); //period in seconds
   float speed = ((m_period_distance / period_s) * 3600.0f) / 1000.0f; //Km/h
   if (speed > 999.9f) speed = 999.9f;
   return speed;
  }
  case UNIOUT_COND_AIRFL: return (float)val;
  case UNIOUT_COND_TMR: return ((float)val) / 100.0f;
  case UNIOUT_COND_ITTMR: return ((float)val) / 100.0f;
  case UNIOUT_COND_ESTMR: return ((float)val) / 100.0f;
  case UNIOUT_COND_CPOS: return ((float)val) / CHOKE_PHYSICAL_MAGNITUDE_MULTIPLIER;
  case UNIOUT_COND_AANG: return ((float)val) / ANGLE_MULTIPLIER;
  case UNIOUT_COND_KLEV: return ((float)val) / (1.0f / ADC_DISCRETE);
  case UNIOUT_COND_TPS: return ((float)val) / TPS_PHYSICAL_MAGNITUDE_MULTIPLIER;
  case UNIOUT_COND_ATS: return ((float)val) / TEMP_PHYSICAL_MAGNITUDE_MULTIPLIER;
  case UNIOUT_COND_AI1: return ((float)val) / (1.0f / ADC_DISCRETE);
  case UNIOUT_COND_AI2: return ((float)val) / (1.0f / ADC_DISCRETE);
  case UNIOUT_COND_AI3: return ((float)val) / (1.0f / ADC_DISCRETE);
  case UNIOUT_COND_AI4: return ((float)val) / (1.0f / ADC_DISCRETE);
  case UNIOUT_COND_GASV: return (float)val;
  case UNIOUT_COND_IPW: return (val * 3.2f) / 1000.0f;
  case UNIOUT_COND_CE: return (float)val;
  case UNIOUT_COND_OFTMR: return ((float)val) / 100.0f;
 }
 return .0f;
}

bool CondEncoder::isSigned(int cond) const
{
 switch(cond)
 {
  case UNIOUT_COND_CTS:
  case UNIOUT_COND_ATS:
  case UNIOUT_COND_AANG:
   return true;
  default:
   return false;
 }
}
