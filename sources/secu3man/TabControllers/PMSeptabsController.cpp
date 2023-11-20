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

/** \file PMSeptabsController.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "Resources/resource.h"
#include "PMSeptabsController.h"

#include <algorithm>
#include "Application/CommunicationManager.h"
#include "common/fastdelegate.h"
#include "io-core/bitmask.h"
#include "io-core/FirmwareMapsDataHolder.h"
#include "io-core/FirmwareDataMediator.h"
#include "io-core/SECU3IO.h"
#include "io-core/ufcodes.h"
#include "MainFrame/StatusBarManager.h"
#include "ParamDesk/Tables/SepTablesDeskDlg.h"
#include "TablDesk/ButtonsPanel.h"
#include "TablDesk/SeptabsPanel.h"
#include "TablDesk/MapIds.h"
#include "fwimpexp/S3FFileDataIO.h"
#include "fwimpexp/S3FImpExpController.h"
#include "Settings/IsettingsData.h"
#include "ui-core/FileDialogEx.h"
#include "ui-core/MsgBox.h"
#include "TabControllersCommunicator.h"
#include "FirmwareTabController.h"

using namespace fastdelegate;
using namespace SECU3IO;

//------------------------------ Helpful functions ----------------------------------
bool CPMSeptabsController::_CompareViewMap(int i_mapType, size_t size) const
{
 return std::equal(mp_view->GetMap(i_mapType, false),
                   mp_view->GetMap(i_mapType, false) + size,
                   mp_view->GetMap(i_mapType, true));
}

float* CPMSeptabsController::_GetMap(int i_mapType, bool i_original, FWMapsDataHolder* p_maps /*= NULL*/)
{
 if (!p_maps)
  p_maps = i_original ? m_omaps : m_maps;
 
 switch(i_mapType)
 {
  case TYPE_MAP_ATTENUATOR: return p_maps->attenuator_table;
  case TYPE_MAP_DWELLCNTRL: return p_maps->dwellcntrl_table;
  case TYPE_MAP_CTS_CURVE: return p_maps->ctscurve_table;
  case TYPE_MAP_ATS_CURVE: return p_maps->atscurve_table;
  case TYPE_MAP_ATS_CORR: return p_maps->ats_corr_table;
  case TYPE_MAP_GASDOSE: return p_maps->gasdose_pos_table;
  case TYPE_MAP_BAROCORR: return p_maps->barocorr_table;
  case TYPE_MAP_MANIGNTIM: return p_maps->pa4_igntim_corr;
  case TYPE_MAP_TMP2_CURVE: return p_maps->tmp2_curve;
  case TYPE_MAP_CRKCLT_CORR: return p_maps->ctscrk_corr;
  case TYPE_MAP_EH_PAUSE: return p_maps->eh_pause_table;
  case TYPE_MAP_CRANKING_THRD: return p_maps->cranking_thrd;
  case TYPE_MAP_CRANKING_TIME: return p_maps->cranking_time;
  case TYPE_MAP_SMAPABAN_THRD: return p_maps->smapaban_thrd;
  case TYPE_MAP_KNOCK_ZONE: return p_maps->knock_zone;
  case TYPE_MAP_GRTS_CURVE: return p_maps->grts_curve;
  case TYPE_MAP_GRHEAT_DUTY: return p_maps->grheat_duty;
  case TYPE_MAP_PWMIAC_UCOEF: return p_maps->pwmiac_ucoef;
  case TYPE_MAP_AFTSTR_STRK0: return p_maps->aftstr_strk0;
  case TYPE_MAP_AFTSTR_STRK1: return p_maps->aftstr_strk1;
  case TYPE_MAP_GRVDELAY: return p_maps->grv_delay;
  case TYPE_MAP_FTLS_CURVE: return p_maps->ftls_curve;
  case TYPE_MAP_EGTS_CURVE: return p_maps->egts_curve;
  case TYPE_MAP_OPS_CURVE: return p_maps->ops_curve;
  case TYPE_MAP_MANINJPWC: return p_maps->injpw_coef;
  case TYPE_MAP_MAF_CURVE: return p_maps->maf_curve;
  case TYPE_MAP_FTLSCOR: return p_maps->ftls_corr;
  case TYPE_MAP_LAMBDA_ZONE: return p_maps->lambda_zone;
  case TYPE_MAP_FTS_CURVE: return p_maps->fts_curve;
  case TYPE_MAP_FUELDENS_CORR: return p_maps->fueldens_corr;
  case TYPE_MAP_XTAU_XFACC: return p_maps->xtau_xfacc;
  case TYPE_MAP_XTAU_XFDEC: return p_maps->xtau_xfdec;
  case TYPE_MAP_XTAU_TFACC: return p_maps->xtau_tfacc;
  case TYPE_MAP_XTAU_TFDEC: return p_maps->xtau_tfdec;
  case TYPE_MAP_INJNONLINP: return p_maps->inj_nonlinp_corr;
  case TYPE_MAP_INJNONLING: return p_maps->inj_nonling_corr;
 }
 return NULL; //undefined type of map
}

