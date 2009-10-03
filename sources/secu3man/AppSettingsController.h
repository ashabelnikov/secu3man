
#pragma once

#include "IAppSettingsDlg.h"

class IAppSettingsDlg;
class CAppSettingsModel;

class CAppSettingsController  
{
 public:
  CAppSettingsController(IAppSettingsDlg* i_pView, CAppSettingsModel* i_pModel);
  virtual ~CAppSettingsController();
	
  int ShowDialog(void); 

 private:
  void OnOk(void);
  void OnCancel(void);
  void OnActivate(void);

 private:
  IAppSettingsDlg* m_pView; 
  CAppSettingsModel* m_pModel;
};
