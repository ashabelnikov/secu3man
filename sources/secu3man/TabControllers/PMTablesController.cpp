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
#include "io-core/MapIds.h"
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
 return std::equal(mp_view->mp_ButtonsPanel->GetMap(i_mapType, false),
                   mp_view->mp_ButtonsPanel->GetMap(i_mapType, false) + size,
                   mp_view->mp_ButtonsPanel->GetMap(i_mapType, true));
}

float* CPMTablesController::_GetMap(int i_mapType, bool i_original, SECU3FWMapsItem* p_maps /*= NULL*/)
{
 if (!p_maps)
  p_maps = i_original ? m_omaps : m_maps;

 return p_maps->GetMap(i_mapType);
}

void CPMTablesController::_MoveMapToChart(int i_mapType, bool i_original, SECU3FWMapsItem* p_maps /*= NULL*/)
{
 std::copy(_GetMap(i_mapType, i_original, p_maps),
  _GetMap(i_mapType, i_original, p_maps) + SECU3FWMapsItem::GetMapSize(i_mapType),
  mp_view->mp_ButtonsPanel->GetMap(i_mapType, i_original));
}

void CPMTablesController::_MoveMapsToCharts(bool i_original, SECU3FWMapsItem* p_maps /*= NULL*/)
{//ignition maps
 for(int id = ETMT_SET_START; id <= ETMT_SET_END; ++id)
  _MoveMapToChart(id, i_original, p_maps);
}

void CPMTablesController::_ClearAcquisitionFlags(void)
{
 m_maps_flags->name = _T("");

 //See TablDesk/MapIds.h for more information
 for(int id = ETMT_SET_START; id <= ETMT_SET_END; ++id)
  std::fill(_GetMap(id, false, m_maps_flags), _GetMap(id, false, m_maps_flags) + SECU3FWMapsItem::GetMapSize(id), .0f);
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

 /*m_td_changes_timer.KillTimer();*/
}

//----------------------------------------------------------------
void CPMTablesController::OnActivate(void)
{
 _ClearAcquisitionFlags();
 mp_view->SetReadOnlyTablesSetName(false);

 mp_view->Enable(IsValidCache());

 //запускаем таймер по которому будет ограничиваться частота посылки данных в SECU-3
/* m_td_changes_timer.SetTimer(this, &CPMTablesController::OnTableDeskChangesTimer, 250);*/

 mp_view->mp_ButtonsPanel->SetITEdMode(mp_settings->GetITEdMode());
 mp_view->mp_ButtonsPanel->SetActiveVEMap(mp_settings->GetActiveVEMap());
 mp_view->mp_ButtonsPanel->SetCSVSepSymbol(mp_settings->GetMapCSVSepSymbol());
 mp_view->mp_ButtonsPanel->SetClassic2DKeys(mp_settings->GetClassic2DKeys());

 MapPtMovStep mptms;
 mp_settings->GetMapPtMovStep(mptms);
 for(int i = ETMT_SET_START; i <= ETMT_SET_END; ++i)
  mp_view->mp_ButtonsPanel->SetPtMovStep(i, mptms.m_value[i]);
}

