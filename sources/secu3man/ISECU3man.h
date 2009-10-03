/****************************************************************
 *       SECU-3  - An open source, free engine control unit
 *    Designed by Alexey A. Shabelnikov. Ukraine, Gorlovka 2007.
 *       Microprocessors systems - design & programming.
 *    contacts:
 *              http://secu-3.narod.ru
 *              ICQ: 405-791-931
 ****************************************************************/
#pragma once

class CAppSettingsManager;
class CCommunicationManager;
class CMainFrameManager;
class LogWriter;

class ISECU3Man
{
 public:
  static ISECU3Man* GetSECU3Man()
  {
   ISECU3Man* p_application = (dynamic_cast<ISECU3Man*>(AfxGetApp()));
   _ASSERTE(p_application);
   return p_application; 
  };

  virtual CAppSettingsManager*    GetAppSettingsManager(void) const = 0;
  virtual CCommunicationManager*  GetCommunicationManager(void) const = 0;
  virtual CMainFrameManager*      GetMainFrameManager(void) const = 0; 
  virtual LogWriter*              GetLogWriter(void) const = 0;
};
