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
#include <vector>
#include "ControlApp.h"
#include "ccomport.h"
#include "common/MathHelpers.h"
#include "FirmwareMapsDataHolder.h"
#include "NumericConv.h"
#include "ufcodes.h"

using namespace SECU3IO;

//-----------------------------------------------------------------------
CControlApp::CControlApp()
: m_adc_discrete(ADC_DISCRETE)
, m_angle_multiplier(ANGLE_MULTIPLAYER)
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
, m_uart_speed(CBR_9600)
, m_ingoing_packet("")
, m_packets_parse_state(0)
, m_hTimer(NULL)
, m_outgoing_packet("")
, mp_csection(NULL)
, m_work_state(false)
{
 m_pPackets = new Packets();
 memset(&m_recepted_packet,0,sizeof(SECU3Packet));
 memset(&m_pending_packets,0,sizeof(SECU3Packet) * PENDING_PACKETS_QUEUE_SIZE);

 mp_csection = new CSECTION;
 InitializeCriticalSection(GetSyncObject());
}

//-----------------------------------------------------------------------
CControlApp::~CControlApp()
{
 delete m_pPackets;
 DeleteCriticalSection(GetSyncObject());
 delete mp_csection;
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

 m_ingoing_packet  = "";
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

 //Пакет(ы) содержит 0x0 символы.
 _ASSERTE((!memchr(i_buff, '\0', i_size)) && "0x0 characters are not allowed in SECU-3 packets.\
 Please, check firmware code for this problem.");

 while(*p!=0)
 {
  switch(m_packets_parse_state) //я люблю автоматное программирование...
  {
   case 0:       //search '@'
	if (*p=='@')
	{
	 m_ingoing_packet+=*p;
	 m_packets_parse_state = 1;
	}
	break;
   case 1:       //wait '\r'
	if (*p=='\r')
	{
	 m_ingoing_packet+=*p;
	 m_pPackets->push_back(m_ingoing_packet);
	 m_ingoing_packet = "";
	 m_packets_parse_state = 0;
	}
	else
	{
	 m_ingoing_packet+=*p;
	}
	break;
  }//switch
  ++p;
 };

 return m_pPackets->size();
}

//-----------------------------------------------------------------------
bool CControlApp::Parse_SENSOR_DAT(const BYTE* raw_packet)
{
 SECU3IO::SensorDat& m_SensorDat = m_recepted_packet.m_SensorDat;

 if (strlen((char*)raw_packet)!=49)  //размер пакета без сигнального символа, дескриптора
  return false;

 //частота вращения двигателя
 if (false == CNumericConv::Hex16ToBin(raw_packet,&m_SensorDat.frequen))
  return false;
 raw_packet+=4;

 //давление во впускном коллекторе
 int pressure = 0;
 if (false == CNumericConv::Hex16ToBin(raw_packet,&pressure))
  return false;

 m_SensorDat.pressure = ((float)pressure) / MAP_PHYSICAL_MAGNITUDE_MULTIPLAYER;
 raw_packet+=4;

 //напряжение бортовой сети
 int voltage = 0;
 if (false == CNumericConv::Hex16ToBin(raw_packet,&voltage))
  return false;
 m_SensorDat.voltage = ((float)voltage) / UBAT_PHYSICAL_MAGNITUDE_MULTIPLAYER;
 raw_packet+=4;

 //Температура охлаждающей жидкости
 int temperature = 0;
 if (false == CNumericConv::Hex16ToBin(raw_packet,&temperature,true))
  return false;
 m_SensorDat.temperat = ((float)temperature) / TEMP_PHYSICAL_MAGNITUDE_MULTIPLAYER;
 raw_packet+=4;

 //Текущий УОЗ (число со знаком)
 int adv_angle = 0;
 if (false == CNumericConv::Hex16ToBin(raw_packet,&adv_angle,true))
  return false;
 m_SensorDat.adv_angle = ((float)adv_angle) / m_angle_multiplier;
 raw_packet+=4;

 //Уровень детонации двигателя
 int knock_k = 0;
 if (false == CNumericConv::Hex16ToBin(raw_packet,&knock_k))
  return false;
 m_SensorDat.knock_k = ((float)knock_k) * m_adc_discrete;
 raw_packet+=4;

 //Корректировка УОЗ при детонации
 int knock_retard = 0;
 if (false == CNumericConv::Hex16ToBin(raw_packet,&knock_retard, true))
  return false;
 m_SensorDat.knock_retard = ((float)knock_retard) / m_angle_multiplier;
 raw_packet+=4;

 //Расход воздуха
 if (false == CNumericConv::Hex8ToBin(raw_packet,&m_SensorDat.air_flow))
  return false;
 raw_packet+=2;

 //Байт с флажками
 unsigned char byte = 0;
 if (false == CNumericConv::Hex8ToBin(raw_packet,&byte))
  return false;
 raw_packet+=2;

 //Состояние клапана ЭПХХ, Состояние дроссельной заслонки, Состояние газового клапана
 //Состояние клапана ЭМР, Состояние лампы "CE"
 m_SensorDat.ephh_valve = (byte & (1 << 0)) != 0;
 m_SensorDat.carb       = (byte & (1 << 1)) != 0;
 m_SensorDat.gas        = (byte & (1 << 2)) != 0;
 m_SensorDat.epm_valve  = (byte & (1 << 3)) != 0;
 m_SensorDat.ce_state   = (byte & (1 << 4)) != 0;
 m_SensorDat.cool_fan   = (byte & (1 << 5)) != 0;
 m_SensorDat.st_block   = (byte & (1 << 6)) != 0;

 //TPS sensor
 unsigned char tps = 0;
 if (false == CNumericConv::Hex8ToBin(raw_packet,&tps))
  return false;
 m_SensorDat.tps = ((float)tps) / TPS_PHYSICAL_MAGNITUDE_MULTIPLAYER;
 raw_packet+=2;

 //ADD_I1 input
 int add_i1_v = 0;
 if (false == CNumericConv::Hex16ToBin(raw_packet,&add_i1_v))
  return false;
 m_SensorDat.add_i1 = ((float)add_i1_v) * m_adc_discrete;
 raw_packet+=4;

 //ADD_I2 input
 int add_i2_v = 0;
 if (false == CNumericConv::Hex16ToBin(raw_packet,&add_i2_v))
  return false;
 m_SensorDat.add_i2 = ((float)add_i2_v) * m_adc_discrete;
 raw_packet+=4;

 //Биты ошибок СЕ
 int ce_errors = 0;
 if (false == CNumericConv::Hex16ToBin(raw_packet, &ce_errors))
  return false;
 m_SensorDat.ce_errors = ce_errors;
 raw_packet+=4;

 //Choke position
 unsigned char choke_pos = 0;
 if (false == CNumericConv::Hex8ToBin(raw_packet, &choke_pos))
  return false;
 m_SensorDat.choke_pos = ((float)choke_pos) / CHOKE_PHYSICAL_MAGNITUDE_MULTIPLAYER;
 raw_packet+=2;

 if (*raw_packet!='\r')
  return false;

 return true;
}

//-----------------------------------------------------------------------
bool CControlApp::Parse_DBGVAR_DAT(const BYTE* raw_packet)
{
 SECU3IO::DbgvarDat& m_DbgvarDat = m_recepted_packet.m_DbgvarDat;

 if (strlen((char*)raw_packet)!=17)  //размер пакета без сигнального символа, дескриптора
  return false;

 //переменная 1
 if (false == CNumericConv::Hex16ToBin(raw_packet, &m_DbgvarDat.var1))
  return false;
 raw_packet+=4;

 //переменная 2
 if (false == CNumericConv::Hex16ToBin(raw_packet, &m_DbgvarDat.var2))
  return false;
 raw_packet+=4;

 //переменная 3
 if (false == CNumericConv::Hex16ToBin(raw_packet, &m_DbgvarDat.var3))
  return false;
 raw_packet+=4;

 //переменная 4
 if (false == CNumericConv::Hex16ToBin(raw_packet, &m_DbgvarDat.var4))
  return false;
 raw_packet+=4;

 if (*raw_packet!='\r')
  return false;

 return true;
}

//-----------------------------------------------------------------------
bool CControlApp::Parse_FNNAME_DAT(const BYTE* raw_packet)
{
 SECU3IO::FnNameDat& m_FnNameDat = m_recepted_packet.m_FnNameDat;

 if (strlen((char*)raw_packet)!=21)  //размер пакета без сигнального символа, дескриптора
 {
  return false;
 }

 //Общее кол-во наборов (семейств характеристик)
 if (false == CNumericConv::Hex8ToBin(raw_packet,&m_FnNameDat.tables_num))
  return false;
 raw_packet+=2;

 //номер этого набора характеристик
 if (false == CNumericConv::Hex8ToBin(raw_packet,&m_FnNameDat.index))
  return false;
 raw_packet+=2;

 //имя этого набора характеристик
 char* p;
 if (NULL==(p = strchr((char*)raw_packet, '\r')))
  return false;
 *p = 0;

 strcpy(m_FnNameDat.name, (const char*)raw_packet);

 //Заменяем символы FF на 0x20
 size_t count = strlen(m_FnNameDat.name);
 for(size_t i = 0; i < count; ++i)
  if (((unsigned char)m_FnNameDat.name[i]) == 0xFF)
   m_FnNameDat.name[i] = 0x20;

 return true;
}


