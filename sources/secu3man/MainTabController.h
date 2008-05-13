
#pragma once

#include <vector>
#include "ITabController.h"
#include "ui-core/ITabControllerEvent.h"
#include "ui-core/TabController.h"

class CMainTabController  : public ITabControllerEvent
{
  public:
	CMainTabController();
	virtual ~CMainTabController();

    void AddTabController(ITabController* i_pCntr);
    std::vector<ITabController*>& GetControllersList(void);
	void SetTabController(CTabController* i_pTabController) { m_pTabController = i_pTabController;}

    CTabController* m_pTabController;

    virtual void OnSelchangeTabctl(void); 
    virtual void OnSelchangingTabctl(void); 

    
  private:
	std::vector<ITabController*> m_controllers_list;

};

