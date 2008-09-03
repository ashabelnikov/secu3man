
#pragma once

#include <string>
#include <vector>
#include "common/unicodesupport.h"

//хранит и сохраняет/загружает данные 
class CAppSettingsModel  
{
public:
	std::vector<DWORD> m_AllowableBaudRates;
	std::vector<_TSTRING> m_AllowablePorts;

    ////////////////////////////////////////////////////
    CString GetINIFileFullName(void);

    bool ReadSettings(void);
	bool WriteSettings(void);

	//имя секции и названия полей
	const CString m_Name_Options_Section;
	const CString m_Name_PortName;
	const CString m_Name_BaudRateApplication;
	const CString m_Name_BaudRateBootloader;

    //данные которые хранятся в INI-файле
	_TSTRING m_optPortName;      
	DWORD m_optBaudRateApplication;
	DWORD m_optBaudRateBootloader;
    ////////////////////////////////////////////////////

	CAppSettingsModel();
	virtual ~CAppSettingsModel();

  private:
    bool CheckAllowableBaudRate(DWORD baud); 
    TCHAR m_current_directory[MAX_PATH+1]; //директория из которой было запущено приложение
};

