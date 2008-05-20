 /****************************************************************
 *       SECU-3  - An open source, free engine control unit
 *    Designed by Alexey A. Shabelnikov. Ukraine, Gorlovka 2007.
 *       Microprocessors systems - design & programming.
 *    contacts:
 *              http://secu-3.narod.ru
 *              ICQ: 405-791-931
 ****************************************************************/

#include "stdafx.h"
#include "secu3man.h"
#include "ParamMonTabController.h"
#include "common/FastDelegate.h"
#include <map>
#include <algorithm>

#include "io-core/ufcodes.h"


using namespace fastdelegate;
using namespace std; 
using namespace SECU3IO;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CParamMonTabController::CParamMonTabController(CParamMonTabDlg* i_view, CCommunicationManager* i_comm, CStatusBarManager* i_sbar)
: m_view(NULL)
, m_comm(NULL)
, m_sbar(NULL)
, m_pAdapter(NULL)
, m_operation_state(-1)
, m_packet_processing_state(PPS_READ_MONITOR_DATA)
, m_parameters_changed(false)
, m_lastSel(0)

{
  //инициализируем указатели на вспомогательные объекты
  m_view = i_view;
  m_comm = i_comm;
  m_sbar = i_sbar;

  m_view->m_ParamDeskDlg.SetOnTabActivate(MakeDelegate(this,&CParamMonTabController::OnParamDeskTabActivate));
  m_view->m_ParamDeskDlg.SetOnChangeInTab(MakeDelegate(this,&CParamMonTabController::OnParamDeskChangeInTab));

  CWnd* pParent = AfxGetApp()->m_pMainWnd;

  //адаптер будет синхронизировать вызовы приходящие из потока с данным контроллером
  m_pAdapter = new CControlAppAdapter(this);
  m_pAdapter->Create(pParent);

}


CParamMonTabController::~CParamMonTabController()
{
  delete m_pAdapter;
}

//изменились настройки программы!
void CParamMonTabController::OnSettingsChanged(void)
{
  //включаем необходимый для данного контекста коммуникационный контроллер
  m_comm->m_pControlApp->SwitchOn(true);
  m_comm->m_pBootLoader->SwitchOn(false);  
}

//from ParamDesk
void CParamMonTabController::OnParamDeskTabActivate(void)
{
  //после появления новой вкладки необходимо прочитать для нее параметры из SECU
  StartReadingNecessaryParameters();
}

//from ParamDesk
void CParamMonTabController::OnParamDeskChangeInTab(void)
{
  m_parameters_changed = true;
}


//from MainTabController
void CParamMonTabController::OnActivate(void)
{
  //выбираем ранее выбранную вкладку на панели параметров	
  bool result = m_view->m_ParamDeskDlg.SetCurSel(m_lastSel);

  //////////////////////////////////////////////////////////////////
  //устанавливаем обработчики событий специфичные для контекста программы в котором работает контроллер
  m_comm->m_pControlApp->SetEventHandler(m_pAdapter); 
  m_comm->SetOnSettingsChanged(MakeDelegate(this,&CParamMonTabController::OnSettingsChanged)); 
  //////////////////////////////////////////////////////////////////
 
  m_pAdapter->SwitchOn(true); 

  //включаем необходимый для данного контекста коммуникационный контроллер
  m_comm->m_pControlApp->SwitchOn(true);
  m_comm->m_pBootLoader->SwitchOn(false);

  //запускаем таймер по которому будет ограничиваться частота посылки данных в SECU-3
  m_pd_changes_timer.SetTimer(this,&CParamMonTabController::OnParamDeskChangesTimer,500);

  //запускаем процесс инициализации данными из SECU 
  StartCollectingInitialData();
}

//from MainTabController
void CParamMonTabController::OnDeactivate(void)
{
  m_pAdapter->SwitchOn(false); 

  //таймер работает только если мы находимся в контексте "параметров и монитора"
  m_pd_changes_timer.KillTimer(); 

  //запоминаем номер последней выбранной вкладки на панели параметров
  m_lastSel = m_view->m_ParamDeskDlg.GetCurSel();

  m_sbar->SetInformationText(_T(""));
}