namespace {
size_t _GetMapSize(int i_mapType)
{
 switch(i_mapType)
 {
  case TYPE_MAP_ATTENUATOR: return KC_ATTENUATOR_LOOKUP_TABLE_SIZE;
  case TYPE_MAP_DWELLCNTRL: return COIL_ON_TIME_LOOKUP_TABLE_SIZE;
  case TYPE_MAP_CTS_CURVE: return THERMISTOR_LOOKUP_TABLE_SIZE+2;
  case TYPE_MAP_ATS_CURVE: return THERMISTOR_LOOKUP_TABLE_SIZE+2;
  case TYPE_MAP_ATS_CORR: return ATS_CORR_LOOKUP_TABLE_SIZE;
  case TYPE_MAP_GASDOSE: return GASDOSE_POS_TPS_SIZE * GASDOSE_POS_RPM_SIZE;
  case TYPE_MAP_BAROCORR: return BAROCORR_SIZE+2;
  case TYPE_MAP_MANIGNTIM: return PA4_LOOKUP_TABLE_SIZE;
  case TYPE_MAP_TMP2_CURVE: return THERMISTOR_LOOKUP_TABLE_SIZE+2;
  case TYPE_MAP_CRKCLT_CORR: return CTS_CRKCORR_SIZE;
  case TYPE_MAP_EH_PAUSE: return COIL_ON_TIME_LOOKUP_TABLE_SIZE;
  case TYPE_MAP_CRANKING_THRD: return CRANK_THRD_SIZE;
  case TYPE_MAP_CRANKING_TIME: return CRANK_TIME_SIZE;
  case TYPE_MAP_SMAPABAN_THRD: return SMAPABAN_THRD_SIZE;
  case TYPE_MAP_KNOCK_ZONE: return F_WRK_POINTS_L * F_WRK_POINTS_F;
  case TYPE_MAP_GRTS_CURVE: return THERMISTOR_LOOKUP_TABLE_SIZE+2;
  case TYPE_MAP_GRHEAT_DUTY: return F_TMP_POINTS;
  case TYPE_MAP_PWMIAC_UCOEF: return PWMIAC_UCOEF_SIZE;
  case TYPE_MAP_AFTSTR_STRK0: return AFTSTR_STRK_SIZE;
  case TYPE_MAP_AFTSTR_STRK1: return AFTSTR_STRK_SIZE;
  case TYPE_MAP_GRVDELAY: return F_TMP_POINTS;
  case TYPE_MAP_FTLS_CURVE: return FTLS_LOOKUP_TABLE_SIZE+2;
  case TYPE_MAP_EGTS_CURVE: return EGTS_LOOKUP_TABLE_SIZE+2;
  case TYPE_MAP_OPS_CURVE: return OPS_LOOKUP_TABLE_SIZE+2;
  case TYPE_MAP_MANINJPWC: return INJPWCOEF_LUT_SIZE;
  case TYPE_MAP_MAF_CURVE: return MAF_FLOW_CURVE_SIZE+1+2;
  case TYPE_MAP_FTLSCOR: return FTLSCOR_UCOEF_SIZE;
  case TYPE_MAP_LAMBDA_ZONE: return F_WRK_POINTS_L * F_WRK_POINTS_F;
  case TYPE_MAP_FTS_CURVE: return FTS_LOOKUP_TABLE_SIZE+2;
  case TYPE_MAP_FUELDENS_CORR: return FUELDENS_CORR_SIZE;
  case TYPE_MAP_XTAU_XFACC: return XTAU_FACT_SIZE;
  case TYPE_MAP_XTAU_XFDEC: return XTAU_FACT_SIZE;
  case TYPE_MAP_XTAU_TFACC: return XTAU_FACT_SIZE;
  case TYPE_MAP_XTAU_TFDEC: return XTAU_FACT_SIZE;
  case TYPE_MAP_INJNONLINP: return INJ_NONLIN_SIZE * 2;
  case TYPE_MAP_INJNONLING: return INJ_NONLIN_SIZE * 2;
 }
 ASSERT(0);
 return 0; //undefined type of map
}
}

void CPMSeptabsController::_MoveMapToChart(int i_mapType, bool i_original, FWMapsDataHolder* p_maps /*= NULL*/)
{
 std::copy(_GetMap(i_mapType, i_original, p_maps),
  _GetMap(i_mapType, i_original, p_maps) + _GetMapSize(i_mapType),
  mp_view->GetMap(i_mapType, i_original));
}

void CPMSeptabsController::_MoveMapsToCharts(bool i_original, FWMapsDataHolder* p_maps /*= NULL*/)
{
 for(int id = TYPE_MAP_SEP_START; id <= TYPE_MAP_SEP_END; ++id)
 {
  if (_isCacheUpToDate(id))
   _MoveMapToChart(id, i_original, p_maps);
 }
}

void CPMSeptabsController::_ClearAcquisitionFlags(void)
{
 //See TablDesk/MapIds.h for more information
 for(int id = TYPE_MAP_SEP_START; id <= TYPE_MAP_SEP_END; ++id)
  std::fill(_GetMap(id, false, m_maps_flags), _GetMap(id, false, m_maps_flags) + _GetMapSize(id), .0f);
}

void CPMSeptabsController::_ResetModification(int mapId /*= -1*/)
{
 if (mapId== -1)
 { //all maps
  (*m_omaps) = (*m_maps);
 }
 else
 { //certain map
  std::copy(_GetMap(mapId, false), _GetMap(mapId, false) + _GetMapSize(mapId), _GetMap(mapId, true));
 }
}

//-----------------------------------------------------------------------------------
CPMSeptabsController::CPMSeptabsController(VIEW* ip_view, CCommunicationManager* ip_comm, CStatusBarManager* ip_sbar, ISettingsData* ip_settings, EventHandler OnSepReqDataColl)
: Super(ip_view)
, mp_comm(ip_comm)
, mp_sbar(ip_sbar)
, mp_settings(ip_settings)
, MapWndScrPos(ip_settings, true)
, m_OnSepReqDataColl(OnSepReqDataColl)
, m_collectId(-1)
, m_valid_cache(false)
{
 //устанавливаем обработчики событий от view
 mp_view->setOnMapChanged(MakeDelegate(this, &CPMSeptabsController::OnMapChanged));
 mp_view->setOnCloseMapWnd(MakeDelegate(this, &CPMSeptabsController::OnCloseMapWnd));
 mp_view->setOnOpenMapWnd(MakeDelegate(this, &CPMSeptabsController::OnOpenMapWnd));
 mp_view->setOnSaveButton(MakeDelegate(this, &CPMSeptabsController::OnSaveButton));
 mp_view->mp_SeptabsPanel->setOnChangeSettings(MakeDelegate(this, &CPMSeptabsController::OnChangeSettings));

 //maps (current)
 m_maps = new FWMapsDataHolder;
 //maps (original)
 m_omaps = new FWMapsDataHolder;
 //флаги сбора информации
 m_maps_flags = new FWMapsDataHolder;

}

CPMSeptabsController::~CPMSeptabsController()
{
 delete m_maps;
 delete m_omaps;
 delete m_maps_flags;
}

