/* SECU-3  - An open source, free engine control unit
   Copyright (C) 2007 Alexey A. Shabelnikov. Ukraine, Kiev

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
#include "Resources/resource.h"
#include "DiagnostContextMenuManager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDiagnostContextMenuManager::CDiagnostContextMenuManager(CWnd* pWnd /*= NULL*/)
: m_pWnd(pWnd)
{
 //empty
}

CDiagnostContextMenuManager::~CDiagnostContextMenuManager()
{
 //empty
}

void CDiagnostContextMenuManager::Attach(CWnd* pWnd)
{
 ASSERT(pWnd);
 m_pWnd = pWnd;
}

//Fill menu with items and submenus
void CDiagnostContextMenuManager::CreateContent(void)
{
 m_ParentMenu.CreatePopupMenu();

 m_ParentMenu.AppendMenu(MF_STRING,IDM_DEV_DIAG_START_OUTAUTO_TST,MLL::LoadString(IDS_DEV_DIAG_START_OUTAUTO_TST));
 m_ParentMenu.AppendMenu(MF_STRING,IDM_DEV_DIAG_STOP_OUTAUTO_TST,MLL::LoadString(IDS_DEV_DIAG_STOP_OUTAUTO_TST));
 m_ParentMenu.AppendMenu(MF_SEPARATOR);
 m_ParentMenu.AppendMenu(MF_STRING,IDM_DEV_DIAG_ENABLE_BLDE_TST,MLL::LoadString(IDS_DEV_DIAG_ENABLE_BLDE_TST));
}

//показывает контекстное меню
void CDiagnostContextMenuManager::TrackPopupMenu(int x, int y)
{
 ASSERT(m_pWnd);
 m_ParentMenu.TrackPopupMenu(TPM_LEFTALIGN,x,y,m_pWnd);
}

//This method should be called when a pop-up menu is about to become active
void CDiagnostContextMenuManager::OnInitMenuPopup(CMenu* pMenu, UINT nIndex, BOOL bSysMenu)
{
 ASSERT(m_pWnd);

 if (bSysMenu)
  return;     // don't support system menu

 ASSERT(pMenu != NULL);
 // check the enabled state of various menu items

 CCmdUI state;
 state.m_pMenu = pMenu;
 ASSERT(state.m_pOther == NULL);
 ASSERT(state.m_pParentMenu == NULL);

 // determine if menu is popup in top-level menu and set m_pOther to
 //  it if so (m_pParentMenu == NULL indicates that it is secondary popup)
 HMENU hParentMenu;
 if (AfxGetThreadState()->m_hTrackingMenu == pMenu->m_hMenu)
  state.m_pParentMenu = pMenu;    // parent == child for tracking popup
 else if ((hParentMenu = ::GetMenu(m_pWnd->m_hWnd)) != NULL)
 {
  CWnd* pParent = m_pWnd->GetTopLevelParent();
  // child windows don't have menus -- need to go to the top!
  if (pParent != NULL && (hParentMenu = ::GetMenu(pParent->m_hWnd)) != NULL)
  {
   int nIndexMax = ::GetMenuItemCount(hParentMenu);
   for (int nIndex = 0; nIndex < nIndexMax; nIndex++)
   {
    if (::GetSubMenu(hParentMenu, nIndex) == pMenu->m_hMenu)
    {
     // when popup is found, m_pParentMenu is containing menu
     state.m_pParentMenu = CMenu::FromHandle(hParentMenu);
     break;
    }
   }
  }
 }

 state.m_nIndexMax = pMenu->GetMenuItemCount();
 for (state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax; state.m_nIndex++)
 {
  state.m_nID = pMenu->GetMenuItemID(state.m_nIndex);
  if (state.m_nID == 0)
   continue; // menu separator or invalid cmd - ignore it

  ASSERT(state.m_pOther == NULL);
  ASSERT(state.m_pMenu != NULL);
  if (state.m_nID == (UINT)-1)
  {
   // possibly a popup menu, route to first item of that popup
   state.m_pSubMenu = pMenu->GetSubMenu(state.m_nIndex);
   if (state.m_pSubMenu == NULL ||
    (state.m_nID = state.m_pSubMenu->GetMenuItemID(0)) == 0 || state.m_nID == (UINT)-1)
   {
    continue;       // first item of popup can't be routed to
   }
   state.DoUpdate(m_pWnd, FALSE);    // popups are never auto disabled
  }
  else
  {
   // normal menu item
   // Auto enable/disable if frame window has 'm_bAutoMenuEnable'
   // set and command is _not_ a system command.
   state.m_pSubMenu = NULL;
   state.DoUpdate(m_pWnd, TRUE);
  }

  // adjust for menu deletions and additions
  UINT nCount = pMenu->GetMenuItemCount();
  if (nCount < state.m_nIndexMax)
  {
   state.m_nIndex -= (state.m_nIndexMax - nCount);
   while (state.m_nIndex < nCount && pMenu->GetMenuItemID(state.m_nIndex) == state.m_nID)
   {
    state.m_nIndex++;
   }
  }
  state.m_nIndexMax = nCount;
 }//for
}

CMenu& CDiagnostContextMenuManager::GetParentMenu(void)
{
 return m_ParentMenu;
}