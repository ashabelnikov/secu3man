
#pragma once

class CMainFrame;
class CCommunicationManager;
class CAppSettingsManager;
class CStatusBarManager;

class MainFrameController
{
 public:
  MainFrameController(CCommunicationManager* i_pCommunicationManager, 
	                  CAppSettingsManager* i_pAppSettingsManager, 
					  CStatusBarManager* i_pStatusBarManager, 
					  CMainFrame* ip_view = NULL);
  ~MainFrameController();

  void SetView(CMainFrame* ip_view);

 private:
  void OnAppAbout();
  void OnAppSettings();
  void OnAppBeginLog();
  void OnAppEndLog();
  bool IsBeginLoggingAllowed(void);
  bool IsEndLoggingAllowed(void);

  void _SetDelegates(void);

  CMainFrame* mp_view;
  CCommunicationManager* m_pCommunicationManager;
  CAppSettingsManager* m_pAppSettingsManager;
  CStatusBarManager* m_pStatusBarManager;
};