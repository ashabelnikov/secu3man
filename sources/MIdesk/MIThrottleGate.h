
#pragma once

#include "MeasInstrument.h"
#include "ui-core\LEDIndicator.h"

class AFX_EXT_CLASS CMIThrottleGate : public CMeasInstrument  
{
public:
    void DDX_Controls(CDataExchange* pDX, int nIDC_led, int nIDC_caption);

	CMIThrottleGate();
	virtual ~CMIThrottleGate();
	virtual void Create(void);

	bool m_prev_enable;

	CLEDIndicator m_led;
	CStatic       m_caption;

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

