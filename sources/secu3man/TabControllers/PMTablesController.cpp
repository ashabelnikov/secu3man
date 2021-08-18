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

/** \file PMTablesController.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "Resources/resource.h"
#include "PMTablesController.h"

#include <algorithm>
#include "Application/CommunicationManager.h"
#include "common/fastdelegate.h"
#include "io-core/bitmask.h"
#include "io-core/FirmwareMapsDataHolder.h"
#include "io-core/SECU3IO.h"
#include "io-core/ufcodes.h"
#include "MainFrame/StatusBarManager.h"
#include "ParamDesk/Tables/TablesDeskDlg.h"
#include "TablDesk/ButtonsPanel.h"
#include "TablDesk/MapIds.h"
#include "fwimpexp/S3FFileDataIO.h"
#include "fwimpexp/S3FImpExpController.h"
#include "Settings/IsettingsData.h"
#include "ui-core/FileDialogEx.h"
#include "ui-core/MsgBox.h"

using namespace fastdelegate;
using namespace SECU3IO;

//------------------------------ Helpful functions ----------------------------------
bool CPMTablesController::_CompareViewMap(int i_mapType, size_t size) const
{
 return std::equal(mp_view->GetMap(i_mapType, false),
                   mp_view->GetMap(i_mapType, false) + size,
                   mp_view->GetMap(i_mapType, true));
}

float* CPMTablesController::_GetMap(int i_mapType, bool i_original, SECU3FWMapsItem* p_maps /*= NULL*/)
{
 if (!p_maps)
 {
  p_maps = i_original ? m_omaps : m_maps;
 }

 switch(i_mapType)
 {//ignition maps
  case TYPE_MAP_DA_START:
   return p_maps->f_str;
  case TYPE_MAP_DA_IDLE:
   return p_maps->f_idl;
  case TYPE_MAP_DA_WORK:
   return p_maps->f_wrk;
  case TYPE_MAP_DA_TEMP_CORR:
   return p_maps->f_tmp;
  case TYPE_MAP_DA_TEMPI_CORR:
   return p_maps->f_tmp_idl;
  //fuel injection
  case TYPE_MAP_INJ_VE:
   return p_maps->inj_ve;
  case TYPE_MAP_INJ_VE2:
   return p_maps->inj_ve2;
  case TYPE_MAP_INJ_AFR:
   return p_maps->inj_afr;
  case TYPE_MAP_INJ_CRNK:
   return p_maps->inj_cranking;
  case TYPE_MAP_INJ_WRMP:
   return p_maps->inj_warmup;
  case TYPE_MAP_INJ_DEAD:
   return p_maps->inj_dead_time;
  case TYPE_MAP_INJ_IDLR:
   return p_maps->inj_iac_run_pos;
  case TYPE_MAP_INJ_IDLC:
   return p_maps->inj_iac_crank_pos;
  case TYPE_MAP_INJ_AETPS:
   return p_maps->inj_ae_tps;
  case TYPE_MAP_INJ_AERPM:
   return p_maps->inj_ae_rpm;
  case TYPE_MAP_INJ_AFTSTR:
   return p_maps->inj_aftstr;
  case TYPE_MAP_INJ_IT:
   return p_maps->inj_timing;
  case TYPE_MAP_INJ_ITRPM:
   return p_maps->inj_target_rpm;
  case TYPE_MAP_INJ_RIGID:
   return p_maps->inj_idl_rigidity;
  case TYPE_MAP_INJ_EGOCRV:
   return p_maps->inj_ego_curve;
  case TYPE_MAP_INJ_IACC:
   return p_maps->inj_iac_corr;
  case TYPE_MAP_INJ_IACCW:
   return p_maps->inj_iac_corr_w;
  case TYPE_MAP_INJ_IATCLT:
   return p_maps->inj_iatclt_corr;
  case TYPE_MAP_INJ_TPSSWT:
   return p_maps->inj_tpsswt;
  case TYPE_MAP_INJ_GTSC:
   return p_maps->inj_gts_corr;
  case TYPE_MAP_INJ_GPSC:
   return p_maps->inj_gps_corr;
  case TYPE_MAP_INJ_ATSC:
   return p_maps->inj_ats_corr;
  case TYPE_MAP_PWM1:
   return p_maps->pwm_duty1;
  case TYPE_MAP_PWM2:
   return p_maps->pwm_duty2;
  case TYPE_MAP_INJ_IACMAT:
   return p_maps->iac_mat_corr;
 }
 return NULL; //undefined type of map
}

