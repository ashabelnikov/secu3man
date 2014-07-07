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

#pragma once

#include <string>
#include <vector>
#include "common/unicodesupport.h"
#include "ISettingsData.h"

class CAppSettingsController;

//хранит и сохраняет/загружает данные
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
  //Возвращает полное имя INI-файла. INI-файл находится в каталоге из которого
  //запущена программа.
  CString GetINIFileFullName(void) const;

  //чтение настроек из INI-файла
  bool ReadSettings(void);

  //запись настроек в INI-файл
  bool WriteSettings(void);

  //имя секции и названия полей
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

  //позиции окон таблиц УОЗ
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

  //fixture settings
  const CString m_Name_Fixtures_Section;
  const CString m_Name_Tachometer_Max;
  const CString m_Name_Pressure_Max;
  const CString m_Name_PulsesPer1Km;

  //ISettingsData
  virtual const _TSTRING& GetPortName(void) const;
  virtual DWORD GetBaudRateApplication(void) const;
  virtual DWORD GetBaudRateBootloader(void) const;
  virtual const CString& GetLogFilesFolder(void) const;
  virtual bool  GetUseAppFolder(void) const;
  virtual bool  GetAlwaysWriteLog(void) const;
  virtual char  GetCSVSepSymbol(void) const;
  virtual int   GetMIDeskUpdatePeriod(void) const;
  //Возвращает полный путь к каталогу из которого было запущено приложение
  //(каталог который был текущим на момент вызва конструктора этого класса)
  virtual CString GetAppDirectory(void) const;
  //позиции окон
  virtual void SetWndSettings(const WndSettings& i_wndSettings);
  virtual void GetWndSettings(WndSettings& o_wndSettings) const;
  //язык интерфейса и тип платформы
  virtual EInterLang GetInterfaceLanguage(void) const;
  virtual EECUPlatform GetECUPlatformType(void) const;
  //fixtures's customization
  virtual int GetTachometerMax(void) const;
  virtual int GetPressureMax(void) const;
  //Debug features
  virtual bool GetUseDVFeatures(void) const;
  virtual int GetDVDeskUpdatePeriod(void) const;
  virtual bool GetShowToolTips(void) const;
  virtual bool GetShowExFixtures(void) const;
  virtual bool GetHexDataMode(void) const;
  virtual int GetNumPulsesPer1Km(void) const;
  ////////////////////////////////////////////////////

 private:
  //данные которые хранятся в INI-файле
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

  bool m_optUseDVFeatures;
  int m_optDVDeskUpdatePeriod;

  bool m_optShowToolTips;
  bool m_optShowExFixtures;
  bool m_optHexDataMode;

  //проверяет указанное значение скорости на соответствие стандарту
  bool CheckAllowableBaudRate(DWORD baud);
  bool CheckAllowableCSVSepSymbol(char i_symbol);
  bool CheckAllowableLanguage(const _TSTRING& i_string, EInterLang& o_language_id);
  bool CheckAllowablePlatform(const _TSTRING& i_string, EECUPlatform& o_platform_id);

  //директория из которой было запущено приложение
  TCHAR m_current_directory[MAX_PATH+1];
};