//----------------------------------------------------------------
void CPMSeptabsController::OnActivate(void)
{
 InvalidateCache(); 

 CFirmwareTabController* p_fwdcntr = static_cast<CFirmwareTabController*>
 (TabControllersCommunicator::GetInstance()->GetReference(TCC_FIRMWARE_TAB_CONTROLLER));
 mp_view->EnableSaveButton(p_fwdcntr->IsFirmwareOpened());

 mp_view->Enable(false);

 mp_view->mp_SeptabsPanel->SetCSVSepSymbol(mp_settings->GetMapCSVSepSymbol());

 MapPtMovStep mptms;
 mp_settings->GetMapPtMovStep(mptms);
 mp_view->mp_SeptabsPanel->SetPtMovStep(TYPE_MAP_ATTENUATOR, mptms.m_attenuator_map);
 mp_view->mp_SeptabsPanel->SetPtMovStep(TYPE_MAP_DWELLCNTRL, mptms.m_dwellcntrl_map);
 mp_view->mp_SeptabsPanel->SetPtMovStep(TYPE_MAP_CTS_CURVE, mptms.m_cts_curve_map);
 mp_view->mp_SeptabsPanel->SetPtMovStep(TYPE_MAP_ATS_CURVE, mptms.m_ats_curve_map);
 mp_view->mp_SeptabsPanel->SetPtMovStep(TYPE_MAP_ATS_CORR, mptms.m_ats_aac_map);
 mp_view->mp_SeptabsPanel->SetPtMovStep(TYPE_MAP_GASDOSE, mptms.m_gasdose_map);
 mp_view->mp_SeptabsPanel->SetPtMovStep(TYPE_MAP_BAROCORR, mptms.m_barocorr_map);
 mp_view->mp_SeptabsPanel->SetPtMovStep(TYPE_MAP_MANIGNTIM, mptms.m_manigntim_map);
 mp_view->mp_SeptabsPanel->SetPtMovStep(TYPE_MAP_TMP2_CURVE, mptms.m_tmp2_curve_map);
 mp_view->mp_SeptabsPanel->SetPtMovStep(TYPE_MAP_CRKCLT_CORR, mptms.m_crktemp_map);
 mp_view->mp_SeptabsPanel->SetPtMovStep(TYPE_MAP_EH_PAUSE, mptms.m_eh_pause_map);
 mp_view->mp_SeptabsPanel->SetPtMovStep(TYPE_MAP_CRANKING_THRD, mptms.m_cranking_thrd_map);
 mp_view->mp_SeptabsPanel->SetPtMovStep(TYPE_MAP_CRANKING_TIME, mptms.m_cranking_time_map);
 mp_view->mp_SeptabsPanel->SetPtMovStep(TYPE_MAP_SMAPABAN_THRD, mptms.m_smapaban_thrd_map);
 mp_view->mp_SeptabsPanel->SetPtMovStep(TYPE_MAP_KNOCK_ZONE, mptms.m_knock_zone_map);
 mp_view->mp_SeptabsPanel->SetPtMovStep(TYPE_MAP_GRTS_CURVE, mptms.m_grts_curve_map);
 mp_view->mp_SeptabsPanel->SetPtMovStep(TYPE_MAP_GRHEAT_DUTY, mptms.m_grheat_duty_map);
 mp_view->mp_SeptabsPanel->SetPtMovStep(TYPE_MAP_PWMIAC_UCOEF, mptms.m_pwmiac_ucoef_map);
 mp_view->mp_SeptabsPanel->SetPtMovStep(TYPE_MAP_AFTSTR_STRK0, mptms.m_aftstr_strk0_map);
 mp_view->mp_SeptabsPanel->SetPtMovStep(TYPE_MAP_AFTSTR_STRK1, mptms.m_aftstr_strk1_map);
 mp_view->mp_SeptabsPanel->SetPtMovStep(TYPE_MAP_GRVDELAY, mptms.m_grvaldel_map);
 mp_view->mp_SeptabsPanel->SetPtMovStep(TYPE_MAP_FTLS_CURVE, mptms.m_ftls_curve_map);
 mp_view->mp_SeptabsPanel->SetPtMovStep(TYPE_MAP_EGTS_CURVE, mptms.m_egts_curve_map);
 mp_view->mp_SeptabsPanel->SetPtMovStep(TYPE_MAP_OPS_CURVE, mptms.m_ops_curve_map);
 mp_view->mp_SeptabsPanel->SetPtMovStep(TYPE_MAP_MANINJPWC, mptms.m_maninjpwc_map);
 mp_view->mp_SeptabsPanel->SetPtMovStep(TYPE_MAP_MAF_CURVE, mptms.m_mafcurve_map);
 mp_view->mp_SeptabsPanel->SetPtMovStep(TYPE_MAP_FTLSCOR, mptms.m_ftlscor_map);
 mp_view->mp_SeptabsPanel->SetPtMovStep(TYPE_MAP_LAMBDA_ZONE, mptms.m_lambda_zone_map);
 mp_view->mp_SeptabsPanel->SetPtMovStep(TYPE_MAP_FTS_CURVE, mptms.m_fts_curve_map);
 mp_view->mp_SeptabsPanel->SetPtMovStep(TYPE_MAP_FUELDENS_CORR, mptms.m_fueldens_corr_map);
 mp_view->mp_SeptabsPanel->SetPtMovStep(TYPE_MAP_XTAU_XFACC, mptms.m_xtau_xfacc_map);
 mp_view->mp_SeptabsPanel->SetPtMovStep(TYPE_MAP_XTAU_XFDEC, mptms.m_xtau_xfdec_map);
 mp_view->mp_SeptabsPanel->SetPtMovStep(TYPE_MAP_XTAU_TFACC, mptms.m_xtau_tfacc_map);
 mp_view->mp_SeptabsPanel->SetPtMovStep(TYPE_MAP_XTAU_TFDEC, mptms.m_xtau_tfdec_map);
 mp_view->mp_SeptabsPanel->SetPtMovStep(TYPE_MAP_INJNONLINP, mptms.m_injnonlinp_map);
 mp_view->mp_SeptabsPanel->SetPtMovStep(TYPE_MAP_INJNONLING, mptms.m_injnonling_map);
}

