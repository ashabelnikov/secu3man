 /****************************************************************
 *       SECU-3  - An open source, free engine control unit
 *    Designed by Alexey A. Shabelnikov. Ukraine, Gorlovka 2007.
 *       Microprocessors systems - design & programming.
 *    contacts:
 *              http://secu-3.narod.ru
 *              ICQ: 405-791-931
 ****************************************************************/

#include "stdafx.h"
#include <vector>
#include <windows.h>
#include "ControlApp.h"
#include "ccomport.h"
#include "ufcodes.h"


#define MAP_SENSOR_OFFSET   0.45f     //Вольт
#define MAP_SENSOR_GRADIENT 20.5f     //кПа/Вольт  
#define ADC_DISCRETE        0.0025f   //Вольт 
#define ANGLE_MULTIPLAYER   40.0f     //коэффициент масштабирования углов поворота коленвала  


using namespace SECU3IO;

//-----------------------------------------------------------------------
CControlApp::CControlApp() 
: m_map_sensor_offset(MAP_SENSOR_OFFSET)
, m_map_sensor_gradient(MAP_SENSOR_GRADIENT)
, m_adc_discrete(ADC_DISCRETE)
, m_angle_multiplier(ANGLE_MULTIPLAYER)
, m_pEventHandler(NULL)
, m_online_state(false)
, m_force_notify_about_connection(false)
{
  m_p_port      = NULL;
  m_hThread     = NULL;
  m_ThreadId    = 0;
  m_hAwakeEvent = NULL;
  m_is_thread_must_exit = false;
  m_uart_speed  = CBR_9600;

  m_Packets = new Packets(50);

  m_ingoing_packet  = "";
  m_packets_parse_state = 0;
  m_hTimer      = NULL;
  m_outgoing_packet = "";
}

