
#pragma once

#include "io-core\ControlAppAdapter.h"

class CMainFrame;
class CCommunicationManager;
class CAppSettingsManager;
class CStatusBarManager;
class LogWriter;

class MainFrameController : public IAPPEventHandler
{
 public:
  MainFrameController(CCommunicationManager* i_pCommunicationManager, 
	                  CAppSettingsManager* i_pAppSettingsManager, 
					  CStatusBarManager* i_pStatusBarManager,
					  LogWriter* i_pLogWriter, CMainFrame* ip_view = NULL);
  ~MainFrameController();

  void SetView(CMainFrame* ip_view);

  //from IAPPEventHandler
  virtual void OnPacketReceived(const BYTE i_descriptor, SECU3IO::SECU3Packet* ip_packet);
  virtual void OnConnection(const bool i_online);

 private:
  //from view
  void OnAppAbout();
  void OnAppSettings();
  void OnAppBeginLog();
  void OnAppEndLog();
  void OnActivate(bool i_state);
  bool OnFullScreen();
  bool IsBeginLoggingAllowed(void);
  bool IsEndLoggingAllowed(void);
    
  CRect _GetScreenRect(void) const;

  //You have to call this function before using of this controller! 
  void _SetDelegates(void);

  CMainFrame*            mp_view;
  CCommunicationManager* m_pCommunicationManager;
  CAppSettingsManager*   m_pAppSettingsManager;
  CStatusBarManager*     m_pStatusBarManager;
  LogWriter*             m_pLogWriter;

   //true, если главное окно активно (запоминает последнее состояние)
  bool m_active;
  //true, если установлен полноэкранный режим
  bool m_full_screen_mode;
  //запоминает размеры и позицию окна перед переходом в полноэкранный режим
  CRect m_last_wnd_rect;
};
