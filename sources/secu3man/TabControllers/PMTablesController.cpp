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

 //карты
 m_maps.push_back(new SECU3FWMapsItem);
 m_maps.push_back(new SECU3FWMapsItem);
 //флаги
 m_maps_flags.push_back(new SECU3FWMapsItem);
 m_maps_flags.push_back(new SECU3FWMapsItem);
}

CPMTablesController::~CPMTablesController()
{
 size_t i;
 for(i = 0; i < m_maps.size(); ++i)
  delete m_maps[i];
 for(i = 0; i < m_maps_flags.size(); ++i)
  delete m_maps_flags[i];
}

void CPMTablesController::OnActivate(void)
{
 _ClearAcquisitionFlags();
 mp_view->SetReadOnlyTablesSetName(true);
}

void CPMTablesController::OnDeactivate(void)
{
 //empty
}

void CPMTablesController::Enable(bool state)
{
 mp_view->Enable(state);
}

void CPMTablesController::StartDataCollection(void)
{
 Super::StartDataCollection();
}

void CPMTablesController::_MoveMapsToCharts(int fuel_type, bool i_original)
{
 int ft = fuel_type;
 std::copy(m_maps[ft]->f_str, m_maps[ft]->f_str + F_STR_POINTS, mp_view->GetMap(TYPE_MAP_DA_START, i_original));
 std::copy(m_maps[ft]->f_idl, m_maps[ft]->f_idl + F_IDL_POINTS, mp_view->GetMap(TYPE_MAP_DA_IDLE, i_original));
 std::copy(m_maps[ft]->f_wrk, m_maps[ft]->f_wrk + (F_WRK_POINTS_L * F_WRK_POINTS_F), mp_view->GetMap(TYPE_MAP_DA_WORK, i_original));
 std::copy(m_maps[ft]->f_tmp, m_maps[ft]->f_tmp + F_TMP_POINTS, mp_view->GetMap(TYPE_MAP_DA_TEMP_CORR, i_original));
}

void CPMTablesController::OnDataCollected(void)
{
 _MoveMapsToCharts(mp_view->GetCurSel(), false);
 _MoveMapsToCharts(mp_view->GetCurSel(), true);
 mp_view->UpdateOpenedCharts();
 mp_view->SetTablesSetName(m_maps[mp_view->GetCurSel()]->name);
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

namespace {
void UpdateTable(float* map, float* flag, const EditTabPar* data)
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
   UpdateTable(m_maps[tsi]->f_str, m_maps_flags[tsi]->f_str, data);
   break;
  case ETMT_IDLE_MAP: //idle map
   UpdateTable(m_maps[tsi]->f_idl, m_maps_flags[tsi]->f_idl, data);
   break;
  case ETMT_WORK_MAP: //work map
   UpdateTable(m_maps[tsi]->f_wrk, m_maps_flags[tsi]->f_wrk, data);
   break;
  case ETMT_TEMP_MAP: //temp. corr. map
   UpdateTable(m_maps[tsi]->f_tmp, m_maps_flags[tsi]->f_tmp, data);
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
bool FindZero(float* array, size_t size)
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
  if (!FindZero(m_maps_flags[tsi]->f_str, F_STR_POINTS))
   return false;
  if (!FindZero(m_maps_flags[tsi]->f_idl, F_IDL_POINTS))
   return false;
  if (!FindZero(m_maps_flags[tsi]->f_wrk, F_WRK_POINTS_L * F_WRK_POINTS_F))
   return false;
  if (!FindZero(m_maps_flags[tsi]->f_tmp, F_TMP_POINTS))
   return false;
  if (m_maps_flags[tsi]->name == _T(""))
   return false;
 }
 return true;
}

//----------------------------------------------------------------
void CPMTablesController::OnMapChanged(int fuel_type, int i_mapType)
{
 //todo
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
}

void CPMTablesController::OnSaveButton(void)
{
 //todo
}

void CPMTablesController::OnChangeTablesSetName(int fuel_type)
{
 //todo
}
