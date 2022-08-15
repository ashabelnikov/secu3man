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
#include <winnt.h>

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

int FletcherChecksum(std::vector<BYTE>& i_data, size_t offset, size_t size)
{
 BYTE chksum[2] = {0,0};
 for(size_t i = 0; i < size; ++i)
 {
  size_t index = i + offset;
  chksum[0]+=i_data[index];
  chksum[1]+=chksum[0];
 }
 return *((unsigned short*)(&chksum[0]));
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
, m_ignore_n_packets(0)
, m_splitAng(false)
, m_blman_packet(false)
{
 m_pPackets = new Packets();
 m_pPackets->reserve(256);
 for (size_t i = 0; i < m_pPackets->size(); ++i)
  (*m_pPackets)[i].reserve(256);
 m_ingoing_packet.reserve(256);
 m_outgoing_packet.reserve(256);

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
 SECU3IO::SensorDat& sensorDat = m_recepted_packet.m_SensorDat;
 if (size != 89)  //размер пакета без сигнального символа, дескриптора и символа-конца пакета
  return false;

 //частота вращения коленвала двигателя
 if (false == mp_pdp->Hex16ToBin(raw_packet, &sensorDat.frequen))
  return false;

 //давление во впускном коллекторе
 int pressure = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &pressure))
  return false;
 sensorDat.pressure = ((float)pressure) / MAP_PHYSICAL_MAGNITUDE_MULTIPLIER;

 //напряжение бортовой сети
 int voltage = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&voltage))
  return false;
 sensorDat.voltage = ((float)voltage) / UBAT_PHYSICAL_MAGNITUDE_MULTIPLIER;

 //Температура охлаждающей жидкости
 int temperature = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&temperature,true))
  return false;
 sensorDat.temperat = ((float)temperature) / TEMP_PHYSICAL_MAGNITUDE_MULTIPLIER;
 sensorDat.temperat = MathHelpers::RestrictValue(sensorDat.temperat, -99.9f, 999.0f);

 //Текущий УОЗ (число со знаком)
 int adv_angle = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&adv_angle,true))
  return false;
 sensorDat.adv_angle = ((float)adv_angle) / m_angle_multiplier;

 //Уровень детонации двигателя
 int knock_k = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&knock_k))
  return false;
 sensorDat.knock_k = ((float)knock_k) * m_adc_discrete;

 //Корректировка УОЗ при детонации
 int knock_retard = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&knock_retard, true))
  return false;
 sensorDat.knkret_use = (knock_retard != 32767);
 sensorDat.knock_retard = sensorDat.knkret_use ? ((float)knock_retard) / m_angle_multiplier : 0;

 //Расход воздуха
 if (false == mp_pdp->Hex8ToBin(raw_packet,&sensorDat.air_flow))
  return false;

 //16 bit flags
 int flags = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &flags))
  return false;

 //flags
 sensorDat.ephh_valve   = CHECKBIT16(flags, 0);
 sensorDat.carb         = CHECKBIT16(flags, 1);
 sensorDat.gas          = CHECKBIT16(flags, 2);
 sensorDat.epm_valve    = CHECKBIT16(flags, 3);
 sensorDat.ce_state     = CHECKBIT16(flags, 4);
 sensorDat.cool_fan     = CHECKBIT16(flags, 5);
 sensorDat.st_block     = CHECKBIT16(flags, 6);
 sensorDat.acceleration = CHECKBIT16(flags, 7);
 sensorDat.fc_revlim    = CHECKBIT16(flags, 8);
 sensorDat.floodclear   = CHECKBIT16(flags, 9);
 sensorDat.sys_locked   = CHECKBIT16(flags, 10);
 sensorDat.ign_i        = CHECKBIT16(flags, 11);
 sensorDat.cond_i       = CHECKBIT16(flags, 12);
 sensorDat.epas_i       = CHECKBIT16(flags, 13);
 sensorDat.aftstr_enr   = CHECKBIT16(flags, 14);
 sensorDat.iac_cl_loop  = CHECKBIT16(flags, 15);

 //TPS sensor
 unsigned char tps = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet,&tps))
  return false;
 sensorDat.tps = ((float)tps) / TPS_PHYSICAL_MAGNITUDE_MULTIPLIER;

 //ADD_I1 input
 int add_i1_v = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&add_i1_v))
  return false;
 sensorDat.add_i1 = ((float)add_i1_v) * m_adc_discrete;

 //ADD_I2 input
 int add_i2_v = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&add_i2_v))
  return false;
 sensorDat.add_i2 = ((float)add_i2_v) * m_adc_discrete;

 //Биты ошибок СЕ
 unsigned long ce_errors = 0;
 if (false == mp_pdp->Hex32ToBin(raw_packet, &ce_errors))
  return false;
 sensorDat.ce_errors = ce_errors;

 //Choke position
 unsigned char choke_pos = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &choke_pos))
  return false;
 sensorDat.choke_pos = ((float)choke_pos) / CHOKE_PHYSICAL_MAGNITUDE_MULTIPLIER;

 //gas dosator position
 unsigned char gasdose_pos = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &gasdose_pos))
  return false;
 sensorDat.gasdose_pos = ((float)gasdose_pos) / GD_PHYSICAL_MAGNITUDE_MULTIPLIER;

 //Vehicle speed
 int speed = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&speed))
  return false;
 if (0 != speed && 65535 != speed)
 { //speed sensor is used, value is correct
  float period_s = ((float)speed / ((m_quartz_frq==20000000) ? 312500.0f: 250000.0f)); //period in seconds
  sensorDat.speed = ((m_period_distance / period_s) * 3600.0f) / 1000.0f; //Km/h
  if (sensorDat.speed > 999.9f)
   sensorDat.speed = 999.9f;
  //convert to selected unit
  if (m_speedUnit == 1)
   sensorDat.speed/= 1.609344f;
 }
 else //speed sensor is not used or speed is too low
  sensorDat.speed = 0;

 //Distance
 unsigned long distance = 0;
 if (false == mp_pdp->Hex24ToBin(raw_packet,&distance))
  return false;
 sensorDat.distance = (m_period_distance * distance) / 1000.0f;
 if (sensorDat.distance > 9999.99f)
  sensorDat.distance = 9999.99f;
 //convert to selected unit
 if (m_speedUnit == 1)
  sensorDat.distance/= 1.609344f;

 //Fuel flow frequency
 int inj_fff = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &inj_fff))
  return false;
 sensorDat.inj_fff = ((float)inj_fff) / 256.0f; //because raw value multiplied by 256

 //calculate value of fuel flow in L/100km
 if (sensorDat.speed > .0f) 
  sensorDat.inj_ffd = (sensorDat.inj_fff / sensorDat.speed) * ((3600.0f * 100.0f) / ((float)m_fffConst));
 else
  sensorDat.inj_ffd = .0f;

 sensorDat.inj_ffh = (3600.0f * sensorDat.inj_fff) / ((float)m_fffConst); //consumption in L/h

 //Intake air temperature
 int air_temp = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&air_temp,true))
  return false;

 if (air_temp!=0x7FFF)
 {
  sensorDat.air_temp = ((float)air_temp) / TEMP_PHYSICAL_MAGNITUDE_MULTIPLIER;
  sensorDat.air_temp = MathHelpers::RestrictValue(sensorDat.air_temp, -99.9f, 999.0f);
  sensorDat.add_i2_mode = 1;
 }
 else //do not show air temperature
 {
  sensorDat.air_temp = .0f;
  sensorDat.add_i2_mode = 0;
 }

 // Advance angle from start map (signed value)
 int strt_aalt = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&strt_aalt,true))
  return false;
 sensorDat.strt_use = (strt_aalt != 32767);
 sensorDat.strt_aalt = sensorDat.strt_use ? (((float)strt_aalt) / m_angle_multiplier) : 0;

 //Advance angle from idle map (signed value)
 int idle_aalt = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&idle_aalt,true))
  return false;
 sensorDat.idle_use = (idle_aalt != 32767);
 sensorDat.idle_aalt = sensorDat.idle_use ? (((float)idle_aalt) / m_angle_multiplier) : 0;

 // Advance angle from work map (signed value)
 int work_aalt = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&work_aalt,true))
  return false;
 sensorDat.work_use = (work_aalt != 32767);
 sensorDat.work_aalt = sensorDat.work_use ? (((float)work_aalt) / m_angle_multiplier) : 0;

 // Advance angle from coolant temperature correction map (signed value)
 int temp_aalt = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&temp_aalt,true))
  return false;
 sensorDat.temp_use = (temp_aalt != 32767);
 sensorDat.temp_aalt = sensorDat.temp_use ? (((float)temp_aalt) / m_angle_multiplier) : 0;

 // Advance angle from air temperature correction map (signed value)
 int airt_aalt = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&airt_aalt,true))
  return false;
 sensorDat.airt_use = (airt_aalt != 32767);
 sensorDat.airt_aalt = sensorDat.airt_use ? (((float)airt_aalt) / m_angle_multiplier) : 0;

 // Advance angle from correction from idling RPM regulator (signed value)
 int idlreg_aac = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&idlreg_aac,true))
  return false;
 sensorDat.idlreg_use = (idlreg_aac != 32767);
 sensorDat.idlreg_aac = sensorDat.idlreg_use ? (((float)idlreg_aac) / m_angle_multiplier) : 0;

 // Octane correction value (signed value)
 int octan_aac = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&octan_aac,true))
  return false;
 sensorDat.octan_use = (octan_aac != 32767);
 sensorDat.octan_aac = sensorDat.octan_use ? (((float)octan_aac) / m_angle_multiplier) : 0;

 // Lambda correction value (signed value)
 int lambda_corr = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&lambda_corr,true))
  return false;
 sensorDat.lambda_corr = (((float)lambda_corr) / 512.0f) * 100.0f; //obtain value in %

 //Injector pulse width (ms)
 int inj_pw = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &inj_pw))
  return false;
 sensorDat.inj_pw = (inj_pw * 3.2f) / 1000.0f;

 //TPS opening/closing speed (d%/dt = %/s), signed value
 int tpsdot = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &tpsdot, true))
  return false;
 sensorDat.tpsdot = tpsdot;

 //Давление газа
 int map2 = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &map2))
  return false;
 sensorDat.map2 = ((float)map2) / MAP_PHYSICAL_MAGNITUDE_MULTIPLIER;
 //calculate here differential pressure
 sensorDat.mapd = (sensorDat.map2 - sensorDat.pressure);

 //Температура газа
 int tmp2 = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &tmp2,true))
  return false;
 sensorDat.tmp2 = ((float)tmp2) / TEMP_PHYSICAL_MAGNITUDE_MULTIPLIER;
 sensorDat.tmp2 = MathHelpers::RestrictValue(sensorDat.tmp2, -99.9f, 999.0f);

 //AFR value
 int afr = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &afr))
  return false;
 sensorDat.afr = ((float)afr) / AFR_PHYSICAL_MAGNITUDE_MULTIPLIER;

 //Load value
 int load = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &load))
  return false;
 sensorDat.load = ((float)load) / LOAD_PHYSICAL_MAGNITUDE_MULTIPLIER;

 //baro pressure
 int baro_press = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &baro_press))
  return false;
 sensorDat.baro_press = ((float)baro_press) / MAP_PHYSICAL_MAGNITUDE_MULTIPLIER;

 //inj.timing with info
 int iit = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &iit))
  return false;
 int mode = (iit >> 14) & 0x3;
 float inj_timing = ((float)(iit & 0x3FFF)) / 16.0f; //inj.timing in crankshaft degrees
 float inj_pw_degr = (((360.0f/(1000.0f*60.0f))* sensorDat.frequen) * sensorDat.inj_pw); //inj. PW in crankshaft degrees
 if (mode == 0)
 { //begin
  sensorDat.inj_tim_begin = inj_timing;
  sensorDat.inj_tim_end = inj_timing - inj_pw_degr;
 }
 else if (mode == 1)
 { //middle
  sensorDat.inj_tim_begin = inj_timing + (inj_pw_degr / 2);
  sensorDat.inj_tim_end = inj_timing - (inj_pw_degr / 2);
 }
 else
 {//end
  sensorDat.inj_tim_begin = inj_timing + inj_pw_degr;
  sensorDat.inj_tim_end = inj_timing; 
 }
 if (sensorDat.inj_tim_begin > 720.0f)
  sensorDat.inj_tim_begin = sensorDat.inj_tim_begin - 720.f;
 if (sensorDat.inj_tim_end < 0)
  sensorDat.inj_tim_end = sensorDat.inj_tim_end + 720.0f;

 //idling regulator's rigidity
 BYTE rigid_arg = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &rigid_arg))
  return false;
 sensorDat.rigid_use = (rigid_arg != 255);
 sensorDat.rigid_arg = sensorDat.rigid_use ? (1.0f + (((float)rigid_arg) / (256.0f/7.0f))) : 0;

 //Gas reducer's temperature
 int grts = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &grts, true))
  return false;
 sensorDat.grts = ((float)grts) / TEMP_PHYSICAL_MAGNITUDE_MULTIPLIER;
 sensorDat.grts = MathHelpers::RestrictValue(sensorDat.grts, -99.9f, 999.0f);

 //RxL
 int rxlaf = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &rxlaf))
  return false;
 sensorDat.rxlaf = rxlaf * 32;

 //level of fuel in the fuel tank
 int ftls = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &ftls, true))
  return false;
 sensorDat.ftls = ((float)ftls) / FTLS_MULT;

 //exhaust gas temperature
 int egts = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &egts, true))
  return false;
 sensorDat.egts = ((float)egts) / EGTS_MULT;

 //oil pressure 
 int ops = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &ops, true))
  return false;
 sensorDat.ops = ((float)ops) / OPS_MULT;

 //injector's duty
 unsigned char inj_duty = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &inj_duty))
  return false;
 sensorDat.inj_duty = ((float)inj_duty) / 2.0f;

 //mass air flow (g/sec)
 int maf = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &maf, true))
  return false;
 sensorDat.maf = ((float)maf) / MAFS_MULT;

 //PWM duty of cooling fan
 unsigned char vent_duty = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &vent_duty))
  return false;
 sensorDat.vent_duty = ((float)vent_duty) / 2.0f;

 //8 bit flags of universal outputs
 int uniout = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &uniout))
  return false;
 //universal output's state flags
 for(int i = 0; i < UNI_OUTPUT_NUM; ++i)
  sensorDat.uniout[i]   = CHECKBIT8(uniout, i);

 //MAPdot (dP/dt = kPa/s), signed value
 int mapdot = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &mapdot, true))
  return false;
 sensorDat.mapdot = mapdot;

 //FTS (Fuel temparature sensor)
 int fts = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &fts, true))
  return false;
 sensorDat.fts = ((float)fts) / FTS_MULT;

 return true;
}

//-----------------------------------------------------------------------
bool CControlApp::Parse_DBGVAR_DAT(const BYTE* raw_packet, size_t size)
{
 SECU3IO::DbgvarDat& m_DbgvarDat = m_recepted_packet.m_DbgvarDat;
 if (size !=  8)  //размер пакета без сигнального символа, дескриптора и символа-конца пакета
  return false;

 //variable 1
 if (false == mp_pdp->Hex16ToBin(raw_packet, &m_DbgvarDat.var1))
  return false;

 //variable 2
 if (false == mp_pdp->Hex16ToBin(raw_packet, &m_DbgvarDat.var2))
  return false;

 //variable 3
 if (false == mp_pdp->Hex16ToBin(raw_packet, &m_DbgvarDat.var3))
  return false;

 //variable 4
 if (false == mp_pdp->Hex16ToBin(raw_packet, &m_DbgvarDat.var4))
  return false;

 return true;
}

//-----------------------------------------------------------------------
bool CControlApp::Parse_FNNAME_DAT(const BYTE* raw_packet, size_t size)
{
 SECU3IO::FnNameDat& fnNameDat = m_recepted_packet.m_FnNameDat;
 if (size != 18)  //размер пакета без сигнального символа, дескриптора и символа-конца пакета
  return false;

 //Общее кол-во наборов (семейств характеристик)
 if (false == mp_pdp->Hex8ToBin(raw_packet,&fnNameDat.tables_num))
  return false;

 //номер этого набора характеристик
 if (false == mp_pdp->Hex8ToBin(raw_packet,&fnNameDat.index))
  return false;

 //имя этого набора характеристик
 size_t fn_name_size = size - 2;
 mp_pdp->HexStrToBin(raw_packet, fn_name_size, fnNameDat.name);

 //Заменяем символы FF на 0x20 
 for(size_t i = 0; i < fn_name_size; ++i)
  if (((unsigned char)fnNameDat.name[i]) == 0xFF)
   fnNameDat.name[i] = 0x20;

 return true;
}

