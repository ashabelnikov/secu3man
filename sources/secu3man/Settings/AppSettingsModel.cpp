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

#include "stdafx.h"
#include "AppSettingsModel.h"

#include <limits>
#include <algorithm>

#undef max //avoid conflicts with C++

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAppSettingsModel::CAppSettingsModel()
: m_Name_Options_Section(_T("Options"))
, m_Name_BaudRateApplication(_T("Application_baud_rate"))
, m_Name_BaudRateBootloader(_T("Boot_loader_baud_rate"))
, m_Name_PortName(_T("COM_port"))
, m_Name_LogFilesFolder(_T("LogFilesFolder"))
, m_Name_UseAppFolder(_T("UseAppFolder"))
, m_Name_AlwaysWriteLog(_T("AlwaysWriteLog"))
, m_Name_CSVSepSymbol(_T("CSVSeparatingSymbol"))
, m_Name_MIDeskUpdatePeriod(_T("MI_Desk_UpdatePeriod"))
, m_Name_InterfaceLang(_T("InterfaceLanguage"))
, m_Name_ECUPlatformType(_T("ECUPlatformType"))
, m_Name_UseDVFeatures(_T("UseDVFeatures"))
, m_Name_ShowToolTips(_T("ShowToolTips"))
, m_Name_ShowExFixtures(_T("ExFixtures"))
, m_Name_HexDataMode(_T("HexDataMode"))
, m_Name_DVDeskUpdatePeriod(_T("DVDeskUpdatePeriod"))
//positions of windows
, m_Name_WndSettings_Section(_T("WndSettings"))
, m_Name_StrtMapWnd_X(_T("StrtMapWnd_X"))
, m_Name_StrtMapWnd_Y(_T("StrtMapWnd_Y"))
, m_Name_IdleMapWnd_X(_T("IdleMapWnd_X"))
, m_Name_IdleMapWnd_Y(_T("IdleMapWnd_Y"))
, m_Name_WorkMapWnd_X(_T("WorkMapWnd_X"))
, m_Name_WorkMapWnd_Y(_T("WorkMapWnd_Y"))
, m_Name_TempMapWnd_X(_T("TempMapWnd_X"))
, m_Name_TempMapWnd_Y(_T("TempMapWnd_Y"))
, m_Name_AttenMapWnd_X(_T("AttenMapWnd_X"))
, m_Name_AttenMapWnd_Y(_T("AttenMapWnd_Y"))
, m_Name_MainFrmWnd_X(_T("MainFrmWnd_X"))
, m_Name_MainFrmWnd_Y(_T("MainFrmWnd_Y"))
, m_Name_DwellCntrlMapWnd_X(_T("DwellCntrlMapWnd_X"))
, m_Name_DwellCntrlMapWnd_Y(_T("DwellCntrlMapWnd_Y"))
, m_Name_CTSCurveMapWnd_X(_T("CTSCurveMapWnd_X"))
, m_Name_CTSCurveMapWnd_Y(_T("CTSCurveMapWnd_Y"))
, m_Name_ChokeOpMapWnd_X(_T("ChokeOpMapWnd_X"))
, m_Name_ChokeOpMapWnd_Y(_T("ChokeOpMapWnd_Y"))
, m_Name_GridMapWnd_X(_T("GridMapWnd_X"))
, m_Name_GridMapWnd_Y(_T("GridMapWnd_Y"))
, m_Name_VEMapWnd_X(_T("VEMapWnd_X"))
, m_Name_VEMapWnd_Y(_T("VEMapWnd_Y"))
, m_Name_AFRMapWnd_X(_T("AFRMapWnd_X"))
, m_Name_AFRMapWnd_Y(_T("AFRMapWnd_Y"))
, m_Name_CrnkMapWnd_X(_T("CrnkMapWnd_X"))
, m_Name_CrnkMapWnd_Y(_T("CrnkMapWnd_Y"))
, m_Name_WrmpMapWnd_X(_T("WrmpMapWnd_X"))
, m_Name_WrmpMapWnd_Y(_T("WrmpMapWnd_Y"))
, m_Name_DeadMapWnd_X(_T("DeadMapWnd_X"))
, m_Name_DeadMapWnd_Y(_T("DeadMapWnd_Y"))
, m_Name_IdlrMapWnd_X(_T("IdlrMapWnd_X"))
, m_Name_IdlrMapWnd_Y(_T("IdlrMapWnd_Y"))
, m_Name_IdlcMapWnd_X(_T("IdlcMapWnd_X"))
, m_Name_IdlcMapWnd_Y(_T("IdlcMapWnd_Y"))
, m_Name_ATSCurvMapWnd_X(_T("ATSCurvMapWnd_X"))
, m_Name_ATSCurvMapWnd_Y(_T("ATSCurvMapWnd_Y"))
, m_Name_ATSCorrMapWnd_X(_T("ATSCorrMapWnd_X"))
, m_Name_ATSCorrMapWnd_Y(_T("ATSCorrMapWnd_Y"))
//fixtures
, m_Name_Fixtures_Section("Fixtures")
, m_Name_Tachometer_Max(_T("Tachometer_Max"))
, m_Name_Pressure_Max(_T("Pressure_Max"))
, m_Name_PulsesPer1Km(_T("PulsesPer1Km"))
{
 //заполняем базу данных допустимых скоростей для COM-порта
 m_AllowableBaudRates.push_back(CBR_600);
 m_AllowableBaudRates.push_back(CBR_1200);
 m_AllowableBaudRates.push_back(CBR_2400);
 m_AllowableBaudRates.push_back(CBR_4800);
 m_AllowableBaudRates.push_back(CBR_9600);
 m_AllowableBaudRates.push_back(CBR_14400);
 m_AllowableBaudRates.push_back(CBR_19200);
 m_AllowableBaudRates.push_back(CBR_38400);
 m_AllowableBaudRates.push_back(CBR_56000);
 m_AllowableBaudRates.push_back(CBR_57600);
 m_AllowableBaudRates.push_back(CBR_115200);
 m_AllowableBaudRates.push_back(    230400);
 m_AllowableBaudRates.push_back(    250000);
 m_AllowableBaudRates.push_back(CBR_256000);

 //заполняем базу данных допустимых COM-портов
 for(int i = 1; i < 256; i++)
 {
  CString str;
  str.Format(_T("COM%d"),i);
  m_AllowablePorts.push_back(_TSTRING(str));
 }

 _tcscpy(m_current_directory,_T(""));

 //определение тек. директории
 GetCurrentDirectory(MAX_PATH, m_current_directory);

 m_AllowaleCSVSepSymbols.push_back(std::make_pair(_TSTRING(_T("\",\"  comma")),     ','));
 m_AllowaleCSVSepSymbols.push_back(std::make_pair(_TSTRING(_T("\";\"  semicolon")), ';'));
 m_AllowaleCSVSepSymbols.push_back(std::make_pair(_TSTRING(_T("\":\"  colon")),     ':'));
 m_AllowaleCSVSepSymbols.push_back(std::make_pair(_TSTRING(_T("\"+\"  plus")),      '+'));
 m_AllowaleCSVSepSymbols.push_back(std::make_pair(_TSTRING(_T("\"-\"  minus")),     '-'));

 m_AllowableLanguages.push_back(std::make_pair(std::make_pair(_TSTRING(_T("English")), _TSTRING(_T("english"))), IL_ENGLISH) );
 m_AllowableLanguages.push_back(std::make_pair(std::make_pair(_TSTRING(_T("Russian")), _TSTRING(_T("russian"))), IL_RUSSIAN) );

 m_AllowablePlatforms.push_back(std::make_pair(std::make_pair(_TSTRING(_T("ATMega16")), _TSTRING(_T("atmega16"))), EP_ATMEGA16) );
 m_AllowablePlatforms.push_back(std::make_pair(std::make_pair(_TSTRING(_T("ATMega32")), _TSTRING(_T("atmega32"))), EP_ATMEGA32) );
 m_AllowablePlatforms.push_back(std::make_pair(std::make_pair(_TSTRING(_T("ATMega64")), _TSTRING(_T("atmega64"))), EP_ATMEGA64) );
 m_AllowablePlatforms.push_back(std::make_pair(std::make_pair(_TSTRING(_T("ATMega644")), _TSTRING(_T("atmega644"))), EP_ATMEGA644) );
 m_AllowablePlatforms.push_back(std::make_pair(std::make_pair(_TSTRING(_T("ATMega128")), _TSTRING(_T("atmega128"))), EP_ATMEGA128) );
}

