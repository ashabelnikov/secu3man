
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

  CAppSettingsModel();
  virtual ~CAppSettingsModel();

  ////////////////////////////////////////////////////
  //¬озвращает полное им€ INI-файла. INI-файл находитс€ в каталоге из которого
  //запущена программа.
  CString GetINIFileFullName(void) const;

  //чтение настроек из INI-файла
  bool ReadSettings(void);
    
  //запись настроек в INI-файл
  bool WriteSettings(void);

  //им€ секции и названи€ полей
  const CString m_Name_Options_Section;
  const CString m_Name_PortName;
  const CString m_Name_BaudRateApplication;
  const CString m_Name_BaudRateBootloader;
  const CString m_Name_LogFilesFolder;
  const CString m_Name_UseAppFolder;
  const CString m_Name_CSVSepSymbol;
  const CString m_Name_MIDeskUpdatePeriod;

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

  //ISettingsData
  virtual const _TSTRING& GetPortName(void) const;   
  virtual DWORD GetBaudRateApplication(void) const;
  virtual DWORD GetBaudRateBootloader(void) const;
  virtual const CString& GetLogFilesFolder(void) const;
  virtual bool  GetUseAppFolder(void) const;
  virtual char  GetCSVSepSymbol(void) const;
  virtual int   GetMIDeskUpdatePeriod(void) const;
  //¬озвращает полный путь к каталогу из которого было запущено приложение
  //(каталог который был текущим на момент вызва конструктора этого класса)
  virtual CString GetAppDirectory(void) const;
  //позиции окон
  virtual void SetWndSettings(const WndSettings& i_wndSettings);
  virtual void GetWndSettings(WndSettings& o_wndSettings) const;
  ////////////////////////////////////////////////////

 private:
  //данные которые хран€тс€ в INI-файле
  _TSTRING m_optPortName;      
  DWORD m_optBaudRateApplication;
  DWORD m_optBaudRateBootloader;
  CString m_optLogFilesFolder;
  bool  m_optUseAppFolder;
  char  m_optCSVSepSymbol;
  int   m_optMIDeskUpdatePeriod;

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

  //провер€ет указанное значение скорости на соответствие стандарту
  bool CheckAllowableBaudRate(DWORD baud);
  bool CheckAllowableCSVSepSymbol(char i_symbol);

  //директори€ из которой было запущено приложение
  TCHAR m_current_directory[MAX_PATH+1]; 
};
