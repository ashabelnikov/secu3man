
#pragma once

#include "ControlApp.h"
#include "common/unicodesupport.h"
#include <map>

////////////////////////////////////////////////////////////////////////

//Этот адаптер необходим для разрешения проблемы вызова MFC функций из "неродного" потока

//ACHTUNG! CWnd должен наследоваться первым, иначе класс будет неправильно работать!
class AFX_EXT_CLASS CControlAppAdapter : public CWnd, public IAPPEventHandler
{
  public:
	  CControlAppAdapter();	  
      virtual ~CControlAppAdapter();

	  //невидимое окно необходимо нам только для того чтобы посылать самим себе сообщения!
      BOOL Create(CWnd* pParentWnd);	  

	  //Включает/выключает адаптер. Если адаптер выключен, то он не перенаправляет 
	  //пакеты с данными получателю
	  void SwitchOn(bool state);

	  //добавление обозревателя
      bool AddEventHandler(IAPPEventHandler* i_pEventHandler, const _TSTRING &i_observer_key);

	  //удаление обозревателя
      bool RemoveEventHandler(const _TSTRING &i_observer_key);

  protected:
	  afx_msg LRESULT msgOnConnection(WPARAM wParam, LPARAM lParam);
	  afx_msg LRESULT msgOnPacketReceived(WPARAM wParam, LPARAM lParam);

	  DECLARE_MESSAGE_MAP();           

  private:
      typedef std::map<_TSTRING, IAPPEventHandler*> Observers;
      typedef std::map<_TSTRING, IAPPEventHandler*>::iterator ObserversIterator;

      Observers m_observers;                //список обозревателей событий      
	  bool m_switch_on;
	  bool m_switch_on_thread_side;

      inline void _UpdateInternalState(void);

  	  //from IAPPEventHandler, called by thread:
	  virtual void OnPacketReceived(const BYTE i_descriptor, SECU3IO::SECU3Packet* ip_packet);
	  virtual void OnConnection(const bool i_online);
};
////////////////////////////////////////////////////////////////////////
