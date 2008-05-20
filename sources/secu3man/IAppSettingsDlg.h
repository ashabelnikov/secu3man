
#pragma once

#include "common/FastDelegate.h"
#include "common/unicodesupport.h"

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
   virtual void SetMAPCurveSlope(float i_slope) = 0;
   virtual void SetMAPCurveOffset(float i_offset) = 0;
   
   //"Get" - получение данных представления
   virtual _TSTRING GetPortName(void) = 0;
   virtual DWORD GetBaudRateApplication(void) = 0;
   virtual DWORD GetBaudRateBootloader(void) = 0;
   virtual float GetMAPCurveSlope(void) = 0;
   virtual float GetMAPCurveOffset(void) = 0;  

   //устанавливают обработчики событий
   virtual void setFunctionOnOk(EventHandler OnOk) = 0;
   virtual void setFunctionOnCancel(EventHandler OnCancel) = 0;
   virtual void setFunctionOnActivate(EventHandler OnOk) = 0;

   virtual ShowDialog(void) = 0;
};





