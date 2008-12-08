#pragma once
#include "..\CustomItem.h"

#include <string>

class AFX_EXT_CLASS CBtnCheckBox : public ICustomItem
{
public:
  CBtnCheckBox(void);

public:
  virtual CPropertyGrid::EEditMode GetEditMode();
  virtual void DrawItem(CDC& dc, CRect rc, bool focused);
  virtual bool OnLButtonDown(CRect rc, CPoint pt);
  virtual void OnMouseMove(CRect rc, CPoint pt);
  virtual void OnLButtonUp(CRect rc, CPoint pt);

  bool IsButtonPressed(void) const;
  void SetButtonState(bool i_state);
  void SetCheckedText(const CString &i_text);
  void SetUncheckedText(const CString &i_text);

protected:
  COLORREF m_clrLeft;
  COLORREF m_clrRight;
  bool m_nButtonPushed; //false: depressed, true: pressed
  int m_state; //for internal use
  CString m_unchecked_text;
  CString m_checked_text;  
};