CAppSettingsModel::~CAppSettingsModel()
{
 //empty
}

CString CAppSettingsModel::GetINIFileFullName(void) const
{
 CString directory(m_current_directory);
 if (directory.IsEmpty())
  return _T("");

 CString last_char = directory.Right(1);
 if (last_char != _T("\\")) //если корневой каталог, то '\' уже есть
  directory+=_T("\\");

 return directory + AfxGetApp()->m_pszExeName + _T(".ini");
}

CString CAppSettingsModel::GetAppDirectory(void) const
{
 return m_current_directory;
}

bool CAppSettingsModel::CheckAllowableBaudRate(DWORD baud)
{
 std::vector<DWORD>::iterator it;
 it = std::find(m_AllowableBaudRates.begin(),m_AllowableBaudRates.end(),baud);
 if (it != m_AllowableBaudRates.end())
  return true;
 return false; //invalid baud rate
}

bool CAppSettingsModel::CheckAllowableCSVSepSymbol(char i_symbol)
{
 size_t count = m_AllowaleCSVSepSymbols.size();
 for(size_t i = 0; i < count; i++)
  if (i_symbol == m_AllowaleCSVSepSymbols[i].second)
   return true;
 return false;
}

bool CAppSettingsModel::CheckAllowableLanguage(const _TSTRING& i_string, EInterLang& o_language_id)
{
 size_t count = m_AllowableLanguages.size();
 for(size_t i = 0; i < count; ++i)
  if (i_string == m_AllowableLanguages[i].first.second)
  {
   o_language_id = (EInterLang)m_AllowableLanguages[i].second;
   return true;
  }
 return false;
}

bool CAppSettingsModel::CheckAllowablePlatform(const _TSTRING& i_string, EECUPlatform& o_platform_id)
{
 size_t count = m_AllowablePlatforms.size();
 for(size_t i = 0; i < count; ++i)
  if (i_string == m_AllowablePlatforms[i].first.second)
  {
   o_platform_id = (EECUPlatform)m_AllowablePlatforms[i].second;
   return true;
  }
 return false;
}


