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

/** \file AppSettingsModel.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "AppSettingsModel.h"

#include <limits>
#include <algorithm>

#undef max //avoid conflicts with C++

#define SECU3_CBR_500000 500000

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
, m_Name_COMPortBother(_T("COMPortBother"))
, m_Name_UseHotKeys(_T("UseHotKeys"))
, m_Name_ShowWelcome(_T("ShowWelcome"))
, m_Name_RPMAverage(_T("RPMAverage"))
, m_Name_VoltAverage(_T("VoltAverage"))
, m_Name_MAPAverage(_T("MAPAverage"))
, m_Name_AI1Average(_T("AI1Average"))
, m_Name_TPSAverage(_T("TPSAverage"))
, m_Name_AllowVisualTheme(_T("AllowVisualTheme"))
, m_Name_AutoDiagEnter(_T("AutoDiagEnter"))
, m_Name_SaveWarning(_T("SaveWarning"))
, m_Name_AutoCERead(_T("AutoCERead"))
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
, m_Name_AETPSMapWnd_X(_T("AETPSMapWnd_X"))
, m_Name_AETPSMapWnd_Y(_T("AETPSMapWnd_Y"))
, m_Name_AERPMMapWnd_X(_T("AERPMMapWnd_X"))
, m_Name_AERPMMapWnd_Y(_T("AERPMMapWnd_Y"))
, m_Name_AftstrMapWnd_X(_T("AftstrMapWnd_X"))
, m_Name_AftstrMapWnd_Y(_T("AftstrMapWnd_Y"))
, m_Name_GasdoseMapWnd_X(_T("GasdoseMapWnd_X"))
, m_Name_GasdoseMapWnd_Y(_T("GasdoseMapWnd_Y"))
, m_Name_ITMapWnd_X(_T("ITMapWnd_X"))
, m_Name_ITMapWnd_Y(_T("ITMapWnd_Y"))
, m_Name_ITRPMMapWnd_X(_T("ITRPMMapWnd_X"))
, m_Name_ITRPMMapWnd_Y(_T("ITRPMMapWnd_Y"))
, m_Name_RigidMapWnd_X(_T("RigidMapWnd_X"))
, m_Name_RigidMapWnd_Y(_T("RigidMapWnd_Y"))
, m_Name_EGOCrvMapWnd_X(_T("EGOCrvMapWnd_X"))
, m_Name_EGOCrvMapWnd_Y(_T("EGOCrvMapWnd_Y"))
, m_Name_IACCMapWnd_X(_T("IACCMapWnd_X"))
, m_Name_IACCMapWnd_Y(_T("IACCMapWnd_Y"))
, m_Name_IACCWMapWnd_X(_T("IACCWMapWnd_X"))
, m_Name_IACCWMapWnd_Y(_T("IACCWMapWnd_Y"))
, m_Name_IATCLTMapWnd_X(_T("IATCLTCorrMapWnd_X"))
, m_Name_IATCLTMapWnd_Y(_T("IATCLTCorrMapWnd_Y"))
, m_Name_BarocorrMapWnd_X(_T("BarocorrMapWnd_X"))
, m_Name_BarocorrMapWnd_Y(_T("BarocorrMapWnd_Y"))
, m_Name_ManIgntimMapWnd_X(_T("ManIgntimMapWnd_X"))
, m_Name_ManIgntimMapWnd_Y(_T("ManIgntimMapWnd_Y"))
, m_Name_CESettingsWnd_X(_T("CESettingsWnd_X"))
, m_Name_CESettingsWnd_Y(_T("CESettingsWnd_Y"))
, m_Name_TpsswtMapWnd_X(_T("TpsSwtMapWnd_X"))
, m_Name_TpsswtMapWnd_Y(_T("TpsSwtMapWnd_Y"))
, m_Name_Tmp2CurveMapWnd_X(_T("Tmp2CurveMapWnd_X"))
, m_Name_Tmp2CurveMapWnd_Y(_T("Tmp2CurveMapWnd_Y"))
, m_Name_GtscMapWnd_X(_T("GTSCMapWnd_X"))
, m_Name_GtscMapWnd_Y(_T("GTSCMapWnd_Y"))
, m_Name_GpscMapWnd_X(_T("GPSCMapWnd_X"))
, m_Name_GpscMapWnd_Y(_T("GPSCMapWnd_Y"))
//fixtures
, m_Name_Fixtures_Section("Fixtures")
, m_Name_Tachometer_Max(_T("Tachometer_Max"))
, m_Name_Pressure_Max(_T("Pressure_Max"))
, m_Name_PulsesPer1Km(_T("PulsesPer1Km"))
, m_Name_SpeedUnit(_T("SpeedUnit"))
{
 //заполняем базу данных допустимых скоростей для COM-порта
 m_AllowableBaudRates.push_back(CBR_9600);
 m_AllowableBaudRates.push_back(CBR_14400);
 m_AllowableBaudRates.push_back(CBR_19200);
 m_AllowableBaudRates.push_back(CBR_38400);
 m_AllowableBaudRates.push_back(CBR_56000);
 m_AllowableBaudRates.push_back(CBR_57600);
 m_AllowableBaudRates.push_back(CBR_115200);
 m_AllowableBaudRates.push_back(CBR_256000);
 m_AllowableBaudRates.push_back(SECU3_CBR_500000);  //not standard
 //fill database with allowed COM-port numbers
 for(int i = 1; i < 256; i++)
 {
  CString str;
  str.Format(_T("COM%d"),i);
  m_AllowablePorts.push_back(_TSTRING(str));
 }

 _tcscpy(m_current_directory,_T(""));

 //get current directory name
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
 GetPrivateProfileString(m_Name_Options_Section,m_Name_COMPortBother,_T("1"),read_str,255,IniFileName);
 i_val = _ttoi(read_str);

 if (i_val != 0 && i_val != 1)
 {
  status = false;
  m_optCOMPortBother = 0;
 }
 else
 {
  m_optCOMPortBother = i_val;
 }

 //-----------------------------------------
 GetPrivateProfileString(m_Name_Options_Section,m_Name_UseHotKeys,_T("1"),read_str,255,IniFileName);
 i_val = _ttoi(read_str);

 if (i_val != 0 && i_val != 1)
 {
  status = false;
  m_optUseHotKeys = 0;
 }
 else
 {
  m_optUseHotKeys = i_val;
 }

 //-----------------------------------------
 GetPrivateProfileString(m_Name_Options_Section,m_Name_ShowWelcome,_T("1"),read_str,255,IniFileName);
 i_val = _ttoi(read_str);

 if (i_val != 0 && i_val != 1)
 {
  status = false;
  m_optShowWelcome = 0;
 }
 else
 {
  m_optShowWelcome = i_val;
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

 //-----------------------------------------
 GetPrivateProfileString(m_Name_Fixtures_Section,m_Name_RPMAverage,_T("4"),read_str,255,IniFileName);
 i_val = _ttoi(read_str);

 if (i_val < 0 || i_val > 16)
 {
  status = false;
  m_optRPMAverage = 0; //no avaraging
 }
 else
 {
  m_optRPMAverage = i_val;
 }

 //-----------------------------------------
 GetPrivateProfileString(m_Name_Fixtures_Section,m_Name_VoltAverage,_T("4"),read_str,255,IniFileName);
 i_val = _ttoi(read_str);

 if (i_val < 0 || i_val > 16)
 {
  status = false;
  m_optVoltAverage = 0; //no avaraging
 }
 else
 {
  m_optVoltAverage = i_val;
 }

 //-----------------------------------------
 GetPrivateProfileString(m_Name_Fixtures_Section,m_Name_MAPAverage,_T("4"),read_str,255,IniFileName);
 i_val = _ttoi(read_str);

 if (i_val < 0 || i_val > 16)
 {
  status = false;
  m_optMAPAverage = 0; //no avaraging
 }
 else
 {
  m_optMAPAverage = i_val;
 }

 //-----------------------------------------
 GetPrivateProfileString(m_Name_Fixtures_Section,m_Name_AI1Average,_T("4"),read_str,255,IniFileName);
 i_val = _ttoi(read_str);

 if (i_val < 0 || i_val > 16)
 {
  status = false;
  m_optAI1Average = 0; //no avaraging
 }
 else
 {
  m_optAI1Average = i_val;
 }

 //-----------------------------------------
 GetPrivateProfileString(m_Name_Fixtures_Section,m_Name_TPSAverage,_T("4"),read_str,255,IniFileName);
 i_val = _ttoi(read_str);

 if (i_val < 0 || i_val > 16)
 {
  status = false;
  m_optTPSAverage = 0; //no avaraging
 }
 else
 {
  m_optTPSAverage = i_val;
 }

 //-----------------------------------------
 GetPrivateProfileString(m_Name_Options_Section,m_Name_AllowVisualTheme,_T("1"),read_str,255,IniFileName);
 i_val = _ttoi(read_str);

 if (i_val != 0 && i_val != 1)
 {
  status = false;
  m_optAllowVisualTheme = 0;
 }
 else
 {
  m_optAllowVisualTheme = i_val;
 }

 //-----------------------------------------
 GetPrivateProfileString(m_Name_Options_Section,m_Name_AutoDiagEnter,_T("0"),read_str,255,IniFileName);
 i_val = _ttoi(read_str);

 if (i_val != 0 && i_val != 1)
 {
  status = false;
  m_optAutoDiagEnter = 0;
 }
 else
 {
  m_optAutoDiagEnter = i_val;
 }

 //-----------------------------------------
 GetPrivateProfileString(m_Name_Options_Section,m_Name_SaveWarning,_T("1"),read_str,255,IniFileName);
 i_val = _ttoi(read_str);

 if (i_val != 0 && i_val != 1)
 {
  status = false;
  m_optSaveWarning = 0;
 }
 else
 {
  m_optSaveWarning = i_val;
 }

 //-----------------------------------------
 GetPrivateProfileString(m_Name_Options_Section,m_Name_AutoCERead,_T("0"),read_str,255,IniFileName);
 i_val = _ttoi(read_str);

 if (i_val != 0 && i_val != 1)
 {
  status = false;
  m_optAutoCERead = 0;
 }
 else
 {
  m_optAutoCERead = i_val;
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

 _GETWNDPOSITION(m_Name_WndSettings_Section, AETPSMapWnd_X, std::numeric_limits<int>::max());
 _GETWNDPOSITION(m_Name_WndSettings_Section, AETPSMapWnd_Y, std::numeric_limits<int>::max());

 _GETWNDPOSITION(m_Name_WndSettings_Section, AERPMMapWnd_X, std::numeric_limits<int>::max());
 _GETWNDPOSITION(m_Name_WndSettings_Section, AERPMMapWnd_Y, std::numeric_limits<int>::max());

 _GETWNDPOSITION(m_Name_WndSettings_Section, AftstrMapWnd_X, std::numeric_limits<int>::max());
 _GETWNDPOSITION(m_Name_WndSettings_Section, AftstrMapWnd_Y, std::numeric_limits<int>::max());

 _GETWNDPOSITION(m_Name_WndSettings_Section, ATSCurvMapWnd_X, std::numeric_limits<int>::max());
 _GETWNDPOSITION(m_Name_WndSettings_Section, ATSCurvMapWnd_Y, std::numeric_limits<int>::max());

 _GETWNDPOSITION(m_Name_WndSettings_Section, ATSCorrMapWnd_X, std::numeric_limits<int>::max());
 _GETWNDPOSITION(m_Name_WndSettings_Section, ATSCorrMapWnd_Y, std::numeric_limits<int>::max());

 _GETWNDPOSITION(m_Name_WndSettings_Section, GasdoseMapWnd_X, std::numeric_limits<int>::max());
 _GETWNDPOSITION(m_Name_WndSettings_Section, GasdoseMapWnd_Y, std::numeric_limits<int>::max());

 _GETWNDPOSITION(m_Name_WndSettings_Section, ITMapWnd_X, std::numeric_limits<int>::max());
 _GETWNDPOSITION(m_Name_WndSettings_Section, ITMapWnd_Y, std::numeric_limits<int>::max());

 _GETWNDPOSITION(m_Name_WndSettings_Section, ITRPMMapWnd_X, std::numeric_limits<int>::max());
 _GETWNDPOSITION(m_Name_WndSettings_Section, ITRPMMapWnd_Y, std::numeric_limits<int>::max());

 _GETWNDPOSITION(m_Name_WndSettings_Section, RigidMapWnd_X, std::numeric_limits<int>::max());
 _GETWNDPOSITION(m_Name_WndSettings_Section, RigidMapWnd_Y, std::numeric_limits<int>::max());

 _GETWNDPOSITION(m_Name_WndSettings_Section, EGOCrvMapWnd_X, std::numeric_limits<int>::max());
 _GETWNDPOSITION(m_Name_WndSettings_Section, EGOCrvMapWnd_Y, std::numeric_limits<int>::max());

 _GETWNDPOSITION(m_Name_WndSettings_Section, IACCMapWnd_X, std::numeric_limits<int>::max());
 _GETWNDPOSITION(m_Name_WndSettings_Section, IACCMapWnd_Y, std::numeric_limits<int>::max());

 _GETWNDPOSITION(m_Name_WndSettings_Section, IACCWMapWnd_X, std::numeric_limits<int>::max());
 _GETWNDPOSITION(m_Name_WndSettings_Section, IACCWMapWnd_Y, std::numeric_limits<int>::max());

 _GETWNDPOSITION(m_Name_WndSettings_Section, IATCLTMapWnd_X, std::numeric_limits<int>::max());
 _GETWNDPOSITION(m_Name_WndSettings_Section, IATCLTMapWnd_Y, std::numeric_limits<int>::max());

 _GETWNDPOSITION(m_Name_WndSettings_Section, BarocorrMapWnd_X, std::numeric_limits<int>::max());
 _GETWNDPOSITION(m_Name_WndSettings_Section, BarocorrMapWnd_Y, std::numeric_limits<int>::max());

 _GETWNDPOSITION(m_Name_WndSettings_Section, ManIgntimMapWnd_X, std::numeric_limits<int>::max());
 _GETWNDPOSITION(m_Name_WndSettings_Section, ManIgntimMapWnd_Y, std::numeric_limits<int>::max());

 _GETWNDPOSITION(m_Name_WndSettings_Section, CESettingsWnd_X, std::numeric_limits<int>::max());
 _GETWNDPOSITION(m_Name_WndSettings_Section, CESettingsWnd_Y, std::numeric_limits<int>::max());

 _GETWNDPOSITION(m_Name_WndSettings_Section, TpsswtMapWnd_X, std::numeric_limits<int>::max());
 _GETWNDPOSITION(m_Name_WndSettings_Section, TpsswtMapWnd_Y, std::numeric_limits<int>::max());

 _GETWNDPOSITION(m_Name_WndSettings_Section, Tmp2CurveMapWnd_X, std::numeric_limits<int>::max());
 _GETWNDPOSITION(m_Name_WndSettings_Section, Tmp2CurveMapWnd_Y, std::numeric_limits<int>::max());

 _GETWNDPOSITION(m_Name_WndSettings_Section, GtscMapWnd_X, std::numeric_limits<int>::max());
 _GETWNDPOSITION(m_Name_WndSettings_Section, GtscMapWnd_Y, std::numeric_limits<int>::max());

 _GETWNDPOSITION(m_Name_WndSettings_Section, GpscMapWnd_X, std::numeric_limits<int>::max());
 _GETWNDPOSITION(m_Name_WndSettings_Section, GpscMapWnd_Y, std::numeric_limits<int>::max());

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

 //-----------------------------------------
 GetPrivateProfileString(m_Name_Fixtures_Section,m_Name_SpeedUnit,_T("kmh"),read_str,255,IniFileName);
 if (_TSTRING(read_str)==_T("kmh"))
  m_optSpeedUnit = SU_KMH;
 else if (_TSTRING(read_str)==_T("mph"))
  m_optSpeedUnit = SU_MPH;
 else
 {
  status = false;
  m_optSpeedUnit = SU_KMH;
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
 write_str.Format(_T("%d"),(int)m_optCOMPortBother);
 WritePrivateProfileString(m_Name_Options_Section,m_Name_COMPortBother,write_str,IniFileName);

 //-----------------------------------------
 write_str.Format(_T("%d"),(int)m_optUseHotKeys);
 WritePrivateProfileString(m_Name_Options_Section,m_Name_UseHotKeys,write_str,IniFileName);

 //-----------------------------------------
 write_str.Format(_T("%d"),(int)/*m_optShowWelcome*/0);
 WritePrivateProfileString(m_Name_Options_Section,m_Name_ShowWelcome,write_str,IniFileName);

 //-----------------------------------------
 write_str.Format(_T("%d"),(int)m_optShowExFixtures);
 WritePrivateProfileString(m_Name_Fixtures_Section,m_Name_ShowExFixtures,write_str,IniFileName);

 //-----------------------------------------
 write_str.Format(_T("%d"),(int)m_optRPMAverage);
 WritePrivateProfileString(m_Name_Fixtures_Section,m_Name_RPMAverage,write_str,IniFileName);

 //-----------------------------------------
 write_str.Format(_T("%d"),(int)m_optVoltAverage);
 WritePrivateProfileString(m_Name_Fixtures_Section,m_Name_VoltAverage,write_str,IniFileName);

 //-----------------------------------------
 write_str.Format(_T("%d"),(int)m_optMAPAverage);
 WritePrivateProfileString(m_Name_Fixtures_Section,m_Name_MAPAverage,write_str,IniFileName);

 //-----------------------------------------
 write_str.Format(_T("%d"),(int)m_optAI1Average);
 WritePrivateProfileString(m_Name_Fixtures_Section,m_Name_AI1Average,write_str,IniFileName);

 //-----------------------------------------
 write_str.Format(_T("%d"),(int)m_optTPSAverage);
 WritePrivateProfileString(m_Name_Fixtures_Section,m_Name_TPSAverage,write_str,IniFileName);

 //-----------------------------------------
 write_str.Format(_T("%d"),(int)m_optAllowVisualTheme);
 WritePrivateProfileString(m_Name_Options_Section,m_Name_AllowVisualTheme,write_str,IniFileName);

 //-----------------------------------------
 write_str.Format(_T("%d"),(int)m_optAutoDiagEnter);
 WritePrivateProfileString(m_Name_Options_Section,m_Name_AutoDiagEnter,write_str,IniFileName);

 //-----------------------------------------
 write_str.Format(_T("%d"),(int)m_optSaveWarning);
 WritePrivateProfileString(m_Name_Options_Section,m_Name_SaveWarning,write_str,IniFileName);

 //-----------------------------------------
 write_str.Format(_T("%d"),(int)m_optAutoCERead);
 WritePrivateProfileString(m_Name_Options_Section,m_Name_AutoCERead,write_str,IniFileName);

 //-----------------------------------------

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

 write_str.Format(_T("%d"),m_optAETPSMapWnd_X);
 WritePrivateProfileString(m_Name_WndSettings_Section,m_Name_AETPSMapWnd_X,write_str,IniFileName);

 write_str.Format(_T("%d"),m_optAETPSMapWnd_Y);
 WritePrivateProfileString(m_Name_WndSettings_Section,m_Name_AETPSMapWnd_Y,write_str,IniFileName);

 write_str.Format(_T("%d"),m_optAERPMMapWnd_X);
 WritePrivateProfileString(m_Name_WndSettings_Section,m_Name_AERPMMapWnd_X,write_str,IniFileName);

 write_str.Format(_T("%d"),m_optAERPMMapWnd_Y);
 WritePrivateProfileString(m_Name_WndSettings_Section,m_Name_AERPMMapWnd_Y,write_str,IniFileName);

 write_str.Format(_T("%d"),m_optAftstrMapWnd_X);
 WritePrivateProfileString(m_Name_WndSettings_Section,m_Name_AftstrMapWnd_X,write_str,IniFileName);

 write_str.Format(_T("%d"),m_optAftstrMapWnd_Y);
 WritePrivateProfileString(m_Name_WndSettings_Section,m_Name_AftstrMapWnd_Y,write_str,IniFileName);

 write_str.Format(_T("%d"),m_optATSCurvMapWnd_X);
 WritePrivateProfileString(m_Name_WndSettings_Section,m_Name_ATSCurvMapWnd_X,write_str,IniFileName);

 write_str.Format(_T("%d"),m_optATSCurvMapWnd_Y);
 WritePrivateProfileString(m_Name_WndSettings_Section,m_Name_ATSCurvMapWnd_Y,write_str,IniFileName);

 write_str.Format(_T("%d"),m_optATSCorrMapWnd_X);
 WritePrivateProfileString(m_Name_WndSettings_Section,m_Name_ATSCorrMapWnd_X,write_str,IniFileName);

 write_str.Format(_T("%d"),m_optATSCorrMapWnd_Y);
 WritePrivateProfileString(m_Name_WndSettings_Section,m_Name_ATSCorrMapWnd_Y,write_str,IniFileName);

 write_str.Format(_T("%d"),m_optGasdoseMapWnd_X);
 WritePrivateProfileString(m_Name_WndSettings_Section,m_Name_GasdoseMapWnd_X,write_str,IniFileName);

 write_str.Format(_T("%d"),m_optGasdoseMapWnd_Y);
 WritePrivateProfileString(m_Name_WndSettings_Section,m_Name_GasdoseMapWnd_Y,write_str,IniFileName);

 write_str.Format(_T("%d"),m_optITMapWnd_X);
 WritePrivateProfileString(m_Name_WndSettings_Section,m_Name_ITMapWnd_X,write_str,IniFileName);

 write_str.Format(_T("%d"),m_optITMapWnd_Y);
 WritePrivateProfileString(m_Name_WndSettings_Section,m_Name_ITMapWnd_Y,write_str,IniFileName);

 write_str.Format(_T("%d"),m_optITRPMMapWnd_X);
 WritePrivateProfileString(m_Name_WndSettings_Section,m_Name_ITRPMMapWnd_X,write_str,IniFileName);

 write_str.Format(_T("%d"),m_optITRPMMapWnd_Y);
 WritePrivateProfileString(m_Name_WndSettings_Section,m_Name_ITRPMMapWnd_Y,write_str,IniFileName);

 write_str.Format(_T("%d"),m_optRigidMapWnd_X);
 WritePrivateProfileString(m_Name_WndSettings_Section,m_Name_RigidMapWnd_X,write_str,IniFileName);

 write_str.Format(_T("%d"),m_optRigidMapWnd_Y);
 WritePrivateProfileString(m_Name_WndSettings_Section,m_Name_RigidMapWnd_Y,write_str,IniFileName);

 write_str.Format(_T("%d"),m_optEGOCrvMapWnd_X);
 WritePrivateProfileString(m_Name_WndSettings_Section,m_Name_EGOCrvMapWnd_X,write_str,IniFileName);

 write_str.Format(_T("%d"),m_optEGOCrvMapWnd_Y);
 WritePrivateProfileString(m_Name_WndSettings_Section,m_Name_EGOCrvMapWnd_Y,write_str,IniFileName);

 write_str.Format(_T("%d"),m_optIACCMapWnd_X);
 WritePrivateProfileString(m_Name_WndSettings_Section,m_Name_IACCMapWnd_X,write_str,IniFileName);

 write_str.Format(_T("%d"),m_optIACCMapWnd_Y);
 WritePrivateProfileString(m_Name_WndSettings_Section,m_Name_IACCMapWnd_Y,write_str,IniFileName);

 write_str.Format(_T("%d"),m_optIACCWMapWnd_X);
 WritePrivateProfileString(m_Name_WndSettings_Section,m_Name_IACCWMapWnd_X,write_str,IniFileName);

 write_str.Format(_T("%d"),m_optIACCWMapWnd_Y);
 WritePrivateProfileString(m_Name_WndSettings_Section,m_Name_IACCWMapWnd_Y,write_str,IniFileName);

 write_str.Format(_T("%d"),m_optIATCLTMapWnd_X);
 WritePrivateProfileString(m_Name_WndSettings_Section,m_Name_IATCLTMapWnd_X,write_str,IniFileName);

 write_str.Format(_T("%d"),m_optIATCLTMapWnd_Y);
 WritePrivateProfileString(m_Name_WndSettings_Section,m_Name_IATCLTMapWnd_Y,write_str,IniFileName);

 write_str.Format(_T("%d"),m_optBarocorrMapWnd_X);
 WritePrivateProfileString(m_Name_WndSettings_Section,m_Name_BarocorrMapWnd_X,write_str,IniFileName);

 write_str.Format(_T("%d"),m_optBarocorrMapWnd_Y);
 WritePrivateProfileString(m_Name_WndSettings_Section,m_Name_BarocorrMapWnd_Y,write_str,IniFileName);

 write_str.Format(_T("%d"),m_optManIgntimMapWnd_X);
 WritePrivateProfileString(m_Name_WndSettings_Section,m_Name_ManIgntimMapWnd_X,write_str,IniFileName);

 write_str.Format(_T("%d"),m_optManIgntimMapWnd_Y);
 WritePrivateProfileString(m_Name_WndSettings_Section,m_Name_ManIgntimMapWnd_Y,write_str,IniFileName);

 write_str.Format(_T("%d"),m_optCESettingsWnd_X);
 WritePrivateProfileString(m_Name_WndSettings_Section,m_Name_CESettingsWnd_X,write_str,IniFileName);

 write_str.Format(_T("%d"),m_optCESettingsWnd_Y);
 WritePrivateProfileString(m_Name_WndSettings_Section,m_Name_CESettingsWnd_Y,write_str,IniFileName);

 write_str.Format(_T("%d"),m_optTpsswtMapWnd_X);
 WritePrivateProfileString(m_Name_WndSettings_Section,m_Name_TpsswtMapWnd_X,write_str,IniFileName);

 write_str.Format(_T("%d"),m_optTpsswtMapWnd_Y);
 WritePrivateProfileString(m_Name_WndSettings_Section,m_Name_TpsswtMapWnd_Y,write_str,IniFileName);

 write_str.Format(_T("%d"),m_optTmp2CurveMapWnd_X);
 WritePrivateProfileString(m_Name_WndSettings_Section,m_Name_Tmp2CurveMapWnd_X,write_str,IniFileName);

 write_str.Format(_T("%d"),m_optTmp2CurveMapWnd_Y);
 WritePrivateProfileString(m_Name_WndSettings_Section,m_Name_Tmp2CurveMapWnd_Y,write_str,IniFileName);

 write_str.Format(_T("%d"),m_optGtscMapWnd_X);
 WritePrivateProfileString(m_Name_WndSettings_Section,m_Name_GtscMapWnd_X,write_str,IniFileName);

 write_str.Format(_T("%d"),m_optGtscMapWnd_Y);
 WritePrivateProfileString(m_Name_WndSettings_Section,m_Name_GtscMapWnd_Y,write_str,IniFileName);

 write_str.Format(_T("%d"),m_optGpscMapWnd_X);
 WritePrivateProfileString(m_Name_WndSettings_Section,m_Name_GpscMapWnd_X,write_str,IniFileName);

 write_str.Format(_T("%d"),m_optGpscMapWnd_Y);
 WritePrivateProfileString(m_Name_WndSettings_Section,m_Name_GpscMapWnd_Y,write_str,IniFileName);

 //-----------------------------------------
 write_str.Format(_T("%d"),(int)m_optTachometerMax);
 WritePrivateProfileString(m_Name_Fixtures_Section,m_Name_Tachometer_Max,write_str,IniFileName);

 write_str.Format(_T("%d"),(int)m_optPressureMax);
 WritePrivateProfileString(m_Name_Fixtures_Section,m_Name_Pressure_Max,write_str,IniFileName);

 write_str.Format(_T("%d"),(int)m_optPulsesPer1Km);
 WritePrivateProfileString(m_Name_Fixtures_Section,m_Name_PulsesPer1Km,write_str,IniFileName);

 write_str = (m_optSpeedUnit == SU_KMH) ? _T("kmh") : _T("mph");
 WritePrivateProfileString(m_Name_Fixtures_Section,m_Name_SpeedUnit,write_str,IniFileName);

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
 m_optAETPSMapWnd_X = i_wndSettings.m_AETPSMapWnd_X;
 m_optAETPSMapWnd_Y = i_wndSettings.m_AETPSMapWnd_Y; 
 m_optAERPMMapWnd_X = i_wndSettings.m_AERPMMapWnd_X;
 m_optAERPMMapWnd_Y = i_wndSettings.m_AERPMMapWnd_Y; 
 m_optAftstrMapWnd_X = i_wndSettings.m_AftstrMapWnd_X;
 m_optAftstrMapWnd_Y = i_wndSettings.m_AftstrMapWnd_Y; 
 m_optATSCurvMapWnd_X = i_wndSettings.m_ATSCurvMapWnd_X;
 m_optATSCurvMapWnd_Y = i_wndSettings.m_ATSCurvMapWnd_Y;
 m_optATSCorrMapWnd_X = i_wndSettings.m_ATSCorrMapWnd_X;
 m_optATSCorrMapWnd_Y = i_wndSettings.m_ATSCorrMapWnd_Y; 
 m_optGasdoseMapWnd_X = i_wndSettings.m_GasdoseMapWnd_X;
 m_optGasdoseMapWnd_Y = i_wndSettings.m_GasdoseMapWnd_Y; 
 m_optITMapWnd_X = i_wndSettings.m_ITMapWnd_X;
 m_optITMapWnd_Y = i_wndSettings.m_ITMapWnd_Y;
 m_optITRPMMapWnd_X = i_wndSettings.m_ITRPMMapWnd_X;
 m_optITRPMMapWnd_Y = i_wndSettings.m_ITRPMMapWnd_Y;
 m_optRigidMapWnd_X = i_wndSettings.m_RigidMapWnd_X;
 m_optRigidMapWnd_Y = i_wndSettings.m_RigidMapWnd_Y;
 m_optEGOCrvMapWnd_X = i_wndSettings.m_EGOCrvMapWnd_X;
 m_optEGOCrvMapWnd_Y = i_wndSettings.m_EGOCrvMapWnd_Y;
 m_optIACCMapWnd_X = i_wndSettings.m_IACCMapWnd_X;
 m_optIACCMapWnd_Y = i_wndSettings.m_IACCMapWnd_Y;
 m_optIACCWMapWnd_X = i_wndSettings.m_IACCWMapWnd_X;
 m_optIACCWMapWnd_Y = i_wndSettings.m_IACCWMapWnd_Y;
 m_optIATCLTMapWnd_X = i_wndSettings.m_IATCLTMapWnd_X;
 m_optIATCLTMapWnd_Y = i_wndSettings.m_IATCLTMapWnd_Y;
 m_optBarocorrMapWnd_X = i_wndSettings.m_BarocorrMapWnd_X;
 m_optBarocorrMapWnd_Y = i_wndSettings.m_BarocorrMapWnd_Y; 
 m_optManIgntimMapWnd_X = i_wndSettings.m_ManIgntimMapWnd_X;
 m_optManIgntimMapWnd_Y = i_wndSettings.m_ManIgntimMapWnd_Y; 
 m_optCESettingsWnd_X = i_wndSettings.m_CESettingsWnd_X;
 m_optCESettingsWnd_Y = i_wndSettings.m_CESettingsWnd_Y; 
 m_optTpsswtMapWnd_X = i_wndSettings.m_TpsswtMapWnd_X;
 m_optTpsswtMapWnd_Y = i_wndSettings.m_TpsswtMapWnd_Y;
 m_optTmp2CurveMapWnd_X = i_wndSettings.m_Tmp2CurveMapWnd_X;
 m_optTmp2CurveMapWnd_Y = i_wndSettings.m_Tmp2CurveMapWnd_Y; 
 m_optGtscMapWnd_X = i_wndSettings.m_GtscMapWnd_X;
 m_optGtscMapWnd_Y = i_wndSettings.m_GtscMapWnd_Y;
 m_optGpscMapWnd_X = i_wndSettings.m_GpscMapWnd_X;
 m_optGpscMapWnd_Y = i_wndSettings.m_GpscMapWnd_Y;
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
 o_wndSettings.m_AETPSMapWnd_X = m_optAETPSMapWnd_X;
 o_wndSettings.m_AETPSMapWnd_Y = m_optAETPSMapWnd_Y;
 o_wndSettings.m_AERPMMapWnd_X = m_optAERPMMapWnd_X;
 o_wndSettings.m_AERPMMapWnd_Y = m_optAERPMMapWnd_Y;
 o_wndSettings.m_AftstrMapWnd_X = m_optAftstrMapWnd_X;
 o_wndSettings.m_AftstrMapWnd_Y = m_optAftstrMapWnd_Y;
 o_wndSettings.m_ATSCurvMapWnd_X = m_optATSCurvMapWnd_X;
 o_wndSettings.m_ATSCurvMapWnd_Y = m_optATSCurvMapWnd_Y;
 o_wndSettings.m_ATSCorrMapWnd_X = m_optATSCorrMapWnd_X;
 o_wndSettings.m_ATSCorrMapWnd_Y = m_optATSCorrMapWnd_Y;
 o_wndSettings.m_GasdoseMapWnd_X = m_optGasdoseMapWnd_X;
 o_wndSettings.m_GasdoseMapWnd_Y = m_optGasdoseMapWnd_Y;
 o_wndSettings.m_ITMapWnd_X = m_optITMapWnd_X;
 o_wndSettings.m_ITMapWnd_Y = m_optITMapWnd_Y;
 o_wndSettings.m_ITRPMMapWnd_X = m_optITRPMMapWnd_X;
 o_wndSettings.m_ITRPMMapWnd_Y = m_optITRPMMapWnd_Y;
 o_wndSettings.m_RigidMapWnd_X = m_optRigidMapWnd_X;
 o_wndSettings.m_RigidMapWnd_Y = m_optRigidMapWnd_Y;
 o_wndSettings.m_EGOCrvMapWnd_X = m_optEGOCrvMapWnd_X;
 o_wndSettings.m_EGOCrvMapWnd_Y = m_optEGOCrvMapWnd_Y;
 o_wndSettings.m_IACCMapWnd_X = m_optIACCMapWnd_X;
 o_wndSettings.m_IACCMapWnd_Y = m_optIACCMapWnd_Y;
 o_wndSettings.m_IACCWMapWnd_X = m_optIACCWMapWnd_X;
 o_wndSettings.m_IACCWMapWnd_Y = m_optIACCWMapWnd_Y;
 o_wndSettings.m_IATCLTMapWnd_X = m_optIATCLTMapWnd_X;
 o_wndSettings.m_IATCLTMapWnd_Y = m_optIATCLTMapWnd_Y;
 o_wndSettings.m_BarocorrMapWnd_X = m_optBarocorrMapWnd_X;
 o_wndSettings.m_BarocorrMapWnd_Y = m_optBarocorrMapWnd_Y;
 o_wndSettings.m_ManIgntimMapWnd_X = m_optManIgntimMapWnd_X;
 o_wndSettings.m_ManIgntimMapWnd_Y = m_optManIgntimMapWnd_Y;
 o_wndSettings.m_CESettingsWnd_X = m_optCESettingsWnd_X;
 o_wndSettings.m_CESettingsWnd_Y = m_optCESettingsWnd_Y;
 o_wndSettings.m_TpsswtMapWnd_X = m_optTpsswtMapWnd_X;
 o_wndSettings.m_TpsswtMapWnd_Y = m_optTpsswtMapWnd_Y;
 o_wndSettings.m_Tmp2CurveMapWnd_X = m_optTmp2CurveMapWnd_X;
 o_wndSettings.m_Tmp2CurveMapWnd_Y = m_optTmp2CurveMapWnd_Y;
 o_wndSettings.m_GtscMapWnd_X = m_optGtscMapWnd_X;
 o_wndSettings.m_GtscMapWnd_Y = m_optGtscMapWnd_Y;
 o_wndSettings.m_GpscMapWnd_X = m_optGpscMapWnd_X;
 o_wndSettings.m_GpscMapWnd_Y = m_optGpscMapWnd_Y;
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

