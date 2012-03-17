//Original code written by Mark C. Malburg
//Midified by Alexey A. Shabelnikov

// OScopeCtrl.h : header file
//

#ifndef __OScopeCtrl_H__
#define __OScopeCtrl_H__

#include <deque>

/////////////////////////////////////////////////////////////////////////////
// COScopeCtrl window

class AFX_EXT_CLASS COScopeCtrl : public CWnd
{
 public:
  COScopeCtrl();
  virtual ~COScopeCtrl();

  virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID = 0);

  //Use for add new point
  //dNewPoint - value of new point
  //i_left - if true, then point will be added from left side (reverse)
  void AppendPoint(double dNewPoint, bool i_reverse = false);

  void SetRange(double dLower, double dUpper, int nDecimalPlaces=1);
  void SetXUnits(CString string);
  void SetYUnits(CString string);
  void SetGridColor(COLORREF color);
  void SetPlotColor(COLORREF color);
  void SetBackgroundColor(COLORREF color);
  void InvalidateCtrl();
  void DrawPoint(bool i_reverse);
  void Reset();

 //Implementation
 protected:
  size_t _GetPtCount(void);
  void _SetStateColors(bool state);
  // Generated message map functions
  afx_msg void OnPaint();
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnEnable(BOOL bEnable);
  DECLARE_MESSAGE_MAP()

 private:
  const int m_nShiftPixels;    // amount to shift with each new point
  int m_nYDecimals;

  CString m_strXUnitsString;
  CString m_strYUnitsString;

  COLORREF m_crBackColor;      // background color
  COLORREF m_crGridColor;      // grid color
  COLORREF m_crPlotColor;      // data color

  COLORREF m_normalBackColor;
  COLORREF m_normalGridColor;
  COLORREF m_normalPlotColor;

  double m_dCurrentPosition;   // current position
  double m_dPreviousPosition;  // previous position

  double m_dLowerLimit;        // lower bounds
  double m_dUpperLimit;        // upper bounds
  double m_dRange;
  double m_dVerticalFactor;

  CRect  m_rectClient;
  CRect  m_rectPlot;
  CPen   m_penPlot;
  CBrush m_brushBack;         //background brush
  CBrush m_BlackBrush;        //black brush

  CDC     m_dcGrid;
  CDC     m_dcPlot;
  CBitmap *m_pbitmapOldGrid;
  CBitmap *m_pbitmapOldPlot;
  CBitmap m_bitmapGrid;
  CBitmap m_bitmapPlot;

  //stores values of all points
  std::deque<double> m_points;
  //stores point position, used for correct scrolling
  size_t m_point_position;
};

/////////////////////////////////////////////////////////////////////////////
#endif