void CPMSeptabsController::OnDeactivate(void)
{
}

void CPMSeptabsController::Enable(bool state)
{
 mp_view->Enable(state);
}

void CPMSeptabsController::StartDataCollection(void)
{
 Super::StartDataCollection();
}

bool CPMSeptabsController::CollectData(const BYTE i_descriptor, const void* i_packet_data)
{
 if (m_collectId==-1)
 {
  switch(m_operation_state)
  {
   case 0: //Read out RPM grid
    mp_sbar->SetInformationText(MLL::LoadString(IDS_PM_READING_RPMGRD));
    if (i_descriptor != RPMGRD_PAR)
     mp_comm->m_pControlApp->ChangeContext(RPMGRD_PAR);
    else
    {//save RPM grid
     const SepTabPar* data = (const SepTabPar*)i_packet_data;
     memcpy(m_rpmGrid, data->table_data, F_RPM_SLOTS*sizeof(float)); //save RPM grid
     m_operation_state = 1;
    }
    break;
   case 1: //Read out CLT grid
    mp_sbar->SetInformationText(MLL::LoadString(IDS_PM_READING_CLTGRD));
    if (i_descriptor != CLTGRD_PAR)
     mp_comm->m_pControlApp->ChangeContext(CLTGRD_PAR);
    else
    {//save CLT grid
     const SepTabPar* data = (const SepTabPar*)i_packet_data;
     memcpy(m_cltGrid, data->table_data, F_TMP_SLOTS*sizeof(float)); //save CLT grid
     m_operation_state = 2;
    }
    break;
   case 2: //Read out load grid
    mp_sbar->SetInformationText(MLL::LoadString(IDS_PM_READING_LODGRD));
    if (i_descriptor != LODGRD_PAR)
     mp_comm->m_pControlApp->ChangeContext(LODGRD_PAR);
    else
    {//save load grid
     const SepTabPar* data = (const SepTabPar*)i_packet_data;
     memcpy(m_lodGrid, data->table_data, F_LOAD_SLOTS*sizeof(float)); //save load grid

     mp_view->SetRPMGrid(m_rpmGrid);  //Set RPM grid read out from SECU-3  
     mp_view->SetCLTGrid(m_cltGrid);  //Set CLT grid read out from SECU-3  
     mp_view->SetLoadGrid(m_lodGrid); //Set load grid read out from SECU-3
     m_valid_cache = true;
     _ClearAcquisitionFlags();

     //check for opened tables
     bool opened = false;
     for(int i = TYPE_MAP_SEP_START; i <= TYPE_MAP_SEP_END; ++i)
     {
      HWND hwnd = mp_view->mp_SeptabsPanel->GetMapWindow(i);
      if (hwnd)
      {
       opened = true;
       mp_sbar->SetInformationText(MLL::LoadString(IDS_PM_READING_TABLES));
       mp_comm->m_pControlApp->ChangeContext(EDITAB_PAR, i);
       m_operation_state = 4; //continue: grab some opened maps
       break;
      }     
     }

     if (!opened)
     {
      m_operation_state = -1;
      mp_sbar->SetInformationText(MLL::LoadString(IDS_PM_READY));
      return true; //finished!
     }
    }
    break;

   case 4:
    {
     if (i_descriptor != EDITAB_PAR)
     {
      for(int i = TYPE_MAP_SEP_START; i <= TYPE_MAP_SEP_END; ++i)
      {
       HWND hwnd = mp_view->mp_SeptabsPanel->GetMapWindow(i);
       if (hwnd)
       {
        mp_comm->m_pControlApp->ChangeContext(EDITAB_PAR, i);
        break;
       }     
      }
      break;
     }

     //update chache and perform checking
     const EditTabPar* data = (const EditTabPar*)i_packet_data;
     _UpdateCache(data);

     bool all_uptodate = true;
     for(int i = TYPE_MAP_SEP_START; i <= TYPE_MAP_SEP_END; ++i)
     {
      HWND hwnd = mp_view->mp_SeptabsPanel->GetMapWindow(i);
      if (hwnd && !_isCacheUpToDate(i))
      {
       if (data->tab_id != i)
        mp_comm->m_pControlApp->ChangeContext(EDITAB_PAR, i);
       all_uptodate = false;
       break;
      }     
     }

     if (all_uptodate)
     {
      m_operation_state = -1;
      mp_sbar->SetInformationText(MLL::LoadString(IDS_PM_READY));
      OnDataCollected();
      return true; //работа КА завершена
     }
    }
    break;
  }
 }
 else
 {
  switch(m_operation_state)
  {
   case 0: //Read out RPM grid
    mp_sbar->SetInformationText(MLL::LoadString(IDS_PM_READING_TABLES));
    mp_comm->m_pControlApp->ChangeContext(EDITAB_PAR, m_collectId);
    m_operation_state = 1;
    break;
   case 1:
    if (i_descriptor != EDITAB_PAR || ((const EditTabPar*)i_packet_data)->tab_id != m_collectId)
    {
     m_operation_state = 0;
     break;
    }
    //update chache and perform checking
    const EditTabPar* data = (const EditTabPar*)i_packet_data;
    _UpdateCache(data);
    if (_isCacheUpToDate(data->tab_id))
    {
     mp_sbar->SetInformationText(MLL::LoadString(IDS_PM_READY));
     m_operation_state = -1;
     m_collectId = -1;
     OnDataCollected(data->tab_id);
     return true;
    }
    break;
  }
 }

 return false;
}


void CPMSeptabsController::InvalidateCache(void)
{
 _ClearAcquisitionFlags();
 m_valid_cache = false;
}

bool CPMSeptabsController::IsValidCache(void)
{
 if (!m_valid_cache)
  return false; //RPM, CLT and load grids are not up to date

 for(int i = TYPE_MAP_SEP_START; i <= TYPE_MAP_SEP_END; ++i)
 {
  HWND hwnd = mp_view->mp_SeptabsPanel->GetMapWindow(i);
  if (hwnd)
  { //map is opened
   if (!_isCacheUpToDate(i))
    return false; //cache is not up to date!
  }
 }

 return true;
}

