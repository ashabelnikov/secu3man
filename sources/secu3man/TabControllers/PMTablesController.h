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

#pragma once

#include <vector>
#include "common/ObjectTimer.h"
#include "PMControllerBase.h"

class CCommunicationManager;
class CStatusBarManager;
class CTablesDeskDlg;
struct SECU3FWMapsItem;
namespace SECU3IO {struct EditTabPar;}

class CPMTablesController : public CPMControllerBase<CTablesDeskDlg>
{
  typedef CPMControllerBase<VIEW> Super;
 public:
  CPMTablesController(VIEW* ip_view, CCommunicationManager* ip_comm, CStatusBarManager* ip_sbar);
  virtual ~CPMTablesController();

  //начало работы контроллера
  void OnActivate(void);

  //конец работы контроллера
  void OnDeactivate(void);

  virtual void Enable(bool state);

  virtual void StartDataCollection(void);

  //Чтение всех необходимых данных после коннекта (конечный автомат)
  //возвращает true когда все данные прочитаны  
  virtual bool CollectData(const BYTE i_descriptor, const void* i_packet_data);

  void InvalidateCache(void);
  bool IsValidCache(void);

 public:
  //Events from view
  void OnMapChanged(int fuel_type, int i_mapType);
  void OnCloseMapWnd(HWND i_hwnd, int i_mapType);
  void OnOpenMapWnd(HWND i_hwnd, int i_mapType);
  void OnTabActivate(void);
  void OnSaveButton(void);
  void OnChangeTablesSetName(int fuel_type);

 private:
  void _ResetModification(int fuel_type);
  void _MoveMapToChart(int fuel_type, int i_mapType, bool i_original);
  void _MoveMapsToCharts(int fuel_type, bool i_original);
  bool _CompareViewMap(int i_mapType, size_t size) const;
  bool _IsModificationMade(void) const;
  float* _GetMap(int fuel_type, int i_mapType, bool i_original);
  void _SynchronizeMap(int fuel_type, int i_mapType);

  void OnDataCollected(void);
  void OnTableDeskChangesTimer(void);

  CCommunicationManager* mp_comm;
  CStatusBarManager* mp_sbar;

  //кеш таблиц 
  std::vector<SECU3FWMapsItem*> m_maps;  //current
  std::vector<SECU3FWMapsItem*> m_omaps; //original

  void _UpdateCache(const SECU3IO::EditTabPar* data); 

  bool _IsCacheUpToDate(void);

  void _ClearAcquisitionFlags(void);
  std::vector<SECU3FWMapsItem*> m_maps_flags;

  bool m_valid_cache;
  CObjectTimer<CPMTablesController> m_td_changes_timer;
};
