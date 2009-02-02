 /****************************************************************
 *       SECU-3  - An open source, free engine control unit
 *    Designed by Alexey A. Shabelnikov. Ukraine, Gorlovka 2007.
 *       Microprocessors systems - design & programming.
 *    contacts:
 *              http://secu-3.narod.ru
 *              ICQ: 405-791-931
 ****************************************************************/

#include "stdafx.h"
#include "MainTabController.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMainTabController::CMainTabController()
: m_pTabController(NULL)
{

}

CMainTabController::~CMainTabController()
{

}


void CMainTabController::AddTabController(ITabController* i_pCntr)
{
  m_controllers_list.push_back(i_pCntr);
}


std::vector<ITabController*>& CMainTabController::GetControllersList(void)
{
  return m_controllers_list;
}

void CMainTabController::OnSelchangeTabctl(void)
{
 ASSERT(m_controllers_list.size());
 ASSERT(m_pTabController);
 m_controllers_list[m_pTabController->GetCurSel()]->OnActivate();
}
 
void CMainTabController::OnSelchangingTabctl(void)
{
 ASSERT(m_controllers_list.size());
 ASSERT(m_pTabController);
 m_controllers_list[m_pTabController->GetCurSel()]->OnDeactivate();
} 


ITabController* CMainTabController::GetActiveController() const
{
return m_controllers_list[m_pTabController->GetCurSel()];
}
