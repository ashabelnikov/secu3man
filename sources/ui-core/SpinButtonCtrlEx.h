
#pragma once

// Внимание! control должет быть со стилем UDS_AUTOBUDDY и без стиля UDS_SETBUDDYINT !

class AFX_EXT_CLASS CSpinButtonCtrlEx : public CSpinButtonCtrl
{
 public:
  CSpinButtonCtrlEx();
  virtual ~CSpinButtonCtrlEx();
	
  double GetPos();
  void   SetPos(double val);

  void SetRangeAndDelta(double lower, double upper, double delta);
  void GetRangeAndDelta(double &lower, double& upper, double& delta);
	
 protected:
  void InitSpinButtonCtrl();
  void SetIntegerPos(double pos);
  void SetBuddyValue(double value);

 protected:
  virtual void PreSubclassWindow();
  afx_msg BOOL OnDeltapos (NMHDR* pNMHDR, LRESULT* pResult);
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  DECLARE_MESSAGE_MAP()

 private:
  UINT   m_IntRange;
  double m_MinVal;
  double m_MaxVal;
  double m_Delta;
};
