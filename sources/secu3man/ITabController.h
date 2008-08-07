
#pragma once


//через этот интерфейс главный контроллер управляет контроллерами вкладок (Tab control) 
class ITabController  
{
  public:
    virtual void OnActivate(void) = 0;
	virtual void OnDeactivate(void) = 0;
	virtual bool OnClose(void) = 0;
};