namespace {
size_t _GetMapSize(int i_mapType)
{
 switch(i_mapType)
 {//ignition maps
  case TYPE_MAP_DA_START:
   return F_STR_POINTS;
  case TYPE_MAP_DA_IDLE:
   return F_IDL_POINTS;
  case TYPE_MAP_DA_WORK:
   return F_WRK_POINTS_L * F_WRK_POINTS_F;
  case TYPE_MAP_DA_TEMP_CORR:
   return F_TMP_POINTS;
  case TYPE_MAP_DA_TEMPI_CORR:
   return F_TMP_POINTS;
  //fuel injection maps
  case TYPE_MAP_INJ_VE:
   return INJ_VE_POINTS_L * INJ_VE_POINTS_F;
  case TYPE_MAP_INJ_VE2:
   return INJ_VE_POINTS_L * INJ_VE_POINTS_F;
  case TYPE_MAP_INJ_AFR:
   return INJ_VE_POINTS_L * INJ_VE_POINTS_F;
  case TYPE_MAP_INJ_CRNK:
   return INJ_CRANKING_LOOKUP_TABLE_SIZE;
  case TYPE_MAP_INJ_WRMP:
   return INJ_WARMUP_LOOKUP_TABLE_SIZE;
  case TYPE_MAP_INJ_DEAD:
   return INJ_DT_LOOKUP_TABLE_SIZE;
  case TYPE_MAP_INJ_IDLR:
  case TYPE_MAP_INJ_IDLC:
   return INJ_IAC_POS_TABLE_SIZE;
  case TYPE_MAP_INJ_AETPS:
   return INJ_AE_TPS_LOOKUP_TABLE_SIZE * 2; //bins and values
  case TYPE_MAP_INJ_AERPM:
   return INJ_AE_RPM_LOOKUP_TABLE_SIZE * 2; //bins and values
  case TYPE_MAP_INJ_AFTSTR:
   return INJ_AFTSTR_LOOKUP_TABLE_SIZE;
  case TYPE_MAP_INJ_IT:
   return INJ_VE_POINTS_L * INJ_VE_POINTS_F;
  case TYPE_MAP_INJ_ITRPM:
   return INJ_TARGET_RPM_TABLE_SIZE;
  case TYPE_MAP_INJ_RIGID:
   return INJ_IDL_RIGIDITY_TABLE_SIZE;
  case TYPE_MAP_INJ_EGOCRV:
   return INJ_EGO_CURVE_SIZE+2;
  case TYPE_MAP_INJ_IACC:
   return INJ_IAC_CORR_SIZE+2;
  case TYPE_MAP_INJ_IACCW:
   return INJ_IAC_CORR_W_SIZE+2;
  case TYPE_MAP_INJ_IATCLT:
   return INJ_IATCLT_CORR_SIZE+2;
  case TYPE_MAP_INJ_TPSSWT:
   return INJ_TPSSWT_SIZE;
  case TYPE_MAP_INJ_GTSC:
   return INJ_GTS_CORR_SIZE;
  case TYPE_MAP_INJ_GPSC:
   return INJ_GPS_CORR_SIZE+2;
  case TYPE_MAP_INJ_ATSC:
   return INJ_ATS_CORR_SIZE;
  case TYPE_MAP_PWM1:
   return F_WRK_POINTS_L * F_WRK_POINTS_F;
  case TYPE_MAP_PWM2:
   return F_WRK_POINTS_L * F_WRK_POINTS_F;
  case TYPE_MAP_INJ_IACMAT:
   return INJ_ATS_CORR_SIZE;
 }
 ASSERT(0);
 return 0; //undefined type of map
}
}

void CPMTablesController::_MoveMapToChart(int i_mapType, bool i_original, SECU3FWMapsItem* p_maps /*= NULL*/)
{
 std::copy(_GetMap(i_mapType, i_original, p_maps),
  _GetMap(i_mapType, i_original, p_maps) + _GetMapSize(i_mapType),
  mp_view->GetMap(i_mapType, i_original));
}

void CPMTablesController::_MoveMapsToCharts(bool i_original, SECU3FWMapsItem* p_maps /*= NULL*/)
{//ignition maps
 for(int id = TYPE_MAP_SET_START; id <= TYPE_MAP_SET_END; ++id)
  _MoveMapToChart(id, i_original, p_maps);
}

void CPMTablesController::_ClearAcquisitionFlags(void)
{
 m_maps_flags->name = _T("");

 //See TablDesk/MapIds.h for more information
 for(int id = TYPE_MAP_SET_START; id <= TYPE_MAP_SET_END; ++id)
  std::fill(_GetMap(id, false, m_maps_flags), _GetMap(id, false, m_maps_flags) + _GetMapSize(id), .0f);
}

void CPMTablesController::_ResetModification(void)
{
 (*m_omaps) = (*m_maps);
}

void CPMTablesController::_SetTablesSetName(const _TSTRING name)
{
 _TSTRING str = name;
 //if string consists from all spaces or is empty, then use "<no name>"
 bool has_name = false;
 for(size_t i = 0; i < str.size(); ++i)
  if (str[i]!=_T(' '))
   has_name = true;
 if (0==str.length() || false==has_name)
  str = _T("<no name>");
 //also, remove all spaces from right side of string
 size_t endpos = str.find_last_not_of(_T(" \t"));
 if(_TSTRING::npos != endpos)
  str = str.substr(0, endpos + 1);
 mp_view->SetTablesSetName(str);
}

//-----------------------------------------------------------------------------------
CPMTablesController::CPMTablesController(VIEW* ip_view, CCommunicationManager* ip_comm, CStatusBarManager* ip_sbar, ISettingsData* ip_settings)
: Super(ip_view)
, mp_comm(ip_comm)
, mp_sbar(ip_sbar)
, m_valid_cache(false)
, mp_settings(ip_settings)
, MapWndScrPos(ip_settings, true)
{
 //устанавливаем обработчики событий от view
 mp_view->setOnMapChanged(MakeDelegate(this, &CPMTablesController::OnMapChanged));
 mp_view->setOnCloseMapWnd(MakeDelegate(this, &CPMTablesController::OnCloseMapWnd));
 mp_view->setOnOpenMapWnd(MakeDelegate(this, &CPMTablesController::OnOpenMapWnd));
 mp_view->setOnSaveButton(MakeDelegate(this, &CPMTablesController::OnSaveButton));
 mp_view->setOnChangeTablesSetName(MakeDelegate(this, &CPMTablesController::OnChangeTablesSetName));
 mp_view->setOnLoadTablesFrom(MakeDelegate(this, &CPMTablesController::OnLoadTablesFrom));
 mp_view->setOnSaveTablesTo(MakeDelegate(this, &CPMTablesController::OnSaveTablesTo));
 mp_view->setOnImportFromS3F(MakeDelegate(this, &CPMTablesController::OnImportFromS3F));
 mp_view->setOnExportToS3F(MakeDelegate(this, &CPMTablesController::OnExportToS3F));
 mp_view->mp_ButtonsPanel->setOnChangeSettings(MakeDelegate(this, &CPMTablesController::OnChangeSettings));

 //карты (текущие)
 m_maps = new SECU3FWMapsItem;
 //карты (оригинальные)
 m_omaps = new SECU3FWMapsItem;
 //флаги сбора информации
 m_maps_flags = new SECU3FWMapsItem;

 mp_settings->GetLamDelMap(mp_view->mp_ButtonsPanel->GetLamDelMap(0), mp_view->mp_ButtonsPanel->GetLamDelMap(1), mp_view->mp_ButtonsPanel->GetLamDelMap(2));
 mp_view->mp_ButtonsPanel->SetAFRError(mp_settings->GetAFRError());
 mp_view->mp_ButtonsPanel->SetStatSize(mp_settings->GetStatSize());
 mp_view->mp_ButtonsPanel->SetAutoBlockThrd(mp_settings->GetAutoBlockThrd());
 mp_view->mp_ButtonsPanel->SetGrowingMode(mp_settings->GetGrowingMode());
 mp_settings->GetBlockedCells(mp_view->mp_ButtonsPanel->GetBlockedCells());
 mp_view->mp_ButtonsPanel->SetMinAFR(mp_settings->GetMinAFR());
 mp_view->mp_ButtonsPanel->SetMaxAFR(mp_settings->GetMaxAFR());
 mp_view->mp_ButtonsPanel->SetMinDistThrd(mp_settings->GetMinDistThrd());
 mp_view->mp_ButtonsPanel->SetMinTPS(mp_settings->GetMinTPS());
 mp_view->mp_ButtonsPanel->SetMaxTPS(mp_settings->GetMaxTPS());
 mp_view->mp_ButtonsPanel->SetCLTThrd(mp_settings->GetCLTThrd());
 mp_view->mp_ButtonsPanel->SetMapEditorSettings(mp_settings->GetGradSaturation(), mp_settings->GetGradBrightness(), mp_settings->GetBoldFont(), mp_settings->GetSpotMarkers(), mp_settings->GetSpotMarkersSize());
 mp_view->mp_ButtonsPanel->SetITEdMode(mp_settings->GetITEdMode());
 mp_view->mp_ButtonsPanel->SetActiveVEMap(mp_settings->GetActiveVEMap());
 mp_view->mp_ButtonsPanel->SetTunSoftness(mp_settings->GetTunSoftness());
}