//-----------------------------------------------------------------------
bool CControlApp::Parse_STARTR_PAR(const BYTE* raw_packet)
{
 SECU3IO::StartrPar& m_StartrPar = m_recepted_packet.m_StartrPar;

 if (strlen((char*)raw_packet)!=9)  //размер пакета без сигнального символа, дескриптора
  return false;

 //Обороты при которых стартер будет выключен
 if (false == CNumericConv::Hex16ToBin(raw_packet,&m_StartrPar.starter_off))
  return false;
 raw_packet+=4;

 //Обороты перехода с пусковой карты
 if (false == CNumericConv::Hex16ToBin(raw_packet,&m_StartrPar.smap_abandon))
  return false;
 raw_packet+=4;

 if (*raw_packet!='\r')
  return false;

 return true;
}


//-----------------------------------------------------------------------
bool CControlApp::Parse_ANGLES_PAR(const BYTE* raw_packet)
{
 SECU3IO::AnglesPar& m_AnglesPar = m_recepted_packet.m_AnglesPar;

 if (strlen((char*)raw_packet)!=22)  //размер пакета без сигнального символа, дескриптора
  return false;

 //Максимальный, допустимый УОЗ (число со знаком)
 int max_angle;
 if (false == CNumericConv::Hex16ToBin(raw_packet,&max_angle,true))
  return false;
 raw_packet+=4;
 m_AnglesPar.max_angle = ((float)max_angle) / m_angle_multiplier;

 //Минимальный, допустимый УОЗ (число со знаком)
 int  min_angle;
 if (false == CNumericConv::Hex16ToBin(raw_packet,&min_angle,true))
  return false;
 raw_packet+=4;
 m_AnglesPar.min_angle = ((float)min_angle) / m_angle_multiplier;

 //Октан-коррекция УОЗ (число со знаком)
 int angle_corr;
 if (false == CNumericConv::Hex16ToBin(raw_packet,&angle_corr,true))
  return false;
 raw_packet+=4;
 m_AnglesPar.angle_corr = ((float)angle_corr) / m_angle_multiplier;

 //Скорость уменьшения УОЗ (число со знаком)
 int dec_spead;
 if (false == CNumericConv::Hex16ToBin(raw_packet,&dec_spead,true))
  return false;
 raw_packet+=4;
 m_AnglesPar.dec_spead = ((float)dec_spead) / m_angle_multiplier;

//Скорость увеличения УОЗ (число со знаком)
 int inc_spead;
 if (false == CNumericConv::Hex16ToBin(raw_packet,&inc_spead,true))
  return false;
 raw_packet+=4;
 m_AnglesPar.inc_spead = ((float)inc_spead) / m_angle_multiplier;

 //Признак нулевого УОЗ
 if (false == CNumericConv::Hex4ToBin(*raw_packet, &m_AnglesPar.zero_adv_ang))
  return false;
 raw_packet+=1;

 if (*raw_packet!='\r')
  return false;

 return true;
}

//-----------------------------------------------------------------------
bool CControlApp::Parse_FUNSET_PAR(const BYTE* raw_packet)
{
 SECU3IO::FunSetPar& m_FunSetPar = m_recepted_packet.m_FunSetPar;

 if (strlen((char*)raw_packet)!=29)  //размер пакета без сигнального символа, дескриптора
  return false;

 //Номер семейства характеристик используемого для бензина
 if (false == CNumericConv::Hex8ToBin(raw_packet,&m_FunSetPar.fn_benzin))
  return false;
 raw_packet+=2;

 //Номер семейства характеристик используемого для газа
 if (false == CNumericConv::Hex8ToBin(raw_packet,&m_FunSetPar.fn_gas))
  return false;
 raw_packet+=2;

 //Нижнее значение давления по оси ДАД
 int map_lower_pressure = 0;
 if (false == CNumericConv::Hex16ToBin(raw_packet,&map_lower_pressure))
  return false;
 raw_packet+=4;
 m_FunSetPar.map_lower_pressure = ((float)map_lower_pressure) / MAP_PHYSICAL_MAGNITUDE_MULTIPLAYER;

 //Верхнее значение давления по оси ДАД
 int map_upper_pressure = 0;
 if (false == CNumericConv::Hex16ToBin(raw_packet,&map_upper_pressure))
  return false;
 raw_packet+=4;
 m_FunSetPar.map_upper_pressure = ((float)map_upper_pressure) / MAP_PHYSICAL_MAGNITUDE_MULTIPLAYER;

 //Смещение кривой ДАД
 int map_curve_offset = 0;
 if (false == CNumericConv::Hex16ToBin(raw_packet, &map_curve_offset, true))
  return false;
 raw_packet+=4;
 m_FunSetPar.map_curve_offset = ((float)map_curve_offset) * m_adc_discrete;

 //Наклон кривой ДАД
 int map_curve_gradient = 0;
 if (false == CNumericConv::Hex16ToBin(raw_packet, &map_curve_gradient, true))
  return false;
 raw_packet+=4;
 m_FunSetPar.map_curve_gradient = ((float)map_curve_gradient) / (MAP_PHYSICAL_MAGNITUDE_MULTIPLAYER * m_adc_discrete * 128.0f);

 //Смещение кривой ДПДЗ
 int tps_curve_offset = 0;
 if (false == CNumericConv::Hex16ToBin(raw_packet, &tps_curve_offset, true))
  return false;
 raw_packet+=4;
 m_FunSetPar.tps_curve_offset = ((float)tps_curve_offset) * m_adc_discrete;

 //Наклон кривой ДПДЗ
 int tps_curve_gradient = 0;
 if (false == CNumericConv::Hex16ToBin(raw_packet, &tps_curve_gradient, true))
  return false;
 raw_packet+=4;
 m_FunSetPar.tps_curve_gradient = ((float)tps_curve_gradient) / ((TPS_PHYSICAL_MAGNITUDE_MULTIPLAYER*64) * m_adc_discrete * 128.0f);

 if (*raw_packet!='\r')
  return false;

 return true;
}


//-----------------------------------------------------------------------
bool CControlApp::Parse_IDLREG_PAR(const BYTE* raw_packet)
{
 SECU3IO::IdlRegPar& m_IdlRegPar = m_recepted_packet.m_IdlRegPar;

 if (strlen((char*)raw_packet)!=30)  //размер пакета без сигнального символа, дескриптора
  return false;

 //признак использования регулятора
 if (false == CNumericConv::Hex4ToBin(*raw_packet,&m_IdlRegPar.idl_regul))
  return false;
 raw_packet+=1;

 //Коэффициент регулятора при  положительной ошибке (число со знаком)
 int ifac1;
 if (false == CNumericConv::Hex16ToBin(raw_packet,&ifac1,true))
  return false;
 raw_packet+=4;
 m_IdlRegPar.ifac1 = ((float)ifac1) / ANGLE_MULTIPLAYER;

 //Коэффициент регулятора при  отрицательной ошибке (число со знаком)
 int ifac2;
 if (false == CNumericConv::Hex16ToBin(raw_packet,&ifac2,true))
  return false;
 raw_packet+=4;
 m_IdlRegPar.ifac2 = ((float)ifac2) / ANGLE_MULTIPLAYER;

 //Зона нечувствительности регулятора
 if (false == CNumericConv::Hex16ToBin(raw_packet,&m_IdlRegPar.MINEFR))
  return false;
 raw_packet+=4;

 //Поддерживаемые обороты
 if (false == CNumericConv::Hex16ToBin(raw_packet,&m_IdlRegPar.idling_rpm))
  return false;
 raw_packet+=4;

 //Минимальный УОЗ (число со знаком)
 int min_angle;
 if (false == CNumericConv::Hex16ToBin(raw_packet,&min_angle,true))
  return false;
 raw_packet+=4;
 m_IdlRegPar.min_angle = ((float)min_angle) / m_angle_multiplier;

 //Максимальный УОЗ (число со знаком)
 int max_angle;
 if (false == CNumericConv::Hex16ToBin(raw_packet,&max_angle,true))
  return false;
 raw_packet+=4;
 m_IdlRegPar.max_angle = ((float)max_angle) / m_angle_multiplier;

 //Порог включения регулятора ХХ по температуре (число со знаком)
 int turn_on_temp = 0;
 if (false == CNumericConv::Hex16ToBin(raw_packet,&turn_on_temp,true))
  return false;
 raw_packet+=4;
 m_IdlRegPar.turn_on_temp = ((float)turn_on_temp) / TEMP_PHYSICAL_MAGNITUDE_MULTIPLAYER;

 if (*raw_packet!='\r')
  return false;

 return true;
}