void CParamMonTabController::StartCollectingInitialData(void) 
{
  m_comm->m_pControlApp->ChangeContext(FNNAME_DAT); //change context!

  m_packet_processing_state = PPS_COLLECT_INITIAL_DATA;
  m_operation_state = 0;
}


//возвращает true когда работа автомата завершена
//m_operation_state = 0 для запуска
bool CParamMonTabController::CollectInitialDataFromSECU(const BYTE i_descriptor, const void* i_packet_data)
{
  static std::vector<_TSTRING> fn_names; //Achtung! singleton.
  static std::vector<int> fn_indexes;        //Achtung! singleton.

  const FnNameDat* fn_data = NULL;

  //state machine 
  switch(m_operation_state)
  {
    case 0: //пытаемся добится от SECU-3 чтобы оно посылало пакеты с информацией о семействах характеристик
	{///////////
      m_sbar->SetInformationText("Чтение конфигурации..."); 

	  if (i_descriptor!=FNNAME_DAT)
	  {
        m_comm->m_pControlApp->ChangeContext(FNNAME_DAT); //!!!		  
	  }
	  else
	  { //контроллер шлет информацию о семействах характеристик
	    m_operation_state = 1; 
		 
   	    fn_data = (FnNameDat*)i_packet_data;
		 
		fn_names.clear();
	    fn_indexes.clear();
        fn_names.resize(fn_data->tables_num);
		 
		//данные в SECU хранятся в виде ASCII, их надо преобразовать в UNICODE
		TCHAR name_string[256];
		OemToChar(fn_data->name.c_str(),name_string);
		fn_names[fn_data->index] = name_string;

	    fn_indexes.push_back(fn_data->index);
	  }
	}///////////
    break;	

    case 1: //принимает (продолжает принимать) и сохраняем информацию о всех семействах характеристик
	{///////////
	  if (i_descriptor!=FNNAME_DAT)
	  {
        m_operation_state = 0;		  
        break; 
	  }

	  fn_data = (FnNameDat*)i_packet_data;
          
  	  TCHAR name_string[256];
	  OemToChar(fn_data->name.c_str(),name_string);
	  fn_names[fn_data->index] = name_string;

	  fn_indexes.push_back(fn_data->index);

      bool all_names_received = true;
	  for(int i = 0; i < fn_data->tables_num; i++)
	  {
	    if (std::find(fn_indexes.begin(),fn_indexes.end(),i)==fn_indexes.end())
		{
		  all_names_received = false;
		}
	  }

      if (all_names_received) //вся информация о семейтвах характеристик принята
	  {
		m_operation_state = -1;  //останов КА - операции выполнены
        m_view->m_ParamDeskDlg.SetFunctionsNames(fn_names);
        return true; //операции выполнены
	  }
	}/////////
    break;	
  }//switch

  return false; //КА продолжает работу...
}


void CParamMonTabController::StartReadingNecessaryParameters(void) 
{
  BYTE view_descriptor = m_view->m_ParamDeskDlg.GetCurrentDescriptor();
  m_comm->m_pControlApp->ChangeContext(view_descriptor);  //change context!	  

  m_packet_processing_state = PPS_READ_NECESSARY_PARAMETERS;
  m_operation_state = 0;
}


//возвращает true когда работа автомата завершена
//m_operation_state = 0 для запуска
bool CParamMonTabController::ReadNecessaryParametersFromSECU(const BYTE i_descriptor, const void* i_packet_data)
{
  m_sbar->SetInformationText("Чтение параметров..."); 

  switch(m_operation_state)
  {
    case 0:  //ожидаем пакета с указанными нами параметрами
	{
	  BYTE view_descriptor = m_view->m_ParamDeskDlg.GetCurrentDescriptor();
      if (i_descriptor!=view_descriptor)
	  {
        m_comm->m_pControlApp->ChangeContext(view_descriptor);	//!!!	  		  
	  }
	  else
	  {//тот что надо!
	    m_view->m_ParamDeskDlg.SetValues(i_descriptor,i_packet_data);
           
	    //процесс инициализации окончен
        m_operation_state = -1; //останов КА - операции выполнены
 	    m_sbar->SetInformationText("Готово.");
        return true; //операции выполнены
	  }
	}	
    break;
  }//switch

  return false; //КА продолжает работу...
}


