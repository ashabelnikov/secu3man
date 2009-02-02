
#pragma once

#include "common/FastDelegate.h"
#include "common/unicodesupport.h"
#include <vector>

class IAppSettingsDlg
{
public:
   typedef fastdelegate::FastDelegate0<> EventHandler;

   virtual void FillCtrlsWithAllowableBaudRates(std::vector<DWORD> i_AllowableBaudRates) = 0;
   virtual void FillCtrlsWithAllowablePorts(std::vector<_TSTRING> i_AllowablePorts) = 0;

   //"Set" - установка данных представления
   virtual void SetPortName(_TSTRING i_PortName) = 0;
   virtual void SetBaudRateApplication(DWORD i_app_baud) = 0;
   virtual void SetBaudRateBootloader(DWORD i_bl_baud) = 0;
   virtual void SetLogFilesFolder(const CString& i_folder) = 0;
   virtual void SetUseAppFolder(bool i_use) = 0;
   
   //"Get" - получение данных представления
   virtual _TSTRING GetPortName(void) = 0;
   virtual DWORD GetBaudRateApplication(void) = 0;
   virtual DWORD GetBaudRateBootloader(void) = 0;
   virtual CString GetLogFilesFolder(void) const = 0;
   virtual bool GetUseAppFolder(void) const = 0;


   //устанавливают обработчики событий
   virtual void setFunctionOnOk(EventHandler OnOk) = 0;
   virtual void setFunctionOnCancel(EventHandler OnCancel) = 0;
   virtual void setFunctionOnActivate(EventHandler OnOk) = 0;

   virtual int ShowDialog(void) = 0;
};





