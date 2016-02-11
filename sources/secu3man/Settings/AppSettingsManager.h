/* SECU-3  - An open source, free engine control unit
   Copyright (C) 2007 Alexey A. Shabelnikov. Ukraine, Kiev

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

   contacts:
              http://secu-3.org
              email: shabelnikov@secu-3.org
*/

/** \file AppSettingsManager.h
 * \author Alexey A. Shabelnikov
 */

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

 //Read settings from disk
 bool ReadSettings(void) const;

 //Write settings to disk
 bool WriteSettings(void) const;

 //Show settings dialog
 int ShowDialog(void) const;

 //возвражает интерфейс к данным настроек программы для чтения/записи
 ISettingsData* GetSettings(void) const;

 //Возвращает указатель на модель с данными настроек программы
 CAppSettingsModel* GetModel(void) const;

private:
 CAppSettingsModel* m_pModel;
 CAppSettingsController* m_pController;
 CAppSettingsDlg* m_pDialog;
};
