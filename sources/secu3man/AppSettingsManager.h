
#pragma once

#include "AppSettingsModel.h"
#include "AppSettingsController.h"
#include "AppSettingsDlg.h"


class CAppSettingsManager  
{
public:

    CAppSettingsModel* m_pModel;
	CAppSettingsController* m_pController;
	CAppSettingsDlg* m_pDialog;

	CAppSettingsManager();
	virtual ~CAppSettingsManager();

	//прочитать настройки с диска
    bool ReadSettings(void) 
	{
      ASSERT(m_pModel);
      return m_pModel->ReadSettings();
	}

	//сохранить настройки на диск
	bool WriteSettings(void) 
	{
      ASSERT(m_pModel);
      return m_pModel->WriteSettings();
	}

	//показать диалог параметров
	int ShowDialog(void) 
	{ //делегируем вызов контроллеру...
      ASSERT(m_pController);
	  return m_pController->ShowDialog(); 
	}
};

