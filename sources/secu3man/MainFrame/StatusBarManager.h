
#pragma once

#include <memory>

class MPCStatusBar;

//Менеджер строки статуса
class CStatusBarManager
{
 public:
  CStatusBarManager(); 
  virtual ~CStatusBarManager();

  bool Create(CWnd* pParentWnd);
  void AddContent(void);

  void SetConnectionState(int i_State);
  void SetInformationText(const CString& i_text);
	
  void ShowProgressBar(bool show);
  void SetProgressRange(short nLower, short nUpper);
  void SetProgressPos(int nPos);
  void SetLoggerState(int i_state);

  enum { STATE_ONLINE = 1, STATE_OFFLINE = 2, STATE_BOOTLOADER = 3 };

  enum { LOG_STATE_WRITING, LOG_STATE_STOPPED};

 protected:
  CWnd* m_pParentWnd;
  std::auto_ptr<MPCStatusBar> mp_wndStatusBar;
  HICON   m_ConnIcons[3];
  HICON   m_LogWrIcon;
  CString m_ConnStrings[3];
  int m_CurrentConnectionState;
};
