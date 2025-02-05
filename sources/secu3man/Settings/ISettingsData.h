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

/** \file ISettingsData.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include "io-core/PlatformParamHolder.h"  //only for EECUPlatform enum
#include "common/SettingsTypes.h"

//Interface which should be used to access settings' data from usual places of program
class ISettingsData
{
 public:
  virtual CString GetINIFileFullName(void) const = 0;

  virtual const _TSTRING& GetPortName(void) const = 0;
  virtual void SetPortName(const _TSTRING& name) = 0;
  virtual DWORD GetBaudRateApplication(void) const = 0;
  virtual DWORD GetBaudRateBootloader(void) const = 0;
  virtual const _TSTRING& GetLogFilesFolder(void) const = 0;
  virtual bool  GetUseAppFolder(void) const = 0;
  virtual bool  GetAlwaysWriteLog(void) const = 0;
  virtual char  GetCSVSepSymbol(void) const = 0;
  virtual char  GetMapCSVSepSymbol(void) const = 0;
  virtual int   GetMIDeskUpdatePeriod(void) const = 0;
  virtual CString GetAppDirectory(void) const = 0;
  virtual bool GetCOMPortBother(void) const = 0;
  virtual bool GetUseHotKeys(void) const = 0;
  virtual bool GetShowWelcome(void) const = 0;
  virtual bool GetAllowVisualTheme(void) const = 0;
  virtual bool GetAutoDiagEnter(void) const = 0;
  virtual bool GetSaveWarning(void) const = 0;
  virtual bool GetAutoCERead(void) const = 0;
  virtual bool GetChildCharts(void) const = 0;
  virtual void SetChildCharts(bool child) = 0;
  virtual bool GetPortAutoReopen(void) const = 0;
  virtual bool GetToggleMapWnd(void) const = 0;
  virtual void SetToggleMapWnd(bool toggle) = 0;
  virtual bool GetEmbedMapWnd(void) const = 0;
  virtual void SetEmbedMapWnd(bool toggle) = 0;
  virtual int GetParamMonVert(void) const = 0;
  virtual void SetParamMonVert(int pos) = 0;
  virtual int GetLogPlayerVert(void) const = 0;
  virtual void SetLogPlayerVert(int pos) = 0;
  virtual bool GetExistingPorts(void) const = 0;
  virtual int GetToolTipTime(void) const = 0;
  virtual int GetIniEditorSyntax(void) const = 0;
  virtual int GetBldrEEPROMBlocks(void) const = 0;
  virtual float GetFuelDensity1(void) const = 0;
  virtual float GetFuelDensity2(void) const = 0;
  virtual bool GetLogBinaryFmt(void) const = 0;
  virtual bool GetClassic2DKeys(void) const = 0;

  //Fixtures's customization settings
  virtual ESpeedUnit GetSpeedUnit(void) const = 0;
  virtual void GetMetAverage(MetAverage& ma) = 0;

  virtual int GetTitleFontSize(void) const = 0;
  virtual int GetValueFontSize(void) const = 0;
  virtual int GetPaneFontSize(void) const = 0;
  virtual int GetLabelFontSize(void) const = 0;

  virtual void SetTitleFontSize(int size) = 0;
  virtual void SetValueFontSize(int size) = 0;
  virtual void SetPaneFontSize(int size) = 0;
  virtual void SetLabelFontSize(int size) = 0;

  virtual bool GetMetersDragNDrop(void) const = 0;
  virtual void SetMetersDragNDrop(bool enable) = 0;

  virtual bool GetIndicatorsDragNDrop(void) const = 0;
  virtual void SetIndicatorsDragNDrop(bool enable) = 0;

  virtual int GetFFFConst(void) const = 0;
  virtual bool GetShowGraphsCursor(void) = 0;
  virtual int GetGraphShtPixels(void) = 0;
  virtual int GetGraphShowValue(void) = 0;
  virtual int GetGraphValueHeight(void) = 0;
  
  //windows' positions
  virtual void SetWndSettings(const WndSettings& i_wndSettings) = 0;
  virtual void GetWndSettings(WndSettings& o_wndSettings) const = 0;
  virtual void SetWndSettings1(const WndSettings& i_wndSettings) = 0; //online
  virtual void GetWndSettings1(WndSettings& o_wndSettings) const = 0; //online

  //Interface language and SECU-3 platform type
  virtual EInterLang GetInterfaceLanguage(void) const = 0;
  virtual EECUPlatform GetECUPlatformType(void) const = 0;
  //Debug features
  virtual bool GetUseDVFeatures(void) const = 0;
  virtual int GetDVDeskUpdatePeriod(void) const = 0;
  virtual bool GetShowToolTips(void) const = 0;
  virtual bool GetShowExFixtures(void) const = 0;
  virtual void SetShowExFixtures(bool i_show) = 0;
  virtual bool GetShowSpeedAndDist(void) const = 0;
  virtual void SetShowSpeedAndDist(bool i_show) = 0;

  //Indicators
  virtual void GetIndicatorsConfig(IndicatorsCfg& o_cfg) const = 0;
  virtual void SetIndicatorsConfig(const IndicatorsCfg& i_cfg) = 0;
  //Meters
  virtual void GetMetersConfig(MetersCfg* o_cfg) const = 0;
  virtual void SetMetersConfig(const MetersCfg* i_cfg) = 0;

  virtual void SetWndSize(const WndSize& i_wndSize) = 0;
  virtual void GetWndSize(WndSize& o_wndSize) const = 0;
  virtual void SetWndSize1(const WndSize& i_wndSize) = 0;
  virtual void GetWndSize1(WndSize& o_wndSize) const = 0;

  virtual void SetWndState(const WndState& i_wndState) = 0;
  virtual void GetWndState(WndState& o_wndState) const = 0;

  virtual void SetLamDelMap(float* map, float* rb, float* lb) = 0;
  virtual void GetLamDelMap(float* map, float* rb, float* lb) = 0;
  virtual float GetAFRError(void) = 0;
  virtual void SetBlockedCells(const bool* blList) = 0;
  virtual void GetBlockedCells(bool* blList) = 0;
  virtual int GetStatSize(void) = 0;
  virtual int GetAutoBlockThrd(void) = 0;
  virtual bool GetGrowingMode(void) = 0;
  virtual float GetMinAFR(void) = 0;
  virtual float GetMaxAFR(void) = 0;
  virtual int GetMinDistThrd(void) = 0;
  virtual float GetMinTPS(void) = 0;
  virtual float GetMaxTPS(void) = 0;
  virtual float GetCLTThrd(void) = 0;
  virtual float GetTunSoftness(void) = 0;

  virtual int GetGradSaturation(void) = 0;
  virtual int GetGradBrightness(void) = 0;
  virtual int GetBoldFont(void) = 0;
  virtual int GetITEdMode(void) const = 0;
  virtual void SetITEdMode(int mode) = 0;
  virtual int GetActiveVEMap(void) const = 0;
  virtual void SetActiveVEMap(int id) = 0;

  //Inj.driver
  virtual bool GetInjDrvTabActive(void) const = 0;
  virtual COLORREF GetVoltLineColor(void) const = 0;
  virtual float GetPeakOnPtMovStep(void) const = 0;
  virtual float GetPeakDutyPtMovStep(void) const = 0;
  virtual float GetHoldDutyPtMovStep(void) const = 0;
  virtual float GetPeakFullPtMovStep(void) const = 0;
  virtual float GetPthPausePtMovStep(void) const = 0;
  virtual float GetPWMulPtMovStep(void) const = 0;
  virtual float GetPWAddPtMovStep(void) const = 0;
  virtual bool GetShowGraphLabels(void) const = 0;

  virtual void SetMapPtMovStep(const MapPtMovStep& i_ptMovStep) = 0;
  virtual void GetMapPtMovStep(MapPtMovStep& o_ptMovStep) const = 0;

  virtual void SetLogFileFields(const LogFileFields& i_flds) = 0;
  virtual void GetLogFileFields(LogFileFields& o_flds) const = 0;

  virtual bool GetWriteLogFields(void) const = 0;
  virtual void SetWriteLogFields(bool value) = 0;

  virtual void GetFunctionality(Functionality& o_fnc) const = 0;

  virtual bool GetSpotMarkers(void) const = 0;
  virtual float GetSpotMarkersSize(void) const = 0;

  virtual bool GetCreateWindows(void) const = 0;
  virtual bool GetDbgVarsToFile(void) const = 0;

  virtual bool GetUseMarkers(void) const = 0;

  virtual void GetDbgVarsConfig(DbgVarsCfg* o_cfg) const = 0;
  virtual void SetDbgVarsConfig(const DbgVarsCfg* i_cfg) = 0;
};
