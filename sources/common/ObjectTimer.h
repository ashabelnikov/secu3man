 /****************************************************************
 *
 *  Created by Alexey A. Shabelnikov. Ukraine, Gorlovka 2008. 
 *   ICQ: 405-791-931. e-mail: shabelnikov-stc@mail.ru
 *  Microprocessors systems - design & programming.
 *
 *****************************************************************/

#pragma once

#include <map>


//эта глобалная переменная необходима для передачи в обработчики правильного this, выбор элементов
//производится по идентификатору таймера, каждый элемент содержит указатель на CObjectTimer 
static std::map<int,void*> g_object_timer_instance_map;


template<class T> class CObjectTimer  
{
 public:
  typedef void (T::*msgHandlerType) (void);

  CObjectTimer(T* ip) : m_pDispatcher(ip), msgHandler(NULL), m_interval_ms(0), m_timer_id(0) {};
  CObjectTimer()      : m_pDispatcher(NULL), msgHandler(NULL), m_interval_ms(0), m_timer_id(0) {};

  //при удалении очередного объекта-таймера мапа становится меньше на один элемент
  virtual ~CObjectTimer() 
  {
   KillTimer();	
  };

  //устанавливает все необходимые параметры и запускает таймер 
  void SetTimer(T* object, msgHandlerType i_function, int interval)
  {
   SetMsgHandler(i_function);
   m_pDispatcher = object;
   SetTimer(interval);
   m_interval_ms = interval;
  };

  //запускать таймер этой функцией можно только если предварительно в конструктор был передан 
  //указатель на объект и была вызвана функция SetMsgHandler() для установки обработчика
  void SetTimer(int interval)
  {
   ASSERT(m_timer_id == 0);
   if (m_timer_id != 0)
    return; //already installed!

   m_timer_id = ::SetTimer(NULL,0,interval,(TIMERPROC)&TimerProc);
   ASSERT(m_timer_id); //failed to set timer!
   g_object_timer_instance_map[m_timer_id] = this;
   m_interval_ms = interval;
  }

  //убивать таймер этой функцией 
  void KillTimer(void)
  {
   ::KillTimer(NULL,m_timer_id);
   if (!g_object_timer_instance_map.empty())
   { 
    if (g_object_timer_instance_map.find(m_timer_id)!=g_object_timer_instance_map.end())	  
     g_object_timer_instance_map.erase(m_timer_id);  	 
    else
     ASSERT(m_timer_id == 0); //timer id is non-zero but we can not find it in map!
   }
   m_timer_id = 0; //indicate that we reset the timer
  }

  //эта функция принимает на входе адрес функции-члена: void T::Function(void)
  inline void SetMsgHandler(msgHandlerType i_function)
  {
   msgHandler = i_function;
  }

  //версия функции которая принимает адрес обьекта и адрес функции
  inline void SetMsgHandler(T* ip_this, msgHandlerType i_function)
  {
   m_pDispatcher = ip_this;
   msgHandler = i_function;
  }

  //возвращает текущий период таймера
  int GetPeriod(void) const
  {
   return m_interval_ms;
  }

 private:
    
  static inline VOID CALLBACK TimerProc(HWND hwnd,UINT uMsg,UINT_PTR idEvent,DWORD dwTime)
  {
   //welcome to hell :-D ... а кто сказал что в С++ есть делегаты!?
   CObjectTimer* p_this = reinterpret_cast<CObjectTimer*>(g_object_timer_instance_map[idEvent]); 
   ASSERT(p_this);
   if (!p_this)  return;

   T *x = p_this->m_pDispatcher;
   ASSERT(x);
   if (!x) return;
	  	  
   //делегируем выполнение операции указанному методу класса T, только если хендлер проинициализирован
   if (p_this->msgHandler!=NULL)
   {
    (x->*(p_this->msgHandler))(); 	  
   }
  };

 private: //internal data
  msgHandlerType msgHandler;
  T*   m_pDispatcher; 
  UINT_PTR m_timer_id;
  int  m_interval_ms;
};
