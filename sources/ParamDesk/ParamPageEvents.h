
#pragma once

#include "common/FastDelegate.h"

class ParamPageEvents  
{
public:
    typedef fastdelegate::FastDelegate0<> EventHandler;

	ParamPageEvents();
	virtual ~ParamPageEvents();
  	
	void setFunctionOnChange(EventHandler OnChange) { m_OnChange = OnChange;};

    void OnChangeNotify(void) 
	{		
	  if (m_OnChange)  //call only if handler present! 
		m_OnChange();
	}

private:
    EventHandler m_OnChange;
};

