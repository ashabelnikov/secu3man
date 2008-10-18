
#pragma once

#include "ControlApp.h"

////////////////////////////////////////////////////////////////////////

//Этот адаптер необходим для разрешения проблемы вызова MFC функций из "неродного" потока

//ACHTUNG! CWnd должен наследоваться первым, иначе класс будет неправильно работать!
class AFX_EXT_CLASS CControlAppAdapter : public CWnd, public IAPPEventHandler
{
  public:
	  CControlAppAdapter(IAPPEventHandler* i_destination_handler)
	  : m_destination_handler(i_destination_handler)
	  , m_switch_on(true)
	  { 
	   ASSERT(i_destination_handler);	  
	  };

      virtual ~CControlAppAdapter() {};

	  //невидимое окно необходимо нам только для того чтобы посылать самим себе сообщения!
      BOOL Create(CWnd* pParentWnd)
	  {
        ASSERT(pParentWnd);		
		return CWnd::Create(NULL,_T("CControlApp_Adapter_Wnd"),0,CRect(0,0,0,0),pParentWnd,0);  
	  }

	  //Включает/выключает адаптер. Если адаптер выключен, то он не перенаправляет 
	  //пакеты с данными получателю
	  void SwitchOn(bool state)
	  {
	    m_switch_on = state;
	  };

  private:
      IAPPEventHandler* m_destination_handler;
	  bool m_switch_on;


  	  //from IAPPEventHandler, called by thread:
	  virtual void OnPacketReceived(const BYTE i_descriptor, SECU3IO::SECU3Packet* ip_packet);
	  virtual void OnConnection(const bool i_online);

  protected:

	  afx_msg LRESULT msgOnConnection(WPARAM wParam, LPARAM lParam);
	  afx_msg LRESULT msgOnPacketReceived(WPARAM wParam, LPARAM lParam);

	  DECLARE_MESSAGE_MAP();
};
////////////////////////////////////////////////////////////////////////