void CPMSeptabsController::ApplyFWOptions(DWORD opt)
{
 Functionality fnc;
 mp_settings->GetFunctionality(fnc);
 mp_view->mp_SeptabsPanel->EnableDwellControl(CHECKBIT32(opt, SECU3IO::COPT_DWELL_CONTROL));
 mp_view->mp_SeptabsPanel->EnableCTSCurve(CHECKBIT32(opt, SECU3IO::COPT_THERMISTOR_CS));
 mp_view->mp_SeptabsPanel->EnableGasdose(fnc.GD_CONTROL && CHECKBIT32(opt, SECU3IO::COPT_GD_CONTROL));
 mp_view->mp_SeptabsPanel->EnableFuelInjection(CHECKBIT32(opt, SECU3IO::COPT_FUEL_INJECT));
 mp_view->mp_SeptabsPanel->EnableTmp2Curve(!CHECKBIT32(opt, SECU3IO::COPT_SECU3T)); 
 mp_view->mp_SeptabsPanel->EnableGrtsCurve(!CHECKBIT32(opt, SECU3IO::COPT_SECU3T)); 
 mp_view->mp_SeptabsPanel->EnableFtlsCurve(!CHECKBIT32(opt, SECU3IO::COPT_SECU3T)); 
 mp_view->mp_SeptabsPanel->EnableEgtsCurve(!CHECKBIT32(opt, SECU3IO::COPT_SECU3T)); 
 mp_view->mp_SeptabsPanel->EnableFtsCurve(!CHECKBIT32(opt, SECU3IO::COPT_SECU3T)); 
 mp_view->mp_SeptabsPanel->EnableOpsCurve(!CHECKBIT32(opt, SECU3IO::COPT_SECU3T)); 
 mp_view->mp_SeptabsPanel->EnableXtauMaps(CHECKBIT32(opt, SECU3IO::COPT_FUEL_INJECT)); 
 mp_view->mp_SeptabsPanel->EnableManInjPwc(!CHECKBIT32(opt, SECU3IO::COPT_SECU3T) && CHECKBIT32(opt, SECU3IO::COPT_FUEL_INJECT)); 
 mp_view->mp_SeptabsPanel->EnableGrHeatDutyMap(!CHECKBIT32(opt, SECU3IO::COPT_SECU3T)); 
 mp_view->mp_SeptabsPanel->EnablePwmIacUCoefMap(CHECKBIT32(opt, SECU3IO::COPT_FUEL_INJECT) || (fnc.GD_CONTROL && CHECKBIT32(opt, SECU3IO::COPT_GD_CONTROL))); 
 mp_view->mp_SeptabsPanel->EnableAftstrStrkMap(CHECKBIT32(opt, SECU3IO::COPT_FUEL_INJECT) || (fnc.GD_CONTROL && CHECKBIT32(opt, SECU3IO::COPT_GD_CONTROL))); 
 mp_view->mp_SeptabsPanel->EnableLambdaZones(CHECKBIT32(opt, SECU3IO::COPT_FUEL_INJECT) || (fnc.GD_CONTROL && CHECKBIT32(opt, SECU3IO::COPT_GD_CONTROL))); 
}


//----------------------------------------------------------------
namespace {
void UpdateMap(float* map, float* flag, const EditTabPar* data)
{
 int address = data->address;
 for(size_t i = 0; i < data->data_size; ++i)
 {
  flag[address] = 1.f; //set flag
  map [address] = data->table_data[i]; //save data
  ++address;
 }
}
}