//-----------------------------------------------------------------------
bool CControlApp::Parse_CARBUR_PAR(const BYTE* raw_packet)
{
 SECU3IO::CarburPar& m_CarburPar = m_recepted_packet.m_CarburPar;

 if (strlen((char*)raw_packet)!=26)  //размер пакета без сигнального символа, дескриптора
  return false;

 //Нижний порог ЭПХХ (бензин)
 if (false == CNumericConv::Hex16ToBin(raw_packet, &m_CarburPar.ephh_lot))
  return false;
 raw_packet+=4;

 //Верхний порог ЭПХХ (бензин)
 if (false == CNumericConv::Hex16ToBin(raw_packet, &m_CarburPar.ephh_hit))
  return false;
 raw_packet+=4;

 //Признак инверсии концевика карбюратора
 if (false == CNumericConv::Hex4ToBin(*raw_packet, &m_CarburPar.carb_invers))
  return false;
 raw_packet+=1;

 //Порог разрежения ЭМР
 int epm_on_threshold = 0;
 if (false == CNumericConv::Hex16ToBin(raw_packet, &epm_on_threshold, true))
  return false;
 raw_packet+=4;
 m_CarburPar.epm_ont = ((float)epm_on_threshold) / MAP_PHYSICAL_MAGNITUDE_MULTIPLAYER;

 //Нижний порог ЭПХХ (газ)
 if (false == CNumericConv::Hex16ToBin(raw_packet, &m_CarburPar.ephh_lot_g))
  return false;
 raw_packet+=4;

 //Верхний порог ЭПХХ (газ)
 if (false == CNumericConv::Hex16ToBin(raw_packet, &m_CarburPar.ephh_hit_g))
  return false;
 raw_packet+=4;

 //Задержка выключения клапана ЭПХХ
 unsigned char shutoff_delay;
 if (false == CNumericConv::Hex8ToBin(raw_packet, &shutoff_delay))
  return false;
 raw_packet+=2;
 m_CarburPar.shutoff_delay = ((float)shutoff_delay) / 100.0f; //переводим в секунды

 //Порог переключения в режим ХХ по ДПДЗ
 unsigned char tps_threshold;
 if (false == CNumericConv::Hex8ToBin(raw_packet, &tps_threshold))
  return false;
 raw_packet+=2;
 m_CarburPar.tps_threshold = ((float)tps_threshold) / TPS_PHYSICAL_MAGNITUDE_MULTIPLAYER;

 if (*raw_packet!='\r')
   return false;

 return true;
}

//-----------------------------------------------------------------------
bool CControlApp::Parse_TEMPER_PAR(const BYTE* raw_packet)
{
 SECU3IO::TemperPar& m_TemperPar = m_recepted_packet.m_TemperPar;

 if (strlen((char*)raw_packet)!=12)  //размер пакета без сигнального символа, дескриптора
  return false;

 //Признак комплектации ДТОЖ (использования ДТОЖ)
 if (false == CNumericConv::Hex4ToBin(*raw_packet,&m_TemperPar.tmp_use))
  return false;
 raw_packet+=1;

 //Флаг использования ШИМ для управления вентилятором охлаждения двигателя
 if (false == CNumericConv::Hex4ToBin(*raw_packet,&m_TemperPar.vent_pwm))
  return false;
 raw_packet+=1;

 //Флаг использования таблицы для задания зависимости температуры от напрящения ДТОЖ
 if (false == CNumericConv::Hex4ToBin(*raw_packet,&m_TemperPar.cts_use_map))
  return false;
 raw_packet+=1;

 //Для удобства и повышения скорости обработки SECU-3 оперирует с температурой представленной
 //дискретами АЦП (как измеренное значение прямо с датчика)

 //Порог включения вентилятора (число со знаком)
 int vent_on = 0;
 if (false == CNumericConv::Hex16ToBin(raw_packet,&vent_on,true))
  return false;
 raw_packet+=4;
 m_TemperPar.vent_on = ((float)vent_on) / TEMP_PHYSICAL_MAGNITUDE_MULTIPLAYER;

 //Порог выключения вентилятора (число со знаком)
 int vent_off = 0;
 if (false == CNumericConv::Hex16ToBin(raw_packet,&vent_off,true))
  return false;
 raw_packet+=4;
 m_TemperPar.vent_off = ((float)vent_off) / TEMP_PHYSICAL_MAGNITUDE_MULTIPLAYER;

 if (*raw_packet!='\r')
  return false;

 return true;
}

//-----------------------------------------------------------------------
bool CControlApp::Parse_ADCRAW_DAT(const BYTE* raw_packet)
{
 SECU3IO::RawSensDat& m_RawSensDat = m_recepted_packet.m_RawSensDat;

 if (strlen((char*)raw_packet)!=29)  //размер пакета без сигнального символа, дескриптора
  return false;

 //MAP sensor
 signed int map = 0;
 if (false == CNumericConv::Hex16ToBin(raw_packet,&map,true))
  return false;
 raw_packet+=4;
 m_RawSensDat.map_value = map * m_adc_discrete;

 //напряжение бортовой сети
 signed int ubat = 0;
 if (false == CNumericConv::Hex16ToBin(raw_packet,&ubat,true))
  return false;
 raw_packet+=4;
 m_RawSensDat.ubat_value = ubat * m_adc_discrete;

 //температура ОЖ (ДТОЖ)
 signed int temp = 0;
 if (false == CNumericConv::Hex16ToBin(raw_packet,&temp,true))
  return false;
 raw_packet+=4;
 m_RawSensDat.temp_value = temp * m_adc_discrete;

 //Уровень сигнала детонации
 signed int knock = 0;
 if (false == CNumericConv::Hex16ToBin(raw_packet,&knock,true))
  return false;
 raw_packet+=4;
 m_RawSensDat.knock_value = knock * m_adc_discrete;

 //Throttle position sensor
 signed int tps = 0;
 if (false == CNumericConv::Hex16ToBin(raw_packet,&tps,true))
  return false;
 raw_packet+=4;
 m_RawSensDat.tps_value = tps * m_adc_discrete;

 //ADD_I1 input
 signed int add_i1 = 0;
 if (false == CNumericConv::Hex16ToBin(raw_packet,&add_i1,true))
  return false;
 raw_packet+=4;
 m_RawSensDat.add_i1_value = add_i1 * m_adc_discrete;

 //ADD_I2 input
 signed int add_i2 = 0;
 if (false == CNumericConv::Hex16ToBin(raw_packet,&add_i2,true))
  return false;
 raw_packet+=4;
 m_RawSensDat.add_i2_value = add_i2 * m_adc_discrete;

 if (*raw_packet!='\r')
  return false;

 return true;
}


