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
#include "TablDesk/MapIds.h"

using namespace fastdelegate;
using namespace SECU3IO;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//------------------------------ Helpful functions ----------------------------------
bool CPMTablesController::_CompareViewMap(int i_mapType, size_t size) const
{
 return std::equal(mp_view->GetMap(i_mapType, false),
                   mp_view->GetMap(i_mapType, false) + size,
                   mp_view->GetMap(i_mapType, true));
}

float* CPMTablesController::_GetMap(int fuel_type, int i_mapType, bool i_original)
{
 switch(i_mapType)
 {
  case TYPE_MAP_DA_START:
   return i_original ? m_omaps[fuel_type]->f_str : m_maps[fuel_type]->f_str;
  case TYPE_MAP_DA_IDLE:
   return i_original ? m_omaps[fuel_type]->f_idl : m_maps[fuel_type]->f_idl;
  case TYPE_MAP_DA_WORK:  
   return i_original ? m_omaps[fuel_type]->f_wrk : m_maps[fuel_type]->f_wrk;
  case TYPE_MAP_DA_TEMP_CORR:
   return i_original ? m_omaps[fuel_type]->f_tmp : m_maps[fuel_type]->f_tmp;
 }
 return NULL; //undefined type of map
}

namespace {
size_t _GetMapSize(int i_mapType)
{
 switch(i_mapType)
 {
  case TYPE_MAP_DA_START:
   return F_STR_POINTS;
  case TYPE_MAP_DA_IDLE:
   return F_IDL_POINTS;
  case TYPE_MAP_DA_WORK:  
   return F_WRK_POINTS_L * F_WRK_POINTS_F;
  case TYPE_MAP_DA_TEMP_CORR:
   return F_TMP_POINTS;
 }
 return 0; //undefined type of map
}
}

void CPMTablesController::_MoveMapToChart(int fuel_type, int i_mapType, bool i_original)
{
 std::copy(_GetMap(fuel_type, i_mapType, i_original),
  _GetMap(fuel_type, i_mapType, i_original) + _GetMapSize(i_mapType),
  mp_view->GetMap(i_mapType, i_original));
}

void CPMTablesController::_MoveMapsToCharts(int fuel_type, bool i_original)
{
 _MoveMapToChart(fuel_type, TYPE_MAP_DA_START, i_original);
 _MoveMapToChart(fuel_type, TYPE_MAP_DA_IDLE, i_original);
 _MoveMapToChart(fuel_type, TYPE_MAP_DA_WORK, i_original);
 _MoveMapToChart(fuel_type, TYPE_MAP_DA_TEMP_CORR, i_original);
}

void CPMTablesController::_ClearAcquisitionFlags(void)
{
 for(size_t i = 0; i < m_maps_flags.size(); ++i)
 {
  std::fill(m_maps_flags[i]->f_str, m_maps_flags[i]->f_str + F_STR_POINTS, .0f);
  std::fill(m_maps_flags[i]->f_idl, m_maps_flags[i]->f_idl + F_IDL_POINTS, .0f);
  std::fill(m_maps_flags[i]->f_wrk, m_maps_flags[i]->f_wrk + (F_WRK_POINTS_L * F_WRK_POINTS_F), .0f);
  std::fill(m_maps_flags[i]->f_tmp, m_maps_flags[i]->f_tmp + F_TMP_POINTS, .0f);
  m_maps_flags[i]->name = _T("");
 }
}

void CPMTablesController::_ResetModification(int fuel_type)
{
 ASSERT(m_maps.size() == m_omaps.size());
 for(size_t i = 0; i < m_maps.size(); ++i)
  (*m_omaps[i]) = (*m_maps[i]);
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

 //карты (текущие)
 m_maps.push_back(new SECU3FWMapsItem);
 m_maps.push_back(new SECU3FWMapsItem);
 //карты (оригинальные)
 m_omaps.push_back(new SECU3FWMapsItem);
 m_omaps.push_back(new SECU3FWMapsItem);
 //флаги сбора информации
 m_maps_flags.push_back(new SECU3FWMapsItem);
 m_maps_flags.push_back(new SECU3FWMapsItem);
}

CPMTablesController::~CPMTablesController()
{
 size_t i;
 for(i = 0; i < m_maps.size(); ++i)
  delete m_maps[i];
 for(i = 0; i < m_omaps.size(); ++i)
  delete m_omaps[i];
 for(i = 0; i < m_maps_flags.size(); ++i)
  delete m_maps_flags[i];

 m_td_changes_timer.KillTimer();
}

