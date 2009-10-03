 /****************************************************************
 *       SECU-3  - An open source, free engine control unit
 *    Designed by Alexey A. Shabelnikov. Ukraine, Gorlovka 2007.
 *       Microprocessors systems - design & programming.
 *    contacts:
 *              http://secu-3.narod.ru
 *              ICQ: 405-791-931
 ****************************************************************/

//интерфейс через который должен производится доступ к настройкам программы
//из обычных мест в коде программы
class ISettingsData
{
 public:
  virtual const _TSTRING& GetPortName(void) const = 0;   
  virtual DWORD GetBaudRateApplication(void) const = 0;
  virtual DWORD GetBaudRateBootloader(void) const = 0;
  virtual const CString& GetLogFilesFolder(void) const = 0;
  virtual bool  GetUseAppFolder(void) const = 0;
  virtual char  GetCSVSepSymbol(void) const = 0;
  virtual int   GetMIDeskUpdatePeriod(void) const = 0;
  virtual CString GetAppDirectory(void) const = 0;
};
