
#pragma once

class CAppSettingsController;
class CAppSettingsDlg;
class CAppSettingsModel;
class ISettingsData;

class CAppSettingsManager  
{
public:

 CAppSettingsManager();
 virtual ~CAppSettingsManager();

 //прочитать настройки с диска
 bool ReadSettings(void) const;

 //сохранить настройки на диск
 bool WriteSettings(void) const;

 //показать диалог параметров
 int ShowDialog(void) const;

 //возвражает интерфейс к данным настроек программы только для чтения
 ISettingsData* GetSettings(void) const;
   
 //Возвращает указатель на модель с данными настроек программы
 CAppSettingsModel* GetModel(void) const;

private:
 CAppSettingsModel* m_pModel;
 CAppSettingsController* m_pController;
 CAppSettingsDlg* m_pDialog;
};
