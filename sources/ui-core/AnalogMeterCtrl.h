
#pragma once

#include "AnalogMeter.h"

class AFX_EXT_CLASS CAnalogMeterCtrl : public CStatic, public CAnalogMeter
{
 public:
  CAnalogMeterCtrl();
  virtual ~CAnalogMeterCtrl();

 public:
  void UpdateNeedle(double dPos);

  void Redraw(void);

 protected:
  // Generated message map functions
  afx_msg void OnPaint();
  DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
