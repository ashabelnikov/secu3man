 /****************************************************************
 *       SECU-3  - An open source, free engine control unit
 *    Designed by Alexey A. Shabelnikov. Ukraine, Gorlovka 2007.
 *       Microprocessors systems - design & programming.
 *    contacts:
 *              http://secu-3.narod.ru
 *              ICQ: 405-791-931
 ****************************************************************/

#include "stdafx.h"
#include "ControlAppAdapter.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define WM_THREAD_ON_CONNECTON        (WM_USER + 1)
#define WM_THREAD_ON_PACKET_RECEIVED  (WM_USER + 2)


//////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CControlAppAdapter,CWnd)
  ON_MESSAGE(WM_THREAD_ON_CONNECTON, msgOnConnection)
  ON_MESSAGE(WM_THREAD_ON_PACKET_RECEIVED, msgOnPacketReceived)
END_MESSAGE_MAP()


//////////////////////Thread side/////////////////////////////////////
void CControlAppAdapter::OnPacketReceived(const BYTE i_descriptor, SECU3IO::SECU3Packet* ip_packet)
{
  if (IsWindow(m_hWnd) && m_switch_on)
    PostMessage(WM_THREAD_ON_PACKET_RECEIVED,i_descriptor,(LPARAM)ip_packet);	  
}

void CControlAppAdapter::OnConnection(const bool i_online)
{
  if (IsWindow(m_hWnd) && m_switch_on)
    PostMessage(WM_THREAD_ON_CONNECTON,i_online,0);	  
}


/////////////////////Application side/////////////////////////////////
LRESULT CControlAppAdapter::msgOnConnection(WPARAM wParam, LPARAM lParam)
{
  //мы не должны "засыпать" получателя пакетами если он "выключил нас", а выключить он нас мог
  //еще тогда когда в очереди сообщений были сообщения для него.
  if (!m_switch_on) 
    return 0;
  ASSERT(m_destination_handler);
  m_destination_handler->OnConnection(wParam ? true : false);  
  return 0;
}

LRESULT CControlAppAdapter::msgOnPacketReceived(WPARAM wParam, LPARAM lParam)
{
  if (!m_switch_on) 
    return 0;
  ASSERT(m_destination_handler);
  m_destination_handler->OnPacketReceived((BYTE)wParam,(SECU3IO::SECU3Packet*)lParam);
  return 0;
}

//////////////////////////////////////////////////////////////////////