//-----------------------------------------------------------------------
bool CControlApp::Parse_STARTR_PAR(const BYTE* raw_packet, size_t size)
{
 SECU3IO::StartrPar& startrPar = m_recepted_packet.m_StartrPar;
 if (size != 16)  //размер пакета без сигнального символа, дескриптора и символа-конца пакета
  return false;

 //Обороты при которых стартер будет выключен
 if (false == mp_pdp->Hex16ToBin(raw_packet,&startrPar.starter_off))
  return false;

 //Обороты перехода с пусковой карты
 if (false == mp_pdp->Hex16ToBin(raw_packet,&startrPar.smap_abandon))
  return false;

 //IAC Crank to run time
 int cranktorun_time = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &cranktorun_time))
  return false;
 startrPar.inj_cranktorun_time = float(cranktorun_time) / 100.0f;

 //Time of afterstart enrichment in strokes
 unsigned char aftstr_strokes = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &aftstr_strokes))
  return false;
 startrPar.inj_aftstr_strokes[0] = aftstr_strokes * 4;

 float discrete = (m_quartz_frq == 20000000 ? 3.2f : 4.0f);
 //prime pulse at -30C
 int prime_cold = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &prime_cold))
  return false;
 startrPar.inj_prime_cold = (float(prime_cold) * discrete) / 1000.0f; //convert to ms
 //prime pulse at 70C
 int prime_hot = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &prime_hot))
  return false;
 startrPar.inj_prime_hot = (float(prime_hot) * discrete) / 1000.0f; //convert to ms
 //prime pulse delay
 unsigned char prime_delay = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &prime_delay))
  return false;
 startrPar.inj_prime_delay = float(prime_delay) / 10.0f;            //convert to seconds

 //flood clear mode entering threshold (% of TPS)
 unsigned char inj_floodclear_tps = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &inj_floodclear_tps))
  return false;
 startrPar.inj_floodclear_tps = float(inj_floodclear_tps) / 2.0f;

 //Time of afterstart enrichment in strokes (gas)
 unsigned char aftstr_strokes1 = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &aftstr_strokes1))
  return false;
 startrPar.inj_aftstr_strokes[1] = aftstr_strokes1 * 4;

 //Starter bloking strokes
 unsigned char stbl_str_cnt = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &stbl_str_cnt))
  return false;
 startrPar.stbl_str_cnt = stbl_str_cnt;

 //Cranking flags
 BYTE strt_flags = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &strt_flags))
  return false;
 startrPar.fldclr_start = CHECKBIT8(strt_flags, 0);

 return true;
}

//-----------------------------------------------------------------------
bool CControlApp::Parse_ANGLES_PAR(const BYTE* raw_packet, size_t size)
{
 SECU3IO::AnglesPar& anglesPar = m_recepted_packet.m_AnglesPar;
 if (size != 14)  //размер пакета без сигнального символа, дескриптора и символа-конца пакета
  return false;

 //Максимальный, допустимый УОЗ (число со знаком)
 int max_angle;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&max_angle,true))
  return false;
 anglesPar.max_angle = ((float)max_angle) / m_angle_multiplier;

 //Минимальный, допустимый УОЗ (число со знаком)
 int  min_angle;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&min_angle,true))
  return false;
 anglesPar.min_angle = ((float)min_angle) / m_angle_multiplier;

 //Октан-коррекция УОЗ (число со знаком)
 int angle_corr;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&angle_corr,true))
  return false;
 anglesPar.angle_corr = ((float)angle_corr) / m_angle_multiplier;

 //Скорость уменьшения УОЗ (число со знаком)
 int dec_speed;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&dec_speed,true))
  return false;
 anglesPar.dec_speed = ((float)dec_speed) / m_angle_multiplier;

//Скорость увеличения УОЗ (число со знаком)
 int inc_speed;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&inc_speed,true))
  return false;
 anglesPar.inc_speed = ((float)inc_speed) / m_angle_multiplier;

 //Признак нулевого УОЗ
 if (false == mp_pdp->Hex4ToBin(raw_packet, &anglesPar.zero_adv_ang))
  return false;

 //Ignition timing flags
 BYTE ign_flags = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &ign_flags))
  return false;
 anglesPar.igntim_wrkmap = CHECKBIT8(ign_flags, 0);
 anglesPar.manigntim_idl = CHECKBIT8(ign_flags, 1);
 anglesPar.zero_adv_ang_oct = CHECKBIT8(ign_flags, 2);

 //Ignition timing for shifting 
 int shift_igntim;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &shift_igntim, true))
  return false;
 anglesPar.shift_igntim = ((float)shift_igntim) / m_angle_multiplier;

 return true;
}

//-----------------------------------------------------------------------
bool CControlApp::Parse_FUNSET_PAR(const BYTE* raw_packet, size_t size)
{
 SECU3IO::FunSetPar& funSetPar = m_recepted_packet.m_FunSetPar;
 if (size != 33)  //размер пакета без сигнального символа, дескриптора и символа-конца пакета
  return false;

 //Номер семейства характеристик используемого для бензина
 if (false == mp_pdp->Hex8ToBin(raw_packet,&funSetPar.fn_benzin))
  return false;

 //Номер семейства характеристик используемого для газа
 if (false == mp_pdp->Hex8ToBin(raw_packet,&funSetPar.fn_gas))
  return false;

 //Нижнее значение давления по оси ДАД
 int map_lower_pressure = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&map_lower_pressure))
  return false;
 funSetPar.map_lower_pressure = ((float)map_lower_pressure) / MAP_PHYSICAL_MAGNITUDE_MULTIPLIER;

 //Верхнее значение давления по оси ДАД
 int map_upper_pressure = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&map_upper_pressure))
  return false;
 funSetPar.map_upper_pressure = ((float)map_upper_pressure) / MAP_PHYSICAL_MAGNITUDE_MULTIPLIER;

 //Смещение кривой ДАД
 int map_curve_offset = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &map_curve_offset, true))
  return false;
 funSetPar.map_curve_offset = ((float)map_curve_offset) * m_adc_discrete;

 //Наклон кривой ДАД
 int map_curve_gradient = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &map_curve_gradient, true))
  return false;
 funSetPar.map_curve_gradient = ((float)map_curve_gradient) / (MAP_PHYSICAL_MAGNITUDE_MULTIPLIER * m_adc_discrete * 128.0f);

 //Смещение кривой ДАД2
 int map2_curve_offset = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &map2_curve_offset, true))
  return false;
 funSetPar.map2_curve_offset = ((float)map2_curve_offset) * m_adc_discrete;

 //Наклон кривой ДАД2
 int map2_curve_gradient = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &map2_curve_gradient, true))
  return false;
 funSetPar.map2_curve_gradient = ((float)map2_curve_gradient) / (MAP_PHYSICAL_MAGNITUDE_MULTIPLIER * m_adc_discrete * 128.0f);

 //TPS sensor curve offset
 int tps_curve_offset = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &tps_curve_offset, true))
  return false;
 funSetPar.tps_curve_offset = ((float)tps_curve_offset) * m_adc_discrete;

 //TPS sensor curve slope
 int tps_curve_gradient = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &tps_curve_gradient, true))
  return false;
 funSetPar.tps_curve_gradient = ((float)tps_curve_gradient) / ((TPS_PHYSICAL_MAGNITUDE_MULTIPLIER*64) * m_adc_discrete * 128.0f);

 //Engine load measurement source
 if (false == mp_pdp->Hex4ToBin(raw_packet,&funSetPar.load_src_cfg))
  return false;

 //Uni.outputs selected for map selection
 BYTE mapsel_uni = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &mapsel_uni))
  return false;
 funSetPar.uni_gas = mapsel_uni >> 4;
 funSetPar.uni_benzin = mapsel_uni & 0xF;
 if (funSetPar.uni_gas == 0xF)
  funSetPar.uni_gas = UNI_OUTPUT_NUM; //disabled
 if (funSetPar.uni_benzin == 0xF)
  funSetPar.uni_benzin = UNI_OUTPUT_NUM; //disabled

 BYTE barocorr_type = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &barocorr_type))
  return false;
 funSetPar.barocorr_type = barocorr_type;

 //flags
 BYTE func_flags = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &func_flags))
  return false;
 funSetPar.use_load_grid = CHECKBIT8(func_flags, 0);

 //VE2 map function
 BYTE ve2_map_func = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &ve2_map_func))
  return false;
 funSetPar.ve2_map_func = ve2_map_func;

 //Uni.outputs selected for GAS_V
 BYTE gas_v_uni = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &gas_v_uni))
  return false;
 funSetPar.uni_gas_v = gas_v_uni;
 if (funSetPar.uni_gas_v == 0xF)
  funSetPar.uni_gas_v = UNI_OUTPUT_NUM; //disabled

 //read-only parameter: number of engine cylinders
 unsigned char cyl_num = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &cyl_num))
  return false;
 funSetPar.cyl_num = cyl_num;
 
 int inj_cyl_disp = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &inj_cyl_disp))
  return false;
 funSetPar.inj_cyl_disp = float(inj_cyl_disp) / 16384.0f;

 unsigned long mafload_const = 0;
 if (false == mp_pdp->Hex32ToBin(raw_packet, &mafload_const))
  return false;
 funSetPar.mafload_const = (float)mafload_const;

 //Throttle position sensor
 signed int tps_raw = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &tps_raw, true))
  return false;
 funSetPar.tps_raw = tps_raw * m_adc_discrete;

 return true;
}


//-----------------------------------------------------------------------
bool CControlApp::Parse_IDLREG_PAR(const BYTE* raw_packet, size_t size)
{
 SECU3IO::IdlRegPar& idlRegPar = m_recepted_packet.m_IdlRegPar;
 if (size != 34)  //размер пакета без сигнального символа, дескриптора и символа-конца пакета
  return false;

 //Idling regulator flags
 BYTE idl_flags = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet,&idl_flags))
  return false;

 idlRegPar.idl_regul = CHECKBIT8(idl_flags, 0);
 idlRegPar.use_regongas = CHECKBIT8(idl_flags, 1);
 idlRegPar.closed_loop = CHECKBIT8(idl_flags, 2);
 idlRegPar.preg_mode = CHECKBIT8(idl_flags, 3);
 idlRegPar.idl_useiacclongas = CHECKBIT8(idl_flags, 4);
 idlRegPar.use_thrassmap = CHECKBIT8(idl_flags, 5);

 //Коэффициент регулятора при  положительной ошибке (число со знаком)
 int ifac1;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&ifac1,true))
  return false;
 idlRegPar.ifac1 = ((float)ifac1) / 256.0f;

 //Коэффициент регулятора при  отрицательной ошибке (число со знаком)
 int ifac2;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&ifac2,true))
  return false;
 idlRegPar.ifac2 = ((float)ifac2) / 256.0f;

 //Зона нечувствительности регулятора
 if (false == mp_pdp->Hex16ToBin(raw_packet,&idlRegPar.MINEFR))
  return false;

 //Поддерживаемые обороты
 if (false == mp_pdp->Hex16ToBin(raw_packet,&idlRegPar.idling_rpm))
  return false;

 //Минимальный УОЗ (число со знаком)
 int min_angle;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&min_angle,true))
  return false;
 idlRegPar.min_angle = ((float)min_angle) / m_angle_multiplier;

 //Максимальный УОЗ (число со знаком)
 int max_angle;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&max_angle,true))
  return false;
 idlRegPar.max_angle = ((float)max_angle) / m_angle_multiplier;

 //Порог включения регулятора ХХ по температуре (число со знаком)
 int turn_on_temp = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&turn_on_temp,true))
  return false;
 idlRegPar.turn_on_temp = ((float)turn_on_temp) / TEMP_PHYSICAL_MAGNITUDE_MULTIPLIER;

 //CLOSED LOOP PARAMETERS (FUEL INJECTION)

 //Value (in %) added to IAC position when exiting from closed loop
 unsigned char idl_to_run_add = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &idl_to_run_add))
  return false;
 idlRegPar.idl_to_run_add = ((float)idl_to_run_add) / 2.0f;

 //Value added to target RPM when vehicle starts to run
 unsigned char rpm_on_run_add = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &rpm_on_run_add))
  return false;
 idlRegPar.rpm_on_run_add = rpm_on_run_add * 10;

 //IAC closeed loop proportional coefficient (-)
 int idl_reg_p = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &idl_reg_p))
  return false;
 idlRegPar.idl_reg_p[0] = ((float)idl_reg_p) / 256.0f;

 //IAC closeed loop proportional coefficient (+)
 idl_reg_p = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &idl_reg_p))
  return false;
 idlRegPar.idl_reg_p[1] = ((float)idl_reg_p) / 256.0f;

 //IAC closed loop integral coefficient (-)
 int idl_reg_i = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &idl_reg_i))
  return false;
 idlRegPar.idl_reg_i[0] = ((float)idl_reg_i) / 256.0f;

 //IAC closed loop integral coefficient (+)
 idl_reg_i = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &idl_reg_i))
  return false;
 idlRegPar.idl_reg_i[1] = ((float)idl_reg_i) / 256.0f;

 //coefficient for calculating closed loop entering RPM threshold
 unsigned char idl_coef_thrd1 = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &idl_coef_thrd1))
  return false;
 idlRegPar.idl_coef_thrd1 = (((float)idl_coef_thrd1) / 128.0f) + 1.0f;

 //coefficient for calculating closed loop leaving RPM threshold
 unsigned char idl_coef_thrd2 = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &idl_coef_thrd2))
  return false;
 idlRegPar.idl_coef_thrd2 = (((float)idl_coef_thrd2) / 128.0f) + 1.0f;

 //RPM error limit for integrator
 unsigned char idl_intrpm_lim = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &idl_intrpm_lim))
  return false;
 idlRegPar.idl_intrpm_lim = idl_intrpm_lim * 10;

 //intake manifold pressure on idling
 int idl_map_value = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &idl_map_value))
  return false;
 idlRegPar.idl_map_value = ((float)idl_map_value) / MAP_PHYSICAL_MAGNITUDE_MULTIPLIER;

 //minimum IAC position
 unsigned char idl_iacminpos = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &idl_iacminpos))
  return false;
 idlRegPar.idl_iacminpos = ((float)idl_iacminpos / 2.0f);

 //maximum IAC position
 unsigned char idl_iacmaxpos = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &idl_iacmaxpos))
  return false;
 idlRegPar.idl_iacmaxpos = ((float)idl_iacmaxpos / 2.0f);

 //Зона нечувствительности регулятора
 if (false == mp_pdp->Hex16ToBin(raw_packet, &idlRegPar.iac_reg_db))
  return false;

 return true;
}

//-----------------------------------------------------------------------
bool CControlApp::Parse_CARBUR_PAR(const BYTE* raw_packet, size_t size)
{
 SECU3IO::CarburPar& carburPar = m_recepted_packet.m_CarburPar;
 if (size != 23)  //размер пакета без сигнального символа, дескриптора и символа-конца пакета
  return false;

 //Нижний порог ЭПХХ (бензин)
 if (false == mp_pdp->Hex16ToBin(raw_packet, &carburPar.ephh_lot))
  return false;

 //Верхний порог ЭПХХ (бензин)
 if (false == mp_pdp->Hex16ToBin(raw_packet, &carburPar.ephh_hit))
  return false;

 //Признак инверсии концевика карбюратора
 if (false == mp_pdp->Hex4ToBin(raw_packet, &carburPar.carb_invers))
  return false;

 //Порог разрежения ЭМР
 int epm_on_threshold = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &epm_on_threshold, true))
  return false;
 carburPar.epm_ont = ((float)epm_on_threshold) / MAP_PHYSICAL_MAGNITUDE_MULTIPLIER;

 //Нижний порог ЭПХХ (газ)
 if (false == mp_pdp->Hex16ToBin(raw_packet, &carburPar.ephh_lot_g))
  return false;

 //Верхний порог ЭПХХ (газ)
 if (false == mp_pdp->Hex16ToBin(raw_packet, &carburPar.ephh_hit_g))
  return false;

 //Задержка выключения клапана ЭПХХ
 unsigned char shutoff_delay;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &shutoff_delay))
  return false;
 carburPar.shutoff_delay = ((float)shutoff_delay) / 100.0f; //переводим в секунды

 //Порог переключения в режим ХХ по ДПДЗ
 unsigned char tps_threshold;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &tps_threshold))
  return false;
 carburPar.tps_threshold = ((float)tps_threshold) / TPS_PHYSICAL_MAGNITUDE_MULTIPLIER;

 int fuelcut_map_thrd=0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &fuelcut_map_thrd))
  return false;
 carburPar.fuelcut_map_thrd = ((float)fuelcut_map_thrd) / MAP_PHYSICAL_MAGNITUDE_MULTIPLIER;

 int fuelcut_cts_thrd=0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &fuelcut_cts_thrd, true))
  return false;
 carburPar.fuelcut_cts_thrd = ((float)fuelcut_cts_thrd) / TEMP_PHYSICAL_MAGNITUDE_MULTIPLIER;

 //Rev.limitting lo threshold
 if (false == mp_pdp->Hex16ToBin(raw_packet, &carburPar.revlim_lot))
  return false;

 //Rev.limitting hi threshold
 if (false == mp_pdp->Hex16ToBin(raw_packet, &carburPar.revlim_hit))
  return false;

 unsigned char fuelcut_uni = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &fuelcut_uni))
  return false;
 carburPar.fuelcut_uni = fuelcut_uni;
 if (carburPar.fuelcut_uni == 0xF)
  carburPar.fuelcut_uni = UNI_OUTPUT_NUM; //disabled

 unsigned char igncut_uni = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &igncut_uni))
  return false;
 carburPar.igncut_uni = igncut_uni;
 if (carburPar.igncut_uni == 0xF)
  carburPar.igncut_uni = UNI_OUTPUT_NUM; //disabled

 return true;
}

