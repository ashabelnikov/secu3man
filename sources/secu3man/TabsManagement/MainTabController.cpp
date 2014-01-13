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
              http://secu-3.org
              email: shabelnikov@secu-3.org
*/

#include "stdafx.h"
#include "MainFrame/ChildView.h"
#include "MainTabController.h"
#include "ITabController.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMainTabController::CMainTabController(CChildView* ip_view)
: mp_view(ip_view)
{
 //empty
}

CMainTabController::~CMainTabController()
{
 //empty
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
 ASSERT(mp_view);
 m_controllers_list[mp_view->GetCurSel()]->OnActivate();
}

bool CMainTabController::OnSelchangingTabctl(void)
{
 if (!m_controllers_list[mp_view->GetCurSel()]->OnAskChangeTab())
  return false; //current tab controller did not allowed changing of tab
 ASSERT(m_controllers_list.size());
 ASSERT(mp_view);
 m_controllers_list[mp_view->GetCurSel()]->OnDeactivate();
 return true;
}

ITabController* CMainTabController::GetActiveController() const
{
 int selected_index = mp_view->GetCurSel();
 ASSERT(selected_index!=-1);
 return m_controllers_list[selected_index];
}
