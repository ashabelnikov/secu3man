
#pragma once

#include "common/FastDelegate.h"
#include "common/unicodesupport.h"

//через этот интерфейс нужно общатся с панелью параметров (представлением)
//Особенность: Любой из следующих методов интерфейса может быть вызван независимо 
//от того - видна вкладка или нет (кроме SetCurSel()). Тип данных и соответственно тип вкладки определяется дескриптором

class IParamDeskView  
{
  typedef fastdelegate::FastDelegate0<> EventHandler;

  virtual void Enable(bool enable) = 0;                                 //разрешить/запретить представление 
  virtual void Show(bool show) = 0;                                     //показать/спрятать контент представления
  virtual bool SetValues(BYTE i_descriptor, const void* i_values) = 0;  //загнать данные в представление
  virtual bool GetValues(BYTE i_descriptor, void* o_values) = 0;        //извлечь данные из представления 

  //установка/получение имен семейств характеристик (для вкладки FunSet...)
  //вектор содержит набор имен семейств характеристик хранимых в прошивке SECU-3
  virtual void SetFunctionsNames(const std::vector<_TSTRING>& i_names) = 0;
  virtual const std::vector<_TSTRING>& GetFunctionsNames(void) = 0; 

  virtual BYTE GetCurrentDescriptor(void) = 0; //получает дескриптор соответствующий текущей (отображаемой) вкладке

  virtual void SetOnTabActivate(EventHandler OnTabActivate) = 0; //обработцик будет вызываться при появлении вкладки
  virtual void SetOnChangeInTab(EventHandler OnChangeInTab) = 0; //обработчик будет вызываться при изменении пользователем данных вкладки

  //вызывать только для активной панели!!!
  virtual bool SetCurSel(int sel) = 0;
  virtual int GetCurSel(void) = 0;
};