CPMTablesController::~CPMTablesController()
{
 delete m_maps;
 delete m_omaps;
 delete m_maps_flags;

 m_td_changes_timer.KillTimer();
}

//----------------------------------------------------------------
void CPMTablesController::OnActivate(void)
{
 _ClearAcquisitionFlags();
 mp_view->SetReadOnlyTablesSetName(false);

 mp_view->Enable(IsValidCache());

 //запускаем таймер по которому будет ограничиваться частота посылки данных в SECU-3
 m_td_changes_timer.SetTimer(this, &CPMTablesController::OnTableDeskChangesTimer, 250);

 mp_view->mp_ButtonsPanel->SetITEdMode(mp_settings->GetITEdMode());
 mp_view->mp_ButtonsPanel->SetActiveVEMap(mp_settings->GetActiveVEMap());

 MapPtMovStep mptms;
 mp_settings->GetMapPtMovStep(mptms);
 mp_view->mp_ButtonsPanel->SetPtMovStep(TYPE_MAP_DA_START, mptms.m_start_map);
 mp_view->mp_ButtonsPanel->SetPtMovStep(TYPE_MAP_DA_IDLE, mptms.m_idle_map);
 mp_view->mp_ButtonsPanel->SetPtMovStep(TYPE_MAP_DA_WORK, mptms.m_work_map);
 mp_view->mp_ButtonsPanel->SetPtMovStep(TYPE_MAP_DA_TEMP_CORR, mptms.m_temp_map);
 mp_view->mp_ButtonsPanel->SetPtMovStep(TYPE_MAP_DA_TEMPI_CORR, mptms.m_tempi_map);
 mp_view->mp_ButtonsPanel->SetPtMovStep(TYPE_MAP_INJ_VE, mptms.m_ve_map);
 mp_view->mp_ButtonsPanel->SetPtMovStep(TYPE_MAP_INJ_VE2, mptms.m_ve2_map);
 mp_view->mp_ButtonsPanel->SetPtMovStep(TYPE_MAP_INJ_AFR, mptms.m_afr_map);
 mp_view->mp_ButtonsPanel->SetPtMovStep(TYPE_MAP_INJ_CRNK, mptms.m_crnk_map);
 mp_view->mp_ButtonsPanel->SetPtMovStep(TYPE_MAP_INJ_WRMP, mptms.m_wrmp_map);
 mp_view->mp_ButtonsPanel->SetPtMovStep(TYPE_MAP_INJ_DEAD, mptms.m_dead_map);
 mp_view->mp_ButtonsPanel->SetPtMovStep(TYPE_MAP_INJ_IDLR, mptms.m_idlr_map);
 mp_view->mp_ButtonsPanel->SetPtMovStep(TYPE_MAP_INJ_IDLC, mptms.m_idlc_map);
 mp_view->mp_ButtonsPanel->SetPtMovStep(TYPE_MAP_INJ_AETPS, mptms.m_aetps_map);
 mp_view->mp_ButtonsPanel->SetPtMovStep(TYPE_MAP_INJ_AERPM, mptms.m_aerpm_map);
 mp_view->mp_ButtonsPanel->SetPtMovStep(TYPE_MAP_INJ_AFTSTR, mptms.m_aftstr_map);
 mp_view->mp_ButtonsPanel->SetPtMovStep(TYPE_MAP_INJ_IT, mptms.m_it_map);
 mp_view->mp_ButtonsPanel->SetPtMovStep(TYPE_MAP_INJ_ITRPM, mptms.m_itrpm_map);
 mp_view->mp_ButtonsPanel->SetPtMovStep(TYPE_MAP_INJ_RIGID, mptms.m_rigid_map);
 mp_view->mp_ButtonsPanel->SetPtMovStep(TYPE_MAP_INJ_EGOCRV, mptms.m_egocrv_map);
 mp_view->mp_ButtonsPanel->SetPtMovStep(TYPE_MAP_INJ_IACC, mptms.m_iacc_map);
 mp_view->mp_ButtonsPanel->SetPtMovStep(TYPE_MAP_INJ_IACCW, mptms.m_iaccw_map);
 mp_view->mp_ButtonsPanel->SetPtMovStep(TYPE_MAP_INJ_IATCLT, mptms.m_iatclt_map);
 mp_view->mp_ButtonsPanel->SetPtMovStep(TYPE_MAP_INJ_TPSSWT, mptms.m_tpsswt_map);
 mp_view->mp_ButtonsPanel->SetPtMovStep(TYPE_MAP_INJ_GTSC, mptms.m_gtsc_map);
 mp_view->mp_ButtonsPanel->SetPtMovStep(TYPE_MAP_INJ_GPSC, mptms.m_gpsc_map);
 mp_view->mp_ButtonsPanel->SetPtMovStep(TYPE_MAP_INJ_ATSC, mptms.m_atsc_map);
 mp_view->mp_ButtonsPanel->SetPtMovStep(TYPE_MAP_PWM1, mptms.m_pwm1_map);
 mp_view->mp_ButtonsPanel->SetPtMovStep(TYPE_MAP_PWM2, mptms.m_pwm2_map);
 mp_view->mp_ButtonsPanel->SetPtMovStep(TYPE_MAP_INJ_IACMAT, mptms.m_iacmat_map);
}