//-----------------------------------------------------------------------
bool CControlApp::Parse_TEMPER_PAR(const BYTE* raw_packet, size_t size)
{
 SECU3IO::TemperPar& temperPar = m_recepted_packet.m_TemperPar;
 if (size != 15)  //размер пакета без сигнального символа, дескриптора и символа-конца пакета
  return false;

 //Temperature flags
 unsigned char tmp_flags = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet,&tmp_flags))
  return false;

 //Признак комплектации ДТОЖ (использования ДТОЖ)
 temperPar.tmp_use = CHECKBIT8(tmp_flags, 0);
 //Флаг использования таблицы для задания зависимости температуры от напрящения ДТОЖ
 temperPar.cts_use_map = CHECKBIT8(tmp_flags, 1);
 //Флаг использования ШИМ для управления вентилятором охлаждения двигателя
 temperPar.vent_pwm = CHECKBIT8(tmp_flags, 2);

 //Для удобства и повышения скорости обработки SECU-3 оперирует с температурой представленной
 //дискретами АЦП (как измеренное значение прямо с датчика)

 //Порог включения вентилятора (число со знаком)
 int vent_on = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&vent_on,true))
  return false;
 temperPar.vent_on = ((float)vent_on) / TEMP_PHYSICAL_MAGNITUDE_MULTIPLIER;

 //Порог выключения вентилятора (число со знаком)
 int vent_off = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&vent_off,true))
  return false;
 temperPar.vent_off = ((float)vent_off) / TEMP_PHYSICAL_MAGNITUDE_MULTIPLIER;

 int vent_pwmfrq = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&vent_pwmfrq))
  return false;
 temperPar.vent_pwmfrq = MathHelpers::Round(1.0/(((double)vent_pwmfrq) / 524288.0));

 //conditioner clutch turn on threshold (voltage from the refrigerant pressure sensor)
 int cond_pvt_on = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &cond_pvt_on))
  return false;
 temperPar.cond_pvt_on = ((float)cond_pvt_on) * m_adc_discrete;

 //conditioner clutch turn off threshold (voltage from the refrigerant pressure sensor)
 int cond_pvt_off = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &cond_pvt_off))
  return false;
 temperPar.cond_pvt_off = ((float)cond_pvt_off) * m_adc_discrete;

 //Minimum RPM for turning on of conditioner clutch
 int cond_min_rpm = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &cond_min_rpm))
  return false;
 temperPar.cond_min_rpm = cond_min_rpm;

 //Cooling fan's timer (sec.)
 int vent_tmr = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &vent_tmr))
  return false;
 temperPar.vent_tmr = vent_tmr / 100.0f;

 return true;
}

//-----------------------------------------------------------------------
bool CControlApp::Parse_ADCRAW_DAT(const BYTE* raw_packet, size_t size)
{
 SECU3IO::RawSensDat& rawSensDat = m_recepted_packet.m_RawSensDat;
 if (size != 26)  //размер пакета без сигнального символа, дескриптора и символа-конца пакета
  return false;

 //MAP sensor
 signed int map = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&map,true))
  return false;
 rawSensDat.map_value = map * m_adc_discrete;

 //напряжение бортовой сети
 signed int ubat = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&ubat,true))
  return false;
 rawSensDat.ubat_value = ubat * m_adc_discrete;

 //температура ОЖ (ДТОЖ)
 signed int temp = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&temp,true))
  return false;
 rawSensDat.temp_value = temp * m_adc_discrete;

 //Уровень сигнала детонации
 signed int knock = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&knock,true))
  return false;
 rawSensDat.knock_value = knock * m_adc_discrete;

 //Throttle position sensor
 signed int tps = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&tps,true))
  return false;
 rawSensDat.tps_value = tps * m_adc_discrete;

 //ADD_I1 input
 signed int add_i1 = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&add_i1,true))
  return false;
 rawSensDat.add_i1_value = add_i1 * m_adc_discrete;

 //ADD_I2 input
 signed int add_i2 = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&add_i2,true))
  return false;
 rawSensDat.add_i2_value = add_i2 * m_adc_discrete;

 //ADD_I3 input
 signed int add_i3 = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&add_i3,true))
  return false;
 rawSensDat.add_i3_value = add_i3 * m_adc_discrete;

 //ADD_I4 input
 signed int add_i4 = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&add_i4,true))
  return false;
 rawSensDat.add_i4_value = add_i4 * m_adc_discrete;

 //ADD_I5 input
 signed int add_i5 = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&add_i5,true))
  return false;
 rawSensDat.add_i5_value = add_i5 * m_adc_discrete;

 //ADD_I6 input
 signed int add_i6 = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&add_i6,true))
  return false;
 rawSensDat.add_i6_value = add_i6 * m_adc_discrete;

 //ADD_I7 input
 signed int add_i7 = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&add_i7,true))
  return false;
 rawSensDat.add_i7_value = add_i7 * m_adc_discrete;

 //ADD_I8 input
 signed int add_i8 = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&add_i8,true))
  return false;
 rawSensDat.add_i8_value = add_i8 * m_adc_discrete;

 return true;
}


//-----------------------------------------------------------------------
//note: for more information see AVR120 application note.
bool CControlApp::Parse_ADCCOR_PAR(const BYTE* raw_packet, size_t size)
{
 SECU3IO::ADCCompenPar& adcCompenPar = m_recepted_packet.m_ADCCompenPar;
 if (size != 73)  //размер пакета без сигнального символа, дескриптора и символа-конца пакета
  return false;

 //ADC flags
 unsigned char flags = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &flags))
  return false;
 adcCompenPar.adc_comp_mode = CHECKBIT8(flags, 0);

 signed int map_adc_factor = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&map_adc_factor))
  return false;
 adcCompenPar.map_adc_factor = ((float)map_adc_factor) / 16384;

 signed long map_adc_correction = 0;
 if (false == mp_pdp->Hex32ToBin(raw_packet,&map_adc_correction))
  return false;
 adcCompenPar.map_adc_correction = ((((float)map_adc_correction)/16384.0f) - 0.5f) / (adcCompenPar.adc_comp_mode ? adcCompenPar.map_adc_factor : 1.0f);
 adcCompenPar.map_adc_correction*=m_adc_discrete; //в вольты

 signed int ubat_adc_factor = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&ubat_adc_factor))
  return false;
 adcCompenPar.ubat_adc_factor = ((float)ubat_adc_factor) / 16384;

 signed long ubat_adc_correction = 0;
 if (false == mp_pdp->Hex32ToBin(raw_packet,&ubat_adc_correction))
  return false;
 adcCompenPar.ubat_adc_correction = ((((float)ubat_adc_correction)/16384.0f) - 0.5f) / (adcCompenPar.adc_comp_mode ? adcCompenPar.ubat_adc_factor : 1.0f);
 adcCompenPar.ubat_adc_correction*=m_adc_discrete; //в вольты

 signed int temp_adc_factor = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&temp_adc_factor))
  return false;
 adcCompenPar.temp_adc_factor = ((float)temp_adc_factor) / 16384;

 signed long temp_adc_correction = 0;
 if (false == mp_pdp->Hex32ToBin(raw_packet,&temp_adc_correction))
  return false;
 adcCompenPar.temp_adc_correction = ((((float)temp_adc_correction)/16384.0f) - 0.5f) / (adcCompenPar.adc_comp_mode ? adcCompenPar.temp_adc_factor : 1.0f);
 adcCompenPar.temp_adc_correction*=m_adc_discrete; //в вольты
 
 //TPS sensor
 signed int tps_adc_factor = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&tps_adc_factor))
  return false;
 adcCompenPar.tps_adc_factor = ((float)tps_adc_factor) / 16384;

 signed long tps_adc_correction = 0;
 if (false == mp_pdp->Hex32ToBin(raw_packet,&tps_adc_correction))
  return false;
 adcCompenPar.tps_adc_correction = ((((float)tps_adc_correction)/16384.0f) - 0.5f) / (adcCompenPar.adc_comp_mode ? adcCompenPar.tps_adc_factor : 1.0f);
 adcCompenPar.tps_adc_correction*=m_adc_discrete; //в вольты

 //ADD_I1 input
 signed int ai1_adc_factor = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&ai1_adc_factor))
  return false;
 adcCompenPar.ai1_adc_factor = ((float)ai1_adc_factor) / 16384;

 signed long ai1_adc_correction = 0;
 if (false == mp_pdp->Hex32ToBin(raw_packet,&ai1_adc_correction))
  return false;
 adcCompenPar.ai1_adc_correction = ((((float)ai1_adc_correction)/16384.0f) - 0.5f) / (adcCompenPar.adc_comp_mode ? adcCompenPar.ai1_adc_factor : 1.0f);
 adcCompenPar.ai1_adc_correction*=m_adc_discrete; //в вольты

 //ADD_I2 input
 signed int ai2_adc_factor = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&ai2_adc_factor))
  return false;
 adcCompenPar.ai2_adc_factor = ((float)ai2_adc_factor) / 16384;

 signed long ai2_adc_correction = 0;
 if (false == mp_pdp->Hex32ToBin(raw_packet,&ai2_adc_correction))
  return false;
 adcCompenPar.ai2_adc_correction = ((((float)ai2_adc_correction)/16384.0f) - 0.5f) / (adcCompenPar.adc_comp_mode ? adcCompenPar.ai2_adc_factor : 1.0f);
 adcCompenPar.ai2_adc_correction*=m_adc_discrete; //в вольты

 //ADD_I3 input
 signed int ai3_adc_factor = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&ai3_adc_factor))
  return false;
 adcCompenPar.ai3_adc_factor = ((float)ai3_adc_factor) / 16384;

 signed long ai3_adc_correction = 0;
 if (false == mp_pdp->Hex32ToBin(raw_packet,&ai3_adc_correction))
  return false;
 adcCompenPar.ai3_adc_correction = ((((float)ai3_adc_correction)/16384.0f) - 0.5f) / (adcCompenPar.adc_comp_mode ? adcCompenPar.ai3_adc_factor : 1.0f);
 adcCompenPar.ai3_adc_correction*=m_adc_discrete; //в вольты

 //ADD_I4 input
 signed int ai4_adc_factor = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&ai4_adc_factor))
  return false;
 adcCompenPar.ai4_adc_factor = ((float)ai4_adc_factor) / 16384;

 signed long ai4_adc_correction = 0;
 if (false == mp_pdp->Hex32ToBin(raw_packet,&ai4_adc_correction))
  return false;
 adcCompenPar.ai4_adc_correction = ((((float)ai4_adc_correction)/16384.0f) - 0.5f) / (adcCompenPar.adc_comp_mode ? adcCompenPar.ai4_adc_factor : 1.0f);
 adcCompenPar.ai4_adc_correction*=m_adc_discrete; //в вольты

 //ADD_I5 input
 signed int ai5_adc_factor = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&ai5_adc_factor))
  return false;
 adcCompenPar.ai5_adc_factor = ((float)ai5_adc_factor) / 16384;

 signed long ai5_adc_correction = 0;
 if (false == mp_pdp->Hex32ToBin(raw_packet,&ai5_adc_correction))
  return false;
 adcCompenPar.ai5_adc_correction = ((((float)ai5_adc_correction)/16384.0f) - 0.5f) / (adcCompenPar.adc_comp_mode ? adcCompenPar.ai5_adc_factor : 1.0f);
 adcCompenPar.ai5_adc_correction*=m_adc_discrete; //в вольты

 //ADD_I6 input
 signed int ai6_adc_factor = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &ai6_adc_factor))
  return false;
 adcCompenPar.ai6_adc_factor = ((float)ai6_adc_factor) / 16384;

 signed long ai6_adc_correction = 0;
 if (false == mp_pdp->Hex32ToBin(raw_packet,&ai6_adc_correction))
  return false;
 adcCompenPar.ai6_adc_correction = ((((float)ai6_adc_correction)/16384.0f) - 0.5f) / (adcCompenPar.adc_comp_mode ? adcCompenPar.ai6_adc_factor : 1.0f);
 adcCompenPar.ai6_adc_correction*=m_adc_discrete; //в вольты

 //ADD_I7 input
 signed int ai7_adc_factor = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&ai7_adc_factor))
  return false;
 adcCompenPar.ai7_adc_factor = ((float)ai7_adc_factor) / 16384;

 signed long ai7_adc_correction = 0;
 if (false == mp_pdp->Hex32ToBin(raw_packet,&ai7_adc_correction))
  return false;
 adcCompenPar.ai7_adc_correction = ((((float)ai7_adc_correction)/16384.0f) - 0.5f) / (adcCompenPar.adc_comp_mode ? adcCompenPar.ai7_adc_factor : 1.0f);
 adcCompenPar.ai7_adc_correction*=m_adc_discrete; //в вольты

 //ADD_I4 input
 signed int ai8_adc_factor = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&ai8_adc_factor))
  return false;
 adcCompenPar.ai8_adc_factor = ((float)ai8_adc_factor) / 16384;

 signed long ai8_adc_correction = 0;
 if (false == mp_pdp->Hex32ToBin(raw_packet,&ai8_adc_correction))
  return false;
 adcCompenPar.ai8_adc_correction = ((((float)ai8_adc_correction)/16384.0f) - 0.5f) / (adcCompenPar.adc_comp_mode ? adcCompenPar.ai8_adc_factor : 1.0f);
 adcCompenPar.ai8_adc_correction*=m_adc_discrete; //в вольты

 return true;
}

//-----------------------------------------------------------------------
bool CControlApp::Parse_CKPS_PAR(const BYTE* raw_packet, size_t size)
{
 SECU3IO::CKPSPar& ckpsPar = m_recepted_packet.m_CKPSPar;
 if (size != 10)  //размер пакета без сигнального символа, дескриптора и символа-конца пакета
  return false;

 //Количество зубьев до в.м.т.
 if (false == mp_pdp->Hex8ToBin(raw_packet,&ckpsPar.ckps_cogs_btdc))
  return false;

 //Длительность импульса запуска коммутаторов в зубьях шкива
 if (false == mp_pdp->Hex8ToBin(raw_packet,&ckpsPar.ckps_ignit_cogs))
  return false;

 //Кол-во цилиндров двигателя
 if (false == mp_pdp->Hex8ToBin(raw_packet,&ckpsPar.ckps_engine_cyl))
  return false;

 //Кол-во зубьев задающего шкива, включая пропущенные
 if (false == mp_pdp->Hex8ToBin(raw_packet, &ckpsPar.ckps_cogs_num))
  return false;

 //Кол-во пропущенных зубьев задающего шкива (допустимые значения: 0, 1, 2)
 if (false == mp_pdp->Hex8ToBin(raw_packet, &ckpsPar.ckps_miss_num))
  return false;

 //Hall sensor flags
 unsigned char flags = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &flags))
  return false;
 ckpsPar.ckps_rising_spark = CHECKBIT8(flags, 0);
 ckpsPar.ckps_use_cam_ref = CHECKBIT8(flags, 1);
 ckpsPar.ckps_edge_type = CHECKBIT8(flags, 2);
 ckpsPar.ref_s_edge_type = CHECKBIT8(flags, 3);
 ckpsPar.ckps_merge_ign_outs = CHECKBIT8(flags, 4);

 //Hall shutter's window width
 int wnd_width;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&wnd_width,true))
  return false;
 ckpsPar.hall_wnd_width = ((float)wnd_width) / m_angle_multiplier;

 //Hall degrees BTDC
 int degrees_btdc;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&degrees_btdc,true))
  return false;
 ckpsPar.hall_degrees_btdc = ((float)degrees_btdc) / m_angle_multiplier;

 return true;
}

//-----------------------------------------------------------------------
bool CControlApp::Parse_OP_COMP_NC(const BYTE* raw_packet, size_t size)
{
 SECU3IO::OPCompNc& opCompNc = m_recepted_packet.m_OPCompNc;
 if (size != 2)  //размер пакета без сигнального символа, дескриптора и символа-конца пакета
  return false;

 //Код завершенной операции
 if (false == mp_pdp->Hex8ToBin(raw_packet, &opCompNc.opdata))
  return false;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &opCompNc.opcode))
  return false;

 return true;
}

//-----------------------------------------------------------------------
bool CControlApp::Parse_KNOCK_PAR(const BYTE* raw_packet, size_t size)
{
 SECU3IO::KnockPar& knockPar = m_recepted_packet.m_KnockPar;
 if (size != 19)  //размер пакета без сигнального символа, дескриптора и символа-конца пакета
  return false;

 //Разрешен/запрещен
 if (false == mp_pdp->Hex4ToBin(raw_packet,&knockPar.knock_use_knock_channel))
  return false;

 //Частота ПФ
 unsigned char knock_bpf_frequency;
 if (false == mp_pdp->Hex8ToBin(raw_packet,&knock_bpf_frequency))
  return false;
 knockPar.knock_bpf_frequency = knock_bpf_frequency;

 //Начало фазового окна
 int  knock_k_wnd_begin_angle;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&knock_k_wnd_begin_angle,true))
  return false;
 knockPar.knock_k_wnd_begin_angle = ((float)knock_k_wnd_begin_angle) / m_angle_multiplier;

 //Конец фазового окна
 int  knock_k_wnd_end_angle;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&knock_k_wnd_end_angle,true))
  return false;
 knockPar.knock_k_wnd_end_angle = ((float)knock_k_wnd_end_angle) / m_angle_multiplier;

 //Постоянная времени интегрирования
 unsigned char knock_int_time_const;
 if (false == mp_pdp->Hex8ToBin(raw_packet,&knock_int_time_const))
  return false;
 knockPar.knock_int_time_const = knock_int_time_const;

 //-----------------
 //Шаг смещения УОЗ при детонации
 int knock_retard_step;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&knock_retard_step,true))
  return false;
 knockPar.knock_retard_step = ((float)knock_retard_step) / m_angle_multiplier;

 //Шаг восстановления УОЗ
 int knock_advance_step;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&knock_advance_step,true))
  return false;
 knockPar.knock_advance_step = ((float)knock_advance_step) / m_angle_multiplier;

 //Максимальное смещение УОЗ
 int knock_max_retard;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&knock_max_retard,true))
  return false;
 knockPar.knock_max_retard = ((float)knock_max_retard) / m_angle_multiplier;

 //Порог детонации
 int knock_threshold;
 if (false == mp_pdp->Hex16ToBin(raw_packet,&knock_threshold, false))
  return false;
 knockPar.knock_threshold = ((float)knock_threshold) * m_adc_discrete;

 //Задержка восстановления УОЗ
 unsigned char knock_recovery_delay;
 if (false == mp_pdp->Hex8ToBin(raw_packet,&knock_recovery_delay))
  return false;
 knockPar.knock_recovery_delay = knock_recovery_delay;

 //selection of KS for each channel (cylinder)
 unsigned char knock_selch;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &knock_selch))
  return false;
 for (int i = 0; i < 8; ++i)
  knockPar.knock_selch[i] = CHECKBIT8(knock_selch, i);

 //Knock detection CLT threshold
 int knkclt_thrd = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &knkclt_thrd, true))
  return false;
 knockPar.knkclt_thrd = ((float)knkclt_thrd) / TEMP_PHYSICAL_MAGNITUDE_MULTIPLIER;

 return true;
}

