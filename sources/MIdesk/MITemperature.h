
#pragma once

#include "IMeasInstrument.h"
#include "ui-core\AnalogMeterCtrl.h"

class AFX_EXT_CLASS CMITemperature : public IMeasInstrument  
{
 public:
  CMITemperature();
  virtual ~CMITemperature();

  void DDX_Controls(CDataExchange* pDX, int nIDC_meter);

  //-------interface-----------------------
  virtual void Create(void); 
  virtual void Scale(float i_x_factor, float i_y_factor);
  virtual void SetValue(float value);
  virtual float GetValue(void);
  virtual void Show(bool show);
  virtual void Enable(bool enable);
  virtual bool IsVisible(void);
  virtual bool IsEnabled(void);	
  virtual void SetLimits(float loLimit, float upLimit);
  virtual void SetTicks(int number);
  //---------------------------------------

 private:
  CAnalogMeterCtrl m_meter;
};
