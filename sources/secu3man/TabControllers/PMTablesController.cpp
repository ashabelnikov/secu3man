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
#include "PMTablesController.h"

#include <algorithm>
#include "Application/CommunicationManager.h"
#include "common/fastdelegate.h"
#include "io-core/FirmwareMapsDataHolder.h"
#include "io-core/SECU3IO.h"
#include "io-core/ufcodes.h"
#include "MainFrame/StatusBarManager.h"
#include "ParamDesk/Tables/TablesDeskDlg.h"
#include "TablDesk/ButtonsPanel.h"
#include "TablDesk/MapIds.h"

using namespace fastdelegate;
using namespace SECU3IO;

//------------------------------ Helpful functions ----------------------------------
bool CPMTablesController::_CompareViewMap(int i_mapType, size_t size) const
{
 return std::equal(mp_view->GetMap(i_mapType, false),
                   mp_view->GetMap(i_mapType, false) + size,
                   mp_view->GetMap(i_mapType, true));
}

float* CPMTablesController::_GetMap(int i_mapType, bool i_original)
{
 switch(i_mapType)
 {//ignition maps
  case TYPE_MAP_DA_START:
   return i_original ? m_omaps->f_str : m_maps->f_str;
  case TYPE_MAP_DA_IDLE:
   return i_original ? m_omaps->f_idl : m_maps->f_idl;
  case TYPE_MAP_DA_WORK:
   return i_original ? m_omaps->f_wrk : m_maps->f_wrk;
  case TYPE_MAP_DA_TEMP_CORR:
   return i_original ? m_omaps->f_tmp : m_maps->f_tmp;
  //fuel injection
  case TYPE_MAP_INJ_VE:
   return i_original ? m_omaps->inj_ve : m_maps->inj_ve;
  case TYPE_MAP_INJ_AFR:
   return i_original ? m_omaps->inj_afr : m_maps->inj_afr;
  case TYPE_MAP_INJ_CRNK:
   return i_original ? m_omaps->inj_cranking : m_maps->inj_cranking;
  case TYPE_MAP_INJ_WRMP:
   return i_original ? m_omaps->inj_warmup : m_maps->inj_warmup;
  case TYPE_MAP_INJ_DEAD:
   return i_original ? m_omaps->inj_dead_time : m_maps->inj_dead_time;
  case TYPE_MAP_INJ_IDLR:
   return i_original ? m_omaps->inj_iac_run_pos : m_maps->inj_iac_run_pos;
  case TYPE_MAP_INJ_IDLC:
   return i_original ? m_omaps->inj_iac_crank_pos : m_maps->inj_iac_crank_pos;
  case TYPE_MAP_INJ_AETPS:
   return i_original ? m_omaps->inj_ae_tps : m_maps->inj_ae_tps;
  case TYPE_MAP_INJ_AERPM:
   return i_original ? m_omaps->inj_ae_rpm : m_maps->inj_ae_rpm;
  case TYPE_MAP_INJ_AFTSTR:
   return i_original ? m_omaps->inj_aftstr : m_maps->inj_aftstr;
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
  //fuel injection maps
  case TYPE_MAP_INJ_VE:
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
 }
 ASSERT(0);
 return 0; //undefined type of map
}
}

void CPMTablesController::_MoveMapToChart(int i_mapType, bool i_original)
{
 std::copy(_GetMap(i_mapType, i_original),
  _GetMap(i_mapType, i_original) + _GetMapSize(i_mapType),
  mp_view->GetMap(i_mapType, i_original));
}