//-----------------------------------------------------------------------
bool CControlApp::Parse_CE_ERR_CODES(const BYTE* raw_packet, size_t size)
{
 SECU3IO::CEErrors& ceErrors = m_recepted_packet.m_CEErrors;
 if (size != 4)  //размер пакета без сигнального символа, дескриптора и символа-конца пакета
  return false;

 unsigned long flags = 0;
 if (false == mp_pdp->Hex32ToBin(raw_packet, &flags))
  return false;
 ceErrors.flags = flags;

 return true;
}

//-----------------------------------------------------------------------
bool CControlApp::Parse_CE_SAVED_ERR(const BYTE* raw_packet, size_t size)
{
 SECU3IO::CEErrors& ceErrors = m_recepted_packet.m_CEErrors;
 if (size != 4)  //размер пакета без сигнального символа, дескриптора и символа-конца пакета
  return false;

 unsigned long flags = 0;
 if (false == mp_pdp->Hex32ToBin(raw_packet, &flags))
  return false;
 ceErrors.flags = flags;

 return true;
}

//-----------------------------------------------------------------------
bool CControlApp::Parse_FWINFO_DAT(const BYTE* raw_packet, size_t size)
{
 SECU3IO::FWInfoDat& fwInfoDat = m_recepted_packet.m_FWInfoDat;
 if (size != (FW_SIGNATURE_INFO_SIZE + 5))  //размер пакета без сигнального символа, дескриптора и символа-конца пакета
  return false;

 //строка с информацией 
 strncpy(fwInfoDat.info,(const char*)raw_packet, FW_SIGNATURE_INFO_SIZE);
 fwInfoDat.info[FW_SIGNATURE_INFO_SIZE] = 0;
 raw_packet+=FW_SIGNATURE_INFO_SIZE;

 //биты опций прошивки
 DWORD options = 0;
 if (false == mp_pdp->Hex32ToBin(raw_packet,&options))
  return false;
 fwInfoDat.options = options;

 unsigned char fw_version;
 if (false == mp_pdp->Hex8ToBin(raw_packet,&fw_version))
  return false;
 fwInfoDat.fw_version[0] = fw_version & 0x0F;
 fwInfoDat.fw_version[1] = fw_version >> 4;

 return true;
}

//-----------------------------------------------------------------------
bool CControlApp::Parse_MISCEL_PAR(const BYTE* raw_packet, size_t size)
{
 SECU3IO::MiscelPar& miscPar = m_recepted_packet.m_MiscelPar;
 if (size != 20)  //размер пакета без сигнального символа, дескриптора и символа-конца пакета
  return false;

 //Делитель для UART-а
 int divisor = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &divisor))
  return false;

 miscPar.baud_rate = 0;

 for(size_t i = 0; i < SECU3IO::SECU3_ALLOWABLE_UART_DIVISORS_COUNT; ++i)
  if (SECU3IO::secu3_allowable_uart_divisors[i].second == divisor)
    miscPar.baud_rate = SECU3IO::secu3_allowable_uart_divisors[i].first;

 ASSERT(miscPar.baud_rate);

 //Период посылки пакетов в десятках миллисекунд
 unsigned char period_t_ms = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet,&period_t_ms))
  return false;

 miscPar.period_ms = period_t_ms * 10;

 //Отсечка разрешена/запрещена
 if (false == mp_pdp->Hex4ToBin(raw_packet, &miscPar.ign_cutoff))
  return false;

 //Обороты отсечки
 if (false == mp_pdp->Hex16ToBin(raw_packet, &miscPar.ign_cutoff_thrd, true))
  return false;

 //Hall out: start of pulse in degrees BTDC
 signed int start = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &start, true))
  return false;

 miscPar.hop_start_ang = ((float)start) / ANGLE_MULTIPLIER;

 //Hall out: width of pulse in degrees
 int duration = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &duration))
  return false;
 miscPar.hop_durat_ang = ((float)duration) / ANGLE_MULTIPLIER;

 //Fuel pump flags
 unsigned char flpmp_flags = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &flpmp_flags))
  return false;
 miscPar.flpmp_offongas = CHECKBIT8(flpmp_flags, 0);
 miscPar.inj_offongas = CHECKBIT8(flpmp_flags, 1);
 miscPar.inj_offonpet = CHECKBIT8(flpmp_flags, 2);

 //Начальный порог расхода воздуха для EVAP
 int evap_afbegin = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &evap_afbegin))
  return false;
 miscPar.evap_afbegin = ((float)evap_afbegin) * 32.0f;

 //Наклон прямой расхода воздуха для EVAP
 int evap_afslope = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &evap_afslope))
  return false;
 miscPar.evap_afslope = (((float)evap_afslope) / 1048576.0f) / 32.0f; //2^20

 //Fuel pump timeout for cranking
 BYTE fp_timeout_strt = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &fp_timeout_strt))
  return false;
 miscPar.fp_timeout_strt = (((float)fp_timeout_strt) / 10.0f);

 int pwmfrq1 = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &pwmfrq1))
  return false;
 if (pwmfrq1==0)
  miscPar.pwm2_pwmfrq[0] = 5000; //prevent division by zero
 else
  miscPar.pwm2_pwmfrq[0] = MathHelpers::Round(1.0/(((double)pwmfrq1) / 524288.0));

 int pwmfrq2 = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &pwmfrq2))
  return false;
 if (pwmfrq2 == 0)
  miscPar.pwm2_pwmfrq[1] = 5000; //prevent division by zero
 else
  miscPar.pwm2_pwmfrq[1] = MathHelpers::Round(1.0/(((double)pwmfrq2) / 524288.0));

 return true;
}

//-----------------------------------------------------------------------
bool CControlApp::Parse_EDITAB_PAR(const BYTE* raw_packet, size_t size)
{
 SECU3IO::EditTabPar& editTabPar = m_recepted_packet.m_EditTabPar;
 //check for 16 and 24 byte packets
 if (size < 3 || size > 26)  //размер пакета без сигнального символа, дескриптора и символа-конца пакета
  return false;

 //код таблицы в наборе
 if (false == mp_pdp->Hex8ToBin(raw_packet, &editTabPar.tab_id))
  return false;

 if (editTabPar.tab_id != ETMT_STRT_MAP && editTabPar.tab_id != ETMT_IDLE_MAP && editTabPar.tab_id != ETMT_WORK_MAP &&
     editTabPar.tab_id != ETMT_TEMP_MAP && editTabPar.tab_id != ETMT_NAME_STR && editTabPar.tab_id != ETMT_VE_MAP &&
     editTabPar.tab_id != ETMT_AFR_MAP && editTabPar.tab_id != ETMT_CRNK_MAP && editTabPar.tab_id != ETMT_WRMP_MAP &&
     editTabPar.tab_id != ETMT_DEAD_MAP && editTabPar.tab_id != ETMT_IDLR_MAP && editTabPar.tab_id != ETMT_IDLC_MAP &&
     editTabPar.tab_id != ETMT_AETPS_MAP && editTabPar.tab_id != ETMT_AERPM_MAP && editTabPar.tab_id != ETMT_AFTSTR_MAP &&
     editTabPar.tab_id != ETMT_IT_MAP && editTabPar.tab_id != ETMT_ITRPM_MAP && editTabPar.tab_id != ETMT_RIGID_MAP &&
     editTabPar.tab_id != ETMT_EGOCRV_MAP && editTabPar.tab_id != ETMT_IACC_MAP && editTabPar.tab_id != ETMT_IACCW_MAP &&
     editTabPar.tab_id != ETMT_IATCLT_MAP && editTabPar.tab_id != ETMT_TPSSWT_MAP && editTabPar.tab_id != ETMT_GTSC_MAP &&
     editTabPar.tab_id != ETMT_GPSC_MAP && editTabPar.tab_id != ETMT_ATSC_MAP && editTabPar.tab_id != ETMT_PWM1_MAP &&
     editTabPar.tab_id != ETMT_PWM2_MAP && editTabPar.tab_id != ETMT_TEMPI_MAP && editTabPar.tab_id != ETMT_IACMAT_MAP && 
     editTabPar.tab_id != ETMT_VE2_MAP && editTabPar.tab_id != ETMT_TPSZON_MAP && editTabPar.tab_id != ETMT_CYLMULT_MAP && 
     editTabPar.tab_id != ETMT_CYLADD_MAP && editTabPar.tab_id != ETMT_AEMAP_MAP)
  return false;

 //check for 16-byte packets
 if ((editTabPar.tab_id != ETMT_GPSC_MAP) && (editTabPar.tab_id != ETMT_VE_MAP) && (editTabPar.tab_id != ETMT_VE2_MAP) && (editTabPar.tab_id != ETMT_IT_MAP) && (size < 3 || size > 18)) 
  return false;

 //address of fragment of data in table (offset in table)
 unsigned char address;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &address))
  return false;
 editTabPar.address = address;

 size-=2;

 if (editTabPar.tab_id != ETMT_NAME_STR)
 {
  size_t data_size = 0;
  float discrete = (m_quartz_frq == 20000000 ? 3.2f : 4.0f);
  if (editTabPar.tab_id == ETMT_CRNK_MAP || editTabPar.tab_id == ETMT_DEAD_MAP || editTabPar.tab_id == ETMT_RIGID_MAP || editTabPar.tab_id == ETMT_EGOCRV_MAP || editTabPar.tab_id == ETMT_IACC_MAP || editTabPar.tab_id == ETMT_IATCLT_MAP)
  {
   size_t div = 2;
   if (size % div)
    return false;

   //fragment with data (float)
   for(size_t i = 0; i < size / div; ++i)
   {
    int value;
    if (false == mp_pdp->Hex16ToBin(raw_packet, &value, editTabPar.tab_id == ETMT_DEAD_MAP)) //<--signed for dead map
     return false;

    if (editTabPar.tab_id == ETMT_IATCLT_MAP)
     editTabPar.table_data[i] = (address >= INJ_IATCLT_CORR_SIZE) ? (((float)value) * 32.0f) : (((float)value) / 8192.0f);
    else if (editTabPar.tab_id == ETMT_IACC_MAP)
     editTabPar.table_data[i] = (address >= INJ_IAC_CORR_SIZE) ? (((float)value) / 128.0f) : (((float)(((signed short)value) + 8192)) / 8192.0f);
    else if (editTabPar.tab_id == ETMT_RIGID_MAP)
     editTabPar.table_data[i] = ((float)value) / 128.0f;  //convert to user readble value
    else if (editTabPar.tab_id == ETMT_EGOCRV_MAP)
     editTabPar.table_data[i] = (address > 15) ? (value * ADC_DISCRETE) : (((float)value) / EGO_CURVE_M_FACTOR);
    else
     editTabPar.table_data[i] = (((float)value) * discrete) / 1000.0f;  //convert to ms
    ++data_size;
   }
  }
  else
  {
   if (editTabPar.tab_id == ETMT_VE_MAP || editTabPar.tab_id == ETMT_VE2_MAP) //volumetric efficiency maps
   { //VE
    BYTE buff[32]; unsigned char value;
    for(size_t i = 0; i < size; ++i)
    {     
     if (false == mp_pdp->Hex8ToBin(raw_packet, &value))
      return false;
     buff[i] = value;
    }

    if (size % 3) //must contain whole number of cells
     return false;

    size_t numOfCells = (size / 3) * 2;
    for(size_t i = 0; i < numOfCells; ++i)
     editTabPar.table_data[i] = ((float)w12GetCell(buff, i)) / VE_MAPS_M_FACTOR;
    data_size = numOfCells;
   }
   else if (editTabPar.tab_id == ETMT_IT_MAP) //injection timing map
   {
    BYTE buff[32]; unsigned char value;
    for(size_t i = 0; i < size; ++i)
    {     
     if (false == mp_pdp->Hex8ToBin(raw_packet, &value))
      return false;
     buff[i] = value;
    }

    if (size % 3) //must contain whole number of cells
     return false;

    size_t numOfCells = (size / 3) * 2;
    for(size_t i = 0; i < numOfCells; ++i)
     editTabPar.table_data[i] = ((float)w12GetCell(buff, i)) / IT_MAPS_M_FACTOR;
    data_size = numOfCells;
   }
   else
   {
    //data fragment (float)
    for(size_t i = 0; i < size; ++i)
    {
     unsigned char value;
     if (false == mp_pdp->Hex8ToBin(raw_packet, &value))
      return false;
     else if (editTabPar.tab_id == ETMT_AFR_MAP)
      editTabPar.table_data[i] = MathHelpers::RoundP1((((float)value) / AFR_MAPS_M_FACTOR) + 8.0f);
     else if (editTabPar.tab_id == ETMT_PWM1_MAP)
      editTabPar.table_data[i] = m_splitAng ? ((float)((signed char)value)) / AA_MAPS_M_FACTOR : ((float)value * 100) / 255.0f;
     else if (editTabPar.tab_id == ETMT_PWM2_MAP)
      editTabPar.table_data[i] = ((float)value * 100) / 255.0f;
     else if (editTabPar.tab_id == ETMT_WRMP_MAP)
      editTabPar.table_data[i] = ((float)value) / WRMP_MAPS_M_FACTOR;
     else if (editTabPar.tab_id == ETMT_AFTSTR_MAP)
      editTabPar.table_data[i] = ((float)value) / AFTSTR_MAPS_M_FACTOR;
     else if (editTabPar.tab_id == ETMT_IDLR_MAP || editTabPar.tab_id == ETMT_IDLC_MAP || editTabPar.tab_id == ETMT_THRASS_MAP)
      editTabPar.table_data[i] = ((float)value) / IACPOS_MAPS_M_FACTOR;
     else if (editTabPar.tab_id == ETMT_AETPS_MAP)
      editTabPar.table_data[i] = (i >= INJ_AE_TPS_LOOKUP_TABLE_SIZE)?(float((signed char)value)/AETPSB_MAPS_M_FACTOR):(float(value)-AETPSV_MAPS_ADDER);
     else if (editTabPar.tab_id == ETMT_AERPM_MAP)
      editTabPar.table_data[i] = ((float)value) / ((i >= INJ_AE_RPM_LOOKUP_TABLE_SIZE)?AERPMB_MAPS_M_FACTOR:AERPMV_MAPS_M_FACTOR);
     else if (editTabPar.tab_id == ETMT_ITRPM_MAP)
      editTabPar.table_data[i] = ((float)((unsigned char)value)) * 10.0f;
     else if (editTabPar.tab_id == ETMT_IACCW_MAP)
      editTabPar.table_data[i] =  (address >= INJ_IAC_CORR_W_SIZE) ? (((float)value) / 2.0f) : (((float)value) / 256.0f);
     else if (editTabPar.tab_id == ETMT_TPSSWT_MAP)
      editTabPar.table_data[i] = ((float)value) / TPSSWT_MAPS_M_FACTOR;
     else if (editTabPar.tab_id == ETMT_TPSZON_MAP)
      editTabPar.table_data[i] = ((float)value) / TPSZON_MAPS_M_FACTOR;
     else if (editTabPar.tab_id == ETMT_GTSC_MAP)
      editTabPar.table_data[i] = ((float)value) / 128.0f;
     else if (editTabPar.tab_id == ETMT_GPSC_MAP)
      editTabPar.table_data[i] =  (i >= INJ_GPS_CORR_SIZE) ? (((float)value) * 2.0f) : (((float)value) / 128.0f);
     else if (editTabPar.tab_id == ETMT_ATSC_MAP)
      editTabPar.table_data[i] = ((float)value) / 128.0f;
     else if (editTabPar.tab_id == ETMT_IACMAT_MAP)
      editTabPar.table_data[i] = ((float)((signed char)value)) / 4.0f;
     else if (editTabPar.tab_id == ETMT_CYLMULT_MAP)
      editTabPar.table_data[i] = (((float)(value)) / 256.0f) + 0.5f;
     else if (editTabPar.tab_id == ETMT_CYLADD_MAP)
      editTabPar.table_data[i] = ((float)((signed char)value)) * 0.0256f;
     else if (editTabPar.tab_id == ETMT_AEMAP_MAP)
      editTabPar.table_data[i] = (i >= INJ_AE_MAP_LOOKUP_TABLE_SIZE)?(float((signed char)value)/AEMAPB_MAPS_M_FACTOR):(float(value)-AEMAPV_MAPS_ADDER);
     else
      editTabPar.table_data[i] = ((float)((signed char)value)) / AA_MAPS_M_FACTOR;
     ++data_size;
    }
   }
  }
  editTabPar.data_size = data_size;
 }
 else
 {
  //фрагмент с данными (текстовая информация)  
  std::string raw_string((char*)raw_packet, size);
  OemToChar(raw_string.c_str(), editTabPar.name_data);
  editTabPar.data_size = raw_string.size();
 }

 return true;
}

//-----------------------------------------------------------------------
bool CControlApp::Parse_ATTTAB_PAR(const BYTE* raw_packet, size_t size)
{
 SECU3IO::SepTabPar& attTabPar = m_recepted_packet.m_SepTabPar;
 if (size < 2 || size > 17)  //размер пакета без сигнального символа, дескриптора и символа-конца пакета
  return false;

 //адрес фрагмента данных в таблице (смещение в таблице)
 unsigned char address;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &address))
  return false;
 attTabPar.address = address;

 size-=1;

 //fragment with data (codes of coefficients of attenuation)
 size_t data_size = 0;
 for(size_t i = 0; i < size; ++i)
 {
  unsigned char value;
  if (false == mp_pdp->Hex8ToBin(raw_packet, &value))
   return false;
  attTabPar.table_data[i] = value;
  ++data_size;
 }
 attTabPar.data_size = data_size;

 return true;
}

