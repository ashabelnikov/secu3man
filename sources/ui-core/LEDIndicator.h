
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
// Construction
public:
	CLEDIndicator();
	virtual ~CLEDIndicator();

    void SetColor(enum LEDMemberEnum led_member, COLORREF Color);
    void GetColor(enum LEDMemberEnum led_member, COLORREF* pColor);

	void SetState(bool state);
	bool GetState(void);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLEDIndicator)
	//}}AFX_VIRTUAL

// Implementation
public:

	// Generated message map functions
protected:
    CBrush m_onBrush;
	CBrush m_offBrush;
	CPen   m_rectPen;

	COLORREF m_onColor;
	COLORREF m_offColor;
	COLORREF m_rectColor;
	int m_rectWidth;

    bool m_state;

	void ActuateColors();
	//{{AFX_MSG(CLEDIndicator)
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
