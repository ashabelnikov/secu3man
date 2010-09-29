 /****************************************************************
 *       SECU-3  - An open source, free engine control unit
 *    Designed by Alexey A. Shabelnikov. Ukraine, Gorlovka 2007.
 *       Microprocessors systems - design & programming.
 *    contacts:
 *              http://secu-3.narod.ru
 *              ICQ: 405-791-931
 ****************************************************************/

//Holds windows's position data
struct WndSettings
{
 int m_StrtMapWnd_X;
 int m_StrtMapWnd_Y;
 int m_IdleMapWnd_X;
 int m_IdleMapWnd_Y;
 int m_WorkMapWnd_X;
 int m_WorkMapWnd_Y;
 int m_TempMapWnd_X;
 int m_TempMapWnd_Y;
 int m_AttenuatorMapWnd_X;
 int m_AttenuatorMapWnd_Y;
 int m_MainFrmWnd_X;
 int m_MainFrmWnd_Y;
};

typedef enum EInterLang
{
 IL_ENGLISH = 0,
 IL_RUSSIAN
};

typedef enum EECUPlatform
{
 EP_ATMEGA16 = 0,
 EP_ATMEGA32,
 EP_ATMEGA64
};

//интерфейс через который должен производитс€ доступ к настройкам программы
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
  //позиции окон
  virtual void SetWndSettings(const WndSettings& i_wndSettings) = 0;
  virtual void GetWndSettings(WndSettings& o_wndSettings) const = 0;
  //язык интерфейса и тип платформы SECU-3  
  virtual EInterLang GetInterfaceLanguage(void) const = 0;
  virtual EECUPlatform GetECUPlatformType(void) const = 0;
};