//-----------------------------------------------------------------------
bool CControlApp::Parse_LTFT_DAT(const BYTE* raw_packet, size_t size)
{
 SECU3IO::SepTabPar& ltftTabPar = m_recepted_packet.m_SepTabPar;
 if (size < 2 || size > 18)  //размер пакета без сигнального символа, дескриптора и символа-конца пакета
  return false;

 unsigned char reserv;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &reserv))
  return false;
 ltftTabPar.reserv = reserv;

 //адрес фрагмента данных в таблице (смещение в таблице)
 unsigned char address;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &address))
  return false;
 ltftTabPar.address = address;

 size-=2;

 //data fragment
 size_t data_size = 0;
 for(size_t i = 0; i < size; ++i)
 {
  int value = 0;
  if (false == mp_pdp->Hex8ToBin(raw_packet, &value))
   return false;
  ltftTabPar.table_data[i] = (((float)value) / 512.0f) * 100.0f;
  ++data_size;
 }
 ltftTabPar.data_size = data_size;

 return true;
}

//-----------------------------------------------------------------------
bool CControlApp::Parse_RPMGRD_PAR(const BYTE* raw_packet, size_t size)
{
 SECU3IO::SepTabPar& rpmGrdPar = m_recepted_packet.m_SepTabPar;
 if (size != 33)  //размер пакета без сигнального символа, дескриптора и символа-конца пакета
  return false;

 //адрес фрагмента данных в таблице (смещение в таблице)
 unsigned char address;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &address))
  return false;
 rpmGrdPar.address = address;
 if (0!=address)
  return false;  //address must be always zero

 size-=1;
 size_t div = 2;
 if (size % div)
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

  rpmGrdPar.table_data[i] = MAKEWORD(lo_byte, hi_byte);
  ++data_size;
 }
 rpmGrdPar.data_size = data_size;

 return true;
}

//-----------------------------------------------------------------------
bool CControlApp::Parse_CLTGRD_PAR(const BYTE* raw_packet, size_t size)
{
 SECU3IO::SepTabPar& cltGrdPar = m_recepted_packet.m_SepTabPar;
 if (size != 33)  //размер пакета без сигнального символа, дескриптора и символа-конца пакета
  return false;

 //address of fragment of data in table (offset in table)
 unsigned char address;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &address))
  return false;
 cltGrdPar.address = address;
 if (0!=address)
  return false;  //address must be always zero

 size-=1;
 size_t div = 2;
 if (size % div)
  return false;

 //fragment with data (CLT grid)
 size_t data_size = 0;
 for(size_t i = 0; i < size / div; ++i)
 {
  unsigned char lo_byte, hi_byte;
  if (false == mp_pdp->Hex8ToBin(raw_packet, &lo_byte))
   return false;
  if (false == mp_pdp->Hex8ToBin(raw_packet, &hi_byte))
   return false;

  cltGrdPar.table_data[i] = ((float)((signed short)MAKEWORD(lo_byte, hi_byte))) / TEMP_PHYSICAL_MAGNITUDE_MULTIPLIER;
  ++data_size;
 }
 cltGrdPar.data_size = data_size;

 return true;
}

//-----------------------------------------------------------------------
bool CControlApp::Parse_LODGRD_PAR(const BYTE* raw_packet, size_t size)
{
 SECU3IO::SepTabPar& lodGrdPar = m_recepted_packet.m_SepTabPar;
 if (size != 33)  //размер пакета без сигнального символа, дескриптора и символа-конца пакета
  return false;

 //адрес фрагмента данных в таблице (смещение в таблице)
 unsigned char address;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &address))
  return false;
 lodGrdPar.address = address;
 if (0!=address)
  return false;  //address must be always zero

 size-=1;
 size_t div = 2;
 if (size % div)
  return false;

 //load grid data
 size_t data_size = 0;
 for(size_t i = 0; i < size / div; ++i)
 {
  unsigned char lo_byte, hi_byte;
  if (false == mp_pdp->Hex8ToBin(raw_packet, &lo_byte))
   return false;
  if (false == mp_pdp->Hex8ToBin(raw_packet, &hi_byte))
   return false;

  lodGrdPar.table_data[i] = ((float)MAKEWORD(lo_byte, hi_byte)) / LOAD_PHYSICAL_MAGNITUDE_MULTIPLIER;
  ++data_size;
 }
 lodGrdPar.data_size = data_size;

 return true;
}

//-----------------------------------------------------------------------
bool CControlApp::Parse_DIAGINP_DAT(const BYTE* raw_packet, size_t size)
{
 SECU3IO::DiagInpDat& diagInpDat = m_recepted_packet.m_DiagInpDat;
 if (size != 31)  //размер пакета без сигнального символа, дескриптора и символа-конца пакета
  return false;

 //flags variable
 BYTE flags = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &flags))
  return false;
 diagInpDat.f_secu3t = flags;

 //напряжение бортовой сети
 int voltage = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &voltage))
  return false;
 diagInpDat.voltage = ((float)voltage) * m_adc_discrete;

 //датчик абсолютного давления
 int map = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &map))
  return false;
 diagInpDat.map = ((float)map) * m_adc_discrete;

 //датчик температуры охлаждающей жидкости
 int temp = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &temp))
  return false;
 diagInpDat.temp = ((float)temp) * m_adc_discrete;

 //дополнительный I1
 int add_i1 = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &add_i1))
  return false;
 diagInpDat.add_i1 = ((float)add_i1) * m_adc_discrete;

 //дополнительный I2
 int add_i2 = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &add_i2))
  return false;
 diagInpDat.add_i2 = ((float)add_i2) * m_adc_discrete;

 //дополнительный I3 (SECU-3i)
 int add_i3 = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &add_i3))
  return false;
 diagInpDat.add_i3 = ((float)add_i3) * m_adc_discrete;

 //дополнительный I4 (SECU-3i, reserved)
 int add_i4 = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &add_i4))
  return false;
 diagInpDat.add_i4 = ((float)add_i4) * m_adc_discrete;


 //дополнительный I5 (SECU-3i & MCP3204)
 int add_i5 = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &add_i5))
  return false;
 diagInpDat.add_i5 = ((float)add_i5) * m_adc_discrete;

 //дополнительный I6 (SECU-3i & MCP3204)
 int add_i6 = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &add_i6))
  return false;
 diagInpDat.add_i6 = ((float)add_i6) * m_adc_discrete;

 //дополнительный I7 (SECU-3i & MCP3204)
 int add_i7 = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &add_i7))
  return false;
 diagInpDat.add_i7 = ((float)add_i7) * m_adc_discrete;

 //дополнительный I8 (SECU-3i & MCP3204)
 int add_i8 = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &add_i8))
  return false;
 diagInpDat.add_i8 = ((float)add_i8) * m_adc_discrete;

 //датчик положения дроссельной заслонки (концевик карбюратора)
 int carb = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &carb))
  return false;
 diagInpDat.carb = ((float)carb) * m_adc_discrete;

 //Датчик детонации 1
 int ks_1 = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &ks_1))
  return false;
 diagInpDat.ks_1 = ((float)ks_1) * m_adc_discrete;

 //Датчик детонации 2
 int ks_2 = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &ks_2))
  return false;
 diagInpDat.ks_2 = ((float)ks_2) * m_adc_discrete;

 //байт с состоянием цифровых входов
 int word = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &word))
  return false;

 //газовый клапан, ДПКВ, ДНО(VR), ДФ, "Bootloader", "Default EEPROM"
 diagInpDat.gas    = CHECKBIT16(word, 0);
 diagInpDat.ckps   = CHECKBIT16(word, 1);
 diagInpDat.ref_s  = CHECKBIT16(word, 2);
 diagInpDat.ps     = CHECKBIT16(word, 3);
 diagInpDat.bl     = CHECKBIT16(word, 4);
 diagInpDat.de     = CHECKBIT16(word, 5);
 diagInpDat.ign_i  = CHECKBIT16(word, 6); //SECU-3i
 diagInpDat.cond_i = CHECKBIT16(word, 7); //SECU-3i
 diagInpDat.epas_i = CHECKBIT16(word, 8); //SECU-3i

 return true;
}

//-----------------------------------------------------------------------
bool CControlApp::Parse_CHOKE_PAR(const BYTE* raw_packet, size_t size)
{
 SECU3IO::ChokePar& chokePar = m_recepted_packet.m_ChokePar;
 if (size != 14)  //размер пакета без сигнального символа, дескриптора и символа-конца пакета
  return false;

 //Number of stepper motor steps
 if (false == mp_pdp->Hex16ToBin(raw_packet, &chokePar.sm_steps))
  return false;

 //choke testing mode command/state (it is fake parameter)
 if (false == mp_pdp->Hex4ToBin(raw_packet, &chokePar.testing))
  return false;

 //manual position setting delta (it is fake parameter and must be zero in this ingoing packet)
 int delta = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &delta))
  return false;
 chokePar.manual_delta = delta;

 //Choke RPM regulator integral factor
 int choke_rpm_if;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &choke_rpm_if))
  return false;
 chokePar.choke_rpm_if = ((float)choke_rpm_if) / 1024.0f;

 //Startup correction apply time (2 points)
 int choke_corr_time;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &choke_corr_time))
  return false;
 chokePar.choke_corr_time[0] = ((float)choke_corr_time / 100.0f);

 if (false == mp_pdp->Hex16ToBin(raw_packet, &choke_corr_time))
  return false;
 chokePar.choke_corr_time[1] = ((float)choke_corr_time / 100.0f);

 //Choke flags
 BYTE choke_flags = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &choke_flags))
  return false;
 chokePar.useclrpmreg = CHECKBIT8(choke_flags, 0);
 chokePar.offrpmreg_ongas = CHECKBIT8(choke_flags, 1);
 chokePar.usethrottle_pos = CHECKBIT8(choke_flags, 2);
 chokePar.sm_maxfreqinit = CHECKBIT8(choke_flags, 3);

 //frequency of stepper motor's pulses
 int sm_freq = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &sm_freq))
  return false;
 chokePar.sm_freq = sm_freq;

 //IAC Crank to run time
 int cranktorun_time = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &cranktorun_time))
  return false;
 chokePar.inj_cranktorun_time = float(cranktorun_time) / 100.0f;

 return true;
}

//-----------------------------------------------------------------------
bool CControlApp::Parse_GASDOSE_PAR(const BYTE* raw_packet, size_t size)
{
 SECU3IO::GasdosePar& gasdosePar = m_recepted_packet.m_GasdosePar;
 if (size != 13)  //размер пакета без сигнального символа, дескриптора и символа-конца пакета
  return false;

 //Number of stepper motor steps
 if (false == mp_pdp->Hex16ToBin(raw_packet, &gasdosePar.gd_steps))
  return false;

 //choke testing mode command/state (it is fake parameter)
 if (false == mp_pdp->Hex4ToBin(raw_packet, &gasdosePar.testing))
  return false;

 //manual position setting delta (it is fake parameter and must be zero in this ingoing packet)
 int delta = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &delta))
  return false;
 gasdosePar.manual_delta = delta;

 //Closing in fuel cut mode
 BYTE fc_closing = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &fc_closing))
  return false;
 gasdosePar.fc_closing = ((float)fc_closing) / 2.0f;

 int corrlimit_p = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &corrlimit_p))
  return false;
 gasdosePar.lam_corr_limit_p = (float(corrlimit_p) / 512.0f) * 100.0f;

 int corrlimit_m = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &corrlimit_m))
  return false;
 gasdosePar.lam_corr_limit_m = (float(corrlimit_m) / 512.0f) * 100.0f;

 //Stoichiometric value for gas
 int lam_stoichval = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &lam_stoichval))
  return false;
 gasdosePar.lam_stoichval = ((float)lam_stoichval) / 128.0f;

 //frequency of stepper motor's pulses
 int gd_freq = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &gd_freq))
  return false;
 gasdosePar.gd_freq = gd_freq;

 //Flag
 int gd_maxfreqinit = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &gd_maxfreqinit))
  return false;
 gasdosePar.gd_maxfreqinit = gd_maxfreqinit;

 return true;
}

//-----------------------------------------------------------------------
bool CControlApp::Parse_SECUR_PAR(const BYTE* raw_packet, size_t size)
{
 SECU3IO::SecurPar& securPar = m_recepted_packet.m_SecurPar;
 if (size != 15)  //размер пакета без сигнального символа, дескриптора и символа-конца пакета
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

 _tcscpy(securPar.bt_name, _T(""));
 _tcscpy(securPar.bt_pass, _T(""));

 //Bluetooth and security flags
 unsigned char flags = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &flags))
  return false;

 securPar.use_bt   = CHECKBIT8(flags, 0);
 securPar.set_btbr = CHECKBIT8(flags, 1);
 securPar.use_imm  = CHECKBIT8(flags, 2);
 securPar.use_respar = CHECKBIT8(flags, 3);
 securPar.chk_fwcrc = CHECKBIT8(flags, 4);
 securPar.bt_type = (flags >> 5) & 0x3;   //read 5,6 bits

 //Parse out iButton keys
 BYTE key[IBTN_KEY_SIZE]; int i, j;
 for(j = 0; j < IBTN_KEYS_NUM; ++j)
 {
  for(i = 0; i < IBTN_KEY_SIZE; ++i)
  {
   if (false == mp_pdp->Hex8ToBin(raw_packet, &key[i]))
    return false;
  }
  memcpy(securPar.ibtn_keys[j], key, IBTN_KEY_SIZE);
 }

 return true;
}

//-----------------------------------------------------------------------
bool CControlApp::Parse_UNIOUT_PAR(const BYTE* raw_packet, size_t size)
{
 SECU3IO::UniOutPar& uniOutPar = m_recepted_packet.m_UniOutPar;
 if (size != 67)  //размер пакета без сигнального символа, дескриптора и символа-конца пакета
  return false;

 CondEncoder cen(m_quartz_frq, m_period_distance);

 for(int oi = 0; oi < UNI_OUTPUT_NUM; ++oi)
 {
  unsigned char flags = 0;
  if (false == mp_pdp->Hex8ToBin(raw_packet, &flags))
   return false;
  uniOutPar.out[oi].logicFunc = flags >> 4;
  uniOutPar.out[oi].invers_1 = (flags & 0x01) != 0;
  uniOutPar.out[oi].invers_2 = (flags & 0x02) != 0;
  uniOutPar.out[oi].use      = (flags & 0x04) != 0;
  uniOutPar.out[oi].invers   = (flags & 0x08) != 0;

  unsigned char cond1 = 0;
  if (false == mp_pdp->Hex8ToBin(raw_packet, &cond1))
   return false;
  uniOutPar.out[oi].condition1 = cond1;

  unsigned char cond2 = 0;
  if (false == mp_pdp->Hex8ToBin(raw_packet, &cond2))
   return false;
  uniOutPar.out[oi].condition2 = cond2;

  int on_thrd_1 = 0;
  if (false == mp_pdp->Hex16ToBin(raw_packet, &on_thrd_1, cen.isSigned(cond1)))
   return false;
  uniOutPar.out[oi].on_thrd_1 = cen.UniOutDecodeCondVal(on_thrd_1, cond1);

  int off_thrd_1 = 0;
  if (false == mp_pdp->Hex16ToBin(raw_packet, &off_thrd_1, cen.isSigned(cond1)))
   return false;
  uniOutPar.out[oi].off_thrd_1 = cen.UniOutDecodeCondVal(off_thrd_1, cond1);

  int on_thrd_2 = 0;
  if (false == mp_pdp->Hex16ToBin(raw_packet, &on_thrd_2, cen.isSigned(cond2)))
   return false;
  uniOutPar.out[oi].on_thrd_2 = cen.UniOutDecodeCondVal(on_thrd_2, cond2);

  int off_thrd_2 = 0;
  if (false == mp_pdp->Hex16ToBin(raw_packet, &off_thrd_2, cen.isSigned(cond2)))
   return false;
  uniOutPar.out[oi].off_thrd_2 = cen.UniOutDecodeCondVal(off_thrd_2, cond2);
 }

 //logic function for 1st and 2nd outputs
 BYTE lf12 = 0;
 if (false == mp_pdp->Hex4ToBin(raw_packet, &lf12))
  return false;
 uniOutPar.logicFunc12 = lf12;

 return true;
}

