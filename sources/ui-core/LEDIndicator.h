
#pragma once

/////////////////////////////////////////////////////////////////////////////
// Enum Types
enum LEDMemberEnum
{
 led_rect,  //рамка
 led_on,    //включенное состояние
 led_off    //выключенное сщстояние
};

/////////////////////////////////////////////////////////////////////////////
// CLEDIndicator window

class AFX_EXT_CLASS CLEDIndicator : public CStatic
{
 public:
  CLEDIndicator();
  virtual ~CLEDIndicator();

  void SetColor(enum LEDMemberEnum led_member, COLORREF Color);
  void GetColor(enum LEDMemberEnum led_member, COLORREF* pColor);

  void SetState(bool state);
  bool GetState(void);

 protected:
  void ActuateColors();

  // Generated message map functions
  afx_msg void OnPaint();
  DECLARE_MESSAGE_MAP()

 private:
  CBrush m_onBrush;
  CBrush m_offBrush;
  CPen   m_rectPen;

  COLORREF m_onColor;
  COLORREF m_offColor;
  COLORREF m_rectColor;

  int m_rectWidth;
  bool m_state;
};

/////////////////////////////////////////////////////////////////////////////
