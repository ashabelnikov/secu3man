
#pragma once

#include "ui-core/MPCStatusBar.h"

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

	enum { STATE_ONLINE = 1, STATE_OFFLINE = 2, STATE_BOOTLOADER = 3 };

  protected:
	CWnd* m_pParentWnd;
	MPCStatusBar  m_wndStatusBar;
	HICON   m_ConnIcons[3];
	CString m_ConnStrings[3];
	int m_CurrentConnectionState;
};