//-----------------------------------------------------------------------
bool CControlApp::Parse_INJCTR_PAR(const BYTE* raw_packet, size_t size)
{
 SECU3IO::InjctrPar& injctrPar = m_recepted_packet.m_InjctrPar;
 if (size != 47)  //размер пакета без сигнального символа, дескриптора и символа-конца пакета
  return false;

 unsigned char inj_flags = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &inj_flags))
  return false;
 injctrPar.inj_usetimingmap[0] = CHECKBIT8(inj_flags, 0);
 injctrPar.inj_usetimingmap[1] = CHECKBIT8(inj_flags, 1);
 injctrPar.inj_useaddcorrs = CHECKBIT8(inj_flags, 2);
 injctrPar.inj_useairden = CHECKBIT8(inj_flags, 3);
 injctrPar.inj_usediffpress = CHECKBIT8(inj_flags, 4);
 injctrPar.inj_secinjrowswt = CHECKBIT8(inj_flags, 5);

 unsigned char inj_config = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &inj_config))
  return false;
 injctrPar.inj_config[0] = GETHI4BITS(inj_config);      //configuration
 injctrPar.inj_squirt_num[0] = GETLO4BITS(inj_config);  //number of squirts per cycle
 //second fuel:
 if (false == mp_pdp->Hex8ToBin(raw_packet, &inj_config))
  return false;
 injctrPar.inj_config[1] = GETHI4BITS(inj_config);      //configuration
 injctrPar.inj_squirt_num[1] = GETLO4BITS(inj_config);  //number of squirts per cycle

 int inj_flow_rate = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &inj_flow_rate))
  return false;
 injctrPar.inj_flow_rate[0] = float(inj_flow_rate) / 64.0f;
 //second fuel:
 if (false == mp_pdp->Hex16ToBin(raw_packet, &inj_flow_rate))
  return false;
 injctrPar.inj_flow_rate[1] = float(inj_flow_rate) / 64.0f;

 int inj_cyl_disp = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &inj_cyl_disp))
  return false;
 injctrPar.inj_cyl_disp = float(inj_cyl_disp) / 16384.0f;

 unsigned long inj_sd_igl_const = 0;
 if (false == mp_pdp->Hex32ToBin(raw_packet, &inj_sd_igl_const))
  return false;
 injctrPar.inj_sd_igl_const[0] = (float)inj_sd_igl_const;
 //second fuel:
 if (false == mp_pdp->Hex32ToBin(raw_packet, &inj_sd_igl_const))
  return false;
 injctrPar.inj_sd_igl_const[1] = (float)inj_sd_igl_const;

 //read-only parameter: number of engine cylinders
 unsigned char cyl_num = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &cyl_num))
  return false;
 injctrPar.cyl_num = cyl_num;

 //injection timing (phase)
 int  inj_begin_angle;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &inj_begin_angle, true))
  return false;
 injctrPar.inj_timing[0] = ((float)inj_begin_angle) / PARINJTIM_MULT;
 //second fuel:
 if (false == mp_pdp->Hex16ToBin(raw_packet, &inj_begin_angle, true))
  return false;
 injctrPar.inj_timing[1] = ((float)inj_begin_angle) / PARINJTIM_MULT;

 //injection timing (phase) on cranking
 int  inj_begin_angle_crk;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &inj_begin_angle_crk, true))
  return false;
 injctrPar.inj_timing_crk[0] = ((float)inj_begin_angle_crk) / PARINJTIM_MULT;
 //second fuel:
 if (false == mp_pdp->Hex16ToBin(raw_packet, &inj_begin_angle_crk, true))
  return false;
 injctrPar.inj_timing_crk[1] = ((float)inj_begin_angle_crk) / PARINJTIM_MULT;

 //inj.pulse origin
 unsigned char anglespec = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &anglespec))
  return false;
 injctrPar.inj_anglespec[0] = GETLO4BITS(anglespec);
 injctrPar.inj_anglespec[1] = GETHI4BITS(anglespec);  //second fuel

 //fff_const (fuel consumption)
 int  fff;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &fff))
  return false;
 injctrPar.fff_const = ((float)fff / 65536.0f) * (1000.0f * 60.0f);

 //min. injection PW
 float discrete = (m_quartz_frq == 20000000 ? 3.2f : 4.0f);
 unsigned char inj_min_pw = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &inj_min_pw))
  return false;
 injctrPar.inj_min_pw[1] = ((((float)inj_min_pw) * 8) * discrete) / 1000.0f; //convert to ms
 inj_min_pw = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &inj_min_pw))
  return false;
 injctrPar.inj_min_pw[0] = ((((float)inj_min_pw) * 8) * discrete) / 1000.0f; //convert to ms

 //MAF consts
 unsigned long inj_maf_const = 0;
 if (false == mp_pdp->Hex32ToBin(raw_packet, &inj_maf_const))
  return false;
 injctrPar.inj_maf_const[0] = (float)inj_maf_const;
 //second fuel:
 if (false == mp_pdp->Hex32ToBin(raw_packet, &inj_maf_const))
  return false;
 injctrPar.inj_maf_const[1] = (float)inj_maf_const;

 unsigned long mafload_const = 0;
 if (false == mp_pdp->Hex32ToBin(raw_packet, &mafload_const))
  return false;
 injctrPar.mafload_const = (float)mafload_const;

 int inj_max_pw = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &inj_max_pw))
  return false;
 injctrPar.inj_max_pw[0] = float(inj_max_pw) * (3.2f / 1000.0f); //from 3.2 us units to ms
 //second fuel:
 if (false == mp_pdp->Hex16ToBin(raw_packet, &inj_max_pw))
  return false;
 injctrPar.inj_max_pw[1] = float(inj_max_pw) * (3.2f / 1000.0f); //from 3.2 us units to ms

 return true;
}

//-----------------------------------------------------------------------
bool CControlApp::Parse_LAMBDA_PAR(const BYTE* raw_packet, size_t size)
{
 SECU3IO::LambdaPar& lambdaPar = m_recepted_packet.m_LambdaPar;
 if (size != 27)  //размер пакета без сигнального символа, дескриптора и символа-конца пакета
  return false;

 unsigned char strperstp = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &strperstp))
  return false;
 lambdaPar.lam_str_per_stp = strperstp;

 unsigned char stepsize_p = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &stepsize_p))
  return false;
 lambdaPar.lam_step_size_p = (float(stepsize_p) / 512.0f) * 100.0f;

 unsigned char stepsize_m = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &stepsize_m))
  return false;
 lambdaPar.lam_step_size_m = (float(stepsize_m) / 512.0f) * 100.0f;

 int corrlimit_p = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &corrlimit_p))
  return false;
 lambdaPar.lam_corr_limit_p = (float(corrlimit_p) / 512.0f) * 100.0f;

 int corrlimit_m = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &corrlimit_m))
  return false;
 lambdaPar.lam_corr_limit_m = (float(corrlimit_m) / 512.0f) * 100.0f;

 int swtpoint = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &swtpoint))
  return false;
 lambdaPar.lam_swt_point = float(swtpoint) * ADC_DISCRETE;

 int tempthrd = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &tempthrd))
  return false;
 lambdaPar.lam_temp_thrd = float(tempthrd) / TEMP_PHYSICAL_MAGNITUDE_MULTIPLIER;

 int rpmthrd = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &rpmthrd))
  return false;
 lambdaPar.lam_rpm_thrd = rpmthrd;

 int activdelay = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &activdelay))
  return false;
 lambdaPar.lam_activ_delay = activdelay;

 int deadband = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &deadband))
  return false;
 lambdaPar.lam_dead_band = float(deadband) * ADC_DISCRETE;

 unsigned char senstype = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &senstype))
  return false;
 lambdaPar.lam_senstype = senstype;

 unsigned char msperstp = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &msperstp))
  return false;
 lambdaPar.lam_ms_per_stp = msperstp * 10;

 unsigned char lam_flags = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &lam_flags))
  return false;
 lambdaPar.lam_htgdet = CHECKBIT8(lam_flags, 0);
 lambdaPar.lam_idlcorr = CHECKBIT8(lam_flags, 1);
 lambdaPar.lam_crkheat = CHECKBIT8(lam_flags, 2);

 //Stoichiometric value for second fuel
 int lam_2stoichval = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &lam_2stoichval))
  return false;
 lambdaPar.lam_2stoichval = ((float)lam_2stoichval) / 128.0f;

 //heating:
 unsigned char eh_heating_time[2] = {0, 0};
 if (false == mp_pdp->Hex8ToBin(raw_packet, &eh_heating_time[0]))
  return false;
 lambdaPar.eh_heating_time[0] = eh_heating_time[0];

 if (false == mp_pdp->Hex8ToBin(raw_packet, &eh_heating_time[1]))
  return false;
 lambdaPar.eh_heating_time[1] = eh_heating_time[1];

 unsigned char eh_temper_thrd = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &eh_temper_thrd))
  return false;
 lambdaPar.eh_temper_thrd = (float)eh_temper_thrd;

 unsigned char eh_heating_act = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &eh_heating_act))
  return false;
 lambdaPar.eh_heating_act = ((float)eh_heating_act) / 100.0f;

 int eh_aflow_thrd = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &eh_aflow_thrd))
  return false;
 lambdaPar.eh_aflow_thrd = float(eh_aflow_thrd) * 32.0f;

 return true;
}

//-----------------------------------------------------------------------
bool CControlApp::Parse_ACCEL_PAR(const BYTE* raw_packet, size_t size)
{
 SECU3IO::AccelPar& accelPar = m_recepted_packet.m_AccelPar;
 if (size != 8)  //размер пакета без сигнального символа, дескриптора и символа-конца пакета
  return false;

 unsigned char tpsdot_thrd = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &tpsdot_thrd))
  return false;
 accelPar.ae_tpsdot_thrd = (float)tpsdot_thrd;

 int coldacc_mult = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &coldacc_mult))
  return false;
 accelPar.ae_coldacc_mult = ((((float)coldacc_mult) + 128.0f) / 128.0f) * 100.0f; //convert to %

 unsigned char ae_decay_time = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &ae_decay_time))
  return false;
 accelPar.ae_decay_time = ae_decay_time; //strokes

 unsigned char ae_type = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &ae_type))
  return false;
 accelPar.ae_type = ae_type;

 unsigned char ae_time = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &ae_time))
  return false;
 accelPar.ae_time = ae_time; //strokes

 unsigned char ae_ballance = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &ae_ballance))
  return false;
 accelPar.ae_ballance = ((float)ae_ballance) / 2.56f; //multiply by 100% and divide by 256

 unsigned char mapdot_thrd = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &mapdot_thrd))
  return false;
 accelPar.ae_mapdot_thrd = (float)mapdot_thrd;

 return true;
}

//-----------------------------------------------------------------------
bool CControlApp::Parse_INJDRV_PAR(const BYTE* raw_packet, size_t size)
{
 SECU3IO::InjDrvPar& recv = m_recepted_packet.m_InjDrvPar;
 if (size != 209)  //размер пакета без сигнального символа, дескриптора и символа-конца пакета
  return false;

 mp_pdp->resetCRC();

 //status info
 unsigned char ee_status = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &ee_status))
  return false;
 recv.ee_status = CHECKBIT8(ee_status, 0);
 recv.set0_corrupted = CHECKBIT8(ee_status, 1);
 recv.set1_corrupted = CHECKBIT8(ee_status, 2);
 recv.gas_v = CHECKBIT8(ee_status, 3);
 recv.dev_address = CHECKBIT8(ee_status, 4);
 recv.set_idx = CHECKBIT8(ee_status, 7);
 recv.broadcast = false;

 unsigned char type = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &type))
  return false;
 recv.type = type;

 unsigned char version = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &version))
  return false;
 recv.version = version;

 unsigned char fw_opt = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &fw_opt))
  return false;
 recv.fw_opt = fw_opt;

 int voltage = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &voltage))
  return false;
 recv.voltage = (float)(((double)voltage) * 0.004882812);

 //settings
 unsigned char direct_flags = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &direct_flags))
  return false;
 for (int i = 0; i < 8; i++)
  recv.direct_flags[i] = CHECKBIT8(direct_flags, i);

 unsigned char lutabl_flags = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &lutabl_flags))
  return false;
 recv.m_peak_on_usetab = CHECKBIT8(lutabl_flags, 0);
 recv.m_peak_duty_usetab = CHECKBIT8(lutabl_flags, 1);
 recv.m_hold_duty_usetab = CHECKBIT8(lutabl_flags, 2);
 recv.m_peak_full_usetab = CHECKBIT8(lutabl_flags, 3);
 recv.m_pth_pause_usetab = CHECKBIT8(lutabl_flags, 4);

 unsigned char gen_flags = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &gen_flags))
  return false;
 recv.m_tst_peak_pwm = CHECKBIT8(gen_flags, 0);
 recv.m_tst_hold_pwm = CHECKBIT8(gen_flags, 1);

 unsigned char testch_sel = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &testch_sel))
  return false;
 recv.m_testch_sel = testch_sel;

 int testch_frq = 0;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &testch_frq))
  return false;
 recv.m_testch_frq = (float)(1.0/(((double)testch_frq) / 131072.0));

 BYTE testch_duty = 0;
 if (false == mp_pdp->Hex8ToBin(raw_packet, &testch_duty))
  return false;
 recv.m_testch_duty = (float)((((double)testch_duty) / 255.0) * 100.0f);

 int pwm_period;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &pwm_period))
  return false;
 recv.m_pwm_period = ((float)pwm_period) / 20.0f;

 int peak_duty;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &peak_duty))
  return false;
 recv.m_peak_duty = (((float)peak_duty) / 4096.0f) * 100.0f;

 int reserved;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &reserved))
  return false;
 recv.m_reserved = (float)reserved;

 int hold_duty;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &hold_duty))
  return false;
 recv.m_hold_duty = (((float)hold_duty) / 4096.0f) * 100.0f;

 int peak_on_time;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &peak_on_time))
  return false;
 recv.m_peak_on_time = ((float)peak_on_time) / 2.5f;

 int peak_pwm_time;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &peak_pwm_time))
  return false;
 recv.m_peak_pwm_time = ((float)peak_pwm_time) / 2.5f;

 int pth_pause;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &pth_pause))
  return false;
 recv.m_pth_pause = ((float)pth_pause) / 2.5f;

 for (int i = 0; i < LUTABSIZE; ++i)
 {
  int value;
  if (false == mp_pdp->Hex16ToBin(raw_packet, &value))
   return false;
  recv.m_peak_on_tab[i] = ((float)value) / 2.5f;
 }

 for (int i = 0; i < LUTABSIZE; ++i)
 {
  int value;
  if (false == mp_pdp->Hex16ToBin(raw_packet, &value))
   return false;
  recv.m_peak_duty_tab[i] = (((float)value) / 4096.0f) * 100.0f;
 }

 for (int i = 0; i < LUTABSIZE; ++i)
 {
  int value;
  if (false == mp_pdp->Hex16ToBin(raw_packet, &value))
   return false;
  recv.m_hold_duty_tab[i] = (((float)value) / 4096.0f) * 100.0f;
 }

 for (int i = 0; i < LUTABSIZE; ++i)
 {
  int value;
  if (false == mp_pdp->Hex16ToBin(raw_packet, &value))
   return false;
  recv.m_peak_full_tab[i] = ((float)value) / 2.5f;
 }

 for (int i = 0; i < LUTABSIZE; ++i)
 {
  int value;
  if (false == mp_pdp->Hex16ToBin(raw_packet, &value))
   return false;
  recv.m_pth_pause_tab[i] = ((float)value) / 2.5f;
 }

 for (int i = 0; i < PWCORSIZE; ++i)
 {
  int value;
  if (false == mp_pdp->Hex16ToBin(raw_packet, &value))
   return false;
  recv.m_pwmul_tab[i] = ((float)value) / (65536.0f / 100.0f); //convert to %
 }

 for (int i = 0; i < PWCORSIZE; ++i)
 {
  int value;
  if (false == mp_pdp->Hex16ToBin(raw_packet, &value))
   return false;
  recv.m_pwadd_tab[i] = ((float)value) / 2.5f;        //convert to us
 }

 int calc_crc = mp_pdp->getCRC();

 int packet_crc;
 if (false == mp_pdp->Hex16ToBin(raw_packet, &packet_crc))
  return false;
 if (packet_crc != calc_crc)
  return false; //packet corrupted

 return true;
}

//-----------------------------------------------------------------------
bool CControlApp::Parse_LZIDBL_HS(const BYTE* raw_packet, size_t size)
{
 SECU3IO::LzidBLHS& recv = m_recepted_packet.m_LzidBLHS;
 if (size != 5)  //размер пакета без сигнального символа, дескриптора и символа-конца пакета
  return false;

 strncpy(recv.data, (const char*)raw_packet, size); //text data
 return true;
}

//-----------------------------------------------------------------------
//Return: true - if at least 1 packet has been processed successfully
bool CControlApp::ParsePackets()
{
 Packets::iterator it;
 bool status = false;
 m_blman_packet = false;

 ASSERT(m_pPackets);
 for(it = m_pPackets->begin(); it!=m_pPackets->end(); ++it)
 {
  //don't use checksum for LZIDBL_HS packets because a lot of users already have units with bootloader which does't support checksum.
  //For INJDRV_PAR packets' checksum is calculated inside methods of PacketDataProxy class
  bool use_checksum = (*it)[1]!=INJDRV_PAR && (*it)[1]!=LZIDBL_HS; //don't use checksum for specified packet types

  if (it->size() < ((size_t)(use_checksum ? 5 : 3)) || (*it)[0] != '@')
   continue;
  if (it->back() != '\r')
   continue;

  Esc_Rx_Packet(*it, 2, it->size() - 3); //byte stuffing

  if (use_checksum)
  {
   int checksum1 = FletcherChecksum(*it, 2, it->size() - 5);
   int checksum2 = MAKEWORD((*it)[it->size()-2], (*it)[it->size()-3]);
   if (checksum1 != checksum2)
    continue; //packet is corrupted, don't accept it
  }

  size_t p_size = it->size() - ((size_t)(use_checksum ? 5 : 3));
  const BYTE* p_start = &(*it)[2];
  switch((*it)[1])
  {
   case TEMPER_PAR:
    if (Parse_TEMPER_PAR(p_start, p_size))
     break; //packet parsed successfully
    continue;//packet is corrupted, don't accept it
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
    {
     if (m_recepted_packet.m_OPCompNc.opcode == OPCODE_BL_MANSTART && m_recepted_packet.m_OPCompNc.opdata == OPDATA_BL_MANSTART)
      m_blman_packet = true; //special case (this packet shuold not cause online status)
     break;
    }
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
   case LTFT_DAT:
    if (Parse_LTFT_DAT(p_start, p_size))
     break;
    continue;
   case RPMGRD_PAR:
    if (Parse_RPMGRD_PAR(p_start, p_size))
     break;
    continue;
   case CLTGRD_PAR:
    if (Parse_CLTGRD_PAR(p_start, p_size))
     break;
    continue;
   case LODGRD_PAR:
    if (Parse_LODGRD_PAR(p_start, p_size))
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
   case INJDRV_PAR:
    {
    mp_pdp->EnableCRC(true);
    bool result = Parse_INJDRV_PAR(p_start, p_size);
    mp_pdp->EnableCRC(false);
    if (result)
     break;
    continue;
    }
   case LZIDBL_HS:
    if (Parse_LZIDBL_HS(p_start, p_size))
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

  if (m_ignore_n_packets)
   InterlockedDecrement(&m_ignore_n_packets);
  else
  {
   m_pEventHandler->OnPacketReceived((*it)[1], &EndPendingPacket());
   status = true;
  }
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
   if ((p_capp->m_blman_packet==false && p_capp->m_online_state==false)||(p_capp->m_force_notify_about_connection))  //мы были в оффлайне, надо известить пользователя о переходе в онлайн...
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
  case CLTGRD_PAR:
  case LODGRD_PAR:
  case DIAGINP_DAT:
  case DIAGOUT_DAT:
  case CHOKE_PAR:
  case GASDOSE_PAR: //GD
  case SECUR_PAR:
  case UNIOUT_PAR:
  case INJCTR_PAR:
  case LAMBDA_PAR:
  case ACCEL_PAR:
  case INJDRV_PAR:
  case SILENT:
  case LZIDBL_HS:
  case INJDRV_ADR:
  case LTFT_DAT:
   return true;
  default:
   return false;
 }//switch
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
   m_outgoing_packet.push_back(*((BYTE*)i_packet_data));
   break;
  case CHANGEMODE:
   m_outgoing_packet.push_back(*((BYTE*)i_packet_data));
   break;
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
  case INJDRV_PAR:
   mp_pdp->EnableCRC(true);
   Build_INJDRV_PAR((InjDrvPar*)i_packet_data);
   break;
  case INJDRV_ADR:
   mp_pdp->EnableCRC(true);
   Build_INJDRV_ADR((InjDrvAdr*)i_packet_data);
   break;
  case LZIDBL_HS:
   Build_LZIDBL_HS((LzidBLHS*)i_packet_data);
   break;

  default:
   return false; //invalid descriptor
 }//switch

 mp_pdp->EnableCRC(false);


 //don't use checksum for LZIDBL_HS packets because a lot of users already have units with bootloader which does't support checksum.
 if (i_descriptor!=LZIDBL_HS && !mp_pdp->isCRCEnabled())
 {
  //calculate and put checksum value
  int checksum = FletcherChecksum(m_outgoing_packet, 2, m_outgoing_packet.size() - 2);
  mp_pdp->Bin16ToHex(checksum, m_outgoing_packet); //append packet with two bytes of checksum
 }

 m_outgoing_packet.push_back('\r');

 Esc_Tx_Packet(m_outgoing_packet, 2, m_outgoing_packet.size() - 3); //byte stuffing

 return m_p_port->SendBlock(&m_outgoing_packet[0], m_outgoing_packet.size());
}

