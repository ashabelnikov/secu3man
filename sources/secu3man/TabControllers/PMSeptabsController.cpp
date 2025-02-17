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
#include "io-core/MapIds.h"
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
 return std::equal(mp_view->mp_SeptabsPanel->GetMap(i_mapType, false),
                   mp_view->mp_SeptabsPanel->GetMap(i_mapType, false) + size,
                   mp_view->mp_SeptabsPanel->GetMap(i_mapType, true));
}

float* CPMSeptabsController::_GetMap(int i_mapType, bool i_original, FWMapsDataHolder* p_maps /*= NULL*/)
{
 if (!p_maps)
  p_maps = i_original ? m_omaps : m_maps;
 
 return p_maps->GetMap(i_mapType);
}

void CPMSeptabsController::_MoveMapToChart(int i_mapType, bool i_original, FWMapsDataHolder* p_maps /*= NULL*/)
{
 std::copy(_GetMap(i_mapType, i_original, p_maps),
  _GetMap(i_mapType, i_original, p_maps) + FWMapsDataHolder::GetMapSize(i_mapType),
  mp_view->mp_SeptabsPanel->GetMap(i_mapType, i_original));
}

void CPMSeptabsController::_MoveMapsToCharts(bool i_original, FWMapsDataHolder* p_maps /*= NULL*/)
{
 for(int id = ETMT_SEP_START; id <= ETMT_SEP_END; ++id)
 {
  if (_isCacheUpToDate(id))
   _MoveMapToChart(id, i_original, p_maps);
 }
}

void CPMSeptabsController::_ClearAcquisitionFlags(void)
{
 //See TablDesk/MapIds.h for more information
 for(int id = ETMT_SEP_START; id <= ETMT_SEP_END; ++id)
  std::fill(_GetMap(id, false, m_maps_flags), _GetMap(id, false, m_maps_flags) + FWMapsDataHolder::GetMapSize(id), .0f);
}

