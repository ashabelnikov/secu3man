 /****************************************************************
 *       SECU-3  - An open source, free engine control unit
 *    Designed by Alexey A. Shabelnikov. Ukraine, Gorlovka 2007.
 *       Microprocessors systems - design & programming.
 *    contacts:
 *              http://secu-3.narod.ru
 *              ICQ: 405-791-931
 ****************************************************************/

#include "stdafx.h"
#include "secu3man.h"
#include "AppSettingsModel.h"

#include <algorithm>


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
, m_Name_MAPCurveSlope(_T("MAP_curve_slope"))
, m_Name_MAPCurveOffset(_T("MAP_curve_offset"))
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

  //заполняем базу данных допустимых COM-портов
  for(int i = 1; i < 256; i++)
  { 
    CString str;	  
    str.Format(_T("COM%d"),i);	
    m_AllowablePorts.push_back(std::wstring(str));	 
  }
}

CAppSettingsModel::~CAppSettingsModel()
{

}


//возвращает полное имя INI-файла
CString CAppSettingsModel::GetINIFileFullName(void)
{
  TCHAR current_directory[MAX_PATH+1];

  //определение тек. директории  
  if (!GetCurrentDirectory(MAX_PATH,current_directory))
     return _T("");
  
  CString directory(current_directory);

  CString last_char = directory.Right(1);
  if (last_char != _T("\\")) //если корневой каталог, то '\' уже есть
    directory+=_T("\\");

  return directory + AfxGetApp()->m_pszExeName + _T(".ini");
}

//проверяет указанное значение скорости на соответствие стандарту
bool CAppSettingsModel::CheckAllowableBaudRate(DWORD baud)
{
  std::vector<DWORD>::iterator it;
  it = std::find(m_AllowableBaudRates.begin(),m_AllowableBaudRates.end(),baud);
  if (it != m_AllowableBaudRates.end())
    return true;
  return false; //invalid baud rate
}

//чтение настроек из INI-файла
bool CAppSettingsModel::ReadSettings(void)
{
  CString IniFileName = GetINIFileFullName();
  bool status = true;
  TCHAR readed_str[1024];

  int i_val = 0, result;  float f_val = 0;

  //-----------------------------------------
  GetPrivateProfileString(m_Name_Options_Section,m_Name_PortName,_T("COM1"),readed_str,255,IniFileName);
  m_optPortName = readed_str;     
  
  //-----------------------------------------
  GetPrivateProfileString(m_Name_Options_Section,m_Name_BaudRateApplication,_T("9600"),readed_str,255,IniFileName);
  i_val = _wtoi(readed_str);
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
  i_val = _wtoi(readed_str);
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
  GetPrivateProfileString(m_Name_Options_Section,m_Name_MAPCurveSlope,_T("1.0"),readed_str,255,IniFileName);
  result = swscanf(readed_str,_T("%f"),&f_val);
  if (result==0)
  {
    status = false;
	m_optMAPCurveSlope = 1.0;
  }
  else
  {
    m_optMAPCurveSlope = f_val;     
  }

  //-----------------------------------------
  GetPrivateProfileString(m_Name_Options_Section,m_Name_MAPCurveOffset,_T("1.0"),readed_str,255,IniFileName);
  result = swscanf(readed_str,_T("%f"),&f_val);
  if (result==0)
  {
    status = false;
	m_optMAPCurveOffset = 1.0;
  }
  else
  {
    m_optMAPCurveOffset = f_val;     
  }

  return status;
}

//запись настроек в INI-файл
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
  write_str.Format(_T("%0.2f"),m_optMAPCurveSlope);
  WritePrivateProfileString(m_Name_Options_Section,m_Name_MAPCurveSlope,write_str,IniFileName);

  //-----------------------------------------
  write_str.Format(_T("%0.2f"),m_optMAPCurveOffset);
  WritePrivateProfileString(m_Name_Options_Section,m_Name_MAPCurveOffset,write_str,IniFileName);

  return status;
}