ESpeedUnit CAppSettingsModel::GetSpeedUnit(void) const
{
 return m_optSpeedUnit;
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

bool CAppSettingsModel::GetCOMPortBother(void) const
{
 return m_optCOMPortBother;
}

bool CAppSettingsModel::GetUseHotKeys(void) const
{
 return m_optUseHotKeys;
}

bool CAppSettingsModel::GetShowWelcome(void) const
{
 return m_optShowWelcome;
}

int CAppSettingsModel::GetRPMAverage(void) const
{
 return m_optRPMAverage;
}

int CAppSettingsModel::GetVoltAverage(void) const
{
 return m_optVoltAverage;
}

int CAppSettingsModel::GetMAPAverage(void) const
{
 return m_optMAPAverage;
}

int CAppSettingsModel::GetAI1Average(void) const
{
 return m_optAI1Average;
}

int CAppSettingsModel::GetTPSAverage(void) const
{
 return m_optTPSAverage;
}

bool CAppSettingsModel::GetAllowVisualTheme(void) const
{
 return m_optAllowVisualTheme;
}

bool CAppSettingsModel::GetAutoDiagEnter(void) const
{
 return m_optAutoDiagEnter;
}

bool CAppSettingsModel::GetSaveWarning(void) const
{
 return m_optSaveWarning;
}

bool CAppSettingsModel::GetAutoCERead(void) const
{
 return m_optAutoCERead;
}
