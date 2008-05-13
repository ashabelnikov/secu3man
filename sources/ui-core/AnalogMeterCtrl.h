
#pragma once

#include "AnalogMeter.h"


class AFX_EXT_CLASS CAnalogMeterCtrl : public CStatic, public CAnalogMeter
{
// Construction
public:
	CAnalogMeterCtrl();

public:
   void UpdateNeedle(double dPos);

   void Redraw(void)
   {
    m_boolForceRedraw = true;   
    Invalidate();
   }


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAnalogMeterCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CAnalogMeterCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CAnalogMeterCtrl)
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