void CPMTablesController::_MoveMapsToCharts(bool i_original)
{//ignition maps
 _MoveMapToChart(TYPE_MAP_DA_START, i_original);
 _MoveMapToChart(TYPE_MAP_DA_IDLE, i_original);
 _MoveMapToChart(TYPE_MAP_DA_WORK, i_original);
 _MoveMapToChart(TYPE_MAP_DA_TEMP_CORR, i_original);
 //fuel injection maps
 _MoveMapToChart(TYPE_MAP_INJ_VE, i_original);
 _MoveMapToChart(TYPE_MAP_INJ_AFR, i_original);
 _MoveMapToChart(TYPE_MAP_INJ_CRNK, i_original);
 _MoveMapToChart(TYPE_MAP_INJ_WRMP, i_original);
 _MoveMapToChart(TYPE_MAP_INJ_DEAD, i_original);
 _MoveMapToChart(TYPE_MAP_INJ_IDLR, i_original);
 _MoveMapToChart(TYPE_MAP_INJ_IDLC, i_original);
 _MoveMapToChart(TYPE_MAP_INJ_AETPS, i_original);
 _MoveMapToChart(TYPE_MAP_INJ_AERPM, i_original);
 _MoveMapToChart(TYPE_MAP_INJ_AFTSTR, i_original);
}