//-----------------------------------------------------------------------
//note: for more information see AVR120 application note.
bool CControlApp::Parse_ADCCOR_PAR(const BYTE* raw_packet)
{
 SECU3IO::ADCCompenPar& m_ADCCompenPar = m_recepted_packet.m_ADCCompenPar;

 if (strlen((char*)raw_packet)!=73)  //размер пакета без сигнального символа, дескриптора
  return false;

 signed int map_adc_factor = 0;
 if (false == CNumericConv::Hex16ToBin(raw_packet,&map_adc_factor,true))
  return false;
 raw_packet+=4;
 m_ADCCompenPar.map_adc_factor = ((float)map_adc_factor) / 16384;

 signed long map_adc_correction = 0;
 if (false == CNumericConv::Hex32ToBin(raw_packet,&map_adc_correction))
  return false;
 raw_packet+=8;
 m_ADCCompenPar.map_adc_correction = ((((float)map_adc_correction)/16384.0f) - 0.5f) / m_ADCCompenPar.map_adc_factor;
 m_ADCCompenPar.map_adc_correction*=m_adc_discrete; //в вольты

 signed int ubat_adc_factor = 0;
 if (false == CNumericConv::Hex16ToBin(raw_packet,&ubat_adc_factor,true))
  return false;
 raw_packet+=4;
 m_ADCCompenPar.ubat_adc_factor = ((float)ubat_adc_factor) / 16384;

 signed long ubat_adc_correction = 0;
 if (false == CNumericConv::Hex32ToBin(raw_packet,&ubat_adc_correction))
  return false;

 raw_packet+=8;
 m_ADCCompenPar.ubat_adc_correction = ((((float)ubat_adc_correction)/16384.0f) - 0.5f) / m_ADCCompenPar.ubat_adc_factor;
 m_ADCCompenPar.ubat_adc_correction*=m_adc_discrete; //в вольты

 signed int temp_adc_factor = 0;
 if (false == CNumericConv::Hex16ToBin(raw_packet,&temp_adc_factor,true))
  return false;
 raw_packet+=4;
 m_ADCCompenPar.temp_adc_factor = ((float)temp_adc_factor) / 16384;

 signed long temp_adc_correction = 0;
 if (false == CNumericConv::Hex32ToBin(raw_packet,&temp_adc_correction))
  return false;
 raw_packet+=8;
 m_ADCCompenPar.temp_adc_correction = ((((float)temp_adc_correction)/16384.0f) - 0.5f) / m_ADCCompenPar.temp_adc_factor;
 m_ADCCompenPar.temp_adc_correction*=m_adc_discrete; //в вольты
 
 //TPS sensor
 signed int tps_adc_factor = 0;
 if (false == CNumericConv::Hex16ToBin(raw_packet,&tps_adc_factor,true))
  return false;
 raw_packet+=4;
 m_ADCCompenPar.tps_adc_factor = ((float)tps_adc_factor) / 16384;

 signed long tps_adc_correction = 0;
 if (false == CNumericConv::Hex32ToBin(raw_packet,&tps_adc_correction))
  return false;
 raw_packet+=8;
 m_ADCCompenPar.tps_adc_correction = ((((float)tps_adc_correction)/16384.0f) - 0.5f) / m_ADCCompenPar.tps_adc_factor;
 m_ADCCompenPar.tps_adc_correction*=m_adc_discrete; //в вольты

 //ADD_IO1 input
 signed int ai1_adc_factor = 0;
 if (false == CNumericConv::Hex16ToBin(raw_packet,&ai1_adc_factor,true))
  return false;
 raw_packet+=4;
 m_ADCCompenPar.ai1_adc_factor = ((float)ai1_adc_factor) / 16384;

 signed long ai1_adc_correction = 0;
 if (false == CNumericConv::Hex32ToBin(raw_packet,&ai1_adc_correction))
  return false;
 raw_packet+=8;
 m_ADCCompenPar.ai1_adc_correction = ((((float)ai1_adc_correction)/16384.0f) - 0.5f) / m_ADCCompenPar.ai1_adc_factor;
 m_ADCCompenPar.ai1_adc_correction*=m_adc_discrete; //в вольты

 //ADD_IO2 input
 signed int ai2_adc_factor = 0;
 if (false == CNumericConv::Hex16ToBin(raw_packet,&ai2_adc_factor,true))
  return false;
 raw_packet+=4;
 m_ADCCompenPar.ai2_adc_factor = ((float)ai2_adc_factor) / 16384;

 signed long ai2_adc_correction = 0;
 if (false == CNumericConv::Hex32ToBin(raw_packet,&ai2_adc_correction))
  return false;
 raw_packet+=8;
 m_ADCCompenPar.ai2_adc_correction = ((((float)ai2_adc_correction)/16384.0f) - 0.5f) / m_ADCCompenPar.ai2_adc_factor;
 m_ADCCompenPar.ai2_adc_correction*=m_adc_discrete; //в вольты

 if (*raw_packet!='\r')
  return false;

 return true;
}

//-----------------------------------------------------------------------
bool CControlApp::Parse_CKPS_PAR(const BYTE* raw_packet)
{
 SECU3IO::CKPSPar& m_CKPSPar = m_recepted_packet.m_CKPSPar;

 if (strlen((char*)raw_packet)!=14)  //размер пакета без сигнального символа, дескриптора
  return false;

 //Тип фронта ДПКВ
 if (false == CNumericConv::Hex4ToBin(*raw_packet,&m_CKPSPar.ckps_edge_type))
  return false;
 raw_packet+=1;

 //Тип фронта ДНО (вход REF_S)
 if (false == CNumericConv::Hex4ToBin(*raw_packet,&m_CKPSPar.ref_s_edge_type))
  return false;
 raw_packet+=1;

 //Количество зубьев до в.м.т.
 if (false == CNumericConv::Hex8ToBin(raw_packet,&m_CKPSPar.ckps_cogs_btdc))
  return false;
 raw_packet+=2;

 //Длительность импульса запуска коммутаторов в зубьях шкива
 if (false == CNumericConv::Hex8ToBin(raw_packet,&m_CKPSPar.ckps_ignit_cogs))
  return false;
 raw_packet+=2;

 //Кол-во цилиндров двигателя
 if (false == CNumericConv::Hex8ToBin(raw_packet,&m_CKPSPar.ckps_engine_cyl))
  return false;
 raw_packet+=2;

 //Флаг объединения выходов зажигания
 if (false == CNumericConv::Hex4ToBin(*raw_packet, &m_CKPSPar.ckps_merge_ign_outs))
  return false;
 raw_packet+=1;

 //Кол-во зубьев задающего шкива, включая пропущенные
 if (false == CNumericConv::Hex8ToBin(raw_packet, &m_CKPSPar.ckps_cogs_num))
  return false;
 raw_packet+=2;

 //Кол-во пропущенных зубьев задающего шкива (допустимые значения: 0, 1, 2)
 if (false == CNumericConv::Hex8ToBin(raw_packet, &m_CKPSPar.ckps_miss_num))
  return false;
 raw_packet+=2;

 if (*raw_packet!='\r')
  return false;

 return true;
}

//-----------------------------------------------------------------------
bool CControlApp::Parse_OP_COMP_NC(const BYTE* raw_packet)
{
 SECU3IO::OPCompNc& m_OPCompNc = m_recepted_packet.m_OPCompNc;

 if (strlen((char*)raw_packet)!=5)  //размер пакета без сигнального символа, дескриптора
  return false;

 //Код завершенной операции
 if (false == CNumericConv::Hex8ToBin(raw_packet, &m_OPCompNc.opdata))
  return false;
 raw_packet+=2;
 if (false == CNumericConv::Hex8ToBin(raw_packet, &m_OPCompNc.opcode))
  return false;
 raw_packet+=2;

 if (*raw_packet!='\r')
  return false;

 return true;
}

//-----------------------------------------------------------------------
bool CControlApp::Parse_KNOCK_PAR(const BYTE* raw_packet)
{
 SECU3IO::KnockPar& m_KnockPar = m_recepted_packet.m_KnockPar;

 if (strlen((char*)raw_packet)!=(14+18))  //размер пакета без сигнального символа, дескриптора
  return false;

 //Разрешен/запрещен
 if (false == CNumericConv::Hex4ToBin(*raw_packet,&m_KnockPar.knock_use_knock_channel))
  return false;
 raw_packet+=1;

 //Частота ПФ
 unsigned char knock_bpf_frequency;
 if (false == CNumericConv::Hex8ToBin(raw_packet,&knock_bpf_frequency))
  return false;
 m_KnockPar.knock_bpf_frequency = knock_bpf_frequency;
 raw_packet+=2;

 //Начало фазового окна
 int  knock_k_wnd_begin_angle;
 if (false == CNumericConv::Hex16ToBin(raw_packet,&knock_k_wnd_begin_angle,true))
  return false;
 m_KnockPar.knock_k_wnd_begin_angle = ((float)knock_k_wnd_begin_angle) / m_angle_multiplier;
 raw_packet+=4;

 //Конец фазового окна
 int  knock_k_wnd_end_angle;
 if (false == CNumericConv::Hex16ToBin(raw_packet,&knock_k_wnd_end_angle,true))
  return false;
 m_KnockPar.knock_k_wnd_end_angle = ((float)knock_k_wnd_end_angle) / m_angle_multiplier;
 raw_packet+=4;

 //Постоянная времени интегрирования
 unsigned char knock_int_time_const;
 if (false == CNumericConv::Hex8ToBin(raw_packet,&knock_int_time_const))
  return false;
 m_KnockPar.knock_int_time_const = knock_int_time_const;
 raw_packet+=2;

 //-----------------
 //Шаг смещения УОЗ при детонации
 int knock_retard_step;
 if (false == CNumericConv::Hex16ToBin(raw_packet,&knock_retard_step,true))
  return false;
 m_KnockPar.knock_retard_step = ((float)knock_retard_step) / m_angle_multiplier;
 raw_packet+=4;

 //Шаг восстановления УОЗ
 int knock_advance_step;
 if (false == CNumericConv::Hex16ToBin(raw_packet,&knock_advance_step,true))
  return false;
 m_KnockPar.knock_advance_step = ((float)knock_advance_step) / m_angle_multiplier;
 raw_packet+=4;

 //Максимальное смещение УОЗ
 int knock_max_retard;
 if (false == CNumericConv::Hex16ToBin(raw_packet,&knock_max_retard,true))
  return false;
 m_KnockPar.knock_max_retard = ((float)knock_max_retard) / m_angle_multiplier;
 raw_packet+=4;

 //Порог детонации
 int knock_threshold;
 if (false == CNumericConv::Hex16ToBin(raw_packet,&knock_threshold, false))
  return false;
 m_KnockPar.knock_threshold = ((float)knock_threshold) * m_adc_discrete;
 raw_packet+=4;

 //Задержка восстановления УОЗ
 unsigned char knock_recovery_delay;
 if (false == CNumericConv::Hex8ToBin(raw_packet,&knock_recovery_delay))
  return false;
 m_KnockPar.knock_recovery_delay = knock_recovery_delay;
 raw_packet+=2;

 if (*raw_packet!='\r')
  return false;

 return true;
}

