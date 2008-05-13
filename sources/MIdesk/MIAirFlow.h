
#pragma once

#include "MeasInstrument.h"
#include "ui-core\ProgressCtrlEx.h"

class AFX_EXT_CLASS CMIAirFlow : public CMeasInstrument  
{
    float m_loLimit;
	float m_upLimit; 

public:
    void DDX_Controls(CDataExchange* pDX, int nIDC_progress, int nIDC_digits, int nIDC_caption);
	CMIAirFlow();
	virtual ~CMIAirFlow();

    CProgressCtrlEx m_progress;
	CStatic       m_digits;
	CStatic       m_caption; 


	bool m_prev_enable;

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