bool CAppSettingsModel::ReadSettings(void)
{
 CString IniFileName = GetINIFileFullName();
 bool status = true;
 TCHAR read_str[1024];

 int i_val = 0;  float f_val = 0;

 //-----------------------------------------
 GetPrivateProfileString(m_Name_Options_Section,m_Name_PortName,_T("COM1"),read_str,255,IniFileName);
 m_optPortName = read_str;

 //-----------------------------------------
 GetPrivateProfileString(m_Name_Options_Section,m_Name_BaudRateApplication,_T("57600"),read_str,255,IniFileName);
 i_val = _ttoi(read_str);

 if (!CheckAllowableBaudRate(i_val))
 {
  status = false;
  m_optBaudRateApplication = 57600;
 }
 else
 {
  m_optBaudRateApplication = i_val;
 }

 //-----------------------------------------
 GetPrivateProfileString(m_Name_Options_Section,m_Name_BaudRateBootloader,_T("57600"),read_str,255,IniFileName);
 i_val = _ttoi(read_str);

 if (!CheckAllowableBaudRate(i_val))
 {
  status = false;
  m_optBaudRateBootloader = 57600;
 }
 else
 {
  m_optBaudRateBootloader = i_val;
 }

 //-----------------------------------------
 CString def_value = m_current_directory;
 GetPrivateProfileString(m_Name_Options_Section,m_Name_LogFilesFolder,def_value,read_str,MAX_PATH,IniFileName);
 m_optLogFilesFolder = read_str;

 //-----------------------------------------
 GetPrivateProfileString(m_Name_Options_Section,m_Name_UseAppFolder,_T("1"),read_str,255,IniFileName);
 i_val = _ttoi(read_str);

 if (i_val != 0 && i_val != 1)
 {
  status = false;
  m_optUseAppFolder = 0;
 }
 else
 {
  m_optUseAppFolder = i_val;
 }

 //-----------------------------------------
 GetPrivateProfileString(m_Name_Options_Section,m_Name_AlwaysWriteLog,_T("1"),read_str,255,IniFileName);
 i_val = _ttoi(read_str);

 if (i_val != 0 && i_val != 1)
 {
  status = false;
  m_optAlwaysWriteLog = 1;
 }
 else
 {
  m_optAlwaysWriteLog = i_val;
 }

 //-----------------------------------------
 GetPrivateProfileString(m_Name_Options_Section,m_Name_CSVSepSymbol,_T("44"),read_str,255,IniFileName);
 i_val = _ttoi(read_str);

 if (!CheckAllowableCSVSepSymbol(i_val))
 {
  status = false;
  m_optCSVSepSymbol = 44; //comma by default
 }
 else
 {
  m_optCSVSepSymbol = i_val;
 }

 //-----------------------------------------
 GetPrivateProfileString(m_Name_Options_Section,m_Name_MIDeskUpdatePeriod,_T("40"),read_str,255,IniFileName);
 if (_stscanf(read_str, _T("%d"), &i_val) == 1 && i_val >= 0)
  m_optMIDeskUpdatePeriod = i_val;
 else
 { //error
  status = false;
  m_optMIDeskUpdatePeriod = 40;
 }

 //-----------------------------------------
 GetPrivateProfileString(m_Name_Options_Section,m_Name_InterfaceLang,m_AllowableLanguages[0].first.second.c_str(),read_str,255,IniFileName);

 if (!CheckAllowableLanguage(read_str, m_optInterLang))
 {
  status = false;
  m_optInterLang = (EInterLang)m_AllowableLanguages[0].second; //english by default
 }

 //-----------------------------------------
 GetPrivateProfileString(m_Name_Options_Section,m_Name_ECUPlatformType,m_AllowablePlatforms[3].first.second.c_str(),read_str,255,IniFileName);

 if (!CheckAllowablePlatform(read_str, m_optECUPlatformType))
 {
  status = false;
  m_optECUPlatformType = (EECUPlatform)m_AllowablePlatforms[1].second; //atmega32 by default
 }

 //-----------------------------------------
 GetPrivateProfileString(m_Name_Options_Section,m_Name_DVDeskUpdatePeriod,_T("40"),read_str,255,IniFileName);
 if (_stscanf(read_str, _T("%d"), &i_val) == 1 && i_val >= 0)
  m_optDVDeskUpdatePeriod = i_val;
 else
 { //error
  status = false;
  m_optDVDeskUpdatePeriod = 40;
 }

 //-----------------------------------------
 GetPrivateProfileString(m_Name_Options_Section,m_Name_UseDVFeatures,_T("0"),read_str,255,IniFileName);
 i_val = _ttoi(read_str);

 if (i_val != 0 && i_val != 1)
 {
  status = false;
  m_optUseDVFeatures = 0;
 }
 else
 {
  m_optUseDVFeatures = i_val;
 }

 //-----------------------------------------
 GetPrivateProfileString(m_Name_Options_Section,m_Name_ShowToolTips,_T("1"),read_str,255,IniFileName);
 i_val = _ttoi(read_str);

 if (i_val != 0 && i_val != 1)
 {
  status = false;
  m_optShowToolTips = 0;
 }
 else
 {
  m_optShowToolTips = i_val;
 }

//-----------------------------------------
 GetPrivateProfileString(m_Name_Options_Section,m_Name_HexDataMode,_T("0"),read_str,255,IniFileName);
 i_val = _ttoi(read_str);

 if (i_val != 0 && i_val != 1)
 {
  status = false;
  m_optHexDataMode = 0;
 }
 else
 {
  m_optHexDataMode = i_val;
 }

 //-----------------------------------------
 GetPrivateProfileString(m_Name_Fixtures_Section,m_Name_ShowExFixtures,_T("1"),read_str,255,IniFileName);
 i_val = _ttoi(read_str);

 if (i_val != 0 && i_val != 1)
 {
  status = false;
  m_optShowExFixtures = 0;
 }
 else
 {
  m_optShowExFixtures = i_val;
 }

 //Positions of windows
#define _GETWNDPOSITION(sectName, fldName, defVal) \
 {\
  std::vector<TCHAR> strDefVal(32);\
  GetPrivateProfileString((sectName),m_Name_##fldName,_itot((defVal), &strDefVal[0], 10),read_str,255,IniFileName);\
  i_val = _ttoi(read_str);\
  if (i_val < -10000 || i_val > 10000)\
  {\
   status = false;\
   m_opt##fldName = (defVal);\
  }\
  else\
  {\
   m_opt##fldName = i_val;\
  }\
 }

 _GETWNDPOSITION(m_Name_WndSettings_Section, StrtMapWnd_X, std::numeric_limits<int>::max());
 _GETWNDPOSITION(m_Name_WndSettings_Section, StrtMapWnd_Y, std::numeric_limits<int>::max());

 _GETWNDPOSITION(m_Name_WndSettings_Section, IdleMapWnd_X, std::numeric_limits<int>::max());
 _GETWNDPOSITION(m_Name_WndSettings_Section, IdleMapWnd_Y, std::numeric_limits<int>::max());

 _GETWNDPOSITION(m_Name_WndSettings_Section, WorkMapWnd_X, std::numeric_limits<int>::max());
 _GETWNDPOSITION(m_Name_WndSettings_Section, WorkMapWnd_Y, std::numeric_limits<int>::max());

 _GETWNDPOSITION(m_Name_WndSettings_Section, TempMapWnd_X, std::numeric_limits<int>::max());
 _GETWNDPOSITION(m_Name_WndSettings_Section, TempMapWnd_Y, std::numeric_limits<int>::max());

 _GETWNDPOSITION(m_Name_WndSettings_Section, AttenMapWnd_X, std::numeric_limits<int>::max());
 _GETWNDPOSITION(m_Name_WndSettings_Section, AttenMapWnd_Y, std::numeric_limits<int>::max());

 //Main frame window
 _GETWNDPOSITION(m_Name_WndSettings_Section, MainFrmWnd_X, std::numeric_limits<int>::max());
 _GETWNDPOSITION(m_Name_WndSettings_Section, MainFrmWnd_Y, std::numeric_limits<int>::max());

 _GETWNDPOSITION(m_Name_WndSettings_Section, DwellCntrlMapWnd_X, std::numeric_limits<int>::max());
 _GETWNDPOSITION(m_Name_WndSettings_Section, DwellCntrlMapWnd_Y, std::numeric_limits<int>::max());

 _GETWNDPOSITION(m_Name_WndSettings_Section, CTSCurveMapWnd_X, std::numeric_limits<int>::max());
 _GETWNDPOSITION(m_Name_WndSettings_Section, CTSCurveMapWnd_Y, std::numeric_limits<int>::max());

 _GETWNDPOSITION(m_Name_WndSettings_Section, ChokeOpMapWnd_X, std::numeric_limits<int>::max());
 _GETWNDPOSITION(m_Name_WndSettings_Section, ChokeOpMapWnd_Y, std::numeric_limits<int>::max());

 _GETWNDPOSITION(m_Name_WndSettings_Section, GridMapWnd_X, std::numeric_limits<int>::max());
 _GETWNDPOSITION(m_Name_WndSettings_Section, GridMapWnd_Y, std::numeric_limits<int>::max());

 _GETWNDPOSITION(m_Name_WndSettings_Section, VEMapWnd_X, std::numeric_limits<int>::max());
 _GETWNDPOSITION(m_Name_WndSettings_Section, VEMapWnd_Y, std::numeric_limits<int>::max());

 _GETWNDPOSITION(m_Name_WndSettings_Section, AFRMapWnd_X, std::numeric_limits<int>::max());
 _GETWNDPOSITION(m_Name_WndSettings_Section, AFRMapWnd_Y, std::numeric_limits<int>::max());

 _GETWNDPOSITION(m_Name_WndSettings_Section, CrnkMapWnd_X, std::numeric_limits<int>::max());
 _GETWNDPOSITION(m_Name_WndSettings_Section, CrnkMapWnd_Y, std::numeric_limits<int>::max());

 _GETWNDPOSITION(m_Name_WndSettings_Section, WrmpMapWnd_X, std::numeric_limits<int>::max());
 _GETWNDPOSITION(m_Name_WndSettings_Section, WrmpMapWnd_Y, std::numeric_limits<int>::max());

 _GETWNDPOSITION(m_Name_WndSettings_Section, DeadMapWnd_X, std::numeric_limits<int>::max());
 _GETWNDPOSITION(m_Name_WndSettings_Section, DeadMapWnd_Y, std::numeric_limits<int>::max());

 _GETWNDPOSITION(m_Name_WndSettings_Section, IdlrMapWnd_X, std::numeric_limits<int>::max());
 _GETWNDPOSITION(m_Name_WndSettings_Section, IdlrMapWnd_Y, std::numeric_limits<int>::max());

 _GETWNDPOSITION(m_Name_WndSettings_Section, IdlcMapWnd_X, std::numeric_limits<int>::max());
 _GETWNDPOSITION(m_Name_WndSettings_Section, IdlcMapWnd_Y, std::numeric_limits<int>::max());

 _GETWNDPOSITION(m_Name_WndSettings_Section, ATSCurvMapWnd_X, std::numeric_limits<int>::max());
 _GETWNDPOSITION(m_Name_WndSettings_Section, ATSCurvMapWnd_Y, std::numeric_limits<int>::max());

 _GETWNDPOSITION(m_Name_WndSettings_Section, ATSCorrMapWnd_X, std::numeric_limits<int>::max());
 _GETWNDPOSITION(m_Name_WndSettings_Section, ATSCorrMapWnd_Y, std::numeric_limits<int>::max());

 //-----------------------------------------
 GetPrivateProfileString(m_Name_Fixtures_Section,m_Name_Tachometer_Max,_T("8000"),read_str,255,IniFileName);
 if (_stscanf(read_str, _T("%d"), &i_val) == 1 && i_val >= 0 && i_val <= 15000)
  m_optTachometerMax = i_val;
 else
 { //error
  status = false;
  m_optTachometerMax = 8000;
 }

 //-----------------------------------------
 GetPrivateProfileString(m_Name_Fixtures_Section,m_Name_Pressure_Max,_T("110"),read_str,255,IniFileName);
 if (_stscanf(read_str, _T("%d"), &i_val) == 1 && i_val >= 0 && i_val <= 500)
  m_optPressureMax = i_val;
 else
 { //error
  status = false;
  m_optPressureMax = 110;
 }

 //-----------------------------------------
 GetPrivateProfileString(m_Name_Fixtures_Section,m_Name_PulsesPer1Km,_T("6000"),read_str,255,IniFileName);
 if (_stscanf(read_str, _T("%d"), &i_val) == 1 && i_val >= 0 && i_val <= 60000)
  m_optPulsesPer1Km = i_val;
 else
 { //error
  status = false;
  m_optPulsesPer1Km = 6000;
 }

 return status;
}

bool CAppSettingsModel::WriteSettings(void)
{
 CString IniFileName = GetINIFileFullName();
 bool status = true;
 CString write_str;

 //пересоздание секции (или создание заново)
 WritePrivateProfileSection(m_Name_Options_Section,_T(""),IniFileName);

 //-----------------------------------------
 write_str = m_optPortName.c_str();
 WritePrivateProfileString(m_Name_Options_Section,m_Name_PortName,write_str,IniFileName);

 //-----------------------------------------
 write_str.Format(_T("%d"),m_optBaudRateApplication);
 WritePrivateProfileString(m_Name_Options_Section,m_Name_BaudRateApplication,write_str,IniFileName);

 //-----------------------------------------
 write_str.Format(_T("%d"),m_optBaudRateBootloader);
 WritePrivateProfileString(m_Name_Options_Section,m_Name_BaudRateBootloader,write_str,IniFileName);

 //-----------------------------------------
 write_str = m_optLogFilesFolder;
 WritePrivateProfileString(m_Name_Options_Section,m_Name_LogFilesFolder,write_str,IniFileName);

 //-----------------------------------------
 write_str.Format(_T("%d"),(int)m_optUseAppFolder);
 WritePrivateProfileString(m_Name_Options_Section,m_Name_UseAppFolder,write_str,IniFileName);

 //-----------------------------------------
 write_str.Format(_T("%d"),(int)m_optAlwaysWriteLog);
 WritePrivateProfileString(m_Name_Options_Section,m_Name_AlwaysWriteLog,write_str,IniFileName);

 //-----------------------------------------
 write_str.Format(_T("%d"),(int)m_optCSVSepSymbol);
 WritePrivateProfileString(m_Name_Options_Section,m_Name_CSVSepSymbol,write_str,IniFileName);

 //-----------------------------------------
 write_str.Format(_T("%d"),(int)m_optMIDeskUpdatePeriod);
 WritePrivateProfileString(m_Name_Options_Section,m_Name_MIDeskUpdatePeriod,write_str,IniFileName);

 //-----------------------------------------
 size_t i;
 for(i = 0; i < m_AllowableLanguages.size(); ++i)
  if (m_optInterLang == m_AllowableLanguages[i].second)
   write_str = m_AllowableLanguages[i].first.second.c_str();
 WritePrivateProfileString(m_Name_Options_Section,m_Name_InterfaceLang,write_str,IniFileName);

 //-----------------------------------------
 for(i = 0; i < m_AllowablePlatforms.size(); ++i)
  if (m_optECUPlatformType == m_AllowablePlatforms[i].second)
   write_str = m_AllowablePlatforms[i].first.second.c_str();
 WritePrivateProfileString(m_Name_Options_Section,m_Name_ECUPlatformType,write_str,IniFileName);

 //-----------------------------------------
 write_str.Format(_T("%d"),(int)m_optUseDVFeatures);
 WritePrivateProfileString(m_Name_Options_Section,m_Name_UseDVFeatures,write_str,IniFileName);

 //-----------------------------------------
 write_str.Format(_T("%d"),(int)m_optDVDeskUpdatePeriod);
 WritePrivateProfileString(m_Name_Options_Section,m_Name_DVDeskUpdatePeriod,write_str,IniFileName);

 //-----------------------------------------
 write_str.Format(_T("%d"),(int)m_optShowToolTips);
 WritePrivateProfileString(m_Name_Options_Section,m_Name_ShowToolTips,write_str,IniFileName);

 //-----------------------------------------
 write_str.Format(_T("%d"),(int)m_optHexDataMode);
 WritePrivateProfileString(m_Name_Options_Section,m_Name_HexDataMode,write_str,IniFileName);

 //-----------------------------------------
 write_str.Format(_T("%d"),(int)m_optShowExFixtures);
 WritePrivateProfileString(m_Name_Fixtures_Section,m_Name_ShowExFixtures,write_str,IniFileName);

 //Positions of windows
 WritePrivateProfileSection(m_Name_WndSettings_Section,_T(""),IniFileName);

 write_str.Format(_T("%d"),m_optStrtMapWnd_X);
 WritePrivateProfileString(m_Name_WndSettings_Section,m_Name_StrtMapWnd_X,write_str,IniFileName);

 write_str.Format(_T("%d"),m_optStrtMapWnd_Y);
 WritePrivateProfileString(m_Name_WndSettings_Section,m_Name_StrtMapWnd_Y,write_str,IniFileName);

 write_str.Format(_T("%d"),m_optIdleMapWnd_X);
 WritePrivateProfileString(m_Name_WndSettings_Section,m_Name_IdleMapWnd_X,write_str,IniFileName);

 write_str.Format(_T("%d"),m_optIdleMapWnd_Y);
 WritePrivateProfileString(m_Name_WndSettings_Section,m_Name_IdleMapWnd_Y,write_str,IniFileName);

 write_str.Format(_T("%d"),m_optWorkMapWnd_X);
 WritePrivateProfileString(m_Name_WndSettings_Section,m_Name_WorkMapWnd_X,write_str,IniFileName);

 write_str.Format(_T("%d"),m_optWorkMapWnd_Y);
 WritePrivateProfileString(m_Name_WndSettings_Section,m_Name_WorkMapWnd_Y,write_str,IniFileName);

 write_str.Format(_T("%d"),m_optTempMapWnd_X);
 WritePrivateProfileString(m_Name_WndSettings_Section,m_Name_TempMapWnd_X,write_str,IniFileName);

 write_str.Format(_T("%d"),m_optTempMapWnd_Y);
 WritePrivateProfileString(m_Name_WndSettings_Section,m_Name_TempMapWnd_Y,write_str,IniFileName);

 write_str.Format(_T("%d"),m_optAttenMapWnd_X);
 WritePrivateProfileString(m_Name_WndSettings_Section,m_Name_AttenMapWnd_X,write_str,IniFileName);

 write_str.Format(_T("%d"),m_optAttenMapWnd_Y);
 WritePrivateProfileString(m_Name_WndSettings_Section,m_Name_AttenMapWnd_Y,write_str,IniFileName);

 write_str.Format(_T("%d"),m_optMainFrmWnd_X);
 WritePrivateProfileString(m_Name_WndSettings_Section,m_Name_MainFrmWnd_X,write_str,IniFileName);

 write_str.Format(_T("%d"),m_optMainFrmWnd_Y);
 WritePrivateProfileString(m_Name_WndSettings_Section,m_Name_MainFrmWnd_Y,write_str,IniFileName);

 write_str.Format(_T("%d"),m_optDwellCntrlMapWnd_X);
 WritePrivateProfileString(m_Name_WndSettings_Section,m_Name_DwellCntrlMapWnd_X,write_str,IniFileName);

 write_str.Format(_T("%d"),m_optDwellCntrlMapWnd_Y);
 WritePrivateProfileString(m_Name_WndSettings_Section,m_Name_DwellCntrlMapWnd_Y,write_str,IniFileName);

 write_str.Format(_T("%d"),m_optCTSCurveMapWnd_X);
 WritePrivateProfileString(m_Name_WndSettings_Section,m_Name_CTSCurveMapWnd_X,write_str,IniFileName);

 write_str.Format(_T("%d"),m_optCTSCurveMapWnd_Y);
 WritePrivateProfileString(m_Name_WndSettings_Section,m_Name_CTSCurveMapWnd_Y,write_str,IniFileName);

 write_str.Format(_T("%d"),m_optChokeOpMapWnd_X);
 WritePrivateProfileString(m_Name_WndSettings_Section,m_Name_ChokeOpMapWnd_X,write_str,IniFileName);

 write_str.Format(_T("%d"),m_optChokeOpMapWnd_Y);
 WritePrivateProfileString(m_Name_WndSettings_Section,m_Name_ChokeOpMapWnd_Y,write_str,IniFileName);

 write_str.Format(_T("%d"),m_optGridMapWnd_X);
 WritePrivateProfileString(m_Name_WndSettings_Section,m_Name_GridMapWnd_X,write_str,IniFileName);

 write_str.Format(_T("%d"),m_optGridMapWnd_Y);
 WritePrivateProfileString(m_Name_WndSettings_Section,m_Name_GridMapWnd_Y,write_str,IniFileName);

 write_str.Format(_T("%d"),m_optVEMapWnd_X);
 WritePrivateProfileString(m_Name_WndSettings_Section,m_Name_VEMapWnd_X,write_str,IniFileName);

 write_str.Format(_T("%d"),m_optVEMapWnd_Y);
 WritePrivateProfileString(m_Name_WndSettings_Section,m_Name_VEMapWnd_Y,write_str,IniFileName);

 write_str.Format(_T("%d"),m_optAFRMapWnd_X);
 WritePrivateProfileString(m_Name_WndSettings_Section,m_Name_AFRMapWnd_X,write_str,IniFileName);

 write_str.Format(_T("%d"),m_optAFRMapWnd_Y);
 WritePrivateProfileString(m_Name_WndSettings_Section,m_Name_AFRMapWnd_Y,write_str,IniFileName);

 write_str.Format(_T("%d"),m_optCrnkMapWnd_X);
 WritePrivateProfileString(m_Name_WndSettings_Section,m_Name_CrnkMapWnd_X,write_str,IniFileName);

 write_str.Format(_T("%d"),m_optCrnkMapWnd_Y);
 WritePrivateProfileString(m_Name_WndSettings_Section,m_Name_CrnkMapWnd_Y,write_str,IniFileName);

 write_str.Format(_T("%d"),m_optWrmpMapWnd_X);
 WritePrivateProfileString(m_Name_WndSettings_Section,m_Name_WrmpMapWnd_X,write_str,IniFileName);

 write_str.Format(_T("%d"),m_optWrmpMapWnd_Y);
 WritePrivateProfileString(m_Name_WndSettings_Section,m_Name_WrmpMapWnd_Y,write_str,IniFileName);

 write_str.Format(_T("%d"),m_optDeadMapWnd_X);
 WritePrivateProfileString(m_Name_WndSettings_Section,m_Name_DeadMapWnd_X,write_str,IniFileName);

 write_str.Format(_T("%d"),m_optDeadMapWnd_Y);
 WritePrivateProfileString(m_Name_WndSettings_Section,m_Name_DeadMapWnd_Y,write_str,IniFileName);

 write_str.Format(_T("%d"),m_optIdlrMapWnd_X);
 WritePrivateProfileString(m_Name_WndSettings_Section,m_Name_IdlrMapWnd_X,write_str,IniFileName);

 write_str.Format(_T("%d"),m_optIdlrMapWnd_Y);
 WritePrivateProfileString(m_Name_WndSettings_Section,m_Name_IdlrMapWnd_Y,write_str,IniFileName);

 write_str.Format(_T("%d"),m_optIdlcMapWnd_X);
 WritePrivateProfileString(m_Name_WndSettings_Section,m_Name_IdlcMapWnd_X,write_str,IniFileName);

 write_str.Format(_T("%d"),m_optIdlcMapWnd_Y);
 WritePrivateProfileString(m_Name_WndSettings_Section,m_Name_IdlcMapWnd_Y,write_str,IniFileName);

 write_str.Format(_T("%d"),m_optATSCurvMapWnd_X);
 WritePrivateProfileString(m_Name_WndSettings_Section,m_Name_ATSCurvMapWnd_X,write_str,IniFileName);

 write_str.Format(_T("%d"),m_optATSCurvMapWnd_Y);
 WritePrivateProfileString(m_Name_WndSettings_Section,m_Name_ATSCurvMapWnd_Y,write_str,IniFileName);

 write_str.Format(_T("%d"),m_optATSCorrMapWnd_X);
 WritePrivateProfileString(m_Name_WndSettings_Section,m_Name_ATSCorrMapWnd_X,write_str,IniFileName);

 write_str.Format(_T("%d"),m_optATSCorrMapWnd_Y);
 WritePrivateProfileString(m_Name_WndSettings_Section,m_Name_ATSCorrMapWnd_Y,write_str,IniFileName);

 //-----------------------------------------
 write_str.Format(_T("%d"),(int)m_optTachometerMax);
 WritePrivateProfileString(m_Name_Fixtures_Section,m_Name_Tachometer_Max,write_str,IniFileName);

 write_str.Format(_T("%d"),(int)m_optPressureMax);
 WritePrivateProfileString(m_Name_Fixtures_Section,m_Name_Pressure_Max,write_str,IniFileName);

 write_str.Format(_T("%d"),(int)m_optPulsesPer1Km);
 WritePrivateProfileString(m_Name_Fixtures_Section,m_Name_PulsesPer1Km,write_str,IniFileName);

 return status;
}

const _TSTRING& CAppSettingsModel::GetPortName(void) const
{
 return m_optPortName;
}

DWORD CAppSettingsModel::GetBaudRateApplication(void) const
{
 return m_optBaudRateApplication;
}

DWORD CAppSettingsModel::GetBaudRateBootloader(void) const
{
 return m_optBaudRateBootloader;
}

const CString& CAppSettingsModel::GetLogFilesFolder(void) const
{
 return m_optLogFilesFolder;
}

bool CAppSettingsModel::GetUseAppFolder(void) const
{
 return m_optUseAppFolder;
}

bool CAppSettingsModel::GetAlwaysWriteLog(void) const
{
 return m_optAlwaysWriteLog;
}

char CAppSettingsModel::GetCSVSepSymbol(void) const
{
 return m_optCSVSepSymbol;
}

int  CAppSettingsModel::GetMIDeskUpdatePeriod(void) const
{
 return m_optMIDeskUpdatePeriod;
}

void CAppSettingsModel::SetWndSettings(const WndSettings& i_wndSettings)
{
 m_optStrtMapWnd_X = i_wndSettings.m_StrtMapWnd_X;
 m_optStrtMapWnd_Y = i_wndSettings.m_StrtMapWnd_Y;
 m_optIdleMapWnd_X = i_wndSettings.m_IdleMapWnd_X;
 m_optIdleMapWnd_Y = i_wndSettings.m_IdleMapWnd_Y;
 m_optWorkMapWnd_X = i_wndSettings.m_WorkMapWnd_X;
 m_optWorkMapWnd_Y = i_wndSettings.m_WorkMapWnd_Y;
 m_optTempMapWnd_X = i_wndSettings.m_TempMapWnd_X;
 m_optTempMapWnd_Y = i_wndSettings.m_TempMapWnd_Y;
 m_optAttenMapWnd_X = i_wndSettings.m_AttenuatorMapWnd_X;
 m_optAttenMapWnd_Y = i_wndSettings.m_AttenuatorMapWnd_Y;
 m_optMainFrmWnd_X = i_wndSettings.m_MainFrmWnd_X;
 m_optMainFrmWnd_Y = i_wndSettings.m_MainFrmWnd_Y;
 m_optDwellCntrlMapWnd_X = i_wndSettings.m_DwellCntrlMapWnd_X;
 m_optDwellCntrlMapWnd_Y = i_wndSettings.m_DwellCntrlMapWnd_Y;
 m_optCTSCurveMapWnd_X = i_wndSettings.m_CTSCurveMapWnd_X;
 m_optCTSCurveMapWnd_Y = i_wndSettings.m_CTSCurveMapWnd_Y;
 m_optChokeOpMapWnd_X = i_wndSettings.m_ChokeOpMapWnd_X;
 m_optChokeOpMapWnd_Y = i_wndSettings.m_ChokeOpMapWnd_Y;
 m_optGridMapWnd_X = i_wndSettings.m_GridMapWnd_X;
 m_optGridMapWnd_Y = i_wndSettings.m_GridMapWnd_Y;
 m_optVEMapWnd_X = i_wndSettings.m_VEMapWnd_X;
 m_optVEMapWnd_Y = i_wndSettings.m_VEMapWnd_Y;
 m_optAFRMapWnd_X = i_wndSettings.m_AFRMapWnd_X;
 m_optAFRMapWnd_Y = i_wndSettings.m_AFRMapWnd_Y;
 m_optCrnkMapWnd_X = i_wndSettings.m_CrnkMapWnd_X;
 m_optCrnkMapWnd_Y = i_wndSettings.m_CrnkMapWnd_Y;
 m_optWrmpMapWnd_X = i_wndSettings.m_WrmpMapWnd_X;
 m_optWrmpMapWnd_Y = i_wndSettings.m_WrmpMapWnd_Y;
 m_optDeadMapWnd_X = i_wndSettings.m_DeadMapWnd_X;
 m_optDeadMapWnd_Y = i_wndSettings.m_DeadMapWnd_Y;
 m_optIdlrMapWnd_X = i_wndSettings.m_IdlrMapWnd_X;
 m_optIdlrMapWnd_Y = i_wndSettings.m_IdlrMapWnd_Y;
 m_optIdlcMapWnd_X = i_wndSettings.m_IdlcMapWnd_X;
 m_optIdlcMapWnd_Y = i_wndSettings.m_IdlcMapWnd_Y;
 m_optATSCurvMapWnd_X = i_wndSettings.m_ATSCurvMapWnd_X;
 m_optATSCurvMapWnd_Y = i_wndSettings.m_ATSCurvMapWnd_Y;
 m_optATSCorrMapWnd_X = i_wndSettings.m_ATSCorrMapWnd_X;
 m_optATSCorrMapWnd_Y = i_wndSettings.m_ATSCorrMapWnd_Y; 
}

void CAppSettingsModel::GetWndSettings(WndSettings& o_wndSettings) const
{
 o_wndSettings.m_StrtMapWnd_X = m_optStrtMapWnd_X;
 o_wndSettings.m_StrtMapWnd_Y = m_optStrtMapWnd_Y;
 o_wndSettings.m_IdleMapWnd_X = m_optIdleMapWnd_X;
 o_wndSettings.m_IdleMapWnd_Y = m_optIdleMapWnd_Y;
 o_wndSettings.m_WorkMapWnd_X = m_optWorkMapWnd_X;
 o_wndSettings.m_WorkMapWnd_Y = m_optWorkMapWnd_Y;
 o_wndSettings.m_TempMapWnd_X = m_optTempMapWnd_X;
 o_wndSettings.m_TempMapWnd_Y = m_optTempMapWnd_Y;
 o_wndSettings.m_AttenuatorMapWnd_X = m_optAttenMapWnd_X;
 o_wndSettings.m_AttenuatorMapWnd_Y = m_optAttenMapWnd_Y;
 o_wndSettings.m_MainFrmWnd_X = m_optMainFrmWnd_X;
 o_wndSettings.m_MainFrmWnd_Y = m_optMainFrmWnd_Y;
 o_wndSettings.m_DwellCntrlMapWnd_X = m_optDwellCntrlMapWnd_X;
 o_wndSettings.m_DwellCntrlMapWnd_Y = m_optDwellCntrlMapWnd_Y;
 o_wndSettings.m_CTSCurveMapWnd_X = m_optCTSCurveMapWnd_X;
 o_wndSettings.m_CTSCurveMapWnd_Y = m_optCTSCurveMapWnd_Y;
 o_wndSettings.m_ChokeOpMapWnd_X = m_optChokeOpMapWnd_X;
 o_wndSettings.m_ChokeOpMapWnd_Y = m_optChokeOpMapWnd_Y;
 o_wndSettings.m_GridMapWnd_X = m_optGridMapWnd_X;
 o_wndSettings.m_GridMapWnd_Y = m_optGridMapWnd_Y;
 o_wndSettings.m_VEMapWnd_X = m_optVEMapWnd_X;
 o_wndSettings.m_VEMapWnd_Y = m_optVEMapWnd_Y;
 o_wndSettings.m_AFRMapWnd_X = m_optAFRMapWnd_X;
 o_wndSettings.m_AFRMapWnd_Y = m_optAFRMapWnd_Y;
 o_wndSettings.m_CrnkMapWnd_X = m_optCrnkMapWnd_X;
 o_wndSettings.m_CrnkMapWnd_Y = m_optCrnkMapWnd_Y;
 o_wndSettings.m_WrmpMapWnd_X = m_optWrmpMapWnd_X;
 o_wndSettings.m_WrmpMapWnd_Y = m_optWrmpMapWnd_Y;
 o_wndSettings.m_DeadMapWnd_X = m_optDeadMapWnd_X;
 o_wndSettings.m_DeadMapWnd_Y = m_optDeadMapWnd_Y;
 o_wndSettings.m_IdlrMapWnd_X = m_optIdlrMapWnd_X;
 o_wndSettings.m_IdlrMapWnd_Y = m_optIdlrMapWnd_Y;
 o_wndSettings.m_IdlcMapWnd_X = m_optIdlcMapWnd_X;
 o_wndSettings.m_IdlcMapWnd_Y = m_optIdlcMapWnd_Y;
 o_wndSettings.m_ATSCurvMapWnd_X = m_optATSCurvMapWnd_X;
 o_wndSettings.m_ATSCurvMapWnd_Y = m_optATSCurvMapWnd_Y;
 o_wndSettings.m_ATSCorrMapWnd_X = m_optATSCorrMapWnd_X;
 o_wndSettings.m_ATSCorrMapWnd_Y = m_optATSCorrMapWnd_Y;
}

EInterLang CAppSettingsModel::GetInterfaceLanguage(void) const
{
 return m_optInterLang;
}

EECUPlatform CAppSettingsModel::GetECUPlatformType(void) const
{
 return m_optECUPlatformType;
}

int CAppSettingsModel::GetTachometerMax(void) const
{
 return m_optTachometerMax;
}

int CAppSettingsModel::GetPressureMax(void) const
{
 return m_optPressureMax;
}

bool CAppSettingsModel::GetUseDVFeatures(void) const
{
 return m_optUseDVFeatures;
}

int CAppSettingsModel::GetDVDeskUpdatePeriod(void) const
{
 return m_optDVDeskUpdatePeriod;
}

bool CAppSettingsModel::GetShowToolTips(void) const
{
 return m_optShowToolTips;
}

bool CAppSettingsModel::GetShowExFixtures(void) const
{
 return m_optShowExFixtures;
}

bool CAppSettingsModel::GetHexDataMode(void) const
{
 return m_optHexDataMode;
}

int CAppSettingsModel::GetNumPulsesPer1Km(void) const
{
 return m_optPulsesPer1Km;
}