//-----------------------------------------------------------------------
bool CControlApp::Parse_CE_ERR_CODES(const BYTE* raw_packet)
{
 SECU3IO::CEErrors& m_CEErrors = m_recepted_packet.m_CEErrors;

 if (strlen((char*)raw_packet)!=5)  //размер пакета без сигнального символа, дескриптора
  return false;

 int flags = 0;
 if (false == CNumericConv::Hex16ToBin(raw_packet,&flags))
  return false;
 m_CEErrors.flags = flags;
 raw_packet+=4;

 if (*raw_packet!='\r')
  return false;

 return true;
}

//-----------------------------------------------------------------------
bool CControlApp::Parse_CE_SAVED_ERR(const BYTE* raw_packet)
{
 SECU3IO::CEErrors& m_CEErrors = m_recepted_packet.m_CEErrors;

 if (strlen((char*)raw_packet)!=5)  //размер пакета без сигнального символа, дескриптора
  return false;

 int flags = 0;
 if (false == CNumericConv::Hex16ToBin(raw_packet,&flags))
  return false;
 m_CEErrors.flags = flags;
 raw_packet+=4;

 if (*raw_packet!='\r')
  return false;

 return true;
}

//-----------------------------------------------------------------------
bool CControlApp::Parse_FWINFO_DAT(const BYTE* raw_packet)
{
 SECU3IO::FWInfoDat& m_FWInfoDat = m_recepted_packet.m_FWInfoDat;

 if (strlen((char*)raw_packet)!=(FW_SIGNATURE_INFO_SIZE+8+1))  //размер пакета без сигнального символа, дескриптора
  return false;

 //строка с информацией 
 strncpy(m_FWInfoDat.info,(const char*)raw_packet, FW_SIGNATURE_INFO_SIZE);
 m_FWInfoDat.info[FW_SIGNATURE_INFO_SIZE] = 0;
 raw_packet+=FW_SIGNATURE_INFO_SIZE;

 //биты опций прошивки
 DWORD options = 0;
 if (false == CNumericConv::Hex32ToBin(raw_packet,&options))
  return false;
 m_FWInfoDat.options = options;
 raw_packet+=8;

 if (*raw_packet!='\r')
  return false;

 return true;
}

//-----------------------------------------------------------------------
bool CControlApp::Parse_MISCEL_PAR(const BYTE* raw_packet)
{
 SECU3IO::MiscelPar& m_MiscPar = m_recepted_packet.m_MiscelPar;

 if (strlen((char*)raw_packet)!=16)  //размер пакета без сигнального символа, дескриптора
  return false;

 //Делитель для UART-а
 int divisor = 0;
 if (false == CNumericConv::Hex16ToBin(raw_packet, &divisor))
  return false;
 raw_packet+=4;

 m_MiscPar.baud_rate = 0;

 for(size_t i = 0; i < SECU3IO::SECU3_ALLOWABLE_UART_DIVISORS_COUNT; ++i)
  if (SECU3IO::secu3_allowable_uart_divisors[i].second == divisor)
    m_MiscPar.baud_rate = SECU3IO::secu3_allowable_uart_divisors[i].first;

  ASSERT(m_MiscPar.baud_rate);

 //Период посылки пакетов в десятках миллисекунд
 unsigned char period_t_ms = 0;
 if (false == CNumericConv::Hex8ToBin(raw_packet,&period_t_ms))
  return false;
 raw_packet+=2;

 m_MiscPar.period_ms = period_t_ms * 10;

 //Отсечка разрешена/запрещена
 if (false == CNumericConv::Hex4ToBin(*raw_packet, &m_MiscPar.ign_cutoff))
  return false;
 raw_packet+=1;

 //Обороты отсечки
 if (false == CNumericConv::Hex16ToBin(raw_packet, &m_MiscPar.ign_cutoff_thrd, true))
  return false;
 raw_packet+=4;

 //Выход ДХ: Начало импульса в зубьях шкива относительно ВМТ
 signed int start = 0;
 if (false == CNumericConv::Hex8ToBin(raw_packet, &start))
  return false;
 raw_packet+=2;

 m_MiscPar.hop_start_cogs = start;

 //Выход ДХ: Длительность импульса в зубьях шкива
 unsigned char duration = 0;
 if (false == CNumericConv::Hex8ToBin(raw_packet, &duration))
  return false;
 raw_packet+=2;

 m_MiscPar.hop_durat_cogs = duration;

 if (*raw_packet!='\r')
  return false;

 return true;
}

//-----------------------------------------------------------------------
bool CControlApp::Parse_EDITAB_PAR(const BYTE* raw_packet)
{
 SECU3IO::EditTabPar& m_EditTabPar = m_recepted_packet.m_EditTabPar;

 int packet_size = strlen((char*)raw_packet);
 if (packet_size < 7 || packet_size > 37)
  return false;

 //номер набора таблиц
 if (false == CNumericConv::Hex4ToBin(*raw_packet, &m_EditTabPar.tab_set_index))
  return false;
 raw_packet+=1;
 if (m_EditTabPar.tab_set_index != ETTS_GASOLINE_SET && m_EditTabPar.tab_set_index != ETTS_GAS_SET)
  return false;

 //код таблицы в наборе
 if (false == CNumericConv::Hex4ToBin(*raw_packet, &m_EditTabPar.tab_id))
  return false;
 raw_packet+=1;

 if (m_EditTabPar.tab_id != ETMT_STRT_MAP && m_EditTabPar.tab_id != ETMT_IDLE_MAP && m_EditTabPar.tab_id != ETMT_WORK_MAP &&
     m_EditTabPar.tab_id != ETMT_TEMP_MAP && m_EditTabPar.tab_id != ETMT_NAME_STR)
  return false;

 //адрес фрагмента данных в таблице (смещение в таблице)
 unsigned char address;
 if (false == CNumericConv::Hex8ToBin(raw_packet, &address))
  return false;
 m_EditTabPar.address = address;
 raw_packet+=2;

 packet_size-=5;
 if (packet_size % 2) // 1 byte in HEX is 2 symbols
  return false;

 if (m_EditTabPar.tab_id != ETMT_NAME_STR)
 {
  //фрагмент с данными (float)
  size_t data_size = 0;
  for(int i = 0; i < packet_size / 2; ++i)
  {
   signed char value;
   if (false == CNumericConv::Hex8ToBin(raw_packet, (unsigned char*)&value))
    return false;
   m_EditTabPar.table_data[i] = ((float)value) / AA_MAPS_M_FACTOR;
   raw_packet+=2;
   ++data_size;
  }
  m_EditTabPar.data_size = data_size;
 }
 else
 {
  //фрагмент с данными (текстовая информация)
  char *p = strchr((char*)raw_packet, '\r');
  std::string raw_string((char*)raw_packet, ((BYTE*)p) - raw_packet);
  OemToChar(raw_string.c_str(), m_EditTabPar.name_data);
  raw_packet+=raw_string.size();
  m_EditTabPar.data_size = raw_string.size();
 }
 
 if (*raw_packet!='\r')
  return false;

 return true;
}

//-----------------------------------------------------------------------
bool CControlApp::Parse_ATTTAB_PAR(const BYTE* raw_packet)
{
 SECU3IO::SepTabPar& m_AttTabPar = m_recepted_packet.m_SepTabPar;

 int packet_size = strlen((char*)raw_packet);
 if (packet_size < 5 || packet_size > 35)
  return false;

 //адрес фрагмента данных в таблице (смещение в таблице)
 unsigned char address;
 if (false == CNumericConv::Hex8ToBin(raw_packet, &address))
  return false;
 m_AttTabPar.address = address;
 raw_packet+=2;

 packet_size-=3;
 if (packet_size % 2) // 1 byte in HEX is 2 symbols
  return false;

 //фрагмент с данными (коды коэффициентов усиления)
 size_t data_size = 0;
 for(int i = 0; i < packet_size / 2; ++i)
 {
  unsigned char value;
  if (false == CNumericConv::Hex8ToBin(raw_packet, &value))
   return false;
  m_AttTabPar.table_data[i] = value;
  raw_packet+=2;
  ++data_size;
 }
 m_AttTabPar.data_size = data_size;
 
 if (*raw_packet!='\r')
  return false;

 return true;
}

