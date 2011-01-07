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
              http://secu-3.narod.ru
              email: secu-3@yandex.ru
*/

#include "stdafx.h"
#include "AppSettingsModel.h"

#include <limits>
#include <algorithm>

#pragma warning( disable : 4800 ) //: forcing value to bool 'true' or 'false' (performance warning)

#undef max //avoid conflicts with C++

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

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
, m_Name_CSVSepSymbol(_T("CSVSeparatingSymbol"))
, m_Name_MIDeskUpdatePeriod(_T("MI_Desk_UpdatePeriod"))
, m_Name_InterfaceLang(_T("InterfaceLanguage"))
, m_Name_ECUPlatformType(_T("ECUPlatformType"))
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
, m_Name_CoilRegulMapWnd_X(_T("CoilRegulMapWnd_X"))
, m_Name_CoilRegulMapWnd_Y(_T("CoilRegulMapWnd_Y"))
//fixtures
, m_Name_Fixtures_Section("Fixtures")
, m_Name_Tachometer_Max(_T("Tachometer_Max"))
, m_Name_Pressure_Max(_T("Pressure_Max"))
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
 GetCurrentDirectory(MAX_PATH,m_current_directory);

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
 m_AllowablePlatforms.push_back(std::make_pair(std::make_pair(_TSTRING(_T("ATMega128")), _TSTRING(_T("atmega128"))), EP_ATMEGA128) );
}

CAppSettingsModel::~CAppSettingsModel()
{
 //na
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
 GetPrivateProfileString(m_Name_Options_Section,m_Name_BaudRateApplication,_T("9600"),read_str,255,IniFileName);
 i_val = _ttoi(read_str);

 if (!CheckAllowableBaudRate(i_val))
 {
  status = false;
  m_optBaudRateApplication = 9600;
 }
 else
 {
  m_optBaudRateApplication = i_val;     
 }

 //-----------------------------------------
 GetPrivateProfileString(m_Name_Options_Section,m_Name_BaudRateBootloader,_T("9600"),read_str,255,IniFileName);
 i_val = _ttoi(read_str);

 if (!CheckAllowableBaudRate(i_val))
 {
  status = false;
  m_optBaudRateBootloader = 9600;
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
 GetPrivateProfileString(m_Name_Options_Section,m_Name_ECUPlatformType,m_AllowablePlatforms[0].first.second.c_str(),read_str,255,IniFileName);

 if (!CheckAllowablePlatform(read_str, m_optECUPlatformType))
 {
  status = false;
  m_optECUPlatformType = (EECUPlatform)m_AllowablePlatforms[0].second; //atmega16 by default
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

 _GETWNDPOSITION(m_Name_WndSettings_Section, CoilRegulMapWnd_X, std::numeric_limits<int>::max());
 _GETWNDPOSITION(m_Name_WndSettings_Section, CoilRegulMapWnd_Y, std::numeric_limits<int>::max());

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

 write_str.Format(_T("%d"),m_optCoilRegulMapWnd_X);
 WritePrivateProfileString(m_Name_WndSettings_Section,m_Name_CoilRegulMapWnd_X,write_str,IniFileName);

 write_str.Format(_T("%d"),m_optCoilRegulMapWnd_Y);
 WritePrivateProfileString(m_Name_WndSettings_Section,m_Name_CoilRegulMapWnd_Y,write_str,IniFileName);

 //-----------------------------------------
 write_str.Format(_T("%d"),(int)m_optTachometerMax);
 WritePrivateProfileString(m_Name_Fixtures_Section,m_Name_Tachometer_Max,write_str,IniFileName);

 write_str.Format(_T("%d"),(int)m_optPressureMax);
 WritePrivateProfileString(m_Name_Fixtures_Section,m_Name_Pressure_Max,write_str,IniFileName);

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
 m_optCoilRegulMapWnd_X = i_wndSettings.m_CoilRegulMapWnd_X;
 m_optCoilRegulMapWnd_Y = i_wndSettings.m_CoilRegulMapWnd_Y;
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
 o_wndSettings.m_CoilRegulMapWnd_X = m_optCoilRegulMapWnd_X;
 o_wndSettings.m_CoilRegulMapWnd_Y = m_optCoilRegulMapWnd_Y;
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
