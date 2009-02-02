
#pragma once

class CAppSettingsModel;
class CAppSettingsDlg;
class CAppSettingsController;


class CAppSettingsManager  
{
public:

    CAppSettingsModel* m_pModel;
	CAppSettingsController* m_pController;
	CAppSettingsDlg* m_pDialog;

	CAppSettingsManager();
	virtual ~CAppSettingsManager();

	//прочитать настройки с диска
    bool ReadSettings(void) const;

	//сохранить настройки на диск
	bool WriteSettings(void) const;

	//показать диалог параметров
	int ShowDialog(void) const;
};

