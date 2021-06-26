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

/** \file LPTablesController.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "LPTablesController.h"

#include "common/FastDelegate.h"
#include "common/MathHelpers.h"
#include "common/GDIHelpers.h"
#include "io-core/EEPROMDataMediator.h"
#include "io-core/FirmwareDataMediator.h"
#include "io-core/FirmwareMapsDataHolder.h"
#include "io-core/BitMask.h"
#include "io-core/SECU3IO.h"
#include "Settings/ISettingsData.h"
#include "TabDialogs/LPControlPanelDlg.h"
#include "TabControllersCommunicator.h"
#include "FirmwareTabController.h"
#include "EEPROMTabController.h"
#include "TablDesk/GridModeEditorIgnDlg.h"
#include "TablDesk/GridModeEditorInjDlg.h"
#include "TablDesk/MapIds.h"
#include "TablDesk/DynamicValues.h"

#undef max

using namespace fastdelegate;
//using namespace SECU3IO;

CLPTablesController::CLPTablesController(CLPControlPanelDlg* ip_view, ISettingsData* ip_settings)
: mp_view(ip_view)
, mp_settings(ip_settings)
, MapWndScrPos(ip_settings)
, mp_fwdcntr(NULL)
, mp_eedcntr(NULL)
, m_current_funset_index(-1)
, m_children_charts(false)
, m_toggleMapWnd(false)
, m_openedChart(NULL)
, m_firmware_opened(false)
, m_eeprom_opened(false)
{
 m_ve2_map_load_slots.reserve(32);
 m_ve2_map_load_slots = MathHelpers::BuildGridFromRange(0.0f, 100.0f, 16, true); //<--reverse order

 memset(m_rpm_grid_values, 0, 16 * sizeof(float));
 memset(m_clt_grid_values, 0, 16 * sizeof(float));
 memset(m_load_grid_values, 0, 16 * sizeof(float));
  
 mp_view->setOnSelectMapset(MakeDelegate(this,&CLPTablesController::_OnSelectMapSet));
 mp_view->setOnGmeIgnButton(MakeDelegate(this, &CLPTablesController::_OnGmeIgnButton));
 mp_view->setOnGmeInjButton(MakeDelegate(this, &CLPTablesController::_OnGmeInjButton));
}

CLPTablesController::~CLPTablesController()
{
 //empty
}

//изменились настройки программы!
void CLPTablesController::OnSettingsChanged(int action)
{
 if (action == 2)
 {
  _MakeChartsChildren(mp_settings->GetChildCharts());
  return;
 }

 if (action == 3)
 {
  m_toggleMapWnd = mp_settings->GetToggleMapWnd();
  return;
 }
}

//from MainTabController
void CLPTablesController::OnActivate(void)
{
 std::vector<_TSTRING> mapsets;
 //load list of names from firmware
 mp_fwdcntr = static_cast<CFirmwareTabController*>
 (TabControllersCommunicator::GetInstance()->GetReference(TCC_FIRMWARE_TAB_CONTROLLER));
 m_firmware_opened = mp_fwdcntr->IsFirmwareOpened();
 if (m_firmware_opened)
 {
  CFirmwareDataMediator *p_fwdm = mp_fwdcntr->GetFWDM();
  mapsets = p_fwdm->GetFunctionsSetNames();
 }

 //load list of names from EEPROM
 mp_eedcntr = static_cast<CEEPROMTabController*>
 (TabControllersCommunicator::GetInstance()->GetReference(TCC_EEPROM_TAB_CONTROLLER));
 m_eeprom_opened = mp_eedcntr->IsEEPROMOpened();
 if (m_eeprom_opened)
 {
  EEPROMDataMediator *p_eedm = mp_eedcntr->GetEEDM();
  std::vector<_TSTRING> eesets = p_eedm->GetFunctionsSetNames();
  std::copy(eesets.begin(), eesets.end(), std::back_inserter(mapsets));
 }

 if (m_firmware_opened && m_eeprom_opened)
  mp_view->FillMapSetCombo(mapsets);
 else
  mp_view->EnableMapSetCombo(false);

 mp_view->EnableGmeButtons(false);

 if (m_firmware_opened)
 { //load x-axis grids
  CFirmwareDataMediator *p_fwdm = mp_fwdcntr->GetFWDM();
  p_fwdm->GetRPMGridMap(m_rpm_grid_values);
  p_fwdm->GetCLTGridMap(m_clt_grid_values);
  p_fwdm->GetLoadGridMap(m_load_grid_values);  
 }

 _MakeChartsChildren(mp_settings->GetChildCharts());
 m_toggleMapWnd = mp_settings->GetToggleMapWnd();

 //restore previous selection
 if (m_current_funset_index != -1)
 {
  mp_view->EnableGmeButtons(true);
  mp_view->SetMapSetSelection(m_current_funset_index);
 }
}

void CLPTablesController::OnDeactivate(void)
{
 if (mp_gridModeEditorIgnDlg.get())
 {
  _OnCloseMapWnd(mp_gridModeEditorIgnDlg->m_hWnd, TYPE_MAP_GME_IGN_WND);
  mp_gridModeEditorIgnDlg->DestroyWindow();
  mp_gridModeEditorIgnDlg.reset(NULL);
 }

 if (mp_gridModeEditorInjDlg.get())
 {
  _OnCloseMapWnd(mp_gridModeEditorInjDlg->m_hWnd, TYPE_MAP_GME_INJ_WND);
  mp_gridModeEditorInjDlg->DestroyWindow();
  mp_gridModeEditorInjDlg.reset(NULL);
 }

 m_firmware_opened = false;
 m_eeprom_opened = false;
}

void CLPTablesController::DisplayCurrentRecord(SECU3IO::SensorDat* sd)
{
 if (m_firmware_opened && (mp_gridModeEditorIgnDlg.get() || mp_gridModeEditorInjDlg.get()))
 {
  TablDesk::DynVal dv;
  dv.rpm = sd->frequen;
  dv.temp = sd->temperat;
  dv.air_flow = sd->air_flow;
  dv.adv_ang = sd->adv_angle;
  dv.knock_retard = sd->knock_retard;
  dv.knkret_use = sd->knkret_use;
  dv.strt_aalt = sd->strt_aalt;
  dv.strt_use = sd->strt_use;
  dv.idle_aalt = sd->idle_aalt;
  dv.idle_use = sd->idle_use;
  dv.work_aalt = sd->work_aalt;
  dv.work_use = sd->work_use;
  dv.temp_aalt = sd->temp_aalt;
  dv.temp_use = sd->temp_use;
  dv.airt_aalt = sd->airt_aalt;
  dv.airt_use = sd->airt_use;
  dv.idlreg_aac = sd->idlreg_aac;
  dv.idlreg_use = sd->idlreg_use;
  dv.octan_aac = sd->octan_aac;
  dv.octan_use = sd->octan_use;
  dv.tps = sd->tps;
  dv.iac_pos = sd->choke_pos;
  dv.tpsdot = sd->tpsdot;
  dv.voltage = sd->voltage;
  dv.add_i1 = sd->add_i1;
  dv.tmp2 = sd->tmp2;      //GTS
  dv.baro_press = sd->baro_press;
  dv.load = sd->load;
  dv.afr = sd->afr;
  dv.acceleration = sd->acceleration;
  dv.ie = sd->ephh_valve;
  dv.air_temp = sd->air_temp;
  dv.rigid_arg = sd->rigid_arg;
  dv.rigid_use = sd->rigid_use;
  dv.map2 = sd->map2; //GPS
  dv.rxlaf = sd->rxlaf; //RxL air flow
  dv.aftstr_enr = sd->aftstr_enr;
  if (mp_gridModeEditorIgnDlg.get())
   mp_gridModeEditorIgnDlg->SetDynamicValues(dv);
  if (mp_gridModeEditorInjDlg.get())
   mp_gridModeEditorInjDlg->SetDynamicValues(dv);
 }
}

void CLPTablesController::_MakeChartsChildren(bool children)
{
 m_children_charts = children;

 if (mp_gridModeEditorIgnDlg.get() && mp_gridModeEditorIgnDlg->GetSafeHwnd())
  GDIHelpers::MakeWindowChild(mp_gridModeEditorIgnDlg->m_hWnd, children);

 if (mp_gridModeEditorInjDlg.get() && mp_gridModeEditorInjDlg->GetSafeHwnd())
  GDIHelpers::MakeWindowChild(mp_gridModeEditorInjDlg->m_hWnd, children);
}

bool CLPTablesController::_IsAllowed(void)
{
 return true;
}

void CLPTablesController::_OnSelectMapSet(void)
{
 mp_view->EnableGmeButtons(true);
 int index = mp_view->GetMapSetSelection();

 if (index != -1 && m_firmware_opened && m_eeprom_opened)
 {
  m_current_funset_index = index;

  if (mp_gridModeEditorInjDlg.get())
   mp_gridModeEditorInjDlg->SetSplitAngMode(_GetSplitAngMode()); //splitting

  if (m_current_funset_index < TABLES_NUMBER)
  { //from flash
   int index = m_current_funset_index;
   CFirmwareDataMediator *p_fwdm = mp_fwdcntr->GetFWDM();  
   p_fwdm->GetStartMap(index, startMap, false);  
   p_fwdm->GetIdleMap(index, idleMap, false);
   p_fwdm->GetWorkMap(index, workMap, false);
   p_fwdm->GetTempMap(index, tempMap, false);
   p_fwdm->GetTempIdlMap(index, tempIMap, false);

   p_fwdm->GetVEMap(index, veMap, false);
   p_fwdm->GetVE2Map(index, ve2Map, false);
   p_fwdm->GetAFRMap(index, afrMap, false);
   p_fwdm->GetITMap(index, itMap, false);
   p_fwdm->GetIdlrMap(index, idlrMap, false);
   p_fwdm->GetIdlcMap(index, idlcMap, false);
   p_fwdm->GetITRPMMap(index, itrpmMap, false);
   p_fwdm->GetRigidMap(index, rigidMap, false);
   p_fwdm->GetIACCorrMap(index, iaccMap, false);
   p_fwdm->GetIACCorrWMap(index, iaccwMap, false);
   p_fwdm->GetAftstrMap(index, aftstrMap, false);
   p_fwdm->GetWrmpMap(index, wrmpMap, false);
   p_fwdm->GetAETPSMap(index, aetpsMap, false);
   p_fwdm->GetAERPMMap(index, aerpmMap, false);
   p_fwdm->GetCrnkMap(index, crnkMap, false);
   p_fwdm->GetDeadMap(index, deadMap, false);
   p_fwdm->GetEGOCurveMap(index, egocrvMap, false);
   p_fwdm->GetIATCLTMap(index, iatcltMap, false);
   p_fwdm->GetTpsswtMap(index, tpsswtMap, false);
   p_fwdm->GetAtscMap(index, atscMap, false);
   p_fwdm->GetGtscMap(index, gtscMap, false);
   p_fwdm->GetGpscMap(index, gpscMap, false);
   p_fwdm->GetPwm1Map(index, pwm1Map, false);
   p_fwdm->GetPwm2Map(index, pwm2Map, false);
   p_fwdm->GetIACMATMap(index, iacmatMap, false);

  }
  else
  { //from eeprom
   int index = m_current_funset_index - TABLES_NUMBER;
   EEPROMDataMediator *p_eedm = mp_eedcntr->GetEEDM();
   p_eedm->GetStartMap(index, startMap, false);  
   p_eedm->GetIdleMap(index, idleMap, false);
   p_eedm->GetWorkMap(index, workMap, false);
   p_eedm->GetTempMap(index, tempMap, false);
   p_eedm->GetTempIdlMap(index, tempIMap, false);  

   p_eedm->GetVEMap(index, veMap, false);
   p_eedm->GetVE2Map(index, ve2Map, false);
   p_eedm->GetAFRMap(index, afrMap, false);
   p_eedm->GetITMap(index, itMap, false);
   p_eedm->GetIdlrMap(index, idlrMap, false);
   p_eedm->GetIdlcMap(index, idlcMap, false);
   p_eedm->GetITRPMMap(index, itrpmMap, false);
   p_eedm->GetRigidMap(index, rigidMap, false);
   p_eedm->GetIACCorrMap(index, iaccMap, false);
   p_eedm->GetIACCorrWMap(index, iaccwMap, false);
   p_eedm->GetAftstrMap(index, aftstrMap, false);
   p_eedm->GetWrmpMap(index, wrmpMap, false);
   p_eedm->GetAETPSMap(index, aetpsMap, false);
   p_eedm->GetAERPMMap(index, aerpmMap, false);
   p_eedm->GetCrnkMap(index, crnkMap, false);
   p_eedm->GetDeadMap(index, deadMap, false);
   p_eedm->GetEGOCurveMap(index, egocrvMap, false);
   p_eedm->GetIATCLTMap(index, iatcltMap, false);
   p_eedm->GetTpsswtMap(index, tpsswtMap, false);
   p_eedm->GetAtscMap(index, atscMap, false);
   p_eedm->GetGtscMap(index, gtscMap, false);
   p_eedm->GetGpscMap(index, gpscMap, false);
   p_eedm->GetPwm1Map(index, pwm1Map, false);
   p_eedm->GetPwm2Map(index, pwm2Map, false);
   p_eedm->GetIACMATMap(index, iacmatMap, false);
  }
 }

 if (mp_gridModeEditorIgnDlg.get())
  mp_gridModeEditorIgnDlg->UpdateView(true);
 if (mp_gridModeEditorInjDlg.get())
  mp_gridModeEditorInjDlg->UpdateView(true);
}

void CLPTablesController::_OnMapChanged(int i_mapType)
{
 ASSERT(m_current_funset_index!=-1);
 CFirmwareDataMediator *p_fwdm = mp_fwdcntr->GetFWDM();
 EEPROMDataMediator *p_eedm = mp_eedcntr->GetEEDM();

 bool source = m_current_funset_index < TABLES_NUMBER; //true - flash, false - eeprom
 int index = source ? m_current_funset_index : m_current_funset_index - TABLES_NUMBER;

 switch(i_mapType)
 {
  //ignition maps
  case TYPE_MAP_DA_START:
   if (source)
    p_fwdm->SetStartMap(index, startMap);
   else
    p_eedm->SetStartMap(index, startMap);
   break;
  case TYPE_MAP_DA_IDLE:
   if (source)
    p_fwdm->SetIdleMap(index, idleMap);
   else
    p_eedm->SetIdleMap(index, idleMap);
   break;
  case TYPE_MAP_DA_WORK:
   if (source)
    p_fwdm->SetWorkMap(index, workMap);
   else
    p_eedm->SetWorkMap(index, workMap);
   break;
  case TYPE_MAP_DA_TEMP_CORR:
   if (source)
    p_fwdm->SetTempMap(index, tempMap);
   else
    p_eedm->SetTempMap(index, tempMap);
   break;
  case TYPE_MAP_DA_TEMPI_CORR:
   if (source)
    p_fwdm->SetTempIdlMap(index, tempIMap);
   else
    p_eedm->SetTempIdlMap(index, tempIMap);
   break;

  case TYPE_MAP_INJ_VE:
   if (source)
    p_fwdm->SetVEMap(index,  veMap);
   else
    p_eedm->SetVEMap(index, veMap);
   break;
  case TYPE_MAP_INJ_VE2:
   if (source)
    p_fwdm->SetVE2Map(index,  ve2Map);
   else
    p_eedm->SetVE2Map(index, ve2Map);
   break;
  case TYPE_MAP_INJ_AFR:
   if (source)
    p_fwdm->SetAFRMap(index,  afrMap);
   else
    p_eedm->SetAFRMap(index, afrMap);
   break;
  case TYPE_MAP_INJ_IT:
   if (source)
    p_fwdm->SetITMap(index,  itMap);
   else
    p_eedm->SetITMap(index, itMap);
   break;
  case TYPE_MAP_INJ_IDLR:
   if (source)
    p_fwdm->SetIdlrMap(index, idlrMap);
   else
    p_eedm->SetIdlrMap(index, idlrMap);
   break;
  case TYPE_MAP_INJ_IDLC:
   if (source)
    p_fwdm->SetIdlcMap(index, idlcMap);
   else
    p_eedm->SetIdlcMap(index, idlcMap);
   break;
  case TYPE_MAP_INJ_ITRPM:
   if (source)
    p_fwdm->SetITRPMMap(index, itrpmMap);
   else
    p_eedm->SetITRPMMap(index, itrpmMap);
   break;
  case TYPE_MAP_INJ_RIGID:
   if (source)
    p_fwdm->SetRigidMap(index, rigidMap);
   else
    p_eedm->SetRigidMap(index, rigidMap);
   break;
  case TYPE_MAP_INJ_IACC:
   if (source)
    p_fwdm->SetIACCorrMap(index, iaccMap);
   else
    p_eedm->SetIACCorrMap(index, iaccMap);
   break;
  case TYPE_MAP_INJ_IACCW:
   if (source)
    p_fwdm->SetIACCorrWMap(index, iaccwMap);
   else
    p_eedm->SetIACCorrWMap(index, iaccwMap);
   break;
  case TYPE_MAP_INJ_AFTSTR:
   if (source)
    p_fwdm->SetAftstrMap(index, aftstrMap);
   else
    p_eedm->SetAftstrMap(index, aftstrMap);
   break;
  case TYPE_MAP_INJ_WRMP:
   if (source)
    p_fwdm->SetWrmpMap(index, wrmpMap);
   else
    p_eedm->SetWrmpMap(index, wrmpMap);
   break;
  case TYPE_MAP_INJ_AETPS:
   if (source)
    p_fwdm->SetAETPSMap(index, aetpsMap);
   else
    p_eedm->SetAETPSMap(index, aetpsMap);
   break;
  case TYPE_MAP_INJ_AERPM:
   if (source)
    p_fwdm->SetAERPMMap(index, aerpmMap);
   else
    p_eedm->SetAERPMMap(index, aerpmMap);
   break;
  case TYPE_MAP_INJ_CRNK:
   if (source)
    p_fwdm->SetCrnkMap(index, crnkMap);
   else
    p_eedm->SetCrnkMap(index, crnkMap);
   break;
  case TYPE_MAP_INJ_DEAD:
   if (source)
    p_fwdm->SetDeadMap(index, deadMap);
   else
    p_eedm->SetDeadMap(index, deadMap);
   break;
  case TYPE_MAP_INJ_EGOCRV:
   if (source)
    p_fwdm->SetEGOCurveMap(index, egocrvMap);
   else
    p_eedm->SetEGOCurveMap(index, egocrvMap);
   break;
  case TYPE_MAP_INJ_IATCLT:
   if (source)
    p_fwdm->SetIATCLTMap(index, iatcltMap);
   else
    p_eedm->SetIATCLTMap(index, iatcltMap);
   break;
  case TYPE_MAP_INJ_TPSSWT:
   if (source)
    p_fwdm->SetTpsswtMap(index, tpsswtMap);
   else
    p_eedm->SetTpsswtMap(index, tpsswtMap);
   break;
  case TYPE_MAP_INJ_ATSC:
   if (source)
    p_fwdm->SetAtscMap(index, atscMap);
   else
    p_eedm->SetAtscMap(index, atscMap);
   break;
  case TYPE_MAP_INJ_GTSC:
   if (source)
    p_fwdm->SetGtscMap(index, gtscMap);
   else
    p_eedm->SetGtscMap(index, gtscMap);
   break;
  case TYPE_MAP_INJ_GPSC:
   if (source)
    p_fwdm->SetGpscMap(index, gpscMap);
   else
    p_eedm->SetGpscMap(index, gpscMap);
   break;
  case TYPE_MAP_PWM1:
   if (source)
    p_fwdm->SetPwm1Map(index, pwm1Map);
   else
    p_eedm->SetPwm1Map(index, pwm1Map);
   break;
  case TYPE_MAP_PWM2:
   if (source)
    p_fwdm->SetPwm2Map(index, pwm2Map);
   else
    p_eedm->SetPwm2Map(index, pwm2Map);
   break;
  case TYPE_MAP_INJ_IACMAT:
   if (source)
    p_fwdm->SetIACMATMap(index, iacmatMap);
   else
    p_eedm->SetIACMATMap(index, iacmatMap);
   break;
 }
}

void CLPTablesController::_OnOpenMapWnd(HWND i_hwnd, int i_mapType)
{
 MapWndScrPos::OnOpenMapWnd(i_hwnd, i_mapType);

 if (m_toggleMapWnd)
 {
  if (m_openedChart)
   ::SendMessage(m_openedChart, WM_CLOSE, 0, 0);
  m_openedChart = i_hwnd;
 }

 GDIHelpers::MakeWindowChild(i_hwnd, m_children_charts);
 ::SendMessage(i_hwnd, WM_NCACTIVATE, TRUE, -1); 
}

void CLPTablesController::_OnCloseMapWnd(HWND i_hwnd, int i_mapType)
{
 MapWndScrPos::OnCloseMapWnd(i_hwnd, i_mapType);

 if (i_mapType == TYPE_MAP_GME_IGN_WND)
  mp_view->SetGmeIgnCheck(false);

 if (i_mapType == TYPE_MAP_GME_INJ_WND)
  mp_view->SetGmeInjCheck(false);
}

void CLPTablesController::_OnGmeIgnButton(void)
{
 if (mp_view->GetGmeIgnCheck())
 {
  mp_gridModeEditorIgnDlg.reset(new CGridModeEditorIgnDlg());
  mp_gridModeEditorIgnDlg->BindMaps(startMap, idleMap, workMap, tempMap, tempIMap);
  mp_gridModeEditorIgnDlg->BindRPMGrid(m_rpm_grid_values);
  mp_gridModeEditorIgnDlg->BindCLTGrid(m_clt_grid_values);
  mp_gridModeEditorIgnDlg->BindLoadGrid(m_load_grid_values);
  mp_gridModeEditorIgnDlg->setIsAllowed(fastdelegate::MakeDelegate(this, &CLPTablesController::_IsAllowed));
  mp_gridModeEditorIgnDlg->setOnMapChanged(fastdelegate::MakeDelegate(this, &CLPTablesController::_OnMapChanged));
  mp_gridModeEditorIgnDlg->setOnCloseMapWnd(fastdelegate::MakeDelegate(this, &CLPTablesController::_OnCloseMapWnd));
  mp_gridModeEditorIgnDlg->setOnOpenMapWnd(fastdelegate::MakeDelegate(this, &CLPTablesController::_OnOpenMapWnd));
  mp_gridModeEditorIgnDlg->EnableAdvanceAngleIndication(true);
  VERIFY(mp_gridModeEditorIgnDlg->Create(CGridModeEditorIgnDlg::IDD, NULL));

  //Read and set load axis's configuration
  SECU3IO::SecurPar securPar;
  SECU3IO::FunSetPar funsetPar;
  CFirmwareDataMediator *p_fwdm = mp_fwdcntr->GetFWDM();
  p_fwdm->GetDefParamValues(SECUR_PAR, &securPar);
  if (securPar.use_respar)
  { //use parameters from flash
   p_fwdm->GetDefParamValues(FUNSET_PAR, &funsetPar);
  }
  else
  { //use parameters from EEPROM
   EEPROMDataMediator *p_eedm = mp_eedcntr->GetEEDM();
   p_eedm->GetDefParamValues(FUNSET_PAR, &funsetPar);
  }
  mp_gridModeEditorIgnDlg->SetLoadAxisCfg(funsetPar.map_lower_pressure, (funsetPar.load_src_cfg == 1) ? std::numeric_limits<float>::max() : funsetPar.map_upper_pressure, funsetPar.use_load_grid, true);

  mp_gridModeEditorIgnDlg->ShowWindow(SW_SHOW);
 }
 else
 {
  _OnCloseMapWnd(mp_gridModeEditorIgnDlg->m_hWnd, TYPE_MAP_GME_IGN_WND);
  mp_gridModeEditorIgnDlg->DestroyWindow();
  mp_gridModeEditorIgnDlg.reset(NULL);
 }
}

void CLPTablesController::_OnGmeInjButton(void)
{
 if (mp_view->GetGmeInjCheck())
 {
  mp_gridModeEditorInjDlg.reset(new CGridModeEditorInjDlg());
  mp_gridModeEditorInjDlg->BindMaps(veMap, afrMap, itMap, idlcMap, idlrMap, itrpmMap, rigidMap, iaccMap, iaccwMap,
                                    aftstrMap, wrmpMap, aetpsMap, aerpmMap, crnkMap, deadMap, egocrvMap, iatcltMap,
                                    tpsswtMap, atscMap, gtscMap, gpscMap, pwm1Map, pwm2Map, iacmatMap, ve2Map);
  mp_gridModeEditorInjDlg->BindRPMGrid(m_rpm_grid_values);
  mp_gridModeEditorInjDlg->BindCLTGrid(m_clt_grid_values);
  mp_gridModeEditorInjDlg->BindLoadGrid(m_load_grid_values, &m_ve2_map_load_slots[0]);
  mp_gridModeEditorInjDlg->setIsAllowed(fastdelegate::MakeDelegate(this, &CLPTablesController::_IsAllowed));
  mp_gridModeEditorInjDlg->setOnMapChanged(fastdelegate::MakeDelegate(this, &CLPTablesController::_OnMapChanged));
  mp_gridModeEditorInjDlg->setOnCloseMapWnd(fastdelegate::MakeDelegate(this, &CLPTablesController::_OnCloseMapWnd));
  mp_gridModeEditorInjDlg->setOnOpenMapWnd(fastdelegate::MakeDelegate(this, &CLPTablesController::_OnOpenMapWnd));
  mp_gridModeEditorInjDlg->setOnChangeSettings(fastdelegate::MakeDelegate(this, &CLPTablesController::_OnChangeSettingsGME));
  mp_gridModeEditorInjDlg->SetActiveVEMap(mp_settings->GetActiveVEMap()); 
  VERIFY(mp_gridModeEditorInjDlg->Create(CGridModeEditorInjDlg::IDD, NULL));

  //Read and set load axis's configuration
  SECU3IO::SecurPar securPar;
  SECU3IO::FunSetPar funsetPar;
  CFirmwareDataMediator *p_fwdm = mp_fwdcntr->GetFWDM();
  p_fwdm->GetDefParamValues(SECUR_PAR, &securPar);
  if (securPar.use_respar)
  { //use parameters from flash
   p_fwdm->GetDefParamValues(FUNSET_PAR, &funsetPar);
  }
  else
  { //use parameters from EEPROM
   EEPROMDataMediator *p_eedm = mp_eedcntr->GetEEDM();
   p_eedm->GetDefParamValues(FUNSET_PAR, &funsetPar);
  }
  mp_gridModeEditorInjDlg->SetLoadAxisCfg(funsetPar.map_lower_pressure, (funsetPar.load_src_cfg == 1) ? std::numeric_limits<float>::max() : funsetPar.map_upper_pressure, funsetPar.use_load_grid, true);

  mp_gridModeEditorInjDlg->SetITMode(mp_settings->GetITEdMode());
  mp_gridModeEditorInjDlg->SetSplitAngMode(_GetSplitAngMode()); //splitting
  mp_gridModeEditorInjDlg->ShowWindow(SW_SHOW);
 }
 else
 {
  OnCloseMapWnd(mp_gridModeEditorInjDlg->m_hWnd, TYPE_MAP_GME_INJ_WND);
  mp_gridModeEditorInjDlg->DestroyWindow();
  mp_gridModeEditorInjDlg.reset(NULL);
 }
}

void CLPTablesController::_OnChangeSettingsGME(void)
{
 mp_settings->SetITEdMode(mp_gridModeEditorInjDlg->GetITMode());
 mp_settings->SetActiveVEMap(mp_gridModeEditorInjDlg->GetActiveVEMap());
}

bool CLPTablesController::_GetSplitAngMode(void)
{
 if (m_current_funset_index < TABLES_NUMBER)
 { //flash
  CFirmwareDataMediator *p_fwdm = mp_fwdcntr->GetFWDM();
  DWORD opt = p_fwdm->GetFWOptions();
  return CHECKBIT32(opt, SECU3IO::COPT_SPLIT_ANGLE);
 }
 else
 { //eeprom
  EEPROMDataMediator *p_eedm = mp_eedcntr->GetEEDM();
  return p_eedm->GetSplitAngMode(0);
 }
}
