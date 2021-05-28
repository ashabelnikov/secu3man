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

/** \file ContextMenuManager.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "resource.h"
#include "ContextMenuManager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMetContextMenuManager::CMetContextMenuManager(CWnd* pWnd /*= NULL*/)
: m_pWnd(pWnd)
{
 //empty
}

CMetContextMenuManager::~CMetContextMenuManager()
{
 //empty
}

void CMetContextMenuManager::Attach(CWnd* pWnd)
{
 ASSERT(pWnd);
 m_pWnd = pWnd;
}

//Fill menu with items and submenus
void CMetContextMenuManager::CreateContent(void)
{
 //"Add gauge" submenu
 m_addMenu.CreatePopupMenu();
 m_addMenu.AppendMenu(MF_STRING, IDM_MI_MET_RPM, MLL::LoadString(IDS_MI_TACHOMETER_TITLE));
 m_addMenu.AppendMenu(MF_STRING, IDM_MI_MET_MAP, MLL::LoadString(IDS_MI_PRESSURE_TITLE));
 m_addMenu.AppendMenu(MF_STRING, IDM_MI_MET_VBAT, MLL::LoadString(IDS_MI_VOLTAGE_TITLE));
 m_addMenu.AppendMenu(MF_STRING, IDM_MI_MET_IGNTIM, MLL::LoadString(IDS_MI_ADVANGLE_TITLE));
 m_addMenu.AppendMenu(MF_STRING, IDM_MI_MET_CLT, MLL::LoadString(IDS_MI_TEMPERATURE_TITLE));
 m_addMenu.AppendMenu(MF_STRING, IDM_MI_MET_ADDI1, MLL::LoadString(IDS_MI_ADD_I1_TITLE));
 m_addMenu.AppendMenu(MF_STRING, IDM_MI_MET_ADDI2, MLL::LoadString(IDS_MI_ADD_I2_TITLE));
 m_addMenu.AppendMenu(MF_STRING, IDM_MI_MET_INJPW, MLL::LoadString(IDS_MI_INJ_PW_TITLE));
 m_addMenu.AppendMenu(MF_STRING, IDM_MI_MET_IAT, MLL::LoadString(IDS_MI_IAT_TITLE));
 m_addMenu.AppendMenu(MF_STRING, IDM_MI_MET_EGOCORR, MLL::LoadString(IDS_MI_EGO_CORR_TITLE));
 m_addMenu.AppendMenu(MF_STRING, IDM_MI_MET_TPS, MLL::LoadString(IDS_MI_THROTTLE_GATE_TITLE));
 m_addMenu.AppendMenu(MF_STRING, IDM_MI_MET_AIRFLOW, MLL::LoadString(IDS_MI_AIR_FLOW_TITLE));
 m_addMenu.AppendMenu(MF_STRING, IDM_MI_MET_VEHSPD, MLL::LoadString(IDS_MI_VSS_TITLE));
 m_addMenu.AppendMenu(MF_STRING, IDM_MI_MET_TPSDOT, MLL::LoadString(IDS_MI_TPSDOT_TITLE));
 m_addMenu.AppendMenu(MF_STRING, IDM_MI_MET_MAP2, MLL::LoadString(IDS_MI_MAP2_TITLE));
 m_addMenu.AppendMenu(MF_STRING, IDM_MI_MET_MAPD, MLL::LoadString(IDS_MI_MAPD_TITLE));
 m_addMenu.AppendMenu(MF_STRING, IDM_MI_MET_TMP2, MLL::LoadString(IDS_MI_TMP2_TITLE));
 m_addMenu.AppendMenu(MF_STRING, IDM_MI_MET_FUELCONSUM, MLL::LoadString(IDS_MI_FUELCONSUM_TITLE));
 m_addMenu.AppendMenu(MF_STRING, IDM_MI_MET_KNOCKRETARD, MLL::LoadString(IDS_MI_KNOCKRETARD_TITLE));
 m_addMenu.AppendMenu(MF_STRING, IDM_MI_MET_KNOCKGRAPH, MLL::LoadString(IDS_MI_KNOCK_TITLE));
 m_addMenu.AppendMenu(MF_STRING, IDM_MI_MET_SENSAFR, MLL::LoadString(IDS_MI_SENSAFR_TITLE));
 m_addMenu.AppendMenu(MF_STRING, IDM_MI_MET_CHOKEPOS, MLL::LoadString(IDS_MI_CHOKEPOS_TITLE));
 m_addMenu.AppendMenu(MF_STRING, IDM_MI_MET_GDPOS, MLL::LoadString(IDS_MI_GDPOS_TITLE));
 m_addMenu.AppendMenu(MF_STRING, IDM_MI_MET_SYNLOAD, MLL::LoadString(IDS_MI_LOAD_TITLE));
 m_addMenu.AppendMenu(MF_STRING, IDM_MI_MET_INJTIMB, MLL::LoadString(IDS_MI_INJTIMB_TITLE));
 m_addMenu.AppendMenu(MF_STRING, IDM_MI_MET_INJTIME, MLL::LoadString(IDS_MI_INJTIME_TITLE));
 m_addMenu.AppendMenu(MF_STRING, IDM_MI_MET_FUELCONSUMF, MLL::LoadString(IDS_MI_FUELCONSUMF_TITLE));
 m_addMenu.AppendMenu(MF_STRING, IDM_MI_MET_GRTS, MLL::LoadString(IDS_MI_GRTS_TITLE));
 m_addMenu.AppendMenu(MF_STRING, IDM_MI_MET_FTLS, MLL::LoadString(IDS_MI_FTLS_TITLE));
 m_addMenu.AppendMenu(MF_STRING, IDM_MI_MET_EGTS, MLL::LoadString(IDS_MI_EGTS_TITLE));
 m_addMenu.AppendMenu(MF_STRING, IDM_MI_MET_OPS, MLL::LoadString(IDS_MI_OPS_TITLE));
 m_addMenu.AppendMenu(MF_STRING, IDM_MI_MET_INJDUTY, MLL::LoadString(IDS_MI_INJ_DUTY_TITLE));

 //"Add graph" submenu
 m_addGraphMenu.CreatePopupMenu();
 m_addGraphMenu.AppendMenu(MF_STRING, IDM_MI_GRH_RPM, MLL::LoadString(IDS_MI_TACHOMETER_TITLE));
 m_addGraphMenu.AppendMenu(MF_STRING, IDM_MI_GRH_MAP, MLL::LoadString(IDS_MI_PRESSURE_TITLE));
 m_addGraphMenu.AppendMenu(MF_STRING, IDM_MI_GRH_VBAT, MLL::LoadString(IDS_MI_VOLTAGE_TITLE));
 m_addGraphMenu.AppendMenu(MF_STRING, IDM_MI_GRH_IGNTIM, MLL::LoadString(IDS_MI_ADVANGLE_TITLE));
 m_addGraphMenu.AppendMenu(MF_STRING, IDM_MI_GRH_CLT, MLL::LoadString(IDS_MI_TEMPERATURE_TITLE));
 m_addGraphMenu.AppendMenu(MF_STRING, IDM_MI_GRH_ADDI1, MLL::LoadString(IDS_MI_ADD_I1_TITLE));
 m_addGraphMenu.AppendMenu(MF_STRING, IDM_MI_GRH_ADDI2, MLL::LoadString(IDS_MI_ADD_I2_TITLE));
 m_addGraphMenu.AppendMenu(MF_STRING, IDM_MI_GRH_INJPW, MLL::LoadString(IDS_MI_INJ_PW_TITLE));
 m_addGraphMenu.AppendMenu(MF_STRING, IDM_MI_GRH_IAT, MLL::LoadString(IDS_MI_IAT_TITLE));
 m_addGraphMenu.AppendMenu(MF_STRING, IDM_MI_GRH_EGOCORR, MLL::LoadString(IDS_MI_EGO_CORR_TITLE));
 m_addGraphMenu.AppendMenu(MF_STRING, IDM_MI_GRH_TPS, MLL::LoadString(IDS_MI_THROTTLE_GATE_TITLE));
 m_addGraphMenu.AppendMenu(MF_STRING, IDM_MI_GRH_AIRFLOW, MLL::LoadString(IDS_MI_AIR_FLOW_TITLE));
 m_addGraphMenu.AppendMenu(MF_STRING, IDM_MI_GRH_VEHSPD, MLL::LoadString(IDS_MI_VSS_TITLE));
 m_addGraphMenu.AppendMenu(MF_STRING, IDM_MI_GRH_TPSDOT, MLL::LoadString(IDS_MI_TPSDOT_TITLE));
 m_addGraphMenu.AppendMenu(MF_STRING, IDM_MI_GRH_MAP2, MLL::LoadString(IDS_MI_MAP2_TITLE));
 m_addGraphMenu.AppendMenu(MF_STRING, IDM_MI_GRH_MAPD, MLL::LoadString(IDS_MI_MAPD_TITLE));
 m_addGraphMenu.AppendMenu(MF_STRING, IDM_MI_GRH_TMP2, MLL::LoadString(IDS_MI_TMP2_TITLE));
 m_addGraphMenu.AppendMenu(MF_STRING, IDM_MI_GRH_FUELCONSUM, MLL::LoadString(IDS_MI_FUELCONSUM_TITLE));
 m_addGraphMenu.AppendMenu(MF_STRING, IDM_MI_GRH_KNOCKRETARD, MLL::LoadString(IDS_MI_KNOCKRETARD_TITLE));
 m_addGraphMenu.AppendMenu(MF_STRING, IDM_MI_GRH_KNOCKGRAPH, MLL::LoadString(IDS_MI_KNOCK_TITLE));
 m_addGraphMenu.AppendMenu(MF_STRING, IDM_MI_GRH_SENSAFR, MLL::LoadString(IDS_MI_SENSAFR_TITLE));
 m_addGraphMenu.AppendMenu(MF_STRING, IDM_MI_GRH_CHOKEPOS, MLL::LoadString(IDS_MI_CHOKEPOS_TITLE));
 m_addGraphMenu.AppendMenu(MF_STRING, IDM_MI_GRH_GDPOS, MLL::LoadString(IDS_MI_GDPOS_TITLE));
 m_addGraphMenu.AppendMenu(MF_STRING, IDM_MI_GRH_SYNLOAD, MLL::LoadString(IDS_MI_LOAD_TITLE));
 m_addGraphMenu.AppendMenu(MF_STRING, IDM_MI_GRH_INJTIMB, MLL::LoadString(IDS_MI_INJTIMB_TITLE));
 m_addGraphMenu.AppendMenu(MF_STRING, IDM_MI_GRH_INJTIME, MLL::LoadString(IDS_MI_INJTIME_TITLE));
 m_addGraphMenu.AppendMenu(MF_STRING, IDM_MI_GRH_FUELCONSUMF, MLL::LoadString(IDS_MI_FUELCONSUMF_TITLE));
 m_addGraphMenu.AppendMenu(MF_STRING, IDM_MI_GRH_GRTS, MLL::LoadString(IDS_MI_GRTS_TITLE));
 m_addGraphMenu.AppendMenu(MF_STRING, IDM_MI_GRH_FTLS, MLL::LoadString(IDS_MI_FTLS_TITLE));
 m_addGraphMenu.AppendMenu(MF_STRING, IDM_MI_GRH_EGTS, MLL::LoadString(IDS_MI_EGTS_TITLE));
 m_addGraphMenu.AppendMenu(MF_STRING, IDM_MI_GRH_OPS, MLL::LoadString(IDS_MI_OPS_TITLE));
 m_addGraphMenu.AppendMenu(MF_STRING, IDM_MI_GRH_INJDUTY, MLL::LoadString(IDS_MI_INJ_DUTY_TITLE));


 //"Number of rows" submenu
 m_rowsMenu.CreatePopupMenu();
 m_rowsMenu.AppendMenu(MF_STRING, IDM_MI_MET_ROWS1, _T("1"));
 m_rowsMenu.AppendMenu(MF_STRING, IDM_MI_MET_ROWS2, _T("2"));
 m_rowsMenu.AppendMenu(MF_STRING, IDM_MI_MET_ROWS3, _T("3"));
 m_rowsMenu.AppendMenu(MF_STRING, IDM_MI_MET_ROWS4, _T("4"));

 //"Title font" submenu
 m_titleFontMenu.CreatePopupMenu();
 m_titleFontMenu.AppendMenu(MF_STRING, IDM_MI_MET_TITLE_FONT050, _T("50%"));
 m_titleFontMenu.AppendMenu(MF_STRING, IDM_MI_MET_TITLE_FONT060, _T("60%"));
 m_titleFontMenu.AppendMenu(MF_STRING, IDM_MI_MET_TITLE_FONT070, _T("70%"));
 m_titleFontMenu.AppendMenu(MF_STRING, IDM_MI_MET_TITLE_FONT080, _T("80%"));
 m_titleFontMenu.AppendMenu(MF_STRING, IDM_MI_MET_TITLE_FONT090, _T("90%"));
 m_titleFontMenu.AppendMenu(MF_STRING, IDM_MI_MET_TITLE_FONT100, _T("100%"));
 m_titleFontMenu.AppendMenu(MF_STRING, IDM_MI_MET_TITLE_FONT110, _T("110%"));
 m_titleFontMenu.AppendMenu(MF_STRING, IDM_MI_MET_TITLE_FONT120, _T("120%"));
 m_titleFontMenu.AppendMenu(MF_STRING, IDM_MI_MET_TITLE_FONT130, _T("130%"));
 m_titleFontMenu.AppendMenu(MF_STRING, IDM_MI_MET_TITLE_FONT140, _T("140%"));
 m_titleFontMenu.AppendMenu(MF_STRING, IDM_MI_MET_TITLE_FONT150, _T("150%"));

 //"Value font" submenu
 m_valueFontMenu.CreatePopupMenu();
 m_valueFontMenu.AppendMenu(MF_STRING, IDM_MI_MET_VALUE_FONT050, _T("50%"));
 m_valueFontMenu.AppendMenu(MF_STRING, IDM_MI_MET_VALUE_FONT060, _T("60%"));
 m_valueFontMenu.AppendMenu(MF_STRING, IDM_MI_MET_VALUE_FONT070, _T("70%"));
 m_valueFontMenu.AppendMenu(MF_STRING, IDM_MI_MET_VALUE_FONT080, _T("80%"));
 m_valueFontMenu.AppendMenu(MF_STRING, IDM_MI_MET_VALUE_FONT090, _T("90%"));
 m_valueFontMenu.AppendMenu(MF_STRING, IDM_MI_MET_VALUE_FONT100, _T("100%"));
 m_valueFontMenu.AppendMenu(MF_STRING, IDM_MI_MET_VALUE_FONT110, _T("110%"));
 m_valueFontMenu.AppendMenu(MF_STRING, IDM_MI_MET_VALUE_FONT120, _T("120%"));
 m_valueFontMenu.AppendMenu(MF_STRING, IDM_MI_MET_VALUE_FONT130, _T("130%"));
 m_valueFontMenu.AppendMenu(MF_STRING, IDM_MI_MET_VALUE_FONT140, _T("140%"));
 m_valueFontMenu.AppendMenu(MF_STRING, IDM_MI_MET_VALUE_FONT150, _T("150%"));

 //"Pane font" submenu
 m_paneFontMenu.CreatePopupMenu();
 m_paneFontMenu.AppendMenu(MF_STRING, IDM_MI_MET_PANE_FONT050, _T("50%"));
 m_paneFontMenu.AppendMenu(MF_STRING, IDM_MI_MET_PANE_FONT060, _T("60%"));
 m_paneFontMenu.AppendMenu(MF_STRING, IDM_MI_MET_PANE_FONT070, _T("70%"));
 m_paneFontMenu.AppendMenu(MF_STRING, IDM_MI_MET_PANE_FONT080, _T("80%"));
 m_paneFontMenu.AppendMenu(MF_STRING, IDM_MI_MET_PANE_FONT090, _T("90%"));
 m_paneFontMenu.AppendMenu(MF_STRING, IDM_MI_MET_PANE_FONT100, _T("100%"));
 m_paneFontMenu.AppendMenu(MF_STRING, IDM_MI_MET_PANE_FONT110, _T("110%"));
 m_paneFontMenu.AppendMenu(MF_STRING, IDM_MI_MET_PANE_FONT120, _T("120%"));
 m_paneFontMenu.AppendMenu(MF_STRING, IDM_MI_MET_PANE_FONT130, _T("130%"));
 m_paneFontMenu.AppendMenu(MF_STRING, IDM_MI_MET_PANE_FONT140, _T("140%"));
 m_paneFontMenu.AppendMenu(MF_STRING, IDM_MI_MET_PANE_FONT150, _T("150%"));

 //"Label font" submenu
 m_labelFontMenu.CreatePopupMenu();
 m_labelFontMenu.AppendMenu(MF_STRING, IDM_MI_MET_LABEL_FONT050, _T("50%"));
 m_labelFontMenu.AppendMenu(MF_STRING, IDM_MI_MET_LABEL_FONT060, _T("60%"));
 m_labelFontMenu.AppendMenu(MF_STRING, IDM_MI_MET_LABEL_FONT070, _T("70%"));
 m_labelFontMenu.AppendMenu(MF_STRING, IDM_MI_MET_LABEL_FONT080, _T("80%"));
 m_labelFontMenu.AppendMenu(MF_STRING, IDM_MI_MET_LABEL_FONT090, _T("90%"));
 m_labelFontMenu.AppendMenu(MF_STRING, IDM_MI_MET_LABEL_FONT100, _T("100%"));
 m_labelFontMenu.AppendMenu(MF_STRING, IDM_MI_MET_LABEL_FONT110, _T("110%"));
 m_labelFontMenu.AppendMenu(MF_STRING, IDM_MI_MET_LABEL_FONT120, _T("120%"));
 m_labelFontMenu.AppendMenu(MF_STRING, IDM_MI_MET_LABEL_FONT130, _T("130%"));
 m_labelFontMenu.AppendMenu(MF_STRING, IDM_MI_MET_LABEL_FONT140, _T("140%"));
 m_labelFontMenu.AppendMenu(MF_STRING, IDM_MI_MET_LABEL_FONT150, _T("150%"));

 //top level menu
 m_parentMenu.CreatePopupMenu();
 m_parentMenu.AppendMenu(MF_POPUP | MF_STRING, (UINT)m_addMenu.m_hMenu, MLL::LoadString(IDS_MI_MET_ADD_GAUGE));
 m_parentMenu.AppendMenu(MF_POPUP | MF_STRING, (UINT)m_addGraphMenu.m_hMenu, MLL::LoadString(IDS_MI_GRH_ADD_GAUGE));
 m_parentMenu.AppendMenu(MF_STRING, IDM_MI_MET_DEL_GAUGE, MLL::LoadString(IDS_MI_MET_DEL_GAUGE));
 m_parentMenu.AppendMenu(MF_POPUP | MF_STRING, (UINT)m_rowsMenu.m_hMenu, MLL::LoadString(IDS_MI_MET_NUMROWS));
 m_parentMenu.AppendMenu(MF_POPUP | MF_STRING, (UINT)m_titleFontMenu.m_hMenu, MLL::LoadString(IDS_MI_MET_TITLE_FONT));
 m_parentMenu.AppendMenu(MF_POPUP | MF_STRING, (UINT)m_valueFontMenu.m_hMenu, MLL::LoadString(IDS_MI_MET_VALUE_FONT));
 m_parentMenu.AppendMenu(MF_POPUP | MF_STRING, (UINT)m_paneFontMenu.m_hMenu, MLL::LoadString(IDS_MI_MET_PANE_FONT));
 m_parentMenu.AppendMenu(MF_POPUP | MF_STRING, (UINT)m_labelFontMenu.m_hMenu, MLL::LoadString(IDS_MI_MET_LABEL_FONT));
 m_parentMenu.AppendMenu(MF_STRING, IDM_MI_MET_ENDRAGNDROP, MLL::LoadString(IDS_MI_MET_ENDRAGNDROP));
}