//----------------------------------------------------------------
void CPMTablesController::OnActivate(void)
{
 _ClearAcquisitionFlags();
 mp_view->SetReadOnlyTablesSetName(true);

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
  case 0:
   mp_sbar->SetInformationText(MLL::LoadString(IDS_PM_READING_TABLES));
   if (i_descriptor != EDITAB_PAR)
    mp_comm->m_pControlApp->ChangeContext(EDITAB_PAR);   
   else
   {//Очищаем флаги, сохраняем принятые данные и идем на продолжение приема
    _ClearAcquisitionFlags();    
    const EditTabPar* data = (const EditTabPar*)i_packet_data;
    _UpdateCache(data);
    m_operation_state = 1;
   }
   break;

  case 1:
   {
    if (i_descriptor != EDITAB_PAR)
    {
     m_operation_state = 0;
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

//----------------------------------------------------------------
namespace {
void UpdateMap(float* map, float* flag, const EditTabPar* data)
{
 int address = data->address;
 for(int i = 0; i < data->data_size; ++i)
 {
  flag[address] = 1.f; //set flag
  map [address] = data->table_data[i]; //save data
  ++address;
 }
}
}
void CPMTablesController::_UpdateCache(const EditTabPar* data)
{
 size_t tsi = data->tab_set_index;
 ASSERT(tsi == ETTS_GASOLINE_SET || tsi == ETTS_GAS_SET); //paranoic check
 switch(data->tab_id)
 {
  case ETMT_STRT_MAP: //start map
   UpdateMap(m_maps[tsi]->f_str, m_maps_flags[tsi]->f_str, data);
   break;
  case ETMT_IDLE_MAP: //idle map
   UpdateMap(m_maps[tsi]->f_idl, m_maps_flags[tsi]->f_idl, data);
   break;
  case ETMT_WORK_MAP: //work map
   UpdateMap(m_maps[tsi]->f_wrk, m_maps_flags[tsi]->f_wrk, data);
   break;
  case ETMT_TEMP_MAP: //temp. corr. map
   UpdateMap(m_maps[tsi]->f_tmp, m_maps_flags[tsi]->f_tmp, data);
   break;
  case ETMT_NAME_STR: //name of set
   m_maps[tsi]->name = data->name_data;
   m_maps_flags[tsi]->name = _T("1");
   break;
  default:
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
 size_t tsi;
 for(tsi = 0; tsi < m_maps_flags.size(); ++tsi)
 {
  if (!_FindZero(m_maps_flags[tsi]->f_str, F_STR_POINTS))
   return false;
  if (!_FindZero(m_maps_flags[tsi]->f_idl, F_IDL_POINTS))
   return false;
  if (!_FindZero(m_maps_flags[tsi]->f_wrk, F_WRK_POINTS_L * F_WRK_POINTS_F))
   return false;
  if (!_FindZero(m_maps_flags[tsi]->f_tmp, F_TMP_POINTS))
   return false;
  if (m_maps_flags[tsi]->name == _T(""))
   return false;
 }
 return true;
}

bool CPMTablesController::_IsModificationMade(void) const
{
 if (false==_CompareViewMap(TYPE_MAP_DA_START, _GetMapSize(TYPE_MAP_DA_START)))
  return true;
 if (false==_CompareViewMap(TYPE_MAP_DA_IDLE, _GetMapSize(TYPE_MAP_DA_IDLE)))
  return true;
 if (false==_CompareViewMap(TYPE_MAP_DA_WORK, _GetMapSize(TYPE_MAP_DA_WORK)))
  return true;
 if (false==_CompareViewMap(TYPE_MAP_DA_TEMP_CORR, _GetMapSize(TYPE_MAP_DA_TEMP_CORR)))
  return true;
 return false; //no modifications
}

void CPMTablesController::_SynchronizeMap(int fuel_type, int i_mapType)
{
 float* pMap = mp_view->GetMap(i_mapType, false); //<--current
 size_t mapSize = _GetMapSize(i_mapType);

 size_t index;
 EditTabPar packet;
 packet.data_size = 0;
 int state = 0;
 for(size_t a = 0; a < mapSize; ++a)
 {
  switch(state)
  {
   case 0:
    if (pMap[a] != _GetMap(fuel_type, i_mapType, false)[a])
    {
     index = 0;
     packet.tab_set_index = fuel_type;
     packet.tab_id = i_mapType;
     packet.address = a;
     packet.table_data[index] = pMap[a];
     packet.data_size = 1;
     state = 1;
    }
    break;
   case 1:
    packet.table_data[index] = pMap[a];
    if (pMap[a] != _GetMap(fuel_type, i_mapType, false)[a])
     packet.data_size = index + 1;
    break;
  }

  ++index;
  if ((16==index || a==(mapSize-1)) && packet.data_size > 0)
  {
   mp_sbar->SetInformationText(MLL::LoadString(IDS_PM_WRITING_TABLES));
   state = 0;
   mp_comm->m_pControlApp->SendPacket(EDITAB_PAR, &packet);
   //transfer copied values from view into chache (save midification)
   for(size_t i = 0; i < packet.data_size; ++i)
    _GetMap(fuel_type, i_mapType, false)[packet.address + i] = pMap[packet.address + i];
  }
 }
}

//----------------------------------------------------------------
void CPMTablesController::OnMapChanged(int fuel_type, int i_mapType)
{
 mp_view->SetModificationFlag(!_CompareViewMap(i_mapType, _GetMapSize(i_mapType)));

 //send to SECU-3 changed fragment(s)
 _SynchronizeMap(fuel_type, i_mapType);

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
 _MoveMapsToCharts(mp_view->GetCurSel(), false);
 _MoveMapsToCharts(mp_view->GetCurSel(), true);
 mp_view->UpdateOpenedCharts();
 mp_view->SetTablesSetName(m_maps[mp_view->GetCurSel()]->name);
 //update modification flag if any of maps changed
 mp_view->SetModificationFlag(_IsModificationMade());
}

void CPMTablesController::OnSaveButton(void)
{
 //todo
}

void CPMTablesController::OnChangeTablesSetName(int fuel_type)
{
 //not used
}

//----------------------------------------------------------------
void CPMTablesController::OnDataCollected(void)
{
 _ResetModification(mp_view->GetCurSel()); //original=current
 _MoveMapsToCharts(mp_view->GetCurSel(), false);
 _MoveMapsToCharts(mp_view->GetCurSel(), true);
 mp_view->UpdateOpenedCharts();
 mp_view->SetTablesSetName(m_maps[mp_view->GetCurSel()]->name);
}

void CPMTablesController::OnTableDeskChangesTimer(void)
{
 //todo
}