void CPMSeptabsController::_ResetModification(int mapId /*= -1*/)
{
 if (mapId== -1)
 { //all maps
  (*m_omaps) = (*m_maps);
 }
 else
 { //certain map
  std::copy(_GetMap(mapId, false), _GetMap(mapId, false) + FWMapsDataHolder::GetMapSize(mapId), _GetMap(mapId, true));
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
 mp_view->mp_SeptabsPanel->SetClassic2DKeys(mp_settings->GetClassic2DKeys());

 MapPtMovStep mptms;
 mp_settings->GetMapPtMovStep(mptms);
 for(int i = ETMT_SEP_START; i <= ETMT_SEP_END; ++i)
  mp_view->mp_SeptabsPanel->SetPtMovStep(i, mptms.m_value[i]);
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
   case 0: //Read out FUNSET_PAR
    mp_sbar->SetInformationText(MLL::LoadString(IDS_PM_READING_PARAMS));
    if (i_descriptor != FUNSET_PAR)
     mp_comm->m_pControlApp->ChangeContext(FUNSET_PAR);
    else
    {//save read parameters
     const FunSetPar* data = (const FunSetPar*)i_packet_data;
     mp_view->mp_SeptabsPanel->SetLoadAxisCfg(data->map_lower_pressure, data->map_upper_pressure, data->load_src_cfg, data->use_load_grid);
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

     mp_view->SetRPMGrid(m_rpmGrid);  //Set RPM grid read out from SECU-3  
     mp_view->SetCLTGrid(m_cltGrid);  //Set CLT grid read out from SECU-3  
     mp_view->SetLoadGrid(m_lodGrid); //Set load grid read out from SECU-3
     m_valid_cache = true;
     _ClearAcquisitionFlags();

     //check for opened tables
     bool opened = false;
     for(int i = ETMT_SEP_START; i <= ETMT_SEP_END; ++i)
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
      for(int i = ETMT_SEP_START; i <= ETMT_SEP_END; ++i)
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
     for(int i = ETMT_SEP_START; i <= ETMT_SEP_END; ++i)
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
   case 0:
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
    //update cache and perform checking
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

 for(int i = ETMT_SEP_START; i <= ETMT_SEP_END; ++i)
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
 if (data->tab_id < ETMT_SEP_START || data->tab_id > ETMT_SEP_END)
 {
  ASSERT(0);
  return;
 }
 UpdateMap(m_maps->GetMap(data->tab_id), m_maps_flags->GetMap(data->tab_id), data);
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
 return _FindZero(_GetMap(id, false, m_maps_flags), FWMapsDataHolder::GetMapSize(id));
}

bool CPMSeptabsController::_IsModificationMade(int i_mapId) const
{
 if (false==_CompareViewMap(i_mapId, FWMapsDataHolder::GetMapSize(i_mapId)))
  return true;

 return false; //no modifications
}

void CPMSeptabsController::_SynchronizeMap(int i_mapType)
{
 float* pMap = mp_view->mp_SeptabsPanel->GetMap(i_mapType, false); //<--current
 size_t mapSize = FWMapsDataHolder::GetMapSize(i_mapType); //map size in items (not bytes)

 size_t pieceSize = 16; //for all maps exept dwell
 if (i_mapType == ETMT_DWELLCNTRL || i_mapType == ETMT_CTS_CURVE || i_mapType == ETMT_ATS_CURVE || i_mapType == ETMT_TMP2_CURVE ||
     i_mapType == ETMT_GRTS_CURVE || i_mapType == ETMT_FTS_CURVE || i_mapType == ETMT_FTLS_CURVE || i_mapType == ETMT_EGTS_CURVE ||
     i_mapType == ETMT_OPS_CURVE || i_mapType == ETMT_MAF_CURVE || i_mapType == ETMT_BAROCORR || i_mapType == ETMT_AFTSTR_STRK0 ||
     i_mapType == ETMT_AFTSTR_STRK1 || i_mapType == ETMT_PWMIAC_UCOEF || i_mapType == ETMT_GRVDELAY || i_mapType == ETMT_MANINJPWC ||
     i_mapType == ETMT_FTLSCOR || i_mapType == ETMT_FUELDENS_CORR || i_mapType == ETMT_XTAU_XFACC || i_mapType == ETMT_XTAU_XFDEC ||
     i_mapType == ETMT_XTAU_TFACC || i_mapType == ETMT_XTAU_TFDEC || i_mapType == ETMT_INJNONLINP || i_mapType == ETMT_INJNONLING ||
     i_mapType == ETMT_EGO_DELAY || i_mapType == ETMT_ETC_SPRPREL || i_mapType == ETMT_ETC_ACCEERR)
  pieceSize = 8;
 else if (i_mapType == ETMT_KNOCK_ZONE || i_mapType == ETMT_LAMBDA_ZONE)
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
   statusStr.Format(MLL::LoadString(IDS_PM_WRITING_TABLES), i_mapType - ETMT_SEP_START); 
   mp_sbar->SetInformationText(statusStr);
   //store values of padding cells from cache
   packet.beginPadding = (packet.address > 0) ? _GetMap(i_mapType, false)[packet.address-1] : 0;
   packet.endPadding = (packet.address + packet.data_size) < FWMapsDataHolder::GetMapSize(i_mapType) ? _GetMap(i_mapType, false)[(packet.address + packet.data_size)] : 0;

   size_t packet_data_size = packet.data_size;
   size_t packet_address = packet.address;
   
   if (i_mapType == ETMT_KNOCK_ZONE || i_mapType == ETMT_LAMBDA_ZONE)
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
  for(int id = ETMT_SEP_START; id <= ETMT_SEP_END; ++id)
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
 for(int i = ETMT_SEP_START; i <= ETMT_SEP_END; ++i)
  OnCloseMapWnd(mp_view->mp_SeptabsPanel->GetMapWindow(i), i);
}

void CPMSeptabsController::OnCloseNotify()
{
 mp_view->CloseAllCharts();
}

void CPMSeptabsController::OnSaveSettings()
{
 //save positions of windows of opened charts
 for(int i = ETMT_SEP_START; i <= ETMT_SEP_END; ++i)
  OnCloseMapWnd(mp_view->mp_SeptabsPanel->GetMapWindow(i), i);
}

void CPMSeptabsController::OnChangeSettings(void)
{
 MapPtMovStep mptms;
 mp_settings->GetMapPtMovStep(mptms);
 for(int i = ETMT_SEP_START; i <= ETMT_SEP_END; ++i)
  mptms.m_value[i] = mp_view->mp_SeptabsPanel->GetPtMovStep(i);
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
 mp_view->mp_SeptabsPanel->SetClassic2DKeys(mp_settings->GetClassic2DKeys());
}

void CPMSeptabsController::OnFunSetChanged(const SECU3IO::FunSetPar* data)
{
 mp_view->mp_SeptabsPanel->SetLoadAxisCfg(data->map_lower_pressure, data->map_upper_pressure, data->load_src_cfg, data->use_load_grid);
}