void CPMSeptabsController::_UpdateCache(const EditTabPar* data)
{
 switch(data->tab_id)
 {
  case TYPE_MAP_ATTENUATOR: UpdateMap(m_maps->attenuator_table, m_maps_flags->attenuator_table, data); break;
  case TYPE_MAP_DWELLCNTRL: UpdateMap(m_maps->dwellcntrl_table, m_maps_flags->dwellcntrl_table, data); break;
  case TYPE_MAP_CTS_CURVE: UpdateMap(m_maps->ctscurve_table, m_maps_flags->ctscurve_table, data); break;
  case TYPE_MAP_ATS_CURVE: UpdateMap(m_maps->atscurve_table, m_maps_flags->atscurve_table, data); break;
  case TYPE_MAP_ATS_CORR: UpdateMap(m_maps->ats_corr_table, m_maps_flags->ats_corr_table, data); break;
  case TYPE_MAP_GASDOSE: UpdateMap(m_maps->gasdose_pos_table, m_maps_flags->gasdose_pos_table, data); break;
  case TYPE_MAP_BAROCORR: UpdateMap(m_maps->barocorr_table, m_maps_flags->barocorr_table, data); break;
  case TYPE_MAP_MANIGNTIM: UpdateMap(m_maps->pa4_igntim_corr, m_maps_flags->pa4_igntim_corr, data); break;
  case TYPE_MAP_TMP2_CURVE: UpdateMap(m_maps->tmp2_curve, m_maps_flags->tmp2_curve, data); break;
  case TYPE_MAP_CRKCLT_CORR: UpdateMap(m_maps->ctscrk_corr, m_maps_flags->ctscrk_corr, data); break;
  case TYPE_MAP_EH_PAUSE: UpdateMap(m_maps->eh_pause_table, m_maps_flags->eh_pause_table, data); break;
  case TYPE_MAP_CRANKING_THRD: UpdateMap(m_maps->cranking_thrd, m_maps_flags->cranking_thrd, data); break;
  case TYPE_MAP_CRANKING_TIME: UpdateMap(m_maps->cranking_time, m_maps_flags->cranking_time, data); break;
  case TYPE_MAP_SMAPABAN_THRD: UpdateMap(m_maps->smapaban_thrd, m_maps_flags->smapaban_thrd, data); break;
  case TYPE_MAP_KNOCK_ZONE: UpdateMap(m_maps->knock_zone, m_maps_flags->knock_zone, data); break;
  case TYPE_MAP_GRTS_CURVE: UpdateMap(m_maps->grts_curve, m_maps_flags->grts_curve, data); break;
  case TYPE_MAP_GRHEAT_DUTY: UpdateMap(m_maps->grheat_duty, m_maps_flags->grheat_duty, data); break;
  case TYPE_MAP_PWMIAC_UCOEF: UpdateMap(m_maps->pwmiac_ucoef, m_maps_flags->pwmiac_ucoef, data); break;
  case TYPE_MAP_AFTSTR_STRK0: UpdateMap(m_maps->aftstr_strk0, m_maps_flags->aftstr_strk0, data); break;
  case TYPE_MAP_AFTSTR_STRK1: UpdateMap(m_maps->aftstr_strk1, m_maps_flags->aftstr_strk1, data); break;
  case TYPE_MAP_GRVDELAY: UpdateMap(m_maps->grv_delay, m_maps_flags->grv_delay, data); break;
  case TYPE_MAP_FTLS_CURVE: UpdateMap(m_maps->ftls_curve, m_maps_flags->ftls_curve, data); break;
  case TYPE_MAP_EGTS_CURVE: UpdateMap(m_maps->egts_curve, m_maps_flags->egts_curve, data); break;
  case TYPE_MAP_OPS_CURVE: UpdateMap(m_maps->ops_curve, m_maps_flags->ops_curve, data); break;
  case TYPE_MAP_MANINJPWC: UpdateMap(m_maps->injpw_coef, m_maps_flags->injpw_coef, data); break;
  case TYPE_MAP_MAF_CURVE: UpdateMap(m_maps->maf_curve, m_maps_flags->maf_curve, data); break;
  case TYPE_MAP_FTLSCOR: UpdateMap(m_maps->ftls_corr, m_maps_flags->ftls_corr, data); break;
  case TYPE_MAP_LAMBDA_ZONE: UpdateMap(m_maps->lambda_zone, m_maps_flags->lambda_zone, data); break;
  case TYPE_MAP_FTS_CURVE: UpdateMap(m_maps->fts_curve, m_maps_flags->fts_curve, data); break;
  case TYPE_MAP_FUELDENS_CORR: UpdateMap(m_maps->fueldens_corr, m_maps_flags->fueldens_corr, data); break;
  case TYPE_MAP_XTAU_XFACC: UpdateMap(m_maps->xtau_xfacc, m_maps_flags->xtau_xfacc, data); break;
  case TYPE_MAP_XTAU_XFDEC: UpdateMap(m_maps->xtau_xfdec, m_maps_flags->xtau_xfdec, data); break;
  case TYPE_MAP_XTAU_TFACC: UpdateMap(m_maps->xtau_tfacc, m_maps_flags->xtau_tfacc, data); break;
  case TYPE_MAP_XTAU_TFDEC: UpdateMap(m_maps->xtau_tfdec, m_maps_flags->xtau_tfdec, data); break;
  case TYPE_MAP_INJNONLINP: UpdateMap(m_maps->inj_nonlinp_corr, m_maps_flags->inj_nonlinp_corr, data); break;
  case TYPE_MAP_INJNONLING: UpdateMap(m_maps->inj_nonling_corr, m_maps_flags->inj_nonling_corr, data); break;
  default: ASSERT(0);
   //error
   break;
 }
}

namespace {
bool _FindZero(float* array, size_t size)
{
 for(size_t i = 0; i < size; ++i)
  if (array[i] == .0f)
   return false;
 return true;
}
}


bool CPMSeptabsController::_isCacheUpToDate(int id)
{
 return _FindZero(_GetMap(id, false, m_maps_flags), _GetMapSize(id));
}

bool CPMSeptabsController::_IsModificationMade(int i_mapId) const
{
 if (false==_CompareViewMap(i_mapId, _GetMapSize(i_mapId)))
  return true;

 return false; //no modifications
}

