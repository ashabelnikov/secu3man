
#pragma once

#include "common/FastDelegate.h"

class AFX_EXT_CLASS ParamPageEvents
{
  typedef fastdelegate::FastDelegate0<> EventHandler;

 public:
  ParamPageEvents();
  virtual ~ParamPageEvents();

  void setFunctionOnChange(EventHandler OnChange);

 protected:
  void OnChangeNotify(void);

 private:
  EventHandler m_OnChange;
};
