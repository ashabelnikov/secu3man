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
#include "ITabController.h"
#include "ui-core/TabController.h"

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
 //na
}

CMainTabController::~CMainTabController()
{
 //na
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
 int selected_index = m_pTabController->GetCurSel();
 ASSERT(selected_index!=-1);
 return m_controllers_list[selected_index];
}

void CMainTabController::SetTabController(CTabController* i_pTabController) 
{ 
 m_pTabController = i_pTabController;
}