//-----------------------------------------------------------------------
CControlApp::~CControlApp()
{
	delete m_Packets;
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
int CControlApp::SplitPackets(BYTE* i_buff)
{
	m_Packets->clear();

	BYTE* p = i_buff;
   
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
		   m_Packets->push_back(m_ingoing_packet);
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

 return m_Packets->size();  
}

//Helper function for 19.3828 temperature sensor
//Приводит температуру представленную дискретами АЦП к температуре в градусах Цельсия
float CControlApp::FromTemperatureSensor(int temperature_adc)
{
 float temperature_in_volts = ((float)temperature_adc) * m_adc_discrete;
 return (temperature_in_volts * 100.0f) - 273.0f;   //датчик выдает 2.73 В при 273 градусах по Кельвину 
}

//Переводит из температуры в градусах Цельсия в температуру представленную дискретами АЦП (в контексте датчика 19.3828)
int CControlApp::ToTemperatureSensor(float celsius_temperature)
{
 float temperature_in_volts = (273.0f + celsius_temperature) / 100.0f;
 return CNumericConv::Round((temperature_in_volts / m_adc_discrete));
}


//-----------------------------------------------------------------------
bool CControlApp::Parse_SENSOR_DAT(BYTE* raw_packet)
{

 if (strlen((char*)raw_packet)!=26)  //размер пакета без сигнального символа, дескриптора
	 return false;

 //частота вращения двигателя
 if (false == CNumericConv::Hex16ToBin(raw_packet,&m_SensorDat.frequen))
     return false;
 raw_packet+=4;  
 
 //давление во впускном коллекторе
 int pressure = 0;
 if (false == CNumericConv::Hex16ToBin(raw_packet,&pressure))
     return false;
 float pressure_in_volts = (((float)pressure) * m_adc_discrete);
 m_SensorDat.pressure = (pressure_in_volts + m_map_sensor_offset) * m_map_sensor_gradient;
 raw_packet+=4;  

 //напряжение бортовой сети
 int voltage = 0;
 if (false == CNumericConv::Hex16ToBin(raw_packet,&voltage))
     return false;
 m_SensorDat.voltage = ((float)voltage) * m_adc_discrete; 
 raw_packet+=4;  

 //Температура охлаждающей жидкости
 int temperature = 0;
 if (false == CNumericConv::Hex16ToBin(raw_packet,&temperature))
     return false;
 m_SensorDat.temperat = FromTemperatureSensor(temperature);
 raw_packet+=4;  

 //Текущий УОЗ 
 int adv_angle = 0;
 if (false == CNumericConv::Hex16ToBin(raw_packet,&adv_angle))
     return false;
 m_SensorDat.adv_angle = ((float)adv_angle) / m_angle_multiplier;
 raw_packet+=4;  

 //Расход воздуха
 if (false == CNumericConv::Hex8ToBin(raw_packet,&m_SensorDat.air_flow))
     return false;
 raw_packet+=2;  

 //Состояние клапана ЭПХХ 
 if (false == CNumericConv::Hex4ToBin(*raw_packet,&m_SensorDat.ephh_valve))
     return false;
 raw_packet+=1;  

 //Состояние дроссельной заслонки 
 if (false == CNumericConv::Hex4ToBin(*raw_packet,&m_SensorDat.carb))
     return false;
 raw_packet+=1;  

 //Состояние газового клапана 
 if (false == CNumericConv::Hex4ToBin(*raw_packet,&m_SensorDat.gas))
     return false;
 raw_packet+=1;  

 if (*raw_packet!='\r')
	 return false;

 return true;
}


//-----------------------------------------------------------------------
bool CControlApp::Parse_FNNAME_DAT(BYTE* raw_packet)
{
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
 if (NULL==(p = strchr((char*)raw_packet,'\r')))
     return false;
 *p = 0;

 m_FnNameDat.name = (char*)raw_packet;

 return true;
}


//-----------------------------------------------------------------------
bool CControlApp::Parse_STARTR_PAR(BYTE* raw_packet)
{

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
bool CControlApp::Parse_ANGLES_PAR(BYTE* raw_packet)
{
 if (strlen((char*)raw_packet)!=13)  //размер пакета без сигнального символа, дескриптора
	 return false;

 //Минимальный, допустимый УОЗ 
 int  min_angle;
 if (false == CNumericConv::Hex16ToBin(raw_packet,&min_angle))
     return false;
 raw_packet+=4;  
 m_AnglesPar.min_angle = ((float)min_angle) / m_angle_multiplier;

 //Максимальный, допустимый УОЗ
 int max_angle;
 if (false == CNumericConv::Hex16ToBin(raw_packet,&max_angle))
     return false;
 raw_packet+=4;
 m_AnglesPar.max_angle = ((float)max_angle) / m_angle_multiplier;

 //Октан-коррекция УОЗ 
 int angle_corr;
 if (false == CNumericConv::Hex16ToBin(raw_packet,&angle_corr))
     return false;
 raw_packet+=4;  
 m_AnglesPar.angle_corr = ((float)angle_corr) / m_angle_multiplier;

 if (*raw_packet!='\r')
	 return false;

 return true;
}

//-----------------------------------------------------------------------
bool CControlApp::Parse_FUNSET_PAR(BYTE* raw_packet)
{
 if (strlen((char*)raw_packet)!=11)  //размер пакета без сигнального символа, дескриптора
	 return false;

 //Номер семейства характеристик используемого для бензина
 if (false == CNumericConv::Hex8ToBin(raw_packet,&m_FunSetPar.fn_benzin))
     return false;
 raw_packet+=2;

 //Номер семейства характеристик используемого для газа
 if (false == CNumericConv::Hex8ToBin(raw_packet,&m_FunSetPar.fn_gas))
     return false;
 raw_packet+=2;

 //Наклон шкалы ДАД (давление в дискретах АЦП)
 unsigned char map_gradient = 0;
 if (false == CNumericConv::Hex8ToBin(raw_packet,&map_gradient))
     return false;
 raw_packet+=2;  
 m_FunSetPar.map_grad = (((float)map_gradient) * m_adc_discrete) * m_map_sensor_gradient;  //пользователю удобно обозревать и редактировать наклон когда он а кПа

 //Перепад давления между атмосферным и максимальным
 int pressure_swing = 0;
 if (false == CNumericConv::Hex16ToBin(raw_packet,&pressure_swing))
     return false;
 raw_packet+=4;  
 m_FunSetPar.press_swing = (((float)pressure_swing) * m_adc_discrete) * m_map_sensor_gradient;

 if (*raw_packet!='\r')
	 return false;

 return true;
}


//-----------------------------------------------------------------------
bool CControlApp::Parse_IDLREG_PAR(BYTE* raw_packet)
{
 if (strlen((char*)raw_packet)!=18)  //размер пакета без сигнального символа, дескриптора
	 return false;

 //признак использования регулятора
 if (false == CNumericConv::Hex4ToBin(*raw_packet,&m_IdlRegPar.idl_regul))
     return false;
 raw_packet+=1;  


 //Коэффициент регулятора при  положительной ошибке 
 int ifac1;
 if (false == CNumericConv::Hex16ToBin(raw_packet,&ifac1))
     return false;
 raw_packet+=4;
 m_IdlRegPar.ifac1 = ((float)ifac1) / ANGLE_MULTIPLAYER;

 //Коэффициент регулятора при  отрицательной ошибке
 int ifac2;
 if (false == CNumericConv::Hex16ToBin(raw_packet,&ifac2))
     return false;
 raw_packet+=4;
 m_IdlRegPar.ifac2 = ((float)ifac2) / ANGLE_MULTIPLAYER;

 //Зона нечувствительности регулятора 
 if (false == CNumericConv::Hex16ToBin(raw_packet,&m_IdlRegPar.MINEFR))
     return false;
 raw_packet+=4;

 //Поддерживаемые обороты 
 if (false == CNumericConv::Hex16ToBin(raw_packet,&m_IdlRegPar.idl_turns))
     return false;
 raw_packet+=4;

 if (*raw_packet!='\r')
	 return false;

 return true;
}

//-----------------------------------------------------------------------
bool CControlApp::Parse_CARBUR_PAR(BYTE* raw_packet)
{
 if (strlen((char*)raw_packet)!=10)  //размер пакета без сигнального символа, дескриптора
	 return false;

 //Нижний порог ЭПХХ 
 if (false == CNumericConv::Hex16ToBin(raw_packet,&m_CarburPar.ephh_lot))
     return false;
 raw_packet+=4;

 //Верхний порог ЭПХХ 
 if (false == CNumericConv::Hex16ToBin(raw_packet,&m_CarburPar.ephh_hit))
     return false;
 raw_packet+=4;

 //Признак инверсии концевика карбюратора 
 if (false == CNumericConv::Hex4ToBin(*raw_packet,&m_CarburPar.carb_invers))
     return false;
 raw_packet+=1;  

 if (*raw_packet!='\r')
	 return false;

 return true;
}

//-----------------------------------------------------------------------
bool CControlApp::Parse_TEMPER_PAR(BYTE* raw_packet)
{
 if (strlen((char*)raw_packet)!=10)  //размер пакета без сигнального символа, дескриптора
	 return false;

 //Признак комплектации ДТОЖ (использования ДТОЖ)
 if (false == CNumericConv::Hex4ToBin(*raw_packet,&m_TemperPar.tmp_use))
     return false;
 raw_packet+=1;  

 //Для удобства и повышения скорости обработки SECU-3 оперирует с температурой представленной 
 //дискретами АЦП (как измеренное значение прямо с датчика)

 //Порог включения вентилятора
 int vent_on = 0;
 if (false == CNumericConv::Hex16ToBin(raw_packet,&vent_on))
     return false;
 raw_packet+=4;
 m_TemperPar.vent_on = FromTemperatureSensor(vent_on);

 //Порог выключения вентилятора 
 int vent_off = 0;
 if (false == CNumericConv::Hex16ToBin(raw_packet,&vent_off))
     return false;
 raw_packet+=4;
 m_TemperPar.vent_off = FromTemperatureSensor(vent_off);

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


	for(it = m_Packets->begin(); it!=m_Packets->end(); ++it)
	{	
		BYTE* raw_packet = (unsigned char*)it->c_str();
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
			 {
	           m_pEventHandler->OnPacketReceived(descriptor,&m_TemperPar);		
			   break;
			 }
			 continue;
		 case CARBUR_PAR: 
			 if (Parse_CARBUR_PAR(raw_packet))
			 {
	           m_pEventHandler->OnPacketReceived(descriptor,&m_CarburPar);		
			   break;
			 }
			 continue;
		 case IDLREG_PAR: 
			 if (Parse_IDLREG_PAR(raw_packet))
			 {
	           m_pEventHandler->OnPacketReceived(descriptor,&m_IdlRegPar);		
			   break;
			 }
			 continue;
		 case ANGLES_PAR:
			 if (Parse_ANGLES_PAR(raw_packet))
			 {
	           m_pEventHandler->OnPacketReceived(descriptor,&m_AnglesPar);		
			   break;
			 }
			 continue;
		 case FUNSET_PAR:
			 if (Parse_FUNSET_PAR(raw_packet))
			 {
	           m_pEventHandler->OnPacketReceived(descriptor,&m_FunSetPar);		
			   break;
			 }
			 continue;
		 case STARTR_PAR:
			 if (Parse_STARTR_PAR(raw_packet))
			 {
	           m_pEventHandler->OnPacketReceived(descriptor,&m_StartrPar);		
			   break;
			 }
			 continue;
		 case FNNAME_DAT:
			 if (Parse_FNNAME_DAT(raw_packet))
			 {
	           m_pEventHandler->OnPacketReceived(descriptor,&m_FnNameDat);		
			   break;
			 }
			 continue;
		 case SENSOR_DAT:					 
			 if (Parse_SENSOR_DAT(raw_packet))
			 {
	           m_pEventHandler->OnPacketReceived(descriptor,&m_SensorDat);		
			   break; //пакет успешно разобран по составляющим
			 }
			 continue; //пакет не прошел сурового отбора нашим жюри :-)
         default:
             continue;
		}//switch        

  	    status = true;
	}//for

 return status;
}


//-----------------------------------------------------------------------
DWORD WINAPI CControlApp::BackgroundProcess(LPVOID lpParameter)
{
  CControlApp* p_capp = (CControlApp*)lpParameter;
  CComPort* p_port = p_capp->m_p_port;
  IAPPEventHandler* pEventHandler; 
  BYTE read_buf[RAW_BYTES_TO_READ_MAX+1];

  DWORD actual_received;
  actual_received = 0;

  //bool online_state = false;  
  //bool first_call_of_OnConnection = false;
  
  while(1) 
  {
	  WaitForSingleObject(p_capp->m_hAwakeEvent,INFINITE); //sleep if need

	  if (p_capp->m_is_thread_must_exit)
		  break;  //поступила команда завершения работы потока

	  pEventHandler = p_capp->m_pEventHandler;
	  ASSERT(pEventHandler); //перед тем как обрабатывать пакеты, необходимо приаттачить обработчик событий
  //    if (!first_call_of_OnConnection)
	//  {
//	    first_call_of_OnConnection = true;
  //      pEventHandler->OnConnection(online_state);	  
	//  }


	  //читаем блок данных
	  p_port->RecvBlock(read_buf,RAW_BYTES_TO_READ_MAX,&actual_received);
	  read_buf[actual_received] = 0;
	  
//    int j=0; while(read_buf[j]!=0){if (read_buf[j]=='\r')  printf("\n");else  printf("%c",read_buf[j]);j++;}

	  //парсим данные и пытаемся выделить пакеты
	  p_capp->SplitPackets(read_buf);

	  //проводим грамматический разбор каждого пакета
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
	  ResetEvent(m_hAwakeEvent);  //поток заснет и не будет отнимать процессорное время
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
void CControlApp::SwitchOn(bool state)
{
	COMMTIMEOUTS timeouts;
	float ms_need_for_one_byte; 
	
    //кол-во мс необходимое для приема/передачи одного байта	
	ms_need_for_one_byte = CComPort::ms_need_for_one_byte_8N1(m_uart_speed);

	if (state)
	{ //возобновить работу
      //перед возобновлением работы необходимо установить параметры (сброс операции при ошибке и таймауты) 
      m_p_port->Purge();

	  m_p_port->AccessDCB()->fAbortOnError = FALSE;     //прекращение операции при ошибке
	  m_p_port->AccessDCB()->BaudRate = m_uart_speed;   //для работы с приложением своя скорость
	  m_p_port->SetState();
   
	  //теперь необходимо настроить таймауты (я нихрена так и не понял ничего в этих таймаутах)
	  timeouts.ReadIntervalTimeout = 0; 
	  timeouts.ReadTotalTimeoutMultiplier = CNumericConv::Round(ms_need_for_one_byte * 2);
      timeouts.ReadTotalTimeoutConstant = 1; 

	  timeouts.WriteTotalTimeoutConstant = 500;
      timeouts.WriteTotalTimeoutMultiplier = CNumericConv::Round(ms_need_for_one_byte * 5);
	  m_p_port->SetTimeouts(&timeouts);	

	  Sleep(CNumericConv::Round(ms_need_for_one_byte*5));
	  m_force_notify_about_connection = true;
      SwitchOnThread(true);
	  SetPacketsTimer(m_dat_packet_timeout);
	}
	else
	{ //остановить работу
      SwitchOnThread(false);
	  Sleep(CNumericConv::Round(ms_need_for_one_byte*5));
	  CancelWaitableTimer(m_hTimer);

	  if (m_pEventHandler)
	    m_pEventHandler->OnConnection(false);	  
//	  	  m_force_notify_about_connection = true;
	}
}

//-----------------------------------------------------------------------
//А вдруг на верхнем уровне иерархии кто нибуть решить использовать некорректные дескрипторы
bool CControlApp::IsValidDescriptor(const BYTE descriptor)
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
  m_outgoing_packet+= '\r';
}

//-----------------------------------------------------------------------
void CControlApp::Build_IDLREG_PAR(IdlRegPar* packet_data)
{
  CNumericConv::Bin4ToHex(packet_data->idl_regul,m_outgoing_packet);

  int ifac1 =  CNumericConv::Round((packet_data->ifac1 * ANGLE_MULTIPLAYER));
  CNumericConv::Bin16ToHex(ifac1,m_outgoing_packet);

  int ifac2 = CNumericConv::Round((packet_data->ifac2 * ANGLE_MULTIPLAYER));
  CNumericConv::Bin16ToHex(ifac2,m_outgoing_packet);

  CNumericConv::Bin16ToHex(packet_data->MINEFR,m_outgoing_packet);
  CNumericConv::Bin16ToHex(packet_data->idl_turns,m_outgoing_packet);
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
  int vent_on = ToTemperatureSensor(packet_data->vent_on);
  CNumericConv::Bin16ToHex(vent_on,m_outgoing_packet);
  int vent_off = ToTemperatureSensor(packet_data->vent_off);
  CNumericConv::Bin16ToHex(vent_off,m_outgoing_packet);
  m_outgoing_packet+= '\r';
}

//-----------------------------------------------------------------------
void CControlApp::Build_ANGLES_PAR(AnglesPar* packet_data)
{
  int min_angle = CNumericConv::Round(packet_data->min_angle * m_angle_multiplier);
  CNumericConv::Bin16ToHex(min_angle,m_outgoing_packet);
  int max_angle = CNumericConv::Round(packet_data->max_angle * m_angle_multiplier);
  CNumericConv::Bin16ToHex(max_angle,m_outgoing_packet);
  int angle_corr = CNumericConv::Round(packet_data->angle_corr * m_angle_multiplier);
  CNumericConv::Bin16ToHex(angle_corr,m_outgoing_packet);
  m_outgoing_packet+= '\r';
}

//-----------------------------------------------------------------------
void CControlApp::Build_FUNSET_PAR(FunSetPar* packet_data)
{
  CNumericConv::Bin4ToHex(packet_data->fn_benzin,m_outgoing_packet);
  CNumericConv::Bin4ToHex(packet_data->fn_gas,m_outgoing_packet);
  unsigned char map_gradient = CNumericConv::Round((packet_data->map_grad / m_map_sensor_gradient) / m_adc_discrete);
  CNumericConv::Bin8ToHex(map_gradient,m_outgoing_packet);
  int press_swing = CNumericConv::Round((packet_data->press_swing / m_map_sensor_gradient) / m_adc_discrete);
  CNumericConv::Bin16ToHex(press_swing,m_outgoing_packet);
  m_outgoing_packet+= '\r';
}

//-----------------------------------------------------------------------
void CControlApp::SetSettings(float i_map_sensor_offset, float i_map_sensor_gradient)
{
  m_map_sensor_offset = i_map_sensor_offset;
  m_map_sensor_gradient = i_map_sensor_gradient;
}

//-----------------------------------------------------------------------
void CControlApp::SetEventHandler(IAPPEventHandler* i_pEventHandler) 
{ 
  m_pEventHandler = i_pEventHandler;
};
//-----------------------------------------------------------------------