//-----------------------------------------------------------------------
bool CControlApp::Parse_DIAGINP_DAT(const BYTE* raw_packet)
{
 SECU3IO::DiagInpDat& m_DiagInpDat = m_recepted_packet.m_DiagInpDat;

 if (strlen((char*)raw_packet)!=35)  //размер пакета без сигнального символа, дескриптора
  return false;

 //напряжение бортовой сети
 int voltage = 0;
 if (false == CNumericConv::Hex16ToBin(raw_packet, &voltage))
  return false;
 m_DiagInpDat.voltage = ((float)voltage) * m_adc_discrete;
 raw_packet+=4;

 //датчик абсолютного давления
 int map = 0;
 if (false == CNumericConv::Hex16ToBin(raw_packet, &map))
  return false;
 m_DiagInpDat.map = ((float)map) * m_adc_discrete;
 raw_packet+=4;

 //датчик температуры охлаждающей жидкости
 int temp = 0;
 if (false == CNumericConv::Hex16ToBin(raw_packet, &temp))
  return false;
 m_DiagInpDat.temp = ((float)temp) * m_adc_discrete;
 raw_packet+=4;

 //дополнительный IO1
 int add_io1 = 0;
 if (false == CNumericConv::Hex16ToBin(raw_packet, &add_io1))
  return false;
 m_DiagInpDat.add_io1 = ((float)add_io1) * m_adc_discrete;
 raw_packet+=4;

 //дополнительный IO2
 int add_io2 = 0;
 if (false == CNumericConv::Hex16ToBin(raw_packet, &add_io2))
  return false;
 m_DiagInpDat.add_io2 = ((float)add_io2) * m_adc_discrete;
 raw_packet+=4;

 //датчик положения дроссельной заслонки (концевик карбюратора)
 int carb = 0;
 if (false == CNumericConv::Hex16ToBin(raw_packet, &carb))
  return false;
 m_DiagInpDat.carb = ((float)carb) * m_adc_discrete;
 raw_packet+=4;

 //Датчик детонации 1
 int ks_1 = 0;
 if (false == CNumericConv::Hex16ToBin(raw_packet, &ks_1))
  return false;
 m_DiagInpDat.ks_1 = ((float)ks_1) * m_adc_discrete;
 raw_packet+=4;

 //Датчик детонации 2
 int ks_2 = 0;
 if (false == CNumericConv::Hex16ToBin(raw_packet, &ks_2))
  return false;
 m_DiagInpDat.ks_2 = ((float)ks_2) * m_adc_discrete;
 raw_packet+=4;

 //байт с состоянием цифровых входов
 unsigned char byte = 0;
 if (false == CNumericConv::Hex8ToBin(raw_packet, &byte))
  return false;
 raw_packet+=2;

 //газовый клапан, ДПКВ, ДНО(VR), ДФ, "Bootloader", "Default EEPROM"
 m_DiagInpDat.gas   = (byte & (1 << 0)) != 0;
 m_DiagInpDat.ckps  = (byte & (1 << 1)) != 0;
 m_DiagInpDat.ref_s = (byte & (1 << 2)) != 0;
 m_DiagInpDat.ps    = (byte & (1 << 3)) != 0;
 m_DiagInpDat.bl    = (byte & (1 << 4)) != 0;
 m_DiagInpDat.de    = (byte & (1 << 5)) != 0;

 if (*raw_packet!='\r')
  return false;

 return true;
}

//-----------------------------------------------------------------------
bool CControlApp::Parse_CHOKE_PAR(const BYTE* raw_packet)
{
 SECU3IO::ChokePar& m_ChokePar = m_recepted_packet.m_ChokePar;

 if (strlen((char*)raw_packet)!=6)  //размер пакета без сигнального символа, дескриптора
  return false;

 //Number of stepper motor steps
 if (false == CNumericConv::Hex16ToBin(raw_packet,&m_ChokePar.sm_steps))
  return false;
 raw_packet+=4;

 //choke testing mode command/state (it is fake parameter)
 if (false == CNumericConv::Hex4ToBin(*raw_packet,&m_ChokePar.testing))
  return false;
 raw_packet+=1;

 if (*raw_packet!='\r')
  return false;

 return true;
}

//-----------------------------------------------------------------------
//Return: true - если хотя бы один пакет был получен
bool CControlApp::ParsePackets()
{
 Packets::iterator it;
 bool status = false;
 BYTE descriptor = 0;

 ASSERT(m_pPackets);
 for(it = m_pPackets->begin(); it!=m_pPackets->end(); ++it)
 {
  const BYTE* raw_packet = (const BYTE*)it->c_str();
  if (*raw_packet!='@')
   continue;
  ++raw_packet;   //пропустили '@'
  if (*raw_packet==0)
   continue;
  descriptor = *raw_packet;
  ++raw_packet;  //пропустили символ дескриптора
  switch(descriptor)
  {
   case TEMPER_PAR:
    if (Parse_TEMPER_PAR(raw_packet))
     break; //пакет успешно разобран по составляющим
    continue;//пакет не прошел сурового отбора нашим жюри :-)
   case CARBUR_PAR:
    if (Parse_CARBUR_PAR(raw_packet))
     break;
    continue;
   case IDLREG_PAR:
    if (Parse_IDLREG_PAR(raw_packet))
     break;
    continue;
   case ANGLES_PAR:
    if (Parse_ANGLES_PAR(raw_packet))
     break;
    continue;
   case FUNSET_PAR:
    if (Parse_FUNSET_PAR(raw_packet))
     break;
    continue;
   case STARTR_PAR:
    if (Parse_STARTR_PAR(raw_packet))
     break;
    continue;
   case FNNAME_DAT:
    if (Parse_FNNAME_DAT(raw_packet))
     break;
    continue;
   case SENSOR_DAT:
    if (Parse_SENSOR_DAT(raw_packet))
     break;
    continue;
   case DBGVAR_DAT:
    if (Parse_DBGVAR_DAT(raw_packet))
     break;
    continue;
   case ADCRAW_DAT:
    if (Parse_ADCRAW_DAT(raw_packet))
     break;
    continue;
   case ADCCOR_PAR:
    if (Parse_ADCCOR_PAR(raw_packet))
     break;
    continue;
   case CKPS_PAR:
    if (Parse_CKPS_PAR(raw_packet))
     break;
    continue;
   case OP_COMP_NC:
    if (Parse_OP_COMP_NC(raw_packet))
     break;
    continue;
   case KNOCK_PAR:
    if (Parse_KNOCK_PAR(raw_packet))
     break;
    continue;
   case CE_ERR_CODES:
    if (Parse_CE_ERR_CODES(raw_packet))
     break;
    continue;
   case CE_SAVED_ERR:
    if (Parse_CE_SAVED_ERR(raw_packet))
     break;
    continue;
   case FWINFO_DAT:
    if (Parse_FWINFO_DAT(raw_packet))
     break;
    continue;
   case MISCEL_PAR:
    if (Parse_MISCEL_PAR(raw_packet))
     break;
    continue;
   case EDITAB_PAR:
    if (Parse_EDITAB_PAR(raw_packet))
     break;
    continue;
   case ATTTAB_PAR:
    if (Parse_ATTTAB_PAR(raw_packet))
     break;
    continue;
   case DIAGINP_DAT:
    if (Parse_DIAGINP_DAT(raw_packet))
     break;
    continue;
   case CHOKE_PAR:
    if (Parse_CHOKE_PAR(raw_packet))
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
  m_pEventHandler->OnPacketReceived(descriptor, &EndPendingPacket());
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

  //если порт не открыт, то для того чтобы не грузить процессор, засыпаем
  //на 100мс в каждом цикле
  if (p_port->GetHandle()==INVALID_HANDLE_VALUE)
   Sleep(100);

  if (p_capp->m_is_thread_must_exit)
   break;  //поступила команда завершения работы потока

  pEventHandler = p_capp->m_pEventHandler;
  ASSERT(pEventHandler); //перед тем как обрабатывать пакеты, необходимо приаттачить обработчик событий

  //читаем блок данных
  actual_received = 0;
  p_port->RecvBlock(read_buf,RAW_BYTES_TO_READ_MAX,&actual_received);
  read_buf[actual_received] = 0;

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
  case DIAGINP_DAT:
  case DIAGOUT_DAT:
  case CHOKE_PAR: 
   return true;
  default:
   return false;
 }//switch
}

//-----------------------------------------------------------------------
bool CControlApp::SendPacket(const BYTE i_descriptor, const void* i_packet_data)
{
 if (false==IsValidDescriptor(i_descriptor))
  return false;

 m_outgoing_packet = "";
 m_outgoing_packet+='!';
 m_outgoing_packet+=i_descriptor;

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

  default:
   return false; //invalid descriptor
  }//switch
 return m_p_port->SendASCII(m_outgoing_packet.c_str());
}

//-----------------------------------------------------------------------
//посылает команду изменения контекста на новый контекст специфицированный i_new_descriptor-ом
bool CControlApp::ChangeContext(const BYTE i_new_descriptor)
{
 m_outgoing_packet = "";

 if (false==IsValidDescriptor(i_new_descriptor))  //передали правильный дескриптор ?
  return false;

 m_outgoing_packet+='!';
 m_outgoing_packet+= CHANGEMODE;
 m_outgoing_packet+= i_new_descriptor;
 m_outgoing_packet+= '\r';
 return m_p_port->SendASCII(m_outgoing_packet.c_str());   //посылаем команду изменения дескриптора
}
//-----------------------------------------------------------------------
//Посылает команду запуска бутлоадера
bool CControlApp::StartBootLoader()
{
 m_outgoing_packet = "";

 m_outgoing_packet+='!';
 m_outgoing_packet+= BOOTLOADER;
 m_outgoing_packet+= 'l';
 m_outgoing_packet+= '\r';
 return m_p_port->SendASCII(m_outgoing_packet.c_str());   //посылаем команду запуска бутлоадера
}