//-----------------------------------------------------------------------
//посылает команду изменения контекста на новый контекст специфицированный i_new_descriptor-ом
bool CControlApp::ChangeContext(const BYTE i_new_descriptor)
{
 if (false==IsValidDescriptor(i_new_descriptor))  //передали правильный дескриптор ?
  return false;

 BYTE data = i_new_descriptor; 
 return SendPacket(CHANGEMODE, &data);
}
//-----------------------------------------------------------------------
//Посылает команду запуска бутлоадера
bool CControlApp::StartBootLoader()
{
 BYTE data = 'l'; 
 return SendPacket(BOOTLOADER, &data);
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
 int fuelcut_uni = (packet_data->fuelcut_uni==UNI_OUTPUT_NUM) ? 0xF : packet_data->fuelcut_uni;
 mp_pdp->Bin8ToHex(fuelcut_uni, m_outgoing_packet);
 int igncut_uni = (packet_data->igncut_uni==UNI_OUTPUT_NUM) ? 0xF : packet_data->igncut_uni;
 mp_pdp->Bin8ToHex(igncut_uni, m_outgoing_packet);
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
 WRITEBIT8(flags, 5, packet_data->use_thrassmap);
 
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

 int idl_reg_p = MathHelpers::Round(packet_data->idl_reg_p[0] * 256.0f);
 mp_pdp->Bin16ToHex(idl_reg_p, m_outgoing_packet);
 idl_reg_p = MathHelpers::Round(packet_data->idl_reg_p[1] * 256.0f);
 mp_pdp->Bin16ToHex(idl_reg_p, m_outgoing_packet);

 int idl_reg_i = MathHelpers::Round(packet_data->idl_reg_i[0] * 256.0f);
 mp_pdp->Bin16ToHex(idl_reg_i, m_outgoing_packet);
 idl_reg_i = MathHelpers::Round(packet_data->idl_reg_i[1] * 256.0f);
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

 mp_pdp->Bin16ToHex(packet_data->iac_reg_db, m_outgoing_packet);
}

//-----------------------------------------------------------------------
void CControlApp::Build_STARTR_PAR(StartrPar* packet_data)
{
 mp_pdp->Bin16ToHex(packet_data->starter_off,m_outgoing_packet);
 mp_pdp->Bin16ToHex(packet_data->smap_abandon,m_outgoing_packet);
 int cranktorun_time = MathHelpers::Round(packet_data->inj_cranktorun_time * 100.0f);
 mp_pdp->Bin16ToHex(cranktorun_time, m_outgoing_packet);
 int inj_aftstr_strokes = MathHelpers::Round(packet_data->inj_aftstr_strokes[0] / 4.0f);
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
 int inj_aftstr_strokes1 = MathHelpers::Round(packet_data->inj_aftstr_strokes[1] / 4.0f);
 mp_pdp->Bin8ToHex(inj_aftstr_strokes1, m_outgoing_packet);
 int stbl_str_cnt = MathHelpers::Round(packet_data->stbl_str_cnt);
 mp_pdp->Bin8ToHex(stbl_str_cnt, m_outgoing_packet);
 unsigned char flags = 0; //not used now
 WRITEBIT8(flags, 0, packet_data->fldclr_start);
 mp_pdp->Bin8ToHex(flags, m_outgoing_packet);
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
 unsigned char flags = 0;
 WRITEBIT8(flags, 0, packet_data->igntim_wrkmap);
 WRITEBIT8(flags, 1, packet_data->manigntim_idl);
 WRITEBIT8(flags, 2, packet_data->zero_adv_ang_oct);
 mp_pdp->Bin8ToHex(flags, m_outgoing_packet);
 int shift_igntim = MathHelpers::Round(packet_data->shift_igntim * m_angle_multiplier);
 mp_pdp->Bin16ToHex(shift_igntim, m_outgoing_packet);
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
 unsigned char flags = 0;
 WRITEBIT8(flags, 0, packet_data->use_load_grid);
 mp_pdp->Bin8ToHex(flags, m_outgoing_packet);
 unsigned char ve2_map_func = packet_data->ve2_map_func;
 mp_pdp->Bin8ToHex(ve2_map_func, m_outgoing_packet);
 int gas_v_uni = (packet_data->uni_gas_v==UNI_OUTPUT_NUM) ? 0xF : packet_data->uni_gas_v;
 mp_pdp->Bin8ToHex(gas_v_uni, m_outgoing_packet);
 mp_pdp->Bin8ToHex(0, m_outgoing_packet); //stub for cyl_num
 int inj_cyl_disp = MathHelpers::Round(packet_data->inj_cyl_disp * 16384.0f);
 mp_pdp->Bin16ToHex(inj_cyl_disp, m_outgoing_packet);
 mp_pdp->Bin32ToHex((unsigned long)packet_data->mafload_const, m_outgoing_packet);
}

//-----------------------------------------------------------------------
void CControlApp::Build_ADCCOR_PAR(ADCCompenPar* packet_data)
{
 unsigned char flags = 0;
 WRITEBIT8(flags, 0, packet_data->adc_comp_mode);
 mp_pdp->Bin8ToHex(flags, m_outgoing_packet);

 signed int map_adc_factor = MathHelpers::Round(packet_data->map_adc_factor * 16384);
 mp_pdp->Bin16ToHex(map_adc_factor,m_outgoing_packet);
 signed long map_correction_d = MathHelpers::Round((-packet_data->map_adc_correction) / m_adc_discrete); //переводим из вольтов в дискреты АЦП
 signed long map_adc_correction = MathHelpers::Round(16384 * (0.5f - map_correction_d * (packet_data->adc_comp_mode ? packet_data->map_adc_factor : 1.0f)));
 mp_pdp->Bin32ToHex(map_adc_correction,m_outgoing_packet);

 signed int ubat_adc_factor = MathHelpers::Round(packet_data->ubat_adc_factor * 16384);
 mp_pdp->Bin16ToHex(ubat_adc_factor,m_outgoing_packet);
 signed long ubat_correction_d = MathHelpers::Round((-packet_data->ubat_adc_correction) / m_adc_discrete); //переводим из вольтов в дискреты АЦП
 signed long ubat_adc_correction = MathHelpers::Round(16384 * (0.5f - ubat_correction_d * (packet_data->adc_comp_mode ? packet_data->ubat_adc_factor : 1.0f)));
 mp_pdp->Bin32ToHex(ubat_adc_correction,m_outgoing_packet);

 signed int temp_adc_factor = MathHelpers::Round(packet_data->temp_adc_factor * 16384);
 mp_pdp->Bin16ToHex(temp_adc_factor,m_outgoing_packet);
 signed long temp_correction_d = MathHelpers::Round((-packet_data->temp_adc_correction) / m_adc_discrete); //переводим из вольтов в дискреты АЦП
 signed long temp_adc_correction = MathHelpers::Round(16384 * (0.5f - temp_correction_d * (packet_data->adc_comp_mode ? packet_data->temp_adc_factor : 1.0f)));
 mp_pdp->Bin32ToHex(temp_adc_correction,m_outgoing_packet);
 //TPS sensor
 signed int tps_adc_factor = MathHelpers::Round(packet_data->tps_adc_factor * 16384);
 mp_pdp->Bin16ToHex(tps_adc_factor,m_outgoing_packet);
 signed long tps_correction_d = MathHelpers::Round((-packet_data->tps_adc_correction) / m_adc_discrete); //переводим из вольтов в дискреты АЦП
 signed long tps_adc_correction = MathHelpers::Round(16384 * (0.5f - tps_correction_d * (packet_data->adc_comp_mode ? packet_data->tps_adc_factor : 1.0f)));
 mp_pdp->Bin32ToHex(tps_adc_correction,m_outgoing_packet);
 //ADD_I1 input
 signed int ai1_adc_factor = MathHelpers::Round(packet_data->ai1_adc_factor * 16384);
 mp_pdp->Bin16ToHex(ai1_adc_factor,m_outgoing_packet);
 signed long ai1_correction_d = MathHelpers::Round((-packet_data->ai1_adc_correction) / m_adc_discrete); //переводим из вольтов в дискреты АЦП
 signed long ai1_adc_correction = MathHelpers::Round(16384 * (0.5f - ai1_correction_d * (packet_data->adc_comp_mode ? packet_data->ai1_adc_factor : 1.0f)));
 mp_pdp->Bin32ToHex(ai1_adc_correction,m_outgoing_packet);
 //ADD_I2 input
 signed int ai2_adc_factor = MathHelpers::Round(packet_data->ai2_adc_factor * 16384);
 mp_pdp->Bin16ToHex(ai2_adc_factor,m_outgoing_packet);
 signed long ai2_correction_d = MathHelpers::Round((-packet_data->ai2_adc_correction) / m_adc_discrete); //переводим из вольтов в дискреты АЦП
 signed long ai2_adc_correction = MathHelpers::Round(16384 * (0.5f - ai2_correction_d * (packet_data->adc_comp_mode ? packet_data->ai2_adc_factor : 1.0f)));
 mp_pdp->Bin32ToHex(ai2_adc_correction,m_outgoing_packet);
 //ADD_I3 input
 signed int ai3_adc_factor = MathHelpers::Round(packet_data->ai3_adc_factor * 16384);
 mp_pdp->Bin16ToHex(ai3_adc_factor,m_outgoing_packet);
 signed long ai3_correction_d = MathHelpers::Round((-packet_data->ai3_adc_correction) / m_adc_discrete); //переводим из вольтов в дискреты АЦП
 signed long ai3_adc_correction = MathHelpers::Round(16384 * (0.5f - ai3_correction_d * (packet_data->adc_comp_mode ? packet_data->ai3_adc_factor : 1.0f)));
 mp_pdp->Bin32ToHex(ai3_adc_correction,m_outgoing_packet);
 //ADD_I4 input
 signed int ai4_adc_factor = MathHelpers::Round(packet_data->ai4_adc_factor * 16384);
 mp_pdp->Bin16ToHex(ai4_adc_factor,m_outgoing_packet);
 signed long ai4_correction_d = MathHelpers::Round((-packet_data->ai4_adc_correction) / m_adc_discrete); //переводим из вольтов в дискреты АЦП
 signed long ai4_adc_correction = MathHelpers::Round(16384 * (0.5f - ai4_correction_d * (packet_data->adc_comp_mode ? packet_data->ai4_adc_factor : 1.0f)));
 mp_pdp->Bin32ToHex(ai4_adc_correction,m_outgoing_packet);
 //ADD_I5 input
 signed int ai5_adc_factor = MathHelpers::Round(packet_data->ai5_adc_factor * 16384);
 mp_pdp->Bin16ToHex(ai5_adc_factor,m_outgoing_packet);
 signed long ai5_correction_d = MathHelpers::Round((-packet_data->ai5_adc_correction) / m_adc_discrete); //переводим из вольтов в дискреты АЦП
 signed long ai5_adc_correction = MathHelpers::Round(16384 * (0.5f - ai5_correction_d * (packet_data->adc_comp_mode ? packet_data->ai5_adc_factor : 1.0f)));
 mp_pdp->Bin32ToHex(ai5_adc_correction, m_outgoing_packet);
 //ADD_I6 input
 signed int ai6_adc_factor = MathHelpers::Round(packet_data->ai6_adc_factor * 16384);
 mp_pdp->Bin16ToHex(ai6_adc_factor,m_outgoing_packet);
 signed long ai6_correction_d = MathHelpers::Round((-packet_data->ai6_adc_correction) / m_adc_discrete); //переводим из вольтов в дискреты АЦП
 signed long ai6_adc_correction = MathHelpers::Round(16384 * (0.5f - ai6_correction_d * (packet_data->adc_comp_mode ? packet_data->ai6_adc_factor : 1.0f)));
 mp_pdp->Bin32ToHex(ai6_adc_correction, m_outgoing_packet);
 //ADD_I7 input
 signed int ai7_adc_factor = MathHelpers::Round(packet_data->ai7_adc_factor * 16384);
 mp_pdp->Bin16ToHex(ai7_adc_factor,m_outgoing_packet);
 signed long ai7_correction_d = MathHelpers::Round((-packet_data->ai7_adc_correction) / m_adc_discrete); //переводим из вольтов в дискреты АЦП
 signed long ai7_adc_correction = MathHelpers::Round(16384 * (0.5f - ai7_correction_d * (packet_data->adc_comp_mode ? packet_data->ai7_adc_factor : 1.0f)));
 mp_pdp->Bin32ToHex(ai7_adc_correction,m_outgoing_packet);
 //ADD_I8 input
 signed int ai8_adc_factor = MathHelpers::Round(packet_data->ai8_adc_factor * 16384);
 mp_pdp->Bin16ToHex(ai8_adc_factor,m_outgoing_packet);
 signed long ai8_correction_d = MathHelpers::Round((-packet_data->ai8_adc_correction) / m_adc_discrete); //переводим из вольтов в дискреты АЦП
 signed long ai8_adc_correction = MathHelpers::Round(16384 * (0.5f - ai8_correction_d * (packet_data->adc_comp_mode ? packet_data->ai8_adc_factor : 1.0f)));
 mp_pdp->Bin32ToHex(ai8_adc_correction,m_outgoing_packet);
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

 unsigned char knock_selch = 0;
 for (int i = 0; i < 8; ++i)
  WRITEBIT8(knock_selch, i, packet_data->knock_selch[i]); 
 mp_pdp->Bin8ToHex(knock_selch, m_outgoing_packet);

 int knkclt_thrd = MathHelpers::Round(packet_data->knkclt_thrd * TEMP_PHYSICAL_MAGNITUDE_MULTIPLIER);
 mp_pdp->Bin16ToHex(knkclt_thrd, m_outgoing_packet);
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
 mp_pdp->Bin32ToHex(packet_data->flags, m_outgoing_packet);
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
 int hop_start_ang = MathHelpers::Round(packet_data->hop_start_ang * ANGLE_MULTIPLIER);
 mp_pdp->Bin16ToHex(hop_start_ang, m_outgoing_packet);
 int hop_durat_ang = MathHelpers::Round(packet_data->hop_durat_ang * ANGLE_MULTIPLIER);
 mp_pdp->Bin16ToHex(hop_durat_ang, m_outgoing_packet);

 unsigned char fpf_flags = 0;
 WRITEBIT8(fpf_flags, 0, packet_data->flpmp_offongas);
 WRITEBIT8(fpf_flags, 1, packet_data->inj_offongas);
 WRITEBIT8(fpf_flags, 2, packet_data->inj_offonpet);
 mp_pdp->Bin8ToHex(fpf_flags, m_outgoing_packet);

 int evap_afbegin = MathHelpers::Round(packet_data->evap_afbegin / 32.0f);
 mp_pdp->Bin16ToHex(evap_afbegin, m_outgoing_packet);
 int evap_afslope = MathHelpers::Round(packet_data->evap_afslope * 1048576.0f * 32.0f); //2^20
 mp_pdp->Bin16ToHex(evap_afslope, m_outgoing_packet);

 BYTE fp_timeout_strt = MathHelpers::Round(packet_data->fp_timeout_strt * 10.0f);
 mp_pdp->Bin8ToHex(fp_timeout_strt, m_outgoing_packet);

 int pwmfrq1 = MathHelpers::Round((1.0/packet_data->pwm2_pwmfrq[0]) * 524288.0);
 mp_pdp->Bin16ToHex(pwmfrq1, m_outgoing_packet);

 int pwmfrq2 = MathHelpers::Round((1.0/packet_data->pwm2_pwmfrq[1]) * 524288.0);
 mp_pdp->Bin16ToHex(pwmfrq2, m_outgoing_packet);
}

