
#pragma once

#include <vector>
#include "ui-core/ITabControllerEvent.h"

class CTabController;
class ITabController;

class CMainTabController  : public ITabControllerEvent
{
 public:
  CMainTabController();
  virtual ~CMainTabController();

  //добавляет контроллер
  void AddTabController(ITabController* i_pCntr);
   
  //возвращает список контроллеров
  std::vector<ITabController*>& GetControllersList(void);

  //Привязка к Tab-контролу
  void SetTabController(CTabController* i_pTabController);
  
  //возвращает контроллер активной вкладки
  virtual ITabController* GetActiveController() const;
   
 private:
  //ITabControllerEvent imp.
  virtual void OnSelchangeTabctl(void); 
  virtual void OnSelchangingTabctl(void); 
    
 private:
  std::vector<ITabController*> m_controllers_list;
  CTabController* m_pTabController;
};
