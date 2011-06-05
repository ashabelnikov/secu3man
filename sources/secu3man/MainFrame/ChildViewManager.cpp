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
#include "ChildViewManager.h"

#include "ChildView.h"
#include "MainFrame.h"
#include "TabsManagement/MainTabManager.h"

CChildViewManager::CChildViewManager()
: mp_wndView(new CChildView)
, mp_MainTabManager(new CMainTabManager)
{
 //na
}

CChildViewManager::~CChildViewManager()
{
 //na
}

bool CChildViewManager::Init(CMainFrame* i_pMainFrame)
{
 ASSERT(i_pMainFrame);
 ASSERT(mp_MainTabManager.get());
 // create a view to occupy the client area of the frame
 if (!mp_wndView->Create(i_pMainFrame))
 {
  TRACE0("Failed to create view window\n");
  return false;
 }
 //Set standard ID for this window can be attached to framework.
 mp_wndView->SetDlgCtrlID(AFX_IDW_PANE_FIRST);

 i_pMainFrame->SetView(mp_wndView.get());
 mp_MainTabManager->Init(mp_wndView.get());
 return true;
}

void CChildViewManager::OnAfterCreate(void)
{
 mp_MainTabManager->OnAfterCreate();
}

bool CChildViewManager::OnClose(void)
{
 return mp_MainTabManager->OnClose();
}

bool CChildViewManager::OnAskFullScreen(void)
{
 return mp_MainTabManager->OnAskFullScreen();
}

void CChildViewManager::OnFullScreen(bool i_what)
{
 mp_MainTabManager->OnFullScreen(i_what);
}
