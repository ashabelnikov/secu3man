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
#include "io-core/MapIds.h"

#undef max

using namespace fastdelegate;

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
 m_ve2_map_load_slots = MathHelpers::BuildGridFromRange(0.0f, 100.0f, 16);

 memset(m_rpm_grid_values, 0, 16 * sizeof(float));
 memset(m_irpm_grid_values, 0, 8 * sizeof(float)); //idling
 memset(m_clt_grid_values, 0, 16 * sizeof(float));
 memset(m_load_grid_values, 0, 16 * sizeof(float));
 memset(m_iload_grid_values, 0, 8 * sizeof(float)); //idling
  
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
 {
  mp_view->FillMapSetCombo(mapsets);
  mp_view->EnableMapSetCombo(true);
 }
 else
  mp_view->EnableMapSetCombo(false);

 mp_view->EnableGmeButtons(false);

 if (m_firmware_opened)
 { //load x-axis grids
  CFirmwareDataMediator *p_fwdm = mp_fwdcntr->GetFWDM();
  p_fwdm->GetRPMGridMap(m_rpm_grid_values);
  p_fwdm->GetCLTGridMap(m_clt_grid_values);
  p_fwdm->GetLoadGridMap(m_load_grid_values);  
  p_fwdm->GetIRPMGridMap(m_irpm_grid_values);
  p_fwdm->GetILoadGridMap(m_iload_grid_values);  
  p_fwdm->GetTLoadGridMap(m_tload_grid_values);  
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
  _OnCloseMapWnd(mp_gridModeEditorIgnDlg->m_hWnd, ETMT_GME_IGN_WND);

 if (mp_gridModeEditorInjDlg.get())
  _OnCloseMapWnd(mp_gridModeEditorInjDlg->m_hWnd, ETMT_GME_INJ_WND);

 m_firmware_opened = false;
 m_eeprom_opened = false;
}

