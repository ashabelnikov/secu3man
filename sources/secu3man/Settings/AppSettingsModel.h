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
  std::vector<std::pair<std::pair<_TSTRING, _TSTRING>, int> > m_AllowablePlatforms;

  CAppSettingsModel();
  virtual ~CAppSettingsModel();

  ////////////////////////////////////////////////////
  //¬озвращает полное им€ INI-файла. INI-файл находитс€ в каталоге из которого
  //запущена программа.
  CString GetINIFileFullName(void) const;

  //read settings from INI-file
  bool ReadSettings(void);

  //write settings into INI-file
  bool WriteSettings(void);

  //Section and fields names
  const CString m_Name_Options_Section;
  const CString m_Name_PortName;
  const CString m_Name_BaudRateApplication;
  const CString m_Name_BaudRateBootloader;
  const CString m_Name_LogFilesFolder;
  const CString m_Name_UseAppFolder;
  const CString m_Name_AlwaysWriteLog;
  const CString m_Name_CSVSepSymbol;
  const CString m_Name_MIDeskUpdatePeriod;
  const CString m_Name_InterfaceLang;
  const CString m_Name_ECUPlatformType;
  const CString m_Name_UseDVFeatures;
  const CString m_Name_DVDeskUpdatePeriod;
  const CString m_Name_ShowToolTips;
  const CString m_Name_ShowExFixtures;
  const CString m_Name_HexDataMode;
  const CString m_Name_COMPortBother;
  const CString m_Name_UseHotKeys;
  const CString m_Name_ShowWelcome;
  const CString m_Name_RPMAverage;
  const CString m_Name_VoltAverage;
  const CString m_Name_MAPAverage;
  const CString m_Name_AI1Average;
  const CString m_Name_TPSAverage;
  const CString m_Name_AllowVisualTheme;
  const CString m_Name_AutoDiagEnter;
  const CString m_Name_SaveWarning;
  const CString m_Name_AutoCERead;

  //позиции окон таблиц ”ќ«
  const CString m_Name_WndSettings_Section;
  const CString m_Name_StrtMapWnd_X;
  const CString m_Name_StrtMapWnd_Y;
  const CString m_Name_IdleMapWnd_X;
  const CString m_Name_IdleMapWnd_Y;
  const CString m_Name_WorkMapWnd_X;
  const CString m_Name_WorkMapWnd_Y;
  const CString m_Name_TempMapWnd_X;
  const CString m_Name_TempMapWnd_Y;
  const CString m_Name_AttenMapWnd_X;
  const CString m_Name_AttenMapWnd_Y;
  const CString m_Name_MainFrmWnd_X;
  const CString m_Name_MainFrmWnd_Y;
  const CString m_Name_DwellCntrlMapWnd_X;
  const CString m_Name_DwellCntrlMapWnd_Y;
  const CString m_Name_CTSCurveMapWnd_X;
  const CString m_Name_CTSCurveMapWnd_Y;
  const CString m_Name_ChokeOpMapWnd_X;
  const CString m_Name_ChokeOpMapWnd_Y;
  const CString m_Name_GridMapWnd_X;
  const CString m_Name_GridMapWnd_Y;
  const CString m_Name_VEMapWnd_X;
  const CString m_Name_VEMapWnd_Y;
  const CString m_Name_AFRMapWnd_X;
  const CString m_Name_AFRMapWnd_Y;
  const CString m_Name_CrnkMapWnd_X;
  const CString m_Name_CrnkMapWnd_Y;
  const CString m_Name_WrmpMapWnd_X;
  const CString m_Name_WrmpMapWnd_Y;
  const CString m_Name_DeadMapWnd_X;
  const CString m_Name_DeadMapWnd_Y;
  const CString m_Name_IdlrMapWnd_X;
  const CString m_Name_IdlrMapWnd_Y;
  const CString m_Name_IdlcMapWnd_X;
  const CString m_Name_IdlcMapWnd_Y;
  const CString m_Name_ATSCurvMapWnd_X;
  const CString m_Name_ATSCurvMapWnd_Y;
  const CString m_Name_ATSCorrMapWnd_X;
  const CString m_Name_ATSCorrMapWnd_Y;
  const CString m_Name_AETPSMapWnd_X;
  const CString m_Name_AETPSMapWnd_Y;
  const CString m_Name_AERPMMapWnd_X;
  const CString m_Name_AERPMMapWnd_Y;
  const CString m_Name_AftstrMapWnd_X;
  const CString m_Name_AftstrMapWnd_Y;
  const CString m_Name_GasdoseMapWnd_X;
  const CString m_Name_GasdoseMapWnd_Y;
  const CString m_Name_ITMapWnd_X;
  const CString m_Name_ITMapWnd_Y;
  const CString m_Name_ITRPMMapWnd_X;
  const CString m_Name_ITRPMMapWnd_Y;
  const CString m_Name_RigidMapWnd_X;
  const CString m_Name_RigidMapWnd_Y;
  const CString m_Name_EGOCrvMapWnd_X;
  const CString m_Name_EGOCrvMapWnd_Y;
  const CString m_Name_IACCMapWnd_X;
  const CString m_Name_IACCMapWnd_Y;
  const CString m_Name_IACCWMapWnd_X;
  const CString m_Name_IACCWMapWnd_Y;
  const CString m_Name_IATCLTMapWnd_X;
  const CString m_Name_IATCLTMapWnd_Y;
  const CString m_Name_BarocorrMapWnd_X;
  const CString m_Name_BarocorrMapWnd_Y;
  const CString m_Name_ManIgntimMapWnd_X;
  const CString m_Name_ManIgntimMapWnd_Y;

  //fixture settings
  const CString m_Name_Fixtures_Section;
  const CString m_Name_Tachometer_Max;
  const CString m_Name_Pressure_Max;
  const CString m_Name_PulsesPer1Km;
  const CString m_Name_SpeedUnit;

  //ISettingsData
  virtual const _TSTRING& GetPortName(void) const;
  virtual DWORD GetBaudRateApplication(void) const;
  virtual DWORD GetBaudRateBootloader(void) const;
  virtual const CString& GetLogFilesFolder(void) const;
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
  virtual int GetRPMAverage(void) const;
  virtual int GetVoltAverage(void) const;
  virtual int GetMAPAverage(void) const;
  virtual int GetAI1Average(void) const;
  virtual int GetTPSAverage(void) const;
  virtual bool GetAllowVisualTheme(void) const;
  virtual bool GetAutoDiagEnter(void) const;
  virtual bool GetSaveWarning(void) const;
  virtual bool GetAutoCERead(void) const;

  //windows positions
  virtual void SetWndSettings(const WndSettings& i_wndSettings);
  virtual void GetWndSettings(WndSettings& o_wndSettings) const;
  //€зык интерфейса и тип платформы
  virtual EInterLang GetInterfaceLanguage(void) const;
  virtual EECUPlatform GetECUPlatformType(void) const;
  //fixtures's customization
  virtual int GetTachometerMax(void) const;
  virtual int GetPressureMax(void) const;
  virtual ESpeedUnit GetSpeedUnit(void) const;

  //Debug features
  virtual bool GetUseDVFeatures(void) const;
  virtual int GetDVDeskUpdatePeriod(void) const;
  virtual bool GetShowToolTips(void) const;
  virtual bool GetShowExFixtures(void) const;
  virtual bool GetHexDataMode(void) const;
  virtual int GetNumPulsesPer1Km(void) const;
  ////////////////////////////////////////////////////

 private:
  //data which stored in the INI-file
  _TSTRING m_optPortName;
  DWORD m_optBaudRateApplication;
  DWORD m_optBaudRateBootloader;
  CString m_optLogFilesFolder;
  bool  m_optUseAppFolder;
  bool  m_optAlwaysWriteLog;
  char  m_optCSVSepSymbol;
  int   m_optMIDeskUpdatePeriod;
  EInterLang m_optInterLang;
  EECUPlatform m_optECUPlatformType;
  int m_optTachometerMax;
  int m_optPressureMax;
  int m_optPulsesPer1Km;
  ESpeedUnit m_optSpeedUnit;
  bool m_optCOMPortBother;
  bool m_optUseHotKeys;
  bool m_optShowWelcome;
  int  m_optRPMAverage;
  int  m_optVoltAverage;
  int  m_optMAPAverage;
  int  m_optAI1Average;
  int  m_optTPSAverage;
  bool m_optAllowVisualTheme;
  bool m_optAutoDiagEnter;
  bool m_optSaveWarning;
  bool m_optAutoCERead;

  int m_optStrtMapWnd_X;
  int m_optStrtMapWnd_Y;
  int m_optIdleMapWnd_X;
  int m_optIdleMapWnd_Y;
  int m_optWorkMapWnd_X;
  int m_optWorkMapWnd_Y;
  int m_optTempMapWnd_X;
  int m_optTempMapWnd_Y;
  int m_optAttenMapWnd_X;
  int m_optAttenMapWnd_Y;
  int m_optMainFrmWnd_X;
  int m_optMainFrmWnd_Y;
  int m_optDwellCntrlMapWnd_X;
  int m_optDwellCntrlMapWnd_Y;
  int m_optCTSCurveMapWnd_X;
  int m_optCTSCurveMapWnd_Y;
  int m_optChokeOpMapWnd_X;
  int m_optChokeOpMapWnd_Y;
  int m_optGridMapWnd_X;
  int m_optGridMapWnd_Y;
  int m_optVEMapWnd_X;
  int m_optVEMapWnd_Y;
  int m_optAFRMapWnd_X;
  int m_optAFRMapWnd_Y;
  int m_optCrnkMapWnd_X;
  int m_optCrnkMapWnd_Y;
  int m_optWrmpMapWnd_X;
  int m_optWrmpMapWnd_Y;
  int m_optDeadMapWnd_X;
  int m_optDeadMapWnd_Y;
  int m_optIdlrMapWnd_X;
  int m_optIdlrMapWnd_Y;
  int m_optIdlcMapWnd_X;
  int m_optIdlcMapWnd_Y;
  int m_optATSCurvMapWnd_X;
  int m_optATSCurvMapWnd_Y;
  int m_optATSCorrMapWnd_X;
  int m_optATSCorrMapWnd_Y;
  int m_optAETPSMapWnd_X;
  int m_optAETPSMapWnd_Y;
  int m_optAERPMMapWnd_X;
  int m_optAERPMMapWnd_Y;
  int m_optAftstrMapWnd_X;
  int m_optAftstrMapWnd_Y;
  int m_optGasdoseMapWnd_X;
  int m_optGasdoseMapWnd_Y;
  int m_optITMapWnd_X;
  int m_optITMapWnd_Y;
  int m_optITRPMMapWnd_X;
  int m_optITRPMMapWnd_Y;
  int m_optRigidMapWnd_X;
  int m_optRigidMapWnd_Y;
  int m_optEGOCrvMapWnd_X;
  int m_optEGOCrvMapWnd_Y;
  int m_optIACCMapWnd_X;
  int m_optIACCMapWnd_Y;
  int m_optIACCWMapWnd_X;
  int m_optIACCWMapWnd_Y;
  int m_optIATCLTMapWnd_X;
  int m_optIATCLTMapWnd_Y;
  int m_optBarocorrMapWnd_X;
  int m_optBarocorrMapWnd_Y;
  int m_optManIgntimMapWnd_X;
  int m_optManIgntimMapWnd_Y;

  bool m_optUseDVFeatures;
  int m_optDVDeskUpdatePeriod;

  bool m_optShowToolTips;
  bool m_optShowExFixtures;
  bool m_optHexDataMode;

  //провер€ет указанное значение скорости на соответствие стандарту
  bool CheckAllowableBaudRate(DWORD baud);
  bool CheckAllowableCSVSepSymbol(char i_symbol);
  bool CheckAllowableLanguage(const _TSTRING& i_string, EInterLang& o_language_id);
  bool CheckAllowablePlatform(const _TSTRING& i_string, EECUPlatform& o_platform_id);

  //директори€ из которой было запущено приложение
  TCHAR m_current_directory[MAX_PATH+1];
};