void CPMTablesController::OnDeactivate(void)
{
 /*m_td_changes_timer.KillTimer();*/
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
  case 4: //Read out idling RPM grid
   mp_sbar->SetInformationText(MLL::LoadString(IDS_PM_READING_IRPMGRD));
   if (i_descriptor != IRPMGRD_PAR)
    mp_comm->m_pControlApp->ChangeContext(IRPMGRD_PAR);
   else
   {//save idling RPM grid
    const SepTabPar* data = (const SepTabPar*)i_packet_data;
    memcpy(m_rpmGridI, data->table_data, F_IRPM_SLOTS*sizeof(float)); //save idling RPM grid
    m_operation_state = 5;
   }
   break;
  case 5: //Read out idling load grid
   mp_sbar->SetInformationText(MLL::LoadString(IDS_PM_READING_ILODGRD));
   if (i_descriptor != ILODGRD_PAR)
    mp_comm->m_pControlApp->ChangeContext(ILODGRD_PAR);
   else
   {//save load grid
    const SepTabPar* data = (const SepTabPar*)i_packet_data;
    memcpy(m_lodGridI, data->table_data, F_ILOAD_SLOTS*sizeof(float)); //save idling load grid
    m_operation_state = 6;
   }
   break;
  case 6: //Read out load grid
   mp_sbar->SetInformationText(MLL::LoadString(IDS_PM_READING_TLODGRD));
   if (i_descriptor != TLODGRD_PAR)
    mp_comm->m_pControlApp->ChangeContext(TLODGRD_PAR);
   else
   {//save load grid
    const SepTabPar* data = (const SepTabPar*)i_packet_data;
    memcpy(m_lodGridT, data->table_data, F_TLOAD_SLOTS*sizeof(float)); //save VE2 load grid
    m_operation_state = 7;
   }
   break;
  case 7:
   {
   mp_sbar->SetInformationText(MLL::LoadString(IDS_PM_READING_TABLES));
   const EditTabPar* data = (const EditTabPar*)i_packet_data;
   if (i_descriptor != EDITAB_PAR || (data->tab_id < ETMT_SET_START || data->tab_id > ETMT_SET_END))
    mp_comm->m_pControlApp->ChangeContext(EDITAB_PAR, 255); //<-- want all tables
   else
   {//Очищаем флаги, сохраняем принятые данные и идем на продолжение приема
    _ClearAcquisitionFlags();
    const EditTabPar* data = (const EditTabPar*)i_packet_data;
    _UpdateCache(data);
    m_operation_state = 8;
   }
   }
   break;

  case 8:
   {
    if (i_descriptor != EDITAB_PAR)
    {
     m_operation_state = 7;
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
 if (data->tab_id == ETMT_NAME_STR)
 { 
  m_maps->name = data->name_data;
  m_maps_flags->name = _T("1");
 }
 else
 {
  if (data->tab_id < ETMT_SET_START || data->tab_id > ETMT_SET_END)
  {
   ASSERT(0);
   return;
  }

  UpdateMap(m_maps->GetMap(data->tab_id), m_maps_flags->GetMap(data->tab_id), data);
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
 for(int id = ETMT_SET_START; id <= ETMT_SET_END; ++id)
 {
  if (!_FindZero(_GetMap(id, false, m_maps_flags), SECU3FWMapsItem::GetMapSize(id)))
   return false;
 }

 return true;
}

bool CPMTablesController::_IsModificationMade(void) const
{
 if (m_maps->name != m_omaps->name)
  return true;

 //See TablDesk/MapIds.h for more information
 for(int id = ETMT_SET_START; id <= ETMT_SET_END; ++id)
 {
  if (false==_CompareViewMap(id, SECU3FWMapsItem::GetMapSize(id)))
   return true;
 }

 return false; //no modifications
}

void CPMTablesController::_SynchronizeMap(int i_mapType)
{
 float* pMap = mp_view->mp_ButtonsPanel->GetMap(i_mapType, false); //<--current
 size_t mapSize = SECU3FWMapsItem::GetMapSize(i_mapType); //map size in items (not bytes)

 size_t pieceSize = 16; //for all maps exept cranking PW and inj. dead time and rigidity function
 if (i_mapType == ETMT_INJ_CRNK || i_mapType == ETMT_INJ_DEAD || i_mapType == ETMT_INJ_RIGID || i_mapType == ETMT_INJ_EGOCRV || i_mapType == ETMT_INJ_IACC || i_mapType == ETMT_INJ_IATCLT || i_mapType == ETMT_INJ_IVE)
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
   //store values of padding cells from cache
   packet.beginPadding = (packet.address > 0) ? _GetMap(i_mapType, false)[packet.address-1] : 0;
   packet.endPadding = (packet.address + packet.data_size) < SECU3FWMapsItem::GetMapSize(i_mapType) ? _GetMap(i_mapType, false)[(packet.address + packet.data_size)] : 0;
   //send packet!
   mp_comm->m_pControlApp->SendPacket(EDITAB_PAR, &packet);
   Sleep(20);
   //transfer copied values from view into cache (save modification)
   for(size_t i = 0; i < packet.data_size; ++i)
    _GetMap(i_mapType, false)[packet.address + i] = pMap[packet.address + i];
   state = 0;   
   packet.data_size = 0;
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
 if (i_mapType == ETMT_GME_INJ_WND)
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
 std::copy(m_rpmGridI, m_rpmGridI + F_IRPM_SLOTS, data.irpm_slots);
 std::copy(m_lodGridI, m_lodGridI + F_ILOAD_SLOTS, data.iload_slots);
 std::copy(m_lodGridT, m_lodGridT + F_TLOAD_SLOTS, data.tload_slots);

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
   mp_view->SetIRPMGrid(m_rpmGridI);       //RPM grid for idling VE
   mp_view->SetILoadGrid(m_lodGridI);      //load grid for idling VE
   mp_view->SetTLoadGrid(m_lodGridT);      //VE2 load grid
  _SetTablesSetName(data.maps[0].name);    //name
  
  //Send updated set name to SECU-3
  OnChangeTablesSetName();

  mp_view->TransformValues(); //transform values in some maps before they will be rendered for user

  //send updated data to SECU-3
  //See TablDesk/MapIds.h for more information
  for(int id = ETMT_SET_START; id <= ETMT_SET_END; ++id)
   _SynchronizeMap(id);

  mp_sbar->SetInformationText(MLL::LoadString(IDS_PM_READY));

  //update stored map values
  *m_maps = data.maps[0];  //current
  *m_omaps = data.maps[0]; //original
  std::copy(data.rpm_slots, data.rpm_slots + F_RPM_SLOTS, m_rpmGrid);
  std::copy(data.clt_slots, data.clt_slots + F_TMP_SLOTS, m_cltGrid);
  std::copy(data.load_slots, data.load_slots + F_LOAD_SLOTS, m_lodGrid);
  std::copy(data.irpm_slots, data.irpm_slots + F_IRPM_SLOTS, m_rpmGridI);
  std::copy(data.iload_slots, data.iload_slots + F_ILOAD_SLOTS, m_lodGridI);
  std::copy(data.tload_slots, data.tload_slots + F_TLOAD_SLOTS, m_lodGridT);

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
 std::copy(m_rpmGridI, m_rpmGridI + F_IRPM_SLOTS, s3f_io.GetDataLeft().irpm_slots);
 std::copy(m_lodGridI, m_lodGridI + F_ILOAD_SLOTS, s3f_io.GetDataLeft().iload_slots);
 std::copy(m_lodGridT, m_lodGridT + F_TLOAD_SLOTS, s3f_io.GetDataLeft().tload_slots);

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
 mp_view->SetModificationFlag(false);
 _ResetModification(); //original=current
 _MoveMapsToCharts(false);
 _MoveMapsToCharts(true);
 //Set RPM grid read out from SECU-3
 mp_view->SetRPMGrid(m_rpmGrid);
 //Set CLT grid read out from SECU-3
 mp_view->SetCLTGrid(m_cltGrid);
 //Set load grid read out from SECU-3
 mp_view->SetLoadGrid(m_lodGrid);
 //grids for idling VE
 mp_view->SetIRPMGrid(m_rpmGridI);
 mp_view->SetILoadGrid(m_lodGridI);
 //Set VE2 load grid read out from SECU-3
 mp_view->SetTLoadGrid(m_lodGridT);

 mp_view->TransformValues(); //transform values in some maps before they will be rendered for user
 mp_view->UpdateOpenedCharts();
 _SetTablesSetName(m_maps->name);
}

/*
void CPMTablesController::OnTableDeskChangesTimer(void)
{
 //todo
}*/

void CPMTablesController::OnClose()
{
 //save positions of windows of opened charts
 for(int i = ETMT_SET_START; i <= ETMT_SET_END; ++i)
  OnCloseMapWnd(mp_view->mp_ButtonsPanel->GetMapWindow(i), i);
 OnCloseMapWnd(mp_view->mp_ButtonsPanel->GetMapWindow(ETMT_GME_IGN_WND), ETMT_GME_IGN_WND);
 OnCloseMapWnd(mp_view->mp_ButtonsPanel->GetMapWindow(ETMT_GME_INJ_WND), ETMT_GME_INJ_WND);
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
 for(int i = ETMT_SET_START; i <= ETMT_SET_END; ++i)
  OnCloseMapWnd(mp_view->mp_ButtonsPanel->GetMapWindow(i), i);
 OnCloseMapWnd(mp_view->mp_ButtonsPanel->GetMapWindow(ETMT_GME_IGN_WND), ETMT_GME_IGN_WND);
 OnCloseMapWnd(mp_view->mp_ButtonsPanel->GetMapWindow(ETMT_GME_INJ_WND), ETMT_GME_INJ_WND);
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
 for(int i = ETMT_SET_START; i <= ETMT_SET_END; ++i)
  mptms.m_value[i] = mp_view->mp_ButtonsPanel->GetPtMovStep(i);
 mp_settings->SetMapPtMovStep(mptms);

 if (m_OnChangeSettings)
  m_OnChangeSettings();
}

void CPMTablesController::setOnChangeSettings(EventHandler OnCB)
{
 m_OnChangeSettings = OnCB;
}

void CPMTablesController::OnSettingsChanged(void)
{
 mp_view->mp_ButtonsPanel->SetCSVSepSymbol(mp_settings->GetMapCSVSepSymbol());
 mp_view->mp_ButtonsPanel->SetClassic2DKeys(mp_settings->GetClassic2DKeys());
}
