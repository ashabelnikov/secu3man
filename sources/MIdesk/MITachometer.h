
#pragma once

#include "MeasInstrument.h"
#include "ui-core\AnalogMeterCtrl.h"

class AFX_EXT_CLASS CMITachometer : public CMeasInstrument  
{
public:
	CAnalogMeterCtrl m_meter;
	CMITachometer();
	virtual ~CMITachometer();

    virtual void Create(void); 

	//-------interface-----------------------
    virtual void SetValue(float value);
    virtual float GetValue(void);
	virtual void Show(bool show);
	virtual void Enable(bool enable);
	virtual bool IsVisible(void);
	virtual bool IsEnabled(void);	
	virtual void SetLimits(float loLimit, float upLimit);
	virtual void SetTicks(int number);
	//---------------------------------------
};

