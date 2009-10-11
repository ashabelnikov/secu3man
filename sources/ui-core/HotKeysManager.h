
#pragma once

#include <vector>

class CHotKeysToCmdRouter;

class AFX_EXT_CLASS HotKeysManager
{
  HotKeysManager(); //закрытый конструктор!

 public:
  ~HotKeysManager();

 static HotKeysManager* GetInstance(void);

 void DeactivateAllHotKeys(void);
 void ActivateAllHotKeys(void);

private:
 friend CHotKeysToCmdRouter;
 void _AddRouter(CHotKeysToCmdRouter* ip_router);
 void _RemoveRouter(CHotKeysToCmdRouter* ip_router);

 bool RegisterOneRouter(CHotKeysToCmdRouter* ip_router, bool i_register);

 std::vector<CHotKeysToCmdRouter*> m_hk_routers;
};
