
#pragma once

#include <map>

class ITabController;

class TabControllersCommunicator
{
   TabControllersCommunicator();

 public:
  ~TabControllersCommunicator();

  static TabControllersCommunicator* GetInstance(void);

  void AddReference(ITabController* ip_controller, int i_key);
  
  ITabController* GetReference(int i_key);

 private:
  std::map<int, ITabController*> m_references;
};


//identifiers of tab controllers
#define TCC_FIRMWARE_TAB_CONTROLLER 1