void CPMTablesController::OnDeactivate(void)
{
 m_td_changes_timer.KillTimer();
}

void CPMTablesController::Enable(bool state)
{
 mp_view->Enable(state);
}

void CPMTablesController::StartDataCollection(void)
{
 Super::StartDataCollection();
}

bool CPMTablesController::CollectData(const BYTE i_descriptor, const void* i_packet_data)
{
 switch(m_operation_state)
 {
  case 0: //Read out FUNSET_PAR
   mp_sbar->SetInformationText(MLL::LoadString(IDS_PM_READING_PARAMS));
   if (i_descriptor != FUNSET_PAR)
    mp_comm->m_pControlApp->ChangeContext(FUNSET_PAR);
   else
   {//save read parameters
    const FunSetPar* data = (const FunSetPar*)i_packet_data;
    mp_view->mp_ButtonsPanel->SetLoadAxisCfg(data->map_lower_pressure, data->map_upper_pressure, data->load_src_cfg, data->use_load_grid);
    mp_view->mp_ButtonsPanel->SetVE2MapFunc(data->ve2_map_func);
    m_operation_state = 1;
   }
   break;

  case 1: //Read out RPM grid
   mp_sbar->SetInformationText(MLL::LoadString(IDS_PM_READING_RPMGRD));
   if (i_descriptor != RPMGRD_PAR)
    mp_comm->m_pControlApp->ChangeContext(RPMGRD_PAR);
   else
   {//save RPM grid
    const SepTabPar* data = (const SepTabPar*)i_packet_data;
    memcpy(m_rpmGrid, data->table_data, F_RPM_SLOTS*sizeof(float)); //save RPM grid
    m_operation_state = 2;
   }
   break;
  case 2: //Read out CLT grid
   mp_sbar->SetInformationText(MLL::LoadString(IDS_PM_READING_CLTGRD));
   if (i_descriptor != CLTGRD_PAR)
    mp_comm->m_pControlApp->ChangeContext(CLTGRD_PAR);
   else
   {//save CLT grid
    const SepTabPar* data = (const SepTabPar*)i_packet_data;
    memcpy(m_cltGrid, data->table_data, F_TMP_SLOTS*sizeof(float)); //save CLT grid
    m_operation_state = 3;
   }
   break;
  case 3: //Read out load grid
   mp_sbar->SetInformationText(MLL::LoadString(IDS_PM_READING_LODGRD));
   if (i_descriptor != LODGRD_PAR)
    mp_comm->m_pControlApp->ChangeContext(LODGRD_PAR);
   else
   {//save load grid
    const SepTabPar* data = (const SepTabPar*)i_packet_data;
    memcpy(m_lodGrid, data->table_data, F_LOAD_SLOTS*sizeof(float)); //save load grid
    m_operation_state = 4;
   }
   break;

  case 4:
   mp_sbar->SetInformationText(MLL::LoadString(IDS_PM_READING_TABLES));
   if (i_descriptor != EDITAB_PAR)
    mp_comm->m_pControlApp->ChangeContext(EDITAB_PAR);
   else
   {//Очищаем флаги, сохраняем принятые данные и идем на продолжение приема
    _ClearAcquisitionFlags();
    const EditTabPar* data = (const EditTabPar*)i_packet_data;
    _UpdateCache(data);
    m_operation_state = 5;
   }
   break;

  case 5:
   {
    if (i_descriptor != EDITAB_PAR)
    {
     m_operation_state = 4;
     break;
    }

    //update chache and perform checking
    const EditTabPar* data = (const EditTabPar*)i_packet_data;
    _UpdateCache(data);
    if (_IsCacheUpToDate())
    {
     m_operation_state = -1;
     mp_sbar->SetInformationText(MLL::LoadString(IDS_PM_READY));
     m_valid_cache = true;
     OnDataCollected();
     return true; //работа КА завершена
    }
   }
   break;
 }
 return false;
}

void CPMTablesController::InvalidateCache(void)
{
 m_valid_cache = false;
}

bool CPMTablesController::IsValidCache(void)
{
 return m_valid_cache;
}

void CPMTablesController::SetFunctionsNames(const std::vector<_TSTRING>& i_fwnames, const std::vector<_TSTRING>& i_eenames)
{
 //note: we insert seperator before item with index = TABLES_NUMBER
 mp_view->SetFunctionsNames(i_fwnames, i_eenames, TABLES_NUMBER);
}

