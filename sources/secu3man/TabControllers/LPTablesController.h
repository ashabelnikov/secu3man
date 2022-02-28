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

/** \file LPTablesController.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include <memory>
#include <vector>
#include "common/unicodesupport.h"
#include "TablDesk/DynamicValues.h"
#include "MapWndScrPos.h"

class CLPControlPanelDlg;
class ISettingsData;
class CGridModeEditorIgnDlg;
class CGridModeEditorInjDlg;
class CFirmwareTabController;
class CEEPROMTabController;
namespace SECU3IO {struct SensorDat;}

class CLPTablesController : public MapWndScrPos
{
 public:
  CLPTablesController(CLPControlPanelDlg* i_view, ISettingsData* ip_settings);
  virtual ~CLPTablesController();

  virtual void OnActivate(void);
  virtual void OnDeactivate(void);
  void DisplayCurrentRecord(SECU3IO::SensorDat* sd);
  void OnSettingsChanged(int action);

 private:
  void _OnSelectMapSet(void);
  void _OnGmeIgnButton(void);
  void _OnGmeInjButton(void);
  void _OnOpenMapWnd(HWND i_hwnd, int i_mapType);
  void _OnMapChanged(int i_mapType);
  void _OnCloseMapWnd(HWND i_hwnd, int i_mapType);
  bool _IsAllowed(void);
  void _MakeChartsChildren(bool children);
  void _OnChangeSettingsGME(void);
  bool _GetSplitAngMode(void);

  //Ignition maps:
  float startMap[16];
  float idleMap[16];
  float workMap[256];
  float tempMap[16];
  float tempIMap[16];
  //axis grids:
  float m_rpm_grid_values[16];
  float m_clt_grid_values[16];
  float m_load_grid_values[16];
  //Fuel injection maps:
  float veMap[256];
  float ve2Map[256];
  float afrMap[256];
  float itMap[256];
  float idlcMap[16];
  float idlrMap[16];
  float itrpmMap[16];
  float rigidMap[8];
  float iaccMap[8+2];
  float iaccwMap[16+2];
  float aftstrMap[16];
  float wrmpMap[16];
  float aetpsMap[8*2]; //map+bins
  float aerpmMap[4*2]; //map+bins
  float crnkMap[16];
  float deadMap[32];
  float egocrvMap[16+2];
  float iatcltMap[8+2];
  float tpsswtMap[16];
  float tpszonMap[16];
  float atscMap[16];
  float gtscMap[16];
  float gpscMap[17+2];
  float pwm1Map[256];
  float pwm2Map[256];
  float iacmatMap[16];
  float cylmultMap[8];
  float cyladdMap[8];

 private:
  CFirmwareTabController* mp_fwdcntr;
  CEEPROMTabController* mp_eedcntr;
  CLPControlPanelDlg*  mp_view;
  ISettingsData* mp_settings;
  std::auto_ptr<CGridModeEditorIgnDlg> mp_gridModeEditorIgnDlg;
  std::auto_ptr<CGridModeEditorInjDlg> mp_gridModeEditorInjDlg;

  int m_current_funset_index;
  bool m_children_charts;
  bool m_toggleMapWnd;
  HWND m_openedChart;
  bool m_firmware_opened;
  bool m_eeprom_opened;
  std::vector<float> m_ve2_map_load_slots;
};