void CLPTablesController::DisplayCurrentRecord(SECU3IO::SensorDat* sd)
{
 if (m_firmware_opened && (mp_gridModeEditorIgnDlg.get() || mp_gridModeEditorInjDlg.get()))
 {  if (mp_gridModeEditorIgnDlg.get())
   mp_gridModeEditorIgnDlg->SetDynamicValues(*sd);
  if (mp_gridModeEditorInjDlg.get())
   mp_gridModeEditorInjDlg->SetDynamicValues(*sd);
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
   for(int i = ETMT_SET_START; i <= ETMT_SET_END; ++i)
   {
    p_fwdm->GetSetMap(index, i, m_md[1].GetMap(i), false);  //modified 
    p_fwdm->GetSetMap(index, i, m_md[0].GetMap(i), true);   //original
   }
  }
  else
  { //from eeprom
   int index = m_current_funset_index - TABLES_NUMBER;
   EEPROMDataMediator *p_eedm = mp_eedcntr->GetEEDM();
   for(int i = ETMT_SET_START; i <= ETMT_SET_END; ++i)
   {
    p_eedm->GetSetMap(index, i, m_md[1].GetMap(i), false);  //modified
    p_eedm->GetSetMap(index, i, m_md[0].GetMap(i), true);   //original
   }
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

 if (source)
  p_fwdm->SetSetMap(index, i_mapType, m_md[1].GetMap(i_mapType));
 else
  p_eedm->SetSetMap(index, i_mapType, m_md[1].GetMap(i_mapType));
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

 if (i_mapType == ETMT_GME_IGN_WND)
 {
  mp_view->SetGmeIgnCheck(false);
  mp_gridModeEditorIgnDlg->DestroyWindow();
  mp_gridModeEditorIgnDlg.reset(NULL);
 }

 if (i_mapType == ETMT_GME_INJ_WND)
 {
  mp_view->SetGmeInjCheck(false);
  mp_gridModeEditorInjDlg->DestroyWindow();
  mp_gridModeEditorInjDlg.reset(NULL);
 }
}

void CLPTablesController::_OnGmeIgnButton(void)
{
 if (mp_view->GetGmeIgnCheck())
 {
  mp_gridModeEditorIgnDlg.reset(new CGridModeEditorIgnDlg());
  mp_gridModeEditorIgnDlg->BindMaps(m_md[1].f_str, m_md[1].f_idl, m_md[1].f_wrk, m_md[1].f_tmp, m_md[1].f_tmp_idl);
  mp_gridModeEditorIgnDlg->BindMapsOrig(m_md[0].f_str, m_md[0].f_idl, m_md[0].f_wrk, m_md[0].f_tmp, m_md[0].f_tmp_idl);
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
  mp_gridModeEditorIgnDlg->SetLoadAxisCfg(funsetPar.map_lower_pressure, funsetPar.map_upper_pressure, funsetPar.load_src_cfg, funsetPar.use_load_grid, true);

  mp_gridModeEditorIgnDlg->ShowWindow(SW_SHOW);

  if (m_OnOpenMapWnd)
   m_OnOpenMapWnd();
 }
 else
 {
  _OnCloseMapWnd(mp_gridModeEditorIgnDlg->m_hWnd, ETMT_GME_IGN_WND);
 }
}

void CLPTablesController::_OnGmeInjButton(void)
{
 if (mp_view->GetGmeInjCheck())
 {
  mp_gridModeEditorInjDlg.reset(new CGridModeEditorInjDlg());

  mp_gridModeEditorInjDlg->BindMaps(m_md[1].inj_ve, m_md[1].inj_afr, m_md[1].inj_timing, m_md[1].inj_iac_crank_pos, m_md[1].inj_iac_run_pos, m_md[1].inj_target_rpm, m_md[1].inj_idl_rigidity, m_md[1].inj_iac_corr,
                                    m_md[1].inj_iac_corr_w, m_md[1].inj_aftstr, m_md[1].inj_warmup, m_md[1].inj_ae_tps, m_md[1].inj_ae_rpm, m_md[1].inj_cranking, m_md[1].inj_dead_time, m_md[1].inj_ego_curve, 
                                    m_md[1].inj_iatclt_corr, m_md[1].inj_tpsswt, m_md[1].inj_ats_corr, m_md[1].inj_gts_corr, m_md[1].inj_gps_corr, m_md[1].pwm_duty1, m_md[1].pwm_duty2, m_md[1].iac_mat_corr,
                                    m_md[1].inj_ve2, m_md[1].inj_tpszon, m_md[1].inj_cylmult, m_md[1].inj_cyladd, m_md[1].inj_ae_map, m_md[1].inj_thrass, m_md[1].inj_ive);

  mp_gridModeEditorInjDlg->BindMapsOrig(m_md[0].inj_ve, m_md[0].inj_afr, m_md[0].inj_timing, m_md[0].inj_iac_crank_pos, m_md[0].inj_iac_run_pos, m_md[0].inj_target_rpm, m_md[0].inj_idl_rigidity, m_md[0].inj_iac_corr,
                                    m_md[0].inj_iac_corr_w, m_md[0].inj_aftstr, m_md[0].inj_warmup, m_md[0].inj_ae_tps, m_md[0].inj_ae_rpm, m_md[0].inj_cranking, m_md[0].inj_dead_time, m_md[0].inj_ego_curve, 
                                    m_md[0].inj_iatclt_corr, m_md[0].inj_tpsswt, m_md[0].inj_ats_corr, m_md[0].inj_gts_corr, m_md[0].inj_gps_corr, m_md[0].pwm_duty1, m_md[0].pwm_duty2, m_md[0].iac_mat_corr,
                                    m_md[0].inj_ve2, m_md[0].inj_tpszon, m_md[0].inj_cylmult, m_md[0].inj_cyladd, m_md[0].inj_ae_map, m_md[0].inj_thrass, m_md[0].inj_ive);

  mp_gridModeEditorInjDlg->BindRPMGrid(m_rpm_grid_values, m_irpm_grid_values);
  mp_gridModeEditorInjDlg->BindCLTGrid(m_clt_grid_values);
  mp_gridModeEditorInjDlg->BindLoadGrid(m_load_grid_values, &m_ve2_map_load_slots[0], m_iload_grid_values);
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
  mp_gridModeEditorInjDlg->SetLoadAxisCfg(funsetPar.map_lower_pressure, funsetPar.map_upper_pressure, funsetPar.load_src_cfg, funsetPar.use_load_grid, true);

  mp_gridModeEditorInjDlg->SetITMode(mp_settings->GetITEdMode());
  mp_gridModeEditorInjDlg->SetSplitAngMode(_GetSplitAngMode()); //splitting
  mp_gridModeEditorInjDlg->ShowWindow(SW_SHOW);

  if (m_OnOpenMapWnd)
   m_OnOpenMapWnd();
 }
 else
 {
  _OnCloseMapWnd(mp_gridModeEditorInjDlg->m_hWnd, ETMT_GME_INJ_WND);
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
