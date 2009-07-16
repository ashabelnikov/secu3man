 /****************************************************************
 *       SECU-3  - An open source, free engine control unit
 *    Designed by Alexey A. Shabelnikov. Ukraine, Gorlovka 2007.
 *       Microprocessors systems - design & programming.
 *    contacts:
 *              http://secu-3.narod.ru
 *              ICQ: 405-791-931
 ****************************************************************/

#include "stdafx.h"
#include "AppSettingsModel.h"

#include <algorithm>

#pragma warning( disable : 4800 ) //: forcing value to bool 'true' or 'false' (performance warning)


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
}

CAppSettingsModel::~CAppSettingsModel()
{

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


bool CAppSettingsModel::ReadSettings(void)
{
  CString IniFileName = GetINIFileFullName();
  bool status = true;
  TCHAR readed_str[1024];

  int i_val = 0;  float f_val = 0;

  //-----------------------------------------
  GetPrivateProfileString(m_Name_Options_Section,m_Name_PortName,_T("COM1"),readed_str,255,IniFileName);
  m_optPortName = readed_str;     
  
  //-----------------------------------------
  GetPrivateProfileString(m_Name_Options_Section,m_Name_BaudRateApplication,_T("9600"),readed_str,255,IniFileName);
  i_val = _ttoi(readed_str);

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
  GetPrivateProfileString(m_Name_Options_Section,m_Name_BaudRateBootloader,_T("9600"),readed_str,255,IniFileName);
  i_val = _ttoi(readed_str);

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
  GetPrivateProfileString(m_Name_Options_Section,m_Name_LogFilesFolder,def_value,readed_str,MAX_PATH,IniFileName);
  m_optLogFilesFolder = readed_str;  

  //-----------------------------------------
  GetPrivateProfileString(m_Name_Options_Section,m_Name_UseAppFolder,_T("1"),readed_str,255,IniFileName);
  i_val = _ttoi(readed_str);

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
  GetPrivateProfileString(m_Name_Options_Section,m_Name_CSVSepSymbol,_T("44"),readed_str,255,IniFileName);
  i_val = _ttoi(readed_str);

  if (!CheckAllowableCSVSepSymbol(i_val))
  {
    status = false;
	m_optCSVSepSymbol = 44; //comma by default
  }
  else
  {
    m_optCSVSepSymbol = i_val;     
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

  return status;
}