//-----------------------------------------------------------------------
void CControlApp::Build_CARBUR_PAR(CarburPar* packet_data)
{
 CNumericConv::Bin16ToHex(packet_data->ephh_lot,m_outgoing_packet);
 CNumericConv::Bin16ToHex(packet_data->ephh_hit,m_outgoing_packet);
 CNumericConv::Bin4ToHex(packet_data->carb_invers,m_outgoing_packet);
 int epm_on_threshold = MathHelpers::Round(packet_data->epm_ont * MAP_PHYSICAL_MAGNITUDE_MULTIPLAYER);
 CNumericConv::Bin16ToHex(epm_on_threshold,m_outgoing_packet);
 CNumericConv::Bin16ToHex(packet_data->ephh_lot_g,m_outgoing_packet);
 CNumericConv::Bin16ToHex(packet_data->ephh_hit_g,m_outgoing_packet);
 unsigned char shutoff_delay = MathHelpers::Round(packet_data->shutoff_delay * 100.0f);
 CNumericConv::Bin8ToHex(shutoff_delay,m_outgoing_packet);
 unsigned char tps_threshold = MathHelpers::Round(packet_data->tps_threshold * TPS_PHYSICAL_MAGNITUDE_MULTIPLAYER);
 CNumericConv::Bin8ToHex(tps_threshold, m_outgoing_packet);
 m_outgoing_packet+= '\r';
}

//-----------------------------------------------------------------------
void CControlApp::Build_IDLREG_PAR(IdlRegPar* packet_data)
{
 CNumericConv::Bin4ToHex(packet_data->idl_regul,m_outgoing_packet);

 int ifac1 =  MathHelpers::Round((packet_data->ifac1 * m_angle_multiplier));
 CNumericConv::Bin16ToHex(ifac1,m_outgoing_packet);

 int ifac2 = MathHelpers::Round((packet_data->ifac2 * m_angle_multiplier));
 CNumericConv::Bin16ToHex(ifac2,m_outgoing_packet);

 CNumericConv::Bin16ToHex(packet_data->MINEFR,m_outgoing_packet);
 CNumericConv::Bin16ToHex(packet_data->idling_rpm,m_outgoing_packet);

 int min_angle = MathHelpers::Round((packet_data->min_angle * m_angle_multiplier));
 CNumericConv::Bin16ToHex(min_angle,m_outgoing_packet);
 int max_angle = MathHelpers::Round((packet_data->max_angle * m_angle_multiplier));
 CNumericConv::Bin16ToHex(max_angle,m_outgoing_packet);

 int turn_on_temp = MathHelpers::Round((packet_data->turn_on_temp * TEMP_PHYSICAL_MAGNITUDE_MULTIPLAYER));
 CNumericConv::Bin16ToHex(turn_on_temp, m_outgoing_packet);

 m_outgoing_packet+= '\r';
}

//-----------------------------------------------------------------------
void CControlApp::Build_STARTR_PAR(StartrPar* packet_data)
{
 CNumericConv::Bin16ToHex(packet_data->starter_off,m_outgoing_packet);
 CNumericConv::Bin16ToHex(packet_data->smap_abandon,m_outgoing_packet);
 m_outgoing_packet+= '\r';
}
//-----------------------------------------------------------------------

void CControlApp::Build_TEMPER_PAR(TemperPar* packet_data)
{
 CNumericConv::Bin4ToHex(packet_data->tmp_use,m_outgoing_packet);
 CNumericConv::Bin4ToHex(packet_data->vent_pwm,m_outgoing_packet);
 CNumericConv::Bin4ToHex(packet_data->cts_use_map,m_outgoing_packet);
 int vent_on = MathHelpers::Round(packet_data->vent_on * TEMP_PHYSICAL_MAGNITUDE_MULTIPLAYER);
 CNumericConv::Bin16ToHex(vent_on,m_outgoing_packet);
 int vent_off = MathHelpers::Round(packet_data->vent_off * TEMP_PHYSICAL_MAGNITUDE_MULTIPLAYER);
 CNumericConv::Bin16ToHex(vent_off,m_outgoing_packet);
 m_outgoing_packet+= '\r';
}

//-----------------------------------------------------------------------
void CControlApp::Build_ANGLES_PAR(AnglesPar* packet_data)
{
 int max_angle = MathHelpers::Round(packet_data->max_angle * m_angle_multiplier);
 CNumericConv::Bin16ToHex(max_angle,m_outgoing_packet);
 int min_angle = MathHelpers::Round(packet_data->min_angle * m_angle_multiplier);
 CNumericConv::Bin16ToHex(min_angle,m_outgoing_packet);
 int angle_corr = MathHelpers::Round(packet_data->angle_corr * m_angle_multiplier);
 CNumericConv::Bin16ToHex(angle_corr,m_outgoing_packet);
 int dec_spead = MathHelpers::Round(packet_data->dec_spead * m_angle_multiplier);
 CNumericConv::Bin16ToHex(dec_spead,m_outgoing_packet);
 int inc_spead = MathHelpers::Round(packet_data->inc_spead * m_angle_multiplier);
 CNumericConv::Bin16ToHex(inc_spead,m_outgoing_packet);
 CNumericConv::Bin4ToHex(packet_data->zero_adv_ang, m_outgoing_packet);
 m_outgoing_packet+= '\r';
}

//-----------------------------------------------------------------------
void CControlApp::Build_FUNSET_PAR(FunSetPar* packet_data)
{
 CNumericConv::Bin8ToHex(packet_data->fn_benzin,m_outgoing_packet);
 CNumericConv::Bin8ToHex(packet_data->fn_gas,m_outgoing_packet);
 unsigned int map_lower_pressure = MathHelpers::Round(packet_data->map_lower_pressure * MAP_PHYSICAL_MAGNITUDE_MULTIPLAYER);
 CNumericConv::Bin16ToHex(map_lower_pressure,m_outgoing_packet);
 int map_upper_pressure = MathHelpers::Round(packet_data->map_upper_pressure * MAP_PHYSICAL_MAGNITUDE_MULTIPLAYER);
 CNumericConv::Bin16ToHex(map_upper_pressure,m_outgoing_packet);
 int map_curve_offset = MathHelpers::Round(packet_data->map_curve_offset / m_adc_discrete);
 CNumericConv::Bin16ToHex(map_curve_offset, m_outgoing_packet);
 int map_curve_gradient = MathHelpers::Round(128.0f * packet_data->map_curve_gradient * MAP_PHYSICAL_MAGNITUDE_MULTIPLAYER * m_adc_discrete);
 CNumericConv::Bin16ToHex(map_curve_gradient, m_outgoing_packet);
 int tps_curve_offset = MathHelpers::Round(packet_data->tps_curve_offset / m_adc_discrete);
 CNumericConv::Bin16ToHex(tps_curve_offset, m_outgoing_packet);
 int tps_curve_gradient = MathHelpers::Round(128.0f * packet_data->tps_curve_gradient * (TPS_PHYSICAL_MAGNITUDE_MULTIPLAYER*64) * m_adc_discrete);
 CNumericConv::Bin16ToHex(tps_curve_gradient, m_outgoing_packet);
 m_outgoing_packet+= '\r';
}

