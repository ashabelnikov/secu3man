
#pragma once

class AFX_EXT_CLASS ITabControllerEvent
{
 public:
  virtual void OnSelchangeTabctl(void) = 0;
  virtual void OnSelchangingTabctl(void) = 0;
};