void CPMSeptabsController::_SynchronizeMap(int i_mapType)
{
 float* pMap = mp_view->GetMap(i_mapType, false); //<--current
 size_t mapSize = _GetMapSize(i_mapType); //map size in items (not bytes)

 size_t pieceSize = 16; //for all maps exept dwell
 if (i_mapType == TYPE_MAP_DWELLCNTRL || i_mapType == TYPE_MAP_CTS_CURVE || i_mapType == TYPE_MAP_ATS_CURVE || i_mapType == TYPE_MAP_TMP2_CURVE || i_mapType == TYPE_MAP_GRTS_CURVE || i_mapType == TYPE_MAP_FTS_CURVE || i_mapType == TYPE_MAP_FTLS_CURVE || i_mapType == TYPE_MAP_EGTS_CURVE || i_mapType == TYPE_MAP_OPS_CURVE || i_mapType == TYPE_MAP_MAF_CURVE || i_mapType == TYPE_MAP_BAROCORR || i_mapType == TYPE_MAP_AFTSTR_STRK0 || i_mapType == TYPE_MAP_AFTSTR_STRK1 || i_mapType == TYPE_MAP_PWMIAC_UCOEF || i_mapType == TYPE_MAP_GRVDELAY || i_mapType == TYPE_MAP_MANINJPWC || i_mapType == TYPE_MAP_FTLSCOR || i_mapType == TYPE_MAP_FUELDENS_CORR || i_mapType == TYPE_MAP_XTAU_XFACC || i_mapType == TYPE_MAP_XTAU_XFDEC || i_mapType == TYPE_MAP_XTAU_TFACC || i_mapType == TYPE_MAP_XTAU_TFDEC || i_mapType == TYPE_MAP_INJNONLINP || i_mapType == TYPE_MAP_INJNONLING)
  pieceSize = 8;
 else if (i_mapType == TYPE_MAP_KNOCK_ZONE || i_mapType == TYPE_MAP_LAMBDA_ZONE)
  pieceSize = F_WRK_POINTS_F*2;

 size_t index = 0;
 EditTabPar packet;
 packet.data_size = 0;
 int state = 0;
 for(size_t a = 0; a < mapSize; ++a)
 {
  switch(state)
  {
   case 0:
    if (pMap[a] != _GetMap(i_mapType, false)[a]) //find address of the first modification
    {
     index = 0;
     packet.tab_id = i_mapType;
     packet.address = a;
     packet.table_data[index] = pMap[a];
     packet.data_size = 1;
     state = 1;
    }
    break;
   case 1:
    packet.table_data[index] = pMap[a];
    if (pMap[a] != _GetMap(i_mapType, false)[a])
     packet.data_size = index + 1;
    break;
  }

  //Split big volume of data in to small pieces (8 or 16 items in one piece)
  ++index;
  if ((pieceSize==index || a==(mapSize-1)) && packet.data_size > 0)
  {
   CString statusStr;
   statusStr.Format(MLL::LoadString(IDS_PM_WRITING_TABLES), i_mapType - TYPE_MAP_SEP_START); 
   mp_sbar->SetInformationText(statusStr);
   //store values of padding cells from cache
   packet.beginPadding = (packet.address > 0) ? _GetMap(i_mapType, false)[packet.address-1] : 0;
   packet.endPadding = (packet.address + packet.data_size) < _GetMapSize(i_mapType) ? _GetMap(i_mapType, false)[(packet.address + packet.data_size)] : 0;

   size_t packet_data_size = packet.data_size;
   size_t packet_address = packet.address;
   
   if (i_mapType == TYPE_MAP_KNOCK_ZONE || i_mapType == TYPE_MAP_LAMBDA_ZONE)
   { //allign data with 16 items boundaries (left and right)
    float block[68];
    size_t nleft = packet.address % F_WRK_POINTS_F;
    size_t rem = ((packet.address + packet.data_size) % F_WRK_POINTS_F);
    size_t nright = rem==0 ? 0 : (F_WRK_POINTS_F - rem);
    std::copy(packet.table_data, packet.table_data + packet.data_size, block);
    for(size_t i = 0; i < nleft; ++i)
     packet.table_data[i] = _GetMap(i_mapType, false)[(packet.address - nleft) + i];
    std::copy(block, block + packet.data_size, packet.table_data + nleft);
    for(size_t i = 0; i < nright; ++i)
     packet.table_data[nleft + packet.data_size + i] = _GetMap(i_mapType, false)[packet.address + packet.data_size + i];
    packet.data_size = nleft + packet.data_size + nright; //correct size of data
    packet.address = (packet.address - nleft) / F_WRK_POINTS_F; //correct address
   }   
/* TRACE("\naddr: %d, size: %d\n", packet.address, packet.data_size);
   for (size_t i = 0; i < packet.data_size; i++)
   {
    int v = (int)packet.table_data[i];
    TRACE("%d ", v);
   }*/
  
   //send packet!
   mp_comm->m_pControlApp->SendPacket(EDITAB_PAR, &packet);
   Sleep(20);
   //transfer copied values from view into cache (save modification)
   for(size_t i = 0; i < packet_data_size; ++i)
    _GetMap(i_mapType, false)[packet_address + i] = pMap[packet_address + i];
   packet.data_size = 0;
   state = 0;
  }
 }
}

//----------------------------------------------------------------
void CPMSeptabsController::OnMapChanged(int i_mapType)
{
 mp_view->SetModificationFlag(_IsModificationMade(i_mapType));
 
 _SynchronizeMap(i_mapType); //send to SECU-3 changed fragment(s)

 mp_sbar->SetInformationText(MLL::LoadString(IDS_PM_READY));
}

void CPMSeptabsController::OnCloseMapWnd(HWND i_hwnd, int i_mapType)
{
 MapWndScrPos::OnCloseMapWnd(i_hwnd, i_mapType);
}

void CPMSeptabsController::OnOpenMapWnd(HWND i_hwnd, int i_mapType)
{
 if (!_isCacheUpToDate(i_mapType))
 {
  m_collectId = i_mapType;
  m_OnSepReqDataColl();
 }

 MapWndScrPos::OnOpenMapWnd(i_hwnd, i_mapType);
}

void CPMSeptabsController::OnSaveButton(void)
{
 //load list of names from firmware
 CFirmwareTabController* p_fwdcntr = static_cast<CFirmwareTabController*>
 (TabControllersCommunicator::GetInstance()->GetReference(TCC_FIRMWARE_TAB_CONTROLLER));
 if (p_fwdcntr->IsFirmwareOpened())
 {
  for(int id = TYPE_MAP_SEP_START; id <= TYPE_MAP_SEP_END; ++id)
  {
   if (_isCacheUpToDate(id))
    p_fwdcntr->GetFWDM()->SetSepMap(id, _GetMap(id, false));
  }

  //reset modification flag
  _ResetModification();
  mp_view->SetModificationFlag(false);
 }
}


//----------------------------------------------------------------
void CPMSeptabsController::OnDataCollected(int tabId /*= -1*/)
{
 if (tabId==-1)
 { //All maps
  mp_view->SetModificationFlag(false);
  _ResetModification(); //original=current
  _MoveMapsToCharts(false);
  _MoveMapsToCharts(true);  
  mp_view->UpdateOpenedCharts();
 }
 else
 { //particular map
  _ResetModification(tabId);
  _MoveMapToChart(tabId, false);
  _MoveMapToChart(tabId, true);
  mp_view->UpdateOpenedCharts(); //TODO: update only particular map
 }
}

void CPMSeptabsController::OnClose()
{
 //save positions of windows of opened charts
 for(int i = TYPE_MAP_SEP_START; i <= TYPE_MAP_SEP_END; ++i)
  OnCloseMapWnd(mp_view->mp_SeptabsPanel->GetMapWindow(i), i);
}

void CPMSeptabsController::OnCloseNotify()
{
 mp_view->CloseAllCharts();
}

void CPMSeptabsController::OnSaveSettings()
{
 //save positions of windows of opened charts
 for(int i = TYPE_MAP_SEP_START; i <= TYPE_MAP_SEP_END; ++i)
  OnCloseMapWnd(mp_view->mp_SeptabsPanel->GetMapWindow(i), i);
}