//hurrah!!! получен пакет от SECU-3
void CParamMonTabController::OnPacketReceived(const BYTE i_descriptor, const void* i_packet_data)
{
  //обработка приходящих пакетов в зависимости от текущего режима
  switch(m_packet_processing_state)
  {
    case PPS_COLLECT_INITIAL_DATA:  //инициализация данными из SECU-3	
	  if (CollectInitialDataFromSECU(i_descriptor,i_packet_data))
	    StartReadingNecessaryParameters();
	break;

	case PPS_READ_NECESSARY_PARAMETERS:  //чтение указанных параметров
	  if (ReadNecessaryParametersFromSECU(i_descriptor,i_packet_data))
	  {
	    m_packet_processing_state = PPS_BEFORE_READ_MONITOR_DATA;

	    //конфигурация прочитана - можно разрешать панели
        bool state = m_comm->m_pControlApp->GetOnlineStatus();
        m_view->m_ParamDeskDlg.Enable(state);
	  }
	break;
	
    case PPS_BEFORE_READ_MONITOR_DATA: //в этот режим мы попадаем только один раз
	  if (i_descriptor!=SENSOR_DAT)
	  {
        m_comm->m_pControlApp->ChangeContext(SENSOR_DAT); //!!!		  		
	  }
	  else
	  {
        //устанавливаем значения приборов, разрешаем их и переходим в основной режим
	    m_view->m_MIDeskDlg.SetValues((SensorDat*)(i_packet_data)); 	
        bool state = m_comm->m_pControlApp->GetOnlineStatus();
	    m_view->m_MIDeskDlg.Enable(state);
	    m_packet_processing_state = PPS_READ_MONITOR_DATA;
	  }
	break;

	case PPS_READ_MONITOR_DATA:  //получение данных для монитора
	  if (i_descriptor!=SENSOR_DAT)
	  {
        m_comm->m_pControlApp->ChangeContext(SENSOR_DAT); //!!!		  		
	  }
	  else
	  {
	    m_view->m_MIDeskDlg.SetValues((SensorDat*)(i_packet_data)); 	
	  }
	break;	
	}//switch
}

//состояние подключения изменилось!
void CParamMonTabController::OnConnection(const bool i_online)
{
  int state;
  ASSERT(m_sbar);

  if (i_online) //перешли в онлайн
  {
	state = CStatusBarManager::STATE_ONLINE;
	StartCollectingInitialData();
  }
  else
  {
	state = CStatusBarManager::STATE_OFFLINE;  
  }
 
  if (i_online==false) //здесь мы можем только запрещать панели, а разрешать их будем только тогда, когда прочитана конфигурация 
  {
    m_view->m_MIDeskDlg.Enable(i_online);
    m_view->m_ParamDeskDlg.Enable(i_online);
  }

  m_sbar->SetConnectionState(state);
}



//передача пакетов с параметрами в SECU будет происходить не чаще чем вызов этого обработчика
void CParamMonTabController::OnParamDeskChangesTimer(void)
{ 
  if (m_parameters_changed)
  {
    //получаем данные от view и сохраняем их во временный буфер 
    BYTE packet_data[1024];
    BYTE view_descriptor = m_view->m_ParamDeskDlg.GetCurrentDescriptor();
    m_view->m_ParamDeskDlg.GetValues(view_descriptor,packet_data);

    //послали измененные пользователем данные (эта операция блокирует поток, поэтому за данные в стеке можно не беспокоиться)
    m_comm->m_pControlApp->SendPacket(view_descriptor,packet_data);

    m_parameters_changed = false; //обработали событие - сбрасываем признак
  }
}
