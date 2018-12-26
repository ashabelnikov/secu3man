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

  //ISettingsData
  virtual const _TSTRING& GetPortName(void) const;
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
  virtual bool GetPortAutoReopen(void) const;
  virtual bool GetToggleMapWnd(void) const;
  virtual int GetParamMonVert(void) const;
  virtual void SetParamMonVert(int pos);
  virtual bool GetExistingPorts(void) const;

  //windows positions
  virtual void SetWndSettings(const WndSettings& i_wndSettings);
  virtual void GetWndSettings(WndSettings& o_wndSettings) const;

  virtual void SetWndSettings1(const WndSettings& i_wndSettings);
  virtual void GetWndSettings1(WndSettings& o_wndSettings) const;

  //windows' size
  virtual void SetWndSize(const WndSize& i_wndSize);
  virtual void GetWndSize(WndSize& o_wndSize) const;
  //windows' state
  virtual void SetWndState(const WndState& i_wndState);
  virtual void GetWndState(WndState& o_wndState) const;

  //€зык интерфейса и тип платформы
  virtual EInterLang GetInterfaceLanguage(void) const;
  virtual EECUPlatform GetECUPlatformType(void) const;

  //fixtures's customization
  virtual int GetTachometerMax(void) const;
  virtual int GetPressureMax(void) const;
  virtual ESpeedUnit GetSpeedUnit(void) const;
  virtual int GetRPMAverage(void) const;
  virtual int GetVoltAverage(void) const;
  virtual int GetMAPAverage(void) const;
  virtual int GetAI1Average(void) const;
  virtual int GetTPSAverage(void) const;
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
  virtual void GetMetersConfig(MetersCfg& o_cfg) const;
  virtual void SetMetersConfig(const MetersCfg& i_cfg);

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

  virtual int GetGradSaturation(void);
  virtual int GetGradBrightness(void);
  virtual int GetBoldFont(void);

 private:
  //¬озвращает полное им€ INI-файла. INI-файл находитс€ в каталоге из которого
  //запущена программа.
  CString GetINIFileFullName(void) const;

  //data which stored in the INI-file:
  //Section names
  const CString m_Name_Options_Section;
  const CString m_Name_WndSettings_Section;  
  const CString m_Name_WndSettings_Section1;  //online
  const CString m_Name_WndSize_Section;  
  const CString m_Name_WndState_Section;  
  const CString m_Name_Fixtures_Section;
  CString m_Name_Indicators_Section[2];
  CString m_Name_Meters_Section[2];
  const CString m_Name_IndColors_Section;
  const CString m_Name_AutoTune_Section;
  const CString m_Name_MapEditor_Section;
  const CString m_Name_Splitters_Section;

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
  OptField_t<int> m_optTitleFontSize;
  OptField_t<int> m_optValueFontSize;
  OptField_t<int> m_optPaneFontSize;
  OptField_t<int> m_optLabelFontSize;
  OptField_t<bool> m_optMetersDragNDrop;
  OptField_t<bool> m_optIndicatorsDragNDrop;

  //windows' positions
  OptField_t<POINT> m_optStrtMapWnd;
  OptField_t<POINT> m_optIdleMapWnd;
  OptField_t<POINT> m_optWorkMapWnd;
  OptField_t<POINT> m_optTempMapWnd;
  OptField_t<POINT> m_optAttenMapWnd;
  OptField_t<POINT> m_optMainFrmWnd;
  OptField_t<POINT> m_optDwellCntrlMapWnd;
  OptField_t<POINT> m_optCTSCurveMapWnd;
  OptField_t<POINT> m_optGridMapIgnWnd;
  OptField_t<POINT> m_optGridMapInjWnd;
  OptField_t<POINT> m_optVEMapWnd;
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

  //windows' positions (online tables)
  OptField_t<POINT> m_optStrtMapWnd1;
  OptField_t<POINT> m_optIdleMapWnd1;
  OptField_t<POINT> m_optWorkMapWnd1;
  OptField_t<POINT> m_optTempMapWnd1;
  OptField_t<POINT> m_optGridMapIgnWnd1;
  OptField_t<POINT> m_optGridMapInjWnd1;
  OptField_t<POINT> m_optVEMapWnd1;
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

  //window size
  OptField_t<POINT> m_optMainFrmWndSize;

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
  //meters
  OptField_t<int> m_optMetRows[2];
  OptField_t<int> m_optMetRPM[2];
  OptField_t<int> m_optMetMAP[2];
  OptField_t<int> m_optMetVBat[2];
  OptField_t<int> m_optMetIgnTim[2];
  OptField_t<int> m_optMetCLT[2];
  OptField_t<int> m_optMetAddI1[2];
  OptField_t<int> m_optMetAddI2[2];
  OptField_t<int> m_optMetInjPW[2];
  OptField_t<int> m_optMetIAT[2];
  OptField_t<int> m_optMetEGOCorr[2];
  OptField_t<int> m_optMetTPS[2];
  OptField_t<int> m_optMetAirFlow[2];
  OptField_t<int> m_optMetVehicleSpeed[2];
  OptField_t<int> m_optMetTPSDot[2];
  OptField_t<int> m_optMetMAP2[2];
  OptField_t<int> m_optMetMAPD[2];
  OptField_t<int> m_optMetTmp2[2];
  OptField_t<int> m_optMetFuelConsum[2];
  OptField_t<int> m_optMetKnockRetard[2];
  OptField_t<int> m_optMetKnockGraph[2];
  OptField_t<int> m_optMetSensAFR[2];
  OptField_t<int> m_optMetChokePos[2];
  OptField_t<int> m_optMetGDPos[2];
  OptField_t<int> m_optMetSynLoad[2];

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

  //Map editor
  OptField_t<int> m_optGradSaturation;
  OptField_t<int> m_optGradBrightness;
  OptField_t<int> m_optBoldFont;

  //Splitters
  OptField_t<int> m_optParamMonVert;

  //folder from which application has been started
  TCHAR m_current_directory[MAX_PATH+1];
};
