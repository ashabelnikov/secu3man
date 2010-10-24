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
              http://secu-3.narod.ru
              email: secu-3@yandex.ru
*/

#include "stdafx.h"
#include "MainTabController.h"
#include "ITabController.h"
#include "ui-core\TabController.h"

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
