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

/** \file PMTablesController.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include <vector>
#include "common/ObjectTimer.h"
#include "common/fastdelegate.h"
#include "PMControllerBase.h"
#include "MapWndScrPos.h"

class CCommunicationManager;
class CStatusBarManager;
class CTablesDeskDlg;
struct SECU3FWMapsItem;
class ISettingsData;
namespace SECU3IO {struct EditTabPar; struct FunSetPar;}

class CPMTablesController : public CPMControllerBase<CTablesDeskDlg>, public MapWndScrPos
{
  typedef CPMControllerBase<VIEW> Super;
  typedef fastdelegate::FastDelegate0<> EventHandler;
 public:
  CPMTablesController(VIEW* ip_view, CCommunicationManager* ip_comm, CStatusBarManager* ip_sbar, ISettingsData* ip_settings);
  virtual ~CPMTablesController();

  //beginning of controller's work
  void OnActivate(void);

  //end of controller's work
  void OnDeactivate(void);

  virtual void Enable(bool state);

  virtual void StartDataCollection(void);

  void OnClose();

  void OnCloseNotify();

  void OnSaveSettings();

  //„тение всех необходимых данных после коннекта (конечный автомат)
  //возвращает true когда все данные прочитаны  
  virtual bool CollectData(const BYTE i_descriptor, const void* i_packet_data);

  void InvalidateCache(void);
  bool IsValidCache(void);

  void SetFunctionsNames(const std::vector<_TSTRING>& i_fwnames, const std::vector<_TSTRING>& i_eenames); //names of read-only and read/write tables

  void ApplyFWOptions(DWORD opt);

  void OnFunSetChanged(const SECU3IO::FunSetPar* data);

  void setOnChangeSettings(EventHandler OnCB);

  void OnSettingsChanged(void);

 private:
  //Events from view
  void OnMapChanged(int i_mapType);
  virtual void OnCloseMapWnd(HWND i_hwnd, int i_mapType);
  virtual void OnOpenMapWnd(HWND i_hwnd, int i_mapType);
  void OnSaveButton(void);
  void OnChangeTablesSetName(void);
  void OnLoadTablesFrom(int index);
  void OnSaveTablesTo(int index);
  void OnImportFromS3F(void);
  void OnExportToS3F(void);
  void OnChangeSettings(void);

 private:
  //helpful methods
  bool _CompareViewMap(int i_mapType, size_t size) const;
  float* _GetMap(int i_mapType, bool i_original, SECU3FWMapsItem* p_maps = NULL);
  void _MoveMapToChart(int i_mapType, bool i_original, SECU3FWMapsItem* p_maps = NULL);
  void _MoveMapsToCharts(bool i_original, SECU3FWMapsItem* p_maps = NULL);
  void _ClearAcquisitionFlags(void);
  void _ResetModification(void);
  void _SetTablesSetName(const _TSTRING name);

  void _UpdateCache(const SECU3IO::EditTabPar* data); 
  bool _IsCacheUpToDate(void);
  bool _IsModificationMade(void) const;
  void _SynchronizeMap(int i_mapType);

  //tables's data collected
  void OnDataCollected(void);
  //from timer
  void OnTableDeskChangesTimer(void);

 private:
  EventHandler m_OnChangeSettings;

  CCommunicationManager* mp_comm;
  CStatusBarManager* mp_sbar;
  ISettingsData* mp_settings;

  //кеш таблиц
  SECU3FWMapsItem* m_maps;  //current
  SECU3FWMapsItem* m_omaps; //original
  //флаги описывающие состо€ние сбора информации по таблицам
  SECU3FWMapsItem* m_maps_flags;

  bool m_valid_cache; //признак валидного кеша (данные в менеджере соответствуют данным в SECU-3)
  /*CObjectTimer<CPMTablesController> m_td_changes_timer;*/

  float m_rpmGrid[32];
  float m_cltGrid[32];
  float m_lodGrid[32];
  float m_rpmGridI[32];
  float m_lodGridI[32];
  float m_lodGridT[32];
};