void CPMTablesController::ApplyFWOptions(DWORD opt)
{
 Functionality fnc;
 mp_settings->GetFunctionality(fnc);

 mp_view->mp_ButtonsPanel->EnableFuelInjection(CHECKBIT32(opt, COPT_FUEL_INJECT));
 mp_view->mp_ButtonsPanel->EnableGasdose(fnc.GD_CONTROL && CHECKBIT32(opt, COPT_GD_CONTROL));
 mp_view->mp_ButtonsPanel->EnableCarbAfr(CHECKBIT32(opt, COPT_CARB_AFR));
 bool en_for_gd = (CHECKBIT32(opt, COPT_ATMEGA1284) || CHECKBIT32(opt, COPT_FUEL_INJECT)); //TODO: remove this line after migration to M1284!
 mp_view->mp_ButtonsPanel->EnableGasCorr(!CHECKBIT32(opt, COPT_SECU3T) && en_for_gd);
 mp_view->mp_ButtonsPanel->EnableChokeOp(fnc.SM_CONTROL && CHECKBIT32(opt, COPT_SM_CONTROL));
 mp_comm->m_pControlApp->SetSplitAngMode(CHECKBIT32(opt, COPT_SPLIT_ANGLE)); 
 mp_view->mp_ButtonsPanel->SetSplitAngMode(CHECKBIT32(opt, COPT_SPLIT_ANGLE)); 
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
void CPMTablesController::_UpdateCache(const EditTabPar* data)
{
 switch(data->tab_id)
 {
  case ETMT_NAME_STR: //name of set
   m_maps->name = data->name_data;
   m_maps_flags->name = _T("1");
   break;
  //ignition maps
  case ETMT_STRT_MAP: //start map
   UpdateMap(m_maps->f_str, m_maps_flags->f_str, data);
   break;
  case ETMT_IDLE_MAP: //idle map
   UpdateMap(m_maps->f_idl, m_maps_flags->f_idl, data);
   break;
  case ETMT_WORK_MAP: //work map
   UpdateMap(m_maps->f_wrk, m_maps_flags->f_wrk, data);
   break;
  case ETMT_TEMP_MAP: //temp. corr. map (working)
   UpdateMap(m_maps->f_tmp, m_maps_flags->f_tmp, data);
   break;
  case ETMT_TEMPI_MAP: //temp. corr. map (idling)
   UpdateMap(m_maps->f_tmp_idl, m_maps_flags->f_tmp_idl, data);
   break;
  //fuel injection maps
  case ETMT_VE_MAP: //ve map
   UpdateMap(m_maps->inj_ve, m_maps_flags->inj_ve, data);
   break;
  case ETMT_VE2_MAP: //ve map
   UpdateMap(m_maps->inj_ve2, m_maps_flags->inj_ve2, data);
   break;
  case ETMT_AFR_MAP: //afr map
   UpdateMap(m_maps->inj_afr, m_maps_flags->inj_afr, data);
   break;
  case ETMT_CRNK_MAP: //cranking pw map
   UpdateMap(m_maps->inj_cranking, m_maps_flags->inj_cranking, data);
   break;
  case ETMT_WRMP_MAP: //warmup enrichment map
   UpdateMap(m_maps->inj_warmup, m_maps_flags->inj_warmup, data);
   break;
  case ETMT_DEAD_MAP: //injector's dead time map
   UpdateMap(m_maps->inj_dead_time, m_maps_flags->inj_dead_time, data);
   break;
  case ETMT_IDLR_MAP: //run IAC pos map
   UpdateMap(m_maps->inj_iac_run_pos, m_maps_flags->inj_iac_run_pos, data);
   break;
  case ETMT_IDLC_MAP: //cranking IAC pos map
   UpdateMap(m_maps->inj_iac_crank_pos, m_maps_flags->inj_iac_crank_pos, data);
   break;
  case ETMT_AETPS_MAP: //AE TPS map
   UpdateMap(m_maps->inj_ae_tps, m_maps_flags->inj_ae_tps, data);
   break;
  case ETMT_AERPM_MAP: //AE TPS map
   UpdateMap(m_maps->inj_ae_rpm, m_maps_flags->inj_ae_rpm, data);
   break;
  case ETMT_AFTSTR_MAP: //afterstart enrichment map
   UpdateMap(m_maps->inj_aftstr, m_maps_flags->inj_aftstr, data);
   break;
  case ETMT_IT_MAP: //inj.timing map
   UpdateMap(m_maps->inj_timing, m_maps_flags->inj_timing, data);
   break;
  case ETMT_ITRPM_MAP: //idling RPM map
   UpdateMap(m_maps->inj_target_rpm, m_maps_flags->inj_target_rpm, data);
   break;
  case ETMT_RIGID_MAP: //idl. regulator's rigidity map
   UpdateMap(m_maps->inj_idl_rigidity, m_maps_flags->inj_idl_rigidity, data);
   break;
  case ETMT_EGOCRV_MAP: //WBO emulation
   UpdateMap(m_maps->inj_ego_curve, m_maps_flags->inj_ego_curve, data);
   break;
  case ETMT_IACC_MAP: //mixture correction vs IAC pos
   UpdateMap(m_maps->inj_iac_corr, m_maps_flags->inj_iac_corr, data);
   break;
  case ETMT_IACCW_MAP: //weight of mixture correction vs IAC pos
   UpdateMap(m_maps->inj_iac_corr_w, m_maps_flags->inj_iac_corr_w, data);
   break;
  case ETMT_IATCLT_MAP: //IAT/CLT correction vs air flow
   UpdateMap(m_maps->inj_iatclt_corr, m_maps_flags->inj_iatclt_corr, data);
   break;
  case ETMT_TPSSWT_MAP: //MAP/TPS switch point vs RPM
   UpdateMap(m_maps->inj_tpsswt, m_maps_flags->inj_tpsswt, data);
   break;
  case ETMT_GTSC_MAP: //Inj. PW correction coefficient  vs gas temperature
   UpdateMap(m_maps->inj_gts_corr, m_maps_flags->inj_gts_corr, data);
   break;
  case ETMT_GPSC_MAP: //Inj. PW correction coefficient  vs gas pressure
   UpdateMap(m_maps->inj_gps_corr, m_maps_flags->inj_gps_corr, data);
   break;
  case ETMT_ATSC_MAP: //Inj. PW correction coefficient  vs air temperature
   UpdateMap(m_maps->inj_ats_corr, m_maps_flags->inj_ats_corr, data);
   break;
  case ETMT_PWM1_MAP: //PWM1 map
   UpdateMap(m_maps->pwm_duty1, m_maps_flags->pwm_duty1, data);
   break;
  case ETMT_PWM2_MAP: //PWM2 map
   UpdateMap(m_maps->pwm_duty2, m_maps_flags->pwm_duty2, data);
   break;
  case ETMT_IACMAT_MAP: //IAC position's correction vs MAT
   UpdateMap(m_maps->iac_mat_corr, m_maps_flags->iac_mat_corr, data);
   break;

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
bool CPMTablesController::_IsCacheUpToDate(void)
{
 if (m_maps_flags->name == _T(""))
  return false;

 //See TablDesk/MapIds.h for more information
 for(int id = TYPE_MAP_SET_START; id <= TYPE_MAP_SET_END; ++id)
 {
  if (!_FindZero(_GetMap(id, false, m_maps_flags), _GetMapSize(id)))
   return false;
 }

 return true;
}

bool CPMTablesController::_IsModificationMade(void) const
{
 if (m_maps->name != m_omaps->name)
  return true;

 //See TablDesk/MapIds.h for more information
 for(int id = TYPE_MAP_SET_START; id <= TYPE_MAP_SET_END; ++id)
 {
  if (false==_CompareViewMap(id, _GetMapSize(id)))
   return true;
 }

 return false; //no modifications
}

void CPMTablesController::_SynchronizeMap(int i_mapType)
{
 float* pMap = mp_view->GetMap(i_mapType, false); //<--current
 size_t mapSize = _GetMapSize(i_mapType); //map size in items (not bytes)

 size_t pieceSize = 16; //for all maps exept cranking PW and inj. dead time and rigidity function
 if (i_mapType == TYPE_MAP_INJ_CRNK || i_mapType == TYPE_MAP_INJ_DEAD || i_mapType == TYPE_MAP_INJ_RIGID || i_mapType == TYPE_MAP_INJ_EGOCRV || i_mapType == TYPE_MAP_INJ_IACC || i_mapType == TYPE_MAP_INJ_IATCLT)
  pieceSize = 8;

 size_t index;
 EditTabPar packet;
 packet.data_size = 0;
 int state = 0;
 for(size_t a = 0; a < mapSize; ++a)
 {
  switch(state)
  {
   case 0:
    if (pMap[a] != _GetMap(i_mapType, false)[a])
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
   statusStr.Format(MLL::LoadString(IDS_PM_WRITING_TABLES), i_mapType); 
   mp_sbar->SetInformationText(statusStr);
   state = 0;   
   //store values of padding cells from cache
   packet.beginPadding = (packet.address > 0) ? _GetMap(i_mapType, false)[packet.address-1] : 0;
   packet.endPadding = (packet.address + packet.data_size) < _GetMapSize(i_mapType) ? _GetMap(i_mapType, false)[(packet.address + packet.data_size)] : 0;
   //send packet!
   mp_comm->m_pControlApp->SendPacket(EDITAB_PAR, &packet);
   Sleep(20);
   //transfer copied values from view into cache (save modification)
   for(size_t i = 0; i < packet.data_size; ++i)
    _GetMap(i_mapType, false)[packet.address + i] = pMap[packet.address + i];
  }
 }
}

//----------------------------------------------------------------
void CPMTablesController::OnMapChanged(int i_mapType)
{
 mp_view->SetModificationFlag(_IsModificationMade());

 //send to SECU-3 changed fragment(s)
 _SynchronizeMap(i_mapType);

 mp_sbar->SetInformationText(MLL::LoadString(IDS_PM_READY));
}

void CPMTablesController::OnCloseMapWnd(HWND i_hwnd, int i_mapType)
{
 if (i_mapType == TYPE_MAP_GME_INJ_WND)
 {
  mp_settings->SetLamDelMap(mp_view->mp_ButtonsPanel->GetLamDelMap(0), mp_view->mp_ButtonsPanel->GetLamDelMap(1), mp_view->mp_ButtonsPanel->GetLamDelMap(2));
  mp_settings->SetBlockedCells(mp_view->mp_ButtonsPanel->GetBlockedCells());
 }

 MapWndScrPos::OnCloseMapWnd(i_hwnd, i_mapType);
}

void CPMTablesController::OnOpenMapWnd(HWND i_hwnd, int i_mapType)
{
 MapWndScrPos::OnOpenMapWnd(i_hwnd, i_mapType);
}

void CPMTablesController::OnSaveButton(void)
{
 //not used
}

void CPMTablesController::OnChangeTablesSetName(void)
{
 mp_sbar->SetInformationText(MLL::LoadString(IDS_PM_WRITING_TABLES_NAME));
 //Update cache and update midification indicator (flag)
 _TSTRING name = mp_view->GetTablesSetName();
 size_t count = F_NAME_SIZE - name.length();
 name.append(count, _T(' '));
 m_maps->name = name;
 mp_view->SetModificationFlag(_IsModificationMade());

 EditTabPar packet;
 packet.tab_id = ETMT_NAME_STR;
 packet.address = 0; //whole content
 _tcscpy(packet.name_data, name.c_str());
 packet.data_size = name.length();
 ASSERT(packet.data_size == F_NAME_SIZE);
 mp_comm->m_pControlApp->SendPacket(EDITAB_PAR, &packet);
 mp_sbar->SetInformationText(MLL::LoadString(IDS_PM_READY));
}

void CPMTablesController::OnLoadTablesFrom(int index)
{
 OPCompNc packet_data;
 packet_data.opcode = OPCODE_LOAD_TABLSET;
 packet_data.opdata = ((BYTE)index);
 mp_comm->m_pControlApp->SendPacket(OP_COMP_NC, &packet_data);
 //reset modification flag
 _ResetModification();
 mp_view->SetModificationFlag(false);
}

void CPMTablesController::OnSaveTablesTo(int index)
{
 mp_sbar->SetInformationText(MLL::LoadString(IDS_PM_WRITING_TABLSET));
 OPCompNc packet_data;
 packet_data.opcode = OPCODE_SAVE_TABLSET;
 packet_data.opdata = ((BYTE)(index + TABLES_NUMBER));
 mp_comm->m_pControlApp->SendPacket(OP_COMP_NC, &packet_data);
 //reset modification flag
 _ResetModification();
 mp_view->SetModificationFlag(false);
}

void CPMTablesController::OnImportFromS3F(void)
{
 //Set data (maps and RPM grid)
 FWMapsDataHolder data(1); //1 set
 data.maps[0] = *m_maps; 
 std::copy(m_rpmGrid, m_rpmGrid + F_RPM_SLOTS, data.rpm_slots);
 std::copy(m_cltGrid, m_cltGrid + F_TMP_SLOTS, data.clt_slots);
 std::copy(m_lodGrid, m_lodGrid + F_LOAD_SLOTS, data.load_slots);

 S3FImportController import(&data, false); //without separate maps

 int result = import.DoImport();
 if (result == IDOK)
 {
  //Update view
  _MoveMapsToCharts(false, &data.maps[0]); //current
  _MoveMapsToCharts(true, &data.maps[0]);  //original
   mp_view->SetRPMGrid(m_rpmGrid);         //RPM grid
   mp_view->SetCLTGrid(m_cltGrid);         //CLT grid
   mp_view->SetLoadGrid(m_lodGrid);        //load grid
  _SetTablesSetName(data.maps[0].name);    //name
  
  //Send updated set name to SECU-3
  OnChangeTablesSetName();

  mp_view->TransformValues(); //transform values in some maps before they will be rendered for user

  //send updated data to SECU-3
  //See TablDesk/MapIds.h for more information
  for(int id = TYPE_MAP_SET_START; id <= TYPE_MAP_SET_END; ++id)
   _SynchronizeMap(id);

  mp_sbar->SetInformationText(MLL::LoadString(IDS_PM_READY));

  //update stored map values
  *m_maps = data.maps[0];  //current
  *m_omaps = data.maps[0]; //original
  std::copy(data.rpm_slots, data.rpm_slots + F_RPM_SLOTS, m_rpmGrid);
  std::copy(data.clt_slots, data.clt_slots + F_TMP_SLOTS, m_cltGrid);
  std::copy(data.load_slots, data.load_slots + F_LOAD_SLOTS, m_lodGrid);

  //update view and update modification flag if any of maps changed
  mp_view->UpdateOpenedCharts();
  mp_view->SetModificationFlag(_IsModificationMade());
 }
}

void CPMTablesController::OnExportToS3F(void)
{
 //этот класс необходим чтобы изменять дефаултное расширение в зависимости от выбранного фильтра
 class CFileDialogEx1 : public CFileDialogEx
 {
  public:
   CFileDialogEx1() : CFileDialogEx(FALSE,_T("s3f"), NULL, NULL,
     _T("SECU-3 Format Files (*.s3f)|*.s3f|All Files (*.*)|*.*||"), NULL) {};
   virtual ~CFileDialogEx1() {};

  protected:
   virtual void OnTypeChange( )
   {
    if (m_ofn.nFilterIndex==1)
     m_ofn.lpstrDefExt = _T("s3f");
   }
 };

 CFileDialogEx1 save;
 S3FFileDataIO s3f_io(false); //no separate maps

 //Set data (maps and RPM grid)
 s3f_io.GetDataLeft() = FWMapsDataHolder(1); //resize to 1 set
 s3f_io.GetDataLeft().maps[0] = *m_maps; 
 std::copy(m_rpmGrid, m_rpmGrid + F_RPM_SLOTS, s3f_io.GetDataLeft().rpm_slots);
 std::copy(m_cltGrid, m_cltGrid + F_TMP_SLOTS, s3f_io.GetDataLeft().clt_slots);
 std::copy(m_lodGrid, m_lodGrid + F_LOAD_SLOTS, s3f_io.GetDataLeft().load_slots);

 //saveto file
 if (save.DoModal()==IDOK)
 {
  bool result = s3f_io.Save(_TSTRING(save.GetPathName()));
  if (!result)
   SECUMessageBox(MLL::LoadString(IDS_CANT_SAVE_THIS_FILE),MB_OK|MB_ICONWARNING);
 }
}

//----------------------------------------------------------------
void CPMTablesController::OnDataCollected(void)
{
 _ResetModification(); //original=current
 _MoveMapsToCharts(false);
 _MoveMapsToCharts(true);
 //Set RPM grid read out from SECU-3
 mp_view->SetRPMGrid(m_rpmGrid);
 //Set CLT grid read out from SECU-3
 mp_view->SetCLTGrid(m_cltGrid);
 //Set load grid read out from SECU-3
 mp_view->SetLoadGrid(m_lodGrid);
 mp_view->TransformValues(); //transform values in some maps before they will be rendered for user
 mp_view->UpdateOpenedCharts();
 _SetTablesSetName(m_maps->name);
}

void CPMTablesController::OnTableDeskChangesTimer(void)
{
 //todo
}

void CPMTablesController::OnClose()
{
 //save positions of windows of opened charts
 for(int i = TYPE_MAP_SET_START; i <= TYPE_MAP_SET_END; ++i)
  OnCloseMapWnd(mp_view->mp_ButtonsPanel->GetMapWindow(i), i);
 OnCloseMapWnd(mp_view->mp_ButtonsPanel->GetMapWindow(TYPE_MAP_GME_IGN_WND), TYPE_MAP_GME_IGN_WND);
 OnCloseMapWnd(mp_view->mp_ButtonsPanel->GetMapWindow(TYPE_MAP_GME_INJ_WND), TYPE_MAP_GME_INJ_WND);
}

void CPMTablesController::OnCloseNotify()
{
 mp_view->CloseAllCharts();
 mp_settings->SetLamDelMap(mp_view->mp_ButtonsPanel->GetLamDelMap(0), mp_view->mp_ButtonsPanel->GetLamDelMap(1), mp_view->mp_ButtonsPanel->GetLamDelMap(2));
 mp_settings->SetBlockedCells(mp_view->mp_ButtonsPanel->GetBlockedCells());
}

void CPMTablesController::OnSaveSettings()
{
 //save positions of windows of opened charts
 for(int i = TYPE_MAP_SET_START; i <= TYPE_MAP_SET_END; ++i)
  OnCloseMapWnd(mp_view->mp_ButtonsPanel->GetMapWindow(i), i);
 OnCloseMapWnd(mp_view->mp_ButtonsPanel->GetMapWindow(TYPE_MAP_GME_IGN_WND), TYPE_MAP_GME_IGN_WND);
 OnCloseMapWnd(mp_view->mp_ButtonsPanel->GetMapWindow(TYPE_MAP_GME_INJ_WND), TYPE_MAP_GME_INJ_WND);
}

void CPMTablesController::OnFunSetChanged(const SECU3IO::FunSetPar* data)
{
 mp_view->mp_ButtonsPanel->SetLoadAxisCfg(data->map_lower_pressure, data->map_upper_pressure, data->load_src_cfg, data->use_load_grid);
 mp_view->mp_ButtonsPanel->SetVE2MapFunc(data->ve2_map_func);
}

void CPMTablesController::OnChangeSettings(void)
{
 mp_settings->SetITEdMode(mp_view->mp_ButtonsPanel->GetITEdMode());
 mp_settings->SetActiveVEMap(mp_view->mp_ButtonsPanel->GetActiveVEMap());

 MapPtMovStep mptms;
 mp_settings->GetMapPtMovStep(mptms);
 mptms.m_start_map = mp_view->mp_ButtonsPanel->GetPtMovStep(TYPE_MAP_DA_START);
 mptms.m_idle_map = mp_view->mp_ButtonsPanel->GetPtMovStep(TYPE_MAP_DA_IDLE);
 mptms.m_work_map = mp_view->mp_ButtonsPanel->GetPtMovStep(TYPE_MAP_DA_WORK);
 mptms.m_temp_map = mp_view->mp_ButtonsPanel->GetPtMovStep(TYPE_MAP_DA_TEMP_CORR);
 mptms.m_tempi_map = mp_view->mp_ButtonsPanel->GetPtMovStep(TYPE_MAP_DA_TEMPI_CORR);
 mptms.m_ve_map = mp_view->mp_ButtonsPanel->GetPtMovStep(TYPE_MAP_INJ_VE);
 mptms.m_ve2_map = mp_view->mp_ButtonsPanel->GetPtMovStep(TYPE_MAP_INJ_VE2);
 mptms.m_afr_map = mp_view->mp_ButtonsPanel->GetPtMovStep(TYPE_MAP_INJ_AFR);
 mptms.m_crnk_map = mp_view->mp_ButtonsPanel->GetPtMovStep(TYPE_MAP_INJ_CRNK);
 mptms.m_wrmp_map = mp_view->mp_ButtonsPanel->GetPtMovStep(TYPE_MAP_INJ_WRMP);
 mptms.m_dead_map = mp_view->mp_ButtonsPanel->GetPtMovStep(TYPE_MAP_INJ_DEAD);
 mptms.m_idlr_map = mp_view->mp_ButtonsPanel->GetPtMovStep(TYPE_MAP_INJ_IDLR);
 mptms.m_idlc_map = mp_view->mp_ButtonsPanel->GetPtMovStep(TYPE_MAP_INJ_IDLC);
 mptms.m_aetps_map = mp_view->mp_ButtonsPanel->GetPtMovStep(TYPE_MAP_INJ_AETPS);
 mptms.m_aerpm_map = mp_view->mp_ButtonsPanel->GetPtMovStep(TYPE_MAP_INJ_AERPM);
 mptms.m_aftstr_map = mp_view->mp_ButtonsPanel->GetPtMovStep(TYPE_MAP_INJ_AFTSTR);
 mptms.m_it_map = mp_view->mp_ButtonsPanel->GetPtMovStep(TYPE_MAP_INJ_IT);
 mptms.m_itrpm_map = mp_view->mp_ButtonsPanel->GetPtMovStep(TYPE_MAP_INJ_ITRPM);
 mptms.m_rigid_map = mp_view->mp_ButtonsPanel->GetPtMovStep(TYPE_MAP_INJ_RIGID);
 mptms.m_egocrv_map = mp_view->mp_ButtonsPanel->GetPtMovStep(TYPE_MAP_INJ_EGOCRV);
 mptms.m_iacc_map = mp_view->mp_ButtonsPanel->GetPtMovStep(TYPE_MAP_INJ_IACC);
 mptms.m_iaccw_map = mp_view->mp_ButtonsPanel->GetPtMovStep(TYPE_MAP_INJ_IACCW);
 mptms.m_iatclt_map = mp_view->mp_ButtonsPanel->GetPtMovStep(TYPE_MAP_INJ_IATCLT);
 mptms.m_tpsswt_map = mp_view->mp_ButtonsPanel->GetPtMovStep(TYPE_MAP_INJ_TPSSWT);
 mptms.m_gtsc_map = mp_view->mp_ButtonsPanel->GetPtMovStep(TYPE_MAP_INJ_GTSC);
 mptms.m_gpsc_map = mp_view->mp_ButtonsPanel->GetPtMovStep(TYPE_MAP_INJ_GPSC);
 mptms.m_atsc_map = mp_view->mp_ButtonsPanel->GetPtMovStep(TYPE_MAP_INJ_ATSC);
 mptms.m_pwm1_map = mp_view->mp_ButtonsPanel->GetPtMovStep(TYPE_MAP_PWM1);
 mptms.m_pwm2_map = mp_view->mp_ButtonsPanel->GetPtMovStep(TYPE_MAP_PWM2);
 mptms.m_iacmat_map = mp_view->mp_ButtonsPanel->GetPtMovStep(TYPE_MAP_INJ_IACMAT);

 mp_settings->SetMapPtMovStep(mptms);

 if (m_OnChangeSettings)
  m_OnChangeSettings();
}

void CPMTablesController::setOnChangeSettings(EventHandler OnCB)
{
 m_OnChangeSettings = OnCB;
}
