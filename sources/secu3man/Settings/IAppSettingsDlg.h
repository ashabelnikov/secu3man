/* SECU-3  - An open source, free engine control unit
   Copyright (C) 2007 Alexey A. Shabelnikov. Ukraine, Gorlovka

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

#pragma once

#include <vector>
#include "common/FastDelegate.h"
#include "common/unicodesupport.h"

class IAppSettingsDlg
{
 public:
  typedef fastdelegate::FastDelegate0<> EventHandler;

  virtual void FillCtrlsWithAllowableBaudRates(std::vector<DWORD> i_AllowableBaudRates) = 0;
  virtual void FillCtrlsWithAllowablePorts(std::vector<_TSTRING> i_AllowablePorts) = 0;
  virtual void FillCtrlsWithAllowableCSVSepSymbols(std::vector<std::pair<_TSTRING, char> >  i_AllowableCSVSepSymbols) = 0;
  virtual void FillCtrlsWithAllowableInterfaceLanguages(std::vector<std::pair<std::pair<_TSTRING, _TSTRING>, int> > i_AllowableInterfaceLanguages) = 0;
  virtual void FillCtrlsWithAllowableECUPlatformTypes(std::vector<std::pair<std::pair<_TSTRING, _TSTRING>, int> > i_AllowableECUPlatformTypes) = 0;

  //"Set" - установка данных представления
  virtual void SetPortName(_TSTRING i_PortName) = 0;
  virtual void SetBaudRateApplication(DWORD i_app_baud) = 0;
  virtual void SetBaudRateBootloader(DWORD i_bl_baud) = 0;
  virtual void SetLogFilesFolder(const CString& i_folder) = 0;
  virtual void SetUseAppFolder(bool i_use) = 0;
  virtual void SetCSVSepSymbol(size_t i_index) = 0;
  virtual void SetMIDeskUpdatePeriod(int i_period) = 0;
  virtual void SetInterfaceLanguage(int i_iface_lang) = 0;
  virtual void SetECUPlatformType(int i_platform_type) = 0;
  virtual void SetTachometerMax(int i_max) = 0;
  virtual void SetPressureMax(int i_max) = 0;
   
  //"Get" - получение данных представления
  virtual _TSTRING GetPortName(void) = 0;
  virtual DWORD GetBaudRateApplication(void) = 0;
  virtual DWORD GetBaudRateBootloader(void) = 0;
  virtual CString GetLogFilesFolder(void) const = 0;
  virtual bool GetUseAppFolder(void) const = 0;
  virtual size_t GetCSVSepSymbol(void) = 0;
  virtual int GetMIDeskUpdatePeriod(void) = 0;
  virtual int GetInterfaceLanguage(void) const = 0;
  virtual int GetECUPlatformType(void) const = 0;
  virtual int GetTachometerMax(void) const = 0;
  virtual int GetPressureMax(void) const = 0;

  //устанавливают обработчики событий
  virtual void setFunctionOnOk(EventHandler OnOk) = 0;
  virtual void setFunctionOnCancel(EventHandler OnCancel) = 0;
  virtual void setFunctionOnActivate(EventHandler OnOk) = 0;

  virtual int ShowDialog(void) = 0;
};
