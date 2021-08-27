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

/** \file AppSettingsModel.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include <string>
#include <vector>
#include "common/unicodesupport.h"
#include "ISettingsData.h"
#include "IniFileIO.h"

class CAppSettingsController;

//хранит и сохран€ет/загружает данные
class CAppSettingsModel : public ISettingsData
{
  friend CAppSettingsController;

 public:
  std::vector<DWORD> m_AllowableBaudRates;
  std::vector<_TSTRING> m_AllowablePorts;

  //<UIstring, SepSymbol>
  std::vector<std::pair<_TSTRING, char> > m_AllowaleCSVSepSymbols;

  //<<UIString, INIString>, ID>
  std::vector<std::pair<std::pair<_TSTRING, _TSTRING>, int> > m_AllowableLanguages;

  //<<UIString, INIString>, ID>
  std::vector<std::pair<std::pair<_TSTRING, _TSTRING>, int> > m_AllowableWndStates;

  //<<UIString, INIString>, ID>
  std::vector<std::pair<std::pair<_TSTRING, _TSTRING>, int> > m_AllowablePlatforms;

  //<<UIString, INIString>, ID>
  std::vector<std::pair<std::pair<_TSTRING, _TSTRING>, int> > m_AllowableSpeedUnits;

  CAppSettingsModel();
  virtual ~CAppSettingsModel();

  //read settings from INI-file
  bool ReadSettings(void);

  //write settings into INI-file
  bool WriteSettings(void);

  //¬озвращает полное им€ INI-файла. INI-файл находитс€ в каталоге из которого
  //запущена программа.
  virtual CString GetINIFileFullName(void) const;

  //ISettingsData
  virtual const _TSTRING& GetPortName(void) const;
  virtual void SetPortName(const _TSTRING& name);
  virtual DWORD GetBaudRateApplication(void) const;
  virtual DWORD GetBaudRateBootloader(void) const;
  virtual const _TSTRING& GetLogFilesFolder(void) const;
  virtual bool  GetUseAppFolder(void) const;
  virtual bool  GetAlwaysWriteLog(void) const;
  virtual char  GetCSVSepSymbol(void) const;
  virtual int   GetMIDeskUpdatePeriod(void) const;
  //¬озвращает полный путь к каталогу из которого было запущено приложение
  //(каталог который был текущим на момент вызва конструктора этого класса)
  virtual CString GetAppDirectory(void) const;
  //To bother about opening of COM port
  virtual bool GetCOMPortBother(void) const;
  virtual bool GetUseHotKeys(void) const;
  virtual bool GetShowWelcome(void) const;
  virtual bool GetAllowVisualTheme(void) const;
  virtual bool GetAutoDiagEnter(void) const;
  virtual bool GetSaveWarning(void) const;
  virtual bool GetAutoCERead(void) const;
  virtual bool GetChildCharts(void) const;
  virtual void SetChildCharts(bool child);
  virtual bool GetPortAutoReopen(void) const;
  virtual bool GetToggleMapWnd(void) const;
  virtual int GetParamMonVert(void) const;
  virtual void SetParamMonVert(int pos);
  virtual int GetLogPlayerVert(void) const;
  virtual void SetLogPlayerVert(int pos);
  virtual bool GetExistingPorts(void) const;
  virtual void SetToggleMapWnd(bool toggle);
  virtual int GetIniEditorSyntax(void) const;
  virtual int GetBldrEEPROMBlocks(void) const;

  //windows positions
  virtual void SetWndSettings(const WndSettings& i_wndSettings);
  virtual void GetWndSettings(WndSettings& o_wndSettings) const;

  virtual void SetWndSettings1(const WndSettings& i_wndSettings);
  virtual void GetWndSettings1(WndSettings& o_wndSettings) const;

  //windows' size
  virtual void SetWndSize(const WndSize& i_wndSize);
  virtual void GetWndSize(WndSize& o_wndSize) const;
  virtual void SetWndSize1(const WndSize& i_wndSize);
  virtual void GetWndSize1(WndSize& o_wndSize) const;

  //windows' state
  virtual void SetWndState(const WndState& i_wndState);
  virtual void GetWndState(WndState& o_wndState) const;

  //€зык интерфейса и тип платформы
  virtual EInterLang GetInterfaceLanguage(void) const;
  virtual EECUPlatform GetECUPlatformType(void) const;

  //fixtures's customization
  virtual int GetTachometerMax(void) const;
  virtual int GetPressureMax(void) const;
  virtual int GetTemperatureMax(void) const;
  virtual int GetInjPWMax(void) const;
  virtual ESpeedUnit GetSpeedUnit(void) const;

  virtual int GetRPMAverage(void) const;
  virtual int GetVoltAverage(void) const;
  virtual int GetMAPAverage(void) const;
  virtual int GetAI1Average(void) const;
  virtual int GetTPSAverage(void) const;
  virtual int GetKnockAverage(void) const;
  virtual int GetIgnTimAverage(void) const;
  virtual int GetCLTAverage(void) const;
  virtual int GetAddI2Average(void) const;
  virtual int GetInjPWAverage(void) const;
  virtual int GetIATAverage(void) const;
  virtual int GetEGOCorrAverage(void) const;
  virtual int GetAirFlowAverage(void) const;
  virtual int GetVehicleSpeedAverage(void) const;
  virtual int GetTPSDotAverage(void) const;
  virtual int GetMAP2Average(void) const;
  virtual int GetMAPDAverage(void) const;
  virtual int GetTmp2Average(void) const;
  virtual int GetFuelConsumAverage(void) const;
  virtual int GetFuelConsumFAverage(void) const;
  virtual int GetKnockRetardAverage(void) const;
  virtual int GetSensAFRAverage(void) const;
  virtual int GetChokePosAverage(void) const;
  virtual int GetGDPosAverage(void) const;
  virtual int GetSynLoadAverage(void) const;
  virtual int GetInjTimBAverage(void) const;
  virtual int GetInjTimEAverage(void) const;
  virtual int GetGrtsAverage(void) const;
  virtual int GetFtlsAverage(void) const;
  virtual int GetEgtsAverage(void) const;
  virtual int GetOpsAverage(void) const;
  virtual int GetMAFAverage(void) const;

  virtual int GetTitleFontSize(void) const;
  virtual int GetValueFontSize(void) const;
  virtual int GetPaneFontSize(void) const;
  virtual int GetLabelFontSize(void) const;

  virtual void SetTitleFontSize(int size);
  virtual void SetValueFontSize(int size);
  virtual void SetPaneFontSize(int size);
  virtual void SetLabelFontSize(int size);
  
  virtual bool GetMetersDragNDrop(void) const;
  virtual void SetMetersDragNDrop(bool enable);

  virtual bool GetIndicatorsDragNDrop(void) const;
  virtual void SetIndicatorsDragNDrop(bool enable);

  virtual int GetFFFConst(void) const;

  virtual bool GetShowGraphsCursor(void);
  virtual int GetGraphShtPixels(void);
  virtual int GetGraphShowValue(void);

  //Debug features
  virtual bool GetUseDVFeatures(void) const;
  virtual int GetDVDeskUpdatePeriod(void) const;
  virtual bool GetShowToolTips(void) const;
  virtual bool GetShowExFixtures(void) const;
  virtual void SetShowExFixtures(bool i_show);
  virtual bool GetHexDataMode(void) const;
  virtual int GetNumPulsesPer1Km(void) const;
  ////////////////////////////////////////////////////
  virtual void GetIndicatorsConfig(IndicatorsCfg& o_cfg) const;
  virtual void SetIndicatorsConfig(const IndicatorsCfg& i_cfg);
  virtual void GetMetersConfig(MetersCfg* o_cfg) const;
  virtual void SetMetersConfig(const MetersCfg* i_cfg);

  virtual void SetLamDelMap(float* map, float* rb, float* lb);
  virtual void GetLamDelMap(float* map, float* rb, float* lb);
  virtual float GetAFRError(void);
  virtual void SetBlockedCells(const bool* blList);
  virtual void GetBlockedCells(bool* blList);
  virtual int GetStatSize(void);
  virtual int GetAutoBlockThrd(void);
  virtual bool GetGrowingMode(void);
  virtual float GetMinAFR(void);
  virtual float GetMaxAFR(void);
  virtual int GetMinDistThrd(void);
  virtual float GetMinTPS(void);
  virtual float GetMaxTPS(void);
  virtual float GetCLTThrd(void);
  virtual float GetTunSoftness(void);

  virtual int GetGradSaturation(void);
  virtual int GetGradBrightness(void);
  virtual int GetBoldFont(void);
  virtual int GetITEdMode(void) const;
  virtual void SetITEdMode(int mode);
  virtual int GetActiveVEMap(void) const;
  virtual void SetActiveVEMap(int id);

  virtual bool GetInjDrvTabActive(void) const;
  virtual COLORREF GetVoltLineColor(void) const;
  virtual float GetPeakOnPtMovStep(void) const;
  virtual float GetPeakDutyPtMovStep(void) const;
  virtual float GetHoldDutyPtMovStep(void) const;
  virtual float GetPeakFullPtMovStep(void) const;
  virtual float GetPthPausePtMovStep(void) const;
  virtual float GetPWMulPtMovStep(void) const;
  virtual float GetPWAddPtMovStep(void) const;
  virtual bool GetShowGraphLabels(void) const;

  virtual void SetMapPtMovStep(const MapPtMovStep& i_ptMovStep);
  virtual void GetMapPtMovStep(MapPtMovStep& o_ptMovStep) const;

  virtual void SetLogFileFields(const LogFileFields& i_flds);
  virtual void GetLogFileFields(LogFileFields& o_flds) const;

  virtual bool GetWriteLogFields(void) const;
  virtual void SetWriteLogFields(bool value);

  virtual void GetFunctionality(Functionality& o_fnc) const;

  virtual int GetToolTipTime(void) const;

  virtual bool GetSpotMarkers(void) const;
  virtual float GetSpotMarkersSize(void) const;

 private:
  bool _CheckAndCorrectLFCRs(void);

  //data which stored in the INI-file:
  //Section names
  const CString m_Name_Options_Section;
  const CString m_Name_WndSettings_Section;
  const CString m_Name_WndSettings_Section1;  //online
  const CString m_Name_WndSize_Section;
  const CString m_Name_WndSize_Section1;
  const CString m_Name_WndState_Section;
  const CString m_Name_Fixtures_Section;
  CString m_Name_Indicators_Section[2];
  CString m_Name_Meters_Section[2];
  const CString m_Name_IndColors_Section;
  const CString m_Name_AutoTune_Section;
  const CString m_Name_MapEditor_Section;
  const CString m_Name_Splitters_Section;
  const CString m_Name_InjDrv_Section;
  const CString m_Name_MapPtMovStep_Section;
  const CString m_Name_LogFileFields_Section;
  const CString m_Name_Functionality_Section;

  //Options
  OptField_t<_TSTRING> m_optPortName;
  OptField_t<DWORD> m_optBaudRateApplication;
  OptField_t<DWORD> m_optBaudRateBootloader;
  OptField_t<int> m_optInterfaceLang;
  OptField_t<int> m_optECUPlatformType;
  OptField_t<char> m_optCSVSepSymbol;
  OptField_t<_TSTRING> m_optLogFilesFolder;
  OptField_t<bool> m_optUseAppFolder;
  OptField_t<bool>  m_optAlwaysWriteLog;
  OptField_t<int> m_optMIDeskUpdatePeriod;
  OptField_t<int> m_optTachometerMax;
  OptField_t<int> m_optPressureMax;
  OptField_t<int> m_optTemperatureMax;
  OptField_t<int> m_optInjPWMax;
  OptField_t<int> m_optPulsesPer1Km;
  OptField_t<int> m_optSpeedUnit;
  OptField_t<bool> m_optCOMPortBother;
  OptField_t<bool> m_optUseHotKeys;
  OptField_t<bool> m_optShowWelcome;
  OptField_t<bool> m_optAllowVisualTheme;
  OptField_t<bool> m_optAutoDiagEnter;
  OptField_t<bool> m_optSaveWarning;
  OptField_t<bool> m_optAutoCERead;
  OptField_t<bool> m_optChildCharts;
  OptField_t<bool> m_optPortAutoReopen;  
  OptField_t<int> m_optToggleMapWnd;
  OptField_t<bool> m_optExistingPorts;
  OptField_t<int> m_optToolTipTime;
  OptField_t<bool> m_optIniEdSyntax;
  OptField_t<bool> m_optBldrEEPROMBlocks;

  //fixtures
  OptField_t<bool> m_optUseDVFeatures;
  OptField_t<int> m_optDVDeskUpdatePeriod;
  OptField_t<bool> m_optShowToolTips;
  OptField_t<bool> m_optShowExFixtures;
  OptField_t<bool> m_optHexDataMode;

  OptField_t<int>  m_optRPMAverage;
  OptField_t<int>  m_optVoltAverage;
  OptField_t<int>  m_optMAPAverage;
  OptField_t<int>  m_optAI1Average;
  OptField_t<int>  m_optTPSAverage;
  OptField_t<int>  m_optKnockAverage;
  OptField_t<int>  m_optIgnTimAverage;
  OptField_t<int>  m_optCLTAverage;
  OptField_t<int>  m_optAddI2Average;
  OptField_t<int>  m_optInjPWAverage;
  OptField_t<int>  m_optIATAverage;
  OptField_t<int>  m_optEGOCorrAverage;
  OptField_t<int>  m_optAirFlowAverage;
  OptField_t<int>  m_optVehicleSpeedAverage;
  OptField_t<int>  m_optTPSDotAverage;
  OptField_t<int>  m_optMAP2Average;
  OptField_t<int>  m_optMAPDAverage;
  OptField_t<int>  m_optTmp2Average;
  OptField_t<int>  m_optFuelConsumAverage;
  OptField_t<int>  m_optKnockRetardAverage;
  OptField_t<int>  m_optSensAFRAverage;
  OptField_t<int>  m_optChokePosAverage;
  OptField_t<int>  m_optGDPosAverage;
  OptField_t<int>  m_optSynLoadAverage;
  OptField_t<int>  m_optInjTimBAverage;
  OptField_t<int>  m_optInjTimEAverage;
  OptField_t<int>  m_optFuelConsumFAverage;
  OptField_t<int>  m_optGrtsAverage;
  OptField_t<int>  m_optFtlsAverage;
  OptField_t<int>  m_optEgtsAverage;
  OptField_t<int>  m_optOpsAverage;
  OptField_t<int>  m_optMAFAverage;

  OptField_t<int> m_optTitleFontSize;
  OptField_t<int> m_optValueFontSize;
  OptField_t<int> m_optPaneFontSize;
  OptField_t<int> m_optLabelFontSize;
  OptField_t<bool> m_optMetersDragNDrop;
  OptField_t<bool> m_optIndicatorsDragNDrop;
  OptField_t<int> m_optFFFConst;
  OptField_t<bool> m_optShowGraphCursor;
  OptField_t<int> m_optGraphShtPixels;
  OptField_t<int> m_optGraphShowValue;

  //windows' positions
  OptField_t<POINT> m_optMainFrmWnd;
  OptField_t<POINT> m_optStrtMapWnd;
  OptField_t<POINT> m_optIdleMapWnd;
  OptField_t<POINT> m_optWorkMapWnd;
  OptField_t<POINT> m_optTempMapWnd;
  OptField_t<POINT> m_optTempIdlMapWnd;
  OptField_t<POINT> m_optAttenMapWnd;
  OptField_t<POINT> m_optDwellCntrlMapWnd;
  OptField_t<POINT> m_optCTSCurveMapWnd;
  OptField_t<POINT> m_optGridMapIgnWnd;
  OptField_t<POINT> m_optGridMapInjWnd;
  OptField_t<POINT> m_optVEMapWnd;
  OptField_t<POINT> m_optVE2MapWnd;
  OptField_t<POINT> m_optAFRMapWnd;
  OptField_t<POINT> m_optCrnkMapWnd;
  OptField_t<POINT> m_optWrmpMapWnd;
  OptField_t<POINT> m_optDeadMapWnd;
  OptField_t<POINT> m_optIdlrMapWnd;
  OptField_t<POINT> m_optIdlcMapWnd;
  OptField_t<POINT> m_optATSCurvMapWnd;
  OptField_t<POINT> m_optATSCorrMapWnd;
  OptField_t<POINT> m_optAETPSMapWnd;
  OptField_t<POINT> m_optAERPMMapWnd;
  OptField_t<POINT> m_optAftstrMapWnd;
  OptField_t<POINT> m_optGasdoseMapWnd;
  OptField_t<POINT> m_optITMapWnd;
  OptField_t<POINT> m_optITRPMMapWnd;
  OptField_t<POINT> m_optRigidMapWnd;
  OptField_t<POINT> m_optEGOCrvMapWnd;
  OptField_t<POINT> m_optIACCMapWnd;
  OptField_t<POINT> m_optIACCWMapWnd;
  OptField_t<POINT> m_optIATCLTMapWnd;
  OptField_t<POINT> m_optBarocorrMapWnd;
  OptField_t<POINT> m_optManIgntimMapWnd;
  OptField_t<POINT> m_optCESettingsWnd;
  OptField_t<POINT> m_optTpsswtMapWnd;
  OptField_t<POINT> m_optTmp2CurveMapWnd;
  OptField_t<POINT> m_optGtscMapWnd;
  OptField_t<POINT> m_optGpscMapWnd;
  OptField_t<POINT> m_optAtscMapWnd;
  OptField_t<POINT> m_optCrkTempMapWnd;
  OptField_t<POINT> m_optEHPauseMapWnd;
  OptField_t<POINT> m_optCrankingThrdMapWnd;
  OptField_t<POINT> m_optCrankingTimeMapWnd;
  OptField_t<POINT> m_optSmapabanThrdMapWnd;
  OptField_t<POINT> m_optPwm1MapWnd;
  OptField_t<POINT> m_optPwm2MapWnd;
  OptField_t<POINT> m_optKnockZoneMapWnd;
  OptField_t<POINT> m_optGrtsCurveMapWnd;
  OptField_t<POINT> m_optGrHeatDutyMapWnd;
  OptField_t<POINT> m_optPwmIacUCoefMapWnd;
  OptField_t<POINT> m_optAftstrStrk0MapWnd;
  OptField_t<POINT> m_optAftstrStrk1MapWnd;
  OptField_t<POINT> m_optGrValDelMapWnd;
  OptField_t<POINT> m_optIACMATMapWnd;
  OptField_t<POINT> m_optFtlsCurveMapWnd;
  OptField_t<POINT> m_optEgtsCurveMapWnd;
  OptField_t<POINT> m_optOpsCurveMapWnd;
  OptField_t<POINT> m_optManInjPwcMapWnd;
  OptField_t<POINT> m_optMAFCurveMapWnd;

  //windows' positions (online tables)
  OptField_t<POINT> m_optStrtMapWnd1;
  OptField_t<POINT> m_optIdleMapWnd1;
  OptField_t<POINT> m_optWorkMapWnd1;
  OptField_t<POINT> m_optTempMapWnd1;
  OptField_t<POINT> m_optTempIdlMapWnd1;
  OptField_t<POINT> m_optGridMapIgnWnd1;
  OptField_t<POINT> m_optGridMapInjWnd1;
  OptField_t<POINT> m_optVEMapWnd1;
  OptField_t<POINT> m_optVE2MapWnd1;
  OptField_t<POINT> m_optAFRMapWnd1;
  OptField_t<POINT> m_optCrnkMapWnd1;
  OptField_t<POINT> m_optWrmpMapWnd1;
  OptField_t<POINT> m_optDeadMapWnd1;
  OptField_t<POINT> m_optIdlrMapWnd1;
  OptField_t<POINT> m_optIdlcMapWnd1;
  OptField_t<POINT> m_optAETPSMapWnd1;
  OptField_t<POINT> m_optAERPMMapWnd1;
  OptField_t<POINT> m_optAftstrMapWnd1;
  OptField_t<POINT> m_optITMapWnd1;
  OptField_t<POINT> m_optITRPMMapWnd1;
  OptField_t<POINT> m_optRigidMapWnd1;
  OptField_t<POINT> m_optEGOCrvMapWnd1;
  OptField_t<POINT> m_optIACCMapWnd1;
  OptField_t<POINT> m_optIACCWMapWnd1;
  OptField_t<POINT> m_optIATCLTMapWnd1;
  OptField_t<POINT> m_optTpsswtMapWnd1;
  OptField_t<POINT> m_optGtscMapWnd1;
  OptField_t<POINT> m_optGpscMapWnd1;
  OptField_t<POINT> m_optAtscMapWnd1;
  OptField_t<POINT> m_optPwm1MapWnd1;
  OptField_t<POINT> m_optPwm2MapWnd1;
  OptField_t<POINT> m_optIACMATMapWnd1;

  //window's sizes
  OptField_t<POINT> m_optMainFrmWndSize;
  OptField_t<POINT> m_optStrtMapWndSize;
  OptField_t<POINT> m_optIdleMapWndSize;
  OptField_t<POINT> m_optWorkMapWndSize;
  OptField_t<POINT> m_optTempMapWndSize;
  OptField_t<POINT> m_optTempIdlMapWndSize;
  OptField_t<POINT> m_optAttenMapWndSize;
  OptField_t<POINT> m_optDwellCntrlMapWndSize;
  OptField_t<POINT> m_optCTSCurveMapWndSize;
  OptField_t<POINT> m_optGridMapIgnWndSize;
  OptField_t<POINT> m_optGridMapInjWndSize;
  OptField_t<POINT> m_optVEMapWndSize;
  OptField_t<POINT> m_optVE2MapWndSize;
  OptField_t<POINT> m_optAFRMapWndSize;
  OptField_t<POINT> m_optCrnkMapWndSize;
  OptField_t<POINT> m_optWrmpMapWndSize;
  OptField_t<POINT> m_optDeadMapWndSize;
  OptField_t<POINT> m_optIdlrMapWndSize;
  OptField_t<POINT> m_optIdlcMapWndSize;
  OptField_t<POINT> m_optATSCurvMapWndSize;
  OptField_t<POINT> m_optATSCorrMapWndSize;
  OptField_t<POINT> m_optAETPSMapWndSize;
  OptField_t<POINT> m_optAERPMMapWndSize;
  OptField_t<POINT> m_optAftstrMapWndSize;
  OptField_t<POINT> m_optGasdoseMapWndSize;
  OptField_t<POINT> m_optITMapWndSize;
  OptField_t<POINT> m_optITRPMMapWndSize;
  OptField_t<POINT> m_optRigidMapWndSize;
  OptField_t<POINT> m_optEGOCrvMapWndSize;
  OptField_t<POINT> m_optIACCMapWndSize;
  OptField_t<POINT> m_optIACCWMapWndSize;
  OptField_t<POINT> m_optIATCLTMapWndSize;
  OptField_t<POINT> m_optBarocorrMapWndSize;
  OptField_t<POINT> m_optManIgntimMapWndSize;
  OptField_t<POINT> m_optCESettingsWndSize;
  OptField_t<POINT> m_optTpsswtMapWndSize;
  OptField_t<POINT> m_optTmp2CurveMapWndSize;
  OptField_t<POINT> m_optGtscMapWndSize;
  OptField_t<POINT> m_optGpscMapWndSize;
  OptField_t<POINT> m_optAtscMapWndSize;
  OptField_t<POINT> m_optCrkTempMapWndSize;
  OptField_t<POINT> m_optEHPauseMapWndSize;
  OptField_t<POINT> m_optCrankingThrdMapWndSize;
  OptField_t<POINT> m_optCrankingTimeMapWndSize;
  OptField_t<POINT> m_optSmapabanThrdMapWndSize;
  OptField_t<POINT> m_optPwm1MapWndSize;
  OptField_t<POINT> m_optPwm2MapWndSize;
  OptField_t<POINT> m_optKnockZoneMapWndSize;
  OptField_t<POINT> m_optGrtsCurveMapWndSize;
  OptField_t<POINT> m_optGrHeatDutyMapWndSize;
  OptField_t<POINT> m_optPwmIacUCoefMapWndSize;
  OptField_t<POINT> m_optAftstrStrk0MapWndSize;
  OptField_t<POINT> m_optAftstrStrk1MapWndSize;
  OptField_t<POINT> m_optGrValDelMapWndSize;
  OptField_t<POINT> m_optIACMATMapWndSize;
  OptField_t<POINT> m_optFtlsCurveMapWndSize;
  OptField_t<POINT> m_optEgtsCurveMapWndSize;
  OptField_t<POINT> m_optOpsCurveMapWndSize;
  OptField_t<POINT> m_optManInjPwcMapWndSize;
  OptField_t<POINT> m_optMAFCurveMapWndSize;

  //windows' sizes (online tables)
  OptField_t<POINT> m_optStrtMapWndSize1;
  OptField_t<POINT> m_optIdleMapWndSize1;
  OptField_t<POINT> m_optWorkMapWndSize1;
  OptField_t<POINT> m_optTempMapWndSize1;
  OptField_t<POINT> m_optTempIdlMapWndSize1;
  OptField_t<POINT> m_optGridMapIgnWndSize1;
  OptField_t<POINT> m_optGridMapInjWndSize1;
  OptField_t<POINT> m_optVEMapWndSize1;
  OptField_t<POINT> m_optVE2MapWndSize1;
  OptField_t<POINT> m_optAFRMapWndSize1;
  OptField_t<POINT> m_optCrnkMapWndSize1;
  OptField_t<POINT> m_optWrmpMapWndSize1;
  OptField_t<POINT> m_optDeadMapWndSize1;
  OptField_t<POINT> m_optIdlrMapWndSize1;
  OptField_t<POINT> m_optIdlcMapWndSize1;
  OptField_t<POINT> m_optAETPSMapWndSize1;
  OptField_t<POINT> m_optAERPMMapWndSize1;
  OptField_t<POINT> m_optAftstrMapWndSize1;
  OptField_t<POINT> m_optITMapWndSize1;
  OptField_t<POINT> m_optITRPMMapWndSize1;
  OptField_t<POINT> m_optRigidMapWndSize1;
  OptField_t<POINT> m_optEGOCrvMapWndSize1;
  OptField_t<POINT> m_optIACCMapWndSize1;
  OptField_t<POINT> m_optIACCWMapWndSize1;
  OptField_t<POINT> m_optIATCLTMapWndSize1;
  OptField_t<POINT> m_optTpsswtMapWndSize1;
  OptField_t<POINT> m_optGtscMapWndSize1;
  OptField_t<POINT> m_optGpscMapWndSize1;
  OptField_t<POINT> m_optAtscMapWndSize1;
  OptField_t<POINT> m_optPwm1MapWndSize1;
  OptField_t<POINT> m_optPwm2MapWndSize1;
  OptField_t<POINT> m_optIACMATMapWndSize1;

  //window state
  OptField_t<int> m_optMainFrmWndState;

  //indicators
  OptField_t<float> m_optIndHeightPercent[2];
  OptField_t<int> m_optIndRows[2];
  OptField_t<int> m_optIndGas_v[2];
  OptField_t<int> m_optIndCarb[2];
  OptField_t<int> m_optIndIdleValve[2];
  OptField_t<int> m_optIndPowerValve[2];
  OptField_t<int> m_optIndStBlock[2];
  OptField_t<int> m_optIndAE[2];
  OptField_t<int> m_optIndCoolingFan[2];
  OptField_t<int> m_optIndCE[2];
  OptField_t<int> m_optIndFCRevLim[2];
  OptField_t<int> m_optIndFloodClear[2];
  OptField_t<int> m_optIndSysLocked[2];
  OptField_t<int> m_optIndIgn_i[2];
  OptField_t<int> m_optIndCond_i[2];
  OptField_t<int> m_optIndEpas_i[2];
  OptField_t<int> m_optIndAftStrEnr[2];
  OptField_t<int> m_optIndIacClLoop[2];
  OptField_t<int> m_optIndUniOut1[2];
  OptField_t<int> m_optIndUniOut2[2];
  OptField_t<int> m_optIndUniOut3[2];
  OptField_t<int> m_optIndUniOut4[2];
  OptField_t<int> m_optIndUniOut5[2];
  OptField_t<int> m_optIndUniOut6[2];

  //meters
  OptField_t<int> m_optMetRows[2];
  OptField_t<int> m_optMetRPM[2][2];
  OptField_t<int> m_optMetMAP[2][2];
  OptField_t<int> m_optMetVBat[2][2];
  OptField_t<int> m_optMetIgnTim[2][2];
  OptField_t<int> m_optMetCLT[2][2];
  OptField_t<int> m_optMetAddI1[2][2];
  OptField_t<int> m_optMetAddI2[2][2];
  OptField_t<int> m_optMetInjPW[2][2];
  OptField_t<int> m_optMetIAT[2][2];
  OptField_t<int> m_optMetEGOCorr[2][2];
  OptField_t<int> m_optMetTPS[2][2];
  OptField_t<int> m_optMetAirFlow[2][2];
  OptField_t<int> m_optMetVehicleSpeed[2][2];
  OptField_t<int> m_optMetTPSDot[2][2];
  OptField_t<int> m_optMetMAP2[2][2];
  OptField_t<int> m_optMetMAPD[2][2];
  OptField_t<int> m_optMetTmp2[2][2];
  OptField_t<int> m_optMetFuelConsum[2][2];
  OptField_t<int> m_optMetKnockRetard[2][2];
  OptField_t<int> m_optMetKnockGraph[2][2];
  OptField_t<int> m_optMetSensAFR[2][2];
  OptField_t<int> m_optMetChokePos[2][2];
  OptField_t<int> m_optMetGDPos[2][2];
  OptField_t<int> m_optMetSynLoad[2][2];
  OptField_t<int> m_optMetInjTimB[2][2];
  OptField_t<int> m_optMetInjTimE[2][2];
  OptField_t<int> m_optMetFuelConsumF[2][2];
  OptField_t<int> m_optMetGrts[2][2];
  OptField_t<int> m_optMetFtls[2][2];
  OptField_t<int> m_optMetEgts[2][2];
  OptField_t<int> m_optMetOps[2][2];
  OptField_t<int> m_optMetInjDuty[2][2];
  OptField_t<int> m_optMetMAF[2][2];
  OptField_t<int> m_optMetVentDuty[2][2];

  //colors of indicators
  OptField_t<COLORREF> m_optColGas_v;
  OptField_t<COLORREF> m_optColCarb;
  OptField_t<COLORREF> m_optColIdleValve;
  OptField_t<COLORREF> m_optColPowerValve;
  OptField_t<COLORREF> m_optColStBlock;
  OptField_t<COLORREF> m_optColAE;
  OptField_t<COLORREF> m_optColCoolingFan;
  OptField_t<COLORREF> m_optColCE;
  OptField_t<COLORREF> m_optColFCRevLim;
  OptField_t<COLORREF> m_optColFloodClear;
  OptField_t<COLORREF> m_optColSysLocked;
  OptField_t<COLORREF> m_optColIgn_i;
  OptField_t<COLORREF> m_optColCond_i;
  OptField_t<COLORREF> m_optColEpas_i;
  OptField_t<COLORREF> m_optColAftStrEnr;
  OptField_t<COLORREF> m_optColIacClLoop;
  OptField_t<COLORREF> m_optColUniOut1;
  OptField_t<COLORREF> m_optColUniOut2;
  OptField_t<COLORREF> m_optColUniOut3;
  OptField_t<COLORREF> m_optColUniOut4;
  OptField_t<COLORREF> m_optColUniOut5;
  OptField_t<COLORREF> m_optColUniOut6;


  //AutoTune
  OptField_t<std::vector<int> > m_optLambdaDelay;
  OptField_t<std::vector<int> > m_optLambdaDelayL;
  OptField_t<std::vector<int> > m_optLambdaDelayR;
  OptField_t<float> m_optAFRError;
  OptField_t<std::vector<int> > m_optBlockedCells;
  OptField_t<int> m_optStatSize;
  OptField_t<int> m_optAutoBlockThrd;
  OptField_t<bool> m_optGrowingMode;
  OptField_t<float> m_optMinAFR;
  OptField_t<float> m_optMaxAFR;
  OptField_t<int> m_optMinDistThrd;
  OptField_t<float> m_optMinTPS;
  OptField_t<float> m_optMaxTPS;
  OptField_t<float> m_optCLTThrd;
  OptField_t<float> m_optTunSoftness;

  //Map editor
  OptField_t<int> m_optGradSaturation;
  OptField_t<int> m_optGradBrightness;
  OptField_t<int> m_optBoldFont;
  OptField_t<int> m_optITEdMode;
  OptField_t<int> m_optSpotMarkers;
  OptField_t<float> m_optSpotMarkersSize;
  OptField_t<int> m_optActiveVEMap;
  
  //Splitters
  OptField_t<int> m_optParamMonVert;
  OptField_t<int> m_optLogPlayerVert;

  //Inj. Driver related tab settings
  OptField_t<bool> m_optInjDrvTabActive;
  OptField_t<COLORREF> m_optVoltLineColor;
  OptField_t<float> m_optPeakOnPtMovStep;
  OptField_t<float> m_optPeakDutyPtMovStep;
  OptField_t<float> m_optHoldDutyPtMovStep;
  OptField_t<float> m_optPeakFullPtMovStep;
  OptField_t<float> m_optPthPausePtMovStep;
  OptField_t<bool> m_optShowGraphLabels;
  OptField_t<float> m_optPWMulPtMovStep;
  OptField_t<float> m_optPWAddPtMovStep;

  //Points's moving step in map editor windows
  OptField_t<float> m_optPtMovStepWorkMap;
  OptField_t<float> m_optPtMovStepTempMap;
  OptField_t<float> m_optPtMovStepTempIdlMap;
  OptField_t<float> m_optPtMovStepStartMap;
  OptField_t<float> m_optPtMovStepIdleMap;
  OptField_t<float> m_optPtMovStepVeMap;
  OptField_t<float> m_optPtMovStepVe2Map;
  OptField_t<float> m_optPtMovStepAfrMap;
  OptField_t<float> m_optPtMovStepCrnkMap;
  OptField_t<float> m_optPtMovStepWrmpMap;
  OptField_t<float> m_optPtMovStepDeadMap;
  OptField_t<float> m_optPtMovStepIdlrMap;
  OptField_t<float> m_optPtMovStepIdlcMap;
  OptField_t<float> m_optPtMovStepAetpsMap;
  OptField_t<float> m_optPtMovStepAerpmMap;
  OptField_t<float> m_optPtMovStepAftstrMap;
  OptField_t<float> m_optPtMovStepItMap;
  OptField_t<float> m_optPtMovStepItrpmMap;
  OptField_t<float> m_optPtMovStepRigidMap;
  OptField_t<float> m_optPtMovStepEgocrvMap;
  OptField_t<float> m_optPtMovStepIaccMap;
  OptField_t<float> m_optPtMovStepIaccwMap;
  OptField_t<float> m_optPtMovStepIatcltMap;
  OptField_t<float> m_optPtMovStepTpsswtMap;
  OptField_t<float> m_optPtMovStepGtscMap;
  OptField_t<float> m_optPtMovStepGpscMap;
  OptField_t<float> m_optPtMovStepAtscMap;
  OptField_t<float> m_optPtMovStepPwm1Map;
  OptField_t<float> m_optPtMovStepPwm2Map;
  OptField_t<float> m_optPtMovStepIACMATMap;
  //separate
  OptField_t<float> m_optPtMovStepCrkTempMap;
  OptField_t<float> m_optPtMovStepEHPauseMap;
  OptField_t<float> m_optPtMovStepAttenMap;
  OptField_t<float> m_optPtMovStepDwellCntrlMap;
  OptField_t<float> m_optPtMovStepCTSCurveMap;
  OptField_t<float> m_optPtMovStepBarocorrMap;
  OptField_t<float> m_optPtMovStepManIgntimMap;
  OptField_t<float> m_optPtMovStepATSCurvMap;
  OptField_t<float> m_optPtMovStepATSCorrMap;
  OptField_t<float> m_optPtMovStepGasdoseMap;
  OptField_t<float> m_optPtMovStepTmp2CurveMap;
  OptField_t<float> m_optPtMovStepCrankingThrdMap;
  OptField_t<float> m_optPtMovStepCrankingTimeMap;
  OptField_t<float> m_optPtMovStepSmapabanThrdMap;
  OptField_t<float> m_optPtMovStepKnockZoneMap;
  OptField_t<float> m_optPtMovStepGrtsCurveMap;
  OptField_t<float> m_optPtMovStepGrHeatDutyMap;
  OptField_t<float> m_optPtMovStepPwmIacUCoefMap;
  OptField_t<float> m_optPtMovStepAftstrStrk0Map;
  OptField_t<float> m_optPtMovStepAftstrStrk1Map;
  OptField_t<float> m_optPtMovStepGrValDelMap;
  OptField_t<float> m_optPtMovStepFtlsCurveMap;
  OptField_t<float> m_optPtMovStepEgtsCurveMap;
  OptField_t<float> m_optPtMovStepOpsCurveMap;
  OptField_t<float> m_optPtMovStepManInjPwcMap;
  OptField_t<float> m_optPtMovStepMAFCurveMap;

  //Log file's fileds
  OptField_t<bool> m_optWriteLogFields;
  OptField_t<_TSTRING> m_optLogFieldTime;
  OptField_t<_TSTRING> m_optLogFieldRPM;
  OptField_t<_TSTRING> m_optLogFieldIgnTim;
  OptField_t<_TSTRING> m_optLogFieldMAP;
  OptField_t<_TSTRING> m_optLogFieldVBat;
  OptField_t<_TSTRING> m_optLogFieldCLT;
  OptField_t<_TSTRING> m_optLogFieldKnock;
  OptField_t<_TSTRING> m_optLogFieldKnockCorr;
  OptField_t<_TSTRING> m_optLogFieldLoadCurve;
  OptField_t<_TSTRING> m_optLogFieldCarb;
  OptField_t<_TSTRING> m_optLogFieldGas_v;
  OptField_t<_TSTRING> m_optLogFieldIdleValve;
  OptField_t<_TSTRING> m_optLogFieldPowerValve;
  OptField_t<_TSTRING> m_optLogFieldCoolingFan;
  OptField_t<_TSTRING> m_optLogFieldStBlock;
  OptField_t<_TSTRING> m_optLogFieldAE;
  OptField_t<_TSTRING> m_optLogFieldFCRevLim;
  OptField_t<_TSTRING> m_optLogFieldFloodClear;
  OptField_t<_TSTRING> m_optLogFieldSysLocked;
  OptField_t<_TSTRING> m_optLogFieldCE;
  OptField_t<_TSTRING> m_optLogFieldIgn_i;
  OptField_t<_TSTRING> m_optLogFieldCond_i;
  OptField_t<_TSTRING> m_optLogFieldEpas_i;
  OptField_t<_TSTRING> m_optLogFieldTPS;
  OptField_t<_TSTRING> m_optLogFieldAdd_i1;
  OptField_t<_TSTRING> m_optLogFieldAdd_i2;
  OptField_t<_TSTRING> m_optLogFieldChokePos;
  OptField_t<_TSTRING> m_optLogFieldGDPos;
  OptField_t<_TSTRING> m_optLogFieldVehSpeed;
  OptField_t<_TSTRING> m_optLogFieldPasDist;
  OptField_t<_TSTRING> m_optLogFieldFuelConsum;
  OptField_t<_TSTRING> m_optLogFieldFuelConsFreq;
  OptField_t<_TSTRING> m_optLogFieldIAT;
  OptField_t<_TSTRING> m_optLogFieldStrtIgnTim;
  OptField_t<_TSTRING> m_optLogFieldIdleIgnTim;
  OptField_t<_TSTRING> m_optLogFieldWorkIgnTim;
  OptField_t<_TSTRING> m_optLogFieldTempIgnTim;
  OptField_t<_TSTRING> m_optLogFieldIATIgnTim;
  OptField_t<_TSTRING> m_optLogFieldIdlRegIgnTim;
  OptField_t<_TSTRING> m_optLogFieldOctanCorr;
  OptField_t<_TSTRING> m_optLogFieldEGOCorr;
  OptField_t<_TSTRING> m_optLogFieldInjPW;
  OptField_t<_TSTRING> m_optLogFieldTPSdot;
  OptField_t<_TSTRING> m_optLogFieldMAP2;
  OptField_t<_TSTRING> m_optLogFieldTmp2;
  OptField_t<_TSTRING> m_optLogFieldDiffMAP;
  OptField_t<_TSTRING> m_optLogFieldAFR;
  OptField_t<_TSTRING> m_optLogFieldSynLoad;
  OptField_t<_TSTRING> m_optLogFieldBaroPress;
  OptField_t<_TSTRING> m_optLogFieldInjTimBeg;
  OptField_t<_TSTRING> m_optLogFieldInjTimEnd;
  OptField_t<_TSTRING> m_optLogFieldLogMarks;
  OptField_t<_TSTRING> m_optLogFieldCECodes;
  OptField_t<_TSTRING> m_optLogFieldGrts;
  OptField_t<_TSTRING> m_optLogFieldFtls;
  OptField_t<_TSTRING> m_optLogFieldEgts;
  OptField_t<_TSTRING> m_optLogFieldOps;
  OptField_t<_TSTRING> m_optLogFieldAftStrEnr;
  OptField_t<_TSTRING> m_optLogFieldIacClLoop;
  OptField_t<_TSTRING> m_optLogFieldInjDuty;
  OptField_t<_TSTRING> m_optLogFieldRigidArg;
  OptField_t<_TSTRING> m_optLogFieldServFlag;
  OptField_t<_TSTRING> m_optLogFieldRxlaf;
  OptField_t<_TSTRING> m_optLogFieldMAF;
  OptField_t<_TSTRING> m_optLogFieldVentDuty;
  OptField_t<_TSTRING> m_optLogFieldUniOuts;

  //Functionality Section
  OptField_t<bool> m_optFuncSM_CONTROL;
  OptField_t<bool> m_optFuncGD_CONTROL;

  //folder from which application has been started
  TCHAR m_current_directory[MAX_PATH+1];
};
