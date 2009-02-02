
#pragma once

#include "IAppSettingsDlg.h"


class IAppSettingsDlg;
class CAppSettingsModel;

class CAppSettingsController  
{
IAppSettingsDlg* m_pView; 
CAppSettingsModel* m_pModel;

public:
	CAppSettingsController(IAppSettingsDlg* i_pView, CAppSettingsModel* i_pModel);
	virtual ~CAppSettingsController();

	void OnOk(void);
	void OnCancel(void);
	void OnActivate(void);

	int ShowDialog(void) 
	{
      ASSERT(m_pView);
	  return m_pView->ShowDialog(); 
	}
};