//Shows context menu
void CMetContextMenuManager::TrackPopupMenu(int x, int y)
{
 ASSERT(m_pWnd);
 m_parentMenu.TrackPopupMenu(TPM_LEFTALIGN,x,y,m_pWnd);
}

//This method should be called when a pop-up menu is about to become active
void CMetContextMenuManager::OnInitMenuPopup(CMenu* pMenu, UINT nIndex, BOOL bSysMenu)
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

CMenu& CMetContextMenuManager::GetParentMenu(void)
{
 return m_parentMenu;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIndContextMenuManager::CIndContextMenuManager(CWnd* pWnd /*= NULL*/)
: m_pWnd(pWnd)
{
 //empty
}

CIndContextMenuManager::~CIndContextMenuManager()
{
 //empty
}

void CIndContextMenuManager::Attach(CWnd* pWnd)
{
 ASSERT(pWnd);
 m_pWnd = pWnd;
}

//Fill menu with items and submenus
void CIndContextMenuManager::CreateContent(void)
{
 //"Add indicator" submenu
 m_addMenu.CreatePopupMenu();
 m_addMenu.AppendMenu(MF_STRING, IDM_MI_IND_GAS_V, MLL::LoadString(IDS_MI_IND_GAS_V));
 m_addMenu.AppendMenu(MF_STRING, IDM_MI_IND_CARB, MLL::LoadString(IDS_MI_IND_CARB));
 m_addMenu.AppendMenu(MF_STRING, IDM_MI_IND_IDLEVALVE, MLL::LoadString(IDS_MI_IND_IDLEVALVE));
 m_addMenu.AppendMenu(MF_STRING, IDM_MI_IND_POWERVALVE, MLL::LoadString(IDS_MI_IND_POWERVALVE));
 m_addMenu.AppendMenu(MF_STRING, IDM_MI_IND_STBLOCK, MLL::LoadString(IDS_MI_IND_STBLOCK));
 m_addMenu.AppendMenu(MF_STRING, IDM_MI_IND_AE, MLL::LoadString(IDS_MI_IND_AE));
 m_addMenu.AppendMenu(MF_STRING, IDM_MI_IND_COOLINGFAN, MLL::LoadString(IDS_MI_IND_COOLINGFAN));
 m_addMenu.AppendMenu(MF_STRING, IDM_MI_IND_CE, MLL::LoadString(IDS_MI_IND_CE));
 m_addMenu.AppendMenu(MF_STRING, IDM_MI_IND_FCREVLIM, MLL::LoadString(IDS_MI_IND_FCREVLIM));
 m_addMenu.AppendMenu(MF_STRING, IDM_MI_IND_FLOODCLEAR, MLL::LoadString(IDS_MI_IND_FLOODCLEAR));
 m_addMenu.AppendMenu(MF_STRING, IDM_MI_IND_SYSLOCKED, MLL::LoadString(IDS_MI_IND_SYSLOCKED));
 m_addMenu.AppendMenu(MF_STRING, IDM_MI_IND_IGN_I, MLL::LoadString(IDS_MI_IND_IGN_I));
 m_addMenu.AppendMenu(MF_STRING, IDM_MI_IND_COND_I, MLL::LoadString(IDS_MI_IND_COND_I));
 m_addMenu.AppendMenu(MF_STRING, IDM_MI_IND_EPAS_I, MLL::LoadString(IDS_MI_IND_EPAS_I));
 m_addMenu.AppendMenu(MF_STRING, IDM_MI_IND_AFTSTRENR, MLL::LoadString(IDS_MI_IND_AFTSTRENR));
 m_addMenu.AppendMenu(MF_STRING, IDM_MI_IND_IACCLLOOP, MLL::LoadString(IDS_MI_IND_IACCLLOOP));

 //"Number of rows" submenu
 m_rowsMenu.CreatePopupMenu();
 m_rowsMenu.AppendMenu(MF_STRING, IDM_MI_IND_ROWS1, _T("1"));
 m_rowsMenu.AppendMenu(MF_STRING, IDM_MI_IND_ROWS2, _T("2"));
 m_rowsMenu.AppendMenu(MF_STRING, IDM_MI_IND_ROWS3, _T("3"));
 m_rowsMenu.AppendMenu(MF_STRING, IDM_MI_IND_ROWS4, _T("4"));

 //"Percent of height" submenu
 m_perhMenu.CreatePopupMenu();
 m_perhMenu.AppendMenu(MF_STRING, IDM_MI_IND_PERCHEIGHT03, _T("3%"));
 m_perhMenu.AppendMenu(MF_STRING, IDM_MI_IND_PERCHEIGHT05, _T("5%"));
 m_perhMenu.AppendMenu(MF_STRING, IDM_MI_IND_PERCHEIGHT07, _T("7%"));
 m_perhMenu.AppendMenu(MF_STRING, IDM_MI_IND_PERCHEIGHT10, _T("10%"));
 m_perhMenu.AppendMenu(MF_STRING, IDM_MI_IND_PERCHEIGHT15, _T("15%"));

 //top level menu
 m_parentMenu.CreatePopupMenu();
 m_parentMenu.AppendMenu(MF_POPUP | MF_STRING, (UINT)m_addMenu.m_hMenu, MLL::LoadString(IDS_MI_IND_ADD_INDICATOR));
 m_parentMenu.AppendMenu(MF_STRING, IDM_MI_IND_DEL_INDICATOR, MLL::LoadString(IDS_MI_IND_DEL_INDICATOR));
 m_parentMenu.AppendMenu(MF_POPUP | MF_STRING, (UINT)m_rowsMenu.m_hMenu, MLL::LoadString(IDS_MI_IND_NUMROWS));
 m_parentMenu.AppendMenu(MF_STRING, IDM_MI_IND_SET_COLOR, MLL::LoadString(IDS_MI_IND_SET_COLOR));
 m_parentMenu.AppendMenu(MF_POPUP | MF_STRING, (UINT)m_perhMenu.m_hMenu, MLL::LoadString(IDS_MI_IND_PERCHEIGHT));
 m_parentMenu.AppendMenu(MF_STRING, IDM_MI_IND_ENDRAGNDROP, MLL::LoadString(IDS_MI_IND_ENDRAGNDROP));
}

//Shows context menu
void CIndContextMenuManager::TrackPopupMenu(int x, int y)
{
 ASSERT(m_pWnd);
 m_parentMenu.TrackPopupMenu(TPM_LEFTALIGN,x,y,m_pWnd);
}

//This method should be called when a pop-up menu is about to become active
void CIndContextMenuManager::OnInitMenuPopup(CMenu* pMenu, UINT nIndex, BOOL bSysMenu)
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

CMenu& CIndContextMenuManager::GetParentMenu(void)
{
 return m_parentMenu;
}
