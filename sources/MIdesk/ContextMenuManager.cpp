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
 m_addMenu.AppendMenu(MF_STRING, IDM_MI_MET_MAF, MLL::LoadString(IDS_MI_MAF_TITLE));
 m_addMenu.AppendMenu(MF_STRING, IDM_MI_MET_VENTDUTY, MLL::LoadString(IDS_MI_VENT_DUTY_TITLE));
 m_addMenu.AppendMenu(MF_STRING, IDM_MI_MET_MAPDOT, MLL::LoadString(IDS_MI_MAPDOT_TITLE));
 m_addMenu.AppendMenu(MF_STRING, IDM_MI_MET_FTS, MLL::LoadString(IDS_MI_FTS_TITLE));
 m_addMenu.AppendMenu(MF_STRING, IDM_MI_MET_EGOCORR2, MLL::LoadString(IDS_MI_EGO_CORR2_TITLE));
 m_addMenu.AppendMenu(MF_STRING, IDM_MI_MET_SENSAFR2, MLL::LoadString(IDS_MI_SENSAFR2_TITLE));
 m_addMenu.AppendMenu(MF_STRING, IDM_MI_MET_TARGAFR, MLL::LoadString(IDS_MI_TARGAFR_TITLE));
 m_addMenu.AppendMenu(MF_STRING, IDM_MI_MET_DIFFAFR, MLL::LoadString(IDS_MI_DIFFAFR_TITLE));
 m_addMenu.AppendMenu(MF_STRING, IDM_MI_MET_DIFFAFR2, MLL::LoadString(IDS_MI_DIFFAFR2_TITLE));
 m_addMenu.AppendMenu(MF_STRING, IDM_MI_MET_GPS, MLL::LoadString(IDS_MI_GPS_TITLE));
 m_addMenu.AppendMenu(MF_STRING, IDM_MI_MET_FPS, MLL::LoadString(IDS_MI_FPS_TITLE));

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
 m_addGraphMenu.AppendMenu(MF_STRING, IDM_MI_GRH_MAF, MLL::LoadString(IDS_MI_MAF_TITLE));
 m_addGraphMenu.AppendMenu(MF_STRING, IDM_MI_GRH_VENTDUTY, MLL::LoadString(IDS_MI_VENT_DUTY_TITLE));
 m_addGraphMenu.AppendMenu(MF_STRING, IDM_MI_GRH_MAPDOT, MLL::LoadString(IDS_MI_MAPDOT_TITLE));
 m_addGraphMenu.AppendMenu(MF_STRING, IDM_MI_GRH_FTS, MLL::LoadString(IDS_MI_FTS_TITLE));
 m_addGraphMenu.AppendMenu(MF_STRING, IDM_MI_GRH_EGOCORR2, MLL::LoadString(IDS_MI_EGO_CORR2_TITLE));
 m_addGraphMenu.AppendMenu(MF_STRING, IDM_MI_GRH_SENSAFR2, MLL::LoadString(IDS_MI_SENSAFR2_TITLE));
 m_addGraphMenu.AppendMenu(MF_STRING, IDM_MI_GRH_TARGAFR, MLL::LoadString(IDS_MI_TARGAFR_TITLE));
 m_addGraphMenu.AppendMenu(MF_STRING, IDM_MI_GRH_DIFFAFR, MLL::LoadString(IDS_MI_DIFFAFR_TITLE));
 m_addGraphMenu.AppendMenu(MF_STRING, IDM_MI_GRH_DIFFAFR2, MLL::LoadString(IDS_MI_DIFFAFR2_TITLE));
 m_addGraphMenu.AppendMenu(MF_STRING, IDM_MI_GRH_GPS, MLL::LoadString(IDS_MI_GPS_TITLE));
 m_addGraphMenu.AppendMenu(MF_STRING, IDM_MI_GRH_FPS, MLL::LoadString(IDS_MI_FPS_TITLE));

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
 m_addMenu.AppendMenu(MF_STRING, IDM_MI_IND_UNIOUT1, MLL::LoadString(IDS_MI_IND_UNIOUT1));
 m_addMenu.AppendMenu(MF_STRING, IDM_MI_IND_UNIOUT2, MLL::LoadString(IDS_MI_IND_UNIOUT2));
 m_addMenu.AppendMenu(MF_STRING, IDM_MI_IND_UNIOUT3, MLL::LoadString(IDS_MI_IND_UNIOUT3));
 m_addMenu.AppendMenu(MF_STRING, IDM_MI_IND_UNIOUT4, MLL::LoadString(IDS_MI_IND_UNIOUT4));
 m_addMenu.AppendMenu(MF_STRING, IDM_MI_IND_UNIOUT5, MLL::LoadString(IDS_MI_IND_UNIOUT5));
 m_addMenu.AppendMenu(MF_STRING, IDM_MI_IND_UNIOUT6, MLL::LoadString(IDS_MI_IND_UNIOUT6));
 m_addMenu.AppendMenu(MF_STRING, IDM_MI_IND_GPA4_I, MLL::LoadString(IDS_MI_IND_GPA4_I));
 m_addMenu.AppendMenu(MF_STRING, IDM_MI_IND_INPUT1, MLL::LoadString(IDS_MI_IND_INPUT1));
 m_addMenu.AppendMenu(MF_STRING, IDM_MI_IND_INPUT2, MLL::LoadString(IDS_MI_IND_INPUT2));
 m_addMenu.AppendMenu(MF_STRING, IDM_MI_IND_AUTO_I, MLL::LoadString(IDS_MI_IND_AUTO_I));
 m_addMenu.AppendMenu(MF_STRING, IDM_MI_IND_MAPSEL0, MLL::LoadString(IDS_MI_IND_MAPSEL0));
 m_addMenu.AppendMenu(MF_STRING, IDM_MI_IND_REFPRS_I, MLL::LoadString(IDS_MI_IND_REFPRS_I));
 m_addMenu.AppendMenu(MF_STRING, IDM_MI_IND_ALTRN_I, MLL::LoadString(IDS_MI_IND_ALTRN_I));

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

CMenu& CIndContextMenuManager::GetParentMenu(void)
{
 return m_parentMenu;
}