//-----------------------------------------------------------------------
void CControlApp::Build_ADCCOR_PAR(ADCCompenPar* packet_data)
{
 signed int map_adc_factor = MathHelpers::Round(packet_data->map_adc_factor * 16384);
 CNumericConv::Bin16ToHex(map_adc_factor,m_outgoing_packet);
 signed long map_correction_d = MathHelpers::Round((-packet_data->map_adc_correction) / m_adc_discrete); //переводим из вольтов в дискреты АЦП
 signed long map_adc_correction = MathHelpers::Round(16384 * (0.5f - map_correction_d * packet_data->map_adc_factor));
 CNumericConv::Bin32ToHex(map_adc_correction,m_outgoing_packet);

 signed int ubat_adc_factor = MathHelpers::Round(packet_data->ubat_adc_factor * 16384);
 CNumericConv::Bin16ToHex(ubat_adc_factor,m_outgoing_packet);
 signed long ubat_correction_d = MathHelpers::Round((-packet_data->ubat_adc_correction) / m_adc_discrete); //переводим из вольтов в дискреты АЦП
 signed long ubat_adc_correction = MathHelpers::Round(16384 * (0.5f - ubat_correction_d * packet_data->ubat_adc_factor));
 CNumericConv::Bin32ToHex(ubat_adc_correction,m_outgoing_packet);

 signed int temp_adc_factor = MathHelpers::Round(packet_data->temp_adc_factor * 16384);
 CNumericConv::Bin16ToHex(temp_adc_factor,m_outgoing_packet);
 signed long temp_correction_d = MathHelpers::Round((-packet_data->temp_adc_correction) / m_adc_discrete); //переводим из вольтов в дискреты АЦП
 signed long temp_adc_correction = MathHelpers::Round(16384 * (0.5f - temp_correction_d * packet_data->temp_adc_factor));
 CNumericConv::Bin32ToHex(temp_adc_correction,m_outgoing_packet);
 //TPS sensor
 signed int tps_adc_factor = MathHelpers::Round(packet_data->tps_adc_factor * 16384);
 CNumericConv::Bin16ToHex(tps_adc_factor,m_outgoing_packet);
 signed long tps_correction_d = MathHelpers::Round((-packet_data->tps_adc_correction) / m_adc_discrete); //переводим из вольтов в дискреты АЦП
 signed long tps_adc_correction = MathHelpers::Round(16384 * (0.5f - tps_correction_d * packet_data->tps_adc_factor));
 CNumericConv::Bin32ToHex(tps_adc_correction,m_outgoing_packet);
 //ADD_IO1 input
 signed int ai1_adc_factor = MathHelpers::Round(packet_data->ai1_adc_factor * 16384);
 CNumericConv::Bin16ToHex(ai1_adc_factor,m_outgoing_packet);
 signed long ai1_correction_d = MathHelpers::Round((-packet_data->ai1_adc_correction) / m_adc_discrete); //переводим из вольтов в дискреты АЦП
 signed long ai1_adc_correction = MathHelpers::Round(16384 * (0.5f - ai1_correction_d * packet_data->ai1_adc_factor));
 CNumericConv::Bin32ToHex(ai1_adc_correction,m_outgoing_packet);
 //ADD_IO2 input
 signed int ai2_adc_factor = MathHelpers::Round(packet_data->ai2_adc_factor * 16384);
 CNumericConv::Bin16ToHex(ai2_adc_factor,m_outgoing_packet);
 signed long ai2_correction_d = MathHelpers::Round((-packet_data->ai2_adc_correction) / m_adc_discrete); //переводим из вольтов в дискреты АЦП
 signed long ai2_adc_correction = MathHelpers::Round(16384 * (0.5f - ai2_correction_d * packet_data->ai2_adc_factor));
 CNumericConv::Bin32ToHex(ai2_adc_correction,m_outgoing_packet);
 m_outgoing_packet+= '\r';
}

//-----------------------------------------------------------------------
void CControlApp::Build_CKPS_PAR(CKPSPar* packet_data)
{
 CNumericConv::Bin4ToHex(packet_data->ckps_edge_type, m_outgoing_packet);
 CNumericConv::Bin4ToHex(packet_data->ref_s_edge_type, m_outgoing_packet);
 CNumericConv::Bin8ToHex(packet_data->ckps_cogs_btdc, m_outgoing_packet);
 CNumericConv::Bin8ToHex(packet_data->ckps_ignit_cogs, m_outgoing_packet);
 CNumericConv::Bin8ToHex(packet_data->ckps_engine_cyl, m_outgoing_packet);
 CNumericConv::Bin4ToHex(packet_data->ckps_merge_ign_outs, m_outgoing_packet);
 CNumericConv::Bin8ToHex(packet_data->ckps_cogs_num, m_outgoing_packet);
 CNumericConv::Bin8ToHex(packet_data->ckps_miss_num, m_outgoing_packet);
 m_outgoing_packet+= '\r';
}

//-----------------------------------------------------------------------
void CControlApp::Build_KNOCK_PAR(KnockPar* packet_data)
{
 CNumericConv::Bin4ToHex(packet_data->knock_use_knock_channel,m_outgoing_packet);
 unsigned char knock_bpf_frequency = (unsigned char)packet_data->knock_bpf_frequency;
 CNumericConv::Bin8ToHex(knock_bpf_frequency,m_outgoing_packet);
 int knock_k_wnd_begin_angle = MathHelpers::Round(packet_data->knock_k_wnd_begin_angle * m_angle_multiplier);
 CNumericConv::Bin16ToHex(knock_k_wnd_begin_angle,m_outgoing_packet);
 int knock_k_wnd_end_angle = MathHelpers::Round(packet_data->knock_k_wnd_end_angle * m_angle_multiplier);
 CNumericConv::Bin16ToHex(knock_k_wnd_end_angle,m_outgoing_packet);
 unsigned char knock_int_time_const = (unsigned char)packet_data->knock_int_time_const;
 CNumericConv::Bin8ToHex(knock_int_time_const, m_outgoing_packet);

 int knock_retard_step = MathHelpers::Round(packet_data->knock_retard_step * m_angle_multiplier);
 CNumericConv::Bin16ToHex(knock_retard_step, m_outgoing_packet);
 int knock_advance_step = MathHelpers::Round(packet_data->knock_advance_step * m_angle_multiplier);
 CNumericConv::Bin16ToHex(knock_advance_step, m_outgoing_packet);
 int knock_max_retard = MathHelpers::Round(packet_data->knock_max_retard * m_angle_multiplier);
 CNumericConv::Bin16ToHex(knock_max_retard, m_outgoing_packet);
 int knock_threshold = MathHelpers::Round(packet_data->knock_threshold / m_adc_discrete);
 CNumericConv::Bin16ToHex(knock_threshold, m_outgoing_packet);
 unsigned char knock_recovery_delay = (unsigned char)packet_data->knock_recovery_delay;
 CNumericConv::Bin8ToHex(knock_recovery_delay, m_outgoing_packet);

 m_outgoing_packet+= '\r';
}

//-----------------------------------------------------------------------
void CControlApp::Build_OP_COMP_NC(SECU3IO::OPCompNc* packet_data)
{
 CNumericConv::Bin8ToHex(packet_data->opdata, m_outgoing_packet);
 CNumericConv::Bin8ToHex(packet_data->opcode, m_outgoing_packet);
 m_outgoing_packet+= '\r';
}

//-----------------------------------------------------------------------
void CControlApp::Build_CE_SAVED_ERR(SECU3IO::CEErrors* packet_data)
{
 CNumericConv::Bin16ToHex(packet_data->flags, m_outgoing_packet);
 m_outgoing_packet+= '\r';
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

 CNumericConv::Bin16ToHex(divisor, m_outgoing_packet);
 unsigned char perid_ms = packet_data->period_ms / 10;
 CNumericConv::Bin8ToHex(perid_ms, m_outgoing_packet);
 CNumericConv::Bin4ToHex(packet_data->ign_cutoff, m_outgoing_packet);
 CNumericConv::Bin16ToHex(packet_data->ign_cutoff_thrd, m_outgoing_packet);
 CNumericConv::Bin8ToHex(packet_data->hop_start_cogs, m_outgoing_packet);
 CNumericConv::Bin8ToHex(packet_data->hop_durat_cogs, m_outgoing_packet);
 m_outgoing_packet+= '\r';
}

//-----------------------------------------------------------------------
void CControlApp::Build_EDITAB_PAR(EditTabPar* packet_data)
{
 CNumericConv::Bin4ToHex(packet_data->tab_set_index, m_outgoing_packet);
 CNumericConv::Bin4ToHex(packet_data->tab_id, m_outgoing_packet);
 CNumericConv::Bin8ToHex(packet_data->address, m_outgoing_packet);

 if (packet_data->tab_id != ETMT_NAME_STR)
 {
  for(unsigned int i = 0; i < packet_data->data_size; ++i)
  {
   signed char value = MathHelpers::Round(packet_data->table_data[i] * AA_MAPS_M_FACTOR);
   CNumericConv::Bin8ToHex(value, m_outgoing_packet);
  }
 }
 else //string
 {
  char raw_string[64];
  CharToOem(packet_data->name_data, raw_string);
  std::string str(raw_string, packet_data->data_size);
  m_outgoing_packet+=str;
 }

 m_outgoing_packet+= '\r';
}

//-----------------------------------------------------------------------
void CControlApp::Build_DIAGOUT_DAT(DiagOutDat* packet_data)
{
 unsigned int bits = ((packet_data->ign_out1 != 0) << 0) | ((packet_data->ign_out2 != 0) << 1) |
 ((packet_data->ign_out3 != 0) << 2) | ((packet_data->ign_out4 != 0) << 3) | ((packet_data->add_io1 != 0) << 4) |
 ((packet_data->add_io2 != 0) << 5) | ((packet_data->ie != 0) << 6) | ((packet_data->fe != 0) << 7) |
 ((packet_data->ecf != 0) << 8) | ((packet_data->ce != 0) << 9) | ((packet_data->st_block != 0) << 10);

 CNumericConv::Bin16ToHex(bits, m_outgoing_packet);

 m_outgoing_packet+= '\r';
}

//-----------------------------------------------------------------------
void CControlApp::Build_CHOKE_PAR(ChokePar* packet_data)
{
 CNumericConv::Bin16ToHex(packet_data->sm_steps, m_outgoing_packet);
 CNumericConv::Bin4ToHex(packet_data->testing, m_outgoing_packet); //fake parameter (actually it is command)
 m_outgoing_packet+= '\r';
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

