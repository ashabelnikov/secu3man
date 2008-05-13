
#pragma once

#include <string>
#include <vector>

/*
//данные которые представляются диалогом параметров
struct Options
{
  std::wstring  optPortName;      
  DWORD         optBaudRateApplication;
  DWORD         optBaudRateBootloader;
  float         optMAPCurveSlope;  //kPa/V
  float         optMAPCurveOffset; //Volts
};
*/


class CAppSettingsModel  
{
public:
	std::vector<DWORD> m_AllowableBaudRates;
	std::vector<std::wstring> m_AllowablePorts;

    ////////////////////////////////////////////////////
    CString GetINIFileFullName(void);

    bool ReadSettings(void);
	bool WriteSettings(void);

	//имя секции и названия полей
	const CString m_Name_Options_Section;
	const CString m_Name_PortName;
	const CString m_Name_BaudRateApplication;
	const CString m_Name_BaudRateBootloader;
	const CString m_Name_MAPCurveSlope;
	const CString m_Name_MAPCurveOffset;

    //данные которые хранятся в INI-файле
	std::wstring m_optPortName;      
	DWORD m_optBaudRateApplication;
	DWORD m_optBaudRateBootloader;
	float m_optMAPCurveSlope;  //kPa/V
	float m_optMAPCurveOffset; //Volts
    ////////////////////////////////////////////////////


	CAppSettingsModel();
	virtual ~CAppSettingsModel();

  private:
    bool CheckAllowableBaudRate(DWORD baud); 

};

