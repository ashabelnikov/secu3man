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

/** \file PMSeptabsController.h
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
class CSepTablesDeskDlg;
struct FWMapsDataHolder;
class ISettingsData;
namespace SECU3IO {struct EditTabPar; struct FunSetPar;}

class CPMSeptabsController : public CPMControllerBase<CSepTablesDeskDlg>, public MapWndScrPos
{
  typedef CPMControllerBase<VIEW> Super;
  typedef fastdelegate::FastDelegate0<> EventHandler;
 public:
  CPMSeptabsController(VIEW* ip_view, CCommunicationManager* ip_comm, CStatusBarManager* ip_sbar, ISettingsData* ip_settings, EventHandler OnSepReqDataColl);
  virtual ~CPMSeptabsController();

  //beginning of controller's work
  void OnActivate(void);

  //end of controller's work
  void OnDeactivate(void);

  virtual void Enable(bool state);

  virtual void StartDataCollection(void);

  void OnClose();

  void OnCloseNotify();

  void OnSaveSettings();

  //Чтение всех необходимых данных после коннекта (конечный автомат)
  //возвращает true когда все данные прочитаны  
  virtual bool CollectData(const BYTE i_descriptor, const void* i_packet_data);

  void InvalidateCache(void);
  bool IsValidCache(void);

  void ApplyFWOptions(DWORD opt);

  void setOnChangeSettings(EventHandler OnCB);

  void OnSettingsChanged(void);

  void OnFunSetChanged(const SECU3IO::FunSetPar* data);
  
 private:
  //Events from view

  void OnMapChanged(int i_mapType);
  virtual void OnCloseMapWnd(HWND i_hwnd, int i_mapType);
  virtual void OnOpenMapWnd(HWND i_hwnd, int i_mapType);
  void OnSaveButton(void);
  void OnChangeSettings(void);

 private:
  //helpful methods
  bool _CompareViewMap(int i_mapType, size_t size) const;

  float* _GetMap(int i_mapType, bool i_original, FWMapsDataHolder* p_maps = NULL);

  void _MoveMapToChart(int i_mapType, bool i_original, FWMapsDataHolder* p_maps = NULL);
  void _MoveMapsToCharts(bool i_original, FWMapsDataHolder* p_maps = NULL);

  void _ClearAcquisitionFlags(void);

  void _ResetModification(int mapId = -1);

  void _UpdateCache(const SECU3IO::EditTabPar* data); 

  bool _IsModificationMade(int i_mapId) const;
  void _SynchronizeMap(int i_mapType);

  //tables' data collected
  void OnDataCollected(int tabId = -1);


  bool _isCacheUpToDate(int id);

 private:
  EventHandler m_OnChangeSettings;
  EventHandler m_OnSepReqDataColl;
  int m_collectId;
  bool m_valid_cache;

  CCommunicationManager* mp_comm;
  CStatusBarManager* mp_sbar;
  ISettingsData* mp_settings;

  FWMapsDataHolder* m_maps;  //current
  FWMapsDataHolder* m_omaps; //original
  FWMapsDataHolder* m_maps_flags;

  float m_rpmGrid[32];
  float m_cltGrid[32];
  float m_lodGrid[32];
};
