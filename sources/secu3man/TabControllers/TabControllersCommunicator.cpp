/****************************************************************
 *       SECU-3  - An open source, free engine control unit
 *    Designed by Alexey A. Shabelnikov. Ukraine, Gorlovka 2007.
 *       Microprocessors systems - design & programming.
 *    contacts:
 *              http://secu-3.narod.ru
 *              ICQ: 405-791-931
 ****************************************************************/

#include "stdafx.h"
#include "TabControllersCommunicator.h"
#include "TabsManagement\ITabController.h"

TabControllersCommunicator::TabControllersCommunicator()
{
 //todo 
}

TabControllersCommunicator::~TabControllersCommunicator()
{
 //todo
}

/*static*/ TabControllersCommunicator* TabControllersCommunicator::GetInstance(void)
{
 static TabControllersCommunicator communicator;
 return &communicator;
}

void TabControllersCommunicator::AddReference(ITabController* ip_controller, int i_key)
{
 std::map<int, ITabController*>::const_iterator it = m_references.find(i_key);
 if (it==m_references.end())
  m_references.insert(std::make_pair(i_key, ip_controller));
 else
  ASSERT(0);
}
  
ITabController* TabControllersCommunicator::GetReference(int i_key)
{
 std::map<int, ITabController*>::const_iterator it = m_references.find(i_key);
 if (it!=m_references.end())
  return (*it).second;

 return NULL;
}