void CPMTablesController::_ClearAcquisitionFlags(void)
{
 m_maps_flags->name = _T("");
 //ignition maps
 std::fill(m_maps_flags->f_str, m_maps_flags->f_str + F_STR_POINTS, .0f);
 std::fill(m_maps_flags->f_idl, m_maps_flags->f_idl + F_IDL_POINTS, .0f);
 std::fill(m_maps_flags->f_wrk, m_maps_flags->f_wrk + (F_WRK_POINTS_L * F_WRK_POINTS_F), .0f);
 std::fill(m_maps_flags->f_tmp, m_maps_flags->f_tmp + F_TMP_POINTS, .0f);
 //fuel injection maps
 std::fill(m_maps_flags->inj_ve, m_maps_flags->inj_ve + (INJ_VE_POINTS_L * INJ_VE_POINTS_F), .0f);
 std::fill(m_maps_flags->inj_afr, m_maps_flags->inj_afr + (INJ_VE_POINTS_L * INJ_VE_POINTS_F), .0f);
 std::fill(m_maps_flags->inj_cranking, m_maps_flags->inj_cranking + INJ_CRANKING_LOOKUP_TABLE_SIZE, .0f);
 std::fill(m_maps_flags->inj_warmup, m_maps_flags->inj_warmup + INJ_WARMUP_LOOKUP_TABLE_SIZE, .0f);
 std::fill(m_maps_flags->inj_dead_time, m_maps_flags->inj_dead_time + INJ_DT_LOOKUP_TABLE_SIZE, .0f);
 std::fill(m_maps_flags->inj_iac_run_pos, m_maps_flags->inj_iac_run_pos + INJ_IAC_POS_TABLE_SIZE, .0f);
 std::fill(m_maps_flags->inj_iac_crank_pos, m_maps_flags->inj_iac_crank_pos + INJ_IAC_POS_TABLE_SIZE, .0f);
 std::fill(m_maps_flags->inj_ae_tps, m_maps_flags->inj_ae_tps + (INJ_AE_TPS_LOOKUP_TABLE_SIZE * 2), .0f);
 std::fill(m_maps_flags->inj_ae_rpm, m_maps_flags->inj_ae_rpm + (INJ_AE_RPM_LOOKUP_TABLE_SIZE * 2), .0f);
 std::fill(m_maps_flags->inj_aftstr, m_maps_flags->inj_aftstr + INJ_AFTSTR_LOOKUP_TABLE_SIZE, .0f);
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
CPMTablesController::CPMTablesController(VIEW* ip_view, CCommunicationManager* ip_comm, CStatusBarManager* ip_sbar)
: Super(ip_view)
, mp_comm(ip_comm)
, mp_sbar(ip_sbar)
, m_valid_cache(false)
{
 //устанавливаем обработчики событий от view
 mp_view->setOnMapChanged(MakeDelegate(this, &CPMTablesController::OnMapChanged));
 mp_view->setOnCloseMapWnd(MakeDelegate(this, &CPMTablesController::OnCloseMapWnd));
 mp_view->setOnOpenMapWnd(MakeDelegate(this, &CPMTablesController::OnOpenMapWnd));
 mp_view->setOnTabActivate(MakeDelegate(this, &CPMTablesController::OnTabActivate));
 mp_view->setOnSaveButton(MakeDelegate(this, &CPMTablesController::OnSaveButton));
 mp_view->setOnChangeTablesSetName(MakeDelegate(this, &CPMTablesController::OnChangeTablesSetName));
 mp_view->setOnLoadTablesFrom(MakeDelegate(this, &CPMTablesController::OnLoadTablesFrom));
 mp_view->setOnSaveTablesTo(MakeDelegate(this, &CPMTablesController::OnSaveTablesTo));

 //карты (текущие)
 m_maps = new SECU3FWMapsItem;
 //карты (оригинальные)
 m_omaps = new SECU3FWMapsItem;
 //флаги сбора информации
 m_maps_flags = new SECU3FWMapsItem;
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
  case 0: //Read out RPM grid
   mp_sbar->SetInformationText(MLL::LoadString(IDS_PM_READING_RPMRGD));
   if (i_descriptor != RPMGRD_PAR)
    mp_comm->m_pControlApp->ChangeContext(RPMGRD_PAR);
   else
   {//save RPM grid
    const SepTabPar* data = (const SepTabPar*)i_packet_data;
    memcpy(m_rpmGrid, data->table_data, F_RPM_SLOTS*sizeof(float)); //save RPM grid
    m_operation_state = 1;
   }
   break;

  case 1:
   mp_sbar->SetInformationText(MLL::LoadString(IDS_PM_READING_TABLES));
   if (i_descriptor != EDITAB_PAR)
    mp_comm->m_pControlApp->ChangeContext(EDITAB_PAR);
   else
   {//Очищаем флаги, сохраняем принятые данные и идем на продолжение приема
    _ClearAcquisitionFlags();
    const EditTabPar* data = (const EditTabPar*)i_packet_data;
    _UpdateCache(data);
    m_operation_state = 2;
   }
   break;

  case 2:
   {
    if (i_descriptor != EDITAB_PAR)
    {
     m_operation_state = 1;
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
 mp_view->mp_ButtonsPanel->EnableFuelInjection((opt & (1 << COPT_FUEL_INJECT)) > 0);
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
  case ETMT_TEMP_MAP: //temp. corr. map
   UpdateMap(m_maps->f_tmp, m_maps_flags->f_tmp, data);
   break;
  //fuel injection maps
  case ETMT_VE_MAP: //ve map
   UpdateMap(m_maps->inj_ve, m_maps_flags->inj_ve, data);
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
  //ignition maps
  if (!_FindZero(m_maps_flags->f_str, F_STR_POINTS))
   return false;
  if (!_FindZero(m_maps_flags->f_idl, F_IDL_POINTS))
   return false;
  if (!_FindZero(m_maps_flags->f_wrk, F_WRK_POINTS_L * F_WRK_POINTS_F))
   return false;
  if (!_FindZero(m_maps_flags->f_tmp, F_TMP_POINTS))
   return false;
  //fuel injection maps
  if (!_FindZero(m_maps_flags->inj_ve, INJ_VE_POINTS_L * INJ_VE_POINTS_F))
   return false;
  if (!_FindZero(m_maps_flags->inj_afr, INJ_VE_POINTS_L * INJ_VE_POINTS_F))
   return false;
  if (!_FindZero(m_maps_flags->inj_cranking, INJ_CRANKING_LOOKUP_TABLE_SIZE))
   return false;
  if (!_FindZero(m_maps_flags->inj_warmup, INJ_WARMUP_LOOKUP_TABLE_SIZE))
   return false;
  if (!_FindZero(m_maps_flags->inj_dead_time, INJ_DT_LOOKUP_TABLE_SIZE))
   return false;
  if (!_FindZero(m_maps_flags->inj_iac_run_pos, INJ_IAC_POS_TABLE_SIZE))
   return false;
  if (!_FindZero(m_maps_flags->inj_iac_crank_pos, INJ_IAC_POS_TABLE_SIZE))
   return false;
  if (!_FindZero(m_maps_flags->inj_ae_tps, INJ_AE_TPS_LOOKUP_TABLE_SIZE*2))
   return false;
  if (!_FindZero(m_maps_flags->inj_ae_rpm, INJ_AE_RPM_LOOKUP_TABLE_SIZE*2))
   return false;
  if (!_FindZero(m_maps_flags->inj_aftstr, INJ_AFTSTR_LOOKUP_TABLE_SIZE))
   return false;
 return true;
}

bool CPMTablesController::_IsModificationMade(void) const
{
 if (m_maps->name != m_omaps->name)
  return true;
 //ignition maps
 if (false==_CompareViewMap(TYPE_MAP_DA_START, _GetMapSize(TYPE_MAP_DA_START)))
  return true;
 if (false==_CompareViewMap(TYPE_MAP_DA_IDLE, _GetMapSize(TYPE_MAP_DA_IDLE)))
  return true;
 if (false==_CompareViewMap(TYPE_MAP_DA_WORK, _GetMapSize(TYPE_MAP_DA_WORK)))
  return true;
 if (false==_CompareViewMap(TYPE_MAP_DA_TEMP_CORR, _GetMapSize(TYPE_MAP_DA_TEMP_CORR)))
  return true;
 //fuel injection maps
 if (false==_CompareViewMap(TYPE_MAP_INJ_VE, _GetMapSize(TYPE_MAP_INJ_VE)))
  return true;
 if (false==_CompareViewMap(TYPE_MAP_INJ_AFR, _GetMapSize(TYPE_MAP_INJ_AFR)))
  return true;
 if (false==_CompareViewMap(TYPE_MAP_INJ_CRNK, _GetMapSize(TYPE_MAP_INJ_CRNK)))
  return true;
 if (false==_CompareViewMap(TYPE_MAP_INJ_WRMP, _GetMapSize(TYPE_MAP_INJ_WRMP)))
  return true;
 if (false==_CompareViewMap(TYPE_MAP_INJ_DEAD, _GetMapSize(TYPE_MAP_INJ_DEAD)))
  return true;
 if (false==_CompareViewMap(TYPE_MAP_INJ_IDLR, _GetMapSize(TYPE_MAP_INJ_IDLR)))
  return true;
 if (false==_CompareViewMap(TYPE_MAP_INJ_IDLC, _GetMapSize(TYPE_MAP_INJ_IDLC)))
  return true;
 if (false==_CompareViewMap(TYPE_MAP_INJ_AETPS, _GetMapSize(TYPE_MAP_INJ_AETPS)))
  return true;
 if (false==_CompareViewMap(TYPE_MAP_INJ_AERPM, _GetMapSize(TYPE_MAP_INJ_AERPM)))
  return true;
 if (false==_CompareViewMap(TYPE_MAP_INJ_AFTSTR, _GetMapSize(TYPE_MAP_INJ_AFTSTR)))
  return true;

 return false; //no modifications
}

void CPMTablesController::_SynchronizeMap(int i_mapType)
{
 float* pMap = mp_view->GetMap(i_mapType, false); //<--current
 size_t mapSize = _GetMapSize(i_mapType); //map size in items (not bytes)

 size_t pieceSize = 16; //for all maps exept cranking PW and inj. dead time
 if (i_mapType == TYPE_MAP_INJ_CRNK || i_mapType == TYPE_MAP_INJ_DEAD)
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
   mp_sbar->SetInformationText(MLL::LoadString(IDS_PM_WRITING_TABLES));
   state = 0;
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
 //todo
}

void CPMTablesController::OnOpenMapWnd(HWND i_hwnd, int i_mapType)
{
 //todo
}

void CPMTablesController::OnTabActivate(void)
{
 _MoveMapsToCharts(false);
 _MoveMapsToCharts(true);
 mp_view->UpdateOpenedCharts();
 _SetTablesSetName(m_maps->name);
 //update modification flag if any of maps changed
 mp_view->SetModificationFlag(_IsModificationMade());
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

//----------------------------------------------------------------
void CPMTablesController::OnDataCollected(void)
{
 _ResetModification(); //original=current
 _MoveMapsToCharts(false);
 _MoveMapsToCharts(true);
 //Set RPM grid read out from SECU-3
 mp_view->SetRPMGrid(m_rpmGrid);
 mp_view->UpdateOpenedCharts();
 _SetTablesSetName(m_maps->name);
}

void CPMTablesController::OnTableDeskChangesTimer(void)
{
 //todo
}