void CPMSeptabsController::OnChangeSettings(void)
{
 MapPtMovStep mptms;
 mp_settings->GetMapPtMovStep(mptms);
 mptms.m_attenuator_map = mp_view->mp_SeptabsPanel->GetPtMovStep(TYPE_MAP_ATTENUATOR);
 mptms.m_dwellcntrl_map = mp_view->mp_SeptabsPanel->GetPtMovStep(TYPE_MAP_DWELLCNTRL);
 mptms.m_cts_curve_map = mp_view->mp_SeptabsPanel->GetPtMovStep(TYPE_MAP_CTS_CURVE);
 mptms.m_ats_curve_map = mp_view->mp_SeptabsPanel->GetPtMovStep(TYPE_MAP_ATS_CURVE);
 mptms.m_ats_aac_map = mp_view->mp_SeptabsPanel->GetPtMovStep(TYPE_MAP_ATS_CORR);
 mptms.m_gasdose_map = mp_view->mp_SeptabsPanel->GetPtMovStep(TYPE_MAP_GASDOSE);
 mptms.m_barocorr_map = mp_view->mp_SeptabsPanel->GetPtMovStep(TYPE_MAP_BAROCORR);
 mptms.m_manigntim_map = mp_view->mp_SeptabsPanel->GetPtMovStep(TYPE_MAP_MANIGNTIM);
 mptms.m_tmp2_curve_map = mp_view->mp_SeptabsPanel->GetPtMovStep(TYPE_MAP_TMP2_CURVE);
 mptms.m_crktemp_map = mp_view->mp_SeptabsPanel->GetPtMovStep(TYPE_MAP_CRKCLT_CORR);
 mptms.m_eh_pause_map = mp_view->mp_SeptabsPanel->GetPtMovStep(TYPE_MAP_EH_PAUSE);
 mptms.m_cranking_thrd_map = mp_view->mp_SeptabsPanel->GetPtMovStep(TYPE_MAP_CRANKING_THRD);
 mptms.m_cranking_time_map = mp_view->mp_SeptabsPanel->GetPtMovStep(TYPE_MAP_CRANKING_TIME);
 mptms.m_smapaban_thrd_map = mp_view->mp_SeptabsPanel->GetPtMovStep(TYPE_MAP_SMAPABAN_THRD);
 mptms.m_knock_zone_map = mp_view->mp_SeptabsPanel->GetPtMovStep(TYPE_MAP_KNOCK_ZONE);
 mptms.m_grts_curve_map = mp_view->mp_SeptabsPanel->GetPtMovStep(TYPE_MAP_GRTS_CURVE);
 mptms.m_grheat_duty_map = mp_view->mp_SeptabsPanel->GetPtMovStep(TYPE_MAP_GRHEAT_DUTY);
 mptms.m_pwmiac_ucoef_map = mp_view->mp_SeptabsPanel->GetPtMovStep(TYPE_MAP_PWMIAC_UCOEF);
 mptms.m_aftstr_strk0_map = mp_view->mp_SeptabsPanel->GetPtMovStep(TYPE_MAP_AFTSTR_STRK0);
 mptms.m_aftstr_strk1_map = mp_view->mp_SeptabsPanel->GetPtMovStep(TYPE_MAP_AFTSTR_STRK1);
 mptms.m_grvaldel_map = mp_view->mp_SeptabsPanel->GetPtMovStep(TYPE_MAP_GRVDELAY);
 mptms.m_ftls_curve_map = mp_view->mp_SeptabsPanel->GetPtMovStep(TYPE_MAP_FTLS_CURVE);
 mptms.m_egts_curve_map = mp_view->mp_SeptabsPanel->GetPtMovStep(TYPE_MAP_EGTS_CURVE);
 mptms.m_ops_curve_map = mp_view->mp_SeptabsPanel->GetPtMovStep(TYPE_MAP_OPS_CURVE);
 mptms.m_maninjpwc_map = mp_view->mp_SeptabsPanel->GetPtMovStep(TYPE_MAP_MANINJPWC);
 mptms.m_mafcurve_map = mp_view->mp_SeptabsPanel->GetPtMovStep(TYPE_MAP_MAF_CURVE);
 mptms.m_ftlscor_map = mp_view->mp_SeptabsPanel->GetPtMovStep(TYPE_MAP_FTLSCOR);
 mptms.m_lambda_zone_map = mp_view->mp_SeptabsPanel->GetPtMovStep(TYPE_MAP_LAMBDA_ZONE);
 mptms.m_fts_curve_map = mp_view->mp_SeptabsPanel->GetPtMovStep(TYPE_MAP_FTS_CURVE);
 mptms.m_fueldens_corr_map = mp_view->mp_SeptabsPanel->GetPtMovStep(TYPE_MAP_FUELDENS_CORR);
 mptms.m_xtau_xfacc_map = mp_view->mp_SeptabsPanel->GetPtMovStep(TYPE_MAP_XTAU_XFACC);
 mptms.m_xtau_xfdec_map = mp_view->mp_SeptabsPanel->GetPtMovStep(TYPE_MAP_XTAU_XFDEC);
 mptms.m_xtau_tfacc_map = mp_view->mp_SeptabsPanel->GetPtMovStep(TYPE_MAP_XTAU_TFACC);
 mptms.m_xtau_tfdec_map = mp_view->mp_SeptabsPanel->GetPtMovStep(TYPE_MAP_XTAU_TFDEC);
 mptms.m_injnonlinp_map = mp_view->mp_SeptabsPanel->GetPtMovStep(TYPE_MAP_INJNONLINP);
 mptms.m_injnonling_map = mp_view->mp_SeptabsPanel->GetPtMovStep(TYPE_MAP_INJNONLING);
 mp_settings->SetMapPtMovStep(mptms);

 if (m_OnChangeSettings)
  m_OnChangeSettings();
}

void CPMSeptabsController::setOnChangeSettings(EventHandler OnCB)
{
 m_OnChangeSettings = OnCB;
}

void CPMSeptabsController::OnSettingsChanged(void)
{
 mp_view->mp_SeptabsPanel->SetCSVSepSymbol(mp_settings->GetMapCSVSepSymbol());
}
