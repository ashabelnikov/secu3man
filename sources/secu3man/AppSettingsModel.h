
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

	//<UIstring, SepSymbol>
	std::vector<std::pair<_TSTRING, char> > m_AllowaleCSVSepSymbols;

	CAppSettingsModel();
	virtual ~CAppSettingsModel();

    ////////////////////////////////////////////////////
	//Возвращает полное имя INI-файла. INI-файл находится в каталоге из которого
	//запущена программа.
    CString GetINIFileFullName(void) const;

	//Возвращает полный путь к каталогу из которого было запущено приложение
	//(каталог который был текущим на момент вызва конструктора этого класса)
	CString GetAppDirectory(void) const;

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
	const CString m_Name_CSVSepSymbol;

    //данные которые хранятся в INI-файле
	_TSTRING m_optPortName;      
	DWORD m_optBaudRateApplication;
	DWORD m_optBaudRateBootloader;
	CString m_optLogFilesFolder;
	bool  m_optUseAppFolder;
	char  m_optCSVSepSymbol;
    ////////////////////////////////////////////////////

  private:
	//проверяет указанное значение скорости на соответствие стандарту
    bool CheckAllowableBaudRate(DWORD baud);

	bool CheckAllowableCSVSepSymbol(char i_symbol);

	//директория из которой было запущено приложение
    TCHAR m_current_directory[MAX_PATH+1]; 
};

