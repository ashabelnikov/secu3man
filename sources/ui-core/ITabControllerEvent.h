
#pragma once

class AFX_EXT_CLASS ITabControllerEvent
{
 public:
  ITabControllerEvent();
 ~ITabControllerEvent();

  virtual void OnSelchangeTabctl(void) = 0;
  virtual void OnSelchangingTabctl(void) = 0;
};