//-----------------------------------------------------------------------
void CControlApp::Build_EDITAB_PAR(EditTabPar* packet_data)
{
 mp_pdp->Bin8ToHex(packet_data->tab_id, m_outgoing_packet);
 mp_pdp->Bin8ToHex(packet_data->address, m_outgoing_packet);

 if (packet_data->tab_id == ETMT_VE_MAP || packet_data->tab_id == ETMT_VE2_MAP) //Volumetric efficiency map
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
   else if (packet_data->tab_id == ETMT_PWM1_MAP)
   {
    unsigned char value = m_splitAng ? MathHelpers::Round(packet_data->table_data[i] * AA_MAPS_M_FACTOR) : MathHelpers::Round((packet_data->table_data[i]*255.0f) / 100.0f);
    mp_pdp->Bin8ToHex(value, m_outgoing_packet);
   }
   else if (packet_data->tab_id == ETMT_PWM2_MAP)
   {
    unsigned char value = MathHelpers::Round((packet_data->table_data[i]*255.0f) / 100.0f);
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
   else if (packet_data->tab_id == ETMT_IDLR_MAP || packet_data->tab_id == ETMT_IDLC_MAP || packet_data->tab_id == ETMT_THRASS_MAP)
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
   else if (packet_data->tab_id == ETMT_TPSZON_MAP)
   {
    unsigned char value = MathHelpers::Round(packet_data->table_data[i] * TPSZON_MAPS_M_FACTOR);
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
   else if (packet_data->tab_id == ETMT_IACMAT_MAP)
   {
    signed char value = MathHelpers::Round(packet_data->table_data[i] * 4.0f);
    mp_pdp->Bin8ToHex(value, m_outgoing_packet);
   }
   else if (packet_data->tab_id == ETMT_CYLMULT_MAP)
   {
    unsigned char value = MathHelpers::Round((packet_data->table_data[i] - 0.5f) * 256.0f);
    mp_pdp->Bin8ToHex(value, m_outgoing_packet);
   }
   else if (packet_data->tab_id == ETMT_CYLADD_MAP)
   {
    signed char value = MathHelpers::Round(packet_data->table_data[i] / 0.0256f);
    mp_pdp->Bin8ToHex(value, m_outgoing_packet);
   }
   else if (packet_data->tab_id == ETMT_AEMAP_MAP)
   {
    unsigned char value = MathHelpers::Round((packet_data->address>=INJ_AE_MAP_LOOKUP_TABLE_SIZE)?(packet_data->table_data[i]*AEMAPB_MAPS_M_FACTOR):(packet_data->table_data[i]+AEMAPV_MAPS_ADDER));
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
 int bitNum = packet_data->mode ? 22 : 13;
 int bitIdx = 0;
 for(int i = 0; i < bitNum; ++i)
 {
  if (i==11 || i==12 || i==21) //BL or DE (special cases), TACH_O also (see DevDiagnostTabDlg.h for more information about enum's constants)
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

 int pwmfrq = MathHelpers::Round((1.0/packet_data->diag_frq) * 524288.0);
 mp_pdp->Bin16ToHex(pwmfrq, m_outgoing_packet);

 BYTE duty = MathHelpers::Round((packet_data->diag_duty / 100.0) * 255);
 mp_pdp->Bin8ToHex(duty, m_outgoing_packet);

 BYTE chan = packet_data->diag_chan;
 mp_pdp->Bin8ToHex(chan, m_outgoing_packet);
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
 flags|= (packet_data->bt_type << 5);        //write 5,6 bits

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
  unsigned char flags = ((packet_data->out[oi].logicFunc) << 4) | ((int)packet_data->out[oi].invers << 3) | ((int)packet_data->out[oi].use << 2) | ((int)packet_data->out[oi].invers_2 << 1) | ((int)packet_data->out[oi].invers_1);
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
 WRITEBIT8(inj_flags, 5, packet_data->inj_secinjrowswt); 
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

 int inj_begin_angle = MathHelpers::Round(packet_data->inj_timing[0] * PARINJTIM_MULT);
 mp_pdp->Bin16ToHex(inj_begin_angle, m_outgoing_packet);
 inj_begin_angle = MathHelpers::Round(packet_data->inj_timing[1] * PARINJTIM_MULT);
 mp_pdp->Bin16ToHex(inj_begin_angle, m_outgoing_packet);

 int inj_begin_angle_crk = MathHelpers::Round(packet_data->inj_timing_crk[0] * PARINJTIM_MULT);
 mp_pdp->Bin16ToHex(inj_begin_angle_crk, m_outgoing_packet);
 inj_begin_angle_crk = MathHelpers::Round(packet_data->inj_timing_crk[1] * PARINJTIM_MULT);
 mp_pdp->Bin16ToHex(inj_begin_angle_crk, m_outgoing_packet);

 unsigned char inj_anglespec = MAKEBYTE(packet_data->inj_anglespec[1], packet_data->inj_anglespec[0]);
 mp_pdp->Bin8ToHex(inj_anglespec, m_outgoing_packet);

 int fff = MathHelpers::Round((packet_data->fff_const / (1000.0f*60.0f))*65536.0f);
 mp_pdp->Bin16ToHex(fff, m_outgoing_packet);

 float discrete = (m_quartz_frq == 20000000 ? 3.2f : 4.0f);
 unsigned char inj_min_pw = MathHelpers::Round(((packet_data->inj_min_pw[1] * 1000.0f) / discrete) / 8.0f);
 mp_pdp->Bin8ToHex(inj_min_pw, m_outgoing_packet);
 inj_min_pw = MathHelpers::Round(((packet_data->inj_min_pw[0] * 1000.0f) / discrete) / 8.0f);
 mp_pdp->Bin8ToHex(inj_min_pw, m_outgoing_packet);
 //MAF consts
 mp_pdp->Bin32ToHex((unsigned long)packet_data->inj_maf_const[0], m_outgoing_packet);
 mp_pdp->Bin32ToHex((unsigned long)packet_data->inj_maf_const[1], m_outgoing_packet);
 mp_pdp->Bin32ToHex((unsigned long)packet_data->mafload_const, m_outgoing_packet);

 int inj_max_pw = MathHelpers::Round(packet_data->inj_max_pw[0] * (1000.0f / 3.2f));
 mp_pdp->Bin16ToHex(inj_max_pw, m_outgoing_packet);
 inj_max_pw = MathHelpers::Round(packet_data->inj_max_pw[1] * (1000.0f / 3.2f));
 mp_pdp->Bin16ToHex(inj_max_pw, m_outgoing_packet);
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
 unsigned char lam_flags = 0;
 WRITEBIT8(lam_flags, 0, packet_data->lam_htgdet);
 WRITEBIT8(lam_flags, 1, packet_data->lam_idlcorr);
 WRITEBIT8(lam_flags, 2, packet_data->lam_crkheat);
 mp_pdp->Bin8ToHex(lam_flags, m_outgoing_packet);
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

 int coldacc_mult = MathHelpers::Round(((packet_data->ae_coldacc_mult/100.0f) - 1.00f) * 128.0f);
 mp_pdp->Bin16ToHex(coldacc_mult, m_outgoing_packet);

 unsigned char ae_decay_time = packet_data->ae_decay_time;
 mp_pdp->Bin8ToHex(ae_decay_time, m_outgoing_packet);

 unsigned char ae_type = packet_data->ae_type;
 mp_pdp->Bin8ToHex(ae_type, m_outgoing_packet);

 unsigned char ae_time = packet_data->ae_time;
 mp_pdp->Bin8ToHex(ae_time, m_outgoing_packet);

 unsigned int b = MathHelpers::Round(packet_data->ae_ballance * 2.56f); //divide by 100% and multiply by 256
 if (b > 255)
  b = 255;
 unsigned char ae_ballance = b;
 mp_pdp->Bin8ToHex(ae_ballance, m_outgoing_packet);

 unsigned char mapdot_thrd = MathHelpers::Round(packet_data->ae_mapdot_thrd);
 mp_pdp->Bin8ToHex(mapdot_thrd, m_outgoing_packet);
}

//-----------------------------------------------------------------------
void CControlApp::Build_INJDRV_PAR(InjDrvPar* packet_data)
{
 unsigned char command = packet_data->ee_status; //save command
 WRITEBIT8(command, 7, packet_data->set_idx);
 WRITEBIT8(command, 1, packet_data->start_bldr);
 WRITEBIT8(command, 2, packet_data->reset_eeprom);
 WRITEBIT8(command, 3, packet_data->dev_address);
 WRITEBIT8(command, 4, packet_data->broadcast);

 mp_pdp->resetCRC();

 mp_pdp->Bin8ToHex(command, m_outgoing_packet);

 unsigned char direct_flags = 0;
 for(int i = 0; i < 8; ++i)
 {
  WRITEBIT8(direct_flags, i, packet_data->direct_flags);
 }
 mp_pdp->Bin8ToHex(direct_flags, m_outgoing_packet);

 BYTE lutabl_flags = 0;
 WRITEBIT8(lutabl_flags, 0, packet_data->m_peak_on_usetab);
 WRITEBIT8(lutabl_flags, 1, packet_data->m_peak_duty_usetab);
 WRITEBIT8(lutabl_flags, 2, packet_data->m_hold_duty_usetab);
 WRITEBIT8(lutabl_flags, 3, packet_data->m_peak_full_usetab);
 WRITEBIT8(lutabl_flags, 4, packet_data->m_pth_pause_usetab);

 mp_pdp->Bin8ToHex(lutabl_flags, m_outgoing_packet); //look up tables related flags

 BYTE gen_flags = 0;
 WRITEBIT8(gen_flags, 0, packet_data->m_tst_peak_pwm);
 WRITEBIT8(gen_flags, 1, packet_data->m_tst_hold_pwm);
 mp_pdp->Bin8ToHex(gen_flags, m_outgoing_packet); //general flags

 BYTE testch_sel = packet_data->m_testch_sel;
 mp_pdp->Bin8ToHex(testch_sel, m_outgoing_packet); //select channel for testing

 double frq =packet_data->m_testch_frq; 
 if (frq < 1.0)
  frq = 1.0;
 int testch_frq = MathHelpers::Round((1.0/frq) * 131072.0);
 mp_pdp->Bin16ToHex(testch_frq, m_outgoing_packet); //select channel for testing

 BYTE testch_duty = MathHelpers::Round((packet_data->m_testch_duty / 100.0) * 255);
 mp_pdp->Bin8ToHex(testch_duty, m_outgoing_packet); //select channel for testing

 int pwm_period = MathHelpers::Round(packet_data->m_pwm_period * 20.0f);
 mp_pdp->Bin16ToHex(pwm_period, m_outgoing_packet);

 int peak_duty = MathHelpers::Round((packet_data->m_peak_duty / 100.0f) * 4096.0f);
 mp_pdp->Bin16ToHex(peak_duty, m_outgoing_packet);

 int reserved = 0;
 mp_pdp->Bin16ToHex(reserved, m_outgoing_packet);

 int hold_duty = MathHelpers::Round((packet_data->m_hold_duty / 100.0f) * 4096.0f);
 mp_pdp->Bin16ToHex(hold_duty, m_outgoing_packet);

 int peak_on_time = MathHelpers::Round(packet_data->m_peak_on_time * 2.5f);
 mp_pdp->Bin16ToHex(peak_on_time, m_outgoing_packet);

 int peak_pwm_time = MathHelpers::Round(packet_data->m_peak_pwm_time * 2.5f);
 mp_pdp->Bin16ToHex(peak_pwm_time, m_outgoing_packet);

 int pth_pause = MathHelpers::Round(packet_data->m_pth_pause * 2.5f);
 mp_pdp->Bin16ToHex(pth_pause, m_outgoing_packet);

 for (int i = 0; i < LUTABSIZE; ++i)
 {
  int value = MathHelpers::Round(packet_data->m_peak_on_tab[i] * 2.5f);
  mp_pdp->Bin16ToHex(value, m_outgoing_packet);
 }

 for (int i = 0; i < LUTABSIZE; ++i)
 {
  int value = MathHelpers::Round((packet_data->m_peak_duty_tab[i] / 100.0f) * 4096.0f);
  mp_pdp->Bin16ToHex(value, m_outgoing_packet);
 }

 for (int i = 0; i < LUTABSIZE; ++i)
 {
  int value = MathHelpers::Round((packet_data->m_hold_duty_tab[i] / 100.0f) * 4096.0f);
  mp_pdp->Bin16ToHex(value, m_outgoing_packet);
 }

 for (int i = 0; i < LUTABSIZE; ++i)
 {
  int value = MathHelpers::Round(packet_data->m_peak_full_tab[i] * 2.5f);
  mp_pdp->Bin16ToHex(value, m_outgoing_packet);
 }

 for (int i = 0; i < LUTABSIZE; ++i)
 {
  int value = MathHelpers::Round(packet_data->m_pth_pause_tab[i] * 2.5f);
  mp_pdp->Bin16ToHex(value, m_outgoing_packet);
 }

 for (int i = 0; i < PWCORSIZE; ++i)
 {
  int value = MathHelpers::Round(packet_data->m_pwmul_tab[i] * (65536.0f / 100.0f));  //convert from %
  mp_pdp->Bin16ToHex(value, m_outgoing_packet);
 }

 for (int i = 0; i < PWCORSIZE; ++i)
 {
  int value = MathHelpers::Round(packet_data->m_pwadd_tab[i] * (2.5f));  //convert from us
  mp_pdp->Bin16ToHex(value, m_outgoing_packet);
 }

 //--------------------------------------------------------------------

 mp_pdp->Bin16ToHex(mp_pdp->getCRC(), m_outgoing_packet);
}

//-----------------------------------------------------------------------
void CControlApp::Build_LZIDBL_HS(LzidBLHS* packet_data)
{
 for(size_t i = 0; i < 5; ++i)
  m_outgoing_packet.push_back(packet_data->data[i]);
}

//-----------------------------------------------------------------------
void CControlApp::Build_INJDRV_ADR(InjDrvAdr* packet_data)
{
 BYTE dev_address = packet_data->dev_address;
 mp_pdp->resetCRC();
 mp_pdp->Bin8ToHex(dev_address, m_outgoing_packet);
 mp_pdp->Bin16ToHex(mp_pdp->getCRC(), m_outgoing_packet); //put CRC
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
void CControlApp::IgnoreNPackets(int n)
{
 ASSERT(n > 0); //zero is not safe with current implementation
 if (n > 0)
  InterlockedExchange(&m_ignore_n_packets, n);
}

//-----------------------------------------------------------------------
void CControlApp::SetSplitAngMode(bool mode)
{
 m_splitAng = mode;
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
  case UNIOUT_COND_AI5: return MathHelpers::Round(val * (1.0 / ADC_DISCRETE));
  case UNIOUT_COND_AI6: return MathHelpers::Round(val * (1.0 / ADC_DISCRETE));
  case UNIOUT_COND_AI7: return MathHelpers::Round(val * (1.0 / ADC_DISCRETE));
  case UNIOUT_COND_AI8: return MathHelpers::Round(val * (1.0 / ADC_DISCRETE));
  case UNIOUT_COND_GASV: return MathHelpers::Round(val);
  case UNIOUT_COND_IPW: return MathHelpers::Round((val * 1000.0f) / 3.2);
  case UNIOUT_COND_CE: return MathHelpers::Round(val);
  case UNIOUT_COND_OFTMR: return MathHelpers::Round(val * 100.0f);
  case UNIOUT_COND_LPTMR: return MathHelpers::Round(val * 100.0f);
  case UNIOUT_COND_GRTS:  return MathHelpers::Round(val * TEMP_PHYSICAL_MAGNITUDE_MULTIPLIER);
  case UNIOUT_COND_MAP2:  return MathHelpers::Round(val * MAP_PHYSICAL_MAGNITUDE_MULTIPLIER);
  case UNIOUT_COND_TMP2: return MathHelpers::Round(val * TEMP_PHYSICAL_MAGNITUDE_MULTIPLIER);
  case UNIOUT_COND_INPUT1: return MathHelpers::Round(val);
  case UNIOUT_COND_INPUT2: return MathHelpers::Round(val);
  case UNIOUT_COND_MAF:  return MathHelpers::Round(val * MAFS_MULT);
  case UNIOUT_COND_TPSDOT: return MathHelpers::Round(val); //%/s
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
  case UNIOUT_COND_AI5: return ((float)val) / (1.0f / ADC_DISCRETE);
  case UNIOUT_COND_AI6: return ((float)val) / (1.0f / ADC_DISCRETE);
  case UNIOUT_COND_AI7: return ((float)val) / (1.0f / ADC_DISCRETE);
  case UNIOUT_COND_AI8: return ((float)val) / (1.0f / ADC_DISCRETE);
  case UNIOUT_COND_GASV: return (float)val;
  case UNIOUT_COND_IPW: return (val * 3.2f) / 1000.0f;
  case UNIOUT_COND_CE: return (float)val;
  case UNIOUT_COND_OFTMR: return ((float)val) / 100.0f;
  case UNIOUT_COND_LPTMR: return ((float)val) / 100.0f;
  case UNIOUT_COND_GRTS:  return (((float)val) / TEMP_PHYSICAL_MAGNITUDE_MULTIPLIER);
  case UNIOUT_COND_MAP2:  return (((float)val) / MAP_PHYSICAL_MAGNITUDE_MULTIPLIER);
  case UNIOUT_COND_TMP2: return ((float)val) / TEMP_PHYSICAL_MAGNITUDE_MULTIPLIER;
  case UNIOUT_COND_INPUT1: return (float)val;
  case UNIOUT_COND_INPUT2: return (float)val;
  case UNIOUT_COND_MAF:  return (((float)val) / MAFS_MULT);
  case UNIOUT_COND_TPSDOT: return (float)val;
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
